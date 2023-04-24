#pragma once

#include <any>
#include <type_traits>

#include "engine/core/core.h"

namespace light::meta
{
	// size in pointers of std::function and std::any (roughly 3 pointers larger than std::string when building debug)
	inline constexpr int kSmallObjectNumPtrs = 6 + 16 / sizeof(void*);

	inline constexpr size_t kAnyTrivialSpaceSize = (kSmallObjectNumPtrs - 1) * sizeof(void*);

	template <class Ty>
	inline constexpr bool kIsTrivial =
		alignof(Ty) <= alignof(max_align_t) && std::is_trivially_copyable_v<Ty> && sizeof(Ty) <= kAnyTrivialSpaceSize;

	// 多个一个指针
	inline constexpr size_t kAnySmallSpaceSize = (kSmallObjectNumPtrs - 2) * sizeof(void*);

	template <class Ty>
	inline constexpr bool kIsSmall = alignof(Ty) <= alignof(max_align_t) && std::is_nothrow_move_constructible_v<Ty> && sizeof(Ty) <= kAnySmallSpaceSize;

	struct SmallObjectFuncCollectionBase
	{
		using CopyFunc = void(*)(const void*, void*);
		using MoveFunc = void(*)(void*, void*);
		using DestroyFunc = void(*)(void*);


		CopyFunc copy_func;
		MoveFunc move_func;
		DestroyFunc destroy_func;
	};

	template<typename T>
	struct SmallObjectFuncCollection : SmallObjectFuncCollectionBase
	{
		static void Copy(const void* src,void* dest)
		{
			new (dest) T(static_cast<T*>(src));
		}

		static void Move(void* src,void* dest)
		{
			new (dest) T(std::move(static_cast<T*>(src)));
		}

		static void Destroy(void* ptr)
		{
			static_cast<T*>(ptr)->~T();
		}

		SmallObjectFuncCollection()
		{
			copy_func = &Copy;
			move_func = &Move;
			destroy_func = &Destroy;
		}
	};

	struct BigObjectFuncCollectionBase
	{
		using CopyFunc = void*(*)(const void*);
		using MoveFunc = void(*)(void*, void*);
		using DestroyFunc = void(*)(void*);


		CopyFunc copy_func;
		MoveFunc move_func;
		DestroyFunc destroy_func;
	};

	template<typename T>
	struct BigObjectFuncCollection : BigObjectFuncCollectionBase
	{
		static void* Copy(const void* src)
		{
			return new T(*static_cast<T*>(src));
		}

		static void Move(void* src, void* dest)
		{
			new (dest) T(std::move(static_cast<T*>(src)));
		}

		static void Destroy(void* ptr)
		{
			T* v = static_cast<T*>(ptr);
			delete v;
		}

		BigObjectFuncCollection()
		{
			copy_func = &Copy;
			move_func = &Move;
			destroy_func = &Destroy;
		}
	};

	template<typename T>
	const SmallObjectFuncCollection<T> kSmallObjectFuncCollection;

	template<typename T>
	const BigObjectFuncCollection<T> kBigObjectFuncCollection;

	class Any
	{
	public:
		enum class AnyValueObjectType : uint8_t { kInvalid, kTrivial, kBig, kSmall };

		Any()
		{
			data_.object_type = AnyValueObjectType::kInvalid;
		}

		template<typename Value>
		Any(Value&& value)
		{
			Ctor<std::decay_t<Value>>(std::forward<Value>(value));
		}

		Any(const Any& other)
		{
			if (!other.IsValid())
			{
				return;
			}

			data_.object_type = other.data_.object_type;
			if (data_.object_type == AnyValueObjectType::kTrivial)
			{
				memcpy(data_.trivial_object_data.data, other.data_.trivial_object_data.data, sizeof(data_.trivial_object_data.data));
			}
			else if (data_.object_type == AnyValueObjectType::kSmall)
			{
				data_.small_object_data.object_func_collection = other.data_.small_object_data.object_func_collection;
				data_.small_object_data.object_func_collection->copy_func(other.data_.small_object_data.data, data_.small_object_data.data);
			}
			else if (data_.object_type == AnyValueObjectType::kBig)
			{
				data_.big_object_data.object_func_collection = other.data_.big_object_data.object_func_collection;
				data_.big_object_data.ptr = data_.big_object_data.object_func_collection->copy_func(other.data_.big_object_data.ptr);
			}
		}

