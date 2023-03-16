#include <typeindex>
#include <iostream>

#include "engine/event/key_event.h"
#include "log/log.h"

using namespace light;

int main()
{
	log::Init();

	KeyPressedEvent e1;
	e1.keycode = 1;

	LOG_ENGINE_TRACE("{}",e1);

	return 0;
}