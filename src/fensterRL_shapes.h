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

void rl_DrawRectangle(int posX, int posY, int width, int height, uint32_t color) {
    for (int y = posY; y < posY + height; y++) {
        for (int x = posX; x < posX + width; x++) {
            rl_DrawPixel(x, y, color);
        }
    }
}

void rl_DrawRectangleV(Vector2 position, Vector2 size, uint32_t color) {
    rl_DrawRectangle(position.x, position.y, size.x, size.y, color);
}

void rl_DrawRectangleRec(Rectangle rec, uint32_t color) {
    rl_DrawRectangle(rec.x, rec.y, rec.width, rec.height, color);
}

void rl_DrawRectangleGradientV(int posX, int posY, int width, int height, uint32_t topColor, uint32_t bottomColor) {
    for (int y = 0; y < height; y++) {
        float t = (float)y / height;
        uint32_t interpolatedColor = rl_ColorLerp(topColor, bottomColor, t);
        for (int x = 0; x < width; x++) {
            rl_DrawPixel(posX + x, posY + y, interpolatedColor);
        }
    }
}

void rl_DrawRectangleGradientH(int posX, int posY, int width, int height, uint32_t leftColor, uint32_t rightColor) {
    for (int x = 0; x < width; x++) {
        float t = (float)x / width;
        uint32_t interpolatedColor = rl_ColorLerp(leftColor, rightColor, t);
        for (int y = 0; y < height; y++) {
            rl_DrawPixel(posX + x, posY + y, interpolatedColor);
        }
    }
}

void rl_DrawRectangleGradientEx(Rectangle rec, uint32_t topLeftColor, uint32_t bottomLeftColor, 
                                 uint32_t topRightColor, uint32_t bottomRightColor) {
    for (int y = 0; y < rec.height; y++) {
        float yLerp = (float)y / rec.height;
        
        // Interpolate left and right colors for this row
        uint32_t leftRowColor = rl_ColorLerp(topLeftColor, bottomLeftColor, yLerp);
        uint32_t rightRowColor = rl_ColorLerp(topRightColor, bottomRightColor, yLerp);
        
        for (int x = 0; x < rec.width; x++) {
            float xLerp = (float)x / rec.width;
            uint32_t pixelColor = rl_ColorLerp(leftRowColor, rightRowColor, xLerp);
            rl_DrawPixel(rec.x + x, rec.y + y, pixelColor);
        }
    }
}

void rl_DrawRectangleLines(int posX, int posY, int width, int height, uint32_t color) {
    rl_DrawLine(posX, posY, posX + width, posY, color);
    rl_DrawLine(posX, posY + height, posX + width, posY + height, color);
    
    rl_DrawLine(posX, posY, posX, posY + height, color);
    rl_DrawLine(posX + width, posY, posX + width, posY + height, color);
}

void rl_DrawRectangleLinesEx(Rectangle rec, int lineThick, uint32_t color) {
    for (int i = 0; i < lineThick; i++) {
        rl_DrawRectangleLines(
            rec.x + i, 
            rec.y + i, 
            rec.width - 2*i, 
            rec.height - 2*i, 
            color
        );
    }
}

void rl_DrawRectangleRounded(Rectangle rec, float roundness, int segments, uint32_t color) {
    // Ensure roundness is between 0 and 1
    roundness = roundness > 1.0f ? 1.0f : (roundness < 0.0f ? 0.0f : roundness);
    
    // Calculate corner radius based on smallest dimension
    int cornerRadius = (int)((rec.width < rec.height ? rec.width : rec.height) * roundness * 0.5f);
    
    // If roundness is too small, draw a regular rectangle
    if (cornerRadius <= 0) {
        rl_DrawRectangleRec(rec, color);
        return;
    }
    
    // Center points for each corner
    Vector2 topLeftCenter = {rec.x + cornerRadius, rec.y + cornerRadius};
    Vector2 topRightCenter = {rec.x + rec.width - cornerRadius, rec.y + cornerRadius};
    Vector2 bottomRightCenter = {rec.x + rec.width - cornerRadius, rec.y + rec.height - cornerRadius};
    Vector2 bottomLeftCenter = {rec.x + cornerRadius, rec.y + rec.height - cornerRadius};
    
    // Fill the central rectangular area
    rl_DrawRectangle(
        rec.x + cornerRadius, 
        rec.y, 
        rec.width - 2 * cornerRadius, 
        rec.height, 
        color
    );
    rl_DrawRectangle(
        rec.x, 
        rec.y + cornerRadius, 
        rec.width, 
        rec.height - 2 * cornerRadius, 
        color
    );
    
    // Draw filled circular sectors for corners
    rl_DrawCircleSector((Vector2){topLeftCenter.x, topLeftCenter.y}, cornerRadius, 180, 270, segments, color);
    rl_DrawCircleSector((Vector2){topRightCenter.x, topRightCenter.y}, cornerRadius, 270, 360, segments, color);
    rl_DrawCircleSector((Vector2){bottomRightCenter.x, bottomRightCenter.y}, cornerRadius, 0, 90, segments, color);
    rl_DrawCircleSector((Vector2){bottomLeftCenter.x, bottomLeftCenter.y}, cornerRadius, 90, 180, segments, color);
}

