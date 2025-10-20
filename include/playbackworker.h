#ifndef PLAYBACKWORKER_H
#define PLAYBACKWORKER_H

#include <QObject>
#include <QThread>
#include <vector>
#include <atomic>
#include "controllerapp.h" // For KeyEvent struct definition

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <Carbon/Carbon.h> // For CGKeyCode and event functions
#endif

class PlaybackWorker : public QObject {
    Q_OBJECT

public:
    explicit PlaybackWorker(QObject *parent = nullptr);

public slots:
    void doWork(const std::vector<KeyEvent>& sequence, int repeatCount = 1);
    void stopWork();

signals:
    void finished();

private:
    void emulate_key_press(const KeyEvent& event);

    std::atomic<bool> m_running{false};
};

#endif // PLAYBACKWORKER_H 