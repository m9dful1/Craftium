#ifndef CONTROLLERAPP_H
#define CONTROLLERAPP_H

#include <QWidget>
#include <QObject>
#include <QThread>
#include <QLabel>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QCheckBox>
#include <QMenuBar>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <QSettings>
#include <QMutex>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <Carbon/Carbon.h>
#include <CoreGraphics/CoreGraphics.h>
#endif

class PlaybackWorker;

struct KeyEvent {
    std::string key;
    std::string state;
    long long delay;
#ifdef _WIN32
    WORD winKeyCode;
#elif defined(__APPLE__)
    CGKeyCode macKeyCode;
#endif
};

#include <QMetaType>
Q_DECLARE_METATYPE(std::vector<KeyEvent>)

class ControllerApp : public QWidget {
    Q_OBJECT

public:
    explicit ControllerApp(QWidget *parent = nullptr);
    ~ControllerApp() override;

    // Moved from private to public for callback access
#ifdef __APPLE__
    void recordKeyEvent(CGKeyCode keyCode, bool isPress);
#endif

    // Public getter for recording state
    bool isRecording() const;

public slots:
    void startRecording();
    void stopRecording();
    void startPlayback(int repeatCount = 1, bool external = false);
    void stopPlayback();
    void saveSequence();
    void loadSequence();
    void clearSequence();
    void toggleSequencePanel();
    void updateSequenceText();
    void clearFocusFromControls();
    void setAlwaysOnTop(bool enable);
    void toggleDarkMode(bool enabled);
    void applyTheme(bool darkMode = false);
    void showAboutDialog();
    void openDonationPage();
    void showHelpDialog();

signals:
    void startPlaybackSignal(const std::vector<KeyEvent>& sequence);
    void stopPlaybackSignal();

private slots:
    void handlePlaybackFinished();

private:
    void setupUI();
    void setupMenuBar();
    QAction* findActionInMenu(const QList<QAction*>& menuActions, const QString& menuName, const QString& actionName);
    void updateStatusLabel(const QString& message);

#ifdef _WIN32
    std::string vkCodeToString(DWORD vkCode) const;
    WORD stringToVkCode(const std::string& keyName) const;
    void startGlobalKeyListener();
    void stopGlobalKeyListener();
    void recordKeyEvent(DWORD vkCode, bool isPress);
#elif defined(__APPLE__)
    std::string keyCodeToString(CGKeyCode keyCode) const;
    CGKeyCode stringToKeyCode(const std::string& keyName) const;
    bool startGlobalKeyListener();
    void stopGlobalKeyListener();
    void checkAndRequestAccessibilityPermissions();
    CFMachPortRef eventTap = nullptr;
    CFRunLoopSourceRef runLoopSource = nullptr;
#endif

    bool recording;
    bool playing;
    std::vector<KeyEvent> sequence;
    mutable QMutex sequenceMutex;  // Protects sequence vector from concurrent access
    std::chrono::high_resolution_clock::time_point lastEventTime;

    QThread* playbackThread = nullptr;
    PlaybackWorker* playbackWorker = nullptr;

    // UI elements for status display
    QLabel* statusLabel = nullptr;
    QSpinBox* repeatCountSpinner = nullptr;

    // Sequence details panel
    QTextEdit* sequenceTextEdit = nullptr;
    QPushButton* expandButton = nullptr;
    QPropertyAnimation* animation = nullptr;
    bool sequencePanelVisible = false;

    // Always on top control
    QCheckBox* alwaysOnTopCheckbox = nullptr;

    // Dark mode control
    QSettings* settings = nullptr;
    bool isDarkMode = false;
    QMenuBar* menuBar = nullptr;

    // For "Always on Top" feature
    bool alwaysOnTop;

#if defined(__APPLE__)
    // void recordKeyEvent(CGKeyCode keyCode, bool isPress); // Moved to public
#endif
};

#endif // CONTROLLERAPP_H 