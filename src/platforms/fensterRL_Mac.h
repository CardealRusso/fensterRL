#ifndef FENSTERRL_MAC_H
#define FENSTERRL_MAC_H

#include <time.h>
#include <CoreGraphics/CoreGraphics.h>
#include <objc/NSObjCRuntime.h>
#include <objc/objc-runtime.h>
#include <ApplicationServices/ApplicationServices.h>

#define msg(r, o, s) ((r(*)(id, SEL))objc_msgSend)(o, sel_getUid(s))
#define msg1(r, o, s, A, a) ((r(*)(id, SEL, A))objc_msgSend)(o, sel_getUid(s), a)
#define msg2(r, o, s, A, a, B, b) ((r(*)(id, SEL, A, B))objc_msgSend)(o, sel_getUid(s), a, b)
#define msg3(r, o, s, A, a, B, b, C, c) ((r(*)(id, SEL, A, B, C))objc_msgSend)(o, sel_getUid(s), a, b, c)
#define msg4(r, o, s, A, a, B, b, C, c, D, d) ((r(*)(id, SEL, A, B, C, D))objc_msgSend)(o, sel_getUid(s), a, b, c, d)
#define OBJC_CALL(ret, obj, sel, ...) ((ret (*)(id, SEL, ...))objc_msgSend)(obj, sel_getUid(sel), ##__VA_ARGS__)

#define cls(x) ((id)objc_getClass(x))

extern id const NSDefaultRunLoopMode;
extern id const NSApp;

typedef struct {
    id wnd;  // Window object
} PlatformData;

static void WindowShouldClose(id self, SEL cmd, id sender);
static void WindowDidResize(id self, SEL cmd, id notification);
static void DrawRect(id self, SEL cmd, CGRect rect);

static void WindowDidMove(__unused id self, __unused SEL cmd, __unused id notification) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    CGRect frame = OBJC_CALL(CGRect, platform->wnd, "frame");

    fenster.windowPosX = frame.origin.x;
    fenster.windowPosY = frame.origin.y;
}

void WindowDidBecomeKey(__unused id self, __unused SEL _cmd, __unused id notification) {
    fenster.isFocused = 1;
}

void WindowDidResignKey(__unused id self, __unused SEL _cmd, __unused id notification) {
    fenster.isFocused = 0;
}

static void PlatformInitWindow(const char* title) {
    PlatformData* platform = calloc(1, sizeof(PlatformData));
    fenster.platformData = platform;

    msg(id, cls("NSApplication"), "sharedApplication");
    msg1(void, NSApp, "setActivationPolicy:", NSInteger, 0);

    // Create window
    platform->wnd = msg4(id, msg(id, cls("NSWindow"), "alloc"),
                         "initWithContentRect:styleMask:backing:defer:", 
                         CGRect, CGRectMake(0, 0, fenster.width, fenster.height), 
                         NSUInteger, 3, 
                         NSUInteger, 2, 
                         BOOL, NO);

    // Create window delegate
  Class windelegate =
      objc_allocateClassPair((Class)cls("NSObject"), "FensterDelegate", 0);
  class_addMethod(windelegate, sel_getUid("windowShouldClose:"),
                  (IMP)WindowShouldClose, "c@:@");
  class_addMethod(windelegate, sel_getUid("windowDidMove:"),
                  (IMP)WindowDidMove, "v@:@");
  class_addMethod(windelegate, sel_getUid("windowDidBecomeKey:"), 
                  (IMP)WindowDidBecomeKey, "v@:@");
  class_addMethod(windelegate, sel_getUid("windowDidResignKey:"), 
                  (IMP)WindowDidResignKey, "v@:@");
  class_addMethod(windelegate, sel_getUid("windowDidResize:"), 
                  (IMP)WindowDidResize, "v@:@");
  objc_registerClassPair(windelegate);
  msg1(void, platform->wnd, "setDelegate:", id,
       msg(id, msg(id, (id)windelegate, "alloc"), "init"));
  Class c = objc_allocateClassPair((Class)cls("NSView"), "FensterView", 0);
  class_addMethod(c, sel_getUid("drawRect:"), (IMP)DrawRect, "i@:@@");
  objc_registerClassPair(c);

    // Set up view
    id view = msg(id, msg(id, (id)c, "alloc"), "init");
    msg1(void, platform->wnd, "setContentView:", id, view);
    
    // Set window title
    id nsTitle = msg1(id, cls("NSString"), "stringWithUTF8String:", const char*, title);
    msg1(void, platform->wnd, "setTitle:", id, nsTitle);

    // Show and activate window
    msg1(void, platform->wnd, "makeKeyAndOrderFront:", id, nil);
    msg(void, platform->wnd, "center");
    msg1(void, NSApp, "activateIgnoringOtherApps:", BOOL, YES);
}

static void WindowShouldClose(id self, SEL cmd, id sender) {
    (void)self, (void)cmd, (void)sender;
    fenster.hasCloseRequest = 1;
}

static void WindowDidResize(id self, SEL cmd, id notification) {
    (void)self, (void)cmd;
    CGRect frame = msg(CGRect, msg(id, notification, "object"), "frame");
    
    free(fenster.buffer);
    fenster.buffer = malloc(frame.size.width * frame.size.height * sizeof(uint32_t));
    
    fenster.width = frame.size.width;
    fenster.height = frame.size.height;
    fenster.hasResized = 1;
}

