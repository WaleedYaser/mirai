#include "core/logger.h"
#include "platform/platfom.h"

#include <stdio.h>
#include <stdarg.h>

void
mc_log(MC_LOG_LEVEL level, const char *message, ...)
{
    const char *level_strings[6] = {
        "[MIRAI TRACE]: ",
        "[MIRAI DEBUG]: ",
        "[MIRAI INFO]:  ",
        "[MIRAI WARN]:  ",
        "[MIRAI ERROR]: ",
        "[MIRAI FATAL]: "
    };

    // get variadic arguments after message and format it into formated message, note that we have
    // a 32K limmit for the formated message.
    char formated_message[32 * 1024];
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(formated_message, sizeof(formated_message), message, arg_ptr);
    va_end(arg_ptr);

    // add level string to formated message and store result in message_with_level, we have the
    // same size limit for message_with_level
    char message_with_level[32 * 1024];
    sprintf(message_with_level, "%s%s\n", level_strings[level], formated_message);

    MP_COLOR color;
    switch (level)
    {
        case MC_LOG_LEVEL_TRACE: color = MP_COLOR_FG_GRAY; break;
        case MC_LOG_LEVEL_DEBUG: color = MP_COLOR_FG_BLUE; break;
        case MC_LOG_LEVEL_INFO:  color = MP_COLOR_FG_GRAY; break;
        case MC_LOG_LEVEL_WARN:  color = MP_COLOR_FG_YELLOW; break;
        case MC_LOG_LEVEL_ERROR: color = MP_COLOR_FG_RED; break;
        case MC_LOG_LEVEL_FATAL: color = MP_COLOR_BG_RED; break;
    }

    // if level is error write it to error console instead
    if (level > MC_LOG_LEVEL_WARN)
        mp_console_write_error(message_with_level, color);
    else
        mp_console_write(message_with_level, color);
}
