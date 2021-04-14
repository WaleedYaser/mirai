#include <platform/platfom.h>

#include <stdio.h>
#include <assert.h>

int
main(void)
{
    Platform_Window *window = platform_window_create((Platform_Window_Desc){
        .title = "mirai playground",
        // FIXME: window position doesn't look right
        .x = 200,
        .y = 200,
        .width = 1280,
        .height = 720
    });

    // TODO: add game loop
    while (platform_window_poll(window))
    {
        switch (window->last_event.type)
        {
            case PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS:
                printf("mouse press\n");
                break;
            case PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE:
                printf("mouse release\n");
                break;
            case PLATFORM_WINDOW_EVENT_TYPE_MOUSE_MOVE:
                printf("mouse move\n");
                break;
            case PLATFORM_WINDOW_EVENT_TYPE_KEY_PRESS:
                printf("key press\n");
                break;
            case PLATFORM_WINDOW_EVENT_TYPE_KEY_RELEASE:
                printf("key release\n");
                break;
            case PLATFORM_WINDOW_EVENT_TYPE_NONE:
                // ignone
                break;
            default:
                assert(FALSE && "unhandled platform window event");
                break;
        }
    }

    platform_window_destroy(window);

    return 0;
}