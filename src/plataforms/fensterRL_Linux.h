// fensterRL_Linux.h - Linux implementation
#ifndef FENSTERRL_LINUX_H
#define FENSTERRL_LINUX_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct {
    Display* display;
    Window window;
    GC gc;
    XImage* image;
    Atom wm_delete_window;
} PlatformData;

static double PlatformGetTime(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static void PlatformSleep(long microseconds) {
    usleep(microseconds);
}

static void PlatformInitWindow(const char* title) {
    PlatformData* platform = malloc(sizeof(PlatformData));
    fenster.platformData = platform;
    
    platform->display = XOpenDisplay(NULL);
    if (!platform->display) return;
    
    int screen = DefaultScreen(platform->display);
    Window root = RootWindow(platform->display, screen);
    
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
    
    XSelectInput(platform->display, platform->window, ExposureMask | KeyPressMask | StructureNotifyMask);
    
    platform->gc = XCreateGC(platform->display, platform->window, 0, NULL);
    
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

static bool PlatformWindowShouldClose(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    XEvent event;
    
    while (XPending(platform->display)) {
        XNextEvent(platform->display, &event);
        if (event.type == ClientMessage) {
            if (event.xclient.data.l[0] == platform->wm_delete_window) {
                return true;
            }
        }
    }
    
    // Update window content
    XPutImage(
        platform->display, platform->window, platform->gc, platform->image,
        0, 0, 0, 0, fenster.width, fenster.height
    );
    XFlush(platform->display);
    
    return false;
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

#endif // FENSTERRL_LINUX_H
