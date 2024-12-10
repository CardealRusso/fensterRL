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
    uint32_t RED = 0xFF0000;
    uint32_t GREEN = 0x00FF00;
    uint32_t BLUE = 0x0000FF;
    uint32_t YELLOW = 0xFFFF00;

    // Criando Vector2 manualmente
    Vector2 center = {400, 300};

        // Desenhar uma elipse sólida
        rl_DrawEllipse(200, 200, 100, 50, RED);

        // Desenhar contorno de elipse
        rl_DrawEllipseLines(600, 200, 80, 40, GREEN);

        // Desenhar um anel sólido
        rl_DrawRingIsoTrap(center, 50, 100, 0, 270, 15, BLUE);

        // Desenhar contorno de anel
        rl_DrawRingLines(center, 120, 150, 0, 180, 8, YELLOW);
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
