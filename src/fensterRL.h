#ifndef FENSTERRL_H
#define FENSTERRL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define FLAG_WINDOW_RESIZABLE (1 << 0)
#define RL_LIGHTGRAY  0xc8c8c8  // Light Gray
#define RL_GRAY       0x828282  // Gray
#define RL_DARKGRAY   0x505050  // Dark Gray
#define RL_YELLOW     0xfdf900  // Yellow
#define RL_GOLD       0xffcb00  // Gold
#define RL_ORANGE     0xffa100  // Orange
#define RL_PINK       0xff6dc2  // Pink
#define RL_RED        0xe62937  // Red
#define RL_MAROON     0xbe2137  // Maroon
#define RL_GREEN      0x00e430  // Green
#define RL_LIME       0x009e2f  // Lime
#define RL_DARKGREEN  0x00752c  // Dark Green
#define RL_SKYBLUE    0x66bfff  // Sky Blue
#define RL_BLUE       0x0079f1  // Blue
#define RL_DARKBLUE   0x0052ac  // Dark Blue
#define RL_PURPLE     0xc87aff  // Purple
#define RL_VIOLET     0x873cbe  // Violet
#define RL_DARKPURPLE 0x701f7e  // Dark Purple
#define RL_BEIGE      0xd3b083  // Beige
#define RL_BROWN      0x7f6a4f  // Brown
#define RL_DARKBROWN  0x4c3f2f  // Dark Brown
#define RL_WHITE      0xffffff  // White
#define RL_BLACK      0x000000  // Black
#define RL_BLANK      0x000000  // Blank (Transparent)
#define RL_MAGENTA    0xff00ff  // Magenta
#define RL_RAYWHITE   0xf5f5f5  // My own White (raylib logo)

static struct {
  int width;
  int height;
  uint32_t* buffer;
  double lastFrameTime;
  bool isResizable, hasResized, isFocused, hasCloseRequest, isFullScreen;
  void* platformData;
  int screenWidth, screenHeight;
  int windowPosX, windowPosY;
  int mousePosition[2];
  bool mouseButtonsPressed[5];
  bool mouseButtonsHold[3];
  int fps;
  unsigned long long pressedKeys, holdKeys;
} fenster = {0};

typedef struct Vector2 {
  int x, y;
} Vector2;

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
  return 0;
}

static inline uint32_t rl_GetPixelUnsafe(int x, int y) {
  return fenster.buffer[y * fenster.width + x];
}

void rl_PollInputEvents(void) {
  memset(fenster.mouseButtonsPressed, 0, sizeof(fenster.mouseButtonsPressed));
  fenster.pressedKeys = 0;
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
  return (fenster.holdKeys & (1ULL << keycode)) != 0;
}

bool rl_IsKeyPressed(int keycode) {
  return (fenster.pressedKeys & (1ULL << keycode)) != 0;
}

#ifdef USE_FONTS
#include "fensterRL_fonts.h"
#endif

#endif // FENSTERRL_H
