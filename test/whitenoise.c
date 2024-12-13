#include "fensterRL.h"
#include <stdio.h>

int main(void) {
  rl_InitWindow(800, 600, "White Noise Example");
printf("ok\n");
  while (true) {
    rl_PollInputEvents();
    rl_ClearBackground(RL_RAYWHITE);
    if (rl_IsCloseRequested() || rl_IsKeyPressed(1)) {
      break;
    }
#define WHITE 0xFFFFFF
#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF
#define YELLOW 0xFFFF00
#define PURPLE 0xFF00FF

    // Desenha uma linha simples
    Vector2 start = {100, 100};
    Vector2 end = {700, 200};
    rl_DrawLineV(start, end, BLUE);

    // Desenha uma linha grossa
    Vector2 thick_start = {100, 250};
    Vector2 thick_end = {700, 350};
    rl_DrawLineEx(thick_start, thick_end, 10, RED);

    // Desenha retângulos
    rl_DrawRectangle(400, 350, 200, 150, GREEN);

    Rectangle yellowrect = {
        .x = 200,
        .y = 500,
        .width = 150,
        .height = 100
    };
    rl_DrawRectangleRec(yellowrect, YELLOW);
    Rectangle thick_rect = {
        .x = rl_GetMouseX(),
        .y = rl_GetMouseY(),
        .width = 150,
        .height = 150
    };
    //rl_DrawRectangleRec(thick_rect, RED);
    if (rl_CheckCollisionRecs(yellowrect, thick_rect)) {
      Rectangle boxCollision = rl_GetCollisionRec(yellowrect, thick_rect);
      rl_DrawRectangleRec(boxCollision, WHITE);
    }
    // Desenha círculos
    rl_DrawCircle(5, 5, 15, GREEN);
    rl_DrawRectangle(5, 550, 5, 5, GREEN);
    rl_DrawCircle(200, 200, 100, YELLOW);
    rl_DrawCircle(600, 200, 50, RED);

    //rl_RenderFrame();
		rl_RenderFrameRec(thick_rect);
		rl_RenderFrameRec(yellowrect);
    if (rl_IsWindowFocused()) {
      rl_WindowSync(60);
    } else {
      rl_WindowSync(5);
    }
  }

  rl_CloseWindow();
  return 0;
}
