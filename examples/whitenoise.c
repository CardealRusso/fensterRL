#include "fensterRL.h"
#include <stdio.h>

int main(void) {
  rl_SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  rl_InitWindow(800, 600, "White Noise Example");

  while (true) {
    rl_WindowEventLoop();
    if (rl_IsCloseRequested()) {
      printf("bye\n");
      break;
    }

    printf("%d\n",rl_GetMouseX());
    rl_SetWindowTitle("test");
    for (int y = 0; y < rl_GetWindowHeight(); y++) {
      for (int x = 0; x < rl_GetWindowWidth(); x++) {
        rl_SetPixel(x, y, rand());
      }
    }

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
