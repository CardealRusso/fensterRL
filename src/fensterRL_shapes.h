#ifndef FENSTERRL_SHAPES_H
#define FENSTERRL_SHAPES_H

#include <math.h>

#if defined(USE_SIMD)
    #if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
        #include <immintrin.h>
        #define USE_AVX2
        #define VECTOR_SIZE 8
    #elif defined(__arm64__) || defined(__aarch64__)
        #include <arm_neon.h>
        #define USE_NEON
        #define VECTOR_SIZE 4
    #endif
#endif

static inline void rl_DrawPixel(int posX, int posY, uint32_t color) {
    if (posX >= 0 && posX < fenster.width && posY >= 0 && posY < fenster.height) {
        fenster.buffer[posY * fenster.width + posX] = color;
    }
}

static inline void rl_DrawPixelV(Vector2 position, uint32_t color) {
    rl_DrawPixel(position.x, position.y, color);
}

static void rl_LinearBufferFill_internal(
    uint32_t* destination, 
    const uint32_t* source, 
    size_t count, 
    bool is_single_color
) {
    if (count == 0) return;
    
    #if defined(USE_SIMD)
        size_t vec_count = count / VECTOR_SIZE;
        size_t remainder = count % VECTOR_SIZE;
  
        #if defined(USE_AVX2)
            __m256i transparent_vec = _mm256_set1_epi32(0xFFFFFFFF);
            
            for (size_t i = 0; i < vec_count; i++) {
                __m256i *dest_vec = (__m256i*)(destination + i * VECTOR_SIZE);
                __m256i *src_vec = (__m256i*)(source + i * VECTOR_SIZE);
                
                if (is_single_color) {
                    __m256i color_vec = _mm256_set1_epi32(source[0]);
                    __m256i mask = _mm256_cmpeq_epi32(color_vec, transparent_vec);
                    color_vec = _mm256_andnot_si256(mask, color_vec);
                    _mm256_storeu_si256(dest_vec, color_vec);
                } else {
                    __m256i masked_src = _mm256_andnot_si256(
                        _mm256_cmpeq_epi32(*src_vec, transparent_vec), 
                        *src_vec
                    );
                    _mm256_storeu_si256(dest_vec, masked_src);
                }
            }
        #elif defined(USE_NEON)
            uint32x4_t transparent_vec = vdupq_n_u32(0xFFFFFFFF);
            
            for (size_t i = 0; i < vec_count; i++) {
                if (is_single_color) {
                    uint32x4_t color_vec = vdupq_n_u32(source[0]);
                    uint32x4_t mask = vceqq_u32(color_vec, transparent_vec);
                    uint32x4_t masked_color = vbicq_u32(color_vec, mask);
                    
                    vst1q_u32(destination + i * 4, masked_color);
                } else {
                    uint32x4_t src_vec = vld1q_u32(source + i * 4);
                    uint32x4_t mask = vceqq_u32(src_vec, transparent_vec);
                    uint32x4_t masked_src = vbicq_u32(src_vec, mask);
                    
                    vst1q_u32(destination + i * 4, masked_src);
                }
            }
        #endif
        
        uint32_t *remainder_ptr_dest = destination + (vec_count * VECTOR_SIZE);
        const uint32_t *remainder_ptr_src = source + (vec_count * VECTOR_SIZE);
        
        for (size_t i = 0; i < remainder; i++) {
            remainder_ptr_dest[i] = is_single_color ? source[0] : remainder_ptr_src[i];
            
            if (remainder_ptr_dest[i] == 0xFFFFFFFF) {
                remainder_ptr_dest[i] = 0;
            }
        }
    #else
        for (size_t i = 0; i < count; i++) {
            if (is_single_color && source[0] != 0xFFFFFFFF) {
                destination[i] = source[0];
            } else if (!is_single_color) {
                if (source[i] != 0xFFFFFFFF) {
                    destination[i] = source[i];
                }
            }
        }
    #endif
}


static void rl_LinearBufferFill(size_t offset, size_t count, uint32_t color) {
    if (count == 0) {
        return;
    } else {
        rl_LinearBufferFill_internal(
            fenster.buffer + offset, 
            &color, 
            count, 
            true
        );
    }
}

