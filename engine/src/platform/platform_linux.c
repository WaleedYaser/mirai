#include "platform/platfom.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

#if defined(MIRAI_PLATFORM_LINUX)

typedef struct _Platform_Window_Internal {
    Platform_Window window;

    xcb_connection_t *connection;
    xcb_window_t handle;
} _Platform_Window_Internal;

Platform_Window *
platform_window_create(Platform_Window_Desc desc)
{
    _Platform_Window_Internal *self =
        (_Platform_Window_Internal *)malloc(sizeof(_Platform_Window_Internal));
    memset(self, 0, sizeof(*self));

    self->window.title = desc.title;
    self->window.x = desc.x;
    self->window.y = desc.y;
    self->window.width = desc.width;
    self->window.height = desc.height;

    self->connection = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(self->connection))
    {
        assert(FALSE && "failed to connect to X server via XCB");
        free(self);
        return NULL;
    }

    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(self->connection)).data;

    self->handle = xcb_generate_id(self->connection);
    u32 mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    u32 values[] = {
        screen->black_pixel,
        XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
        XCB_EVENT_MASK_EXPOSURE
    };

    xcb_create_window(
        self->connection,
        XCB_COPY_FROM_PARENT,
        self->handle,
        screen->root,
        desc.x, desc.y,
        desc.width, desc.height,
        10,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        mask, values
    );

    xcb_map_window(self->connection, self->handle);

    xcb_change_property(
        self->connection,
        XCB_PROP_MODE_REPLACE,
        self->handle,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(desc.title),
        desc.title
    );

    xcb_flush(self->connection);

    return &self->window;
}

void
platform_window_destroy(Platform_Window *window)
{
    _Platform_Window_Internal *self = (_Platform_Window_Internal *)window;

    xcb_destroy_window(self->connection, self->handle);
    xcb_disconnect(self->connection);
    free(window);
    window = NULL;
}

b8
platform_window_poll(Platform_Window *window)
{
    _Platform_Window_Internal *self = (_Platform_Window_Internal *)window;
    memset(&self->window.last_event, 0, sizeof(self->window.last_event));

    // TODO: use xcb_poll_for_event instead
    xcb_generic_event_t *event = xcb_wait_for_event(self->connection);
    if (event == NULL)
        return FALSE;

    switch (event->response_type & ~0x80)
    {
        case XCB_BUTTON_PRESS:
            self->window.last_event.type = PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS;
            break;
        case XCB_BUTTON_RELEASE:
            self->window.last_event.type = PLATFORM_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE;
            break;
        case XCB_MOTION_NOTIFY:
            self->window.last_event.type = PLATFORM_WINDOW_EVENT_TYPE_MOUSE_MOVE;
            break;
        case XCB_KEY_PRESS:
            self->window.last_event.type = PLATFORM_WINDOW_EVENT_TYPE_KEY_PRESS;
            break;
        case XCB_KEY_RELEASE:
            self->window.last_event.type = PLATFORM_WINDOW_EVENT_TYPE_KEY_RELEASE;
            break;
        case XCB_CLIENT_MESSAGE:
        {
            // TODO: handle closing window
            break;
        }
        default:
            // assert(FALSE && "unhandled platform window event");
            break;
    }
    free(event);

    return TRUE;
}

#endif