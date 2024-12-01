```C
// Window-related functions
void rl_InitWindow(int width, int height, const char* title);  // Initialize window
void rl_CloseWindow(void);                                     // Close window
void rl_WindowEventLoop(void);                                 // Updates the window events (very needed)
void rl_RenderFrame(void);                                     // Render the frame
void rl_SetWindowTitle(const char* title);                     // Set title for window
void rl_SetWindowPosition(int x, int y);                       // Set window position on screen
void rl_SetWindowSize(int width, int height);                  // Set window dimensions
void rl_SetWindowFocused(void);                                // Set window focused
bool rl_IsWindowResized(void);                                 // Check if window has been resized last frame
bool rl_IsWindowFocused(void);                                 // Check if window is currently focused
bool rl_IsCloseRequested(void);                                // Returns true if the window received WM_DELETE_WINDOW (x11), WM_CLOSE (gdi32) or windowShouldClose: (cocoa)
int rl_GetWindowPositionX(void);                               // Get window position X on monitor
int rl_GetWindowPositionY(void);                               // Get window position Y on monitor
int rl_GetWindowWidth(void);                                   // Get current window width
int rl_GetWindowHeight(void);                                  // Get current window height
int rl_GetScreenWidth(void);                                   // Get current monitor width
int rl_GetScreenHeight(void);                                  // Get current monitor height

// Drawing-related functions
void rl_ClearBackground(uint32_t color);                       // Set background color (framebuffer clear color)
void rl_SetPixel(int x, int y, uint32_t color);                // Draw a pixel
void rl_SetPixelUnsafe(int x, int y, uint32_t color);          // Draw a pixel (Faster and unsafe)
uint32_t rl_GetPixel(int x, int y);                            // Get a pixel color (returns 0 if out of bounds)
uint32_t rl_GetPixelUnsafe(int x, int y);                      // Get a pixel color (Faster and unsafe)

// Timing-related functions
void rl_WindowSync(int fps);                                   // The program will sleep to achieve this fps

// Misc. functions
void rl_SetConfigFlags(int flags);                             // Setup init configuration flags (view FLAGS)

// Input-related functions: mouse
int rl_GetMouseX(void);                                         // Get mouse position X
int rl_GetMouseY(void);                                         // Get mouse position Y
```
