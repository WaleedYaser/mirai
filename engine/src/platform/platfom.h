#pragma once

#include <defines.h>

typedef enum PLATFORM_MOUSE_BUTTON {
    PLATFORM_MOUSE_BUTTON_NONE,
    PLATFORM_MOUSE_BUTTON_LEFT,
    PLATFORM_MOUSE_BUTTON_RIGHT,
    PLATFORM_MOUSE_BUTTON_MIDDLE,
} PLATFORM_MOUSE_BUTTON;

typedef enum PLATFORM_KEY {
    PLATFORM_KEY_NONE,
    // numbers
    PLATFORM_KEY_0,
    PLATFORM_KEY_1,
    PLATFORM_KEY_2,
    PLATFORM_KEY_3,
    PLATFORM_KEY_4,
    PLATFORM_KEY_5,
    PLATFORM_KEY_6,
    PLATFORM_KEY_7,
    PLATFORM_KEY_8,
    PLATFORM_KEY_9,
    // letters
    PLATFORM_KEY_A,
    PLATFORM_KEY_B,
    PLATFORM_KEY_C,
    PLATFORM_KEY_D,
    PLATFORM_KEY_E,
    PLATFORM_KEY_F,
    PLATFORM_KEY_G,
    PLATFORM_KEY_H,
    PLATFORM_KEY_I,
    PLATFORM_KEY_J,
    PLATFORM_KEY_K,
    PLATFORM_KEY_L,
    PLATFORM_KEY_M,
    PLATFORM_KEY_N,
    PLATFORM_KEY_O,
    PLATFORM_KEY_P,
    PLATFORM_KEY_Q,
    PLATFORM_KEY_R,
    PLATFORM_KEY_S,
    PLATFORM_KEY_T,
    PLATFORM_KEY_U,
    PLATFORM_KEY_V,
    PLATFORM_KEY_W,
    PLATFORM_KEY_X,
    PLATFORM_KEY_Y,
    PLATFORM_KEY_Z,
    // special keys
    PLATFORM_KEY_SPACE,
    PLATFORM_KEY_ENTER,
    PLATFORM_KEY_ESCAPE,
} PLATFORM_KEY;

typedef enum PLATFORM_WINDOW_EVENT_TYPE {
    PLATFORM_WINDOW_EVENT_TYPE_NONE,
    PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS,
    PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE,
    PLATFORM_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_DOWN,
    PLATFORM_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_UP,
    PLATFORM_WINDOW_EVENT_TYPE_MOUSE_MOVE,
    PLATFORM_WINDOW_EVENT_TYPE_KEY_PRESS,
    PLATFORM_WINDOW_EVENT_TYPE_KEY_RELEASE,
    PLATFORM_WINDOW_EVENT_TYPE_WINDOW_RESIZE,
    PLATFORM_WINDOW_EVENT_TYPE_WINDOW_CLOSE
} PLATFORM_WINDOW_EVENT_TYPE;

typedef struct Platrofm_Window_Event {
    PLATFORM_WINDOW_EVENT_TYPE type;
    union {
        struct {
            PLATFORM_MOUSE_BUTTON button;
        } mouse_button_press, mouse_button_release;

        struct {
            i32 x, y;
        } mouse_move;

        struct {
            PLATFORM_KEY key;
        } key_press, key_release;

        struct {
            i32 width, height;
        } window_resize;
    };
} Platform_Window_Event;

typedef struct Platform_Window {
    const char *title;
    i32 x, y;
    i32 width, height;
    Platform_Window_Event last_event;
} Platform_Window;

MIRAI_API Platform_Window *
platform_window_create(const char *title, i32 width, i32 height);

MIRAI_API void
platform_window_destroy(Platform_Window *window);

MIRAI_API b8
platform_window_poll(Platform_Window *window);