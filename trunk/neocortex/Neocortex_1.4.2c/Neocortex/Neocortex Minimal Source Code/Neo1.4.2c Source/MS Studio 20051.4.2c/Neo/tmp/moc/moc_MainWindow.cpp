/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created: Wed Jul 14 18:24:48 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyMainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,
      33,   13,   13,   13, 0x0a,
      48,   13,   13,   13, 0x0a,
      65,   13,   13,   13, 0x0a,
      84,   13,   13,   13, 0x0a,
     119,   13,   13,   13, 0x0a,
     141,   13,   13,   13, 0x0a,
     162,   13,   13,   13, 0x0a,
     183,   13,   13,   13, 0x0a,
     207,  200,   13,   13, 0x0a,
     239,  232,   13,   13, 0x0a,
     276,   13,   13,   13, 0x0a,
     288,   13,   13,   13, 0x0a,
     303,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyMainWindow[] = {
    "MyMainWindow\0\0GuiOpenImage(bool)\0"
    "GuiOpenImage()\0GuiLearnFolder()\0"
    "GuiSetParameters()\0"
    "ClearLearningAndCreateNewThinker()\0"
    "GuiRecognizeFlashed()\0GuiRecognizeMoving()\0"
    "GuiRecognizeFolder()\0GuiExitApp(bool)\0"
    "pEvent\0closeEvent(QCloseEvent*)\0pState\0"
    "GuiDisplayPredictionConfidence(bool)\0"
    "Interrupt()\0GuiAbout(bool)\0AboutOK()\0"
};

const QMetaObject MyMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MyMainWindow,
      qt_meta_data_MyMainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MyMainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MyMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MyMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyMainWindow))
        return static_cast<void*>(const_cast< MyMainWindow*>(this));
    if (!strcmp(_clname, "Ui_About"))
        return static_cast< Ui_About*>(const_cast< MyMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MyMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: GuiOpenImage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: GuiOpenImage(); break;
        case 2: GuiLearnFolder(); break;
        case 3: GuiSetParameters(); break;
        case 4: ClearLearningAndCreateNewThinker(); break;
        case 5: GuiRecognizeFlashed(); break;
        case 6: GuiRecognizeMoving(); break;
        case 7: GuiRecognizeFolder(); break;
        case 8: GuiExitApp((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 10: GuiDisplayPredictionConfidence((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: Interrupt(); break;
        case 12: GuiAbout((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: AboutOK(); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
