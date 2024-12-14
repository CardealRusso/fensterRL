#include "fensterRL.h"

//SIMD is not ignoring transparencies (0xFFFFFFFF) so we disabled simd for this example
int main(void) {
  rl_InitWindow(800, 450, "White Noise Example");
  Image* background = rl_LoadImage("cyberpunk_street_background.png");
  Image* midground = rl_LoadImage("cyberpunk_street_midground.png");
  Image* foreground = rl_LoadImage("cyberpunk_street_foreground.png");
  
  float scrollingBack = 0.0f;
  float scrollingMid = 0.0f;
  float scrollingFore = 0.0f;
  
  while (true) {
    rl_PollInputEvents();
    rl_ClearBackground(RL_RAYWHITE);
    if (rl_IsCloseRequested() || rl_IsKeyPressed(1)) {
      break;
    }

    scrollingBack -= 0.1f;
    scrollingMid -= 0.5f;
    scrollingFore -= 1.0f;

    if (scrollingBack <= -background->width*2) scrollingBack = 0;
    if (scrollingMid <= -midground->width*2) scrollingMid = 0;
    if (scrollingFore <= -foreground->width*2) scrollingFore = 0;
        
    rl_DrawImageEx(background, (Vector2){ scrollingBack, 20 }, 0.0f, 2.0f);
    rl_DrawImageEx(background, (Vector2){ background->width*2 + scrollingBack, 20 }, 0.0f, 2.0f);

    rl_DrawImageEx(midground, (Vector2){ scrollingMid, 20 }, 0.0f, 2.0f);
    rl_DrawImageEx(midground, (Vector2){ midground->width*2 + scrollingMid, 20 }, 0.0f, 2.0f);

    rl_DrawImageEx(foreground, (Vector2){ scrollingFore, 70 }, 0.0f, 2.0f);
    rl_DrawImageEx(foreground, (Vector2){ foreground->width*2 + scrollingFore, 70 }, 0.0f, 2.0f);

    
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
