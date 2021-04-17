#pragma once

#include "defines.h"

#define MC_LOG_WARN_ENABLED  1
#define MC_LOG_INFO_ENABLED  1
#define MC_LOG_DEBUG_ENABLED 1
#define MC_LOG_TRACE_ENABLED 1
#define MC_LOG_ERROR_ENABLED 1
#define MC_LOG_FATAL_ENABLED 1

#if MIRAI_RELEASE == 1
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

MIRAI_API void
mc_log(MC_LOG_LEVEL level, const char *message, ...);

#if MC_LOG_TRACE_ENABLED == 1
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