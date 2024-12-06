#ifndef FENSTERRL_MAC_H
#define FENSTERRL_MAC_H

#include <time.h>
#include <CoreGraphics/CoreGraphics.h>
#include <objc/NSObjCRuntime.h>
#include <objc/objc-runtime.h>
#include <ApplicationServices/ApplicationServices.h>

// Simplified macro for Objective-C method calls with variable arguments
#define OBJC_CALL(ret, obj, sel, ...) ((ret (*)(id, SEL, ...))objc_msgSend)(obj, sel_getUid(sel), ##__VA_ARGS__)

#define OBJC_CALL_2(ret, obj, sel, arg1) ((ret (*)(id, SEL, id))objc_msgSend)(obj, sel_getUid(sel), arg1)
#define OBJC_CALL_1(ret, obj, sel) ((ret (*)(id, SEL))objc_msgSend)(obj, sel_getUid(sel))
#define OBJC_CALL_0(ret, obj, sel) ((ret (*)(id, SEL))objc_msgSend)(obj, sel_getUid(sel))
#define CLS(x) ((id)objc_getClass(x))

extern id const NSDefaultRunLoopMode;
extern id const NSApp;

typedef struct {
    id window;
    id view;
    CGContextRef context;
    CGImageRef lastImage;
} PlatformData;

// Suppress unused parameter warnings by using __unused macro
static BOOL WindowShouldClose(__unused id self, __unused SEL cmd, __unused id sender) {
    fenster.hasCloseRequest = 1;
    return NO;
}

static void WindowDidResize(__unused id self, __unused SEL cmd, id notification) {
    if (!fenster.isResizable) return;

    CGRect frame;
    frame = OBJC_CALL(CGRect, OBJC_CALL_1(id, notification, "object"), "frame");
    
    fenster.hasResized = 1;
    free(fenster.buffer);
    fenster.buffer = malloc(frame.size.width * frame.size.height * sizeof(uint32_t));
    fenster.width = frame.size.width;
    fenster.height = frame.size.height;
    
    CGDirectDisplayID display = CGMainDisplayID();
    fenster.screenWidth = CGDisplayPixelsWide(display);
    fenster.screenHeight = CGDisplayPixelsHigh(display);
}

static void WindowDidMove(__unused id self, __unused SEL cmd, __unused id notification) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    CGRect frame = OBJC_CALL(CGRect, platform->window, "frame");
    
    fenster.windowPosX = frame.origin.x;
    fenster.windowPosY = frame.origin.y;
}

static void WindowDidBecomeKey(__unused id self, __unused SEL cmd, __unused id notification) {
    fenster.isFocused = 1;
}

static void WindowDidResignKey(__unused id self, __unused SEL cmd, __unused id notification) {
    fenster.isFocused = 0;
}

static void DrawRect(__unused id self, __unused SEL cmd, __unused CGRect rect) {
    PlatformData* platform = (PlatformData*)fenster.platformData;

    if (platform->lastImage) {
        CGContextRef context = OBJC_CALL(CGContextRef, 
            OBJC_CALL_1(id, CLS("NSGraphicsContext"), "currentContext"), 
            "graphicsPort"
        );

        CGContextDrawImage(
            context,
            CGRectMake(0, 0, fenster.width, fenster.height),
            platform->lastImage
        );
    }
}

static void setupWindowDelegate(id window) {
    Class delegateClass = objc_allocateClassPair(
        (Class)CLS("NSObject"), 
        "FensterRLDelegate", 
        0
    );

    class_addMethod(delegateClass, sel_getUid("windowShouldClose:"), 
        (IMP)WindowShouldClose, "c@:@");
    class_addMethod(delegateClass, sel_getUid("windowDidResize:"), 
        (IMP)WindowDidResize, "v@:@");
    class_addMethod(delegateClass, sel_getUid("windowDidMove:"), 
        (IMP)WindowDidMove, "v@:@");
    class_addMethod(delegateClass, sel_getUid("windowDidBecomeKey:"), 
        (IMP)WindowDidBecomeKey, "v@:@");
    class_addMethod(delegateClass, sel_getUid("windowDidResignKey:"), 
        (IMP)WindowDidResignKey, "v@:@");

    objc_registerClassPair(delegateClass);
    
    id delegateInstance = OBJC_CALL_1(id, OBJC_CALL_1(id, (id)delegateClass, "alloc"), "init");
    OBJC_CALL_2(void, window, "setDelegate:", delegateInstance);

    id defaultCenter = OBJC_CALL_1(id, CLS("NSNotificationCenter"), "defaultCenter");
    OBJC_CALL_0(void, 
        defaultCenter,
        "addObserver:selector:name:object:"
    );
}

