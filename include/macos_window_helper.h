#ifndef MACOS_WINDOW_HELPER_H
#define MACOS_WINDOW_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

// Helper function to set macOS window level
// Pass the WId (void*) from Qt's winId() and whether to use floating level
void setMacOSWindowLevel(void* nsViewPtr, bool floatingLevel);
void craftiumDeactivateApp(void);

#ifdef __cplusplus
}
#endif

#endif // MACOS_WINDOW_HELPER_H
