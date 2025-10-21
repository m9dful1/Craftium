# Craftium - Keyboard Automation Tool

![Build Status](https://github.com/m9dful1/Craftium/workflows/Build%20Craftium/badge.svg)
![Continuous Build](https://github.com/m9dful1/Craftium/workflows/Continuous%20Build/badge.svg)

Craftium is a powerful keyboard automation tool designed to record and replay keyboard sequences with precise timing. Perfect for automating repetitive tasks in productivity workflows, software testing, data entry, or any application that accepts keyboard input.

**IMPORTANT DISCLAIMER**: This software is intended for productivity automation and testing purposes only. Users are solely responsible for ensuring their use complies with the terms of service of any software they interact with. Use in online games or other software may violate terms of service and result in account penalties or bans.

## Features

- ⌨️ **Precise Keyboard Recording**: Capture all keystrokes with exact timing
- ⏱️ **Replay with Delays**: Playback sequences with the exact timing they were recorded
- 💾 **Save & Load**: Save sequences as JSON files for later use
- 🔄 **Repeat Playback**: Set custom repeat counts for sequences
- 🎨 **Dark Mode**: Built-in dark theme for comfortable use
- 📌 **Always on Top**: Enhanced window priority that defers focus back to the underlying app
- 🖥️ **macOS Native**: Optimized for macOS with full system integration

## Use Cases

Craftium is designed for productivity and automation tasks:

- **Software Testing**: Automate repetitive QA test scenarios and regression testing
- **Data Entry**: Speed up form filling and repetitive data input tasks
- **Content Creation**: Automate overlay controls for streamers and content creators
- **Accessibility**: Assist users with repetitive strain injuries or mobility limitations
- **Development**: Test keyboard-heavy applications and shortcuts
- **Productivity Workflows**: Automate common tasks across multiple applications

## Download

### Latest Build
Download the latest automated build from the [Continuous Release](https://github.com/m9dful1/Craftium/releases/tag/continuous) page.

### Stable Releases
For stable, tested releases, visit the [Releases](https://github.com/m9dful1/Craftium/releases) page.

## Installation

### macOS

1. Download `Craftium-macOS-*.dmg` from the releases page
2. Open the DMG file
3. Drag `Craftium.app` to your Applications folder
4. **First launch**: Right-click the app and select "Open" to bypass Gatekeeper
5. Grant **Accessibility** and **Input Monitoring** permissions:
   - Move `Craftium.app` to your Applications folder before granting access
   - Go to System Settings → Privacy & Security
   - Under **Accessibility**, enable Craftium (click the lock to make changes)
   - Under **Input Monitoring**, enable Craftium (add it with the “+” button if needed)
   - Quit and relaunch Craftium

## Usage

### Recording a Sequence
1. Click **"Start Recording"**
2. Press the keys you want to record
3. Click **"Stop Recording"** when done
4. Your sequence is now ready to replay

### Playing Back
1. Set the **Repeat Count** (1-100)
2. Click **"Play"**
3. You'll have 2 seconds to switch to your target application
4. The sequence will play automatically

### Saving & Loading
- **Save**: File → Save Recording (saves as .json)
- **Load**: File → Load Recording

### Viewing Sequences
- Click **"▼ Show Sequence Details"** to see all recorded keystrokes with timing

## macOS Permissions

Craftium listens for global key events using the macOS `CGEventTap` API. macOS protects this capability behind **Accessibility** and **Input Monitoring** permissions.

### Granting Permissions
1. Move `Craftium.app` to `/Applications`.
2. Open **System Settings → Privacy & Security**.
3. Enable Craftium under **Accessibility**.
4. Enable Craftium under **Input Monitoring**. If Craftium is missing, click the “+” button and add it from `/Applications`.
5. Quit and relaunch Craftium to apply the changes.

### After Updating Craftium
macOS ties the permission grant to the exact binary. When you download a new build, replace the existing `Craftium.app` inside `/Applications` to preserve the granted permissions. If you build or notarise your own release, sign it so macOS recognises the app as the same binary:

```bash
codesign --force --deep --sign - /Applications/Craftium.app
```

Replace `-` with a Developer ID certificate if you have one to avoid Gatekeeper warnings.

## Building from Source

### Prerequisites

- macOS 11.0 or later
- CMake 3.16 or later
- Qt 6.7.3
- Xcode Command Line Tools

### Installation Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/m9dful1/Craftium.git
   cd Craftium
   ```

2. **Install Qt** (using Homebrew)
   ```bash
   brew install qt@6
   ```

3. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

4. **Configure and build**
   ```bash
   cmake ..
   make
   ```

5. **Run the application**
   ```bash
   ./Craftium
   ```

### Build Configuration

The project uses CMake with the following features:
- C++17 standard
- Objective-C++ support for macOS integration
- Qt6 Widgets, Core, and Gui modules
- Native macOS frameworks: CoreGraphics, Carbon, AppKit

## CI/CD

This project uses GitHub Actions for automated builds:

### Continuous Builds
- Triggered on every push to `main`/`master`
- Creates a "continuous" release with the latest build
- DMG files are available for download immediately

### Release Builds
- Triggered when creating a new release tag
- Builds are automatically attached to the release
- Use semantic versioning for tags (e.g., `v1.0.0`)

### Creating a Release

```bash
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin v1.0.0
```

Then create a release on GitHub using that tag, and the build will automatically attach.

> Tip: Provide a `MACOS_CODESIGN_IDENTITY` GitHub secret with your Apple Developer ID certificate if you want the CI workflow to produce fully signed builds. Without it, the workflow falls back to ad-hoc signing.

## Technical Details

### Architecture

- **Main Window**: Qt6 Widgets-based UI
- **Recording**: Platform-specific global keyboard hooks
  - macOS: CGEventTap with Accessibility and Input Monitoring permissions
  - Windows: Low-level keyboard hooks
- **Playback**: Worker thread for non-blocking execution
- **Thread Safety**: QMutex protection for sequence data

### Enhanced Features

- **Always on Top**: Uses `NSFloatingWindowLevel` on macOS for maximum window priority
- **Thread-Safe**: All sequence operations protected with mutex locks
- **Non-Blocking UI**: Playback runs on dedicated worker thread

## Troubleshooting

### Recording doesn't work
- Ensure both Accessibility **and** Input Monitoring permissions are granted (System Settings → Privacy & Security)
- Move Craftium to `/Applications`, grant permissions, then restart the app
- Try restarting the application after changing permissions

### Playback doesn't work in target app
- Ensure the target application is in focus before playback starts
- For fullscreen applications, try windowed mode instead
- Some applications may block simulated input for security reasons

### Window not staying on top
- Ensure "Always on Top" checkbox is enabled
- Try toggling it off and back on
- Some fullscreen applications may override window levels

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

Copyright © 2024 SpiritWise Studios LLC

Designed by Jeremy Matthews

## Credits

Built with:
- [Qt 6](https://www.qt.io/) - Cross-platform application framework
- [CMake](https://cmake.org/) - Build system
- [GitHub Actions](https://github.com/features/actions) - CI/CD

## Support

If you find Craftium useful, consider supporting development:
- [PayPal](https://www.paypal.me/spiritwise)
- [Ko-fi](https://ko-fi.com/spiritwise)
- [Patreon](https://patreon.com/SpiritWiseStudios)
