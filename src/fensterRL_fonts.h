#ifndef FENSTERRL_FONTS_H
#define FENSTERRL_FONTS_H

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#include "external/stb/stb_rect_pack.h"
#include "external/stb/stb_truetype.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_FONT_SIZES 16
#define MAX_CHARS 256
#define MAX_LOADED_FONTS 32

#define MAX_SYSTEM_FONTS 256
#define MAX_PATH_LENGTH 256

#if defined(__linux__) || defined(__APPLE__)
#include <glob.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

static struct {
    char paths[MAX_SYSTEM_FONTS][MAX_PATH_LENGTH];
    const char* pathPtrs[MAX_SYSTEM_FONTS];
    int count;
    int initialized;
} systemFonts = {0};

typedef struct {
    unsigned char* bitmap;
    int width;
    int height;
    int x0;
    int y0;
    int advance;
} CachedGlyph;

typedef struct {
    CachedGlyph* chars[MAX_CHARS];
    float size;
} FontSizeCache;

typedef struct {
    stbtt_fontinfo info;
    unsigned char* buffer;
    FontSizeCache sizes[MAX_FONT_SIZES];
    int num_sizes;
    char* path;
} RLFont;

static struct {
    RLFont* fonts[MAX_LOADED_FONTS];
    int count;
} fontRegistry = {0};

const char** rl_GetSystemFonts(void) {
    // If already initialized, return existing paths
    if (systemFonts.initialized) {
        return systemFonts.pathPtrs;
    }

    // Reset count just in case
    systemFonts.count = 0;

    #if defined(__linux__) || defined(__APPLE__)
  const char *patterns[] = {
    "~/.local/share/fonts/**/*.ttf", "~/.fonts/**/*.ttf",
    "/usr/*/fonts/**/*.ttf", "/usr/*/*/fonts/**/*.ttf",
    "/usr/*/*/*/fonts/**/*.ttf", "/usr/*/*/*/*/fonts/**/*.ttf",
    "/Library/Fonts/**/*.ttf", "/System/Library/Fonts/**/*.ttf"
  };
    
    glob_t glob_res;
    for (size_t i = 0; i < sizeof(patterns)/sizeof(*patterns); i++) {
        if (glob(patterns[i], GLOB_TILDE | GLOB_NOSORT, NULL, &glob_res) == 0) {
            for (size_t j = 0; j < glob_res.gl_pathc && systemFonts.count < MAX_SYSTEM_FONTS; j++) {
                strncpy(systemFonts.paths[systemFonts.count], 
                        glob_res.gl_pathv[j], 
                        MAX_PATH_LENGTH - 1);
                systemFonts.count++;
            }
            globfree(&glob_res);
        }
    }
    #elif defined(_WIN32)
    char base_path[MAX_PATH];
    if (GetEnvironmentVariable("SYSTEMROOT", base_path, MAX_PATH)) {
        strcat(base_path, "\\Fonts");
        char search_path[MAX_PATH];
        sprintf(search_path, "%s\\*.ttf", base_path);
        
        WIN32_FIND_DATA fd;
        HANDLE h = FindFirstFile(search_path, &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                sprintf(systemFonts.paths[systemFonts.count], 
                        "%s\\%s", base_path, fd.cFileName);
                systemFonts.count++;
            } while (FindNextFile(h, &fd) && systemFonts.count < MAX_SYSTEM_FONTS);
            FindClose(h);
        }
    }
    #endif

    for (int i = 0; i < systemFonts.count; i++) {
        systemFonts.pathPtrs[i] = systemFonts.paths[i];
    }
    systemFonts.initialized = 1;

    return systemFonts.count > 0 ? systemFonts.pathPtrs : NULL;
}

static RLFont* find_loaded_font(const char* path) {
    for (int i = 0; i < fontRegistry.count; i++) {
        if (strcmp(fontRegistry.fonts[i]->path, path) == 0) {
            return fontRegistry.fonts[i];
        }
    }
    return NULL;
}

static int get_size_index(RLFont* font, float size) {
    for (int i = 0; i < font->num_sizes; i++) {
        if (font->sizes[i].size == size) {
            return i;
        }
    }
    
    if (font->num_sizes < MAX_FONT_SIZES) {
        int idx = font->num_sizes;
        font->sizes[idx].size = size;
        memset(font->sizes[idx].chars, 0, sizeof(font->sizes[idx].chars));
        font->num_sizes++;
        return idx;
    }
    
    // Overwrite first size cache if max reached
    memset(font->sizes[0].chars, 0, sizeof(font->sizes[0].chars));
    font->sizes[0].size = size;
    return 0;
}

