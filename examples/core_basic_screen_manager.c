#include "fensterRL.h"

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const char** fonts = rl_GetSystemFonts();

    rl_InitWindow(screenWidth, screenHeight, "example - basic screen manager");

    GameScreen currentScreen = LOGO;

    int framesCounter = 0;
    while (!rl_IsCloseRequested() && !rl_IsKeyPressed(RL_KEY_ESC)) {
        rl_PollInputEvents();
        
        switch(currentScreen) {
            case LOGO: {
                framesCounter++;
                if (framesCounter > 120) {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE: {
                if (rl_IsMouseButtonPressed(0)) {
                    currentScreen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY: {
                if (rl_IsMouseButtonPressed(0)) {
                    currentScreen = ENDING;
                }
            } break;
            case ENDING: {
                if (rl_IsMouseButtonPressed(0)) {
                    currentScreen = TITLE;
                }
            } break;
            default: break;
        }

            rl_ClearBackground(RL_RAYWHITE);

            switch(currentScreen) {
                case LOGO: {
                    rl_DrawText("LOGO SCREEN", 20, 20, 40, fonts[0], RL_LIGHTGRAY, 0xFFFFFFFF);
                    rl_DrawText("WAIT for 2 SECONDS...", 290, 220, 20, fonts[0], RL_GRAY, 0xFFFFFFFF);

                } break;
                case TITLE: {
                    rl_ClearBackground(RL_GREEN);
                    rl_DrawText("TITLE SCREEN", 20, 20, 40, fonts[0], RL_DARKGREEN, 0xFFFFFFFF);
                    rl_DrawText("PRESS TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, fonts[0], RL_DARKGREEN, 0xFFFFFFFF);

                } break;
                case GAMEPLAY: {
                    rl_ClearBackground(RL_PURPLE);
                    rl_DrawText("GAMEPLAY SCREEN", 20, 20, 40, fonts[0], RL_MAROON, 0xFFFFFFFF);
                    rl_DrawText("PRESS TAP to JUMP to ENDING SCREEN", 130, 220, 20, fonts[0], RL_MAROON, 0xFFFFFFFF);

                } break;
                case ENDING: {
                    rl_ClearBackground(RL_BLUE);
                    rl_DrawText("ENDING SCREEN", 20, 20, 40, fonts[0], RL_DARKBLUE, 0xFFFFFFFF);
                    rl_DrawText("PRESS TAP to RETURN to TITLE SCREEN", 120, 220, 20, fonts[0], RL_DARKBLUE, 0xFFFFFFFF);

                } break;
                default: break;
            }

        rl_RenderFrame();
        rl_WindowSync(60);
    }
    rl_CloseWindow();
    return 0;
}
