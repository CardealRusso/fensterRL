#ifndef FENSTERRL_WINDOWS_H
#define FENSTERRL_WINDOWS_H
#define WIN32_LEAN_AND_MEAN  // This disables sockets

#define Rectangle Gambiarra_Rectangle
#include "windows.h"
#undef Rectangle

static HWND g_hwnd;

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
case WM_KEYDOWN: {
    int scancode = (lParam >> 16) & 0xFF;
    fenster.holdKeys[scancode] = true;
    fenster.pressedKeys[scancode] = true;
    break;
}

case WM_KEYUP: {
    int scancode = (lParam >> 16) & 0xFF;
    fenster.holdKeys[scancode] = false;
    break;
}
case WM_LBUTTONDOWN:
    fenster.mouseButtonsPressed[0] = fenster.mouseButtonsHold[0] = true;
    return 0;
case WM_LBUTTONUP:
    fenster.mouseButtonsHold[0] = false;
    return 0;
case WM_RBUTTONDOWN:
    fenster.mouseButtonsPressed[1] = fenster.mouseButtonsHold[1] = true;
    return 0;
case WM_RBUTTONUP:
    fenster.mouseButtonsHold[1] = false;
    return 0;
case WM_MBUTTONDOWN:
    fenster.mouseButtonsPressed[2] = fenster.mouseButtonsHold[2] = true;
    return 0;
case WM_MBUTTONUP:
    fenster.mouseButtonsHold[2] = false;
    return 0;
case WM_MOUSEWHEEL:
    if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
        fenster.mouseButtonsPressed[3] = true;
    } else {
        fenster.mouseButtonsPressed[4] = true;
    }
    return 0;
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
      fenster.mousePosition[1] = HIWORD(lParam);
      fenster.mousePosition[0] = LOWORD(lParam);
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

  DWORD windowStyle = fenster.isResizable ? WS_OVERLAPPEDWINDOW : (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME);

  g_hwnd = CreateWindowEx(
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
  GetWindowRect(g_hwnd, &rect);
  fenster.windowPosX = rect.left;
  fenster.windowPosY = rect.top;

  fenster.isFocused = (GetForegroundWindow() == g_hwnd);

  SetWindowLongPtr(g_hwnd, GWLP_USERDATA, (LONG_PTR)&fenster);
  ShowWindow(g_hwnd, SW_SHOW);
  UpdateWindow(g_hwnd);
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

static void RenderBitmap(Rectangle rect) {
    int x0 = rect.x < 0 ? 0 : rect.x;
    int y0 = rect.y < 0 ? 0 : rect.y;
    int x1 = rect.x + rect.width > fenster.width ? fenster.width : rect.x + rect.width;
    int y1 = rect.y + rect.height > fenster.height ? fenster.height : rect.y + rect.height;
    int width = x1 - x0;
    int height = y1 - y0;
    
    if (width <= 0 || height <= 0) {
        return;
    }

    HDC hdc = GetDC(g_hwnd);
    BITMAPINFO bi = {0};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = fenster.width;
    bi.bmiHeader.biHeight = -fenster.height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    int sourceY = fenster.height - (rect.y + rect.height);

    StretchDIBits(
        hdc,                   // Destination device context
        rect.x, rect.y,        // Destination x, y
        rect.width, rect.height, // Destination width, height
        rect.x, sourceY,       // Ajustado: Source x, y
        rect.width, rect.height, // Source width, height
        fenster.buffer,        // Source buffer
        &bi,                   // Bitmap info
        DIB_RGB_COLORS,        // Usage
        SRCCOPY                // Raster operation
    );

    ReleaseDC(g_hwnd, hdc);
}


static void PlatformRenderFrame(void) {
  Rectangle fullRect = {0, 0, fenster.width, fenster.height};
  RenderBitmap(fullRect);
}

static void PlatformRenderFrameRec(Rectangle rect) {
  RenderBitmap(rect);
}

static void PlatformCloseWindow(void) {
  DestroyWindow(g_hwnd);
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
  SetWindowText(g_hwnd, title);
}

static void PlatformSetWindowPosition(int x, int y) {
  SetWindowPos(g_hwnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

static void PlatformSetWindowSize(int width, int height) {
  SetWindowPos(g_hwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

static void PlatformSetWindowFocused(void) {
  SetForegroundWindow(g_hwnd);
}

static void PlatformToggleFullscreen(void) {
  fenster.isFullScreen = !fenster.isFullScreen;

  DWORD dwStyle = GetWindowLong(g_hwnd, GWL_STYLE);

  if (fenster.isFullScreen) {
    GetWindowPlacement(g_hwnd, &g_wpPrev);
    SetWindowLong(g_hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfo(MonitorFromWindow(g_hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);

    SetWindowPos(g_hwnd, HWND_TOP, 
      mi.rcMonitor.left, mi.rcMonitor.top,
      mi.rcMonitor.right - mi.rcMonitor.left,
      mi.rcMonitor.bottom - mi.rcMonitor.top,
      SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  } else {
    SetWindowLong(g_hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
    SetWindowPlacement(g_hwnd, &g_wpPrev);
    SetWindowPos(g_hwnd, NULL, 0, 0, 0, 0, 
      SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | 
      SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
}
#endif // FENSTERRL_WINDOWS_H
