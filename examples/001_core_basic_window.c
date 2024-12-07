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
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!rl_IsCloseRequested())    // Detect window close button
    {
        rl_PollInputEvents();
        // Draw
        //----------------------------------------------------------------------------------

        rl_ClearBackground(RL_RAYWHITE);
        if (fonts) {
            rl_DrawText("Congrats! You created your first window!", 190, 200, 20, fonts[0], RL_LIGHTGRAY, 0xFFFFFFFF);
        }
        rl_RenderFrame();
        rl_WindowSync(5);
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    rl_CloseWindow();        // Close window
    //--------------------------------------------------------------------------------------

    return 0;
}
