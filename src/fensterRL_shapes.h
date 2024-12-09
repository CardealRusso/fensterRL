#ifndef FENSTERRL_SHAPES_H
#define FENSTERRL_SHAPES_H

#include <math.h>

static inline void rl_DrawPixel(int posX, int posY, uint32_t color) {
    if (posX >= 0 && posX < fenster.width && posY >= 0 && posY < fenster.height) {
        fenster.buffer[posY * fenster.width + posX] = color;
    }
}

static inline void rl_DrawPixelV(Vector2 position, uint32_t color) {
    rl_DrawPixel(position.x, position.y, color);
}

static inline uint32_t rl_GetPixel(int x, int y) {
    return fenster.buffer[y * fenster.width + x];
}

static inline uint32_t rl_GetPixelV(Vector2 position) {
    return rl_GetPixel(position.x, position.y);
}

void rl_DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, uint32_t color) {
    int dx = abs(endPosX - startPosX);
    int dy = abs(endPosY - startPosY);
    int sx = startPosX < endPosX ? 1 : -1;
    int sy = startPosY < endPosY ? 1 : -1;
    int err = dx - dy;

    while (1) {
        rl_DrawPixel(startPosX, startPosY, color);

        if (startPosX == endPosX && startPosY == endPosY) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            startPosX += sx;
        }
        if (e2 < dx) {
            err += dx;
            startPosY += sy;
        }
    }
}

void rl_DrawLineV(Vector2 startPos, Vector2 endPos, uint32_t color) {
    rl_DrawLine(startPos.x, startPos.y, endPos.x, endPos.y, color);
}

void rl_DrawLineEx(Vector2 startPos, Vector2 endPos, int thick, uint32_t color) {
    for (int i = 0; i < thick; i++) {
        rl_DrawLine(startPos.x + i, startPos.y, 
                    endPos.x + i, endPos.y, color);
        rl_DrawLine(startPos.x, startPos.y + i, 
                    endPos.x, endPos.y + i, color);
    }
}

void rl_DrawLineStrip(const Vector2 *points, int pointCount, uint32_t color) {
    if (pointCount < 2) return;

    for (int i = 0; i < pointCount - 1; i++) {
        rl_DrawLineV(points[i], points[i+1], color);
    }
}

int rl_IntLinearInterpolate(int start, int end, int step, int totalSteps) {
    return start + (end - start) * step / totalSteps;
}

void rl_DrawLineBezier(Vector2 startPos, Vector2 endPos, int thick, uint32_t color) {
    const int steps = 20;
    
    for (int i = 0; i <= steps; i++) {
        Vector2 current = {
            rl_IntLinearInterpolate(startPos.x, endPos.x, i, steps),
            rl_IntLinearInterpolate(startPos.y, endPos.y, i, steps)
        };
        
        rl_DrawLineEx(current, current, thick, color);
    }
}

void rl_DrawCircle(int centerX, int centerY, int radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                rl_DrawPixel(centerX + x, centerY + y, color);
            }
        }
    }
}

void rl_DrawCircleV(Vector2 center, int radius, uint32_t color) {
    rl_DrawCircle(center.x, center.y, radius, color);
}

void rl_DrawCircleLines(int centerX, int centerY, int radius, uint32_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        rl_DrawPixel(centerX + x, centerY + y, color);
        rl_DrawPixel(centerX + y, centerY + x, color);
        rl_DrawPixel(centerX - y, centerY + x, color);
        rl_DrawPixel(centerX - x, centerY + y, color);
        rl_DrawPixel(centerX - x, centerY - y, color);
        rl_DrawPixel(centerX - y, centerY - x, color);
        rl_DrawPixel(centerX + y, centerY - x, color);
        rl_DrawPixel(centerX + x, centerY - y, color);

        y++;
        if (err <= 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x + 1);
        }
    }
}

void rl_DrawCircleLinesV(Vector2 center, int radius, uint32_t color) {
    rl_DrawCircleLines(center.x, center.y, radius, color);
}

void rl_DrawCircleSector(Vector2 center, int radius, int startAngle, int endAngle, int segments, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int dist = x*x + y*y;
            if (dist <= radius*radius) {
                float angle = atan2(y, x) * 180.0f / M_PI;
                if (angle < 0) angle += 360;
                
                if (angle >= startAngle && angle <= endAngle) {
                    rl_DrawPixel(center.x + x, center.y + y, color);
                }
            }
        }
    }
}

void rl_DrawCircleSectorLines(Vector2 center, int radius, int startAngle, int endAngle, int segments, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int dist = x*x + y*y;
            if (abs(dist - radius*radius) < radius) {
                float angle = atan2(y, x) * 180.0f / M_PI;
                if (angle < 0) angle += 360;
                
                if (angle >= startAngle && angle <= endAngle) {
                    rl_DrawPixel(center.x + x, center.y + y, color);
                }
            }
        }
    }
}

void rl_DrawCircleGradient(int centerX, int centerY, int radius, uint32_t innerColor, uint32_t outerColor) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int dist = x * x + y * y;
            if (dist <= radius * radius) {
                uint32_t color = (dist * innerColor + (radius * radius - dist) * outerColor) / (radius * radius);
                rl_DrawPixel(centerX + x, centerY + y, color);
            }
        }
    }
}

#endif // FENSTERRL_SHAPES_H
