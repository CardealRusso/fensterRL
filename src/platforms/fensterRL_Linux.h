// fensterRL_Linux.h - Linux implementation
#ifndef FENSTERRL_LINUX_H
#define FENSTERRL_LINUX_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

typedef struct {
    Display* display;
    Window window;
    GC gc;
    XImage* image;
    Atom wm_delete_window;
} PlatformData;

static void PlatformSleep(long microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    nanosleep(&ts, NULL);
}

static double PlatformGetTime(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec + time.tv_nsec / 1000000000.0;
}

static void PlatformInitWindow(const char* title) {
    PlatformData* platform = malloc(sizeof(PlatformData));
    fenster.platformData = platform;
    
    platform->display = XOpenDisplay(NULL);
    if (!platform->display) return;
    
    int screen = DefaultScreen(platform->display);
    Window root = RootWindow(platform->display, screen);
    
    // Get screen dimensions
    fenster.screenWidth = DisplayWidth(platform->display, screen);
    fenster.screenHeight = DisplayHeight(platform->display, screen);

    platform->window = XCreateSimpleWindow(
        platform->display, root,
        0, 0, fenster.width, fenster.height, 0,
        BlackPixel(platform->display, screen),
        BlackPixel(platform->display, screen)
    );
    
    XStoreName(platform->display, platform->window, title);
    
    // Handle window close button
    platform->wm_delete_window = XInternAtom(platform->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(platform->display, platform->window, &platform->wm_delete_window, 1);
    
    XSelectInput(platform->display, platform->window, 
        ExposureMask | KeyPressMask | StructureNotifyMask | 
        (fenster.isResizable ? 0 : ResizeRedirectMask) | FocusChangeMask
    );
    
    platform->gc = XCreateGC(platform->display, platform->window, 0, NULL);
    
    // Get initial window position
    XWindowAttributes attributes;
    XGetWindowAttributes(platform->display, platform->window, &attributes);
    fenster.windowPosX = attributes.x;
    fenster.windowPosY = attributes.y;
    
    // Initial focus state
    Window focusedWindow;
    int revertTo;
    XGetInputFocus(platform->display, &focusedWindow, &revertTo);
    fenster.isFocused = (focusedWindow == platform->window);

    // Create XImage
    platform->image = XCreateImage(
        platform->display,
        DefaultVisual(platform->display, screen),
        DefaultDepth(platform->display, screen),
        ZPixmap, 0, (char*)fenster.buffer,
        fenster.width, fenster.height,
        32, 0
    );
    
    XMapWindow(platform->display, platform->window);
    XFlush(platform->display);
}

static void PlatformWindowEventLoop(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    XEvent event;
    fenster.hasCloseRequest = false;
    fenster.hasResized = false;

    while (XPending(platform->display)) {
        XNextEvent(platform->display, &event);
        
        switch (event.type) {
            case ClientMessage:
                if (event.xclient.data.l[0] == (long)platform->wm_delete_window) {
                    fenster.hasCloseRequest = true;
                }
                break;
            
            case ConfigureNotify:
                PlatformData* platform = (PlatformData*)fenster.platformData;
                int screen = DefaultScreen(platform->display);
                fenster.screenWidth = DisplayWidth(platform->display, screen);
                fenster.screenHeight = DisplayHeight(platform->display, screen);
                fenster.windowPosX = event.xconfigure.x;
                fenster.windowPosY = event.xconfigure.y;
                if (fenster.isResizable) {
                    int newWidth = event.xconfigure.width;
                    int newHeight = event.xconfigure.height;
                    
                    // If window size changed
                    if (newWidth != fenster.width || newHeight != fenster.height) {
                        // Realloc buffer
                        uint32_t* newBuffer = realloc(fenster.buffer, newWidth * newHeight * sizeof(uint32_t));
                        if (newBuffer) {
                            fenster.hasResized = true;
                            fenster.buffer = newBuffer;
                            
                            // Update platform image
                            platform->image->data = NULL;  // Prevent XDestroyImage from freeing our buffer
                            XDestroyImage(platform->image);
                            
                            // Recreate XImage with new dimensions and buffer
                            platform->image = XCreateImage(
                                platform->display,
                                DefaultVisual(platform->display, DefaultScreen(platform->display)),
                                DefaultDepth(platform->display, DefaultScreen(platform->display)),
                                ZPixmap, 0, (char*)fenster.buffer,
                                newWidth, newHeight, 32, 0
                            );
                            
                            // Update fenster dimensions
                            fenster.width = newWidth;
                            fenster.height = newHeight;
                        }
                    }
                }
                break;
            
            case FocusIn:
                fenster.isFocused = true;
                break;
            
            case FocusOut:
                fenster.isFocused = false;
                break;
            
        }
    }
}

static void PlatformCloseWindow(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    platform->image->data = NULL;  // Prevent XDestroyImage from freeing our buffer
    XDestroyImage(platform->image);
    XFreeGC(platform->display, platform->gc);
    XDestroyWindow(platform->display, platform->window);
    XCloseDisplay(platform->display);
    free(platform);
    fenster.platformData = NULL;
}

void PlatformRenderFrame(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;

    XPutImage(
        platform->display, platform->window, platform->gc, platform->image,
        0, 0, 0, 0, fenster.width, fenster.height
    );
    XFlush(platform->display);
}

static void PlatformSetWindowTitle(const char* title) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    if (platform && platform->display && platform->window) {
        XStoreName(platform->display, platform->window, title);
        XFlush(platform->display);
    }
}

static void PlatformSetWindowPosition(int x, int y) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    if (platform && platform->display && platform->window) {
        XMoveWindow(platform->display, platform->window, x, y);
        XFlush(platform->display);
    }
}

static void PlatformSetWindowSize(int width, int height) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    if (platform && platform->display && platform->window) {
        XResizeWindow(platform->display, platform->window, width, height);
        XFlush(platform->display);
    }
}

static void PlatformSetWindowFocused(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    if (platform && platform->display && platform->window) {
        XRaiseWindow(platform->display, platform->window);
        XFlush(platform->display);
    }
}

#endif // FENSTERRL_LINUX_H
