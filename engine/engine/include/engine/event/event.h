#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <typeindex>
#include <ostream>

#define EVENT_IMPL(Class,Type,Category) \
	const char* GetName() const  override{ return #Class; } \
	static EventType StaticType() { return Type; } \
	static EventCategory StaticCategory() { return Category; }\
	EventType GetType() const override { return Type; }; \
	EventCategory GetCategory() const override { return Category; }

#define BIND_EVENT_1(...) std::bind(__VA_ARGS__, std::placeholders::_1)

namespace light
{
	enum class EventType
	{
		kKeyPressed,
		kKeyReleased,
		kMouseMoved,
		kMouseButtonPressed,
		kMouseButtonReleased,
		kMouseScrolled,
		kWindowsResized,
		kWindowsClosed,
		kWindowsMoved,
	};

	enum class EventCategory
	{
		kNone		= 0,
		kKey		= 1 << 0,
		kMouse		= 1 << 1,
		kWindows	= 1 << 2
	};

	struct Event
	{
		Event() = default;
		virtual ~Event() = default;

		virtual EventType GetType() const = 0;

		virtual EventCategory GetCategory() const = 0;

		virtual const char* GetName() const = 0;

		virtual std::string ToString() const = 0;
	};

	class EventDispatcher
	{
	public:
		explicit EventDispatcher(const Event& event)
			: event_(event)
		{
			std::function<void()> func;
		}

		template<class T>
		requires requires {
			std::is_function_v<decltype(T::StaticType)>;
			std::is_base_of_v<Event, T>;
		}
		void Dispatch(const std::function<void(const T&)>& func)
		{
			if (event_.GetType() == T::StaticType())
			{
				func(static_cast<const T&>(event_));
			}
		}
	private:
		const Event& event_;
	};

	template<class Ostream>
	inline Ostream& operator<<(Ostream& ostream, const Event& e)
	{
		return ostream << e.ToString();
	}
}