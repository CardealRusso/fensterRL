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

#if defined(__linux__) || defined(__APPLE__)
#include <glob.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

// Dynamic system fonts structure
typedef struct {
    char** paths;
    const char** pathPtrs;
    int count;
    int capacity;
    int initialized;
} SystemFonts;
static SystemFonts systemFonts = {0};

// Dynamic cached glyph structure
typedef struct {
    unsigned char* bitmap;
    int width;
    int height;
    int x0;
    int y0;
    int advance;
} CachedGlyph;

// Dynamic font size cache
typedef struct {
    CachedGlyph** chars;
    float size;
    int char_capacity;
} FontSizeCache;

// Dynamic font structure
typedef struct {
    stbtt_fontinfo info;
    unsigned char* buffer;
    FontSizeCache* sizes;
    int num_sizes;
    int size_capacity;
    char* path;
} RLFont;

// Dynamic font registry
typedef struct {
    RLFont** fonts;
    int count;
    int capacity;
} FontRegistry;
static FontRegistry fontRegistry = {0};

// Function to dynamically initialize system fonts
const char** rl_GetSystemFonts(void) {
    if (systemFonts.initialized) {
        return systemFonts.pathPtrs;
    }

    // Initial allocation
    systemFonts.capacity = 256;  // Starting with 256, can grow dynamically
    systemFonts.paths = malloc(systemFonts.capacity * sizeof(char*));
    systemFonts.pathPtrs = malloc(systemFonts.capacity * sizeof(char*));
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
            for (size_t j = 0; j < glob_res.gl_pathc && systemFonts.count < systemFonts.capacity; j++) {
                systemFonts.paths[systemFonts.count] = strdup(glob_res.gl_pathv[j]);
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
                char full_path[MAX_PATH];
                sprintf(full_path, "%s\\%s", base_path, fd.cFileName);
                systemFonts.paths[systemFonts.count] = strdup(full_path);
                systemFonts.count++;
            } while (FindNextFile(h, &fd) && systemFonts.count < systemFonts.capacity);
            FindClose(h);
        }
    }
    #endif

    // Set up path pointers
    for (int i = 0; i < systemFonts.count; i++) {
        systemFonts.pathPtrs[i] = systemFonts.paths[i];
    }
    systemFonts.initialized = 1;

    return systemFonts.count > 0 ? systemFonts.pathPtrs : NULL;
}

// Function to clear all cached glyphs for a specific font size
static void clear_font_size_glyphs(FontSizeCache* size_cache) {
    if (!size_cache || !size_cache->chars) return;

    for (int i = 0; i < size_cache->char_capacity; i++) {
        if (size_cache->chars[i]) {
            free(size_cache->chars[i]->bitmap);
            free(size_cache->chars[i]);
            size_cache->chars[i] = NULL;
        }
    }
}

// Function to free all loaded fonts and clear font registry
void rl_FreeFonts(void) {
    if (!fontRegistry.fonts) return;

    for (int i = 0; i < fontRegistry.count; i++) {
        RLFont* font = fontRegistry.fonts[i];
        
        // Clear glyph caches
        for (int j = 0; j < font->num_sizes; j++) {
            clear_font_size_glyphs(&font->sizes[j]);
            free(font->sizes[j].chars);
        }
        
        // Free font resources
        free(font->buffer);
        free(font->path);
        free(font->sizes);
        free(font);
    }

    // Reset font registry
    free(fontRegistry.fonts);
    fontRegistry.fonts = NULL;
    fontRegistry.count = 0;
    fontRegistry.capacity = 0;
}

// Function to free a specific font by its path
void rl_FreeFont(const char* fontPath) {
    // Find the font in the registry
    for (int i = 0; i < fontRegistry.count; i++) {
        RLFont* font = fontRegistry.fonts[i];
        
        // Check if the paths match
        if (strcmp(font->path, fontPath) == 0) {
            // Clear glyph caches for this font
            for (int j = 0; j < font->num_sizes; j++) {
                clear_font_size_glyphs(&font->sizes[j]);
                free(font->sizes[j].chars);
            }
            
            // Free font resources
            free(font->buffer);
            free(font->path);
            free(font->sizes);
            free(font);
            
            // Remove from registry by shifting elements
            for (int j = i; j < fontRegistry.count - 1; j++) {
                fontRegistry.fonts[j] = fontRegistry.fonts[j + 1];
            }
            
            // Decrease registry count
            fontRegistry.count--;
            
            // Optional: Reduce registry capacity if it's significantly larger than count
            if (fontRegistry.count < fontRegistry.capacity / 2 && fontRegistry.capacity > 32) {
                fontRegistry.capacity /= 2;
                fontRegistry.fonts = realloc(fontRegistry.fonts, fontRegistry.capacity * sizeof(RLFont*));
            }
            
            return;
        }
    }
}

