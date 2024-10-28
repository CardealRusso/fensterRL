#ifndef FENSTERRL_MAC_H
#define FENSTERRL_MAC_H

#include <time.h>
#include <CoreGraphics/CoreGraphics.h>
#include <objc/NSObjCRuntime.h>
#include <objc/objc-runtime.h>
#include <ApplicationServices/ApplicationServices.h>

// Utility macros for Objective-C message sending
#define msg(r, o, s) ((r(*)(id, SEL))objc_msgSend)(o, sel_getUid(s))
#define msg1(r, o, s, A, a) ((r(*)(id, SEL, A))objc_msgSend)(o, sel_getUid(s), a)
#define msg2(r, o, s, A, a, B, b) ((r(*)(id, SEL, A, B))objc_msgSend)(o, sel_getUid(s), a, b)
#define msg4(r, o, s, A, a, B, b, C, c, D, d) ((r(*)(id, SEL, A, B, C, D))objc_msgSend)(o, sel_getUid(s), a, b, c, d)
#define cls(x) ((id)objc_getClass(x))

extern id const NSDefaultRunLoopMode;
extern id const NSApp;

typedef struct {
    id window;
    id view;
    CGContextRef context;
} PlatformData;

static void DrawRect(id self, SEL cmd, CGRect rect) {
    (void)cmd;
    (void)rect;
    
    PlatformData* platform = (PlatformData*)fenster.platformData;
    
    CGContextRef context = msg(CGContextRef, 
        msg(id, cls("NSGraphicsContext"), "currentContext"), 
        "graphicsPort");
    
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithData(
        NULL, fenster.buffer, 
        fenster.width * fenster.height * 4, NULL
    );
    
    CGImageRef img = CGImageCreate(
        fenster.width, fenster.height, 
        8, 32, fenster.width * 4, 
        space,
        kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little,
        provider, NULL, false, 
        kCGRenderingIntentDefault
    );
    
    CGColorSpaceRelease(space);
    CGDataProviderRelease(provider);
    
    CGContextDrawImage(
        context, 
        CGRectMake(0, 0, fenster.width, fenster.height), 
        img
    );
    
    CGImageRelease(img);
}

static BOOL WindowShouldClose(id self, SEL cmd, id sender) {
    (void)self;
    (void)cmd;
    (void)sender;
    msg1(void, NSApp, "terminate:", id, NSApp);
    return YES;
}

static void PlatformInitWindow(const char* title) {
    // Initialize NSApplication
    msg(id, cls("NSApplication"), "sharedApplication");
    msg1(void, NSApp, "setActivationPolicy:", NSInteger, 0);
    
    PlatformData* platform = malloc(sizeof(PlatformData));
    fenster.platformData = platform;
    
    // Create window
    platform->window = msg4(
        id, 
        msg(id, cls("NSWindow"), "alloc"),
        "initWithContentRect:styleMask:backing:defer:",
        CGRect, CGRectMake(0, 0, fenster.width, fenster.height),
        NSUInteger, 3,  // NSTitledWindowMask | NSClosableWindowMask
        NSUInteger, 2,  // NSBackingStoreBuffered
        BOOL, NO
    );
    
    // Create window delegate
    Class windowDelegate = objc_allocateClassPair(
        (Class)cls("NSObject"), 
        "FensterRLDelegate", 
        0
    );
    class_addMethod(
        windowDelegate,
        sel_getUid("windowShouldClose:"),
        (IMP)WindowShouldClose,
        "c@:@"
    );
    objc_registerClassPair(windowDelegate);
    
    msg1(
        void,
        platform->window,
        "setDelegate:",
        id,
        msg(id, msg(id, (id)windowDelegate, "alloc"), "init")
    );
    
    // Create view
    Class viewClass = objc_allocateClassPair(
        (Class)cls("NSView"),
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
    
    platform->view = msg(id, msg(id, (id)viewClass, "alloc"), "init");
    msg1(void, platform->window, "setContentView:", id, platform->view);
    
    // Set window title
    id nsTitle = msg1(
        id,
        cls("NSString"),
        "stringWithUTF8String:",
        const char*,
        title
    );
    msg1(void, platform->window, "setTitle:", id, nsTitle);
    
    // Show window
    msg1(void, platform->window, "makeKeyAndOrderFront:", id, nil);
    msg(void, platform->window, "center");
    msg1(void, NSApp, "activateIgnoringOtherApps:", BOOL, YES);
}

static double PlatformGetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

static void PlatformSleep(long microseconds) {
    usleep(microseconds);
}

static bool PlatformWindowShouldClose(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    
    // Process events
    id event = msg4(
        id,
        NSApp,
        "nextEventMatchingMask:untilDate:inMode:dequeue:",
        NSUInteger, NSUIntegerMax,
        id, NULL,
        id, NSDefaultRunLoopMode,
        BOOL, YES
    );
    
    if (event) {
        msg1(void, NSApp, "sendEvent:", id, event);
    }
    
    // Update view
    msg1(
        void,
        msg(id, platform->window, "contentView"),
        "setNeedsDisplay:",
        BOOL,
        YES
    );
    
    return false;
}

static void PlatformCloseWindow(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    msg(void, platform->window, "close");
    free(platform);
    fenster.platformData = NULL;
}

#endif // FENSTERRL_MAC_H
