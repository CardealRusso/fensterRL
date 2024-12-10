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
    if (segments < 3) segments = 3;
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int dist = x*x + y*y;
            if (dist <= radius*radius) {
                float angle = atan2(y, x) * 180.0f / M_PI;
                if (angle < 0) angle += 360;
                
                float sweepAngle = endAngle - startAngle;
                float segmentAngle = sweepAngle / segments;
                
                for (int i = 0; i < segments; i++) {
                    float segStart = startAngle + i * segmentAngle;
                    float segEnd = segStart + segmentAngle;
                    
                    if (angle >= segStart && angle <= segEnd) {
                        rl_DrawPixel(center.x + x, center.y + y, color);
                        break;
                    }
                }
            }
        }
    }
}

void rl_DrawCircleSectorLines(Vector2 center, int radius, int startAngle, int endAngle, int segments, uint32_t color) {
    if (segments < 3) segments = 3;
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int dist = x*x + y*y;
            if (abs(dist - radius*radius) < radius) {
                float angle = atan2(y, x) * 180.0f / M_PI;
                if (angle < 0) angle += 360;
                
                float sweepAngle = endAngle - startAngle;
                float segmentAngle = sweepAngle / segments;
                
                for (int i = 0; i < segments; i++) {
                    float segStart = startAngle + i * segmentAngle;
                    float segEnd = segStart + segmentAngle;
                    
                    if (angle >= segStart && angle <= segEnd) {
                        rl_DrawPixel(center.x + x, center.y + y, color);
                        break;
                    }
                }
            }
        }
    }
}

uint32_t rl_ColorLerp(uint32_t start, uint32_t end, float t) {
    uint8_t r1 = (start >> 16) & 0xFF;
    uint8_t g1 = (start >> 8) & 0xFF;
    uint8_t b1 = start & 0xFF;

    uint8_t r2 = (end >> 16) & 0xFF;
    uint8_t g2 = (end >> 8) & 0xFF;
    uint8_t b2 = end & 0xFF;

    uint8_t r = r1 + t * (r2 - r1);
    uint8_t g = g1 + t * (g2 - g1);
    uint8_t b = b1 + t * (b2 - b1);

    return (r << 16) | (g << 8) | b;
}

void rl_DrawCircleGradient(int centerX, int centerY, int radius, uint32_t innerColor, uint32_t outerColor) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int dist = x*x + y*y;
            if (dist <= radius*radius) {
                float t = sqrt((float)dist) / radius;
                uint32_t color = rl_ColorLerp(innerColor, outerColor, t);
                rl_DrawPixel(centerX + x, centerY + y, color);
            }
        }
    }
}

void rl_DrawEllipse(int centerX, int centerY, int radiusH, int radiusV, uint32_t color) {
    for (int y = -radiusV; y <= radiusV; y++) {
        for (int x = -radiusH; x <= radiusH; x++) {
            float eqX = (float)x * x / (radiusH * radiusH);
            float eqY = (float)y * y / (radiusV * radiusV);
            if (eqX + eqY <= 1.0f) {
                rl_DrawPixel(centerX + x, centerY + y, color);
            }
        }
    }
}

void rl_DrawEllipseLines(int centerX, int centerY, int radiusH, int radiusV, uint32_t color) {
    for (int y = -radiusV; y <= radiusV; y++) {
        for (int x = -radiusH; x <= radiusH; x++) {
            float eqX = (float)x * x / (radiusH * radiusH);
            float eqY = (float)y * y / (radiusV * radiusV);
            
            if (fabsf(eqX + eqY - 1.0f) < 0.15f) {
                rl_DrawPixel(centerX + x, centerY + y, color);
            }
        }
    }
}

