#ifndef FENSTERRL_H
#define FENSTERRL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "fensterRL_macros.h"
#include "fensterRL_struct.h"

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
  fenster.isFullScreen = false;
  PlatformInitWindow(title);
}

void rl_PollInputEvents(void) {
  memset(fenster.mouseButtonsPressed, 0, sizeof(fenster.mouseButtonsPressed));
  memset(fenster.pressedKeys, 0, sizeof(fenster.pressedKeys));
  PlatformPollInputEvents();
}

void rl_WindowSync(int fps) {
  if (fps > 0) {
    int64_t targetFrameTime = 1000 / fps;
    int64_t currentTime = PlatformGetTime();
    int64_t elapsed = currentTime - fenster.lastFrameTime;

    if (elapsed < targetFrameTime) {
      PlatformSleep(targetFrameTime - elapsed);
      currentTime = PlatformGetTime();
    }

    if (elapsed > 0) {
      fenster.fps = 1000 / elapsed;
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

void rl_SetWindowPositionV(Vector2 position) {
  rl_SetWindowPosition(position.x, position.y);
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

Vector2 rl_GetWindowPositionV(void) {
  return (Vector2){ rl_GetWindowPositionX(), rl_GetWindowPositionY() };
}

int rl_GetMouseX(void) {
  return fenster.mousePosition[0];
}

int rl_GetMouseY(void) {
  return fenster.mousePosition[1];
}

Vector2 rl_GetMouseV(void) {
  return (Vector2){ rl_GetMouseX(), rl_GetMouseY() };
}

void rl_WaitTime(double seconds) {
  if (seconds > 0) {
    int64_t ms = (int64_t)(seconds * 1000.0);
    PlatformSleep(ms);
  }
}

int rl_GetFPS(void) {
  return fenster.fps;
}

void rl_ToggleFullscreen(void) {
  PlatformToggleFullscreen();
}

bool rl_IsWindowFullscreen(void) {
  return fenster.isFullScreen;
}

bool rl_IsMouseButtonPressed(int button) {
  return fenster.mouseButtonsPressed[button];
}

bool rl_IsMouseButtonDown(int button) {
  return fenster.mouseButtonsHold[button];
}

bool rl_IsKeyDown(int keycode) {
  return fenster.holdKeys[keycode];
}

bool rl_IsKeyPressed(int keycode) {
  return fenster.pressedKeys[keycode];
}

#ifdef USE_SHAPES
#include "fensterRL_shapes.h"
#endif

#ifdef USE_FONTS
#include "fensterRL_fonts.h"
#endif

#endif // FENSTERRL_H
