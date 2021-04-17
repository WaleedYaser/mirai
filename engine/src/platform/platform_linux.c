#include "platform/platfom.h"
#if defined(MIRAI_PLATFORM_LINUX)

#include "core/asserts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ulsefull resource for xcb
 * https://www.x.org/releases/current/doc/libxcb/tutorial/index.html
 */
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

typedef struct _MP_Window_Internal {
    MP_Window window;

    xcb_connection_t *connection;
    xcb_window_t handle;
    xcb_atom_t wm_delete_win;
    xcb_key_symbols_t *key_symbols;
} _MP_Window_Internal;

static MP_KEY
_mp_key_from_xcb_keycode(_MP_Window_Internal *self, xcb_keycode_t keycode)
{
    xcb_keysym_t key = xcb_key_symbols_get_keysym(self->key_symbols, keycode, 0);
    switch (key)
    {
        // numbers
        case '0': return MP_KEY_0;
        case '1': return MP_KEY_1;
        case '2': return MP_KEY_2;
        case '3': return MP_KEY_3;
        case '4': return MP_KEY_4;
        case '5': return MP_KEY_5;
        case '6': return MP_KEY_6;
        case '7': return MP_KEY_7;
        case '8': return MP_KEY_8;
        case '9': return MP_KEY_9;
        // letters
        case 'a': return MP_KEY_A;
        case 'b': return MP_KEY_B;
        case 'c': return MP_KEY_C;
        case 'd': return MP_KEY_D;
        case 'e': return MP_KEY_E;
        case 'f': return MP_KEY_F;
        case 'g': return MP_KEY_G;
        case 'h': return MP_KEY_H;
        case 'i': return MP_KEY_I;
        case 'j': return MP_KEY_J;
        case 'k': return MP_KEY_K;
        case 'l': return MP_KEY_L;
        case 'm': return MP_KEY_M;
        case 'n': return MP_KEY_N;
        case 'o': return MP_KEY_O;
        case 'p': return MP_KEY_P;
        case 'q': return MP_KEY_Q;
        case 'r': return MP_KEY_R;
        case 's': return MP_KEY_S;
        case 't': return MP_KEY_T;
        case 'u': return MP_KEY_U;
        case 'v': return MP_KEY_V;
        case 'w': return MP_KEY_W;
        case 'x': return MP_KEY_X;
        case 'y': return MP_KEY_Y;
        case 'z': return MP_KEY_Z;
        // special keys
        case XK_Return: return MP_KEY_ENTER;
        case XK_Escape: return MP_KEY_ESCAPE;
        case XK_space: return MP_KEY_SPACE;
        default: return MP_KEY_NONE;
    }
    return MP_KEY_NONE;
}

static const char *
_mp_color_to_str(MP_COLOR color)
{
    switch (color)
    {
        case MP_COLOR_FG_GRAY: return "1;30";
        case MP_COLOR_FG_BLUE: return "1;34";
        case MP_COLOR_FG_GREEN: return "1;32";
        case MP_COLOR_FG_YELLOW: return "1;33";
        case MP_COLOR_FG_RED: return "1;31";
        case MP_COLOR_BG_RED: return "0;41";
        default: MC_ASSERT_MSG_DEBUG(FALSE, "unexpected MP_COLOR"); return "";
    }
    return "";
}

