/****************************************************************************
** Meta object code from reading C++ file 'graphicspage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "graphicspage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'graphicspage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Launcher__GraphicsPage_t {
    QByteArrayData data[6];
    char stringdata[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_Launcher__GraphicsPage_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_Launcher__GraphicsPage_t qt_meta_stringdata_Launcher__GraphicsPage = {
    {
QT_MOC_LITERAL(0, 0, 22),
QT_MOC_LITERAL(1, 23, 8),
QT_MOC_LITERAL(2, 32, 0),
QT_MOC_LITERAL(3, 33, 23),
QT_MOC_LITERAL(4, 57, 6),
QT_MOC_LITERAL(5, 64, 13)
    },
    "Launcher::GraphicsPage\0setupSDL\0\0"
    "getAvailableResolutions\0screen\0"
    "screenChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Launcher__GraphicsPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x08,
       3,    1,   30,    2, 0x08,
       5,    1,   33,    2, 0x08,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::QStringList, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,

       0        // eod
};

void Launcher::GraphicsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GraphicsPage *_t = static_cast<GraphicsPage *>(_o);
        switch (_id) {
        case 0: { bool _r = _t->setupSDL();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { QStringList _r = _t->getAvailableResolutions((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 2: _t->screenChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Launcher::GraphicsPage::staticMetaObject = {
    { &Page::staticMetaObject, qt_meta_stringdata_Launcher__GraphicsPage.data,
      qt_meta_data_Launcher__GraphicsPage,  qt_static_metacall, 0, 0}
};


const QMetaObject *Launcher::GraphicsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Launcher::GraphicsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Launcher__GraphicsPage.stringdata))
        return static_cast<void*>(const_cast< GraphicsPage*>(this));
    return Page::qt_metacast(_clname);
}

int Launcher::GraphicsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Page::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
