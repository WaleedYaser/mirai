#include "platform/platfom.h"

#if defined(MIRAI_PLATFORM_WINOS)

Platform_Window *
platform_window_create(const char *, i32, i32)
{
    // TODO: implement
    return NULL;
}

void
platform_window_destroy(Platform_Window)
{
    // TODO: implement
    return NULL;
}

b8
platform_window_poll(Platform_Window)
{
    // TODO: implement
    return FALSE;
}

#endif