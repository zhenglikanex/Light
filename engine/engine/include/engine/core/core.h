#pragma once

#include <cassert>

#include "engine/log/log.h"

#if DEBUG
#define LIGHT_ASSET(cond,msg) if(!cond) { LOG_ENGINE_ERROR(msg); assert(false && msg); }  
#else
#define LIGHT_ASSET(cond,msg)
#endif