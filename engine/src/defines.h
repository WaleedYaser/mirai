#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef i32 b32;

#define TRUE 1
#define FALSE 0

#if defined(__clang__) || defined(__gcc__)
    #define MIRAI_STATIC_ASSERT _Static_assert
#else
    #define MIRAI_STATIC_ASSERT static_assert
#endif

MIRAI_STATIC_ASSERT(sizeof(u8) == 1, "u8 size expected to be 1 byte");
MIRAI_STATIC_ASSERT(sizeof(u16) == 2, "u16 size expected to be 2 byte");
MIRAI_STATIC_ASSERT(sizeof(u32) == 4, "u32 size expected to be 4 byte");
MIRAI_STATIC_ASSERT(sizeof(u64) == 8, "u64 size expected to be 8 byte");

MIRAI_STATIC_ASSERT(sizeof(i8) == 1, "i8 size expected to be 1 byte");
MIRAI_STATIC_ASSERT(sizeof(i16) == 2, "i16 size expected to be 2 byte");
MIRAI_STATIC_ASSERT(sizeof(i32) == 4, "i32 size expected to be 4 byte");
MIRAI_STATIC_ASSERT(sizeof(i64) == 8, "i64 size expected to be 8 byte");

MIRAI_STATIC_ASSERT(sizeof(f32) == 4, "f32 size expected to be 4 byte");
MIRAI_STATIC_ASSERT(sizeof(f64) == 8, "f64 size expected to be 8 byte");

MIRAI_STATIC_ASSERT(sizeof(b8) == 1, "b8 size expected to be 1 byte");
MIRAI_STATIC_ASSERT(sizeof(b32) == 4, "b32 size expected to be 4 byte");

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define MIRAI_PLATFORM_WINOS 1
    #ifndef _WIN64
        #error "64-bit is required on Windows!"
    #endif
#elif
    #error "unsupported platform"
#endif

#if defined(MIRAI_EXPORT)
    #ifdef _MSC_VER
        #define MIRAI_API __declspec(dllexport)
    #else
        #define MIRAI_API __attribute__((visibility("default")))
    #endif
#elif defined(MIRAI_IMPORT)
    #ifdef _MSC_VER
        #define MIRAI_API __declspec(dllimport)
    #else
        #define MIRAI_API
    #endif
#endif
