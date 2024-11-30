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
  int targetFPS;
  double lastFrameTime;
  bool isResizable;
  void* platformData;
} fenster = {0};

#ifdef __linux__
  #include "plataforms/fensterRL_Linux.h"
#elif defined(_WIN32)
  #include "plataforms/fensterRL_Windows.h"
#elif defined(__APPLE__)
  #include "plataforms/fensterRL_Mac.h"
#endif

void rl_SetConfigFlags(int flags) {
  fenster.isResizable = !!(flags & FLAG_WINDOW_RESIZABLE);
}

void rl_InitWindow(int width, int height, const char* title) {
  fenster.width = width;
  fenster.height = height;
  fenster.buffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
  fenster.targetFPS = 60;
  fenster.lastFrameTime = 0;
  PlatformInitWindow(title);
}

void rl_SetTargetFPS(int fps) {
  fenster.targetFPS = fps;
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

bool rl_WindowShouldClose(void) {
  if (fenster.targetFPS > 0) {
    double targetFrameTime = 1.0 / fenster.targetFPS;
    double currentTime = PlatformGetTime();
    double elapsed = currentTime - fenster.lastFrameTime;

    if (elapsed < targetFrameTime) {
      PlatformSleep((targetFrameTime - elapsed) * 1000000);
      currentTime = PlatformGetTime();
    }

    fenster.lastFrameTime = currentTime;
  }

  return PlatformWindowShouldClose();
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
  return PlatformGetScreenWidth();
}

int rl_GetScreenHeight(void) {
  return PlatformGetScreenHeight();
}

bool rl_IsWindowFocused(void) {
  return PlataformIsWindowFocused();
}
#endif // FENSTERRL_H
