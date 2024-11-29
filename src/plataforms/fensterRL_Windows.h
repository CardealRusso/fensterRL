#ifndef FENSTERRL_WINDOWS_H
#define FENSTERRL_WINDOWS_H
#include <windows.h>

typedef struct {
    HWND hwnd;
    HDC memoryDC;
    HBITMAP bitmap;
    BITMAPINFO bitmapInfo;
} PlatformData;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            BITMAPINFO bi = {0};
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = fenster.width;
            bi.bmiHeader.biHeight = -fenster.height;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            
            StretchDIBits(
                hdc, 
                0, 0, 
                fenster.width, fenster.height, 
                0, 0, 
                fenster.width, fenster.height, 
                fenster.buffer, 
                &bi, 
                DIB_RGB_COLORS, 
                SRCCOPY
            );
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_SIZE: {
            if (!fenster.isResizable) return 0;
            
            int newWidth = LOWORD(lParam);
            int newHeight = HIWORD(lParam);
            
            if (newWidth > 0 && newHeight > 0) {
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

static double PlatformGetTime(void) {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return count.QuadPart * 1000.0 / freq.QuadPart / 1000.0;
}

static void PlatformSleep(long microseconds) {
    Sleep(microseconds / 1000);
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

static bool PlatformWindowShouldClose(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    MSG msg;
    
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return true;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    InvalidateRect(platform->hwnd, NULL, FALSE);
    return false;
}

static void PlatformCloseWindow(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    
    DeleteObject(platform->bitmap);
    DeleteDC(platform->memoryDC);
    DestroyWindow(platform->hwnd);
    
    free(platform);
    fenster.platformData = NULL;
    fenster.buffer = NULL;
}

#endif // FENSTERRL_WINDOWS_H
