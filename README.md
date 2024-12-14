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
int rl_MeasureTextWidth(const char *text, int fontSize, const char* fontPath)  // Measure string width
int rl_MeasureTextHeight(const char *text, int fontSize, const char* fontPath) // Measure string Height
char* rl_TextFormat(const char* fmt, ...)                                      // Text formatting with variables (sprintf() style) (Static, limited by fensterRL_macros.h)

// Memory managment
void rl_FreeGlyphs(void)                                                       // Free all cached Glyphs
void rl_FreeFonts(void)                                                        // Free all loaded fonts (and glyphs)
void rl_FreeFont(const char* fontPath)                                         // Free a specific font
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
void rl_DrawRectangleV(Vector2 position, int width, int height, uint32_t color)                      // Draw a color-filled rectangle (Vector version)
void rl_DrawRectangleVS(Vector2 position, Size2 size, uint32_t color)                                // Draw a color-filled rectangle (Vector and size version)
void rl_DrawRectangleRec(Rectangle rec, uint32_t color)                                              // Draw a color-filled rectangle

// Rectangles Line
void rl_DrawRectangleLines(int posX, int posY, int width, int height, uint32_t color)                // Draw rectangle outline (Half SIMD-accelerated)
void rl_DrawRectangleLinesV(Vector2 position, int width, int height, uint32_t color)                 // Draw rectangle outline (Vector version)
void rl_DrawRectangleLinesVS(Vector2 position, Size2 size, uint32_t color)                           // Draw rectangle outline (Vector and size version)
void rl_DrawRectangleLinesRec(Rectangle rec, uint32_t color)                                         // Draw rectangle outline
void rl_DrawRectangleLinesEx(Rectangle rec, float lineThick, uint32_t color)                         // Draw rectangle outline with extended parameters

// Circles
void rl_DrawCircle(int centerX, int centerY, int radius, uint32_t color)                             // Draw a color-filled circle (SIMD-accelerated)
void rl_DrawCircleV(Vector2 center, int radius, uint32_t color)                                      // Draw a color-filled circle (Vector version)

// Basic shapes collision detection functions
bool rl_CheckCollisionRecs(Rectangle rec1, Rectangle rec2)                                           // Check collision between two rectangles
Rectangle rl_GetCollisionRec(Rectangle rec1, Rectangle rec2)                                         // Get collision rectangle for two rectangles collision

// Poly
void rl_DrawPolyPoints(const Vector2 *points, int pointCount, uint32_t color)
void rl_DrawPolyLinePoints(const Vector2 *points, int pointCount, uint32_t color)

void rl_DrawPoly(Vector2 center, int sides, float radius, float rotation, uint32_t color)
void rl_DrawPolyLines(Vector2 center, int sides, float radius, float rotation, uint32_t color)
void rl_DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float lineThick, uint32_t color)

```

```C
// -DUSE_IMAGES (needs -DUSE_SHAPES)
Image* rl_LoadImage(const char* filePath)                                                            // Load image from file
Image* rl_LoadImageFromScreen(void)                                                                  // Load image from screen buffer and (screenshot) (SIMD-accelerated)
Image* rl_LoadImageFromScreenV(Vector2 position, int width, int height)
Image* rl_LoadImageFromScreenVS(Vector2 position, Size2 size)
Image* rl_LoadImageFromScreenRec(Rectangle area)
void rl_UnloadImage (Image* image)                                                                   // Unload image

void rl_DrawImageEx(Image* image, Vector2 position, float rotation, float scale)
```

```C
// fensterRL_struct.h
typedef struct Vector2 {
  int x, y;
} Vector2;

typedef struct Size2 {
  int width, height;
} Size2;

typedef struct Rectangle {
  int x, y;
  int width, height;
} Rectangle;

typedef struct Image {
  uint32_t* pixels;
  int width, height;
} Image;

// fensterRL_macros.h
//Settings
#define MAX_KEYS 1024
#define TEXT_FORMAT_BUFFER_SIZE (1024)

//Flags
#define FLAG_WINDOW_RESIZABLE (1 << 0)

//Colors
#define RL_LIGHTGRAY  0xc8c8c8  // Light Gray
#define RL_GRAY       0x828282  // Gray
#define RL_DARKGRAY   0x505050  // Dark Gray
#define RL_YELLOW     0xfdf900  // Yellow
#define RL_GOLD       0xffcb00  // Gold
#define RL_ORANGE     0xffa100  // Orange
#define RL_PINK       0xff6dc2  // Pink
#define RL_RED        0xe62937  // Red
#define RL_MAROON     0xbe2137  // Maroon
#define RL_GREEN      0x00e430  // Green
#define RL_LIME       0x009e2f  // Lime
#define RL_DARKGREEN  0x00752c  // Dark Green
#define RL_SKYBLUE    0x66bfff  // Sky Blue
#define RL_BLUE       0x0079f1  // Blue
#define RL_DARKBLUE   0x0052ac  // Dark Blue
#define RL_PURPLE     0xc87aff  // Purple
#define RL_VIOLET     0x873cbe  // Violet
#define RL_DARKPURPLE 0x701f7e  // Dark Purple
#define RL_BEIGE      0xd3b083  // Beige
#define RL_BROWN      0x7f6a4f  // Brown
#define RL_DARKBROWN  0x4c3f2f  // Dark Brown
#define RL_WHITE      0xffffff  // White
#define RL_BLACK      0x000000  // Black
#define RL_BLANK      0x000000  // Blank (Transparent)
#define RL_MAGENTA    0xff00ff  // Magenta
#define RL_RAYWHITE   0xf5f5f5  // raylib logo

//Keys
#ifdef __linux__
   #define RL_KEY_ESC   1
   #define RL_KEY_SPACE 57
   #define RL_KEY_LEFT  105
   #define RL_KEY_UP    103
   #define RL_KEY_RIGHT 106
   #define RL_KEY_DOWN  108
#elif defined(_WIN32)
   #define RL_KEY_ESC   1
   #define RL_KEY_SPACE 57
   #define RL_KEY_LEFT  75
   #define RL_KEY_UP    72
   #define RL_KEY_RIGHT 77
   #define RL_KEY_DOWN  80
#elif defined(__APPLE__)
   #define RL_KEY_SPACE 49
   #define RL_KEY_ESC   53
   #define RL_KEY_LEFT  123
   #define RL_KEY_UP    126
   #define RL_KEY_RIGHT 124
   #define RL_KEY_DOWN  125
#endif

```
