#include "fensterRL.h"
#include <stdio.h>

int main(void) {
  rl_InitWindow(800, 600, "White Noise Example");
  rl_ClearBackground(RL_RAYWHITE);
printf("ok\n");
  while (true) {
    rl_PollInputEvents();
    rl_ClearBackground(0);
    if (rl_IsCloseRequested() || rl_IsKeyPressed(1)) {
      break;
    }
    rl_DrawLine(50, 50, rl_GetMouseX(), rl_GetMouseY(), RL_RAYWHITE);
    rl_RenderFrame();

    if (rl_IsWindowFocused()) {
      rl_WindowSync(60);
    } else {
      rl_WindowSync(5);
    }
  }

  rl_CloseWindow();
  return 0;
}
