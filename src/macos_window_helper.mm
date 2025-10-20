#import <AppKit/AppKit.h>
#include <QtCore/Qt>
#import <objc/runtime.h>
#import <CoreGraphics/CoreGraphics.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static const void* kCraftiumOriginalWindowClassKey = &kCraftiumOriginalWindowClassKey;
static const char* kCraftiumFloatingSuffix = "_CraftiumFloating";
static pid_t gCraftiumLastForegroundPID = -1;
static id gCraftiumActivationObserver = nil;
static NSInteger gCraftiumWindowNumber = 0;

static pid_t craftiumFindFrontmostPIDBelowWindow(void);

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
        gCraftiumWindowNumber = [window windowNumber];
        if (floatingLevel) {
            Class currentClass = object_getClass(window);
            if (strstr(class_getName(currentClass), kCraftiumFloatingSuffix) == nullptr) {
                // Remember original class and promote to floating subclass
                objc_setAssociatedObject(window, kCraftiumOriginalWindowClassKey, currentClass, OBJC_ASSOCIATION_ASSIGN);
                if (Class floatingClass = craftiumEnsureFloatingSubclass(currentClass)) {
                    object_setClass(window, floatingClass);
                }
            }

            // Use NSPopUpMenuWindowLevel (higher than NSFloatingWindowLevel)
            // This is level 101, which should stay above most applications including fullscreen apps
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

            [window setLevel:NSNormalWindowLevel];

            // Reset to default behavior
            [window setCollectionBehavior:NSWindowCollectionBehaviorDefault];
        }
    }
    else {
        gCraftiumWindowNumber = 0;
        gCraftiumLastForegroundPID = -1;
    }
}

void craftiumDeactivateApp(void) {
    NSApplication* app = [NSApplication sharedApplication];
    if ([app isActive]) {
        [app deactivate];
    }
}

void craftiumInstallFrontmostObserver(void) {
    if (gCraftiumActivationObserver != nil) {
        return;
    }

    NSWorkspace* workspace = [NSWorkspace sharedWorkspace];
    NSNotificationCenter* center = [workspace notificationCenter];
    NSRunningApplication* selfApp = [NSRunningApplication currentApplication];

    NSRunningApplication* currentFront = [workspace frontmostApplication];
    if (currentFront && ![currentFront isEqual:selfApp]) {
        gCraftiumLastForegroundPID = currentFront.processIdentifier;
        NSLog(@"Craftium: initial frontmost app %@ (%d)", currentFront.localizedName, currentFront.processIdentifier);
    }

    gCraftiumActivationObserver = [center addObserverForName:NSWorkspaceDidActivateApplicationNotification
                                                       object:nil
                                                        queue:[NSOperationQueue mainQueue]
                                                   usingBlock:^(NSNotification* note) {
        id value = note.userInfo[NSWorkspaceApplicationKey];
        if ([value isKindOfClass:[NSRunningApplication class]]) {
            NSRunningApplication* activatedApp = (NSRunningApplication*)value;
            if (![activatedApp isEqual:selfApp]) {
                gCraftiumLastForegroundPID = activatedApp.processIdentifier;
                NSLog(@"Craftium: observed frontmost app %@ (%d)", activatedApp.localizedName, activatedApp.processIdentifier);
            }
        }
    }];
}

void craftiumReactivateLastForegroundApp(void) {
    pid_t candidatePID = craftiumFindFrontmostPIDBelowWindow();
    if (candidatePID > 0) {
        gCraftiumLastForegroundPID = candidatePID;
        NSLog(@"Craftium: inferred frontmost PID below window = %d", candidatePID);
    }

    if (gCraftiumLastForegroundPID <= 0) {
        NSLog(@"Craftium: no stored front app PID (=%d); skipping reactivation", gCraftiumLastForegroundPID);
        return;
    }

    NSRunningApplication* target = [NSRunningApplication runningApplicationWithProcessIdentifier:gCraftiumLastForegroundPID];
    if (!target || [target isTerminated]) {
        NSLog(@"Craftium: stored app PID %d unavailable (target=%@)", gCraftiumLastForegroundPID, target);
        gCraftiumLastForegroundPID = -1;
        return;
    }

    NSLog(@"Craftium: reactivating app %@ (%d)", target.localizedName, gCraftiumLastForegroundPID);
    [target activateWithOptions:0];
}

#ifdef __cplusplus
}
#endif
static pid_t craftiumFindFrontmostPIDBelowWindow(void) {
    if (gCraftiumWindowNumber == 0) {
        return -1;
    }

    CFArrayRef windowInfoList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenBelowWindow | kCGWindowListOptionOnScreenOnly, (CGWindowID)gCraftiumWindowNumber);
    if (!windowInfoList) {
        return -1;
    }

    pid_t result = -1;
    CFIndex count = CFArrayGetCount(windowInfoList);
    for (CFIndex i = 0; i < count; ++i) {
        CFDictionaryRef info = (CFDictionaryRef)CFArrayGetValueAtIndex(windowInfoList, i);
        if (!info) continue;

        CFNumberRef pidNumber = (CFNumberRef)CFDictionaryGetValue(info, kCGWindowOwnerPID);
        CFNumberRef alphaNumber = (CFNumberRef)CFDictionaryGetValue(info, kCGWindowAlpha);
        CFNumberRef layerNumber = (CFNumberRef)CFDictionaryGetValue(info, kCGWindowLayer);

        if (!pidNumber) continue;

        CGFloat alpha = 1.0;
        if (alphaNumber) {
            CFNumberGetValue(alphaNumber, kCFNumberCGFloatType, &alpha);
        }

        int layer = 0;
        if (layerNumber) {
            CFNumberGetValue(layerNumber, kCFNumberIntType, &layer);
        }

        if (alpha <= 0.0 || layer != 0) {
            continue;
        }

        CFNumberGetValue(pidNumber, kCFNumberSInt32Type, &result);
        break;
    }

    CFRelease(windowInfoList);
    return result;
}
