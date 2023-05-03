#pragma once

namespace meta_generated
{
    static void AutoRegisterTypes();
}


#ifdef META_PARSER
    #define META(...) __attribute__((annotate("reflect-class;" #__VA_ARGS__)))
    #define PROPERTY(...) __attribute__((annotate(#__VA_ARGS__)))
    #define FUNCTION(...) __attribute__((annotate(#__VA_ARGS__)))
    #define FRIEND_META(...) 
#else /* else __METAREFLECT__ */
    #define META(...)
    #define PROPERTY(...)
    #define FUNCTION(...)
    #define FRIEND_META(...)  friend void ::meta_generated::AutoRegisterTypes();
#endif /* __METAREFLECT__ */

namespace meta_generated
{
    class ReflectionBuilder;
}

#include "engine/reflection/registry.h"
#include "engine/reflection/type.h"

extern void RegisterTypesGenerated();