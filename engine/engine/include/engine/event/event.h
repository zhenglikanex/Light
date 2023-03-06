#pragma once

#include <string>
#include <string_view>
#include <functional>

#define EVENT_IMPL(Class,Type,Category) \
	static EventType StaticType() { return Type; } \
	static EventCategory StaticCategory() { return Category; }\
	EventType GetType() const override { return Type; }; \
	EventCategory GetCategory() const override { return Category; }

namespace light
{
	enum class EventType
	{
		kKeyPressed,
		kKeyReleased,
		kMouse,
		kMousePressed,
		kMouseReleased,
		kWindowsResize,
		kWindowsClose,
	};

	enum class EventCategory
	{
		kKey		= 1 << 1,
		kMouse		= 1 << 2,
		kWindows	= 1 << 3
	};

	class Event
	{
	public:
		explicit Event(std::string_view name);

		virtual ~Event() = default;

		virtual EventType GetType() const = 0;

		virtual EventCategory GetCategory() const = 0;

		virtual std::string ToString() = 0;
	private:
		std::string name_;
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
		void Dispatch(const std::function<void(Event&)>& func)
		{
			if (event_.GetType() == T::StaticType())
			{
				func(event_);
			}
		}
	private:
		const Event& event_;
	};
}