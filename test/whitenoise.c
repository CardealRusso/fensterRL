#include "fensterRL.h"
#include <stdio.h>

int main(void) {
  rl_SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  rl_InitWindow(800, 600, "White Noise Example");
  const char** fonts = rl_GetSystemFonts();

  while (true) {
    rl_PollInputEvents();
    rl_ClearBackground(RL_RAYWHITE);
    if (rl_IsCloseRequested() || rl_IsKeyPressed(1)) {
      printf("bye\n");
      break;
    }
    if (rl_IsKeyPressed(28)) {
      printf("enter\n");
    }
    uint32_t red = 0xFF0000FF;
    uint32_t green = 0x00FF00FF;
    uint32_t blue = 0x0000FFFF;
    uint32_t yellow = 0xFFFF00FF;

    // Posição central
    Vector2 center = {400, 300};

    // Exemplo 1: Desenhando um setor de círculo (preenchido)
    // Setor de 90 graus, entre 0 e 90 graus
    rl_DrawCircleSector(center, 100, 0, 150, 100, red);

    rl_DrawCircleGradient(150, 200, 50, blue, yellow);

    rl_SetWindowTitle("test");

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
