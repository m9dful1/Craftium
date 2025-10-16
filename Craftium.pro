QT       += core gui widgets
CONFIG   += c++17

TARGET = Craftium
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/controllerapp.cpp

HEADERS += \
    include/controllerapp.h

INCLUDEPATH += include

macx {
    LIBS += -framework Carbon
}

win32 {
    LIBS += -luser32
} 