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
            [window setLevel:NSFloatingWindowLevel];
        } else {
            [window setLevel:NSNormalWindowLevel];
        }
    }
}

#ifdef __cplusplus
}
#endif
