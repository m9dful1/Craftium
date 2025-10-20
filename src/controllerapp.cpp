#include "../include/controllerapp.h"
#include "../include/playbackworker.h"
#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDateTime>
#include <QFileInfo>
#include <QSizePolicy>
#include <QStyle>
#include <QStyleFactory>
#include <QSettings>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>
#include <QTextBrowser>
#include <QSignalBlocker>
#include <QEvent>

#ifdef _WIN32
#include <windows.h>
// Define the key map for Windows at file scope or within the class scope
namespace { // Use an anonymous namespace to limit scope
// Make the map const as it's initialized once and never modified
const std::map<DWORD, std::string> vk_to_string_map = {
    {VK_BACK, "Backspace"}, {VK_TAB, "Tab"}, {VK_RETURN, "Enter"},
    {VK_SHIFT, "Shift"}, {VK_CONTROL, "Ctrl"}, {VK_MENU, "Alt"}, // VK_MENU is Alt
    {VK_PAUSE, "Pause"}, {VK_CAPITAL, "CapsLock"},
    {VK_ESCAPE, "Esc"}, {VK_SPACE, "Space"},
    {VK_PRIOR, "PageUp"}, {VK_NEXT, "PageDown"}, {VK_END, "End"}, {VK_HOME, "Home"},
    {VK_LEFT, "Left"}, {VK_UP, "Up"}, {VK_RIGHT, "Right"}, {VK_DOWN, "Down"},
    {VK_SELECT, "Select"}, {VK_PRINT, "Print"}, {VK_EXECUTE, "Execute"},
    {VK_SNAPSHOT, "PrintScreen"}, {VK_INSERT, "Insert"}, {VK_DELETE, "Delete"}, {VK_HELP, "Help"},
    {VK_LWIN, "LWin"}, {VK_RWIN, "RWin"}, {VK_APPS, "Apps"},
    {VK_SLEEP, "Sleep"},
    {VK_NUMPAD0, "Numpad0"}, {VK_NUMPAD1, "Numpad1"}, {VK_NUMPAD2, "Numpad2"},
    {VK_NUMPAD3, "Numpad3"}, {VK_NUMPAD4, "Numpad4"}, {VK_NUMPAD5, "Numpad5"},
    {VK_NUMPAD6, "Numpad6"}, {VK_NUMPAD7, "Numpad7"}, {VK_NUMPAD8, "Numpad8"},
    {VK_NUMPAD9, "Numpad9"},
    {VK_MULTIPLY, "NumpadMultiply"}, {VK_ADD, "NumpadAdd"}, {VK_SEPARATOR, "NumpadSeparator"},
    {VK_SUBTRACT, "NumpadSubtract"}, {VK_DECIMAL, "NumpadDecimal"}, {VK_DIVIDE, "NumpadDivide"},
    {VK_F1, "F1"}, {VK_F2, "F2"}, {VK_F3, "F3"}, {VK_F4, "F4"}, {VK_F5, "F5"}, {VK_F6, "F6"},
    {VK_F7, "F7"}, {VK_F8, "F8"}, {VK_F9, "F9"}, {VK_F10, "F10"}, {VK_F11, "F11"}, {VK_F12, "F12"},
    {VK_NUMLOCK, "NumLock"}, {VK_SCROLL, "ScrollLock"},
    {VK_LSHIFT, "LShift"}, {VK_RSHIFT, "RShift"},
    {VK_LCONTROL, "LCtrl"}, {VK_RCONTROL, "RCtrl"},
    {VK_LMENU, "LAlt"}, {VK_RMENU, "RAlt"},
    // Add other keys as needed, e.g., OEM keys for punctuation
    {VK_OEM_1, ";"}, {VK_OEM_PLUS, "="}, {VK_OEM_COMMA, ","}, {VK_OEM_MINUS, "-"},
    {VK_OEM_PERIOD, "."}, {VK_OEM_2, "/"}, {VK_OEM_3, "`"}, {VK_OEM_4, "["},
    {VK_OEM_5, "\\"}, {VK_OEM_6, "]"}, {VK_OEM_7, "'"}
};

// This map is modified by initialize_string_to_vk_map, so cannot be const here.
std::map<std::string, WORD> string_to_vk_map; // Reverse map

// Function to initialize the reverse map
void initialize_string_to_vk_map() {
    if (string_to_vk_map.empty()) {
        for (const auto& pair : vk_to_string_map) {
            string_to_vk_map[pair.second] = static_cast<WORD>(pair.first);
        }
        // Add mappings for alphanumeric keys ('A'-'Z', '0'-'9')
        for (char c = 'A'; c <= 'Z'; ++c) {
            string_to_vk_map[std::string(1, c)] = static_cast<WORD>(c);
        }
        for (char c = '0'; c <= '9'; ++c) {
            string_to_vk_map[std::string(1, c)] = static_cast<WORD>(c);
        }
    }
}

} // end anonymous namespace

// Windows global hook state
// Note: These are intentionally global for single-instance application use.
// The Windows low-level keyboard hook callback requires a C-style function pointer,
// which necessitates global state to access the ControllerApp instance.
// These variables are properly initialized and cleaned up in start/stopGlobalKeyListener.
// WARNING: Do not create multiple instances of ControllerApp with recording enabled.
HHOOK keyboardHook = NULL;
static ControllerApp* g_appInstance = nullptr;

// Windows keyboard hook callback
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // Check if the hook is being installed - save the app instance
    if (nCode == HC_ACTION) {
        if (lParam && g_appInstance && g_appInstance->isRecording()) {
            KBDLLHOOKSTRUCT* pKbStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vkCode = pKbStruct->vkCode;
            
            bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
            
            if (isKeyDown || isKeyUp) {
                // Call the record method from the app instance
                g_appInstance->recordKeyEvent(vkCode, isKeyDown);
            }
        }
    }
    
    // Always call the next hook in the chain
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void ControllerApp::startGlobalKeyListener() {
    if (keyboardHook) {
        qDebug() << "Windows keyboard hook already installed.";
        return;
    }
    
    // Save the instance pointer for the hook callback
    g_appInstance = this;
    
    // Install the low-level keyboard hook
    keyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,          // Low-level keyboard events
        LowLevelKeyboardProc,    // Hook procedure
        GetModuleHandle(NULL),   // Instance handle
        0                        // Hook all threads
    );
    
    if (!keyboardHook) {
        DWORD error = GetLastError();
        qCritical() << "Failed to install keyboard hook. Error code:" << error;
        QMessageBox::critical(this, "Error", 
                             "Failed to install keyboard hook. Error code: " + QString::number(error));
        return;
    }
    
    qDebug() << "Windows global keyboard hook installed successfully.";
}

void ControllerApp::stopGlobalKeyListener() {
    if (keyboardHook) {
        if (UnhookWindowsHookEx(keyboardHook)) {
            keyboardHook = NULL;
            g_appInstance = nullptr;
            qDebug() << "Windows global keyboard hook removed successfully.";
        } else {
            DWORD error = GetLastError();
            qWarning() << "Failed to remove keyboard hook. Error code:" << error;
        }
    }
}

void ControllerApp::recordKeyEvent(DWORD vkCode, bool isPress) {
    // This method is called from the hook callback, which runs in a different thread
    // Be careful with UI operations here
    
    if (!recording) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEventTime).count();
    lastEventTime = now;
    
    KeyEvent event;
    event.delay = (delay < 0) ? 0 : delay;
    event.state = isPress ? "down" : "up";
    
    // Store Windows-specific key code
    event.winKeyCode = static_cast<WORD>(vkCode);
    
    // Convert to string representation
    event.key = vkCodeToString(vkCode);
    
    if (event.key != "Unknown") {
        // Add to sequence with mutex protection
        {
            QMutexLocker locker(&sequenceMutex);
            sequence.push_back(event);
        }
        qDebug() << "Recorded (Win):" << QString::fromStdString(event.key)
                 << QString::fromStdString(event.state) << "delay:" << event.delay;

        // Update sequence text if panel is visible
        if (sequencePanelVisible) {
            // Use invokeMethod to safely call from another thread
            QMetaObject::invokeMethod(this, "updateSequenceText", Qt::QueuedConnection);
        }
    } else {
        qDebug() << "Ignored unknown Windows key code:" << vkCode;
    }
}
#elif defined(__APPLE__)
#include <Carbon/Carbon.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include "../include/macos_window_helper.h"  // For window level management

