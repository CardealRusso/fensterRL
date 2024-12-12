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
