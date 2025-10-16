# Button Press Recorder - C++ Design Document

## Overview

The Button Press Recorder is a cross-platform desktop application built with C++ and Qt that allows users to record, save, and play back keyboard input sequences. It's designed to automate repetitive keyboard tasks by recording key presses and replaying them exactly as they were entered. 

## Architecture

The application follows a Model-View-Controller (MVC) pattern with additional components for background processing:

- **View**: Qt widget-based UI
- **Controller**: Main `ControllerApp` class that handles user input and coordinates between UI and background workers
- **Model**: Key event data structures and sequence storage
- **Workers**: Background thread handling for playback operations

## Core Components

### ControllerApp Class

The central controller class that:
- Manages the UI
- Handles global keyboard hooks for recording
- Controls playback operations
- Manages file operations
- Coordinates between UI and background threads

### PlaybackWorker Class

A worker class that runs in a separate thread to:
- Play back recorded sequences
- Handle timing between keystrokes
- Emulate key presses using platform-specific APIs
- Report completion status back to the main thread

### KeyEvent Structure

A cross-platform data structure representing keyboard events:
- Key name (string representation)
- State (up/down)
- Delay time (milliseconds)
- Platform-specific key codes

## Platform-Specific Implementations

### macOS Implementation

- Uses Carbon/CoreGraphics framework for global key event monitoring
- Implements CGEventTap for intercepting keyboard events
- Maps CGKeyCodes to human-readable strings using direct mapping tables
- Uses CoreGraphics events for playback

### Windows Implementation (Partially Implemented)

- Uses Windows Hook API for keyboard event capture
- Maps VK codes to string representations
- Uses SendInput API for playback

## Key Features

### Recording Mechanism

1. **Global Event Hooking**:
   - Intercepts all keyboard events at the system level
   - Records timing between key events with millisecond precision
   - Translates platform-specific key codes to cross-platform representations

2. **Event Storage**:
   - Maintains sequence of key events in memory
   - Each event contains key name, state, delay, and platform-specific codes
   - Cross-platform compatibility in the data model

### Playback Mechanism

1. **Threaded Execution**:
   - Runs in a background thread to prevent UI freezing
   - Respects original timing between events
   - Can be stopped at any time

2. **Key Emulation**:
   - Uses platform-specific APIs to generate synthetic key events
   - Handles special keys and modifiers appropriately
   - Supports repeat functionality for multiple iterations

### Save/Load Functionality

1. **File Format**:
   - Uses JSON for sequence storage
   - Stores all event data including platform-specific codes
   - Human-readable format for debugging

2. **Persistence**:
   - Standard file dialogs for save/load operations
   - Complete serialization and deserialization of event data
   - Cross-platform file compatibility

## User Interface

1. **Main Controls**:
   - Recording start/stop buttons
   - Playback start/stop buttons
   - Sequence management (clear, save, load)
   - Repeat count spinner for multiple playbacks

2. **Information Display**:
   - Status messages with word wrap
   - Sequence details panel (expandable)
   - Always-on-top toggle for accessibility during recording

3. **Input Focus Management**:
   - Clear focus button to prevent accidental input
   - Focus handling during recording

## Technical Implementation Details

### Signal-Slot Mechanism

Qt's signal-slot system is used extensively for:
- UI event handling
- Thread communication
- Async operations

### Thread Management

1. **Main Thread**:
   - Handles UI and user interaction
   - Manages keyboard hooks
   - Coordinates operations

2. **Playback Thread**:
   - Dedicated thread for playback operations
   - Prevents UI freezing during playback
   - Communicates state via signals/slots

### Platform Abstraction

