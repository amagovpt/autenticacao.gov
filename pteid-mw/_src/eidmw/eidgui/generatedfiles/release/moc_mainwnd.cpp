/****************************************************************************
** Meta object code from reading C++ file 'mainwnd.h'
**
** Created: Sun 31. Jul 19:32:53 2011
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWnd[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      35,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      15,    9,    8,    8, 0x08,
      57,    8,    8,    8, 0x08,
      84,    8,    8,    8, 0x08,
     116,    8,    8,    8, 0x08,
     143,    8,    8,    8, 0x08,
     173,    8,    8,    8, 0x08,
     203,    8,    8,    8, 0x08,
     236,    8,    8,    8, 0x08,
     267,    8,    8,    8, 0x08,
     305,    8,    8,    8, 0x08,
     337,    8,    8,    8, 0x08,
     368,    8,    8,    8, 0x08,
     397,    8,    8,    8, 0x08,
     426,    8,    8,    8, 0x08,
     456,    8,    8,    8, 0x08,
     483,    8,    8,    8, 0x08,
     499,    8,    8,    8, 0x08,
     515,    8,    8,    8, 0x08,
     531,    8,    8,    8, 0x08,
     547,    8,    8,    8, 0x08,
     571,  563,    8,    8, 0x08,
     602,  595,    8,    8, 0x08,
     651,    8,    8,    8, 0x08,
     676,    8,    8,    8, 0x08,
     715,  703,    8,    8, 0x08,
     760,  703,    8,    8, 0x08,
     806,    8,    8,    8, 0x08,
     841,    8,    8,    8, 0x08,
     870,    8,    8,    8, 0x08,
     900,    8,    8,    8, 0x08,
     934,    8,    8,    8, 0x08,
     969,  960,    8,    8, 0x08,
     994,    8,    8,    8, 0x08,
    1019, 1013,    8,    8, 0x08,
    1040, 1013,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWnd[] = {
    "MainWnd\0\0index\0on_tabWidget_Identity_currentChanged(int)\0"
    "on_actionAbout_triggered()\0"
    "on_actionReload_eID_triggered()\0"
    "on_actionClear_triggered()\0"
    "on_actionOpen_eID_triggered()\0"
    "on_actionSave_eID_triggered()\0"
    "on_actionSave_eID_as_triggered()\0"
    "on_actionPrint_eID_triggered()\0"
    "on_actionPrinter_Settings_triggered()\0"
    "on_actionPINRequest_triggered()\0"
    "on_actionPINChange_triggered()\0"
    "on_actionOptions_triggered()\0"
    "on_actionZoom_In_triggered()\0"
    "on_actionZoom_Out_triggered()\0"
    "on_actionE_xit_triggered()\0setLanguageEn()\0"
    "setLanguageNl()\0setLanguageFr()\0"
    "setLanguageDe()\0restoreWindow()\0message\0"
    "messageRespond(QString)\0reason\0"
    "iconActivated(QSystemTrayIcon::ActivationReason)\0"
    "on_btnPIN_Test_clicked()\0"
    "on_btnPIN_Change_clicked()\0item,column\0"
    "on_treePIN_itemClicked(QTreeWidgetItem*,int)\0"
    "on_treeCert_itemClicked(QTreeWidgetItem*,int)\0"
    "on_treeCert_itemSelectionChanged()\0"
    "on_btnCert_Details_clicked()\0"
    "on_btnCert_Register_clicked()\0"
    "on_treePIN_itemSelectionChanged()\0"
    "on_btnOCSPCheck_clicked()\0fileName\0"
    "OpenSelectedEid(QString)\0updateReaderList()\0"
    "event\0customEvent(QEvent*)\0"
    "changeEvent(QEvent*)\0"
};

const QMetaObject MainWnd::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWnd,
      qt_meta_data_MainWnd, 0 }
};

const QMetaObject *MainWnd::metaObject() const
{
    return &staticMetaObject;
}

void *MainWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWnd))
        return static_cast<void*>(const_cast< MainWnd*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_tabWidget_Identity_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: on_actionAbout_triggered(); break;
        case 2: on_actionReload_eID_triggered(); break;
        case 3: on_actionClear_triggered(); break;
        case 4: on_actionOpen_eID_triggered(); break;
        case 5: on_actionSave_eID_triggered(); break;
        case 6: on_actionSave_eID_as_triggered(); break;
        case 7: on_actionPrint_eID_triggered(); break;
        case 8: on_actionPrinter_Settings_triggered(); break;
        case 9: on_actionPINRequest_triggered(); break;
        case 10: on_actionPINChange_triggered(); break;
        case 11: on_actionOptions_triggered(); break;
        case 12: on_actionZoom_In_triggered(); break;
        case 13: on_actionZoom_Out_triggered(); break;
        case 14: on_actionE_xit_triggered(); break;
        case 15: setLanguageEn(); break;
        case 16: setLanguageNl(); break;
        case 17: setLanguageFr(); break;
        case 18: setLanguageDe(); break;
        case 19: restoreWindow(); break;
        case 20: messageRespond((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 21: iconActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 22: on_btnPIN_Test_clicked(); break;
        case 23: on_btnPIN_Change_clicked(); break;
        case 24: on_treePIN_itemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 25: on_treeCert_itemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 26: on_treeCert_itemSelectionChanged(); break;
        case 27: on_btnCert_Details_clicked(); break;
        case 28: on_btnCert_Register_clicked(); break;
        case 29: on_treePIN_itemSelectionChanged(); break;
        case 30: on_btnOCSPCheck_clicked(); break;
        case 31: OpenSelectedEid((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 32: updateReaderList(); break;
        case 33: customEvent((*reinterpret_cast< QEvent*(*)>(_a[1]))); break;
        case 34: changeEvent((*reinterpret_cast< QEvent*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 35;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
