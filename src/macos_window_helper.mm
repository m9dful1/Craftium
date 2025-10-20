#import <AppKit/AppKit.h>
#include <QtCore/Qt>

#ifdef __cplusplus
extern "C" {
#endif

// Helper function to set macOS window level
void setMacOSWindowLevel(void* nsViewPtr, bool floatingLevel) {
    NSView* view = (__bridge NSView*)nsViewPtr;
    NSWindow* window = [view window];

    if (window) {
        if (floatingLevel) {
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
            [window setLevel:NSNormalWindowLevel];

            // Reset to default behavior
            [window setCollectionBehavior:NSWindowCollectionBehaviorDefault];
        }
    }
}

#ifdef __cplusplus
}
#endif