// Define the key map for non-printable keys at file scope
namespace { // Use anonymous namespace
// Store the frontmost application ProcessSerialNumber before playback
ProcessSerialNumber gLastActiveProcess = {0, 0};
ProcessSerialNumber gMyProcess = {0, 0};
bool gWaitingForApplicationSwitch = false;
QTimer* appSwitchCheckTimer = nullptr;

// Function to get the frontmost application
bool SaveFrontProcess(ProcessSerialNumber* psn) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    OSStatus err = ::GetFrontProcess(psn);
#pragma clang diagnostic pop
    return (err == noErr);
}

// Function to set the frontmost application
bool RestoreFrontProcess(const ProcessSerialNumber* psn) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    OSStatus err = ::SetFrontProcess(psn);
#pragma clang diagnostic pop
    return (err == noErr);
}

// Make the map const as it's initialized once and never modified
const std::map<CGKeyCode, std::string> nonPrintableKeyMap = {
    {kVK_Delete, "Backspace"}, {kVK_Tab, "Tab"}, {kVK_Return, "Enter"},
    {kVK_Shift, "Shift"}, {kVK_Control, "Ctrl"}, {kVK_Option, "Alt"}, // Option is Alt
    {kVK_Command, "Cmd"},
    {kVK_RightShift, "RShift"}, {kVK_RightControl, "RCtrl"}, {kVK_RightOption, "RAlt"},
    {kVK_RightCommand, "RCmd"},
    {kVK_CapsLock, "CapsLock"},
    {kVK_Escape, "Esc"}, {kVK_Space, "Space"},
    {kVK_PageUp, "PageUp"}, {kVK_PageDown, "PageDown"}, {kVK_End, "End"}, {kVK_Home, "Home"},
    {kVK_LeftArrow, "Left"}, {kVK_UpArrow, "Up"}, {kVK_RightArrow, "Right"}, {kVK_DownArrow, "Down"},
    {kVK_F1, "F1"}, {kVK_F2, "F2"}, {kVK_F3, "F3"}, {kVK_F4, "F4"}, {kVK_F5, "F5"}, {kVK_F6, "F6"},
    {kVK_F7, "F7"}, {kVK_F8, "F8"}, {kVK_F9, "F9"}, {kVK_F10, "F10"}, {kVK_F11, "F11"}, {kVK_F12, "F12"},
    {kVK_ForwardDelete, "Delete"}, {kVK_Help, "Insert"} // Help is often Insert
    // Add other non-printable keys as needed
};
} // end anonymous namespace

// Forward declaration of helper for permission guidance
static void showMacPermissionsDialog(ControllerApp* parent);
static CGEventRef permissionTestCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon);

// Define the key map at file scope - Marked as unused by linter, consider removing if truly unused.
// Made const for good practice.
const std::map<CGKeyCode, std::string> key_map = {
    {kVK_Delete, "Backspace"}, {kVK_Tab, "Tab"}, {kVK_Return, "Enter"},
    {kVK_Shift, "Shift"}, {kVK_Control, "Ctrl"}, {kVK_Option, "Alt"},
    {kVK_CapsLock, "CapsLock"},
    {kVK_Escape, "Esc"}, {kVK_Space, "Space"},
    {kVK_PageUp, "PageUp"}, {kVK_PageDown, "PageDown"}, {kVK_End, "End"}, {kVK_Home, "Home"},
    {kVK_LeftArrow, "Left"}, {kVK_UpArrow, "Up"}, {kVK_RightArrow, "Right"}, {kVK_DownArrow, "Down"}
};

// Removed static instance pointer comment

// C-style callback function for the event tap
// Mark proxy as potentially unused, make event const
CGEventRef keyboardEventCallback([[maybe_unused]] CGEventTapProxy proxy, CGEventType type, const CGEventRef event, void *refcon) {
    // Cast refcon back to our ControllerApp instance
    // Use auto for type deduction
    auto* appInstance = static_cast<ControllerApp*>(refcon);

    // Debug: Log that callback was triggered
    static int callbackCount = 0;
    if (callbackCount < 5) { // Only log first 5 to avoid spam
        qDebug() << "Callback triggered, count:" << ++callbackCount << "type:" << type << "isRecording:" << (appInstance ? appInstance->isRecording() : false);
    }

    if (appInstance == nullptr || !appInstance->isRecording()) { // Use a public getter for recording state
        return event; // Pass event through if not recording or instance is null
    }

    // Check if it's a key down or key up event
    if (type == kCGEventKeyDown || type == kCGEventKeyUp) {
        CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        bool isPress = (type == kCGEventKeyDown);

        qDebug() << "Key event in callback - keyCode:" << keyCode << "isPress:" << isPress;

        // Call the instance method via the pointer
        appInstance->recordKeyEvent(keyCode, isPress);
    }
    // We don't handle kCGEventFlagsChanged here, but could if needed (e.g., for modifier keys)

    // We are just observing, pass the event along
    return event;
}

static CGEventRef permissionTestCallback([[maybe_unused]] CGEventTapProxy proxy,
                                         [[maybe_unused]] CGEventType type,
                                         CGEventRef event,
                                         [[maybe_unused]] void* refcon) {
    return event;
}

