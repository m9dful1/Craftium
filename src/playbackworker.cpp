#include "../include/playbackworker.h"
#include <QDebug>
#include <chrono>

#ifdef _WIN32
// Make sure windows.h is included via playbackworker.h or here if needed directly
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

PlaybackWorker::PlaybackWorker(QObject *parent)
    : QObject(parent) {}

void PlaybackWorker::doWork(const std::vector<KeyEvent>& sequence, int repeatCount) {
    m_running = true;
    qDebug() << "PlaybackWorker started in thread:" << QThread::currentThread() 
             << "with repeat count:" << repeatCount;

    // Add a small initial delay to ensure the target application has focus
    QThread::msleep(300);
    if (!m_running) {
        qDebug() << "PlaybackWorker stopped during initial delay.";
        emit finished();
        return;
    }

    // Loop for the requested number of repetitions
    for (int rep = 0; rep < repeatCount && m_running; rep++) {
        if (rep > 0) {
            // Add a small pause between repetitions
            QThread::msleep(500);
            if (!m_running) break;
        }
        
        qDebug() << "Playing repetition" << (rep + 1) << "of" << repeatCount;
        
        // Play the sequence
        for (const auto& event : sequence) {
            if (!m_running) {
                qDebug() << "PlaybackWorker stopping early.";
                break;
            }
    
            // Use QThread::msleep for delays in the worker thread
            // Ensure delay is non-negative
            long long delay = event.delay;
            if (delay > 0) {
                QThread::msleep(static_cast<unsigned long>(delay));
            }
    
            // Check again after sleep in case stopWork was called during msleep
            if (!m_running) {
                qDebug() << "PlaybackWorker stopping early after delay.";
                break;
            }
    
            emulate_key_press(event);
        }
    }

    // Ensure m_running is reset regardless of loop break reason
    m_running = false;
    qDebug() << "PlaybackWorker finished processing sequence with" << repeatCount << "repetitions.";
    emit finished(); // Signal completion
}

void PlaybackWorker::stopWork() {
    qDebug() << "PlaybackWorker requested to stop.";
    m_running = false; // Set the flag to stop the loop in doWork
}

void PlaybackWorker::emulate_key_press(const KeyEvent& event) {
#ifdef __APPLE__
    // macOS implementation using CGEvent (existing code)
    CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    if (source == NULL) {
        qDebug() << "PlaybackWorker: Failed to create event source";
        return;
    }

    bool isDown = (event.state == "down");
    CGEventRef cgEvent = CGEventCreateKeyboardEvent(source, event.macKeyCode, isDown);
    if (cgEvent == NULL) {
        qDebug() << "PlaybackWorker: Failed to create keyboard event for key:" << QString::fromStdString(event.key);
        CFRelease(source);
        return;
    }

    CGEventPost(kCGHIDEventTap, cgEvent);
    CFRelease(cgEvent);
    CFRelease(source);

    // qDebug() << "PlaybackWorker: Emulated (macOS)" << QString::fromStdString(event.key) << event.state;

#elif defined(_WIN32)
    // Windows implementation using SendInput
    if (event.winKeyCode == 0) {
        qDebug() << "PlaybackWorker (Win): Invalid key code 0 for key:" << QString::fromStdString(event.key);
        return;
    }

    INPUT input = {0}; // Use = {0} to zero-initialize
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = event.winKeyCode;

    // Set KEYEVENTF_KEYUP for key release
    if (event.state == "up") {
        input.ki.dwFlags = KEYEVENTF_KEYUP;
    }

    // Special handling for extended keys (e.g., Right Ctrl, Right Alt, Arrow keys, etc.)
    // See: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    if (event.winKeyCode == VK_RCONTROL || event.winKeyCode == VK_RMENU ||
        event.winKeyCode == VK_INSERT || event.winKeyCode == VK_DELETE ||
        event.winKeyCode == VK_HOME || event.winKeyCode == VK_END ||
        event.winKeyCode == VK_PRIOR || event.winKeyCode == VK_NEXT || // PageUp, PageDown
        event.winKeyCode == VK_LEFT || event.winKeyCode == VK_UP ||
        event.winKeyCode == VK_RIGHT || event.winKeyCode == VK_DOWN ||
        event.winKeyCode == VK_NUMLOCK || event.winKeyCode == VK_SNAPSHOT /*PrintScreen*/ ||
        event.winKeyCode == VK_CANCEL || /* Pause/Break often sends VK_CANCEL */ 
        event.winKeyCode == VK_DIVIDE /* Numpad Divide */)
    {
        input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    // For some special keys, we might need to use scan codes instead of virtual key codes
    // Use scan code if vkCode is unclear (e.g., certain international keys)
    if (event.key == "Unknown" || event.winKeyCode > 255) {
        // Try using scan code if available
        UINT scanCode = MapVirtualKey(event.winKeyCode, MAPVK_VK_TO_VSC);
        if (scanCode) {
            input.ki.wScan = static_cast<WORD>(scanCode);
            input.ki.dwFlags |= KEYEVENTF_SCANCODE; // Use scan code
            // Don't use vk in this case
            input.ki.wVk = 0;
        }
    }

    UINT result = SendInput(1, &input, sizeof(INPUT));
    if (result != 1) {
        qDebug() << "PlaybackWorker (Win): SendInput failed with error code:" << GetLastError()
                 << "for key:" << QString::fromStdString(event.key) << "state:" << QString::fromStdString(event.state);
    }
    // else {
    //     qDebug() << "PlaybackWorker: Emulated (Win)" << QString::fromStdString(event.key) << event.state;
    // }

#else
    qDebug() << "PlaybackWorker: Key emulation not supported on this platform.";
#endif
} 