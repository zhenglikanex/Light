#pragma once

#include <any>
#include <type_traits>

namespace light::meta
{
	// size in pointers of std::function and std::any (roughly 3 pointers larger than std::string when building debug)
	_INLINE_VAR constexpr int kSmallObjectNumPtrs = 6 + 16 / sizeof(void*);

	inline constexpr size_t kAnyTrivialSpaceSize = (kSmallObjectNumPtrs - 1) * sizeof(void*);

	template <class _Ty>
	inline constexpr bool is_trivial =
		alignof(_Ty) <= alignof(max_align_t) && is_trivially_copyable_v<_Ty> && sizeof(_Ty) <= kAnyTrivialSpaceSize;

	inline constexpr size_t kAnySmallSpaceSize = (kSmallObjectNumPtrs - 2) * sizeof(void*);

	template <class _Ty>
	inline constexpr bool is_small = alignof(_Ty) <= alignof(max_align_t) && is_nothrow_move_constructible_v<_Ty> && sizeof(_Ty) <= kAnySmallSpaceSize;

	enum class AnyValueStorageType : uintptr_t { _Trivial, _Big, _Small };

	class Any
	{
	public:
		template<typename T>
		Any(Any&& obj)
		{

		}
	private:
		struct Data
		{
			union
			{
				char small_type_data[];
			};
		};

		
	};
}