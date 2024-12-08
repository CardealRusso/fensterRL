/*******************************************************************************************
*
*   FensterRL example - Basic window
*
*   Example originally created with raylib 1.0, last time updated with fensterRL
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "fensterRL.h"

//------------------------------------------------------------------------------------------
// Types and Structures Definition
//------------------------------------------------------------------------------------------
typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    const char** fonts = rl_GetSystemFonts();

    rl_InitWindow(screenWidth, screenHeight, "fensterRL example - basic window");

    GameScreen currentScreen = LOGO;

    int framesCounter = 0;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!rl_IsCloseRequested() && !rl_IsKeyPressed(1))    // Detect window close button and ESC key
    {
        rl_PollInputEvents();
        // Update
        //----------------------------------------------------------------------------------
        switch(currentScreen)
        {
            case LOGO:
            {
                // TODO: Update LOGO screen variables here!

                framesCounter++;    // Count frames

                // Wait for 2 seconds (120 frames) before jumping to TITLE screen
                if (framesCounter > 120)
                {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE:
            {
                // TODO: Update TITLE screen variables here!

                // Press enter to change to GAMEPLAY screen
                if (rl_IsMouseButtonPressed(0))
                {
                    currentScreen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                // TODO: Update GAMEPLAY screen variables here!

                // Press enter to change to ENDING screen
                if (rl_IsMouseButtonPressed(0))
                {
                    currentScreen = ENDING;
                }
            } break;
            case ENDING:
            {
                // TODO: Update ENDING screen variables here!

                // Press enter to return to TITLE screen
                if (rl_IsMouseButtonPressed(0))
                {
                    currentScreen = TITLE;
                }
            } break;
            default: break;
        }

        // Draw
        //----------------------------------------------------------------------------------
            rl_ClearBackground(RL_RAYWHITE);

            switch(currentScreen)
            {
                case LOGO:
                {
                    // TODO: Draw LOGO screen here!
                    rl_DrawText("LOGO SCREEN", 20, 20, 40, fonts[0], RL_LIGHTGRAY, 0xFFFFFFFF);
                    rl_DrawText("WAIT for 2 SECONDS...", 290, 220, 20, fonts[0], RL_GRAY, 0xFFFFFFFF);

                } break;
                case TITLE:
                {
                    // TODO: Draw TITLE screen here!
                    rl_ClearBackground(RL_GREEN);
                    rl_DrawText("TITLE SCREEN", 20, 20, 40, fonts[0], RL_DARKGREEN, 0xFFFFFFFF);
                    rl_DrawText("PRESS TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, fonts[0], RL_DARKGREEN, 0xFFFFFFFF);

                } break;
                case GAMEPLAY:
                {
                    // TODO: Draw GAMEPLAY screen here!
                    rl_ClearBackground(RL_PURPLE);
                    rl_DrawText("GAMEPLAY SCREEN", 20, 20, 40, fonts[0], RL_MAROON, 0xFFFFFFFF);
                    rl_DrawText("PRESS TAP to JUMP to ENDING SCREEN", 130, 220, 20, fonts[0], RL_MAROON, 0xFFFFFFFF);

                } break;
                case ENDING:
                {
                    // TODO: Draw ENDING screen here!
                    rl_ClearBackground(RL_BLUE);
                    rl_DrawText("ENDING SCREEN", 20, 20, 40, fonts[0], RL_DARKBLUE, 0xFFFFFFFF);
                    rl_DrawText("PRESS TAP to RETURN to TITLE SCREEN", 120, 220, 20, fonts[0], RL_DARKBLUE, 0xFFFFFFFF);

                } break;
                default: break;
            }

        rl_RenderFrame();
        rl_WindowSync(60);
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    rl_CloseWindow();        // Close window
    //--------------------------------------------------------------------------------------

    return 0;
}
