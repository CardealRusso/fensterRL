static struct {
  int width;
  int height;
  uint32_t* buffer;
  double lastFrameTime;
  bool isResizable, hasResized, isFocused, hasCloseRequest, isFullScreen;
  void* platformData;
  int screenWidth, screenHeight;
  int windowPosX, windowPosY;
  int mousePosition[2];
  bool mouseButtonsPressed[5];
  bool mouseButtonsHold[3];
  int fps;
  bool pressedKeys[MAX_KEYS];
  bool holdKeys[MAX_KEYS];
} fenster = {0};

typedef struct Vector2 {
  int x, y;
} Vector2;

typedef struct Rectangle {
    int x, y;
    int width, height;
} Rectangle;