static void DrawRect(id self, SEL cmd, CGRect r) {
    fprintf(stderr, "DrawRect called: width=%d, height=%d, buffer=%p\n", 
            fenster.width, fenster.height, (void*)fenster.buffer);
    (void)r, (void)cmd, (void)self;
    CGContextRef context = msg(CGContextRef, msg(id, cls("NSGraphicsContext"), "currentContext"), "graphicsPort");
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithData(
        NULL, fenster.buffer, fenster.width * fenster.height * 4, NULL);
    CGImageRef img = CGImageCreate(
        fenster.width, fenster.height, 8, 32, fenster.width * 4, space,
        kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little,
        provider, NULL, false, kCGRenderingIntentDefault);
    CGColorSpaceRelease(space);
    CGDataProviderRelease(provider);
    CGContextDrawImage(context, CGRectMake(0, 0, fenster.width, fenster.height), img);
    CGImageRelease(img);
}

static void PlatformPollInputEvents(void) {
    fenster.hasCloseRequest = false;
    fenster.hasResized = false;

    id event = msg4(id, NSApp, "nextEventMatchingMask:untilDate:inMode:dequeue:", 
                    NSUInteger, NSUIntegerMax, 
                    id, nil, 
                    id, NSDefaultRunLoopMode, 
                    BOOL, YES);
    
    if (event) {
        NSUInteger evtype = msg(NSUInteger, event, "type");
        
        switch (evtype) {
            case 1: /* NSEventTypeLeftMouseDown */
                fenster.mouseButtonsPressed[0] = fenster.mouseButtonsHold[0] = true;
                break;
            case 2: /* NSEventTypeLeftMouseUp */
                fenster.mouseButtonsHold[0] = false;
                break;
            case 3: /* NSEventTypeRightMouseDown */
                fenster.mouseButtonsPressed[1] = fenster.mouseButtonsHold[1] = true;
                break;
            case 4: /* NSEventTypeRightMouseUp */
                fenster.mouseButtonsHold[1] = false;
                break;
            case 25: /* NSEventTypeOtherMouseDown */
                fenster.mouseButtonsPressed[2] = fenster.mouseButtonsHold[2] = true;
                break;
            case 26: /* NSEventTypeOtherMouseUp */
                fenster.mouseButtonsHold[2] = false;
                break;
case 22: { /* NSEventTypeScrollWheel */
    CGFloat deltaY = msg1(CGFloat, event, "scrollingDeltaY:", CGFloat, 0);
    if (deltaY > 0) {
        fenster.mouseButtonsPressed[3] = true;
    } else if (deltaY < 0) {
        fenster.mouseButtonsPressed[4] = true;
    }
    break;
}
            case 5: /* NSEventTypeMouseMoved */
            case 6: { /* NSEventTypeMouseMoved */
                CGPoint xy = msg(CGPoint, event, "locationInWindow");
                fenster.mousePosition[0] = (int)xy.x;
                fenster.mousePosition[1] = (int)(fenster.height - xy.y);
                break;
            }
            case 10: { // NSEventTypeKeyDown
                unsigned short keyCode = msg(unsigned short, event, "keyCode");
                fenster.holdKeys |= (1ULL << keyCode);
                fenster.pressedKeys |= (1ULL << keyCode);
                break;
            }
            case 11: { // NSEventTypeKeyUp
                unsigned short keyCode = msg(unsigned short, event, "keyCode");
                fenster.holdKeys &= ~(1ULL << keyCode);
                break;
            }
            default:
                break;
        }
        
        msg1(void, NSApp, "sendEvent:", id, event);
    }
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

void PlatformRenderFrame(void) {
    PlatformData* platform = fenster.platformData;
    if (platform && platform->wnd) {
        id contentView = msg(id, platform->wnd, "contentView");
        msg1(void, contentView, "setNeedsDisplay:", BOOL, YES);
        msg(void, contentView, "display");
    }
}
static void PlatformCloseWindow(void) {
    PlatformData* platform = fenster.platformData;
    if (platform) {
        msg(void, platform->wnd, "close");
        free(platform);
        fenster.platformData = NULL;
    }
}

static void PlatformSetWindowTitle(const char* title) {
    PlatformData* platform = fenster.platformData;
    id nsTitle = msg1(id, cls("NSString"), "stringWithUTF8String:", const char*, title);
    msg1(void, platform->wnd, "setTitle:", id, nsTitle);
}

static void PlatformSetWindowPosition(int x, int y) {
    PlatformData* platform = fenster.platformData;
    CGRect frame = msg(CGRect, platform->wnd, "frame");
    frame.origin.x = x;
    frame.origin.y = y;
    msg2(void, platform->wnd, "setFrame:display:", CGRect, frame, BOOL, YES);
}

static void PlatformSetWindowSize(int width, int height) {
    PlatformData* platform = fenster.platformData;
    CGRect newFrame = CGRectMake(0, 0, width, height);
    msg2(void, platform->wnd, "setFrame:display:", CGRect, newFrame, BOOL, YES);
    msg(void, platform->wnd, "center");
}

static void PlatformSetWindowFocused(void) {
    PlatformData* platform = fenster.platformData;
    msg1(void, platform->wnd, "makeKeyAndOrderFront:", id, nil);
}

static void PlatformToggleFullscreen(void) {
    PlatformData* platform = fenster.platformData;
    msg1(void, platform->wnd, "toggleFullScreen:", id, nil);
    
    NSUInteger styleMask = msg(NSUInteger, platform->wnd, "styleMask");
    fenster.isFullScreen = !fenster.isFullScreen;
    if (fenster.isFullScreen) {
        styleMask |= 1 << 14;
    } else {
        styleMask &= ~(1 << 14);
    }
    msg1(void, platform->wnd, "setStyleMask:", NSUInteger, styleMask);
}

#endif // FENSTERRL_MAC_H
