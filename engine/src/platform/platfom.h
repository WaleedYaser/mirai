#pragma once

#include <defines.h>

typedef enum PLATFORM_WINDOW_EVENT_TYPE {
    PLATFORM_WINDOW_EVENT_TYPE_NONE,
    PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS,
    PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE,
    PLATFORM_WINDOW_EVENT_TYPE_MOUSE_MOVE,
    PLATFORM_WINDOW_EVENT_TYPE_KEY_PRESS,
    PLATFORM_WINDOW_EVENT_TYPE_KEY_RELEASE,
    PLATFORM_WINDOW_EVENT_TYPE_WINDOW_CLOSE
} PLATFORM_WINDOW_EVENT_TYPE;

typedef struct Platrofm_Window_Event {
    PLATFORM_WINDOW_EVENT_TYPE type;
    union {

    };
} Platform_Window_Event;

typedef struct Platform_Window {
    const char *title;
    i32 x, y;
    i32 width, height;
    Platform_Window_Event last_event;
} Platform_Window;

typedef struct Platform_Window_Desc {
    const char *title;
    i32 x, y;
    i32 width, height;
} Platform_Window_Desc;

MIRAI_API Platform_Window *
platform_window_create(Platform_Window_Desc desc);

MIRAI_API void
platform_window_destroy(Platform_Window *window);

MIRAI_API b8
platform_window_poll(Platform_Window *window);