// Function to clear all cached glyphs for all loaded fonts
void rl_FreeGlyphs(void) {
    for (int i = 0; i < fontRegistry.count; i++) {
        RLFont* font = fontRegistry.fonts[i];
        
        for (int j = 0; j < font->num_sizes; j++) {
            clear_font_size_glyphs(&font->sizes[j]);
        }
    }
}

// Function to find an already loaded font
static RLFont* find_loaded_font(const char* path) {
    for (int i = 0; i < fontRegistry.count; i++) {
        if (strcmp(fontRegistry.fonts[i]->path, path) == 0) {
            return fontRegistry.fonts[i];
        }
    }
    return NULL;
}

// Dynamic size index getter
static int get_size_index(RLFont* font, float size) {
    for (int i = 0; i < font->num_sizes; i++) {
        if (font->sizes[i].size == size) {
            return i;
        }
    }
    
    // Expand sizes if needed
    if (font->num_sizes >= font->size_capacity) {
        font->size_capacity *= 2;
        font->sizes = realloc(font->sizes, font->size_capacity * sizeof(FontSizeCache));
    }
    
    int idx = font->num_sizes;
    font->sizes[idx].size = size;
    font->sizes[idx].char_capacity = 256;  // Initial char capacity
    font->sizes[idx].chars = calloc(256, sizeof(CachedGlyph*));
    font->num_sizes++;
    
    return idx;
}

// Dynamic font loading function
static RLFont* load_font(const char* path) {
    // Dynamically expand font registry if needed
    if (fontRegistry.count >= fontRegistry.capacity) {
        fontRegistry.capacity = fontRegistry.capacity ? fontRegistry.capacity * 2 : 32;
        fontRegistry.fonts = realloc(fontRegistry.fonts, fontRegistry.capacity * sizeof(RLFont*));
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
    
    // Initial setup for sizes
    font->size_capacity = 16;  // Initial size capacity
    font->sizes = malloc(font->size_capacity * sizeof(FontSizeCache));
    font->num_sizes = 0;
    
    if (fread(font->buffer, size, 1, f) != 1 ||
        !stbtt_InitFont(&font->info, font->buffer, 0)) {
        free(font->buffer);
        free(font->path);
        free(font->sizes);
        free(font);
        fclose(f);
        return NULL;
    }
    
    fclose(f);
    fontRegistry.fonts[fontRegistry.count++] = font;
    return font;
}

// Dynamic glyph caching function
static CachedGlyph* get_cached_glyph(RLFont* font, unsigned char c, float size) {
    int size_idx = get_size_index(font, size);
    FontSizeCache* size_cache = &font->sizes[size_idx];
    
    // Expand char capacity if needed
    if (c >= size_cache->char_capacity) {
        size_cache->char_capacity *= 2;
        size_cache->chars = realloc(size_cache->chars, size_cache->char_capacity * sizeof(CachedGlyph*));
        memset(size_cache->chars + (size_cache->char_capacity/2), 0, (size_cache->char_capacity/2) * sizeof(CachedGlyph*));
    }
    
    CachedGlyph* glyph = size_cache->chars[c];
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
    
    size_cache->chars[c] = glyph;
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

int rl_MeasureTextWidth(const char *text, int fontSize, const char* fontPath) {
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

int rl_MeasureTextHeight(const char *text, int fontSize, const char* fontPath) {
    RLFont* font = load_font(fontPath);
    if (!font) return 0;
    
    float scale = stbtt_ScaleForPixelHeight(&font->info, fontSize);
    int max_height = 0;
    
    for (const char* p = text; *p; p++) {
        int ascent, descent, lineGap;
        stbtt_GetCodepointBox(&font->info, *p, NULL, NULL, NULL, NULL);
        stbtt_GetFontVMetrics(&font->info, &ascent, &descent, &lineGap);
        
        int char_height = (int)((ascent - descent) * scale);
        if (char_height > max_height) {
            max_height = char_height;
        }
    }
    
    return max_height;
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
