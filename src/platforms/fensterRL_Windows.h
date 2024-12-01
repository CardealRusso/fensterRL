#ifndef FENSTERRL_WINDOWS_H
#define FENSTERRL_WINDOWS_H
#include <windows.h>

typedef struct {
    HWND hwnd;
    HBITMAP bitmap;
    BITMAPINFO bitmapInfo;
} PlatformData;

static bool closeRequested = false;
static bool rl_WindowResized = false;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            closeRequested = true;
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE: {
            if (!fenster.isResizable) return 0;
            
            int newWidth = LOWORD(lParam);
            int newHeight = HIWORD(lParam);
            
            if (newWidth > 0 && newHeight > 0) {
                rl_WindowResized = true;
                fenster.buffer = realloc(fenster.buffer, newWidth * newHeight * sizeof(uint32_t));
                fenster.width = newWidth;
                fenster.height = newHeight;
                
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

static void PlatformInitWindow(const char* title) {
    fenster.buffer = malloc(fenster.width * fenster.height * sizeof(uint32_t));
    
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "FensterRLClass";
    
    RegisterClassEx(&wc);

    PlatformData* platform = malloc(sizeof(PlatformData));
    fenster.platformData = platform;

    DWORD windowStyle = fenster.isResizable ? WS_OVERLAPPEDWINDOW : (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME);

    platform->hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE, 
        "FensterRLClass", 
        title,
        windowStyle, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        fenster.width, 
        fenster.height, 
        NULL, 
        NULL, 
        hInstance, 
        NULL
    );

    SetWindowLongPtr(platform->hwnd, GWLP_USERDATA, (LONG_PTR)&fenster);
    ShowWindow(platform->hwnd, SW_SHOW);
    UpdateWindow(platform->hwnd);
}

static void PlatformWindowEventLoop(void) {
    MSG msg;
    closeRequested = false;
    rl_WindowResized = false;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            closeRequested = true;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

static void PlatformRenderFrame(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    HDC hdc = GetDC(platform->hwnd);

    BITMAPINFO bi = {0};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = fenster.width;
    bi.bmiHeader.biHeight = -fenster.height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(hdc, 
                      0, 0, fenster.width, fenster.height, 
                      0, 0, 
                      0, fenster.height, 
                      fenster.buffer, &bi, DIB_RGB_COLORS);

    ReleaseDC(platform->hwnd, hdc);
}


static void PlatformCloseWindow(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;

    DeleteObject(platform->bitmap);
    DestroyWindow(platform->hwnd);

    free(platform);
    fenster.platformData = NULL;
}

static int PlatformGetScreenWidth(void) {
    return GetSystemMetrics(SM_CXSCREEN);
}

static int PlatformGetScreenHeight(void) {
    return GetSystemMetrics(SM_CYSCREEN);
}

static void PlatformSleep(long microseconds) {
    Sleep(microseconds / 1000);
}

static double PlatformGetTime(void) {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return (double)count.QuadPart / freq.QuadPart;
}

static bool PlatformIsWindowFocused(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    return GetForegroundWindow() == platform->hwnd;
}

static void PlatformSetWindowTitle(const char* title) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    SetWindowText(platform->hwnd, title);
}

static void PlatformSetWindowPosition(int x, int y) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    SetWindowPos(platform->hwnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

static void PlatformSetWindowSize(int width, int height) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    SetWindowPos(platform->hwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

static void PlatformSetWindowFocused(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    SetForegroundWindow(platform->hwnd);
}

static int PlatformGetWindowPositionX(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    RECT rect;
    if (GetWindowRect(platform->hwnd, &rect)) {
        return rect.left;
    }
    return -1;
}

static int PlatformGetWindowPositionY(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    RECT rect;
    if (GetWindowRect(platform->hwnd, &rect)) {
        return rect.top;
    }
    return -1;
}

#endif // FENSTERRL_WINDOWS_H
