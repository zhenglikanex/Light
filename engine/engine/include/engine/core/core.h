#pragma once

#include "light_pch.h"

#include "engine/log/log.h"

#ifdef LIGHT_PLATFORM_WINDOWS
#define LIGHT_DEBUG_BREAK() __debugbreak()
#elif defined(LIGHT_PLATFORM_LINUX)
#define LIGHT_DEBUG_BREAK() raise(SIGTRAP)
#else
#define LIGHT_DEBUG_BREAK() assert(false)
#endif

#if DEBUG
#define LIGHT_ASSERT(cond,msg) if(!(cond)) { LOG_ENGINE_ERROR("{} {} Assert Failed:{}",__FILE__,__LINE__,msg); LIGHT_DEBUG_BREAK(); }  
#else
#define LIGHT_ASSERT(cond,msg)
#endif

#define STATISTICS 1
#define PROFILE 1

namespace light 
{
	struct RefCounter
	{
		RefCounter() = default;
		virtual ~RefCounter() = 0 {}

		// Non-copyable and non-movable
		RefCounter(const RefCounter&) = delete;
		RefCounter(const RefCounter&&) = delete;
		RefCounter& operator=(const RefCounter&) = delete;
		RefCounter& operator=(const RefCounter&&) = delete;

		uint32_t AddRef()
		{
			return ++ref_count_;
		}

		uint32_t Release()
		{
			uint32_t count = --ref_count_;
			if (count == 0)
			{
				delete this;
			}
			return count;
		}
	private:
		std::atomic<uint32_t> ref_count_{ 1 };
	};

    template <typename T>
    class Ref
    {
    public:
        typedef T InterfaceType;

        template <bool b, typename U = void>
        struct EnableIf
        {
        };

        template <typename U>
        struct EnableIf<true, U>
        {
            typedef U type;
        };

    protected:
        InterfaceType* ptr_;
        template<class U> friend class Ref;

        void InternalAddRef() const noexcept
        {
            if (ptr_ != nullptr)
            {
                ptr_->AddRef();
            }
        }

        unsigned long InternalRelease() noexcept
        {
            unsigned long ref = 0;
            T* temp = ptr_;

            if (temp != nullptr)
            {
                ptr_ = nullptr;
                ref = temp->Release();
            }

            return ref;
        }

    public:
        Ref() noexcept : ptr_(nullptr)
        {
        }

        Ref(std::nullptr_t) noexcept : ptr_(nullptr)
        {
        }

        template<class U>
        Ref(U* other) noexcept : ptr_(other)
        {
            InternalAddRef();
        }

        Ref(const Ref& other) noexcept : ptr_(other.ptr_)
        {
            InternalAddRef();
        }

        // copy ctor that allows to instanatiate class when U* is convertible to T*
        template<class U>
        Ref(const Ref<U>& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type* = nullptr) noexcept :
            ptr_(other.ptr_)

        {
            InternalAddRef();
        }

        Ref(Ref&& other) noexcept : ptr_(nullptr)
        {
            if (this != reinterpret_cast<Ref*>(&reinterpret_cast<unsigned char&>(other)))
            {
                Swap(other);
            }
        }

        // Move ctor that allows instantiation of a class when U* is convertible to T*
        template<class U>
        Ref(Ref<U>&& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type* = nullptr) noexcept :
            ptr_(other.ptr_)
        {
            other.ptr_ = nullptr;
        }

        ~Ref() noexcept
        {
            InternalRelease();
        }

        Ref& operator=(std::nullptr_t) noexcept
        {
            InternalRelease();
            return *this;
        }

        Ref& operator=(T* other) noexcept
        {
            if (ptr_ != other)
            {
                Ref(other).Swap(*this);
            }
            return *this;
        }

        template <typename U>
        Ref& operator=(U* other) noexcept
        {
            Ref(other).Swap(*this);
            return *this;
        }

        Ref& operator=(const Ref& other) noexcept  // NOLINT(bugprone-unhandled-self-assignment)
        {
            if (ptr_ != other.ptr_)
            {
                Ref(other).Swap(*this);
            }
            return *this;
        }

        template<class U>
        Ref& operator=(const Ref<U>& other) noexcept
        {
            Ref(other).Swap(*this);
            return *this;
        }

        Ref& operator=(Ref&& other) noexcept
        {
            Ref(static_cast<Ref&&>(other)).Swap(*this);
            return *this;
        }

        template<class U>
        Ref& operator=(Ref<U>&& other) noexcept
        {
            Ref(static_cast<Ref<U>&&>(other)).Swap(*this);
            return *this;
        }

        void Swap(Ref&& r) noexcept
        {
            T* tmp = ptr_;
            ptr_ = r.ptr_;
            r.ptr_ = tmp;
        }

        void Swap(Ref& r) noexcept
        {
            T* tmp = ptr_;
            ptr_ = r.ptr_;
            r.ptr_ = tmp;
        }

        [[nodiscard]] T* Get() const noexcept
        {
            return ptr_;
        }

        operator T* () const
        {
            return ptr_;
        }

        InterfaceType* operator->() const noexcept
        {
            return ptr_;
        }

        T** operator&()   // NOLINT(google-runtime-operator)
        {
            return &ptr_;
        }

        [[nodiscard]] T* const* GetAddressOf() const noexcept
        {
            return &ptr_;
        }

        [[nodiscard]] T** GetAddressOf() noexcept
        {
            return &ptr_;
        }

        [[nodiscard]] T** ReleaseAndGetAddressOf() noexcept
        {
            InternalRelease();
            return &ptr_;
        }

        T* Detach() noexcept
        {
            T* ptr = ptr_;
            ptr_ = nullptr;
            return ptr;
        }

        // Set the pointer while keeping the object's reference count unchanged
        void Attach(InterfaceType* other)
        {
            if (ptr_ != nullptr)
            {
                auto ref = ptr_->Release();
                (void)ref;

                // Attaching to the same object only works if duplicate references are being coalesced. Otherwise
                // re-attaching will cause the pointer to be released and may cause a crash on a subsequent dereference.
                assert(ref != 0 || ptr_ != other);
            }

            ptr_ = other;
        }

        // Create a wrapper around a raw object while keeping the object's reference count unchanged
        static Ref<T> Create(T* other)
        {
            Ref<T> Ptr;
            Ptr.Attach(other);
            return Ptr;
        }

        unsigned long Reset()
        {
            return InternalRelease();
        }
    };    // Ref

    template<class T, class ... Args>
    Ref<T> MakeRef(Args&& ... args)
    {
        auto ptr = new T(std::forward<Args>(args)...);
        return Ref<T>::Create(ptr);
    }
}
