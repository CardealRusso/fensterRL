#include "fensterRL.h"
#include <stdio.h>

int main(void) {
  rl_SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  rl_InitWindow(800, 600, "White Noise Example");
  const char** fonts = rl_GetSystemFonts();
  const char** fonts2 = rl_GetSystemFonts();
  if (fonts) {
    printf("test\n");
    printf("First font path: %s\n", fonts[0]);
  }
  if (fonts2) {
    printf("test\n");
    printf("First font path: %s\n", fonts2[0]);
  }

  while (true) {
    rl_PollInputEvents();
    if (rl_IsCloseRequested()) {
      printf("bye\n");
      break;
    }

    if (rl_IsMouseButtonDown(1)) {
      printf("pressionado\n");
    }
    rl_SetWindowTitle("test");
    for (int y = 0; y < rl_GetWindowHeight(); y++) {
      for (int x = 0; x < rl_GetWindowWidth(); x++) {
        rl_SetPixel(x, y, rand());
      }
    }

    rl_DrawText("test", 0, 0, 32, fonts[0], 0, 0xFFFFFFFF);
    rl_RenderFrame();

    if (rl_IsWindowFocused()) {
      rl_WindowSync(60);
    } else {
      //printf("Reduced FPS while the window is not in focus to save a bit of cpu cycling.\n");
      rl_WindowSync(5);
    }
  }

  rl_CloseWindow();
  return 0;
}
