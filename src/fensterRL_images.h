#ifndef FENSTERRL_IMAGES_H
#define FENSTERRL_IMAGES_H

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

Image* rl_LoadImageFromScreenRec(Rectangle area) {
    int startX = (area.x < 0) ? 0 : area.x;
    int startY = (area.y < 0) ? 0 : area.y;
    int endX = (area.x + area.width > fenster.width) ? fenster.width : area.x + area.width;
    int endY = (area.y + area.height > fenster.height) ? fenster.height : area.y + area.height;

    int rec_width = endX - startX;
    int rec_height = endY - startY;

    Image* image_rec = (Image*)malloc(sizeof(Image));
    if (!image_rec) return NULL;

    image_rec->width = rec_width;
    image_rec->height = rec_height;
    image_rec->pixels = (uint32_t*)malloc(rec_width * rec_height * sizeof(uint32_t));
    if (!image_rec->pixels) {
        free(image_rec);
        return NULL;
    }

    for (int y = startY; y < endY; y++) {
        size_t buffer_offset = y * fenster.width + startX;
        size_t image_offset = (y - startY) * rec_width;

        rl_LinearBufferFill_internal(
            image_rec->pixels + image_offset,
            fenster.buffer + buffer_offset,
            rec_width,
            false
        );
    }

    return image_rec;
}

Image* rl_LoadImageFromScreen(void) {
    Rectangle area = { 0, 0, fenster.width, fenster.height };
    return rl_LoadImageFromScreenRec(area);
}

Image* rl_LoadImageFromScreenV(Vector2 position, int width, int height) {
    Rectangle area = { position.x, position.y, width, height };
    return rl_LoadImageFromScreenRec(area);
}

Image* rl_LoadImageFromScreenVS(Vector2 position, Size2 size) {
    Rectangle area = { position.x, position.y, size.width, size.height };
    return rl_LoadImageFromScreenRec(area);
}

Image* rl_LoadImage(const char* filePath) {
    int width, height, channels;
    unsigned char* image = stbi_load(filePath, &width, &height, &channels, 0);
    if (image == NULL) return NULL;
    
    Image* image_rec = (Image*)malloc(sizeof(Image));
    if (!image_rec) {
        stbi_image_free(image);
        return NULL;
    }
    
    image_rec->width = width;
    image_rec->height = height;
    image_rec->pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (!image_rec->pixels) {
        stbi_image_free(image);
        free(image_rec);
        return NULL;
    }
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char r = image[(y * width + x) * channels + 0];
            unsigned char g = image[(y * width + x) * channels + 1];
            unsigned char b = image[(y * width + x) * channels + 2];
            
            if (channels == 4) {
                unsigned char a = image[(y * width + x) * channels + 3];
                
                if (a == 0) {
                    image_rec->pixels[y * width + x] = 0xFFFFFFFF;
                    continue;
                }
            }
            
            uint32_t pixel = (r << 16) | (g << 8) | b;
            image_rec->pixels[y * width + x] = pixel;
        }
    }
    
    stbi_image_free(image);
    return image_rec;
}

void rl_DrawImageEx(Image* image, Vector2 position, float rotation, float scale) {
    // Skip drawing if image is NULL
    if (!image || !image->pixels) return;

    // Calculate scaled dimensions
    int scaledWidth = (int)(image->width * scale);
    int scaledHeight = (int)(image->height * scale);

    // Precompute rotation values
    float cosRotation = cosf(rotation);
    float sinRotation = sinf(rotation);

    // Iterate through each pixel of the source image
    for (int y = 0; y < scaledHeight; y++) {
        for (int x = 0; x < scaledWidth; x++) {
            // Apply rotation and scaling
            float rotatedX = (x - scaledWidth / 2.0f) * cosRotation - 
                             (y - scaledHeight / 2.0f) * sinRotation;
            float rotatedY = (x - scaledWidth / 2.0f) * sinRotation + 
                             (y - scaledHeight / 2.0f) * cosRotation;

            // Map back to original image coordinates
            int srcX = (int)((rotatedX / scale) + image->width / 2.0f);
            int srcY = (int)((rotatedY / scale) + image->height / 2.0f);

            // Check if pixel is within source image bounds
            if (srcX >= 0 && srcX < image->width && 
                srcY >= 0 && srcY < image->height) {
                
                // Calculate destination screen coordinates
                int destX = position.x + x;
                int destY = position.y + y;

                // Check if destination is within screen bounds
                if (destX >= 0 && destX < fenster.width && 
                    destY >= 0 && destY < fenster.height) {
                    
                    // Get source pixel
                    uint32_t sourcePixel = image->pixels[srcY * image->width + srcX];

                    // Calculate destination buffer offset
                    size_t destOffset = destY * fenster.width + destX;

                    // Copy pixel with alpha blending or direct copy
                    // Assuming rl_LinearBufferFill_internal handles alpha blending
                    rl_LinearBufferFill_internal(
                        fenster.buffer + destOffset,
                        &sourcePixel,
                        1,
                        true  // Enable alpha blending
                    );
                }
            }
        }
    }
}

void rl_UnloadImage (Image* image) {
    if (image) {
        free(image->pixels);
        free(image);
    }
}

#endif //FENSTERRL_IMAGES_H
