#pragma once

#include <any>
#include <type_traits>
#include <vector>
#include <memory>

#include "engine/core/core.h"
#include "engine/reflection/type.h"

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

	class Any;

	struct SmallObjectFuncCollectionBase
	{
		using CopyFunc = void(*)(const void*, void*);
		using MoveFunc = void(*)(void*, void*);
		using DestroyFunc = void(*)(void*);
		using VectorSizeFunc = size_t(*)(const void*);
		using GetElementFunc = Any(*)(void*, size_t);

		CopyFunc copy_func;
		MoveFunc move_func;
		DestroyFunc destroy_func;
		VectorSizeFunc vector_size_func = nullptr;
		GetElementFunc get_element_func = nullptr;
	};

	template<typename T>
	struct SmallObjectFuncCollection : SmallObjectFuncCollectionBase
	{
		static void Copy(const void* src,void* dest)
		{
			new (dest) T(*static_cast<const T*>(src));
		}

		static void Move(void* src,void* dest)
		{
			new (dest) T(std::move(*static_cast<T*>(src)));
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

	template<typename ElementType>
	struct SmallObjectFuncCollection<std::vector<ElementType>> : SmallObjectFuncCollectionBase
	{
		using T = std::vector<ElementType>;

		static void Copy(const void* src, void* dest)
		{
			new (dest) T(*static_cast<const T*>(src));
		}

		static void Move(void* src, void* dest)
		{
			new (dest) T(std::move(*static_cast<T*>(src)));
		}

		static void Destroy(void* ptr)
		{
			static_cast<T*>(ptr)->~T();
		}

		static size_t GetVectorSize(const void* ptr)
		{
			return static_cast<const T*>(ptr)->size();
		}

		static Any GetElement(void* ptr, size_t index);

		SmallObjectFuncCollection()
		{
			copy_func = &Copy;
			move_func = &Move;
			destroy_func = &Destroy;
			vector_size_func = &GetVectorSize;
			get_element_func = &GetElement;
		}
	};

	struct BigObjectFuncCollectionBase
	{
		using CopyFunc = void*(*)(const void*);
		using MoveFunc = void*(*)(void*);
		using DestroyFunc = void(*)(void*);
		using VectorSizeFunc = size_t(*)(const void*);
		using GetElementFunc = Any(*)(void*, size_t);

		CopyFunc copy_func = nullptr;
		MoveFunc move_func = nullptr;
		DestroyFunc destroy_func = nullptr;
		VectorSizeFunc vector_size_func = nullptr;
		GetElementFunc get_element_func = nullptr;
	};

	template<typename T>
	struct BigObjectFuncCollection : BigObjectFuncCollectionBase
	{
		static void* Copy(const void* src)
		{
			return new T(*static_cast<const T*>(src));
		}

		static void* Move(void* src)
		{
			return new T(std::move(*static_cast<T*>(src)));
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

	template<typename ElementType>
	struct BigObjectFuncCollection<std::vector<ElementType>> : BigObjectFuncCollectionBase
	{
		using T = std::vector<ElementType>;

		static void* Copy(const void* src)
		{
			return new T(*static_cast<const T*>(src));
		}

		static void* Move(void* src)
		{
			return new T(std::move(*static_cast<T*>(src)));
		}

		static void Destroy(void* ptr)
		{
			T* v = static_cast<T*>(ptr);
			delete v;
		}

		static size_t GetVectorSize(const void* ptr)
		{
			return static_cast<const T*>(ptr)->size();
		}

		static Any GetElement(void* ptr, size_t index);

		BigObjectFuncCollection()
		{
			copy_func = &Copy;
			move_func = &Move;
			destroy_func = &Destroy;
			vector_size_func = &GetVectorSize;
			get_element_func = &GetElement;
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
			data_.is_ref = false;
		}

		template<typename Value,typename = std::enable_if_t<!std::is_same_v<std::decay_t<Value>,Any>>>
		Any(Value&& value)
		{
			Ctor<std::decay_t<Value>>(std::forward<Value>(value));
			data_.is_ref = false;
			data_.type = Type::Get<std::decay_t<Value>>();
		}

		template<typename Value>
		Any(std::reference_wrapper<Value> value)
		{
			Ctor<std::reference_wrapper<Value>>(value);
			data_.is_ref = true;
			data_.type = Type::Get<Value>();
		}
		
		template<typename Value>
		Any(std::vector<Value>& value)
		{
			Ctor<std::reference_wrapper<Value>>(value);
			data_.is_ref = false;
			data_.type = Type::Get<std::vector<Value>>();
		}

		Any(const Any& other)
		{
			Copy(other);
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

			Reset();
			Copy(other);

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

		~Any()
		{
			Reset();
		}

		template<typename Value>
		std::decay_t<Value>& Cast()
		{
			LIGHT_ASSERT(IsValid(), "Invalid Any");

			if (data_.object_type == AnyValueObjectType::kTrivial)
			{
				if (data_.is_ref)
				{
					return *reinterpret_cast<std::reference_wrapper<std::decay_t<Value>>*>(data_.trivial_object_data.data);
				}
				else
				{
					return *reinterpret_cast<std::decay_t<Value>*>(data_.trivial_object_data.data);
				}
			}
			else if (data_.object_type == AnyValueObjectType::kSmall)
			{
				return *reinterpret_cast<std::decay_t<Value>*>(data_.small_object_data.data);
			}
			else
			{
				return *static_cast<std::decay_t<Value>*>(data_.big_object_data.ptr);
			}
		}

		template<typename Value>
		std::decay_t<Value>& Cast() const
		{
			return const_cast<Any*>(this)->Cast<Value>();
		}

		size_t GetSize() const
		{
			LIGHT_ASSERT(data_.type.IsArray(), "any is not array");

			if (data_.object_type == AnyValueObjectType::kSmall)
			{
				return data_.small_object_data.object_func_collection->vector_size_func(data_.small_object_data.data);
			}
			else
			{
				return data_.big_object_data.object_func_collection->vector_size_func(data_.big_object_data.ptr);
			}
		}

		Any GetElement(size_t index)
		{
			LIGHT_ASSERT(data_.type.IsArray(), "any is not array");

			if (data_.object_type == AnyValueObjectType::kSmall)
			{
				return data_.small_object_data.object_func_collection->get_element_func(data_.small_object_data.data,index);
			}
			else
			{
				return data_.big_object_data.object_func_collection->get_element_func(data_.big_object_data.ptr,index);
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
				data_.is_ref = false;
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
				data_.big_object_data.object_func_collection = &kBigObjectFuncCollection<Value>;
			}
		}

		void Copy(const Any& other)
		{
			if (!other.IsValid())
			{
				return;
			}

			data_.object_type = other.data_.object_type;
			data_.is_ref = other.data_.is_ref;

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

		void Move(Any& other) noexcept
		{
			if (!other.IsValid())
			{
				return;
			}

			data_.object_type = other.data_.object_type;
			data_.is_ref = other.data_.is_ref;
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
				data_.big_object_data.ptr = data_.big_object_data.object_func_collection->move_func(other.data_.big_object_data.ptr);
			}

			other.data_.object_type = AnyValueObjectType::kInvalid;
			other.data_.is_ref = false;
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
			Type type;
			bool is_ref;
		};

		Data data_;
	};

	template <typename ElementType>
	Any SmallObjectFuncCollection<std::vector<ElementType>>::GetElement(void* ptr, size_t index)
	{
		return Any(std::ref(static_cast<T*>(ptr)[index]));
	}

	template <typename ElementType>
	Any BigObjectFuncCollection<std::vector<ElementType>>::GetElement(void* ptr, size_t index)
	{
		return Any(std::ref(static_cast<T*>(ptr)[index]));
	}
}
