#include "engine/event/key_event.h"

#include "spdlog/fmt/fmt.h"

namespace light
{
    KeyPressedEvent::KeyPressedEvent(int keycode, int repeat_count)
        : keycode(keycode)
        , repeat_count(repeat_count)
    {
    }
    std::string KeyPressedEvent::ToString() const
    {
        return fmt::format("{} : keycode = {} repeat_count = {}", GetName(), keycode, repeat_count);
    }

    KeyReleasedEvent::KeyReleasedEvent(int keycode)
        : keycode(keycode)
    {
    }

    std::string KeyReleasedEvent::ToString() const
    {
        return fmt::format("{} : keycode = {}", GetName(),keycode);
    }
}