void rl_DrawRing(Vector2 center, int innerRadius, int outerRadius, int startAngle, int endAngle, int segments, uint32_t color) {
    if (segments < 3) segments = 3;
    
    for (int y = -outerRadius; y <= outerRadius; y++) {
        for (int x = -outerRadius; x <= outerRadius; x++) {
            int distSquared = x*x + y*y;
            
            if (distSquared <= outerRadius*outerRadius && distSquared >= innerRadius*innerRadius) {
                float angle = atan2(y, x) * 180.0f / M_PI;
                if (angle < 0) angle += 360;
                
                float sweepAngle = endAngle - startAngle;
                float segmentAngle = sweepAngle / segments;
                
                for (int i = 0; i < segments; i++) {
                    float segStart = startAngle + i * segmentAngle;
                    float segEnd = segStart + segmentAngle;
                    
                    if (angle >= segStart && angle <= segEnd) {
                        rl_DrawPixel(center.x + x, center.y + y, color);
                        break;
                    }
                }
            }
        }
    }
}

void rl_DrawRingLines(Vector2 center, int innerRadius, int outerRadius, int startAngle, int endAngle, int segments, uint32_t color) {
    if (segments < 3) segments = 3;
    
    for (int y = -outerRadius; y <= outerRadius; y++) {
        for (int x = -outerRadius; x <= outerRadius; x++) {
            int distSquared = x*x + y*y;
            
            if ((abs(distSquared - outerRadius*outerRadius) < outerRadius) || 
                (abs(distSquared - innerRadius*innerRadius) < innerRadius)) {
                float angle = atan2(y, x) * 180.0f / M_PI;
                if (angle < 0) angle += 360;
                
                float sweepAngle = endAngle - startAngle;
                float segmentAngle = sweepAngle / segments;
                
                for (int i = 0; i < segments; i++) {
                    float segStart = startAngle + i * segmentAngle;
                    float segEnd = segStart + segmentAngle;
                    
                    if (angle >= segStart && angle <= segEnd) {
                        rl_DrawPixel(center.x + x, center.y + y, color);
                        break;
                    }
                }
            }
        }
    }
}

void rl_DrawRingIsoTrap(Vector2 center, int innerRadius, int outerRadius, int startAngle, int endAngle, int segments, uint32_t color) {
    if (segments < 3) segments = 3;

    float angleStep = (float)(endAngle - startAngle) / segments;

    for (int i = 0; i < segments; i++) {
        float angleStart = startAngle + i * angleStep;
        float angleEnd = angleStart + angleStep;

        Vector2 p1 = {center.x + cosf(angleStart * M_PI / 180.0f) * innerRadius, center.y + sinf(angleStart * M_PI / 180.0f) * innerRadius};
        Vector2 p2 = {center.x + cosf(angleStart * M_PI / 180.0f) * outerRadius, center.y + sinf(angleStart * M_PI / 180.0f) * outerRadius};
        Vector2 p3 = {center.x + cosf(angleEnd * M_PI / 180.0f) * outerRadius, center.y + sinf(angleEnd * M_PI / 180.0f) * outerRadius};
        Vector2 p4 = {center.x + cosf(angleEnd * M_PI / 180.0f) * innerRadius, center.y + sinf(angleEnd * M_PI / 180.0f) * innerRadius};

        rl_DrawLineV(p1, p2, color);
        rl_DrawLineV(p2, p3, color);
        rl_DrawLineV(p3, p4, color);
        rl_DrawLineV(p4, p1, color);
    }
}

void rl_DrawRingStriped(Vector2 center, int innerRadius, int outerRadius, int startAngle, int endAngle, int segments, uint32_t color) {
    if (segments < 3) segments = 3;

    float angleStep = (float)(endAngle - startAngle) / segments;

    for (int i = 0; i <= segments; i++) {
        float angle = startAngle + i * angleStep;

        Vector2 innerPoint = {center.x + cosf(angle * M_PI / 180.0f) * innerRadius, center.y + sinf(angle * M_PI / 180.0f) * innerRadius};
        Vector2 outerPoint = {center.x + cosf(angle * M_PI / 180.0f) * outerRadius, center.y + sinf(angle * M_PI / 180.0f) * outerRadius};

        rl_DrawLineV(innerPoint, outerPoint, color);
    }
}
#endif // FENSTERRL_SHAPES_H