static void showMacPermissionsDialog(ControllerApp* parent) {
    if (!parent) {
        return;
    }

    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("Permissions Required");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText("<p><b>Craftium needs permission to capture keyboard events.</b></p>");
    msgBox.setInformativeText(
        "<p>Grant both <b>Accessibility</b> and <b>Input Monitoring</b> access in "
        "System Settings so Craftium can record your key presses.</p>"
        "<p><b>Steps:</b></p>"
        "<ol style='margin-left: 20px;'>"
        "<li>Move Craftium.app to your <b>Applications</b> folder if it is running from Downloads.</li>"
        "<li>Open <b>System Settings → Privacy & Security</b>.</li>"
        "<li>Enable Craftium under <b>Accessibility</b>.</li>"
        "<li>Enable Craftium under <b>Input Monitoring</b>.</li>"
        "<li>Quit and relaunch Craftium.</li>"
        "</ol>"
        "<p>If Craftium does not appear in either list, click the “+” button and add it from "
        "the Applications folder.</p>");

    QPushButton* openInputMonitoringBtn = msgBox.addButton("Open Input Monitoring", QMessageBox::ActionRole);
    QPushButton* openAccessibilityBtn = msgBox.addButton("Open Accessibility", QMessageBox::ActionRole);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setDefaultButton(openInputMonitoringBtn);

    msgBox.exec();

    if (msgBox.clickedButton() == openAccessibilityBtn) {
        const void *keys[] = { kAXTrustedCheckOptionPrompt };
        const void *values[] = { kCFBooleanTrue };
        CFDictionaryRef options = CFDictionaryCreate(kCFAllocatorDefault, keys, values, std::size(keys),
                                                     &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        AXIsProcessTrustedWithOptions(options);
        CFRelease(options);
        QDesktopServices::openUrl(QUrl("x-apple.systempreferences:com.apple.preference.security?Privacy_Accessibility"));
    } else if (msgBox.clickedButton() == openInputMonitoringBtn) {
        QDesktopServices::openUrl(QUrl("x-apple.systempreferences:com.apple.preference.security?Privacy_ListenEvent"));
    }
}
#endif

ControllerApp::ControllerApp(QWidget *parent)
    : QWidget(parent), recording(false), playing(false), alwaysOnTop(true) {
    // Initialize settings
    settings = new QSettings("Craftium", "Craftium", this);
    isDarkMode = settings->value("darkMode", true).toBool();
    qDebug() << "Initial dark mode value:" << isDarkMode;
    
    setupUI();
    
    // Apply the initial theme
    applyTheme(isDarkMode);
    
    // Make sure menu matches the state
    if (menuBar) {
        QAction* darkModeAction = findActionInMenu(menuBar->actions(), "View", "Dark Mode");
        if (darkModeAction) {
            darkModeAction->setChecked(isDarkMode);
        }
    }

    // Apply Always on Top at startup
    setAlwaysOnTop(alwaysOnTop);

    // Setup worker thread
    playbackThread = new QThread(this);
    playbackWorker = new PlaybackWorker();
    playbackWorker->moveToThread(playbackThread);

    // Register KeyEvent vector for signal/slot use
    qRegisterMetaType<std::vector<KeyEvent>>("std::vector<KeyEvent>");

    // Connect signals/slots for thread management
    connect(playbackThread, &QThread::finished, playbackWorker, &QObject::deleteLater);
    connect(this, &ControllerApp::destroyed, playbackThread, &QThread::quit);

    // Connect signals/slots for playback control using modern syntax
    // Using playbackWorker as context ensures lambda executes on worker thread
    connect(this, &ControllerApp::startPlaybackSignal, playbackWorker,
            [this](const std::vector<KeyEvent>& sequence) {
                playbackWorker->doWork(sequence, repeatCountSpinner->value());
            }, Qt::QueuedConnection);
    connect(this, &ControllerApp::stopPlaybackSignal, playbackWorker, &PlaybackWorker::stopWork, Qt::DirectConnection);
    connect(playbackWorker, &PlaybackWorker::finished, this, &ControllerApp::handlePlaybackFinished);

    playbackThread->start();

#ifdef __APPLE__
    // Check Accessibility permissions at launch and show dialog if needed
    craftiumInstallFrontmostObserver();
    QTimer::singleShot(500, this, [this]() {
        checkAndRequestAccessibilityPermissions();
    });
#endif
}

ControllerApp::~ControllerApp() {
    qDebug() << "ControllerApp destroyed";

    // Stop recording if active
    if (recording) {
        stopRecording();
    }

    // Stop playback if active - call worker directly to avoid race condition
    if (playing && playbackWorker) {
        playbackWorker->stopWork();  // Stop the worker directly, not via signal
        playing = false;
    }

    // Properly shut down the worker thread
    if (playbackThread && playbackThread->isRunning()) {
        playbackThread->quit();
        playbackThread->wait();
    }

#ifdef __APPLE__
    // Clean up any active timers
    if (appSwitchCheckTimer) {
        appSwitchCheckTimer->stop();
        appSwitchCheckTimer->deleteLater();
        appSwitchCheckTimer = nullptr;
    }
#endif
}

bool ControllerApp::event(QEvent* event) {
#ifdef __APPLE__
    if (alwaysOnTop && event && event->type() == QEvent::WindowActivate) {
        QTimer::singleShot(0, this, [this]() {
            if (!QApplication::activeModalWidget() && !QApplication::activePopupWidget()) {
                craftiumDeactivateApp();
                craftiumReactivateLastForegroundApp();
            }
        });
    }

#endif
    return QWidget::event(event);
}

void ControllerApp::setupUI() {
    // Create main layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5); // Increased overall spacing
    layout->setSizeConstraint(QLayout::SetFixedSize); // Make the window size match contents exactly
    
    // Create and setup the menu bar
    setupMenuBar();
    
    // Set window title
    setWindowTitle("Craftium - Auto Crafter");
    
    // Create control buttons container widget
    QWidget* controlsWidget = new QWidget(this);
    controlsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // Fixed size in both directions
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(8); // Increased spacing between buttons
    controlsLayout->setSizeConstraint(QLayout::SetFixedSize); // Make the layout only as big as needed
    
    // Add "Always on Top" checkbox with a bit of extra margin at the bottom
    QHBoxLayout* checkboxLayout = new QHBoxLayout();
    checkboxLayout->setContentsMargins(0, 0, 0, 5); // Add space below the checkbox
    
    alwaysOnTopCheckbox = new QCheckBox("Always on Top", this);
    alwaysOnTopCheckbox->setChecked(alwaysOnTop);
    alwaysOnTopCheckbox->setToolTip("Keep window on top of others");
    checkboxLayout->addWidget(alwaysOnTopCheckbox);
    
    // Connect checkbox state changes to the slot
    connect(alwaysOnTopCheckbox, &QCheckBox::toggled, this, &ControllerApp::setAlwaysOnTop);
    
    controlsLayout->addLayout(checkboxLayout);
    
    // Recording and playback controls
    QPushButton* recordButton = new QPushButton("Start Recording", this);
    QPushButton* stopRecordButton = new QPushButton("Stop Recording", this);
    QPushButton* playExternalButton = new QPushButton("Play", this);
    QPushButton* stopPlayButton = new QPushButton("Stop", this);
    QPushButton* clearButton = new QPushButton("Clear Sequence", this);
    
    // Connect buttons to slots
    connect(recordButton, &QPushButton::clicked, this, &ControllerApp::startRecording);
    connect(stopRecordButton, &QPushButton::clicked, this, &ControllerApp::stopRecording);
    connect(playExternalButton, &QPushButton::clicked, this, [this]() {
        startPlayback(repeatCountSpinner->value(), true);
    });
    connect(stopPlayButton, &QPushButton::clicked, this, &ControllerApp::stopPlayback);
    connect(clearButton, &QPushButton::clicked, this, &ControllerApp::clearSequence);
    
    // Add buttons to layout
    controlsLayout->addWidget(recordButton);
    controlsLayout->addWidget(stopRecordButton);
    controlsLayout->addWidget(playExternalButton);
    controlsLayout->addWidget(stopPlayButton);
    controlsLayout->addWidget(clearButton);
    
    // Add repeat count spinner
    QHBoxLayout* repeatLayout = new QHBoxLayout();
    repeatLayout->setContentsMargins(0, 5, 0, 5); // Add spacing around spinner
    repeatLayout->setSpacing(5);
    QLabel* repeatLabel = new QLabel("Repeat Count:", this);
    repeatCountSpinner = new QSpinBox(this);
    repeatCountSpinner->setMinimum(1);
    repeatCountSpinner->setMaximum(100);
    repeatCountSpinner->setValue(1);
    repeatCountSpinner->setToolTip("Number of times to repeat the sequence");
    
    repeatLayout->addWidget(repeatLabel);
    repeatLayout->addWidget(repeatCountSpinner);
    repeatLayout->addStretch();
    
    controlsLayout->addLayout(repeatLayout);
    
    // Add expand/collapse button for sequence details
    expandButton = new QPushButton("▼ Show Sequence Details", this);
    connect(expandButton, &QPushButton::clicked, this, &ControllerApp::toggleSequencePanel);
    controlsLayout->addWidget(expandButton);
    
    // Add the controls widget to the main layout
    layout->addWidget(controlsWidget);
    
    // Add sequence text edit (initially hidden)
    sequenceTextEdit = new QTextEdit(this);
    sequenceTextEdit->setReadOnly(true);
    sequenceTextEdit->setVisible(false);
    sequenceTextEdit->setMinimumHeight(150);
    sequenceTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sequenceTextEdit->setLineWrapMode(QTextEdit::WidgetWidth); // Force text to wrap at widget width
    sequenceTextEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    sequenceTextEdit->setPlaceholderText("Recorded sequence will be shown here...");
    layout->addWidget(sequenceTextEdit);
    
    // Status label - moved to the bottom
    statusLabel = new QLabel("Status: Ready", this);
    statusLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    statusLabel->setContentsMargins(5, 2, 5, 2);
    statusLabel->setMinimumHeight(24);
    statusLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    statusLabel->setWordWrap(true);
    layout->addWidget(statusLabel);
    
    // Initialize animation for expanding/collapsing
    animation = new QPropertyAnimation(sequenceTextEdit, "visible", this);
    animation->setDuration(200);
    
    // Let the size adjust to match the content width - will make the window narrower
    setMinimumWidth(0); // Remove minimum width constraint
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred); // Don't allow horizontal expansion
    adjustSize(); // Adjust to fit contents exactly
}

void ControllerApp::updateStatusLabel(const QString& message) {
    if (!statusLabel) return;
    
    // For debug output, always log the full message
    qDebug() << message;
    
    // Simply display the full message - word wrap will handle long text
    statusLabel->setText(message);
}

void ControllerApp::clearSequence() {
    {
        QMutexLocker locker(&sequenceMutex);
        sequence.clear();
    }
    updateStatusLabel("Status: Sequence cleared");
    updateSequenceText();
}

void ControllerApp::saveSequence() {
    // Make a copy of the sequence while holding the mutex
    std::vector<KeyEvent> sequenceCopy;
    {
        QMutexLocker locker(&sequenceMutex);
        if (sequence.empty()) {
            QMessageBox::information(this, "Save Sequence", "No sequence to save.");
            return;
        }
        sequenceCopy = sequence;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Sequence", "", "JSON Files (*.json);;All Files (*)");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Save Sequence",
                            "Could not open file for writing: " + file.errorString());
        return;
    }

    // Create JSON array to hold sequence data
    QJsonArray sequenceArray;

    for (const auto& event : sequenceCopy) {
        QJsonObject eventObject;
        eventObject["key"] = QString::fromStdString(event.key);
        eventObject["state"] = QString::fromStdString(event.state);
        eventObject["delay"] = static_cast<int>(event.delay);

        // Add platform-specific key codes
#ifdef _WIN32
        eventObject["winKeyCode"] = static_cast<int>(event.winKeyCode);
#elif defined(__APPLE__)
        eventObject["macKeyCode"] = static_cast<int>(event.macKeyCode);
#endif

        sequenceArray.append(eventObject);
    }

    QJsonDocument doc(sequenceArray);
    file.write(doc.toJson());
    file.close();

    updateStatusLabel("Status: Sequence saved to " + fileName);
    updateSequenceText();
}