static RLFont* load_font(const char* path) {
    if (fontRegistry.count >= MAX_LOADED_FONTS) {
        return NULL;
    }

    RLFont* font = find_loaded_font(path);
    if (font) return font;

    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    font = malloc(sizeof(RLFont));
    font->buffer = malloc(size);
    font->path = strdup(path);
    font->num_sizes = 0;
    
    if (fread(font->buffer, size, 1, f) != 1 ||
        !stbtt_InitFont(&font->info, font->buffer, 0)) {
        free(font->buffer);
        free(font->path);
        free(font);
        fclose(f);
        return NULL;
    }
    
    fclose(f);
    fontRegistry.fonts[fontRegistry.count++] = font;
    return font;
}

static CachedGlyph* get_cached_glyph(RLFont* font, unsigned char c, float size) {
    int size_idx = get_size_index(font, size);
    CachedGlyph* glyph = font->sizes[size_idx].chars[c];
    
    if (glyph) return glyph;
    
    float scale = stbtt_ScaleForPixelHeight(&font->info, size);
    int advance, lsb, x0, y0, x1, y1;
    stbtt_GetCodepointHMetrics(&font->info, c, &advance, &lsb);
    stbtt_GetCodepointBitmapBox(&font->info, c, scale, scale, &x0, &y0, &x1, &y1);
    
    int width = x1 - x0;
    int height = y1 - y0;
    
    if (width <= 0 || height <= 0) return NULL;
    
    glyph = malloc(sizeof(CachedGlyph));
    glyph->bitmap = malloc(width * height);
    glyph->width = width;
    glyph->height = height;
    glyph->x0 = x0;
    glyph->y0 = y0;
    glyph->advance = advance;
    
    stbtt_MakeCodepointBitmap(&font->info, glyph->bitmap, width, height,
                               width, scale, scale, c);
    
    font->sizes[size_idx].chars[c] = glyph;
    return glyph;
}

char* rl_TextFormat(const char* fmt, ...) {
    static char buffer[TEXT_FORMAT_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, TEXT_FORMAT_BUFFER_SIZE, fmt, args);
    va_end(args);
    return buffer;
}

int rl_MeasureText(const char *text, int fontSize, const char* fontPath) {
    RLFont* font = load_font(fontPath);
    if (!font) return 0;

    float scale = stbtt_ScaleForPixelHeight(&font->info, fontSize);
    int total_width = 0;
    int spaceWidth = fontSize / 3;

    for (const char* p = text; *p; p++) {
        if (*p == ' ') {
            total_width += spaceWidth;
            continue;
        }

        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font->info, *p, &advance, &lsb);
        total_width += advance * scale;
    }

    return total_width;
}

void rl_DrawText(const char* text, int posX, int posY, int fontSize, 
                 const char* fontPath, uint32_t color, uint32_t bgcolor) {
    RLFont* font = load_font(fontPath);
    if (!font) return;

    float scale = stbtt_ScaleForPixelHeight(&font->info, fontSize);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font->info, &ascent, &descent, &lineGap);

    int spaceWidth = fontSize / 3;

    int x = posX;
    int y = posY + ascent * scale;

    for (const char* p = text; *p; p++) {
        if (*p == ' ') {
            x += spaceWidth;
            continue;
        }

        CachedGlyph* glyph = get_cached_glyph(font, *p, fontSize);
        if (!glyph) continue;
        
        for (int i = 0; i < glyph->height; i++) {
            for (int j = 0; j < glyph->width; j++) {
                int px = x + glyph->x0 + j;
                int py = y + glyph->y0 + i;
                if (px >= 0 && px < fenster.width && py >= 0 && py < fenster.height) {
                    if (glyph->bitmap[i * glyph->width + j] > 127) {
                        rl_DrawPixel(px, py, color);
                    } else if (bgcolor != 0xFFFFFFFF) {
                        rl_DrawPixel(px, py, bgcolor);
                    }
                }
            }
        }
        
        x += glyph->advance * scale;
    }
}

#endif // FENSTERRL_FONTS_H
