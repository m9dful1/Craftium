# Button Press Recorder - Design Document

This document outlines the core functionality of the Button Press Recorder application based on the Python implementation.

## Core Features

*   **Record Keyboard Events:** Capture global keyboard press and release events.
    *   Store key identifier (e.g., 'a', 'Key.shift').
    *   Store event state ('down' or 'up').
    *   Store precise timestamp relative to recording start.
*   **Playback Recorded Sequence:** Emulate the recorded keyboard events in the correct order and with accurate timing delays.
*   **Stop Playback:** Allow users to interrupt the playback process prematurely.
*   **Clear Sequence:** Discard the currently recorded sequence from memory.
*   **Set Repetition Count:** Specify the number of times the recorded sequence should be played back consecutively.
*   **Save Sequence:** Persist the recorded sequence to a file (JSON format).
*   **Load Sequence:** Load a previously saved sequence from a file (JSON format).
*   **View Sequence:** Display the recorded sequence details (key, state, timing) in a text area within the UI.
*   **Graphical User Interface (GUI):** Provide interactive controls for all functionalities.
    *   Buttons for Record, Stop Record, Play, Stop Playback, Clear, Expand/Collapse View, Clear Focus.
    *   SpinBox for Repeat Count.
    *   Status Label for feedback.
    *   Menu Bar for Save/Load actions.
*   **Always-on-Top:** Keep the application window visible above other windows.
*   **Focus Management:** Provide a mechanism to clear focus from UI elements to avoid interference with recording/playback.
*   **(Potential/Implied) Gamepad Support:** The sample JSON data suggests possible intent to handle gamepad inputs, although this is not implemented in the current keyboard-focused Python script. 