The application maintains platform independence via:
- Conditional compilation (#ifdef directives)
- Separate implementations for platform-specific functionality
- Common interfaces for cross-platform operations
- Unified data structures

## Build System

The project uses CMake for build configuration with:
- Cross-platform compatibility
- Automatic Qt resource handling (MOC, UIC, RCC)
- Platform-specific library linking
- Dependency management

## Current Limitations

1. **Windows Implementation**: 
   - Key event capture and playback for Windows is partially implemented

2. **Modifier Key Handling**: 
   - Special handling for modifier key combinations could be improved

3. **Error Handling**: 
   - More robust error recovery mechanisms could be implemented

4. **UI Responsiveness**: 
   - More visual feedback during operations would improve user experience

## Suggested Improvements

### Core Functionality Enhancements

1. **Complete Windows Implementation**
   - Finish the Windows keyboard hook implementation
   - Add comprehensive testing on Windows platforms
   - Ensure consistent behavior between macOS and Windows versions

2. **Advanced Sequence Recording**
   - Add support for mouse events (clicks, moves, scrolls)
   - Implement relative vs. absolute positioning options for mouse events
   - Add support for special system keys and multimedia controls

3. **Sequence Editing**
   - Create a dedicated sequence editor UI to:
     - Modify timing between events
     - Add/remove individual events
     - Create sequences manually without recording
   - Add ability to merge multiple sequences

4. **Conditional Logic**
   - Add simple scripting capabilities:
     - Wait for specific screen conditions
     - Basic if/then branching based on pixel color or image recognition
     - Looping specific subsequences

### User Interface Improvements

1. **Modern UI Enhancements**
   - Implement a dark mode theme option
   - Add customizable keyboard shortcuts for application functions
   - Improve layout for better screen space utilization

2. **Visual Feedback**
   - Add a visual indicator when recording is active (e.g., red border around window)
   - Show real-time playback progress (e.g., progress bar or highlighting current step)
   - Implement an optional on-screen display showing keypresses during recording

3. **System Integration**
   - Add system tray icon for quick access
   - Implement startup with system option
   - Support for global hotkeys to control recording/playback

### Reliability & Performance

1. **Error Handling & Recovery**
   - Add better error handling for failed key events
   - Implement auto-save for recovery from crashes
   - Add validation for saved sequence files

2. **Performance Optimizations**
   - Optimize memory usage for very long sequences
   - Reduce CPU usage during idle periods
   - Improve thread synchronization for complex sequences

3. **Testing & Validation**
   - Add comprehensive unit tests
   - Implement automated UI testing
   - Add logging/diagnostics for troubleshooting

### Advanced Features

1. **Sequence Organization**
   - Add library management for multiple sequences
   - Implement categories and tagging
   - Add search functionality for finding sequences

2. **Automation Scheduling**
   - Add ability to schedule playback at specific times
   - Implement trigger-based execution (e.g., when an application launches)
   - Add repeat options (daily, weekly, etc.)

3. **Integration Capabilities**
   - Export/import in standardized formats
   - Add plugin system for extensibility
   - Implement network control API for remote triggering

4. **Accessibility Enhancements**
   - Improve screen reader compatibility
   - Add keyboard-only operation mode
   - Implement internationalization for multiple languages

These improvements would transform the Button Press Recorder from a simple utility into a powerful automation tool suitable for a wide range of productivity tasks while maintaining its core simplicity for basic recording needs.

## File Structure

- **include/**: Header files
  - `controllerapp.h`: Main controller class declaration
  - `playbackworker.h`: Playback worker class declaration

- **src/**: Implementation files
  - `main.cpp`: Application entry point
  - `controllerapp.cpp`: Main controller implementation
  - `playbackworker.cpp`: Playback worker implementation

- **build/**: CMake build directory (generated)

## Development Considerations

1. **Qt Version Compatibility**: 
   - Designed for Qt6 but could be adapted for Qt5

2. **C++ Standard**: 
   - Uses C++17 features

3. **Resource Management**:
   - RAII principles for resource cleanup
   - Smart thread management
   - Proper cleanup in destructors

4. **Error Handling**:
   - Uses Qt's debug output system
   - User-facing error messages
   - Graceful degradation when features are unavailable 