#pragma once

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef signed char        i8;
typedef signed short       i16;
typedef signed int         i32;
typedef signed long long   i64;

typedef float              f32;
typedef double             f64;

typedef i8                 b8;
typedef i32                b32;

#define I8_MIN  ((i8)(-128))
#define I16_MIN ((i16)(-32767-1))
#define I32_MIN ((i32)(-2147483647-1))
#define I64_MIN ((i64)(-9223372036854775807ll-1))
#define U8_MIN  ((u8)0u)
#define U16_MIN ((u16)0u)
#define U32_MIN ((u32)0u)
#define U64_MIN ((u64)0ull)
#define F32_MIN ((f32)1.175494351e-38f)          // min normalized positive value
#define F64_MIN ((f64)2.2250738585072014e-308)   // min normalized positive value

#define I8_MAX  ((i8)127)
#define I16_MAX ((i16)32767)
#define I32_MAX ((i32)2147483647)
#define I64_MAX ((i64)9223372036854775807ll)
#define U8_MAX  ((u8)255u)
#define U16_MAX ((u16)65535u)
#define U32_MAX ((u32)4294967295u)
#define U64_MAX ((u64)18446744073709551615ull)
#define F32_MAX ((f32)3.402823466e+38f)          // max value
#define F64_MAX ((f64)1.7976931348623158e+308)   // max valueypedef i32 b32;

#define TRUE 1
#define FALSE 0

#if defined(__clang__) || defined(__gcc__)
    #define MIRAI_STATIC_ASSERT _Static_assert
#else
    #define MIRAI_STATIC_ASSERT static_assert
#endif

MIRAI_STATIC_ASSERT(sizeof(u8)  == 1, "u8 size expected to be 1 byte");
MIRAI_STATIC_ASSERT(sizeof(u16) == 2, "u16 size expected to be 2 byte");
MIRAI_STATIC_ASSERT(sizeof(u32) == 4, "u32 size expected to be 4 byte");
MIRAI_STATIC_ASSERT(sizeof(u64) == 8, "u64 size expected to be 8 byte");

MIRAI_STATIC_ASSERT(sizeof(i8)  == 1, "i8 size expected to be 1 byte");
MIRAI_STATIC_ASSERT(sizeof(i16) == 2, "i16 size expected to be 2 byte");
MIRAI_STATIC_ASSERT(sizeof(i32) == 4, "i32 size expected to be 4 byte");
MIRAI_STATIC_ASSERT(sizeof(i64) == 8, "i64 size expected to be 8 byte");

MIRAI_STATIC_ASSERT(sizeof(f32) == 4, "f32 size expected to be 4 byte");
MIRAI_STATIC_ASSERT(sizeof(f64) == 8, "f64 size expected to be 8 byte");

MIRAI_STATIC_ASSERT(sizeof(b8)  == 1, "b8 size expected to be 1 byte");
MIRAI_STATIC_ASSERT(sizeof(b32) == 4, "b32 size expected to be 4 byte");

#if 0
    // TODO: move this to unit tests
    #include <stdint.h>
    #include <float.h>

    MIRAI_STATIC_ASSERT(I8_MIN  == INT8_MIN,   "I8_MIN expected to equal INT8_MIN");
    MIRAI_STATIC_ASSERT(I16_MIN == INT16_MIN,  "I16_MIN expected to equal INT16_MIN");
    MIRAI_STATIC_ASSERT(I32_MIN == INT32_MIN,  "I32_MIN expected to equal INT32_MIN");
    MIRAI_STATIC_ASSERT(I64_MIN == INT64_MIN,  "I64_MIN expected to equal INT64_MIN");
    MIRAI_STATIC_ASSERT(U8_MIN  == 0,          "U8_MIN expected to equal 0");
    MIRAI_STATIC_ASSERT(U16_MIN == 0,          "U16_MIN expected to equal 0");
    MIRAI_STATIC_ASSERT(U32_MIN == 0,          "U32_MIN expected to equal 0");
    MIRAI_STATIC_ASSERT(U64_MIN == 0,          "U64_MIN expected to equal 0");
    // MIRAI_STATIC_ASSERT(F32_MIN < FLT_MIN,    "F32_MIN expected to equal FLT_MIN");
    // MIRAI_STATIC_ASSERT(F64_MIN < DBL_MIN,    "F64_MIN expected to equal DBL_MIN");

    MIRAI_STATIC_ASSERT(I8_MAX  == INT8_MAX,   "I8_MAX expected to equal INT8_MIN");
    MIRAI_STATIC_ASSERT(I16_MAX == INT16_MAX,  "I16_MAX expected to equal INT16_MIN");
    MIRAI_STATIC_ASSERT(I32_MAX == INT32_MAX,  "I32_MAX expected to equal INT32_MIN");
    MIRAI_STATIC_ASSERT(I64_MAX == INT64_MAX,  "I64_MAX expected to equal INT64_MIN");
    MIRAI_STATIC_ASSERT(U8_MAX  == UINT8_MAX,  "U8_MAX expected to equal UINT8_MAX");
    MIRAI_STATIC_ASSERT(U16_MAX == UINT16_MAX, "U16_MAX expected to equal UINT16_MAX");
    MIRAI_STATIC_ASSERT(U32_MAX == UINT32_MAX, "U32_MAX expected to equal UINT32_MAX");
    MIRAI_STATIC_ASSERT(U64_MAX == UINT64_MAX, "U64_MAX expected to equal UINT64_MAX");
    // MIRAI_STATIC_ASSERT(F32_MAX < FLT_MAX,    "F32_MAX expected to equal FLT_MAX");
    // MIRAI_STATIC_ASSERT(F64_MAX < DBL_MAX,    "F64_MAX expected to equal DBL_MAX");
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define MIRAI_PLATFORM_WINOS 1
    #ifndef _WIN64
        #error "64-bit is required on Windows!"
    #endif
#elif defined(__linux__) || defined(__gnu_linux__)
    #define MIRAI_PLATFORM_LINUX 1
#else
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
