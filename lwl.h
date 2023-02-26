#ifndef LWL_H
#define LWL_H

#ifdef __linux__
#define PLATFORM_LINUX

#include <X11/Xlib.h>
#include <assert.h>
int x11_black_color;
int x11_white_color;
Window x11_window;
Display* x11_dpy;
GC x11_gc;
int x11_wWidth;
int x11_wHeight;

void x11_init_window(int width, int height, const char* title)
{
    x11_wWidth = width;
    x11_wHeight = height;
    x11_dpy = XOpenDisplay(NULL);
    assert(x11_dpy);
    x11_black_color = BlackPixel(x11_dpy, DefaultScreen(x11_dpy));
    x11_white_color = WhitePixel(x11_dpy, DefaultScreen(x11_dpy));
    x11_window = XCreateSimpleWindow(x11_dpy, DefaultRootWindow(x11_dpy), 0, 0, 
				     width, height, 0, x11_black_color, x11_white_color);
    XStoreName(x11_dpy, x11_window, title);

    Atom del_window = XInternAtom(x11_dpy, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(x11_dpy, x11_window, &del_window, 1);

    XSelectInput(x11_dpy, x11_window, StructureNotifyMask | GraphicsExpose | ConfigureNotify);
    XMapWindow(x11_dpy, x11_window);

    x11_gc = XCreateGC(x11_dpy, x11_window, 0, NULL);
    XSetBackground(x11_dpy, x11_gc, x11_black_color);
    XFlush(x11_dpy);
}

void x11_clear_window()
{
    XFillRectangle(x11_dpy, x11_window, x11_gc, 0, 0, x11_wWidth, x11_wHeight);
}

void x11_swap_buffers()
{
    XFlush(x11_dpy);
}

int x11_update()
{
    for(;;) {
	    XEvent e;
	    XNextEvent(x11_dpy, &e);
	    if (e.type == MapNotify)
		    return 1;
        else if (e.type == ClientMessage)
            return 0;
        else if (e.type == GraphicsExpose)
        {
            XClearWindow(x11_dpy, x11_window);
            return 1;
        }
        else if (e.type == ConfigureNotify)
            x11_wWidth = e.xconfigure.width;
            x11_wHeight = e.xconfigure.height;
            return 1;
    }
}

void x11_terminate_window()
{
    XUnmapWindow(x11_dpy, x11_window);
    XDestroyWindow(x11_dpy, x11_window);
    XCloseDisplay(x11_dpy);
}
#endif

#ifdef __APPLE__
#error Mac OS not supported!
#endif

#ifdef _WIN32
#define PLATFORM_WIN32
#define UNICODE
#include <windows.h>
HINSTANCE win32_hInstance;
#define WIN32_CLASS_NAME L"HeaderWindowLibraryWin32Class"
WNDCLASS win32_wc = { };
HWND win32_hwnd;

LRESULT CALLBACK WIN32_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB(0,0,0)));
        EndPaint(hwnd, &ps);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void win32_init_window(int width, int height, const char* title)
{
    win32_hInstance = GetModuleHandle(0);
    win32_wc.lpfnWndProc = WIN32_WindowProc;
    win32_wc.hInstance = win32_hInstance;
    win32_wc.lpszClassName = WIN32_CLASS_NAME;
    RegisterClass(&win32_wc);
    wchar_t* window_title = (wchar_t*)malloc(sizeof(wchar_t)*4096);
    MultiByteToWideChar(CP_ACP, 0, title, -1, window_title, 4096);
    win32_hwnd = CreateWindowEx(
        0, WIN32_CLASS_NAME,
        window_title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL,
        NULL,
        win32_hInstance,
        NULL
    );
    free(window_title);
    if (win32_hwnd == NULL) {
        perror("Error creating win32 window: ");
        exit(-1);
    }
    ShowWindow(win32_hwnd, SW_SHOW);
}

void win32_clear_window()
{
    UpdateWindow(win32_hwnd);
}

MSG win32_msg = { };
int win32_update()
{
    if (GetMessage(&win32_msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&win32_msg);
        DispatchMessage(&win32_msg);
        return 1;
    } else
    {
        return 0;
    }
}

void win32_terminate_window()
{
    DestroyWindow(win32_hwnd);
    UnregisterClass(WIN32_CLASS_NAME, win32_hInstance);
}
#endif

#ifdef PLATFORM_LINUX
void lwlInitWindow(int width, int height, const char* title)
{
    x11_init_window(width, height, title);
}
void lwlClearWindow()
{
    x11_clear_window();
    x11_swap_buffers();
}
int lwlWindowIsOpen()
{
    return x11_update();
}
void lwlTerminateWindow()
{
    x11_terminate_window();
}
#endif
#ifdef PLATFORM_WIN32
void lwlInitWindow(int width, int height, const char* title)
{
    win32_init_window(width, height, title);
}
void lwlClearWindow()
{
    win32_clear_window();
}
int lwlWindowIsOpen()
{
    return win32_update();
}
void lwlTerminateWindow()
{
    win32_terminate_window();
}
#endif

#endif