static void PlatformInitWindow(const char* title) {
    OBJC_CALL_1(id, CLS("NSApplication"), "sharedApplication");
    ((void (*)(id, SEL, NSInteger))objc_msgSend)(NSApp, sel_getUid("setActivationPolicy:"), 0);

    PlatformData* platform = malloc(sizeof(PlatformData));
    fenster.platformData = platform;
    platform->lastImage = NULL;

    platform->window = ((id (*)(id, SEL, CGRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(
        OBJC_CALL_1(id, CLS("NSWindow"), "alloc"),
        sel_getUid("initWithContentRect:styleMask:backing:defer:"),
        CGRectMake(0, 0, fenster.width, fenster.height),
        fenster.isResizable ? 15 : 3,
        2,
        NO
    );

    CGDirectDisplayID display = CGMainDisplayID();
    fenster.screenWidth = CGDisplayPixelsWide(display);
    fenster.screenHeight = CGDisplayPixelsHigh(display);

    CGRect frame = OBJC_CALL_1(CGRect, platform->window, "frame");
    fenster.windowPosX = frame.origin.x;
    fenster.windowPosY = frame.origin.y;

    setupWindowDelegate(platform->window);

    Class viewClass = objc_allocateClassPair(
        (Class)CLS("NSView"),
        "FensterRLView",
        0
    );

    class_addMethod(
        viewClass,
        sel_getUid("drawRect:"),
        (IMP)DrawRect,
        "v@:@"
    );

    objc_registerClassPair(viewClass);

    platform->view = OBJC_CALL_1(id, OBJC_CALL_1(id, (id)viewClass, "alloc"), "init");
    OBJC_CALL_2(void, platform->window, "setContentView:", platform->view);

    id nsTitle = ((id (*)(id, SEL, const char*))objc_msgSend)(
        CLS("NSString"), 
        sel_getUid("stringWithUTF8String:"), 
        title
    );

    OBJC_CALL_2(void, platform->window, "setTitle:", nsTitle);
    OBJC_CALL_1(void, platform->window, "makeKeyAndOrderFront:");
    OBJC_CALL_1(void, platform->window, "center");
    ((void (*)(id, SEL, BOOL))objc_msgSend)(NSApp, sel_getUid("activateIgnoringOtherApps:"), YES);

    fenster.isFocused = OBJC_CALL_1(BOOL, platform->window, "isKeyWindow");
}


static void PlatformSleep(int64_t ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

static int64_t PlatformGetTime(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000 + (time.tv_nsec / 1000000);
}

static void PlatformPollInputEvents(void) {
    fenster.hasCloseRequest = false;
    fenster.hasResized = false;

    id event = OBJC_CALL(
        id,
        NSApp,
        "nextEventMatchingMask:untilDate:inMode:dequeue:",
        NSUIntegerMax,
        NULL,
        NSDefaultRunLoopMode,
        YES
    );

    if (event) {
        NSUInteger evtype = OBJC_CALL(NSUInteger, event, "type");
        if (evtype == 5 || evtype == 6) { // NSEventTypeMouseMoved
            CGPoint xy = OBJC_CALL(CGPoint, event, "locationInWindow");
            fenster.mousePosition[0] = (int)xy.x;
            fenster.mousePosition[1] = (int)(fenster.mousePosition[1] - xy.y);
        }
        OBJC_CALL(void, NSApp, "sendEvent:", event);
    }
}

static void PlatformRenderFrame(void) {
    PlatformData* platform = fenster.platformData;

    // Release previous image
    if (platform->lastImage) {
        CGImageRelease(platform->lastImage);
    }

    // Create new image
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithData(
        NULL, fenster.buffer,
        fenster.width * fenster.height * 4, NULL
    );

    platform->lastImage = CGImageCreate(
        fenster.width, fenster.height,
        8, 32, fenster.width * 4,
        space,
        kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little,
        provider, NULL, false,
        kCGRenderingIntentDefault
    );

    CGColorSpaceRelease(space);
    CGDataProviderRelease(provider);

    // Trigger redraw
    OBJC_CALL(
        void,
        OBJC_CALL(id, platform->window, "contentView"),
        "setNeedsDisplay:", 
        YES
    );
}

static void PlatformCloseWindow(void) {
    PlatformData* platform = fenster.platformData;

    // Release last image
    if (platform->lastImage) {
        CGImageRelease(platform->lastImage);
        platform->lastImage = NULL;
    }

    OBJC_CALL(void, platform->window, "close");
    free(platform);
    fenster.platformData = NULL;
}

static void PlatformSetWindowTitle(const char* title) {
    PlatformData* platform = fenster.platformData;
    id nsTitle = OBJC_CALL(
        id,
        CLS("NSString"),
        "stringWithUTF8String:",
        title
    );
    OBJC_CALL(void, platform->window, "setTitle:", nsTitle);
}

static void PlatformSetWindowPosition(int x, int y) {
    PlatformData* platform = fenster.platformData;
    CGRect frame = OBJC_CALL(CGRect, platform->window, "frame");
    frame.origin.x = x;
    frame.origin.y = y;
    OBJC_CALL(void, platform->window, "setFrame:display:", frame, YES);
}

static void PlatformSetWindowSize(int width, int height) {
    PlatformData* platform = fenster.platformData;
    CGRect newFrame = CGRectMake(0, 0, width, height);
    OBJC_CALL(void, platform->window, "setFrame:display:", newFrame, YES);
    OBJC_CALL(void, platform->window, "center");
}

static void PlatformSetWindowFocused(void) {
    PlatformData* platform = fenster.platformData;
    OBJC_CALL(void, platform->window, "makeKeyAndOrderFront:", nil);
}

#endif // FENSTERRL_MAC_H
