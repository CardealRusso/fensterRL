#ifndef FENSTERRL_SHAPES_H
#define FENSTERRL_SHAPES_H

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

static void rl_LinearBufferFill(size_t offset, size_t count, uint32_t color) {
    if (count == 0) return;
    if (color == 0) {
        memset(fenster.buffer + offset, 0, count * sizeof(uint32_t));
        return;
    }
    #if defined(USE_SIMD)
        size_t vec_count = count / VECTOR_SIZE;
        size_t remainder = count % VECTOR_SIZE;
  
        #if defined(USE_AVX2)
            __m256i color_vec = _mm256_set1_epi32(color);
            __m256i *buf_vec = (__m256i*)(fenster.buffer + offset);
            for (size_t i = 0; i < vec_count; i++) {
                _mm256_storeu_si256(&buf_vec[i], color_vec);
            }
        #elif defined(USE_NEON)
            uint32x4_t color_vec = vdupq_n_u32(color);
            for (size_t i = 0; i < vec_count; i++) {
                vst1q_u32(fenster.buffer + offset + i * 4, color_vec);
            }
        #endif

        uint32_t *remainder_ptr = fenster.buffer + offset + (vec_count * VECTOR_SIZE);
        for (size_t i = 0; i < remainder; i++) {
            remainder_ptr[i] = color;
        }
    #else
        for (size_t i = 0; i < count; i++) {
            fenster.buffer[offset + i] = color;
        }
    #endif
}

void rl_DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, uint32_t color) {
    if (startPosY == endPosY) {
        startPosX = (startPosX < 0) ? 0 : startPosX;
        endPosX = (endPosX >= fenster.width) ? fenster.width - 1 : endPosX;
        
        if (startPosX > endPosX) return;
        
        size_t offset = startPosY * fenster.width + startPosX;
        size_t count = endPosX - startPosX + 1;
        
        rl_LinearBufferFill(offset, count, color);
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

void rl_ClearBackground(uint32_t color) {
    const size_t total_pixels = (size_t)fenster.width * fenster.height;
    rl_LinearBufferFill(0, total_pixels, color);
}
#endif // FENSTERRL_SHAPES_H
