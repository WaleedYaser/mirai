#include <platform/platfom.h>
#include <core/logger.h>
#include <core/asserts.h>

static const char *
mp_key_str(MP_KEY key)
{
    switch (key)
    {
        case MP_KEY_0: return "0";
        case MP_KEY_1: return "1";
        case MP_KEY_2: return "2";
        case MP_KEY_3: return "3";
        case MP_KEY_4: return "4";
        case MP_KEY_5: return "5";
        case MP_KEY_6: return "6";
        case MP_KEY_7: return "7";
        case MP_KEY_8: return "8";
        case MP_KEY_9: return "9";
        case MP_KEY_A: return "a";
        case MP_KEY_B: return "b";
        case MP_KEY_C: return "c";
        case MP_KEY_D: return "d";
        case MP_KEY_E: return "e";
        case MP_KEY_F: return "f";
        case MP_KEY_G: return "g";
        case MP_KEY_H: return "h";
        case MP_KEY_I: return "i";
        case MP_KEY_J: return "j";
        case MP_KEY_K: return "k";
        case MP_KEY_L: return "l";
        case MP_KEY_M: return "m";
        case MP_KEY_N: return "n";
        case MP_KEY_O: return "o";
        case MP_KEY_P: return "p";
        case MP_KEY_Q: return "q";
        case MP_KEY_R: return "r";
        case MP_KEY_S: return "s";
        case MP_KEY_T: return "t";
        case MP_KEY_U: return "u";
        case MP_KEY_V: return "v";
        case MP_KEY_W: return "w";
        case MP_KEY_X: return "x";
        case MP_KEY_Y: return "y";
        case MP_KEY_Z: return "z";
        case MP_KEY_SPACE: return "space";
        case MP_KEY_ENTER: return "enter";
        case MP_KEY_ESCAPE: return "escape";
        case MP_KEY_NONE: return "none";
    }
}

int
main(void)
{
    MP_Window *window = mp_window_create("mirai playground", 1280, 720);
    if (!window)
    {
        MC_FATAL("failed to create window");
        return 1;
    }

    MC_INFO("window created with title: '%s', width: %d, and height: %d",
        window->title, window->width, window->height);

    // game loop
    b8 running = TRUE;
    while (running)
    {
        // event loop
        while (mp_window_poll(window))
        {
            switch (window->last_event.type)
            {
                case MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS:
                {
                    switch (window->last_event.mouse_button_press.button)
                    {
                        case MP_MOUSE_BUTTON_LEFT:
                            MC_TRACE("left mouse pressed");
                            break;
                        case MP_MOUSE_BUTTON_RIGHT:
                            MC_TRACE("right mouse pressed");
                            break;
                        case MP_MOUSE_BUTTON_MIDDLE:
                            MC_TRACE("middle mouse pressed");
                            break;
                        case MP_MOUSE_BUTTON_NONE:
                            break;
                    }
                    break;
                }
                case MP_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_DOWN:
                    MC_TRACE("mouse wheel scroll down");
                    break;
                case MP_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_UP:
                    MC_TRACE("mouse wheel scroll up");
                    break;
                case MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE:
                {
                    switch (window->last_event.mouse_button_release.button)
                    {
                        case MP_MOUSE_BUTTON_LEFT:
                            MC_TRACE("left mouse released");
                            break;
                        case MP_MOUSE_BUTTON_RIGHT:
                            MC_TRACE("right mouse released");
                            break;
                        case MP_MOUSE_BUTTON_MIDDLE:
                            MC_TRACE("middle mouse released");
                            break;
                        case MP_MOUSE_BUTTON_NONE:
                            break;
                    }
                    break;
                }
                case MP_WINDOW_EVENT_TYPE_MOUSE_MOVE:
                    MC_TRACE("mouse move: %d, %d",
                        window->last_event.mouse_move.x,
                        window->last_event.mouse_move.y);
                    break;
                case MP_WINDOW_EVENT_TYPE_KEY_PRESS:
                    MC_TRACE("key %s press",
                        mp_key_str(window->last_event.key_press.key));
                    break;
                case MP_WINDOW_EVENT_TYPE_KEY_RELEASE:
                    MC_TRACE("key %s released",
                        mp_key_str(window->last_event.key_release.key));
                    break;
                case MP_WINDOW_EVENT_TYPE_WINDOW_RESIZE:
                    MC_TRACE("window resize: %d, %d",
                        window->last_event.window_resize.width,
                        window->last_event.window_resize.height);
                    break;
                case MP_WINDOW_EVENT_TYPE_WINDOW_CLOSE:
                    running = FALSE;
                    break;
                case MP_WINDOW_EVENT_TYPE_NONE:
                    // do nothing
                    break;
            }
        }
    }

    mp_window_destroy(window);
    MC_INFO("window cloesed");

    return 0;
}
