#include "fensterRL.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const char** fonts = rl_GetSystemFonts();

    rl_InitWindow(screenWidth, screenHeight, "example - basic window");
    
    while (!rl_IsCloseRequested() && !rl_IsKeyPressed(RL_KEY_ESC)) {
        rl_PollInputEvents();

        rl_ClearBackground(RL_RAYWHITE);
        if (fonts) {
            rl_DrawText("Congrats! You created your first window!", 190, 200, 20, fonts[0], RL_LIGHTGRAY, 0xFFFFFFFF);
        }
        rl_RenderFrame();
        rl_WindowSync(5);
    }
    
    rl_CloseWindow();
    return 0;
}