void rl_DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, uint32_t color) {
    // Ensure roundness is between 0 and 1
    roundness = roundness > 1.0f ? 1.0f : (roundness < 0.0f ? 0.0f : roundness);
    
    // Calculate corner radius based on smallest dimension
    int cornerRadius = (int)((rec.width < rec.height ? rec.width : rec.height) * roundness * 0.5f);
    
    // If roundness is too small, draw a regular rectangle outline
    if (cornerRadius <= 0) {
        rl_DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, color);
        return;
    }
    
    // Center points for each corner
    Vector2 topLeftCenter = {rec.x + cornerRadius, rec.y + cornerRadius};
    Vector2 topRightCenter = {rec.x + rec.width - cornerRadius, rec.y + cornerRadius};
    Vector2 bottomRightCenter = {rec.x + rec.width - cornerRadius, rec.y + rec.height - cornerRadius};
    Vector2 bottomLeftCenter = {rec.x + cornerRadius, rec.y + rec.height - cornerRadius};
    
    // Draw corner arc outlines
    rl_DrawCircleSectorLines((Vector2){topLeftCenter.x, topLeftCenter.y}, cornerRadius, 180, 270, segments, color);
    rl_DrawCircleSectorLines((Vector2){topRightCenter.x, topRightCenter.y}, cornerRadius, 270, 360, segments, color);
    rl_DrawCircleSectorLines((Vector2){bottomRightCenter.x, bottomRightCenter.y}, cornerRadius, 0, 90, segments, color);
    rl_DrawCircleSectorLines((Vector2){bottomLeftCenter.x, bottomLeftCenter.y}, cornerRadius, 90, 180, segments, color);
    
    // Draw connecting lines between arcs
    rl_DrawLine(
        rec.x + cornerRadius, 
        rec.y, 
        rec.x + rec.width - cornerRadius, 
        rec.y, 
        color
    );
    rl_DrawLine(
        rec.x + rec.width, 
        rec.y + cornerRadius, 
        rec.x + rec.width, 
        rec.y + rec.height - cornerRadius, 
        color
    );
    rl_DrawLine(
        rec.x + cornerRadius, 
        rec.y + rec.height, 
        rec.x + rec.width - cornerRadius, 
        rec.y + rec.height, 
        color
    );
    rl_DrawLine(
        rec.x, 
        rec.y + cornerRadius, 
        rec.x, 
        rec.y + rec.height - cornerRadius, 
        color
    );
}

