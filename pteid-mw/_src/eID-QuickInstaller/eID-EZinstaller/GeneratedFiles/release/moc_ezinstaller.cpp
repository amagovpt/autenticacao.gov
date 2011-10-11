/****************************************************************************
** Meta object code from reading C++ file 'ezinstaller.h'
**
** Created: Sun 31. Jul 19:34:10 2011
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ezinstaller.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ezinstaller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ezInstaller[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      36,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      36,   12,   12,   12, 0x08,
      58,   12,   12,   12, 0x08,
      86,   12,   12,   12, 0x08,
     113,   12,   12,   12, 0x08,
     140,   12,   12,   12, 0x08,
     166,   12,   12,   12, 0x08,
     188,   12,   12,   12, 0x08,
     214,   12,   12,   12, 0x08,
     241,   12,   12,   12, 0x08,
     263,   12,   12,   12, 0x08,
     284,   12,   12,   12, 0x08,
     313,   12,   12,   12, 0x08,
     341,   12,   12,   12, 0x08,
     369,   12,   12,   12, 0x08,
     401,   12,   12,   12, 0x08,
     435,   12,   12,   12, 0x08,
     456,   12,   12,   12, 0x08,
     482,   12,   12,   12, 0x08,
     507,   12,   12,   12, 0x08,
     535,   12,   12,   12, 0x08,
     562,   12,   12,   12, 0x08,
     586,   12,   12,   12, 0x08,
     609,   12,   12,   12, 0x08,
     632,   12,   12,   12, 0x08,
     663,   12,   12,   12, 0x08,
     695,   12,   12,   12, 0x08,
     726,   12,   12,   12, 0x08,
     752,   12,   12,   12, 0x08,
     775,   12,   12,   12, 0x08,
     799,   12,   12,   12, 0x08,
     832,   12,   12,   12, 0x08,
     858,   12,   12,   12, 0x08,
     883,   12,   12,   12, 0x08,
     910,   12,   12,   12, 0x08,
     947,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ezInstaller[] = {
    "ezInstaller\0\0on_clbClose_released()\0"
    "on_clbClose_pressed()\0on_clbOpenReport_released()\0"
    "on_clbOpenReport_pressed()\0"
    "on_clbSaveAsPdf_released()\0"
    "on_clbSaveAsPdf_pressed()\0"
    "on_clbClose_clicked()\0on_clbSaveAsPdf_clicked()\0"
    "on_clbOpenReport_clicked()\0"
    "on_clbBack_released()\0on_clbBack_pressed()\0"
    "on_clbViewDetails_released()\0"
    "on_clbViewDetails_pressed()\0"
    "on_clbViewDetails_clicked()\0"
    "on_lblNext_linkHovered(QString)\0"
    "on_lblNext_linkActivated(QString)\0"
    "on_clbBack_clicked()\0on_clbFrancais_released()\0"
    "on_clbFrancais_pressed()\0"
    "on_clbNederlands_released()\0"
    "on_clbNederlands_pressed()\0"
    "on_clbCancel_released()\0on_clbCancel_pressed()\0"
    "on_clbCancel_clicked()\0"
    "on_commandLinkButton_clicked()\0"
    "on_commandLinkButton_released()\0"
    "on_commandLinkButton_pressed()\0"
    "on_pushButton_3_clicked()\0"
    "on_btnCancel_clicked()\0on_clbPicBack_clicked()\0"
    "on_lineEdit_textChanged(QString)\0"
    "on_textEdit_textChanged()\0"
    "on_clbFrancais_clicked()\0"
    "on_clbNederlands_clicked()\0"
    "on_stackedWidget_currentChanged(int)\0"
    "on_btnNext_clicked()\0"
};

const QMetaObject ezInstaller::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ezInstaller,
      qt_meta_data_ezInstaller, 0 }
};

const QMetaObject *ezInstaller::metaObject() const
{
    return &staticMetaObject;
}

void *ezInstaller::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ezInstaller))
        return static_cast<void*>(const_cast< ezInstaller*>(this));
    return QDialog::qt_metacast(_clname);
}

int ezInstaller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_clbClose_released(); break;
        case 1: on_clbClose_pressed(); break;
        case 2: on_clbOpenReport_released(); break;
        case 3: on_clbOpenReport_pressed(); break;
        case 4: on_clbSaveAsPdf_released(); break;
        case 5: on_clbSaveAsPdf_pressed(); break;
        case 6: on_clbClose_clicked(); break;
        case 7: on_clbSaveAsPdf_clicked(); break;
        case 8: on_clbOpenReport_clicked(); break;
        case 9: on_clbBack_released(); break;
        case 10: on_clbBack_pressed(); break;
        case 11: on_clbViewDetails_released(); break;
        case 12: on_clbViewDetails_pressed(); break;
        case 13: on_clbViewDetails_clicked(); break;
        case 14: on_lblNext_linkHovered((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 15: on_lblNext_linkActivated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: on_clbBack_clicked(); break;
        case 17: on_clbFrancais_released(); break;
        case 18: on_clbFrancais_pressed(); break;
        case 19: on_clbNederlands_released(); break;
        case 20: on_clbNederlands_pressed(); break;
        case 21: on_clbCancel_released(); break;
        case 22: on_clbCancel_pressed(); break;
        case 23: on_clbCancel_clicked(); break;
        case 24: on_commandLinkButton_clicked(); break;
        case 25: on_commandLinkButton_released(); break;
        case 26: on_commandLinkButton_pressed(); break;
        case 27: on_pushButton_3_clicked(); break;
        case 28: on_btnCancel_clicked(); break;
        case 29: on_clbPicBack_clicked(); break;
        case 30: on_lineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 31: on_textEdit_textChanged(); break;
        case 32: on_clbFrancais_clicked(); break;
        case 33: on_clbNederlands_clicked(); break;
        case 34: on_stackedWidget_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 35: on_btnNext_clicked(); break;
        default: ;
        }
        _id -= 36;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
