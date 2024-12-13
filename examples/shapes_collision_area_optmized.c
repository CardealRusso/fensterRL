#include "fensterRL.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;
    const char** fonts = rl_GetSystemFonts();
    
    rl_InitWindow(screenWidth, screenHeight, "example - collision area optmized");

    Rectangle boxA = { 10, rl_GetWindowHeight()/2.0f - 50, 200, 100 };
    Rectangle boxB = { rl_GetWindowWidth()/2.0f - 30, rl_GetWindowHeight()/2.0f - 30, 60, 60 };
    
    Rectangle boxCollision = { 0 };
    int boxASpeedX = 4;
    int screenUpperLimit = 40;
    bool pause = false;
    bool collision = false;
    bool last_collision = false;
    static int frameCount = 118;
    
    rl_ClearBackground(RL_RAYWHITE);
    rl_DrawRectangle(0, 0, screenWidth, screenUpperLimit, RL_BLACK);
    rl_DrawText("Press SPACE to PAUSE/RESUME!", 20, screenHeight - 35, 20, fonts[0], RL_BLACK, 0xFFFFFFFF);
    rl_RenderFrame();
    
    while (!rl_IsCloseRequested() && !rl_IsKeyPressed(RL_KEY_ESC)) {
        frameCount +=1;
        if (++frameCount == 120) {
            frameCount = 0;
            rl_ClearBackground(RL_RAYWHITE);
            rl_DrawRectangle(0, 0, screenWidth, screenUpperLimit, collision? RL_RED : RL_BLACK);
            rl_DrawText("Press SPACE to PAUSE/RESUME!", 20, screenHeight - 35, 20, fonts[0], RL_BLACK, 0xFFFFFFFF);
            rl_RenderFrame();
        }
        rl_PollInputEvents();

        if (boxB.x != rl_GetMouseX() - boxB.width/2 && boxB.y != rl_GetMouseY() - boxB.height/2) {
            rl_DrawRectangleRec(boxB, RL_RAYWHITE);
            rl_RenderFrameRec(boxB);
            boxB.x = rl_GetMouseX() - boxB.width/2;
            boxB.y = rl_GetMouseY() - boxB.height/2;
        }
        rl_DrawRectangleRec(boxA, RL_RAYWHITE);
        rl_RenderFrameRec(boxA);
        boxA.x += boxASpeedX;
        if (((boxA.x + boxA.width) >= rl_GetWindowWidth()) || (boxA.x <= 0)) 
            boxASpeedX *= -1;


        if ((boxB.x + boxB.width) >= rl_GetWindowWidth()) boxB.x = rl_GetWindowWidth() - boxB.width;
        else if (boxB.x <= 0) boxB.x = 0;

        if ((boxB.y + boxB.height) >= rl_GetWindowHeight()) boxB.y = rl_GetWindowHeight() - boxB.height;
        else if (boxB.y <= screenUpperLimit) boxB.y = (float)screenUpperLimit;

        collision = rl_CheckCollisionRecs(boxA, boxB);

        rl_DrawRectangleRec(boxA, RL_GOLD);
        rl_DrawRectangleRec(boxB, RL_BLUE);
        rl_RenderFrameRec(boxB);
        rl_RenderFrameRec(boxA);
        if (last_collision != collision) {
            rl_DrawRectangle(0, 0, screenWidth, screenUpperLimit, collision? RL_RED : RL_BLACK);
            if (collision) {
                rl_DrawText("COLLISION!", rl_GetWindowWidth()/2 - rl_MeasureTextWidth("COLLISION!", 20, fonts[0]), screenUpperLimit/2, 20, fonts[0], RL_BLACK, 0xFFFFFFFF);
            }
        }
        if (collision) {
            boxCollision = rl_GetCollisionRec(boxA, boxB);
            if (frameCount == 0) {
                rl_DrawText("COLLISION!", rl_GetWindowWidth()/2 - rl_MeasureTextWidth("COLLISION!", 20, fonts[0]), screenUpperLimit/2, 20, fonts[0], RL_BLACK, 0xFFFFFFFF);
            }
            rl_DrawRectangleRec(boxCollision, RL_LIME);
            rl_RenderFrameRec(boxCollision);
            char temp[30];
            sprintf(temp,"Collision Area: %i", (int)boxCollision.width*(int)boxCollision.height);
            int textwidth = rl_MeasureTextWidth(temp, 20, fonts[0]);
            int textheight = rl_MeasureTextHeight(temp, 20, fonts[0]);
            rl_DrawText(temp, rl_GetWindowWidth()/2 - 100, screenUpperLimit + 10, 20, fonts[0], RL_BLACK, RL_RAYWHITE);
            rl_RenderFrameRec((Rectangle){rl_GetWindowWidth()/2 - 100, screenUpperLimit + 10, textwidth, textheight});
        }
        if (last_collision != collision) {
            rl_RenderFrameRec((Rectangle){0, 0, screenWidth, screenUpperLimit});
        }
        if (rl_IsKeyPressed(RL_KEY_SPACE)) pause = !pause;
        rl_WindowSync(60);
        last_collision = collision;
    }

    rl_CloseWindow();

    return 0;
}
