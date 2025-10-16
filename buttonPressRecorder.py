import sys
import os
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QPushButton, QSpinBox, QLabel, QTextEdit, QMenuBar, QFileDialog
from PyQt5.QtCore import QTimer, QEventLoop, QRect, QPropertyAnimation, Qt
import time
import json
import logging
from pynput import keyboard
import pyautogui

# Set up logging
logging.basicConfig(filename='app.log', level=logging.DEBUG)

# Main GUI class
class ControllerApp(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

        # Sequence for storing key press data
        self.sequence = []
        self.recording = False
        self.playing = False
        self.explanation_visible = False

        # Listener for keyboard events
        self.listener = keyboard.Listener(on_press=self.on_key_press, on_release=self.on_key_release)
        self.listener.start()

    def initUI(self):
        self.setWindowTitle("Keyboard Recorder")
        self.setGeometry(100, 100, 300, 200)
        self.setWindowFlag(Qt.WindowStaysOnTopHint)

        # Menu Bar
        menubar = QMenuBar(self)
        file_menu = menubar.addMenu("File")

        save_action = file_menu.addAction("Save")
        save_action.triggered.connect(self.save_sequence)

        load_action = file_menu.addAction("Load")
        load_action.triggered.connect(self.load_sequence)

        layout = QVBoxLayout()
        layout.setMenuBar(menubar)

        self.record_button = QPushButton("Start Recording", self)
        self.record_button.clicked.connect(self.start_recording)
        layout.addWidget(self.record_button)

        self.stop_record_button = QPushButton("Stop Recording", self)
        self.stop_record_button.clicked.connect(self.stop_recording)
        layout.addWidget(self.stop_record_button)

        self.play_button = QPushButton("Play", self)
        self.play_button.clicked.connect(self.play_sequence)
        layout.addWidget(self.play_button)

        self.stop_button = QPushButton("Stop", self)
        self.stop_button.clicked.connect(self.stop_sequence)
        layout.addWidget(self.stop_button)

        self.clear_button = QPushButton("Clear Sequence", self)
        self.clear_button.clicked.connect(self.clear_sequence)
        layout.addWidget(self.clear_button)

        self.counter_label = QLabel("Repeat Count:", self)
        layout.addWidget(self.counter_label)

        self.counter = QSpinBox(self)
        self.counter.setMinimum(1)
        self.counter.setMaximum(100)
        layout.addWidget(self.counter)

        self.status_label = QLabel("Status: Idle", self)
        layout.addWidget(self.status_label)

        self.expand_button = QPushButton("▼", self)
        self.expand_button.clicked.connect(self.toggle_button_input)
        layout.addWidget(self.expand_button)

        self.text_edit = QTextEdit(self)
        self.text_edit.setVisible(False)
        layout.addWidget(self.text_edit)

        self.clear_focus_button = QPushButton("Clear Focus", self)
        self.clear_focus_button.clicked.connect(self.clear_focus)
        layout.addWidget(self.clear_focus_button)

        self.setLayout(layout)

        # Animation for expanding and contracting the window
        self.animation = QPropertyAnimation(self, b"geometry")
        self.animation.setDuration(450)

        # Ensure no widget has focus when launching
        self.setFocusPolicy(Qt.NoFocus)

    def start_recording(self):
        if self.recording:
            logging.debug("Already recording, ignoring this request.")
            return

        self.sequence = []
        self.recording = True
        self.start_time = time.time()
        self.status_label.setText("Status: Recording...")
        logging.info("Recording started.")

    def stop_recording(self):
        if not self.recording:
            logging.debug("Not currently recording, nothing to stop.")
            return

        timestamp = time.time() - self.start_time
        self.sequence.append(('stop', 'recording', timestamp))
        self.recording = False
        self.status_label.setText("Status: Recording stopped.")
        logging.info("Recording stopped.")
        self.update_text_edit()

    def clear_sequence(self):
        self.sequence = []
        self.update_text_edit()
        self.status_label.setText("Status: Sequence cleared.")
        logging.info("Sequence cleared.")

    def clear_focus(self):
        self.setFocus()  # This ensures the focus is on the main widget, not any button or input element
        self.status_label.setText("Status: Focus cleared.")
        logging.info("Focus cleared.")

    def on_key_press(self, key):
        if self.recording:
            timestamp = time.time() - self.start_time
            try:
                key_name = key.char if hasattr(key, 'char') and key.char is not None else str(key)
                self.sequence.append((key_name, 'down', timestamp))
                self.status_label.setText(f"Status: Key {key_name} down at {timestamp:.2f}s")
                logging.debug(f"Recorded key {key_name} down at {timestamp:.2f}s")
            except AttributeError:
                logging.error(f"Error recording key press: {key}")

    def on_key_release(self, key):
        if self.recording:
            timestamp = time.time() - self.start_time
            try:
                key_name = key.char if hasattr(key, 'char') and key.char is not None else str(key)
                self.sequence.append((key_name, 'up', timestamp))
                self.status_label.setText(f"Status: Key {key_name} up at {timestamp:.2f}s")
                logging.debug(f"Recorded key {key_name} up at {timestamp:.2f}s")
            except AttributeError:
                logging.error(f"Error recording key release: {key}")

    def play_sequence(self):
        if not self.sequence:
            self.status_label.setText("Status: No sequence recorded.")
            logging.warning("Attempted to play sequence with no recorded data.")
            return

        self.playing = True
        repeat_count = self.counter.value()
        logging.info(f"Playing sequence {repeat_count} times.")

        for _ in range(repeat_count):
            if not self.playing:
                break
            for i in range(len(self.sequence)):
                if not self.playing:
                    break
                key, state, timestamp = self.sequence[i]
                delay = timestamp - self.sequence[i - 1][2] if i > 0 else timestamp
                loop = QEventLoop()
                QTimer.singleShot(int(delay * 1000), loop.quit)
                loop.exec_()
                if key != 'stop':
                    self.emulate_key_press(key, state)

        self.status_label.setText("Status: Playback finished.")
        self.playing = False
        logging.info("Playback finished.")

    def emulate_key_press(self, key, state):
        # Handle special keys and normal keys differently
        special_keys = {
            'Key.enter': 'enter',
            'Key.space': 'space',
            'Key.shift': 'shift',
            'Key.ctrl': 'ctrl',
            'Key.alt': 'alt',
            'Key.cmd': 'command'
        }

        try:
            if state == 'down':
                if key in special_keys:
                    pyautogui.keyDown(special_keys[key])
                elif len(key) == 1:
                    pyautogui.keyDown(key)
            elif state == 'up':
                if key in special_keys:
                    pyautogui.keyUp(special_keys[key])
                elif len(key) == 1:
                    pyautogui.keyUp(key)
            self.status_label.setText(f"Emulating key {key} {state}")
            logging.debug(f"Emulated key {key} {state}")
        except ValueError:
            logging.error(f"Error emulating key press: {key}")

    def stop_sequence(self):
        self.playing = False
        self.status_label.setText("Status: Stopped")
        logging.info("Playback stopped by user.")

    def update_text_edit(self):
        if not self.sequence:
            self.text_edit.setText("")
            return

        text_lines = []
        for i in range(len(self.sequence)):
            key, state, timestamp = self.sequence[i]
            if i == 0:
                text_lines.append(f"<wait {timestamp:.2f}s>")
            else:
                prev_timestamp = self.sequence[i - 1][2]
                wait_time = timestamp - prev_timestamp
                text_lines.append(f"Key {key} {state} <wait {wait_time:.2f}s>")
        text_lines[-1] = f"Key {self.sequence[-1][0]} {self.sequence[-1][1]}"

        self.text_edit.setText("\n".join(text_lines))

    def toggle_button_input(self):
        current_geometry = self.geometry()
        adjustment = 100

        if self.explanation_visible:
            self.text_edit.setVisible(False)
            self.expand_button.setText("▼")
            new_height = current_geometry.height() - self.text_edit.height() - adjustment
        else:
            new_height = current_geometry.height() + self.text_edit.height() + adjustment
            QTimer.singleShot(450, lambda: self.text_edit.setVisible(True))
            self.expand_button.setText("▲")

        new_geometry = QRect(current_geometry.x(), current_geometry.y(), current_geometry.width(), new_height)
        self.animation.setStartValue(current_geometry)
        self.animation.setEndValue(new_geometry)
        self.animation.start()
        self.explanation_visible = not self.explanation_visible

    def save_sequence(self):
        options = QFileDialog.Options()
        file_name, _ = QFileDialog.getSaveFileName(self, "Save Sequence", "", "JSON Files (*.json);;All Files (*)", options=options)
        if file_name:
            with open(file_name, 'w') as file:
                json.dump(self.sequence, file)
            self.status_label.setText("Status: Sequence saved.")
            logging.info(f"Sequence saved to {file_name}.")

    def load_sequence(self):
        options = QFileDialog.Options()
        file_name, _ = QFileDialog.getOpenFileName(self, "Load Sequence", "", "JSON Files (*.json);;All Files (*)", options=options)
        if file_name:
            with open(file_name, 'r') as file:
                self.sequence = json.load(file)
            self.update_text_edit()
            self.status_label.setText("Status: Sequence loaded.")
            logging.info(f"Sequence loaded from {file_name}.")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = ControllerApp()
    ex.show()
    sys.exit(app.exec_())
