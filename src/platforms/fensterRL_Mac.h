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
#define msg4(r, o, s, A, a, B, b, C, c, D, d) ((r(*)(id, SEL, A, B, C, D))objc_msgSend)(o, sel_getUid(s), a, b, c, d)
#define cls(x) ((id)objc_getClass(x))

extern id const NSDefaultRunLoopMode;
extern id const NSApp;

static bool closeRequested = false;
static bool rl_WindowResized = false;

typedef struct {
   id window;
   id view;
   CGContextRef context;
   CGImageRef lastImage;  // Store the last created image
} PlatformData;

static void WindowDidResize(id self, SEL cmd, id notification) {
   (void)cmd;
   PlatformData* platform = (PlatformData*)fenster.platformData;
   
   if (!fenster.isResizable) return;

   CGRect frame = msg(CGRect, msg(id, notification, "object"), "frame");

   rl_WindowResized = true;
   free(fenster.buffer);
   fenster.buffer = malloc(frame.size.width * frame.size.height * sizeof(uint32_t));
   fenster.width = frame.size.width;
   fenster.height = frame.size.height;
}

static void DrawRect(id self, SEL cmd, CGRect rect) {
   (void)cmd;
   (void)rect;
   
   PlatformData* platform = (PlatformData*)fenster.platformData;
   
   if (platform->lastImage) {
       CGContextRef context = msg(CGContextRef, 
           msg(id, cls("NSGraphicsContext"), "currentContext"), 
           "graphicsPort");
       
       CGContextDrawImage(
           context, 
           CGRectMake(0, 0, fenster.width, fenster.height), 
           platform->lastImage
       );
   }
}

static BOOL WindowShouldClose(id self, SEL cmd, id sender) {
   (void)self;
   (void)cmd;
   (void)sender;
   closeRequested = true;
   return NO;
}

static void PlatformInitWindow(const char* title) {
   msg(id, cls("NSApplication"), "sharedApplication");
   msg1(void, NSApp, "setActivationPolicy:", NSInteger, 0);
   
   PlatformData* platform = malloc(sizeof(PlatformData));
   fenster.platformData = platform;
   platform->lastImage = NULL;
   
   platform->window = msg4(
       id, 
       msg(id, cls("NSWindow"), "alloc"),
       "initWithContentRect:styleMask:backing:defer:",
       CGRect, CGRectMake(0, 0, fenster.width, fenster.height),
       NSUInteger, fenster.isResizable ? 15 : 3,
       NSUInteger, 2,
       BOOL, NO
   );
   
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
   
   class_addMethod(
       windowDelegate,
       sel_getUid("windowDidResize:"),
       (IMP)WindowDidResize,
       "v@:@"
   );
   
   objc_registerClassPair(windowDelegate);
   
   id delegateInstance = msg(id, msg(id, (id)windowDelegate, "alloc"), "init");
   
   msg1(
       void,
       platform->window,
       "setDelegate:",
       id,
       delegateInstance
   );

   msg4(void, msg(id, cls("NSNotificationCenter"), "defaultCenter"),
        "addObserver:selector:name:object:", 
        id, delegateInstance,
        SEL, sel_getUid("windowDidResize:"),
        id, msg1(id, cls("NSString"), "stringWithUTF8String:", const char*, "NSWindowDidResizeNotification"),
        id, platform->window);
   
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
   
   id nsTitle = msg1(
       id,
       cls("NSString"),
       "stringWithUTF8String:",
       const char*,
       title
   );
   
   msg1(void, platform->window, "setTitle:", id, nsTitle);
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

static void PlatformWindowEventLoop(void) {
   PlatformData* platform = (PlatformData*)fenster.platformData;
   closeRequested = false;
   rl_WindowResized = false;
   
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
}

static void PlatformRenderFrame(void) {
   PlatformData* platform = (PlatformData*)fenster.platformData;
   
   // Release previous image if exists
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
   
   // Trigger a redraw
   msg1(
       void,
       msg(id, platform->window, "contentView"),
       "setNeedsDisplay:",
       BOOL,
       YES
   );
}

static void PlatformCloseWindow(void) {
   PlatformData* platform = (PlatformData*)fenster.platformData;
   
   // Release the last image if it exists
   if (platform->lastImage) {
       CGImageRelease(platform->lastImage);
       platform->lastImage = NULL;
   }
   
   msg(void, platform->window, "close");
   free(platform);
   fenster.platformData = NULL;
}

static void PlatformSetWindowTitle(const char* title) {
   PlatformData* platform = (PlatformData*)fenster.platformData;
   id nsTitle = msg1(
       id,
       cls("NSString"),
       "stringWithUTF8String:",
       const char*,
       title
   );
   msg1(void, platform->window, "setTitle:", id, nsTitle);
}

static int PlatformGetScreenWidth(void) {
    CGDirectDisplayID display = CGMainDisplayID();
    return CGDisplayPixelsWide(display);
}

static int PlatformGetScreenHeight(void) {
    CGDirectDisplayID display = CGMainDisplayID();
    return CGDisplayPixelsHigh(display);
}

static bool PlatformIsWindowFocused(void) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    return msg(BOOL, platform->window, "isKeyWindow");
}

static void PlatformSetWindowPosition(int x, int y) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    CGRect frame = msg(CGRect, platform->window, "frame");
    frame.origin.x = x;
    frame.origin.y = y;
    msg2(void, platform->window, "setFrame:display:", CGRect, frame, BOOL, YES);
}

static void PlatformSetWindowSize(int width, int height) {
    PlatformData* platform = (PlatformData*)fenster.platformData;
    CGRect newFrame = CGRectMake(0, 0, width, height);
    msg2(void, platform->window, "setFrame:display:", CGRect, newFrame, BOOL, YES);
    msg(void, platform->window, "center");
}

static void PlatformSetWindowFocused(void) {
   PlatformData* platform = (PlatformData*)fenster.platformData;
   msg1(void, platform->window, "makeKeyAndOrderFront:", id, nil);
}


static int PlatformGetWindowPositionX(void) {
   PlatformData* platform = (PlatformData*)fenster.platformData;
   CGRect frame = msg(CGRect, platform->window, "frame");
   return frame.origin.x;
}

static int PlatformGetWindowPositionY(void) {
   PlatformData* platform = (PlatformData*)fenster.platformData;
   CGRect frame = msg(CGRect, platform->window, "frame");
   return frame.origin.y;
}
#endif // FENSTERRL_MAC_H
