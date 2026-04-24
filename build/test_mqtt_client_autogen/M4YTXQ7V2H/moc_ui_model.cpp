/****************************************************************************
** Meta object code from reading C++ file 'ui_model.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/models/ui_model.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ui_model.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_UIModel_t {
    uint offsetsAndSizes[24];
    char stringdata0[8];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[14];
    char stringdata4[11];
    char stringdata5[7];
    char stringdata6[9];
    char stringdata7[9];
    char stringdata8[9];
    char stringdata9[11];
    char stringdata10[14];
    char stringdata11[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_UIModel_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_UIModel_t qt_meta_stringdata_UIModel = {
    {
        QT_MOC_LITERAL(0, 7),  // "UIModel"
        QT_MOC_LITERAL(8, 13),  // "vitalsUpdated"
        QT_MOC_LITERAL(22, 0),  // ""
        QT_MOC_LITERAL(23, 13),  // "statusChanged"
        QT_MOC_LITERAL(37, 10),  // "new_status"
        QT_MOC_LITERAL(48, 6),  // "status"
        QT_MOC_LITERAL(55, 8),  // "hr_value"
        QT_MOC_LITERAL(64, 8),  // "bp_value"
        QT_MOC_LITERAL(73, 8),  // "o2_value"
        QT_MOC_LITERAL(82, 10),  // "temp_value"
        QT_MOC_LITERAL(93, 13),  // "quality_value"
        QT_MOC_LITERAL(107, 11)   // "last_update"
    },
    "UIModel",
    "vitalsUpdated",
    "",
    "statusChanged",
    "new_status",
    "status",
    "hr_value",
    "bp_value",
    "o2_value",
    "temp_value",
    "quality_value",
    "last_update"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_UIModel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       7,   30, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   26,    2, 0x06,    8 /* Public */,
       3,    1,   27,    2, 0x06,    9 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,

 // properties: name, type, flags
       5, QMetaType::QString, 0x00015001, uint(1), 0,
       6, QMetaType::QString, 0x00015001, uint(0), 0,
       7, QMetaType::QString, 0x00015001, uint(0), 0,
       8, QMetaType::QString, 0x00015001, uint(0), 0,
       9, QMetaType::QString, 0x00015001, uint(0), 0,
      10, QMetaType::QString, 0x00015001, uint(0), 0,
      11, QMetaType::QString, 0x00015001, uint(0), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject UIModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_UIModel.offsetsAndSizes,
    qt_meta_data_UIModel,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_UIModel_t,
        // property 'status'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'hr_value'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'bp_value'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'o2_value'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'temp_value'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'quality_value'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'last_update'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UIModel, std::true_type>,
        // method 'vitalsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void UIModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UIModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->vitalsUpdated(); break;
        case 1: _t->statusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UIModel::*)();
            if (_t _q_method = &UIModel::vitalsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UIModel::*)(const QString & );
            if (_t _q_method = &UIModel::statusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<UIModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->getStatus(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->getHrValue(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->getBpValue(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->getO2Value(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->getTempValue(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->getQualityValue(); break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->getLastUpdate(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *UIModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UIModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UIModel.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UIModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void UIModel::vitalsUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void UIModel::statusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
