/****************************************************************************
** Meta object code from reading C++ file 'dlgOptions.h'
**
** Created: Sun 31. Jul 19:32:54 2011
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../dlgOptions.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dlgOptions.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_dlgOptions[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   12,   11,   11, 0x08,
      53,   12,   11,   11, 0x08,
      85,   12,   11,   11, 0x08,
     115,   12,   11,   11, 0x08,
     151,   12,   11,   11, 0x08,
     184,   12,   11,   11, 0x08,
     225,  219,   11,   11, 0x08,
     257,   12,   11,   11, 0x08,
     285,   12,   11,   11, 0x08,
     324,  316,   11,   11, 0x08,
     355,  316,   11,   11, 0x08,
     387,  316,   11,   11, 0x08,
     420,  316,   11,   11, 0x08,
     450,  316,   11,   11, 0x08,
     481,  316,   11,   11, 0x08,
     513,   12,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_dlgOptions[] = {
    "dlgOptions\0\0bChecked\0"
    "on_chbShowToolbar_toggled(bool)\0"
    "on_chbShowPicture_toggled(bool)\0"
    "on_chbUseKeyPad_toggled(bool)\0"
    "on_chbAutoCardReading_toggled(bool)\0"
    "on_chbWinAutoStart_toggled(bool)\0"
    "on_chbStartMinimized_toggled(bool)\0"
    "index\0on_cmbCardReader_activated(int)\0"
    "on_chbRegCert_toggled(bool)\0"
    "on_chbRemoveCert_toggled(bool)\0checked\0"
    "on_rbOCSPNotUsed_clicked(bool)\0"
    "on_rbOCSPOptional_clicked(bool)\0"
    "on_rbOCSPMandatory_clicked(bool)\0"
    "on_rbCRLNotUsed_clicked(bool)\0"
    "on_rbCRLOptional_clicked(bool)\0"
    "on_rbCRLMandatory_clicked(bool)\0"
    "on_chbShowNotification_toggled(bool)\0"
};

const QMetaObject dlgOptions::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_dlgOptions,
      qt_meta_data_dlgOptions, 0 }
};

const QMetaObject *dlgOptions::metaObject() const
{
    return &staticMetaObject;
}

void *dlgOptions::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_dlgOptions))
        return static_cast<void*>(const_cast< dlgOptions*>(this));
    return QDialog::qt_metacast(_clname);
}

int dlgOptions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_chbShowToolbar_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: on_chbShowPicture_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: on_chbUseKeyPad_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: on_chbAutoCardReading_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: on_chbWinAutoStart_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: on_chbStartMinimized_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: on_cmbCardReader_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: on_chbRegCert_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: on_chbRemoveCert_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: on_rbOCSPNotUsed_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: on_rbOCSPOptional_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: on_rbOCSPMandatory_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: on_rbCRLNotUsed_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: on_rbCRLOptional_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: on_rbCRLMandatory_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: on_chbShowNotification_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