void rl_DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, uint32_t color) {
    if (startPosY == endPosY) {
        if (startPosY < 0 || startPosY >= fenster.height || startPosX >= fenster.width) return;

        startPosX = (startPosX < 0) ? 0 : startPosX;
        endPosX = (endPosX >= fenster.width) ? fenster.width - 1 : endPosX;
        
        if (startPosX > endPosX) return;
        
        size_t offset = startPosY * fenster.width + startPosX;
        size_t count = endPosX - startPosX + 1;
        if (color == 0) {
            memset(fenster.buffer + offset, 0, count * sizeof(uint32_t));
        } else {
            rl_LinearBufferFill(offset, count, color);
        }
    } else {
        int dx = abs(endPosX - startPosX);
        int dy = abs(endPosY - startPosY);
        int sx = (startPosX < endPosX) ? 1 : -1;
        int sy = (startPosY < endPosY) ? 1 : -1;

        int err = (dx > dy ? dx : -dy) / 2;
        int e2;

        int x = startPosX;
        int y = startPosY;

        while (1) {
            rl_DrawPixel(x, y, color);

            if (x == endPosX && y == endPosY) break;

            e2 = err;
            if (e2 > -dx) { 
                err -= dy; 
                x += sx; 
            }
            if (e2 < dy) { 
                err += dx; 
                y += sy; 
            }
        }
    }
}

void rl_DrawPolyPoints(const Vector2 *points, int pointCount, uint32_t color) {
    if (points == NULL || pointCount < 3) return;

    int minY = points[0].y;
    int maxY = points[0].y;
    for (int i = 1; i < pointCount; i++) {
        minY = (points[i].y < minY) ? points[i].y : minY;
        maxY = (points[i].y > maxY) ? points[i].y : maxY;
    }

    for (int y = minY; y <= maxY; y++) {
        int intersectX[64];
        int intersectCount = 0;

        for (int i = 0; i < pointCount; i++) {
            int j = (i + 1) % pointCount;
            Vector2 p1 = points[i];
            Vector2 p2 = points[j];

            if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y)) {
                float slope = (float)(p2.x - p1.x) / (p2.y - p1.y);
                int x = p1.x + (int)((y - p1.y) * slope);
                
                intersectX[intersectCount++] = x;
            }
        }

        for (int i = 0; i < intersectCount - 1; i++) {
            for (int j = 0; j < intersectCount - i - 1; j++) {
                if (intersectX[j] > intersectX[j+1]) {
                    int temp = intersectX[j];
                    intersectX[j] = intersectX[j+1];
                    intersectX[j+1] = temp;
                }
            }
        }

        for (int i = 0; i < intersectCount; i += 2) {
            if (i + 1 < intersectCount) {
                rl_DrawLine(intersectX[i], y, intersectX[i+1], y, color);
            }
        }
    }
}

void rl_DrawPolyLinePoints(const Vector2 *points, int pointCount, uint32_t color) {
    if (points == NULL || pointCount < 3) return;

    for (int i = 0; i < pointCount; i++) {
        int next = (i + 1) % pointCount;
        Vector2 start = points[i];
        Vector2 end = points[next];

        rl_DrawLine(start.x, start.y, end.x, end.y, color);
    }
}

void rl_DrawPoly(Vector2 center, int sides, float radius, float rotation, uint32_t color) {
    if (sides < 3) return;

    Vector2 *points = malloc(sides * sizeof(Vector2));
    if (!points) return;

    for (int i = 0; i < sides; i++) {
        float angle = rotation + (2 * M_PI * i / sides);
        points[i].x = center.x + radius * cos(angle);
        points[i].y = center.y + radius * sin(angle);
    }

    rl_DrawPolyPoints(points, sides, color);

    free(points);
}

void rl_DrawPolyLines(Vector2 center, int sides, float radius, float rotation, uint32_t color) {
    if (sides < 3) return;

    Vector2 *points = malloc(sides * sizeof(Vector2));
    if (!points) return;

    for (int i = 0; i < sides; i++) {
        float angle = rotation + (2 * M_PI * i / sides);
        points[i].x = center.x + radius * cos(angle);
        points[i].y = center.y + radius * sin(angle);
    }

    rl_DrawPolyLinePoints(points, sides, color);

    free(points);
}