void ControllerApp::loadSequence() {
    if (recording) {
        QMessageBox::warning(this, "Load Sequence", "Cannot load sequence while recording.");
        return;
    }
    
    if (playing) {
        QMessageBox::warning(this, "Load Sequence", "Cannot load sequence during playback.");
        return;
    }
    
    QString fileName = QFileDialog::getOpenFileName(this,
        "Load Sequence", "", "JSON Files (*.json);;All Files (*)");
    
    if (fileName.isEmpty())
        return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Load Sequence",
                            "Could not open file for reading: " + file.errorString());
        return;
    }
    
    QByteArray saveData = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(saveData));
    
    if (doc.isNull() || !doc.isArray()) {
        QMessageBox::warning(this, "Load Sequence", "Invalid sequence file format.");
        return;
    }
    
    // Clear current sequence and load new one
    {
        QMutexLocker locker(&sequenceMutex);
        sequence.clear();
        QJsonArray sequenceArray = doc.array();

        for (const QJsonValue &value : sequenceArray) {
            if (!value.isObject())
                continue;

            QJsonObject obj = value.toObject();

            KeyEvent event;
            event.key = obj["key"].toString().toStdString();
            event.state = obj["state"].toString().toStdString();
            event.delay = obj["delay"].toInt();

            // Load platform-specific key codes
#ifdef _WIN32
            event.winKeyCode = static_cast<WORD>(obj["winKeyCode"].toInt());
#elif defined(__APPLE__)
            event.macKeyCode = static_cast<CGKeyCode>(obj["macKeyCode"].toInt());
#endif

            sequence.push_back(event);
        }
    }

    updateStatusLabel("Status: Sequence loaded from " + fileName);
    updateSequenceText();
}

void ControllerApp::startRecording() {
    if (!recording && !playing) {
        // Clear focus from any controls before starting to record
        clearFocusFromControls();

#ifdef __APPLE__
        if (!hasInputMonitoringPermission()) {
            updateStatusLabel("Status: Recording blocked - grant macOS permissions");
            showMacPermissionsDialog(this);
            return;
        }
#endif

        recording = true;
        {
            QMutexLocker locker(&sequenceMutex);
            sequence.clear();
        }
        updateStatusLabel("Status: Recording started");
        
        // Reset the last event time
        lastEventTime = std::chrono::high_resolution_clock::now();
        
        // Start the keyboard listener
#ifdef _WIN32
        startGlobalKeyListener();
#elif defined(__APPLE__)
        if (!startGlobalKeyListener()) {
            stopRecording();
            updateStatusLabel("Status: Failed to start recording");
            QMessageBox::critical(this, "Recording Error", 
                                 "Failed to start the global key listener on macOS.");
        }
#endif
    } else if (playing) {
        updateStatusLabel("Status: Cannot start recording during playback");
        QMessageBox::warning(this, "Recording Error", "Cannot start recording while playback is active.");
    }
}

void ControllerApp::stopRecording() {
    if (recording) {
        recording = false;
#ifdef __APPLE__
        stopGlobalKeyListener();
#elif defined(_WIN32)
        // Removed commented-out stopGlobalKeyListener call
#endif
        updateStatusLabel("Status: Recording stopped");
        // Update UI: re-enable play button, change status label?
    }
}

void ControllerApp::startPlayback(int repeatCount, bool external) {
    // Make a copy of the sequence while holding the mutex
    std::vector<KeyEvent> sequenceCopy;
    {
        QMutexLocker locker(&sequenceMutex);
        if (sequence.empty()) {
            updateStatusLabel("Status: No sequence to play");
            QMessageBox::information(this, "Playback Info", "No sequence recorded to play back.");
            return;
        }
        sequenceCopy = sequence;
    }

    if (!playing && !recording) {
        playing = true;

        if (external) {
            updateStatusLabel(QString("Status: Click in the target application..."));

#ifdef __APPLE__
            // Get our own process information for first-time initialization
            if (gMyProcess.highLongOfPSN == 0 && gMyProcess.lowLongOfPSN == 0) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
                GetCurrentProcess(&gMyProcess);
#pragma clang diagnostic pop
            }

            // Set up a flag to track if we're waiting for app switch
            gWaitingForApplicationSwitch = true;

            // Get the current front process
            ProcessSerialNumber currentFrontProcess;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            if (GetFrontProcess(&currentFrontProcess) == noErr) {
#pragma clang diagnostic pop
                // Initialize a timer to check for application switch
                if (appSwitchCheckTimer) {
                    appSwitchCheckTimer->stop();
                    appSwitchCheckTimer->deleteLater();
                }

                appSwitchCheckTimer = new QTimer(this);
                appSwitchCheckTimer->setInterval(100);  // Check every 100ms

                connect(appSwitchCheckTimer, &QTimer::timeout, this, [this, repeatCount, sequenceCopy]() {
                    if (!gWaitingForApplicationSwitch) {
                        appSwitchCheckTimer->stop();
                        return;
                    }

                    // Get the current frontmost process
                    ProcessSerialNumber frontProcess;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
                    if (GetFrontProcess(&frontProcess) == noErr) {
                        // Check if the frontmost process has changed from our app
                        Boolean isSameProcess;
                        if (SameProcess(&frontProcess, &gMyProcess, &isSameProcess) == noErr && !isSameProcess) {
#pragma clang diagnostic pop
                            // Different app is now in front - start playback
                            gWaitingForApplicationSwitch = false;
                            appSwitchCheckTimer->stop();

                            updateStatusLabel(QString("Status: Playback starting (%1 repeats)").arg(repeatCount));
                            emit startPlaybackSignal(sequenceCopy);
                        }
                    }
                });

                // Start the timer
                appSwitchCheckTimer->start();

                // Set a timeout in case no app switch happens (30 seconds)
                QTimer::singleShot(30000, this, [this]() {
                    if (gWaitingForApplicationSwitch) {
                        gWaitingForApplicationSwitch = false;

                        if (appSwitchCheckTimer) {
                            appSwitchCheckTimer->stop();
                            appSwitchCheckTimer->deleteLater();
                            appSwitchCheckTimer = nullptr;
                        }

                        // Cancel playback since no app switch happened
                        playing = false;
                        updateStatusLabel("Status: Playback cancelled - no app switch detected");
                    }
                });
            } else {
                // Couldn't get front process, fall back to timer approach
                QTimer::singleShot(2000, this, [this, repeatCount, sequenceCopy]() {
                    updateStatusLabel(QString("Status: Playback starting (%1 repeats)").arg(repeatCount));
                    emit startPlaybackSignal(sequenceCopy);
                });
            }
#else
            // For non-Mac platforms, use the timer approach as before
            QTimer::singleShot(2000, this, [this, repeatCount, sequenceCopy]() {
                updateStatusLabel(QString("Status: Playback starting (%1 repeats)").arg(repeatCount));
                emit startPlaybackSignal(sequenceCopy);
            });
#endif
        } else {
            // Normal playback without special focus handling
            updateStatusLabel(QString("Status: Playback starting (%1 repeats)").arg(repeatCount));
            emit startPlaybackSignal(sequenceCopy);
        }
    } else if (recording) {
        updateStatusLabel("Status: Cannot start playback during recording");
        QMessageBox::warning(this, "Playback Error", "Cannot start playback while recording is active.");
    } else if (playing) {
        updateStatusLabel("Status: Playback already running");
    }
}

void ControllerApp::stopPlayback() {
    if (playing) {
        emit stopPlaybackSignal();
        playing = false;
        updateStatusLabel("Status: Playback stopped");
        
#ifdef __APPLE__
        // Cancel app switching if we're waiting
        if (gWaitingForApplicationSwitch) {
            gWaitingForApplicationSwitch = false;
            
            // Stop and clean up the timer
            if (appSwitchCheckTimer) {
                appSwitchCheckTimer->stop();
                appSwitchCheckTimer->deleteLater();
                appSwitchCheckTimer = nullptr;
            }
        }
#endif
    }
}

