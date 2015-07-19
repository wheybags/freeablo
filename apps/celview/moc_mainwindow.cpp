/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[16];
    char stringdata[381];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 23),
QT_MOC_LITERAL(2, 35, 0),
QT_MOC_LITERAL(3, 36, 20),
QT_MOC_LITERAL(4, 57, 25),
QT_MOC_LITERAL(5, 83, 21),
QT_MOC_LITERAL(6, 105, 21),
QT_MOC_LITERAL(7, 127, 22),
QT_MOC_LITERAL(8, 150, 26),
QT_MOC_LITERAL(9, 177, 26),
QT_MOC_LITERAL(10, 204, 41),
QT_MOC_LITERAL(11, 246, 40),
QT_MOC_LITERAL(12, 287, 44),
QT_MOC_LITERAL(13, 332, 17),
QT_MOC_LITERAL(14, 350, 16),
QT_MOC_LITERAL(15, 367, 12)
    },
    "MainWindow\0on_actionExit_triggered\0\0"
    "on_selectMPQ_clicked\0on_selectFileList_clicked\0"
    "on_openButton_clicked\0on_leftButton_clicked\0"
    "on_rightButton_clicked\0"
    "on_startStopButton_clicked\0"
    "on_currentFrame_textEdited\0"
    "on_actionSet_background_color_2_triggered\0"
    "on_actionExport_CEL_CL2_to_PNG_triggered\0"
    "on_actionExport_all_CEL_CL2_to_PNG_triggered\0"
    "itemDoubleClicked\0QListWidgetItem*\0"
    "updateRender\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08,
       3,    0,   80,    2, 0x08,
       4,    0,   81,    2, 0x08,
       5,    0,   82,    2, 0x08,
       6,    0,   83,    2, 0x08,
       7,    0,   84,    2, 0x08,
       8,    0,   85,    2, 0x08,
       9,    1,   86,    2, 0x08,
      10,    0,   89,    2, 0x08,
      11,    0,   90,    2, 0x08,
      12,    0,   91,    2, 0x08,
      13,    1,   92,    2, 0x08,
      15,    0,   95,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,    2,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->on_actionExit_triggered(); break;
        case 1: _t->on_selectMPQ_clicked(); break;
        case 2: _t->on_selectFileList_clicked(); break;
        case 3: _t->on_openButton_clicked(); break;
        case 4: _t->on_leftButton_clicked(); break;
        case 5: _t->on_rightButton_clicked(); break;
        case 6: _t->on_startStopButton_clicked(); break;
        case 7: _t->on_currentFrame_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->on_actionSet_background_color_2_triggered(); break;
        case 9: _t->on_actionExport_CEL_CL2_to_PNG_triggered(); break;
        case 10: _t->on_actionExport_all_CEL_CL2_to_PNG_triggered(); break;
        case 11: _t->itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 12: _t->updateRender(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
