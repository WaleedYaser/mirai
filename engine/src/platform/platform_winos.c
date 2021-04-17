#include "platform/platfom.h"

#if defined(MIRAI_PLATFORM_WINOS)

#include "core/asserts.h"
#include "core/logger.h"

#include <Windows.h>
#include <windowsx.h>
#include <stdlib.h>

typedef struct _MP_Window_Internal {
    MP_Window window;

    HWND handle;
    DWORD style;
} _MP_Window_Internal;

static MP_KEY
_mp_key_from_wparam(WPARAM wparam)
{
    switch (wparam)
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
        case 'A': return MP_KEY_A;
        case 'B': return MP_KEY_B;
        case 'C': return MP_KEY_C;
        case 'D': return MP_KEY_D;
        case 'E': return MP_KEY_E;
        case 'F': return MP_KEY_F;
        case 'G': return MP_KEY_G;
        case 'H': return MP_KEY_H;
        case 'I': return MP_KEY_I;
        case 'J': return MP_KEY_J;
        case 'K': return MP_KEY_K;
        case 'L': return MP_KEY_L;
        case 'M': return MP_KEY_M;
        case 'N': return MP_KEY_N;
        case 'O': return MP_KEY_O;
        case 'P': return MP_KEY_P;
        case 'Q': return MP_KEY_Q;
        case 'R': return MP_KEY_R;
        case 'S': return MP_KEY_S;
        case 'T': return MP_KEY_T;
        case 'U': return MP_KEY_U;
        case 'V': return MP_KEY_V;
        case 'W': return MP_KEY_W;
        case 'X': return MP_KEY_X;
        case 'Y': return MP_KEY_Y;
        case 'Z': return MP_KEY_Z;
        // special keys
        case VK_RETURN: return MP_KEY_ENTER;
        case VK_ESCAPE: return MP_KEY_ESCAPE;
        case VK_SPACE: return MP_KEY_SPACE;
        default: return MP_KEY_NONE;
    }
    return MP_KEY_NONE;
}

LRESULT CALLBACK
_mp_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    _MP_Window_Internal *self = (_MP_Window_Internal *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            SetCapture(hwnd);
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS;
            self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_LEFT;
            break;
        }
        case WM_MBUTTONDOWN:
        {
            SetCapture(hwnd);
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS;
            self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_MIDDLE;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            SetCapture(hwnd);
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_PRESS;
            self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_RIGHT;
            break;
        }
        case WM_LBUTTONUP:
        {
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE;
            self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_LEFT;
            break;
        }
        case WM_MBUTTONUP:
        {
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE;
            self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_MIDDLE;
            break;
        }
        case WM_RBUTTONUP:
        {
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_BUTTON_RELEASE;
            self->window.last_event.mouse_button_press.button = MP_MOUSE_BUTTON_RIGHT;
            break;
        }
        case WM_MOUSEWHEEL:
        {
            if (GET_WHEEL_DELTA_WPARAM(wparam) > 0)
                self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_UP;
            else if (GET_WHEEL_DELTA_WPARAM(wparam) < 0)
                self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_WHEEL_SCROLL_DOWN;
            break;
        }
        case WM_MOUSEMOVE:
        {
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_MOUSE_MOVE;
            self->window.last_event.mouse_move.x = GET_X_LPARAM(lparam);
            self->window.last_event.mouse_move.y = GET_Y_LPARAM(lparam);
            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_KEY_PRESS;
            self->window.last_event.key_press.key = _mp_key_from_wparam(wparam);
            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_KEY_RELEASE;
            self->window.last_event.key_release.key = _mp_key_from_wparam(wparam);
            break;
        }
        case WM_SIZE:
        {
            // TODO: handle resize in realtime
            // TODO: why first time self become NULL?
            if (self)
            {
                self->window.last_event.type = MP_WINDOW_EVENT_TYPE_WINDOW_RESIZE;
                self->window.last_event.window_resize.width = LOWORD(lparam);
                self->window.last_event.window_resize.height = HIWORD(lparam);
                self->window.width = LOWORD(lparam);
                self->window.height = HIWORD(lparam);
            }
            break;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            self->window.last_event.type = MP_WINDOW_EVENT_TYPE_WINDOW_CLOSE;
            return 0;
        }
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

static u8
_mp_color_to_u8(MP_COLOR color)
{
    switch (color)
    {
        case MP_COLOR_FG_GRAY: return 8;
        case MP_COLOR_FG_BLUE: return 1;
        case MP_COLOR_FG_GREEN: return 2;
        case MP_COLOR_FG_YELLOW: return 6;
        case MP_COLOR_FG_RED: return 4;
        case MP_COLOR_BG_RED: return 64;
        default: MC_ASSERT_MSG_DEBUG(FALSE, "unexpected MP_COLOR"); return 0;
    }
    return 0;
}

MP_Window *
mp_window_create(const char *title, i32 width, i32 height)
{
    _MP_Window_Internal *self = (_MP_Window_Internal *)malloc(sizeof(_MP_Window_Internal));
    memset(self, 0, sizeof(*self));

    self->window.title = title;
    self->window.width = width;
    self->window.height = height;
    self->style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = _mp_window_proc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "Mirai_Window_Class";
    if (RegisterClassExA(&wc) == 0)
    {
        MC_ASSERT_MSG(FALSE, "failed to register window class");
        free(self);
        return FALSE;
    }

    RECT wr = {0, 0, self->window.width, self->window.height};
    AdjustWindowRect(&wr, self->style, FALSE);

    self->handle = CreateWindowExA(
        0,
        "Mirai_Window_Class",
        title,
        self->style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        self->window.width, self->window.height,
        NULL,
        NULL,
        NULL,
        NULL
    );
    if (self->handle == NULL)
    {
        MC_ASSERT_MSG(FALSE, "failed to create window");
        free(self);
        return FALSE;
    }

    SetWindowLongPtrA(self->handle, GWLP_USERDATA, (LONG_PTR)self);

    return &self->window;
}

void
mp_window_destroy(MP_Window *window)
{
    _MP_Window_Internal *self = (_MP_Window_Internal *)window;

    if (DestroyWindow(self->handle) == FALSE)
    {
        MC_ASSERT_MSG(FALSE, "failed to destroy window");
    }
    free(self);
}

b8
mp_window_poll(MP_Window *window)
{
    _MP_Window_Internal *self = (_MP_Window_Internal *)window;
    memset(&self->window.last_event, 0, sizeof(self->window.last_event));

    MSG msg = {};
    if (PeekMessageA(&msg, self->handle, 0, 0, PM_REMOVE) == FALSE)
        return FALSE;

    TranslateMessage(&msg);
    DispatchMessage(&msg);

    return TRUE;
}

void
mp_console_write(const char *message, MP_COLOR color)
{
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console_handle, _mp_color_to_u8(color));
    OutputDebugStringA(message);
    u64 length = strnlen(message, 2 * 1024);
    WriteConsoleA(console_handle, message, length, NULL, NULL);
}

void
mp_console_write_error(const char *message, MP_COLOR color)
{
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleTextAttribute(console_handle, _mp_color_to_u8(color));
    OutputDebugStringA(message);
    u64 length = strnlen(message, 2 * 1024);
    WriteConsoleA(console_handle, message, length, NULL, NULL);
}

#endif