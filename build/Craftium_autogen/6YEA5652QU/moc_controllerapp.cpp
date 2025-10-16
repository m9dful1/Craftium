/****************************************************************************
** Meta object code from reading C++ file 'controllerapp.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/controllerapp.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'controllerapp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSControllerAppENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSControllerAppENDCLASS = QtMocHelpers::stringData(
    "ControllerApp",
    "startPlaybackSignal",
    "",
    "std::vector<KeyEvent>",
    "sequence",
    "stopPlaybackSignal",
    "startRecording",
    "stopRecording",
    "startPlayback",
    "repeatCount",
    "external",
    "stopPlayback",
    "saveSequence",
    "loadSequence",
    "clearSequence",
    "toggleSequencePanel",
    "updateSequenceText",
    "clearFocusFromControls",
    "setAlwaysOnTop",
    "enable",
    "toggleDarkMode",
    "enabled",
    "applyTheme",
    "darkMode",
    "showAboutDialog",
    "openDonationPage",
    "showHelpDialog",
    "handlePlaybackFinished"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSControllerAppENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  146,    2, 0x06,    1 /* Public */,
       5,    0,  149,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,  150,    2, 0x0a,    4 /* Public */,
       7,    0,  151,    2, 0x0a,    5 /* Public */,
       8,    2,  152,    2, 0x0a,    6 /* Public */,
       8,    1,  157,    2, 0x2a,    9 /* Public | MethodCloned */,
       8,    0,  160,    2, 0x2a,   11 /* Public | MethodCloned */,
      11,    0,  161,    2, 0x0a,   12 /* Public */,
      12,    0,  162,    2, 0x0a,   13 /* Public */,
      13,    0,  163,    2, 0x0a,   14 /* Public */,
      14,    0,  164,    2, 0x0a,   15 /* Public */,
      15,    0,  165,    2, 0x0a,   16 /* Public */,
      16,    0,  166,    2, 0x0a,   17 /* Public */,
      17,    0,  167,    2, 0x0a,   18 /* Public */,
      18,    1,  168,    2, 0x0a,   19 /* Public */,
      20,    1,  171,    2, 0x0a,   21 /* Public */,
      22,    1,  174,    2, 0x0a,   23 /* Public */,
      22,    0,  177,    2, 0x2a,   25 /* Public | MethodCloned */,
      24,    0,  178,    2, 0x0a,   26 /* Public */,
      25,    0,  179,    2, 0x0a,   27 /* Public */,
      26,    0,  180,    2, 0x0a,   28 /* Public */,
      27,    0,  181,    2, 0x08,   29 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    9,   10,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   21,
    QMetaType::Void, QMetaType::Bool,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ControllerApp::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSControllerAppENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSControllerAppENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSControllerAppENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ControllerApp, std::true_type>,
        // method 'startPlaybackSignal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::vector<KeyEvent> &, std::false_type>,
        // method 'stopPlaybackSignal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startRecording'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopRecording'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'startPlayback'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'startPlayback'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'startPlayback'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopPlayback'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveSequence'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loadSequence'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearSequence'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleSequencePanel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateSequenceText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearFocusFromControls'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setAlwaysOnTop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleDarkMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'applyTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'applyTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showAboutDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openDonationPage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showHelpDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handlePlaybackFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ControllerApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ControllerApp *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->startPlaybackSignal((*reinterpret_cast< std::add_pointer_t<std::vector<KeyEvent>>>(_a[1]))); break;
        case 1: _t->stopPlaybackSignal(); break;
        case 2: _t->startRecording(); break;
        case 3: _t->stopRecording(); break;
        case 4: _t->startPlayback((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 5: _t->startPlayback((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->startPlayback(); break;
        case 7: _t->stopPlayback(); break;
        case 8: _t->saveSequence(); break;
        case 9: _t->loadSequence(); break;
        case 10: _t->clearSequence(); break;
        case 11: _t->toggleSequencePanel(); break;
        case 12: _t->updateSequenceText(); break;
        case 13: _t->clearFocusFromControls(); break;
        case 14: _t->setAlwaysOnTop((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 15: _t->toggleDarkMode((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 16: _t->applyTheme((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 17: _t->applyTheme(); break;
        case 18: _t->showAboutDialog(); break;
        case 19: _t->openDonationPage(); break;
        case 20: _t->showHelpDialog(); break;
        case 21: _t->handlePlaybackFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< std::vector<KeyEvent> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ControllerApp::*)(const std::vector<KeyEvent> & );
            if (_t _q_method = &ControllerApp::startPlaybackSignal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ControllerApp::*)();
            if (_t _q_method = &ControllerApp::stopPlaybackSignal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *ControllerApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ControllerApp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSControllerAppENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ControllerApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void ControllerApp::startPlaybackSignal(const std::vector<KeyEvent> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ControllerApp::stopPlaybackSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
