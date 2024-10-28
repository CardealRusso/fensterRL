#include "fensterRL.h"

int main(void) {
  rl_InitWindow(800, 600, "White Noise Example");
  rl_SetTargetFPS(60);

  while (!rl_WindowShouldClose()) {
    rl_ClearBackground(0x000000);

    for (int y = 0; y < 600; y++) {
      for (int x = 0; x < 800; x++) {
        rl_SetPixel(x, y, rand());
      }
    }
  }

  rl_CloseWindow();
  return 0;
}
