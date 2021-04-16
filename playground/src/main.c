#include <platform/platfom.h>

#include <stdio.h>
#include <assert.h>

static const char *
platform_key_str(PLATFORM_KEY key)
{
    switch (key)
    {
        case PLATFORM_KEY_0: return "0";
        case PLATFORM_KEY_1: return "1";
        case PLATFORM_KEY_2: return "2";
        case PLATFORM_KEY_3: return "3";
        case PLATFORM_KEY_4: return "4";
        case PLATFORM_KEY_5: return "5";
        case PLATFORM_KEY_6: return "6";
        case PLATFORM_KEY_7: return "7";
        case PLATFORM_KEY_8: return "8";
        case PLATFORM_KEY_9: return "9";
        case PLATFORM_KEY_A: return "a";
        case PLATFORM_KEY_B: return "b";
        case PLATFORM_KEY_C: return "c";
        case PLATFORM_KEY_D: return "d";
        case PLATFORM_KEY_E: return "e";
        case PLATFORM_KEY_F: return "f";
        case PLATFORM_KEY_G: return "g";
        case PLATFORM_KEY_H: return "h";
        case PLATFORM_KEY_I: return "i";
        case PLATFORM_KEY_J: return "j";
        case PLATFORM_KEY_K: return "k";
        case PLATFORM_KEY_L: return "l";
        case PLATFORM_KEY_M: return "m";
        case PLATFORM_KEY_N: return "n";
        case PLATFORM_KEY_O: return "o";
        case PLATFORM_KEY_P: return "p";
        case PLATFORM_KEY_Q: return "q";
        case PLATFORM_KEY_R: return "r";
        case PLATFORM_KEY_S: return "s";
        case PLATFORM_KEY_T: return "t";
        case PLATFORM_KEY_U: return "u";
        case PLATFORM_KEY_V: return "v";
        case PLATFORM_KEY_W: return "w";
        case PLATFORM_KEY_X: return "x";
        case PLATFORM_KEY_Y: return "y";
        case PLATFORM_KEY_Z: return "z";
        case PLATFORM_KEY_SPACE: return "space";
        case PLATFORM_KEY_ENTER: return "enter";
        case PLATFORM_KEY_ESCAPE: return "escape";
        case PLATFORM_KEY_NONE: return "none";
        default:
            assert(FALSE && "unhandled platform key");
            return "none";
    }
    return "none";
}

int
main(void)
{
    Platform_Window *window = platform_window_create("mirai playground", 1280, 720);

    // game loop
    b8 running = TRUE;
    while (running)
    {
        // event loop
        while (platform_window_poll(window))
        {
            switch (window->last_event.type)
            {
                case PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS:
                {
                    switch (window->last_event.mouse_button_press.button)
                    {
                        case PLATFORM_MOUSE_BUTTON_LEFT:
                            printf("left mouse pressed\n");
                            break;
                        case PLATFORM_MOUSE_BUTTON_RIGHT:
                            printf("right mouse pressed\n");
                            break;
                        case PLATFORM_MOUSE_BUTTON_MIDDLE:
                            printf("middle mouse pressed\n");
                            break;
                        case PLATFORM_MOUSE_BUTTON_NONE:
                        default:
                            // do nothing
                            break;
                    }
                    break;
                }
                case PLATFORM_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_DOWN:
                    printf("mouse wheel scroll down\n");
                    break;
                case PLATFORM_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_UP:
                    printf("mouse wheel scroll up\n");
                    break;
                case PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE:
                {
                    switch (window->last_event.mouse_button_release.button)
                    {
                        case PLATFORM_MOUSE_BUTTON_LEFT:
                            printf("left mouse released\n");
                            break;
                        case PLATFORM_MOUSE_BUTTON_RIGHT:
                            printf("right mouse released\n");
                            break;
                        case PLATFORM_MOUSE_BUTTON_MIDDLE:
                            printf("middle mouse released\n");
                            break;
                        case PLATFORM_MOUSE_BUTTON_NONE:
                        default:
                            // do nothing
                            break;
                    }
                    break;
                }
                case PLATFORM_WINDOW_EVENT_TYPE_MOUSE_MOVE:
                    printf("mouse move: %d, %d\n",
                        window->last_event.mouse_move.x,
                        window->last_event.mouse_move.y);
                    break;
                case PLATFORM_WINDOW_EVENT_TYPE_KEY_PRESS:
                    printf("key %s press\n",
                        platform_key_str(window->last_event.key_press.key));
                    break;
                case PLATFORM_WINDOW_EVENT_TYPE_KEY_RELEASE:
                    printf("key %s released\n",
                        platform_key_str(window->last_event.key_release.key));
                    break;
                case PLATFORM_WINDOW_EVENT_TYPE_WINDOW_RESIZE:
                    printf("window resize: %d, %d\n",
                        window->last_event.window_resize.width,
                        window->last_event.window_resize.height);
                    break;
                case PLATFORM_WINDOW_EVENT_TYPE_WINDOW_CLOSE:
                    running = FALSE;
                    break;
                case PLATFORM_WINDOW_EVENT_TYPE_NONE:
                    // do nothing
                    break;
                default:
                    assert(FALSE && "unhandled platform window event");
                    break;
            }
        }
    }

    platform_window_destroy(window);

    return 0;
}