void ControllerApp::handlePlaybackFinished() {
    playing = false;
    updateStatusLabel("Status: Playback completed");
}

void ControllerApp::showAboutDialog() {
    QMessageBox about(this);
    about.setWindowTitle("About Craftium");
    about.setTextFormat(Qt::RichText);
    
    // Center-aligned rich text content
    QString aboutText = "<div align='center'>"
                        "<h2>Craftium - Auto Crafter</h2>"
                        "<p>Version: 1</p>"
                        "<p>Designed by Jeremy Matthews</p>"
                        "<p>© SpiritWise Studios LLC</p>"
                        "</div>";
    
    about.setText(aboutText);
    about.setStandardButtons(QMessageBox::Ok);
    about.exec();
}

void ControllerApp::openDonationPage() {
    // Create a message box to allow users to choose a donation platform
    QMessageBox donateDialog(this);
    donateDialog.setWindowTitle("Support Craftium Development");
    donateDialog.setTextFormat(Qt::RichText);
    
    QString donateText = "<div align='center'>"
                        "<h3>Support Craftium Development</h3>"
                        "<p>Thank you for considering a donation! "
                        "Your support helps us continue improving Craftium.</p>"
                        "<p>Please choose your preferred donation method:</p>"
                        "</div>";
    
    donateDialog.setText(donateText);
    
    // Add buttons for different donation platforms
    QPushButton* paypalButton = donateDialog.addButton("PayPal", QMessageBox::ActionRole);
    QPushButton* kofiButton = donateDialog.addButton("Ko-fi", QMessageBox::ActionRole);
    QPushButton* patreonButton = donateDialog.addButton("Patreon", QMessageBox::ActionRole);
    donateDialog.addButton(QMessageBox::Cancel);
    
    // Show the dialog and handle the result
    donateDialog.exec();
    
    // Determine which button was clicked and open the appropriate URL
    if (donateDialog.clickedButton() == paypalButton) {
        QDesktopServices::openUrl(QUrl("https://www.paypal.me/spiritwise"));
    } else if (donateDialog.clickedButton() == kofiButton) {
        QDesktopServices::openUrl(QUrl("https://ko-fi.com/spiritwise"));
    } else if (donateDialog.clickedButton() == patreonButton) {
        QDesktopServices::openUrl(QUrl("https://patreon.com/SpiritWiseStudios"));
    }
}

