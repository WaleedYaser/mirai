#pragma once

#include "defines.h"

#define MIRAI_ASSERTION_ENABLED

#if defined(MIRAI_ASSERTION_ENABLED)
    #if _MSC_VER
        #include <intrin.h>
        #define mc_debug_break() __debugbreak()
    #else
        #define mc_debug_break() __builtin_trap()
    #endif

    MIRAI_API void
    mc_report_assertion_failure(const char *expression, const char *message, const char *file, i32 line);

    #define MC_ASSERT(expr)                                             \
    {                                                                   \
        if (!(expr))                                                    \
        {                                                               \
            mn_report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            mc_debug_break();                                           \
        }                                                               \
    }

    #define MC_ASSERT_MSG(expr, message)                                     \
    {                                                                        \
        if (!(expr))                                                         \
        {                                                                    \
            mc_report_assertion_failure(#expr, message, __FILE__, __LINE__); \
            mc_debug_break();                                                \
        }                                                                    \
    }

    #if defined(MIRAI_DEBUG)
        #define MC_ASSERT_DEBUG(expr)                                       \
        {                                                                   \
            if (!(expr))                                                    \
            {                                                               \
                mc_report_assertion_failure(#expr, "", __FILE__, __LINE__); \
                mc_debug_break();                                           \
            }                                                               \
        }
        #define MC_ASSERT_MSG_DEBUG(expr, message)                               \
        {                                                                        \
            if (!(expr))                                                         \
            {                                                                    \
                mc_report_assertion_failure(#expr, message, __FILE__, __LINE__); \
                mc_debug_break();                                                \
            }                                                                    \
        }
    #else
        #define MC_ASSERT_DEBUG(expr)              // do nothing
        #define MC_ASSERT_MSG_DEBUG(expr, message) // do nothing
    #endif
#else
    #define MC_ASSERT(expr)              // do nothing
    #define MC_ASSERT_MSG(expr, message) // do nothing
    #define MC_ASSERT_DEBUG(expr)        // do nothing
#endif