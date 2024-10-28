// fensterRL_Windows.h
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
            PlatformData* platform = (PlatformData*)fenster.platformData;
            
            // Copy from memory DC to window DC
            BitBlt(hdc, 0, 0, fenster.width, fenster.height, 
                   platform->memoryDC, 0, 0, SRCCOPY);
            
            EndPaint(hwnd, &ps);
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
    return count.QuadPart * 1000.0 / freq.QuadPart / 1000.0; // Convert to seconds
}

static void PlatformSleep(long microseconds) {
    Sleep(microseconds / 1000); // Convert to milliseconds
}

static void PlatformInitWindow(const char* title) {
    PlatformData* platform = malloc(sizeof(PlatformData));
    fenster.platformData = platform;
    
    // Register window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "FensterRLClass";
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    RegisterClassEx(&wc);
    
    // Create window with correct client area
    RECT rect = {0, 0, fenster.width, fenster.height};
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE);
    
    platform->hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "FensterRLClass",
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );
    
    // Setup DIB section
    HDC windowDC = GetDC(platform->hwnd);
    platform->memoryDC = CreateCompatibleDC(windowDC);
    
    platform->bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    platform->bitmapInfo.bmiHeader.biWidth = fenster.width;
    platform->bitmapInfo.bmiHeader.biHeight = -fenster.height; // Negative for top-down DIB
    platform->bitmapInfo.bmiHeader.biPlanes = 1;
    platform->bitmapInfo.bmiHeader.biBitCount = 32;
    platform->bitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    platform->bitmap = CreateDIBSection(
        platform->memoryDC,
        &platform->bitmapInfo,
        DIB_RGB_COLORS,
        (void**)&fenster.buffer,
        NULL,
        0
    );
    
    SelectObject(platform->memoryDC, platform->bitmap);
    ReleaseDC(platform->hwnd, windowDC);
    
    // Show window
    ShowWindow(platform->hwnd, SW_NORMAL);
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
    
    // Update the window content
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
    fenster.buffer = NULL;  // Buffer was managed by DIB section
}

#endif // FENSTERRL_WINDOWS_H