void ControllerApp::showHelpDialog() {
    // Create a QDialog for the help content
    QDialog helpDialog(this);
    helpDialog.setWindowTitle("Craftium User Guide");
    helpDialog.setMinimumWidth(600);
    helpDialog.setMinimumHeight(500);
    
    // Create a QTextBrowser for rich text content with hyperlinks
    QTextBrowser* textBrowser = new QTextBrowser(&helpDialog);
    textBrowser->setOpenExternalLinks(true);
    
    // Create layout for dialog
    QVBoxLayout* layout = new QVBoxLayout(&helpDialog);
    layout->addWidget(textBrowser);
    
    // Add a close button at the bottom
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* closeButton = new QPushButton("Close", &helpDialog);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);
    
    // Connect close button
    connect(closeButton, &QPushButton::clicked, &helpDialog, &QDialog::accept);
    
    // Set the HTML content
    QString helpText = R"(
    <html>
    <head>
        <style>
            body { font-family: Arial, sans-serif; margin: 15px; }
            h1 { color: #2c3e50; border-bottom: 1px solid #ddd; padding-bottom: 5px; }
            h2 { color: #3498db; margin-top: 20px; }
            h3 { color: #2980b9; }
            .section { margin-left: 15px; margin-bottom: 15px; }
            .keyboard { background-color: #f5f5f5; padding: 2px 5px; border: 1px solid #ddd; border-radius: 3px; font-family: monospace; }
            table { border-collapse: collapse; width: 100%; margin: 10px 0; }
            th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
            th { background-color: #f2f2f2; }
            .tip { background-color: #3d3d3d; border-left: 4px solid #ffcc00; padding: 10px; margin: 10px 0; color: #e0e0e0; }
        </style>
    </head>
    <body>
        <h1>Craftium - Auto Crafter User Guide</h1>
        
        <h2>Introduction</h2>
        <div class="section">
            <p>Craftium is a powerful automation tool designed to record and replay keyboard sequences. It's ideal for automating repetitive tasks in games, applications, or any software that accepts keyboard input.</p>
        </div>
        
        <h2>Quick Start</h2>
        <div class="section">
            <ol>
                <li><b>Record</b>: Click "Start Recording" and press the keys you want to record</li>
                <li><b>Stop</b>: Click "Stop Recording" when finished</li>
                <li><b>Play</b>: Click "Play" to replay your sequence in another application</li>
            </ol>
        </div>
        
        <h2>Detailed Instructions</h2>
        
        <h3>Recording Keyboard Sequences</h3>
        <div class="section">
            <ol>
                <li>Click the "Start Recording" button</li>
                <li>The status will change to "Recording started"</li>
                <li>Press the keys you want to record (Craftium will capture all keystrokes)</li>
                <li>Click "Stop Recording" when you're finished</li>
                <li>The sequence will be stored in memory and can be viewed by clicking "Show Sequence Details"</li>
            </ol>
            
            <p class="tip"><b>Tip:</b> For best results, make sure to include all keystrokes you need, including modifier keys (Shift, Ctrl, Alt) both press and release actions.</p>
        </div>
        
        <h3>Playing Back Sequences</h3>
        <div class="section">
            <ol>
                <li>Set the "Repeat Count" to specify how many times to repeat the sequence</li>
                <li>Click the "Play" button</li>
                <li>You'll have a brief delay (2 seconds) to switch to your target application</li>
                <li>The sequence will play automatically after the delay</li>
            </ol>
            
            <p class="tip"><b>Tip:</b> The "Always on Top" option keeps Craftium visible while you work with other applications.</p>
        </div>
        
        <h3>Saving and Loading Sequences</h3>
        <div class="section">
            <p>To save your recorded sequence for later use:</p>
            <ol>
                <li>Go to "File" → "Save Recording"</li>
                <li>Choose a location and filename (.json format)</li>
                <li>Click "Save"</li>
            </ol>
            
            <p>To load a previously saved sequence:</p>
            <ol>
                <li>Go to "File" → "Load Recording"</li>
                <li>Browse to your saved .json file</li>
                <li>Click "Open"</li>
            </ol>
        </div>
        
        <h3>Viewing and Understanding Sequences</h3>
        <div class="section">
            <p>Click "Show Sequence Details" to view the recorded sequence. The sequence panel shows:</p>
            <ul>
                <li>Each keystroke (key name and whether it was pressed or released)</li>
                <li>The timing between keystrokes (in milliseconds)</li>
                <li>A summary showing total events and duration</li>
            </ul>
        </div>
        
        <h2>Additional Features</h2>
        
        <h3>Interface Options</h3>
        <div class="section">
            <ul>
                <li><b>Dark Mode</b>: Toggle between light and dark themes (View → Dark Mode)</li>
                <li><b>Always on Top</b>: Keep Craftium window visible above other applications</li>
                <li><b>Show Sequence Panel</b>: View detailed information about your recorded sequence</li>
            </ul>
        </div>
        
        <h3>Troubleshooting</h3>
        <div class="section">
            <p><b>Recording doesn't work:</b></p>
            <ul>
                <li>On macOS, make sure Craftium has Accessibility permissions (System Settings → Privacy & Security → Accessibility)</li>
                <li>Try restarting the application</li>
            </ul>
            
            <p><b>Playback doesn't work properly:</b></p>
            <ul>
                <li>Ensure the target application is in focus before playback starts</li>
                <li>For games, try running in windowed mode rather than fullscreen</li>
                <li>Some applications may block simulated key inputs</li>
            </ul>
        </div>
        
        <h2>Keyboard Shortcut Reference</h2>
        <div class="section">
            <p>Craftium recognizes various keys including:</p>
            <ul>
                <li>Alphanumeric keys (A-Z, 0-9)</li>
                <li>Function keys (F1-F12)</li>
                <li>Modifier keys (Shift, Ctrl, Alt, Command)</li>
                <li>Navigation keys (Arrow keys, Home, End, Page Up/Down)</li>
                <li>Special keys (Enter, Tab, Escape, etc.)</li>
            </ul>
        </div>
    </body>
    </html>
    )";
    
    textBrowser->setHtml(helpText);
    
    // Show the dialog modally
    helpDialog.exec();
}

#ifdef _WIN32
// Mark as const as it doesn't modify member variables
std::string ControllerApp::vkCodeToString(DWORD vkCode) const {
    // Handle alphanumeric keys directly
    if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9')) {
        return std::string(1, static_cast<char>(vkCode));
    }

    // Use init-statement for iterator
    if (const auto it = vk_to_string_map.find(vkCode); it != vk_to_string_map.end()) {
        return it->second;
    }

    // Fallback for unmapped keys - get scan code and use GetKeyNameText
    UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
    if (scanCode) {
        // For extended keys
        if (vkCode == VK_RCONTROL || vkCode == VK_RMENU || 
            vkCode == VK_LEFT || vkCode == VK_RIGHT || vkCode == VK_UP || vkCode == VK_DOWN ||
            vkCode == VK_PRIOR || vkCode == VK_NEXT || vkCode == VK_HOME || vkCode == VK_END ||
            vkCode == VK_INSERT || vkCode == VK_DELETE || vkCode == VK_DIVIDE) {
            scanCode |= 0x100; // Set extended bit
        }
        
        scanCode = scanCode << 16; // Shift to high word
        scanCode |= 0x1; // Add pressed flag
        
        wchar_t keyName[50];
        if (GetKeyNameTextW(scanCode, keyName, sizeof(keyName) / sizeof(keyName[0]))) {
            return QString::fromWCharArray(keyName).toStdString();
        }
    }
    
    qDebug() << "vkCodeToString: Unmapped VK Code:" << vkCode;
    return "Unknown";
}

// Mark as const as it doesn't modify member variables
WORD ControllerApp::stringToVkCode(const std::string& keyName) const {
    initialize_string_to_vk_map(); // Ensure map is initialized

    // Use init-statement for iterator
    if (const auto it = string_to_vk_map.find(keyName); it != string_to_vk_map.end()) {
        return it->second;
    }

    qDebug() << "stringToVkCode: Unmapped key name:" << QString::fromStdString(keyName);
    return 0; // Return 0 or some indicator of failure
}
#elif defined(__APPLE__)
// Mark as const as it doesn't modify member variables
std::string ControllerApp::keyCodeToString(CGKeyCode keyCode) const {
    // First, check our map for non-printable keys
    // Use init-statement for iterator
    if (const auto it = nonPrintableKeyMap.find(keyCode); it != nonPrintableKeyMap.end()) {
        return it->second;
    }

    // Expanded map for printable keys based on US ANSI keyboard layout
    static const std::map<CGKeyCode, std::string> printableKeyMap = {
        {kVK_ANSI_A, "a"}, {kVK_ANSI_B, "b"}, {kVK_ANSI_C, "c"}, {kVK_ANSI_D, "d"},
        {kVK_ANSI_E, "e"}, {kVK_ANSI_F, "f"}, {kVK_ANSI_G, "g"}, {kVK_ANSI_H, "h"},
        {kVK_ANSI_I, "i"}, {kVK_ANSI_J, "j"}, {kVK_ANSI_K, "k"}, {kVK_ANSI_L, "l"},
        {kVK_ANSI_M, "m"}, {kVK_ANSI_N, "n"}, {kVK_ANSI_O, "o"}, {kVK_ANSI_P, "p"},
        {kVK_ANSI_Q, "q"}, {kVK_ANSI_R, "r"}, {kVK_ANSI_S, "s"}, {kVK_ANSI_T, "t"},
        {kVK_ANSI_U, "u"}, {kVK_ANSI_V, "v"}, {kVK_ANSI_W, "w"}, {kVK_ANSI_X, "x"},
        {kVK_ANSI_Y, "y"}, {kVK_ANSI_Z, "z"},
        
        {kVK_ANSI_0, "0"}, {kVK_ANSI_1, "1"}, {kVK_ANSI_2, "2"}, {kVK_ANSI_3, "3"},
        {kVK_ANSI_4, "4"}, {kVK_ANSI_5, "5"}, {kVK_ANSI_6, "6"}, {kVK_ANSI_7, "7"},
        {kVK_ANSI_8, "8"}, {kVK_ANSI_9, "9"},
        
        {kVK_ANSI_Equal, "="}, {kVK_ANSI_Minus, "-"}, 
        {kVK_ANSI_LeftBracket, "["}, {kVK_ANSI_RightBracket, "]"},
        {kVK_ANSI_Backslash, "\\"}, {kVK_ANSI_Semicolon, ";"},
        {kVK_ANSI_Quote, "'"}, {kVK_ANSI_Comma, ","}, 
        {kVK_ANSI_Period, "."}, {kVK_ANSI_Slash, "/"}, 
        {kVK_ANSI_Grave, "`"},
        
        {kVK_Space, " "},
        {kVK_Return, "\n"}
    };
    
    // Check the printable key map
    if (const auto it = printableKeyMap.find(keyCode); it != printableKeyMap.end()) {
        return it->second;
    }
    
    // If we get here, it's an unknown key
    qWarning() << "keyCodeToString: Unknown key code:" << keyCode;
    return "Unknown";
}

// Mark as const as it doesn't modify member variables
CGKeyCode ControllerApp::stringToKeyCode(const std::string& keyName) const {
    // First check non-printable keys
    for (const auto& [code, name] : nonPrintableKeyMap) {
        if (name == keyName) {
            return code;
        }
    }

    // Build reverse map for printable keys (on first call)
    static std::map<std::string, CGKeyCode> reverseMap;
    
    if (reverseMap.empty()) {
        // Build the reverse map from the printable key map
        static const std::map<CGKeyCode, std::string> printableKeyMap = {
            {kVK_ANSI_A, "a"}, {kVK_ANSI_B, "b"}, {kVK_ANSI_C, "c"}, {kVK_ANSI_D, "d"},
            {kVK_ANSI_E, "e"}, {kVK_ANSI_F, "f"}, {kVK_ANSI_G, "g"}, {kVK_ANSI_H, "h"},
            {kVK_ANSI_I, "i"}, {kVK_ANSI_J, "j"}, {kVK_ANSI_K, "k"}, {kVK_ANSI_L, "l"},
            {kVK_ANSI_M, "m"}, {kVK_ANSI_N, "n"}, {kVK_ANSI_O, "o"}, {kVK_ANSI_P, "p"},
            {kVK_ANSI_Q, "q"}, {kVK_ANSI_R, "r"}, {kVK_ANSI_S, "s"}, {kVK_ANSI_T, "t"},
            {kVK_ANSI_U, "u"}, {kVK_ANSI_V, "v"}, {kVK_ANSI_W, "w"}, {kVK_ANSI_X, "x"},
            {kVK_ANSI_Y, "y"}, {kVK_ANSI_Z, "z"},
            
            {kVK_ANSI_0, "0"}, {kVK_ANSI_1, "1"}, {kVK_ANSI_2, "2"}, {kVK_ANSI_3, "3"},
            {kVK_ANSI_4, "4"}, {kVK_ANSI_5, "5"}, {kVK_ANSI_6, "6"}, {kVK_ANSI_7, "7"},
            {kVK_ANSI_8, "8"}, {kVK_ANSI_9, "9"},
            
            {kVK_ANSI_Equal, "="}, {kVK_ANSI_Minus, "-"}, 
            {kVK_ANSI_LeftBracket, "["}, {kVK_ANSI_RightBracket, "]"},
            {kVK_ANSI_Backslash, "\\"}, {kVK_ANSI_Semicolon, ";"},
            {kVK_ANSI_Quote, "'"}, {kVK_ANSI_Comma, ","}, 
            {kVK_ANSI_Period, "."}, {kVK_ANSI_Slash, "/"}, 
            {kVK_ANSI_Grave, "`"},
            
            {kVK_Space, " "},
            {kVK_Return, "\n"}
        };
        
        // Create reverse mapping
        for (const auto& [code, name] : printableKeyMap) {
            reverseMap[name] = code;
        }
        
        // Add special cases for uppercase/lowercase
        for (char c = 'A'; c <= 'Z'; c++) {
            // Map uppercase to same key as lowercase
            std::string upper(1, c);
            std::string lower(1, c + 32); // Convert to lowercase
            
            // Use the appropriate ANSI key code
            CGKeyCode keyCode = kVK_ANSI_A + (c - 'A');
            reverseMap[upper] = keyCode;
            
            // Only add lowercase if it doesn't exist yet
            if (reverseMap.find(lower) == reverseMap.end()) {
                reverseMap[lower] = keyCode;
            }
        }
        
        // Add aliases for Enter/Return
        reverseMap["Enter"] = kVK_Return;
        reverseMap["\r"] = kVK_Return;
    }
    
    // Check if the key is in our reverse map
    if (const auto it = reverseMap.find(keyName); it != reverseMap.end()) {
        return it->second;
    }
    
    // If we get here, it's an unknown key
    qWarning() << "stringToKeyCode: Could not find key code for:" << QString::fromStdString(keyName);
    return UINT16_MAX; // Indicate failure
}

bool ControllerApp::startGlobalKeyListener() {
    if (eventTap) {
        qDebug() << "Event tap already running.";
        return true; // Already started
    }

    qDebug() << "Attempting to start global key listener...";

    // Try to create an event tap first - this is the real test of permissions
    // NOTE: For unsigned apps, AXIsProcessTrusted() can return false even when
    // permissions work, so we test by actually creating the tap
    CGEventMask eventMask = (1 << kCGEventKeyDown) | (1 << kCGEventKeyUp) | (1 << kCGEventFlagsChanged);

    eventTap = CGEventTapCreate(kCGHIDEventTap, // Tap HID events
                              kCGHeadInsertEventTap, // Insert before existing taps
                              kCGEventTapOptionListenOnly, // Just listen, don't modify
                              eventMask, // Mask for keydown/up/flags
                              keyboardEventCallback, // The C callback function
                              this); // Pass 'this' as user data (refcon)

    if (!eventTap) {
        // Event tap creation failed - this means we definitely don't have permissions
        qCritical() << "Failed to create event tap - permissions likely not granted";
        updateStatusLabel("Status: Permission required to record");
        showMacPermissionsDialog(this);

        return false;
    }

    // Event tap created successfully!
    qDebug() << "Event tap created successfully";

    // Create a run loop source
    runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    if (!runLoopSource) {
         qCritical() << "Failed to create run loop source";
         CFRelease(eventTap);
         eventTap = nullptr;
         QMessageBox::critical(this, "Error",
                               "Failed to create run loop source for keyboard listener. Please try restarting the application.");
         return false;
    }

    // Add the source to the current run loop
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

    // Enable the event tap
    CGEventTapEnable(eventTap, true);

    qDebug() << "Global key listener started successfully (macOS Event Tap)";
    return true;
}

void ControllerApp::stopGlobalKeyListener() {
    if (eventTap) {
        CGEventTapEnable(eventTap, false);
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
        CFRelease(runLoopSource);
        runLoopSource = nullptr;
        CFRelease(eventTap);
        eventTap = nullptr;
        qDebug() << "Global key listener stopped (macOS Event Tap)";
    }
}

bool ControllerApp::hasInputMonitoringPermission() const {
    CGEventMask eventMask = (1 << kCGEventKeyDown) | (1 << kCGEventKeyUp);
    CFMachPortRef tempTap = CGEventTapCreate(kCGHIDEventTap,
                                             kCGHeadInsertEventTap,
                                             kCGEventTapOptionListenOnly,
                                             eventMask,
                                             permissionTestCallback,
                                             nullptr);
    if (!tempTap) {
        qWarning() << "Permission check: event tap creation failed";
        return false;
    }

    CFRelease(tempTap);
    return true;
}

void ControllerApp::checkAndRequestAccessibilityPermissions() {
    qDebug() << "Checking Accessibility permissions...";

    // NOTE: For unsigned apps, AXIsProcessTrusted() can return false even when permissions
    // are granted, because macOS ties permissions to the exact binary path.
    // We'll show a one-time informational message, but won't block the app.

    // Check if we've already shown the welcome message
    bool shownWelcome = settings->value("shownAccessibilityWelcome", false).toBool();

    if (!shownWelcome) {
        qDebug() << "First launch - showing Accessibility permissions info";

        // Show informational dialog explaining why permissions might be needed
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Welcome to Craftium!");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(
            "<h3>Welcome to Craftium!</h3>"
            "<p>To record keyboard sequences, Craftium needs <b>Accessibility</b> and "
            "<b>Input Monitoring</b> permissions.</p>"
        );
        msgBox.setInformativeText(
            "<p><b>If you haven't granted permissions yet:</b></p>"
            "<ol style='margin-left: 20px;'>"
            "<li>Click one of the buttons below to jump to the relevant settings pane.</li>"
            "<li>Enable Craftium under <b>Accessibility</b> and <b>Input Monitoring</b>.</li>"
            "<li>Restart Craftium, then click 'Start Recording' to test.</li>"
            "</ol>"
            "<p><i>Note: This message will only appear once.</i></p>"
        );

        QPushButton* openAccessibilityBtn = msgBox.addButton("Open Accessibility", QMessageBox::ActionRole);
        QPushButton* openInputMonitoringBtn = msgBox.addButton("Open Input Monitoring", QMessageBox::ActionRole);
        QPushButton* okBtn = msgBox.addButton("OK, Got It", QMessageBox::AcceptRole);
        msgBox.setDefaultButton(okBtn);

        msgBox.exec();

        if (msgBox.clickedButton() == openAccessibilityBtn) {
            // Open System Settings to Privacy & Security -> Accessibility
            const void *keys[] = { kAXTrustedCheckOptionPrompt };
            const void *values[] = { kCFBooleanTrue };
            CFDictionaryRef options = CFDictionaryCreate(kCFAllocatorDefault, keys, values, std::size(keys),
                                                         &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
            AXIsProcessTrustedWithOptions(options);
            CFRelease(options);
            QDesktopServices::openUrl(QUrl("x-apple.systempreferences:com.apple.preference.security?Privacy_Accessibility"));
        } else if (msgBox.clickedButton() == openInputMonitoringBtn) {
            QDesktopServices::openUrl(QUrl("x-apple.systempreferences:com.apple.preference.security?Privacy_ListenEvent"));
        }

        // Mark that we've shown the welcome message
        settings->setValue("shownAccessibilityWelcome", true);
    }

    qDebug() << "Accessibility permission check complete";
}
#endif

// Removed comments about moving recordKeyEvent to public

// Add a public getter for the recording state
// Already const, no change needed
bool ControllerApp::isRecording() const {
    return recording;
}

// Existing recordKeyEvent implementation remains (now public)
// Removed comment about guarding with #ifdef
#ifdef __APPLE__
void ControllerApp::recordKeyEvent(CGKeyCode keyCode, bool isPress) {
    // This method might be called from a non-GUI thread (event tap callback)
    // Avoid complex UI updates here. If needed, use signals or invokeMethod.

    // We check recording status in the callback now, but an extra check here is harmless
    if (!recording) return;

    auto now = std::chrono::high_resolution_clock::now();
    auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEventTime).count();
    lastEventTime = now;

    KeyEvent event;
    event.delay = (delay < 0) ? 0 : delay;
    event.state = isPress ? "down" : "up";

    event.macKeyCode = keyCode; // Use the passed keycode directly
    // Call the const version of keyCodeToString
    event.key = this->keyCodeToString(event.macKeyCode);

    if (event.key != "Unknown") {
        // Add to sequence with mutex protection
        {
            QMutexLocker locker(&sequenceMutex);
            sequence.push_back(event);
        }
        qDebug() << "Recorded:" << QString::fromStdString(event.key) << QString::fromStdString(event.state) << "delay:" << event.delay;

        // Update sequence text if panel is visible
        if (sequencePanelVisible) {
            updateSequenceText();
        }
    } else {
        qDebug() << "Ignored unknown key code:" << keyCode;
    }
}
#endif 

// Add toggleSequencePanel method to show/hide sequence details
void ControllerApp::toggleSequencePanel() {
    if (sequencePanelVisible) {
        // Hide panel
        sequenceTextEdit->setVisible(false);
        expandButton->setText("▼ Show Sequence Details");
        
        // After hiding, resize the window to fit the now smaller content
        adjustSize();
    } else {
        // Get current width before showing the panel
        int currentWidth = width();
        
        // Show panel
        updateSequenceText();
        sequenceTextEdit->setVisible(true);
        expandButton->setText("▲ Hide Sequence Details");
        
        // After showing, resize the window vertically but maintain the same width
        adjustSize();
        resize(currentWidth, height());
    }
    
    sequencePanelVisible = !sequencePanelVisible;
}

// Add updateSequenceText method to display sequence details
void ControllerApp::updateSequenceText() {
    if (!sequenceTextEdit) return;

    // Make a copy of the sequence while holding the mutex
    std::vector<KeyEvent> sequenceCopy;
    {
        QMutexLocker locker(&sequenceMutex);
        if (sequence.empty()) {
            sequenceTextEdit->setText("No sequence recorded.");
            return;
        }
        sequenceCopy = sequence;
    }

    QString text;
    long long totalTime = 0;

    for (size_t i = 0; i < sequenceCopy.size(); ++i) {
        const KeyEvent& event = sequenceCopy[i];
        totalTime += event.delay;

        QString line;

        if (i == 0) {
            // First event
            line = QString("Wait %1ms\n").arg(event.delay);
        } else {
            line = QString("Key %1 %2 (wait %3ms)\n")
                      .arg(QString::fromStdString(event.key))
                      .arg(QString::fromStdString(event.state))
                      .arg(event.delay);
        }

        text.append(line);
    }

    // Add summary information
    text.append(QString("\n--- Summary ---\n"));
    text.append(QString("Total events: %1\n").arg(sequenceCopy.size()));
    text.append(QString("Total time: %1ms (%2s)\n")
                   .arg(totalTime)
                   .arg(totalTime / 1000.0, 0, 'f', 2));

    sequenceTextEdit->setText(text);
}

void ControllerApp::clearFocusFromControls() {
    // Clear focus from any widget that might have it
    if (QWidget* focusWidget = QApplication::focusWidget()) {
        focusWidget->clearFocus();
    }
    
    // Set focus to the main window itself, which effectively means no widget has focus
    this->setFocus();
    
    updateStatusLabel("Status: Focus cleared");
}

void ControllerApp::setAlwaysOnTop(bool enable) {
    alwaysOnTop = enable;

    setWindowFlag(Qt::WindowStaysOnTopHint, enable);
    setWindowFlag(Qt::Tool, enable);
    setWindowFlag(Qt::WindowDoesNotAcceptFocus, enable);
    setAttribute(Qt::WA_ShowWithoutActivating, enable);
#ifdef __APPLE__
    setAttribute(Qt::WA_MacAlwaysShowToolWindow, enable);
#endif

    show();

    if (alwaysOnTopCheckbox && alwaysOnTopCheckbox->isChecked() != enable) {
        QSignalBlocker blocker(alwaysOnTopCheckbox);
        alwaysOnTopCheckbox->setChecked(enable);
    }

#ifdef __APPLE__
    if (menuBar) {
        if (QAction* alwaysOnTopAction = findActionInMenu(menuBar->actions(), "View", "Always on Top")) {
            if (alwaysOnTopAction->isChecked() != enable) {
                QSignalBlocker blocker(alwaysOnTopAction);
                alwaysOnTopAction->setChecked(enable);
            }
        }
    }
#endif

    updateStatusLabel(enable ? "Status: Window will stay on top" : "Status: Window will behave normally");

#ifdef __APPLE__
    WId windowId = winId();
    if (windowId) {
        setMacOSWindowLevel(reinterpret_cast<void*>(windowId), enable);
        if (enable) {
            qDebug() << "Set window to NSPopUpMenuWindowLevel (high priority)";
        } else {
            qDebug() << "Set window to NSNormalWindowLevel";
        }
    }
#endif
}

void ControllerApp::toggleDarkMode(bool checked) {
    // If sender is an action, get the checked state from the action
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        isDarkMode = action->isChecked();
    } else {
        // If called directly with a bool argument
        isDarkMode = checked;
    }
    
    // Apply the appropriate theme
    applyTheme(isDarkMode);
    
    // Save the setting
    settings->setValue("darkMode", isDarkMode);
    
    // Update menu if this was triggered by the checkbox
    if (!qobject_cast<QAction*>(sender()) && menuBar) {
        QAction* darkModeAction = findActionInMenu(menuBar->actions(), "View", "Dark Mode");
        if (darkModeAction) {
            darkModeAction->setChecked(isDarkMode);
        }
    }
}

// Helper function to find a specific action in a menu
QAction* ControllerApp::findActionInMenu(const QList<QAction*>& menuActions, const QString& menuName, const QString& actionName) {
    for (QAction* action : menuActions) {
        if (action->menu() && action->menu()->title() == "&" + menuName) {
            for (QAction* subAction : action->menu()->actions()) {
                if (subAction->text() == "&" + actionName) {
                    return subAction;
                }
            }
            break;
        }
    }
    return nullptr;
}

void ControllerApp::applyTheme(bool darkMode) {
    qDebug() << "Applying" << (darkMode ? "dark" : "light") << "theme";
    
    QString styleSheet;
    
    if (darkMode) {
        // Dark theme stylesheet
        styleSheet = R"(
            QWidget {
                background-color: #2d2d2d;
                color: #e0e0e0;
            }
            QPushButton {
                background-color: #3d3d3d;
                border: 1px solid #5d5d5d;
                padding: 5px;
            }
            QPushButton:hover {
                background-color: #4d4d4d;
            }
            QPushButton:pressed {
                background-color: #5d5d5d;
            }
            QTextEdit {
                background-color: #363636;
                border: 1px solid #5d5d5d;
                color: #e0e0e0;
            }
            QLabel {
                color: #e0e0e0;
            }
            QSpinBox {
                background-color: #363636;
                color: #e0e0e0;
                border: 1px solid #5d5d5d;
            }
            QCheckBox {
                color: #e0e0e0;
                spacing: 5px;
            }
            QCheckBox::indicator {
                width: 16px;
                height: 16px;
                border: 1px solid #5d5d5d;
                background: #2d2d2d;
            }
            QCheckBox::indicator:checked {
                background: #007bff;
                border: 1px solid #007bff;
            }
            QCheckBox::indicator:hover {
                border: 1px solid #007bff;
            }
            QMenuBar {
                background-color: #2d2d2d;
                color: #e0e0e0;
            }
            QMenuBar::item:selected {
                background-color: #3d3d3d;
            }
            QMenu {
                background-color: #2d2d2d;
                color: #e0e0e0;
                border: 1px solid #5d5d5d;
            }
            QMenu::item:selected {
                background-color: #3d3d3d;
            }
        )";
    } else {
        // Light theme (default Qt style)
        styleSheet = "";
    }
    
    qApp->setStyleSheet(styleSheet);
}

