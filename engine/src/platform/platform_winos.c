#include "platform/platfom.h"

#if defined(MIRAI_PLATFORM_WINOS)

Platform_Window *
platform_window_create(const char *title, i32 width, i32 height)
{
    // TODO: implement
    return NULL;
}

void
platform_window_destroy(Platform_Window window)
{
    // TODO: implement
}

b8
platform_window_poll(Platform_Window window)
{
    // TODO: implement
    return FALSE;
}

void
mp_console_write(const char *message, MP_COLOR color)
{
    // TODO: implement
}

void
mp_console_write_error(const char *message, MP_COLOR color)
{
    // TODO: implement
}

#endif