		Any(Any&& other) noexcept
		{
			Move(other);
		}

		Any& operator=(const Any& other)
		{
			if(&other == this)
			{
				return *this;
			}

			// Any& operator=(Any&& other) noexcept
			*this = Any{ other };

			return *this;
		}

		Any& operator=(Any&& other) noexcept
		{
			if(&other == this)
			{
				return *this;
			}

			Reset();
			Move(other);
			return *this;
		}

		template<typename Value>
		Value Cast()
		{
			LIGHT_ASSERT(IsValid(), "Invalid Any");

			if (data_.object_type == AnyValueObjectType::kTrivial)
			{
				return *reinterpret_cast<Value*>(data_.trivial_object_data.data);
			}
			else if (data_.object_type == AnyValueObjectType::kSmall)
			{
				return *reinterpret_cast<Value*>(data_.small_object_data.data);
			}
			else
			{
				return *static_cast<Value*>(data_.big_object_data.ptr);
			}
		}

		bool IsValid() const
		{
			return data_.object_type != AnyValueObjectType::kInvalid;
		}

		void Reset()
		{
			if (IsValid())
			{
				if (data_.object_type == AnyValueObjectType::kSmall)
				{
					data_.small_object_data.object_func_collection->destroy_func(data_.small_object_data.data);
				}
				else if (data_.object_type == AnyValueObjectType::kBig)
				{
					data_.big_object_data.object_func_collection->destroy_func(data_.big_object_data.ptr);
				}

				data_.object_type = AnyValueObjectType::kInvalid;
			}
		}
	private:
		template<typename Value, typename ... Args>
		void Ctor(Args&& ... args)
		{
			if constexpr (kIsTrivial<Value>)
			{
				data_.object_type = AnyValueObjectType::kTrivial;
				new(data_.trivial_object_data.data) Value(std::forward<Args>(args)...);
			}
			else if constexpr (kIsSmall<Value>)
			{
				data_.object_type = AnyValueObjectType::kSmall;
				new(data_.small_object_data.data) Value(std::forward<Args>(args)...);
				data_.small_object_data.object_func_collection = &kSmallObjectFuncCollection<Value>;
			}
			else
			{
				data_.object_type = AnyValueObjectType::kBig;
				data_.big_object_data.ptr = new Value(std::forward<Args>(args)...);
				data_.big_object_data.object_func_collection = &kSmallObjectFuncCollection<Value>;
			}
		}

		void Move(Any& other) noexcept
		{
			if (!other.IsValid())
			{
				return;
			}

			data_.object_type = other.data_.object_type;
			if (data_.object_type == AnyValueObjectType::kTrivial)
			{
				memcpy(data_.trivial_object_data.data, other.data_.trivial_object_data.data, sizeof(data_.trivial_object_data.data));
			}
			else if (data_.object_type == AnyValueObjectType::kSmall)
			{
				data_.small_object_data.object_func_collection = other.data_.small_object_data.object_func_collection;
				data_.small_object_data.object_func_collection->move_func(other.data_.small_object_data.data, data_.small_object_data.data);
			}
			else if (data_.object_type == AnyValueObjectType::kBig)
			{
				data_.big_object_data.object_func_collection = other.data_.big_object_data.object_func_collection;
				data_.big_object_data.object_func_collection->move_func(other.data_.big_object_data.ptr, data_.big_object_data.ptr);
			}
		}

		struct TrivialObjectData
		{
			char data[kAnyTrivialSpaceSize];
		};

		struct BigObjectData
		{
			void* ptr;
			const BigObjectFuncCollectionBase* object_func_collection;
		};

		struct SmallObjectData
		{
			char data[kAnySmallSpaceSize];
			const SmallObjectFuncCollectionBase* object_func_collection;
		};

		struct Data
		{
			union
			{
				TrivialObjectData trivial_object_data;
				BigObjectData big_object_data;
				SmallObjectData small_object_data;
			};

			AnyValueObjectType object_type;
		};

		Data data_;

	};
}