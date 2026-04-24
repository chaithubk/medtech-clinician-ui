/****************************************************************************
** Meta object code from reading C++ file 'dashboard_window.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/dashboard_window.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dashboard_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_DashboardWindow_t {
    uint offsetsAndSizes[16];
    char stringdata0[16];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[19];
    char stringdata4[22];
    char stringdata5[6];
    char stringdata6[8];
    char stringdata7[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DashboardWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DashboardWindow_t qt_meta_stringdata_DashboardWindow = {
    {
        QT_MOC_LITERAL(0, 15),  // "DashboardWindow"
        QT_MOC_LITERAL(16, 15),  // "onMQTTConnected"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 18),  // "onMQTTDisconnected"
        QT_MOC_LITERAL(52, 21),  // "onMQTTMessageReceived"
        QT_MOC_LITERAL(74, 5),  // "topic"
        QT_MOC_LITERAL(80, 7),  // "payload"
        QT_MOC_LITERAL(88, 9)   // "onTimeout"
    },
    "DashboardWindow",
    "onMQTTConnected",
    "",
    "onMQTTDisconnected",
    "onMQTTMessageReceived",
    "topic",
    "payload",
    "onTimeout"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DashboardWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x0a,    1 /* Public */,
       3,    0,   39,    2, 0x0a,    2 /* Public */,
       4,    2,   40,    2, 0x0a,    3 /* Public */,
       7,    0,   45,    2, 0x0a,    6 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,    6,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject DashboardWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DashboardWindow.offsetsAndSizes,
    qt_meta_data_DashboardWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DashboardWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DashboardWindow, std::true_type>,
        // method 'onMQTTConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMQTTDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMQTTMessageReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void DashboardWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DashboardWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onMQTTConnected(); break;
        case 1: _t->onMQTTDisconnected(); break;
        case 2: _t->onMQTTMessageReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->onTimeout(); break;
        default: ;
        }
    }
}

const QMetaObject *DashboardWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DashboardWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DashboardWindow.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DashboardWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
