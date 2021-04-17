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

    b8 is_error = level > MC_LOG_LEVEL_WARN;

    const i32 msg_length = 32 * 1024;
    char formated_message[msg_length];

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(formated_message, msg_length, message, arg_ptr);
    va_end(arg_ptr);

    char message_with_level[msg_length];
    sprintf(message_with_level, "%s%s\n", level_strings[level], formated_message);

    if (is_error)
        mp_console_write_error(message_with_level, (MP_COLOR)level);
    else
        mp_console_write(message_with_level, (MP_COLOR)level);
}