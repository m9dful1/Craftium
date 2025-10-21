# Craftium – Design Overview

This document captures the intended behaviour of the current Craftium desktop application, which rewrites the original “Button Press Recorder” Python prototype as a cross-platform C++/Qt application. It reflects the implementation that exists in source control today.

## 1. Product Goals

- Provide a desktop automation helper that can record global keyboard activity, replay the captured sequence with precise timings, and repeat playback as required.
- Offer a compact user interface that stays usable while users switch to their target application (including games or productivity tools).
- Give clear guidance around system permissions that are required on macOS (Accessibility and Input Monitoring) and automatically handle loss of permissions between launches.
- Expose quick note‑taking and runbook capabilities so users can retain context alongside each automation.

## 2. Platform Support

Craftium targets macOS and Windows using Qt 6.

- **Recording**
  - macOS: Uses a CGEventTap installed at `kCGHIDEventTap` in listen-only mode.
  - Windows: Uses the Win32 low-level keyboard hook (`WH_KEYBOARD_LL`).
- **Playback**
  - Performed on a dedicated `PlaybackWorker` living in its own `QThread` to keep the UI responsive.
- **Build & Packaging**
  - CMake project with GitHub Actions workflows producing continuous and tagged DMG releases for macOS. (Windows packaging is not yet automated.)

## 3. High-Level Architecture

```
┌────────────────────┐
│   ControllerApp    │  Main QWidget-based UI
├────────────────────┤
│ Playback controls  │  Record/Stop/Play/Stop/Clear buttons
│ Sequence panel     │  Animated slide-out history view
│ Notes panel        │  Animated slide-out notes editor
│ Status & menu bar  │  Feedback + standard actions
└────────┬───────────┘
         │signals/slots
┌────────▼───────────┐
│  PlaybackWorker    │  Runs on background QThread to replay events
└────────┬───────────┘
         │global hooks (platform-specific)
┌────────▼───────────┐
│ macOS CGEventTap   │
│ Windows hook       │  Injects back into ControllerApp via callbacks
└────────────────────┘
```

## 4. User Interface & Behaviour

### 4.1 Layout
- Vertical stack of primary controls: Record, Stop Recording, Play, Stop, Clear Sequence.
- Repeat count spinner (`1–100`).
- Toggle buttons for the two slide-out panels:
  - **Notes** (right-aligned drawer) – editable `QTextEdit` with Load/Save buttons.
  - **Show/Hide Sequence Details** – bottom drawer displaying each recorded key (`key`, `state`, `delay`).
- Status label anchored beneath the controls. The label elides long messages and trims filesystem paths to filenames so the window width remains stable.
- Menu bar with File (Save/Load/Clear), View (Dark Mode, Always on Top, toggle panels), Help (About, Help, etc.).

### 4.2 Always on Top
- Defaults to **off** on first launch; persisted via `QSettings`.
- When enabled:
  - Window gain events are intercepted so Craftium immediately hands focus back to the previously active application while keeping its window above others.
  - Internal widgets (notes editor, dialogs, file pickers) temporarily suppress the focus guard so the user can interact with them without losing focus.

### 4.3 Notes Panel
- Slides in/out with `QPropertyAnimation` on width.
- Content is plain text. Users can load or save `.txt`/`.md` files via standard dialogs.
- Status messages report success using the trimmed filename.

### 4.4 Sequence Panel
- Slides vertically using `QPropertyAnimation` on height when the “Show Sequence Details” button is toggled.
- Display is read-only and refreshed whenever the panel opens, showing the in-memory sequence.

### 4.5 Status Messaging
- `updateStatusLabel` elides text to the available width and trims file paths to filenames.
- Status label stays the same width as the control column so the window no longer grows to accommodate long messages.

## 5. Recording Pipeline

1. **Start Recording**
   - Clears the in-memory sequence.
   - Resets timestamp baseline.
   - Installs the appropriate platform hook (CGEventTap or Win32 hook).

2. **Event Capture**
   - Each event stores: logical key string, state (`down`/`up`), delay since previous event, and platform-specific key code.
   - Sequence access is guarded by `QMutex` to protect against concurrent playback operations.

3. **Stop Recording**
   - Removes the hook and finalises current sequence.

## 6. Playback Pipeline

1. `startPlayback` copies the recorded sequence while holding the mutex to avoid blocking capture.
2. It signals `PlaybackWorker::doWork` on the dedicated thread, passing the sequence and repeat count.
3. Playback worker simulates key events using platform APIs (Win32 `SendInput`, macOS `CGEventPost`).
4. Completion or interruption emits `finished`, resetting UI state.

## 7. Sequence Persistence

- Saving writes JSON array entries: `{ "key", "state", "delay", platform key code }`.
- Loading validates the JSON, repopulates the sequence vector, and updates the display.
- Most recent directory is remembered by the native file dialog (no additional path persistence currently).

## 8. Notes Persistence

- Notes panel can load or save plain text files. If an operation fails the status label displays the error message.
- The application does not persist notes automatically; users must explicitly save.

## 9. Preferences & Settings

- `QSettings` keys:
  - `darkMode` – bool, default `true`.
  - `alwaysOnTop` – bool, default `false`.
  - `shownAccessibilityWelcome` – bool, indicates the user has seen the permissions instructions at least once.

## 10. macOS Permissions Flow

- On startup the app checks for Input Monitoring access by creating a temporary event tap.
- If permissions are missing, it triggers the macOS system prompt (`AXIsProcessTrustedWithOptions(prompt=true)`) and then shows Craftium’s own instruction dialog (forced on top) with deep links to the relevant System Settings panes.
- The dialog also appears the first time Craftium is launched even if permissions are already granted, ensuring the user understands the requirements.

## 11. Error Handling & Feedback

- File I/O errors (save/load notes or sequences) surface via `QMessageBox` warnings and status messages.
- Hook installation failures (permissions denied) produce status updates and dialogs, keeping the user informed.
- Playback guard rails prevent operations when recording is active, and vice versa, with clear feedback.

## 12. Future Considerations

- Reintroduce Windows packaging and macOS notarisation once Developer ID certificates are available.
- Investigate persistence for notes panel (auto-saving alongside sequences).
- Explore command shortcuts or scripting integration once the base UX is fully polished.

This document supersedes the original Python Button Press Recorder plan and should be kept in sync with ongoing C++/Qt development.

