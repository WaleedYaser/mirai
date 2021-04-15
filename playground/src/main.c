#include <platform/platfom.h>

#include <stdio.h>
#include <assert.h>

int
main(void)
{
    Platform_Window *window = platform_window_create((Platform_Window_Desc){
        .title = "mirai playground",
        // FIXME: window position doesn't look right
        .x = 500,
        .y = 500,
        .width = 1280,
        .height = 720
    });

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
                    printf("key press\n");
                    break;
                case PLATFORM_WINDOW_EVENT_TYPE_KEY_RELEASE:
                    printf("key release\n");
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