void ControllerApp::setupMenuBar() {
    menuBar = new QMenuBar(this);
    
    // File menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    
    QAction* saveAction = fileMenu->addAction("&Save Recording");
    connect(saveAction, &QAction::triggered, this, &ControllerApp::saveSequence);
    
    QAction* loadAction = fileMenu->addAction("&Load Recording");
    connect(loadAction, &QAction::triggered, this, &ControllerApp::loadSequence);
    
    QAction* clearAction = fileMenu->addAction("&Clear Recording");
    connect(clearAction, &QAction::triggered, this, &ControllerApp::clearSequence);
    
    fileMenu->addSeparator();
    
    QAction* quitAction = fileMenu->addAction("&Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    // View menu
    QMenu* viewMenu = menuBar->addMenu("&View");
    
    QAction* alwaysOnTopAction = viewMenu->addAction("&Always on Top");
    alwaysOnTopAction->setCheckable(true);
    alwaysOnTopAction->setChecked(alwaysOnTop);
    connect(alwaysOnTopAction, &QAction::toggled, this, &ControllerApp::setAlwaysOnTop);
    
    QAction* darkModeAction = viewMenu->addAction("&Dark Mode");
    darkModeAction->setCheckable(true);
    darkModeAction->setChecked(isDarkMode);
    connect(darkModeAction, &QAction::toggled, this, &ControllerApp::toggleDarkMode);
    
    QAction* showSequenceAction = viewMenu->addAction("&Show Sequence Panel");
    connect(showSequenceAction, &QAction::triggered, this, &ControllerApp::toggleSequencePanel);
    
    // Help menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    
    QAction* aboutAction = helpMenu->addAction("&About Craftium");
    connect(aboutAction, &QAction::triggered, this, &ControllerApp::showAboutDialog);
    
    QAction* helpAction = helpMenu->addAction("&User Guide");
    connect(helpAction, &QAction::triggered, this, &ControllerApp::showHelpDialog);
    
    helpMenu->addSeparator();
    
    QAction* donateAction = helpMenu->addAction("&Donate");
    connect(donateAction, &QAction::triggered, this, &ControllerApp::openDonationPage);
    
    // Add the menu bar to the layout
    layout()->setMenuBar(menuBar);
} 
