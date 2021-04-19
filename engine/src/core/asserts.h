#pragma once

#include "defines.h"

// define/undefine based on whether you want to enable or disable assertion, most propably you will not
// need to disable it.
#define MIRAI_ASSERTION_ENABLED

#if defined(MIRAI_ASSERTION_ENABLED)
    // TODO: understand and document why do we need this check?!
    #if _MSC_VER
        // __debugbreak: causes a breakpoint in the code where the user will be prompted to run debugger.
        #define mc_debug_break() __debugbreak()
    #else
        // __builtin_trap: not continue next instruction
        #define mc_debug_break() __builtin_trap()
    #endif

    // report assertion failure by logging it, most propably you won't need to call this function outside
    // the macros defined bellow.
    //  expression: string for the failed expression
    //  message: string to log as the reason for the failure
    //  file: file where assertion failed
    //  line: line in the file where assertion failed
    MIRAI_API void
    mc_report_assertion_failure(const char *expression, const char *message, const char *file, i32 line);

    // assert an expression in release and debug mode
    #define MC_ASSERT(expr)                                             \
    {                                                                   \
        if (!(expr))                                                    \
        {                                                               \
            mn_report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            mc_debug_break();                                           \
        }                                                               \
    }

    // assert an expression and report error message in release and debug mode
    #define MC_ASSERT_MSG(expr, message)                                     \
    {                                                                        \
        if (!(expr))                                                         \
        {                                                                    \
            mc_report_assertion_failure(#expr, message, __FILE__, __LINE__); \
            mc_debug_break();                                                \
        }                                                                    \
    }

    #if defined(MIRAI_DEBUG)
        // assert an expression in debug mode
        #define MC_ASSERT_DEBUG(expr)                                       \
        {                                                                   \
            if (!(expr))                                                    \
            {                                                               \
                mc_report_assertion_failure(#expr, "", __FILE__, __LINE__); \
                mc_debug_break();                                           \
            }                                                               \
        }
        // assert an expression and report error message in debug mode only
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