void rl_DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float lineThick, uint32_t color) {
    if (sides < 3 || lineThick <= 0) return;

    for (float offset = -lineThick/2; offset <= lineThick/2; offset += 1.0f) {
        Vector2 *points = malloc(sides * sizeof(Vector2));
        if (!points) return;

        for (int i = 0; i < sides; i++) {
            float angle = rotation + (2 * M_PI * i / sides);
            float adjustedRadius = radius + offset;
            points[i].x = center.x + adjustedRadius * cos(angle);
            points[i].y = center.y + adjustedRadius * sin(angle);
        }

        rl_DrawPolyLinePoints(points, sides, color);

        free(points);
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

void rl_ClearBackground(uint32_t color) {
    const size_t total_pixels = (size_t)fenster.width * fenster.height;
    rl_LinearBufferFill(0, total_pixels, color);
}

void rl_DrawRectangle(int posX, int posY, int width, int height, uint32_t color) {
    for (int y = posY; y < posY + height; y++) {
        rl_DrawLine(posX, y, posX + width - 1, y, color);
    }
}

void rl_DrawRectangleV(Vector2 position, int width, int height, uint32_t color) {
    rl_DrawRectangle(position.x, position.y, width, height, color);
}

void rl_DrawRectangleVS(Vector2 position, Size2 size, uint32_t color) {
    rl_DrawRectangle(position.x, position.y, size.width, size.height, color);
}

void rl_DrawRectangleRec(Rectangle rec, uint32_t color) {
    rl_DrawRectangle(rec.x, rec.y, rec.width, rec.height, color);
}

void rl_DrawRectangleLines(int posX, int posY, int width, int height, uint32_t color) {
    rl_DrawLine(posX, posY, posX + width - 1, posY, color);
    rl_DrawLine(posX, posY + height - 1, posX + width - 1, posY + height - 1, color);
    rl_DrawLine(posX, posY, posX, posY + height - 1, color);
    rl_DrawLine(posX + width - 1, posY, posX + width - 1, posY + height - 1, color);
}

void rl_DrawRectangleLinesV(Vector2 position, int width, int height, uint32_t color) {
    rl_DrawRectangleLines(position.x, position.y, width, height, color);
}

void rl_DrawRectangleLinesVS(Vector2 position, Size2 size, uint32_t color) {
    rl_DrawRectangleLines(position.x, position.y, size.width, size.height, color);
}

void rl_DrawRectangleLinesRec(Rectangle rec, uint32_t color) {
    rl_DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, color);
}

void rl_DrawRectangleLinesEx(Rectangle rec, float lineThick, uint32_t color) {
    rl_DrawLineEx((Vector2){rec.x, rec.y}, (Vector2){rec.x + rec.width - 1, rec.y}, lineThick, color);
    rl_DrawLineEx((Vector2){rec.x, rec.y + rec.height - 1}, (Vector2){rec.x + rec.width - 1, rec.y + rec.height - 1}, lineThick, color);
    rl_DrawLineEx((Vector2){rec.x, rec.y}, (Vector2){rec.x, rec.y + rec.height - 1}, lineThick, color);
    rl_DrawLineEx((Vector2){rec.x + rec.width - 1, rec.y}, (Vector2){rec.x + rec.width - 1, rec.y + rec.height - 1}, lineThick, color);
}

void rl_DrawCircle(int centerX, int centerY, int radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        int width = sqrt(radius * radius - y * y);
        rl_DrawLine(centerX - width, centerY + y, centerX + width, centerY + y, color);
    }
}

void rl_DrawCircleV(Vector2 center, int radius, uint32_t color) {
    rl_DrawCircle(center.x, center.y, radius, color);
}

bool rl_CheckCollisionRecs(Rectangle rec1, Rectangle rec2) {
    return (rec1.x < rec2.x + rec2.width &&
            rec1.x + rec1.width > rec2.x &&
            rec1.y < rec2.y + rec2.height &&
            rec1.y + rec1.height > rec2.y);
}

Rectangle rl_GetCollisionRec(Rectangle rec1, Rectangle rec2) {
    float left = fmaxf(rec1.x, rec2.x);
    float top = fmaxf(rec1.y, rec2.y);
    float right = fminf(rec1.x + rec1.width, rec2.x + rec2.width);
    float bottom = fminf(rec1.y + rec1.height, rec2.y + rec2.height);

    if (right > left && bottom > top) {
        return (Rectangle){
            left,
            top,
            right - left,
            bottom - top
        };
    }

    return (Rectangle){ 0, 0, 0, 0 };
}
#endif // FENSTERRL_SHAPES_H
