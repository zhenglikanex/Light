#pragma once

#include "rttr/type"

#ifdef __METAREFLECT__
#define CLASS(...) class __attribute__((annotate("reflect-class;" #__VA_ARGS__)))
#define STRUCT(...) struct __attribute__((annotate("reflect-class;" #__VA_ARGS__)))
#define UNION(...) union __attribute__((annotate("reflect-class;" #__VA_ARGS__)))
#define PROPERTY(...) __attribute__((annotate("reflect-property;" #__VA_ARGS__)))
#define FUNCTION(...) __attribute__((annotate("reflect-function;" #__VA_ARGS__)))
#else
#define CLASS(...) class
#define STRUCT(...) struct
#define UNION(...) union
#define PROPERTY(...)
#define FUNCTION(...)
#endif