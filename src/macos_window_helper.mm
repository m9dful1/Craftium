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

            // Configure window to appear in Mission Control and all Spaces
            [window setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces |
                                          NSWindowCollectionBehaviorParticipatesInCycle |
                                          NSWindowCollectionBehaviorFullScreenAuxiliary];
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
