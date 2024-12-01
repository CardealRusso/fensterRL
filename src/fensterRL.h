#ifndef FENSTERRL_H
#define FENSTERRL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#define FLAG_WINDOW_RESIZABLE (1 << 0)

static struct {
  int width;
  int height;
  uint32_t* buffer;
  double lastFrameTime;
  bool isResizable, hasResized, isFocused, hasCloseRequest;
  void* platformData;
  int screenWidth, screenHeight;
  int windowPosX, windowPosY;
} fenster = {0};

#ifdef __linux__
  #include "platforms/fensterRL_Linux.h"
#elif defined(_WIN32)
  #include "platforms/fensterRL_Windows.h"
#elif defined(__APPLE__)
  #include "platforms/fensterRL_Mac.h"
#endif

void rl_SetConfigFlags(int flags) {
  fenster.isResizable = !!(flags & FLAG_WINDOW_RESIZABLE);
}

void rl_InitWindow(int width, int height, const char* title) {
  fenster.width = width;
  fenster.height = height;
  fenster.buffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
  fenster.lastFrameTime = 0;
  PlatformInitWindow(title);
}

void rl_ClearBackground(uint32_t color) {
  for (int i = 0; i < fenster.width * fenster.height; i++) {
    fenster.buffer[i] = color;
  }
}

void rl_SetPixel(int x, int y, uint32_t color) {
  if (x >= 0 && x < fenster.width && y >= 0 && y < fenster.height) {
    fenster.buffer[y * fenster.width + x] = color;
  }
}

static inline void rl_SetPixelUnsafe(int x, int y, uint32_t color) {
    fenster.buffer[y * fenster.width + x] = color;
}

uint32_t rl_GetPixel(int x, int y) {
  if (x >= 0 && x < fenster.width && y >= 0 && y < fenster.height) {
    return fenster.buffer[y * fenster.width + x];
  }
  return 0; // Return 0 (black) if the coordinates are out of bounds
}

static inline uint32_t rl_GetPixelUnsafe(int x, int y) {
  return fenster.buffer[y * fenster.width + x];
}

void rl_WindowEventLoop(void) {
  PlatformWindowEventLoop();
}

void rl_WindowSync(int fps) {
  if (fps > 0) {
    double targetFrameTime = 1.0 / fps;
    double currentTime = PlatformGetTime();
    double elapsed = currentTime - fenster.lastFrameTime;

    if (elapsed < targetFrameTime) {
      PlatformSleep((targetFrameTime - elapsed) * 1000000);
      currentTime = PlatformGetTime();
    }

    fenster.lastFrameTime = currentTime;
  }
}

void rl_RenderFrame(void){
  PlatformRenderFrame();
}

void rl_CloseWindow(void) {
  PlatformCloseWindow();
  free(fenster.buffer);
  fenster.buffer = NULL;
}

int rl_GetWindowWidth(void) {
  return fenster.width;
}

int rl_GetWindowHeight(void) {
  return fenster.height;
}

int rl_GetScreenWidth(void) {
  return fenster.screenWidth;
}

int rl_GetScreenHeight(void) {
  return fenster.screenHeight;
}

bool rl_IsWindowFocused(void) {
  return fenster.isFocused;
}

bool rl_IsCloseRequested(void) {
  return fenster.hasCloseRequest;
}

void rl_SetWindowTitle(const char* title) {
  PlatformSetWindowTitle(title);
}

void rl_SetWindowPosition(int x, int y) {
  PlatformSetWindowPosition(x, y);
}

void rl_SetWindowSize(int width, int height) {
  PlatformSetWindowSize(width, height);
}

void rl_SetWindowFocused(void) {
  PlatformSetWindowFocused();
}

bool rl_IsWindowResized(void) {
  return fenster.hasResized;
}

int rl_GetWindowPositionX(void) {
  return fenster.windowPosX;
}

int rl_GetWindowPositionY(void) {
  return fenster.windowPosY;
}
#endif // FENSTERRL_H
