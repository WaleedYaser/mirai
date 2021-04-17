#pragma once

#include <defines.h>

typedef enum MP_MOUSE_BUTTON {
    MP_MOUSE_BUTTON_NONE,
    MP_MOUSE_BUTTON_LEFT,
    MP_MOUSE_BUTTON_RIGHT,
    MP_MOUSE_BUTTON_MIDDLE,
} MP_MOUSE_BUTTON;

typedef enum MP_KEY {
    MP_KEY_NONE,
    // numbers
    MP_KEY_0,
    MP_KEY_1,
    MP_KEY_2,
    MP_KEY_3,
    MP_KEY_4,
    MP_KEY_5,
    MP_KEY_6,
    MP_KEY_7,
    MP_KEY_8,
    MP_KEY_9,
    // letters
    MP_KEY_A,
    MP_KEY_B,
    MP_KEY_C,
    MP_KEY_D,
    MP_KEY_E,
    MP_KEY_F,
    MP_KEY_G,
    MP_KEY_H,
    MP_KEY_I,
    MP_KEY_J,
    MP_KEY_K,
    MP_KEY_L,
    MP_KEY_M,
    MP_KEY_N,
    MP_KEY_O,
    MP_KEY_P,
    MP_KEY_Q,
    MP_KEY_R,
    MP_KEY_S,
    MP_KEY_T,
    MP_KEY_U,
    MP_KEY_V,
    MP_KEY_W,
    MP_KEY_X,
    MP_KEY_Y,
    MP_KEY_Z,
    // special keys
    MP_KEY_SPACE,
    MP_KEY_ENTER,
    MP_KEY_ESCAPE,
} MP_KEY;

typedef enum MP_WINDOW_EVENT_TYPE {
    MP_WINDOW_EVENT_TYPE_NONE,
    MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS,
    MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE,
    MP_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_DOWN,
    MP_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_UP,
    MP_WINDOW_EVENT_TYPE_MOUSE_MOVE,
    MP_WINDOW_EVENT_TYPE_KEY_PRESS,
    MP_WINDOW_EVENT_TYPE_KEY_RELEASE,
    MP_WINDOW_EVENT_TYPE_WINDOW_RESIZE,
    MP_WINDOW_EVENT_TYPE_WINDOW_CLOSE
} MP_WINDOW_EVENT_TYPE;

typedef struct MP_Window_Event {
    MP_WINDOW_EVENT_TYPE type;
    union {
        struct {
            MP_MOUSE_BUTTON button;
        } mouse_button_press, mouse_button_release;

        struct {
            i32 x, y;
        } mouse_move;

        struct {
            MP_KEY key;
        } key_press, key_release;

        struct {
            i32 width, height;
        } window_resize;
    };
} MP_Window_Event;

typedef struct MP_Window {
    const char *title;
    i32 width, height;
    MP_Window_Event last_event;
} MP_Window;

MIRAI_API MP_Window *
mp_window_create(const char *title, i32 width, i32 height);

MIRAI_API void
mp_window_destroy(MP_Window *window);

MIRAI_API b8
mp_window_poll(MP_Window *window);

typedef enum MP_COLOR {
    MP_COLOR_FG_GRAY,
    MP_COLOR_FG_BLUE,
    MP_COLOR_FG_GREEN,
    MP_COLOR_FG_YELLOW,
    MP_COLOR_FG_RED,
    MP_COLOR_BG_RED
} MP_COLOR;

MIRAI_API void
mp_console_write(const char *message, MP_COLOR color);

MIRAI_API void
mp_console_write_error(const char *message, MP_COLOR color);