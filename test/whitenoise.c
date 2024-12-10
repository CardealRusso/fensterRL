#include "fensterRL.h"
#include <stdio.h>

float angle = 0.0f;
Rectangle rect = {
    800 / 2 - 100,  // Center horizontally
    600 / 2 - 25,  // Center vertically
    200, 
    50
};
Vector2 origin = {100, 25};  // Center of rectangle

int main(void) {
  rl_InitWindow(800, 600, "White Noise Example");

  while (true) {
    rl_PollInputEvents();
    rl_ClearBackground(RL_RAYWHITE);
    if (rl_IsCloseRequested() || rl_IsKeyPressed(1)) {
      printf("bye\n");
      break;
    }

    angle += 2.0f;  // Increment rotation
    if (angle >= 360.0f) angle -= 360.0f;
    // Draw rotated rectangle
    rl_DrawRectangleRec(rect, 0xFF0000);
    rl_DrawRectanglePro(rect, origin, angle, 0xFF0000);  // Red rectangle
    rl_SetWindowTitle("test");

    rl_DrawText("test ç", 0, 0, 32, "./unifont-15.1.05.ttf", 0, 0xFFFFFFFF);
    rl_RenderFrame();

    if (rl_IsWindowFocused()) {
      rl_WindowSync(30);
    } else {
      //printf("Reduced FPS while the window is not in focus to save a bit of cpu cycling.\n");
      rl_WindowSync(5);
    }
  }

  rl_CloseWindow();
  return 0;
}