void rl_DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, int lineThick, uint32_t color) {
    // Ensure roundness is between 0 and 1
    roundness = roundness > 1.0f ? 1.0f : (roundness < 0.0f ? 0.0f : roundness);
    
    // Calculate corner radius based on smallest dimension
    int cornerRadius = (int)((rec.width < rec.height ? rec.width : rec.height) * roundness * 0.5f);
    
    // If roundness is too small, draw a regular rectangle outline
    if (cornerRadius <= 0) {
        rl_DrawRectangleLinesEx(rec, lineThick, color);
        return;
    }
    
    // Center points for each corner
    Vector2 topLeftCenter = {rec.x + cornerRadius, rec.y + cornerRadius};
    Vector2 topRightCenter = {rec.x + rec.width - cornerRadius, rec.y + cornerRadius};
    Vector2 bottomRightCenter = {rec.x + rec.width - cornerRadius, rec.y + rec.height - cornerRadius};
    Vector2 bottomLeftCenter = {rec.x + cornerRadius, rec.y + rec.height - cornerRadius};
    
    // Draw corner arc outlines with thickness
    for (int i = 0; i < lineThick; i++) {
        rl_DrawCircleSectorLines(
            (Vector2){topLeftCenter.x, topLeftCenter.y}, 
            cornerRadius - i, 180, 270, segments, color
        );
        rl_DrawCircleSectorLines(
            (Vector2){topRightCenter.x, topRightCenter.y}, 
            cornerRadius - i, 270, 360, segments, color
        );
        rl_DrawCircleSectorLines(
            (Vector2){bottomRightCenter.x, bottomRightCenter.y}, 
            cornerRadius - i, 0, 90, segments, color
        );
        rl_DrawCircleSectorLines(
            (Vector2){bottomLeftCenter.x, bottomLeftCenter.y}, 
            cornerRadius - i, 90, 180, segments, color
        );
    }
    
    // Draw connecting lines with thickness
    for (int i = 0; i < lineThick; i++) {
        rl_DrawLineEx(
            (Vector2){rec.x + cornerRadius, rec.y + i}, 
            (Vector2){rec.x + rec.width - cornerRadius, rec.y + i}, 
            1, color
        );
        rl_DrawLineEx(
            (Vector2){rec.x + rec.width - i, rec.y + cornerRadius}, 
            (Vector2){rec.x + rec.width - i, rec.y + rec.height - cornerRadius}, 
            1, color
        );
        rl_DrawLineEx(
            (Vector2){rec.x + cornerRadius, rec.y + rec.height - i}, 
            (Vector2){rec.x + rec.width - cornerRadius, rec.y + rec.height - i}, 
            1, color
        );
        rl_DrawLineEx(
            (Vector2){rec.x + i, rec.y + cornerRadius}, 
            (Vector2){rec.x + i, rec.y + rec.height - cornerRadius}, 
            1, color
        );
    }
}

void rl_DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, uint32_t color) {
    if (rotation == 0.0f) {
        rl_DrawRectangle(rec.x, rec.y, rec.width, rec.height, color);
        return;
    }

    float radian = rotation * (M_PI / 180.0f);
    float sinRotation = sinf(radian);
    float cosRotation = cosf(radian);

    float centerX = rec.x + rec.width / 2.0f;
    float centerY = rec.y + rec.height / 2.0f;

    Vector2 topLeft, topRight, bottomLeft, bottomRight;

    float dx = -origin.x;
    float dy = -origin.y;

    topLeft.x = centerX + (dx) * cosRotation - (dy) * sinRotation;
    topLeft.y = centerY + (dx) * sinRotation + (dy) * cosRotation;

    topRight.x = centerX + (dx + rec.width) * cosRotation - (dy) * sinRotation;
    topRight.y = centerY + (dx + rec.width) * sinRotation + (dy) * cosRotation;

    bottomLeft.x = centerX + (dx) * cosRotation - (dy + rec.height) * sinRotation;
    bottomLeft.y = centerY + (dx) * sinRotation + (dy + rec.height) * cosRotation;

    bottomRight.x = centerX + (dx + rec.width) * cosRotation - (dy + rec.height) * sinRotation;
    bottomRight.y = centerY + (dx + rec.width) * sinRotation + (dy + rec.height) * cosRotation;

    int minX = fminf(fminf(topLeft.x, topRight.x), fminf(bottomLeft.x, bottomRight.x));
    int maxX = fmaxf(fmaxf(topLeft.x, topRight.x), fmaxf(bottomLeft.x, bottomRight.x));
    int minY = fminf(fminf(topLeft.y, topRight.y), fminf(bottomLeft.y, bottomRight.y));
    int maxY = fmaxf(fmaxf(topLeft.y, topRight.y), fmaxf(bottomLeft.y, bottomRight.y));

    for (int py = minY; py <= maxY; py++) {
        for (int px = minX; px <= maxX; px++) {
            int inside = 0;
            Vector2 point = {px, py};

            for (int i = 0, j = 3; i < 4; j = i++) {
                Vector2 vi = (i == 0) ? topLeft : (i == 1) ? topRight : (i == 2) ? bottomRight : bottomLeft;
                Vector2 vj = (j == 0) ? topLeft : (j == 1) ? topRight : (j == 2) ? bottomRight : bottomLeft;

                if (((vi.y > point.y) != (vj.y > point.y)) &&
                    (point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x)) {
                    inside = !inside;
                }
            }

            if (inside) {
                rl_DrawPixel(px, py, color);
            }
        }
    }
}

#endif // FENSTERRL_SHAPES_H

