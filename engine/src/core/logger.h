#pragma once

#include "defines.h"

// switches to enable or disable specific logging level
#define MC_LOG_WARN_ENABLED  1
#define MC_LOG_INFO_ENABLED  1
#define MC_LOG_DEBUG_ENABLED 1
#define MC_LOG_TRACE_ENABLED 0
#define MC_LOG_ERROR_ENABLED 1
#define MC_LOG_FATAL_ENABLED 1

#if MIRAI_RELEASE == 1
    // disable debug and trace logging in release mode
    #define MC_LOG_DEBUG_ENABLED 0
    #define MC_LOG_TRACE_ENABLED 0
#endif

typedef enum MC_LOG_LEVEL {
    MC_LOG_LEVEL_TRACE = 0,
    MC_LOG_LEVEL_DEBUG = 1,
    MC_LOG_LEVEL_INFO  = 2,
    MC_LOG_LEVEL_WARN  = 3,
    MC_LOG_LEVEL_ERROR = 4,
    MC_LOG_LEVEL_FATAL = 5
} MC_LOG_LEVEL;

// log formated message based on it's log level, most propably you will not need to call this
// function directly and will use bellow macros.
//  level: MC_LOG_LEVEL enum to specify log level
//  message: string to log
//  ...: args to pass for formatting the message
MIRAI_API void
mc_log(MC_LOG_LEVEL level, const char *message, ...);

#if MC_LOG_TRACE_ENABLED == 1
    // __VA_ARGS__: accept variable number of arguments in macro
    // we add '##' before __VA_ARGS__ to remove extra ',' before it if variable arguments are
    // omitted or empty.
    #define MC_TRACE(message, ...) mc_log(MC_LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
    #define MC_TRACE(message, ...) // do nothing
#endif

#if MC_LOG_DEBUG_ENABLED == 1
    #define MC_DEBUG(message, ...) mc_log(MC_LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
    #define MC_DEBUG(message, ...) // do nothing
#endif

#if MC_LOG_INFO_ENABLED == 1
    #define MC_INFO(message, ...) mc_log(MC_LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
    #define MC_INFO(message, ...) // do nothing
#endif

#if MC_LOG_WARN_ENABLED == 1
    #define MC_WARN(message, ...) mc_log(MC_LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
    #define MC_WARN(message, ...) // do nothing
#endif

#if MC_LOG_ERROR_ENABLED == 1
    #define MC_ERROR(message, ...) mc_log(MC_LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#else
    #define MC_ERROR(message, ...) // do nothing
#endif

#if MC_LOG_FATAL_ENABLED == 1
    #define MC_FATAL(message, ...) mc_log(MC_LOG_LEVEL_FATAL, message, ##__VA_ARGS__)
#else
    #define MC_FATAL(message, ...) // do nothing
#endif
