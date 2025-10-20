#import <AppKit/AppKit.h>
#include <QtCore/Qt>
#import <objc/runtime.h>

#ifdef __cplusplus
extern "C" {
#endif

// Custom floating panel class to keep Craftium on top without activating other apps
@interface CraftiumFloatingPanel : NSPanel
@end

@implementation CraftiumFloatingPanel
// Allow the panel to receive focus when explicitly requested (e.g., for text input)
- (BOOL)canBecomeKeyWindow {
    return YES;
}

// Prevent the panel from becoming the main window so other apps keep their focus
- (BOOL)canBecomeMainWindow {
    return NO;
}

// Only become key when the system explicitly needs it
- (BOOL)becomesKeyOnlyIfNeeded {
    return YES;
}
@end

static const void* kCraftiumOriginalWindowClassKey = &kCraftiumOriginalWindowClassKey;

// Helper function to set macOS window level
void setMacOSWindowLevel(void* nsViewPtr, bool floatingLevel) {
    NSView* view = (__bridge NSView*)nsViewPtr;
    NSWindow* window = [view window];

    if (window) {
        if (floatingLevel) {
            // Remember the original window class so we can restore it later
            if (![window isKindOfClass:[CraftiumFloatingPanel class]]) {
                Class originalClass = object_getClass(window);
                objc_setAssociatedObject(window, kCraftiumOriginalWindowClassKey, originalClass, OBJC_ASSOCIATION_ASSIGN);
                object_setClass(window, [CraftiumFloatingPanel class]);
            }

            CraftiumFloatingPanel* panel = (CraftiumFloatingPanel*)window;
            [panel setFloatingPanel:YES];
            [panel setWorksWhenModal:YES];

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
            if ([window isKindOfClass:[CraftiumFloatingPanel class]]) {
                CraftiumFloatingPanel* panel = (CraftiumFloatingPanel*)window;
                [panel setFloatingPanel:NO];
                [panel setWorksWhenModal:NO];
            }

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
}

#ifdef __cplusplus
}
#endif
