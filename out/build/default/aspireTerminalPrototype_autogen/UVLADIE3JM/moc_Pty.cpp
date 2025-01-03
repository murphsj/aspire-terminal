/****************************************************************************
** Meta object code from reading C++ file 'Pty.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../src/Pty.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Pty.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_Pty_t {
    uint offsetsAndSizes[20];
    char stringdata0[4];
    char stringdata1[9];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[7];
    char stringdata5[15];
    char stringdata6[18];
    char stringdata7[7];
    char stringdata8[22];
    char stringdata9[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Pty_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Pty_t qt_meta_stringdata_Pty = {
    {
        QT_MOC_LITERAL(0, 3),  // "Pty"
        QT_MOC_LITERAL(4, 8),  // "recieved"
        QT_MOC_LITERAL(13, 0),  // ""
        QT_MOC_LITERAL(14, 16),  // "std::string_view"
        QT_MOC_LITERAL(31, 6),  // "output"
        QT_MOC_LITERAL(38, 14),  // "fdReadNotified"
        QT_MOC_LITERAL(53, 17),  // "QSocketDescriptor"
        QT_MOC_LITERAL(71, 6),  // "socket"
        QT_MOC_LITERAL(78, 21),  // "QSocketNotifier::Type"
        QT_MOC_LITERAL(100, 4)   // "type"
    },
    "Pty",
    "recieved",
    "",
    "std::string_view",
    "output",
    "fdReadNotified",
    "QSocketDescriptor",
    "socket",
    "QSocketNotifier::Type",
    "type"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Pty[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   26,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    2,   29,    2, 0x0a,    3 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8,    7,    9,

       0        // eod
};

Q_CONSTINIT const QMetaObject Pty::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Pty.offsetsAndSizes,
    qt_meta_data_Pty,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Pty_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Pty, std::true_type>,
        // method 'recieved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::string_view, std::false_type>,
        // method 'fdReadNotified'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QSocketDescriptor, std::false_type>,
        QtPrivate::TypeAndForceComplete<QSocketNotifier::Type, std::false_type>
    >,
    nullptr
} };

void Pty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Pty *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->recieved((*reinterpret_cast< std::add_pointer_t<std::string_view>>(_a[1]))); break;
        case 1: _t->fdReadNotified((*reinterpret_cast< std::add_pointer_t<QSocketDescriptor>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QSocketNotifier::Type>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QSocketDescriptor >(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QSocketNotifier::Type >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Pty::*)(std::string_view );
            if (_t _q_method = &Pty::recieved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *Pty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Pty::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Pty.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Pty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Pty::recieved(std::string_view _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
