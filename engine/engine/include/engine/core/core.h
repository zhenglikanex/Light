#pragma once

#include <cassert>

#include "engine/log/log.h"

#ifdef LIGHT_PLATFORM_WINDOWS
#define LIGHT_DEBUG_BREAK() __debugbreak()
#elif defined(LIGHT_PLATFORM_LINUX)
#define LIGHT_DEBUG_BREAK() raise(SIGTRAP)
#else
#define LIGHT_DEBUG_BREAK() assert(false)
#endif

#if DEBUG
#define LIGHT_ASSET(cond,msg) if(!(cond)) { LOG_ENGINE_ERROR("Assert Failed:{}",msg); LIGHT_DEBUG_BREAK(); }  
#else
#define LIGHT_ASSET(cond,msg)
#endif

#define STATISTICS 1
#define PROFILE 1