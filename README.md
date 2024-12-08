```C
// Window-related functions
void rl_InitWindow(int width, int height, const char* title);  // Initialize window
void rl_CloseWindow(void);                                     // Close window
void rl_PollInputEvents(void);                                 // Register all input events
void rl_RenderFrame(void);                                     // Render the frame
void rl_SetWindowTitle(const char* title);                     // Set title for window
void rl_SetWindowPosition(int x, int y);                       // Set window position on screen
void rl_SetWindowSize(int width, int height);                  // Set window dimensions
void rl_SetWindowFocused(void);                                // Set window focused
void rl_ToggleFullscreen(void);                                // Toggle window state: fullscreen/windowed, resizes window to match monitor resolution
bool rl_IsWindowResized(void);                                 // Check if window has been resized last frame
bool rl_IsWindowFocused(void);                                 // Check if window is currently focused
bool rl_IsWindowFullscreen(void);                              // Check if window is currently fullscreen
bool rl_IsCloseRequested(void);                                // Check if the close button has been pressed
int rl_GetWindowPositionX(void);                               // Get window position X on monitor
int rl_GetWindowPositionY(void);                               // Get window position Y on monitor
int rl_GetWindowWidth(void);                                   // Get current window width
int rl_GetWindowHeight(void);                                  // Get current window height
int rl_GetScreenWidth(void);                                   // Get current monitor width
int rl_GetScreenHeight(void);                                  // Get current monitor height

// Drawing-related functions
void rl_ClearBackground(uint32_t color);                       // Set background color
void rl_SetPixel(int x, int y, uint32_t color);                // Draw a pixel
void rl_SetPixelUnsafe(int x, int y, uint32_t color);          // Draw a pixel (Faster and unsafe)
uint32_t rl_GetPixel(int x, int y);                            // Get a pixel color (returns 0 if out of bounds)
uint32_t rl_GetPixelUnsafe(int x, int y);                      // Get a pixel color (Faster and unsafe)

// Timing-related functions
void rl_WindowSync(int fps);                                   // The program will sleep to achieve this fps
void rl_WaitTime(double seconds);                              // Wait for some time (halt program execution)
int rl_GetFPS(void);                                           // Get current FPS (Buggy and depends on WindowSync)

// Misc. functions
void rl_SetConfigFlags(int flags);                             // Setup init configuration flags (view FLAGS)

// Input-related functions: mouse
bool rl_IsMouseButtonPressed(int button);                      // Check if a mouse button has been pressed once
bool rl_IsMouseButtonDown(int button);                         // Check if a mouse button is being pressed
int rl_GetMouseX(void);                                        // Get mouse position X
int rl_GetMouseY(void);                                        // Get mouse position Y

// Input-related functions: keyboard
bool rl_IsKeyPressed(int key);                                 // Check if a key has been pressed once
bool rl_IsKeyDown(int key);                                    // Check if a key is being pressed
```

```C
//-DUSE_FONTS (needs -lm)
const char** rl_GetSystemFonts(void);                          // Returns found system fonts (ttf)
void rl_DrawText(const char* text, int posX, int posY, 
                 int fontSize, const char* fontPath, 
                 uint32_t color, uint32_t bgcolor)             // Draw text. Use 0xFFFFFFFF for transparent background
```
