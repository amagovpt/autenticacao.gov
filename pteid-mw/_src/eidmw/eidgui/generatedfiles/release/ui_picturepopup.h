/********************************************************************************
** Form generated from reading ui file 'picturepopup.ui'
**
** Created: Sun 31. Jul 19:32:55 2011
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PICTUREPOPUP_H
#define UI_PICTUREPOPUP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_PicturePopupClass
{
public:

    void setupUi(QLabel *PicturePopupClass)
    {
        if (PicturePopupClass->objectName().isEmpty())
            PicturePopupClass->setObjectName(QString::fromUtf8("PicturePopupClass"));
        PicturePopupClass->setWindowModality(Qt::WindowModal);
        PicturePopupClass->resize(400, 300);

        retranslateUi(PicturePopupClass);

        QMetaObject::connectSlotsByName(PicturePopupClass);
    } // setupUi

    void retranslateUi(QLabel *PicturePopupClass)
    {
        PicturePopupClass->setWindowTitle(QApplication::translate("PicturePopupClass", "PicturePopup", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(PicturePopupClass);
    } // retranslateUi

};

namespace Ui {
    class PicturePopupClass: public Ui_PicturePopupClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PICTUREPOPUP_H
