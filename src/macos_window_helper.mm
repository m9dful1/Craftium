#import <AppKit/AppKit.h>
#include <QtCore/Qt>
#import <objc/runtime.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static const void* kCraftiumOriginalWindowClassKey = &kCraftiumOriginalWindowClassKey;
static const char* kCraftiumFloatingSuffix = "_CraftiumFloating";
static const void* kCraftiumOriginalStyleMaskKey = &kCraftiumOriginalStyleMaskKey;

static BOOL craftium_canBecomeKeyWindow(id, SEL) {
    return NO;
}

static BOOL craftium_canBecomeMainWindow(id, SEL) {
    return NO;
}

static BOOL craftium_becomesKeyOnlyIfNeeded(id, SEL) {
    return NO;
}

static BOOL craftium_acceptsFirstResponder(id, SEL) {
    return NO;
}

static Class craftiumEnsureFloatingSubclass(Class originalClass) {
    if (!originalClass) {
        return Nil;
    }

    NSString* originalName = NSStringFromClass(originalClass);
    NSString* subclassName = [originalName stringByAppendingString:@(kCraftiumFloatingSuffix)];
    Class floatingClass = objc_lookUpClass([subclassName UTF8String]);

    if (!floatingClass) {
        floatingClass = objc_allocateClassPair(originalClass, [subclassName UTF8String], 0);
        if (!floatingClass) {
            return Nil;
        }

        class_replaceMethod(floatingClass, @selector(canBecomeKeyWindow), (IMP)craftium_canBecomeKeyWindow, "c@:");
        class_replaceMethod(floatingClass, @selector(canBecomeMainWindow), (IMP)craftium_canBecomeMainWindow, "c@:");
        class_replaceMethod(floatingClass, @selector(becomesKeyOnlyIfNeeded), (IMP)craftium_becomesKeyOnlyIfNeeded, "c@:");
        class_replaceMethod(floatingClass, @selector(acceptsFirstResponder), (IMP)craftium_acceptsFirstResponder, "c@:");

        objc_registerClassPair(floatingClass);
    }

    return floatingClass;
}

// Helper function to set macOS window level
void setMacOSWindowLevel(void* nsViewPtr, bool floatingLevel) {
    NSView* view = (__bridge NSView*)nsViewPtr;
    NSWindow* window = [view window];

    if (window) {
        if (floatingLevel) {
            Class currentClass = object_getClass(window);
            if (strstr(class_getName(currentClass), kCraftiumFloatingSuffix) == nullptr) {
                // Remember original class and promote to floating subclass
                objc_setAssociatedObject(window, kCraftiumOriginalWindowClassKey, currentClass, OBJC_ASSOCIATION_ASSIGN);
                if (Class floatingClass = craftiumEnsureFloatingSubclass(currentClass)) {
                    object_setClass(window, floatingClass);
                }
            }

            // Apply the non-activating panel style mask so clicks do not raise other windows
            NSUInteger currentMask = [window styleMask];
            if (!(currentMask & NSWindowStyleMaskNonactivatingPanel)) {
                NSNumber* storedMask = [NSNumber numberWithUnsignedInteger:currentMask];
                objc_setAssociatedObject(window, kCraftiumOriginalStyleMaskKey, storedMask, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
                [window setStyleMask:(currentMask | NSWindowStyleMaskNonactivatingPanel)];
            }

            // Use NSPopUpMenuWindowLevel (higher than NSFloatingWindowLevel)
            // This is level 101, which should stay above most applications including games
            [window setLevel:NSPopUpMenuWindowLevel];

            // Configure window behavior:
            // - CanJoinAllSpaces: Appears on all virtual desktops/Spaces
            // - FullScreenAuxiliary: Appears with fullscreen apps
            // - Stationary: Prevents window from being moved by Exposé
            // - IgnoresCycle: Excluded from Cmd+Tab and normal window cycling
            //
            // Note: We specifically EXCLUDE ParticipatesInCycle to prevent the window
            // from interfering with normal app switching and bringing other windows forward
            [window setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces |
                                          NSWindowCollectionBehaviorFullScreenAuxiliary |
                                          NSWindowCollectionBehaviorStationary |
                                          NSWindowCollectionBehaviorIgnoresCycle];

            // Prevent window from activating the application when clicked
            // This keeps it as a passive overlay that doesn't steal focus
            [window setHidesOnDeactivate:NO];
        } else {
            // Restore original window class if we promoted it to a panel
            Class originalClass = (Class)objc_getAssociatedObject(window, kCraftiumOriginalWindowClassKey);
            if (originalClass) {
                object_setClass(window, originalClass);
                objc_setAssociatedObject(window, kCraftiumOriginalWindowClassKey, nil, OBJC_ASSOCIATION_ASSIGN);
            }

            // Restore original style mask if we modified it
            if (NSNumber* storedMask = (NSNumber*)objc_getAssociatedObject(window, kCraftiumOriginalStyleMaskKey)) {
                [window setStyleMask:[storedMask unsignedIntegerValue]];
                objc_setAssociatedObject(window, kCraftiumOriginalStyleMaskKey, nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
            }

            [window setLevel:NSNormalWindowLevel];

            // Reset to default behavior
            [window setCollectionBehavior:NSWindowCollectionBehaviorDefault];
        }
    }
}

void craftiumDeactivateApp(void) {
    NSApplication* app = [NSApplication sharedApplication];
    if ([app isActive]) {
        [app deactivate];
    }
}

#ifdef __cplusplus
}
#endif
