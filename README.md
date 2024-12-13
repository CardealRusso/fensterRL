```C
// Window-related functions
void rl_InitWindow(int width, int height, const char* title);  // Initialize window
void rl_CloseWindow(void);                                     // Close window
void rl_PollInputEvents(void);                                 // Register all input events
void rl_SetWindowTitle(const char* title);                     // Set title for window
void rl_SetWindowPosition(int x, int y);                       // Set window position on screen
void rl_SetWindowPositionV(Vector2 position)                   // Set window position on screen with Vector2
void rl_SetWindowSize(int width, int height);                  // Set window dimensions
void rl_SetWindowFocused(void);                                // Set window focused
void rl_ToggleFullscreen(void);                                // Toggle window state: fullscreen/windowed, resizes window to match monitor resolution
bool rl_IsWindowResized(void);                                 // Check if window has been resized last frame
bool rl_IsWindowFocused(void);                                 // Check if window is currently focused
bool rl_IsWindowFullscreen(void);                              // Check if window is currently fullscreen
bool rl_IsCloseRequested(void);                                // Check if the close button has been pressed
int rl_GetWindowPositionX(void);                               // Get window position X on monitor
int rl_GetWindowPositionY(void);                               // Get window position Y on monitor
Vector2 rl_GetWindowPositionV(void)                            // Get window position XY on monitor
int rl_GetWindowWidth(void);                                   // Get current window width
int rl_GetWindowHeight(void);                                  // Get current window height
int rl_GetScreenWidth(void);                                   // Get current monitor width
int rl_GetScreenHeight(void);                                  // Get current monitor height

// Frame rendering
void rl_RenderFrame(void);                                     // Draws the buffer in the window
void rl_RenderFrameRec(Rectangle rect);                        // Draws only a rectangular buffer area

// Timing-related functions
void rl_WindowSync(int target_fps);                            // The program will sleep to achieve this fps
void rl_WaitTime(double seconds);                              // Wait for some time (halt program execution)
int rl_GetFPS(void);                                           // Get current FPS (Buggy and depends on WindowSync)

// Misc. functions
void rl_SetConfigFlags(int flags);                             // Setup init configuration flags (view FLAGS)

// Input-related functions: mouse
bool rl_IsMouseButtonPressed(int button);                      // Check if a mouse button has been pressed once
bool rl_IsMouseButtonDown(int button);                         // Check if a mouse button is being pressed
int rl_GetMouseX(void);                                        // Get mouse position X
int rl_GetMouseY(void);                                        // Get mouse position Y
Vector2 rl_GetMouseV(void)                                     // Get mouse position XY

// Input-related functions: keyboard
bool rl_IsKeyPressed(int key);                                 // Check if a key has been pressed once
bool rl_IsKeyDown(int key);                                    // Check if a key is being pressed
```

```C
// -DUSE_FONTS (needs -lm)
const char** rl_GetSystemFonts(void);                                          // Returns found system fonts (ttf)
void rl_DrawText(const char* text, int posX, int posY, 
                 int fontSize, const char* fontPath, 
                 uint32_t color, uint32_t bgcolor)                             // Draw text. Use 0xFFFFFFFF for transparent background
char* rl_TextFormat(const char* fmt, ...)
int rl_MeasureTextWidth(const char *text, int fontSize, const char* fontPath)  // Measure string width
int rl_MeasureTextHeight(const char *text, int fontSize, const char* fontPath) // Measure string Height
char* rl_TextFormat(const char* fmt, ...)                                      // Text formatting with variables (sprintf() style)
```

```C
// -DUSE_SHAPES (needs -lm)
// Optional: -DUSE_SIMD (needs -mavx2 (x86), neon for arm)

// Pixels
void rl_DrawPixel(int posX, int posY, uint32_t color)                                                // Draw a pixel
void rl_DrawPixelV(Vector2 position, uint32_t color)                                                 // Draw a pixel (Vector version)

// Drawing-related functions
void rl_LinearBufferFill(size_t offset, size_t count, uint32_t color)                                // Unsafe linear color fill (SIMD-accelerated, uses memset for color 0)
void rl_ClearBackground(uint32_t color);                                                             // Set background color (SIMD-accelerated)

// Lines
void rl_DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, uint32_t color)             // Draw a line (SIMD-accelerated for horizontal lines)
void rl_DrawLineV(Vector2 startPos, Vector2 endPos, uint32_t color)                                  // Draw a line (Vector version)
void rl_DrawLineEx(Vector2 startPos, Vector2 endPos, int thick, uint32_t color)                      // Draw a thick line

// Rectangles
void rl_DrawRectangle(int posX, int posY, int width, int height, uint32_t color)                     // Draw a color-filled rectangle (SIMD-accelerated)
void rl_DrawRectangleV(Vector2 position, Vector2 size, uint32_t color)                               // Draw a color-filled rectangle (Vector version)
void rl_DrawRectangleRec(Rectangle rec, uint32_t color)                                              // Draw a color-filled rectangle
void rl_DrawRectangleLines(int posX, int posY, int width, int height, uint32_t color)                // Draw rectangle outline (Half SIMD-accelerated)
void rl_DrawRectangleLinesEx(Rectangle rec, float lineThick, uint32_t color)                         // Draw rectangle outline with extended parameters

// Circles
void rl_DrawCircle(int centerX, int centerY, int radius, uint32_t color)                             // Draw a color-filled circle (SIMD-accelerated)
void rl_DrawCircleV(Vector2 center, int radius, uint32_t color)                                      // Draw a color-filled circle (Vector version)

// Basic shapes collision detection functions
bool rl_CheckCollisionRecs(Rectangle rec1, Rectangle rec2)                                           // Check collision between two rectangles
Rectangle rl_GetCollisionRec(Rectangle rec1, Rectangle rec2)                                         // Get collision rectangle for two rectangles collision
```