MP_Window *
mp_window_create(const char *title, i32 width, i32 height)
{
    _MP_Window_Internal *self = (_MP_Window_Internal *)malloc(sizeof(_MP_Window_Internal));
    memset(self, 0, sizeof(*self));

    self->window.title = title;
    self->window.width = width;
    self->window.height = height;

    self->connection = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(self->connection))
    {
        MC_ASSERT_MSG(FALSE, "failed to connect to X server via XCB");
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
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE
    };

    xcb_create_window(
        self->connection,
        XCB_COPY_FROM_PARENT,
        self->handle,
        screen->root,
        0, 0,
        width, height,
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
        strnlen(title, 512),
        title
    );

    xcb_intern_atom_cookie_t wm_delete_cookie =
        xcb_intern_atom(self->connection, 0, strnlen("WM_DELETE_WINDOW", 512), "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wm_protocols_cookie =
        xcb_intern_atom(self->connection, 0, strnlen("WM_PROTOCOLS", 512), "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *wm_delete_reply =
        xcb_intern_atom_reply(self->connection, wm_delete_cookie, NULL);
    xcb_intern_atom_reply_t *wm_protocols_reply =
        xcb_intern_atom_reply(self->connection, wm_protocols_cookie, NULL);
    self->wm_delete_win = wm_delete_reply->atom;

    xcb_change_property(
        self->connection,
        XCB_PROP_MODE_REPLACE,
        self->handle,
        wm_protocols_reply->atom,
        4,
        32,
        1,
        &wm_delete_reply->atom
    );

    if (xcb_flush(self->connection) <= 0)
    {
        MC_ASSERT_MSG(FALSE, "failed to flush connection");
        free(self);
        return NULL;
    }

    self->key_symbols = xcb_key_symbols_alloc(self->connection);
    if (self->key_symbols == NULL)
    {
        MC_ASSERT_MSG(FALSE, "failed to allocate key sombols");
        free(self);
        return NULL;
    }

    return &self->window;
}

void
mp_window_destroy(MP_Window *window)
{
    _MP_Window_Internal *self = (_MP_Window_Internal *)window;

    free(self->key_symbols);
    xcb_destroy_window(self->connection, self->handle);
    xcb_disconnect(self->connection);
    free(self);
}

b8
mp_window_poll(MP_Window *window)
{
    _MP_Window_Internal *self = (_MP_Window_Internal *)window;
    memset(&self->window.last_event, 0, sizeof(self->window.last_event));

    xcb_generic_event_t *event = xcb_poll_for_event(self->connection);
    if (event == NULL)
        return FALSE;

    switch (event->response_type & ~0x80)
    {
        case XCB_BUTTON_PRESS:
        {
            xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;
            switch (bp->detail)
            {
                case 1:
                    self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS;
                    self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_LEFT;
                    break;
                case 2:
                    self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS;
                    self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_MIDDLE;
                    break;
                case 3:
                    self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS;
                    self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_RIGHT;
                    break;
                case 4:
                    self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_UP;
                    break;
                case 5:
                    self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_DOWN;
                    break;
                default:
                    // do nothing
                    break;
            }
            break;
        }
        case XCB_BUTTON_RELEASE:
        {
            xcb_button_release_event_t *br = (xcb_button_release_event_t *)event;
            switch (br->detail)
            {
                case 1:
                    self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE;
                    self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_LEFT;
                    break;
                case 2:
                    self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE;
                    self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_MIDDLE;
                    break;
                case 3:
                    self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE;
                    self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_RIGHT;
                    break;
                case 4:
                case 5:
                default:
                    // do nothing
                    break;
            }
            break;
        }
        case XCB_MOTION_NOTIFY:
        {
            xcb_motion_notify_event_t *mn = (xcb_motion_notify_event_t *)event;
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_MOVE;
            self->window.last_event.mouse_move.x = mn->event_x;
            self->window.last_event.mouse_move.y = mn->event_y;
            break;
        }
        case XCB_KEY_PRESS:
        {
            xcb_key_press_event_t *kp =(xcb_key_press_event_t *)event;
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_KEY_PRESS;
            self->window.last_event.key_press.key = _mp_key_from_xcb_keycode(self, kp->detail);
        }
        case XCB_KEY_RELEASE:
        {
            xcb_key_release_event_t *kr =(xcb_key_release_event_t *)event;
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_KEY_RELEASE;
            self->window.last_event.key_release.key = _mp_key_from_xcb_keycode(self, kr->detail);
            break;
        }
        case XCB_CONFIGURE_NOTIFY:
        {
            xcb_configure_notify_event_t *cn = (xcb_configure_notify_event_t *)event;
            if (cn->width != self->window.width || cn->height != self->window.height)
            {
                self->window.last_event.type = MP_WINDOW_EVENT_TYPE_WINDOW_RESIZE;
                self->window.last_event.window_resize.width = cn->width;
                self->window.last_event.window_resize.height = cn->height;

                self->window.width = cn->width;
                self->window.height = cn->height;
            }
        }
        case XCB_CLIENT_MESSAGE:
        {
            xcb_client_message_event_t *cm = (xcb_client_message_event_t *)event;
            if (cm->data.data32[0] == self->wm_delete_win)
            {
                self->window.last_event.type = MP_WINDOW_EVENT_TYPE_WINDOW_CLOSE;
            }
            break;
        }
        case XCB_MAP_NOTIFY:
        case XCB_MAPPING_NOTIFY:
        case XCB_REPARENT_NOTIFY:
            // do nothing
            break;
        default:
            MC_ASSERT_MSG_DEBUG(FALSE, "unexpected platform window event");
            break;
    }
    free(event);

    return TRUE;
}

void
mp_console_write(const char *message, MP_COLOR color)
{
    printf("\033[%sm%s\033[0m", _mp_color_to_str(color), message);
}

void
mp_console_write_error(const char *message, MP_COLOR color)
{
    printf("\033[%sm%s\033[0m", _mp_color_to_str(color), message);
}

#endif
