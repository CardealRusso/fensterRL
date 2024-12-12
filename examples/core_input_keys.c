#include "fensterRL.h"

int main(void){
    const int screenWidth = 800;
    const int screenHeight = 450;
    const char** fonts = rl_GetSystemFonts();

    rl_InitWindow(screenWidth, screenHeight, "example - keyboard input");

    Vector2 ballPosition = { screenWidth/2, screenHeight/2 };

    while (!rl_IsCloseRequested() && !rl_IsKeyPressed(RL_KEY_ESC)) {
        rl_PollInputEvents();

        if (rl_IsKeyDown(RL_KEY_RIGHT)) ballPosition.x += 2;
        if (rl_IsKeyDown(RL_KEY_LEFT)) ballPosition.x -= 2;
        if (rl_IsKeyDown(RL_KEY_UP)) ballPosition.y -= 2;
        if (rl_IsKeyDown(RL_KEY_DOWN)) ballPosition.y += 2;

        rl_ClearBackground(RL_RAYWHITE);

        rl_DrawText("move the ball with arrow keys", 10, 10, 20, fonts[0], RL_DARKGRAY, 0xFFFFFFFF);

        rl_DrawCircleV(ballPosition, 50, RL_MAROON);

        rl_RenderFrame();
        rl_WindowSync(60);
    }

    rl_CloseWindow();

    return 0;
}
