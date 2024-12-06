#ifndef FENSTERRL_WINDOWS_H
#define FENSTERRL_WINDOWS_H
#include <windows.h>

typedef struct {
  HWND hwnd;
  HBITMAP bitmap;
  BITMAPINFO bitmapInfo;
} PlatformData;

static WINDOWPLACEMENT g_wpPrev = { 
    sizeof(WINDOWPLACEMENT), // length
    0,                      // flags
    SW_NORMAL,             // showCmd
    {0, 0},                // ptMinPosition
    {0, 0},                // ptMaxPosition
    {0, 0, 0, 0}          // rcNormalPosition
};

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_CLOSE:
      fenster.hasCloseRequest = true;
      return 0;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    case WM_SIZE: {
      if (!fenster.isResizable) return 0;
      
      int newWidth = LOWORD(lParam);
      int newHeight = HIWORD(lParam);
      
      if (newWidth > 0 && newHeight > 0) {
        fenster.hasResized = true;
        fenster.buffer = realloc(fenster.buffer, newWidth * newHeight * sizeof(uint32_t));
        fenster.width = newWidth;
        fenster.height = newHeight;
        
        InvalidateRect(hwnd, NULL, TRUE);
      }
      return 0;
    }

    case WM_MOVE: {
      fenster.windowPosX = (int)(short)LOWORD(lParam);
      fenster.windowPosY = (int)(short)HIWORD(lParam);
      return 0;
    }

    case WM_DISPLAYCHANGE: {
      fenster.screenWidth = (int)LOWORD(lParam);
      fenster.screenHeight = (int)HIWORD(lParam);
      return 0;
    }

    case WM_SETFOCUS:
      fenster.isFocused = true;
      return 0;

    case WM_KILLFOCUS:
      fenster.isFocused = false;
      return 0;

    case WM_MOUSEMOVE:
      fenster.mousePosition[0] = HIWORD(lParam);
      fenster.mousePosition[1] = LOWORD(lParam);
      return 0;

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

  fenster.screenWidth = GetSystemMetrics(SM_CXSCREEN);
  fenster.screenHeight = GetSystemMetrics(SM_CYSCREEN);

  RECT rect;
  GetWindowRect(platform->hwnd, &rect);
  fenster.windowPosX = rect.left;
  fenster.windowPosY = rect.top;

  fenster.isFocused = (GetForegroundWindow() == platform->hwnd);

  SetWindowLongPtr(platform->hwnd, GWLP_USERDATA, (LONG_PTR)&fenster);
  ShowWindow(platform->hwnd, SW_SHOW);
  UpdateWindow(platform->hwnd);
}

static void PlatformPollInputEvents(void) {
  MSG msg;
  fenster.hasCloseRequest = false;
  fenster.hasResized = false;

  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      fenster.hasCloseRequest = true;
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

static void PlatformSleep(int64_t microseconds) {
  Sleep(microseconds);
}

static int64_t PlatformGetTime(void) {
  LARGE_INTEGER freq, count;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&count);
  return (int64_t)(count.QuadPart * 1000.0 / freq.QuadPart);
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

static void PlatformToggleFullscreen(void) {
  PlatformData* platform = (PlatformData*)fenster.platformData;
  fenster.isFullScreen = !fenster.isFullScreen;

  DWORD dwStyle = GetWindowLong(platform->hwnd, GWL_STYLE);

  if (fenster.isFullScreen) {
    GetWindowPlacement(platform->hwnd, &g_wpPrev);
    SetWindowLong(platform->hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfo(MonitorFromWindow(platform->hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);

    SetWindowPos(platform->hwnd, HWND_TOP, 
      mi.rcMonitor.left, mi.rcMonitor.top,
      mi.rcMonitor.right - mi.rcMonitor.left,
      mi.rcMonitor.bottom - mi.rcMonitor.top,
      SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  } else {
    SetWindowLong(platform->hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
    SetWindowPlacement(platform->hwnd, &g_wpPrev);
    SetWindowPos(platform->hwnd, NULL, 0, 0, 0, 0, 
      SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | 
      SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
}
#endif // FENSTERRL_WINDOWS_H
