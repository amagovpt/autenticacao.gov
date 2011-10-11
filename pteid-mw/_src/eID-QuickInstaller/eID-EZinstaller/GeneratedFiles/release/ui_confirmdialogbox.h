/********************************************************************************
** Form generated from reading ui file 'confirmdialogbox.ui'
**
** Created: Sun 31. Jul 19:34:10 2011
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CONFIRMDIALOGBOX_H
#define UI_CONFIRMDIALOGBOX_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCommandLinkButton>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_confirmDialogBoxClass
{
public:
    QLabel *lblExclamationMark;
    QLabel *lblAreYouSure;
    QLabel *lblOnlyTested;
    QLabel *lblWarning;
    QLabel *lblTitle;
    QCommandLinkButton *clbCancel;
    QCommandLinkButton *clbTestSignature;
    QCommandLinkButton *clbDoNotTestSignature;

    void setupUi(QDialog *confirmDialogBoxClass)
    {
        if (confirmDialogBoxClass->objectName().isEmpty())
            confirmDialogBoxClass->setObjectName(QString::fromUtf8("confirmDialogBoxClass"));
        confirmDialogBoxClass->resize(591, 348);
        confirmDialogBoxClass->setMinimumSize(QSize(591, 348));
        confirmDialogBoxClass->setMaximumSize(QSize(591, 348));
        lblExclamationMark = new QLabel(confirmDialogBoxClass);
        lblExclamationMark->setObjectName(QString::fromUtf8("lblExclamationMark"));
        lblExclamationMark->setGeometry(QRect(20, 60, 91, 101));
        lblAreYouSure = new QLabel(confirmDialogBoxClass);
        lblAreYouSure->setObjectName(QString::fromUtf8("lblAreYouSure"));
        lblAreYouSure->setGeometry(QRect(120, 70, 381, 61));
        QFont font;
        font.setFamily(QString::fromUtf8("Tahoma"));
        font.setPointSize(12);
        font.setBold(true);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(75);
        font.setStrikeOut(false);
        lblAreYouSure->setFont(font);
        lblAreYouSure->setWordWrap(true);
        lblOnlyTested = new QLabel(confirmDialogBoxClass);
        lblOnlyTested->setObjectName(QString::fromUtf8("lblOnlyTested"));
        lblOnlyTested->setGeometry(QRect(120, 130, 381, 61));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Tahoma"));
        font1.setPointSize(12);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setWeight(50);
        font1.setStrikeOut(false);
        lblOnlyTested->setFont(font1);
        lblOnlyTested->setWordWrap(true);
        lblWarning = new QLabel(confirmDialogBoxClass);
        lblWarning->setObjectName(QString::fromUtf8("lblWarning"));
        lblWarning->setGeometry(QRect(120, 210, 451, 61));
        lblWarning->setFont(font1);
        lblWarning->setWordWrap(true);
        lblTitle = new QLabel(confirmDialogBoxClass);
        lblTitle->setObjectName(QString::fromUtf8("lblTitle"));
        lblTitle->setGeometry(QRect(100, 20, 311, 31));
        QPalette palette;
        QBrush brush(QColor(95, 86, 130, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(118, 116, 108, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        lblTitle->setPalette(palette);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Segoe UI"));
        font2.setPointSize(14);
        font2.setBold(true);
        font2.setItalic(false);
        font2.setUnderline(false);
        font2.setWeight(75);
        font2.setStrikeOut(false);
        lblTitle->setFont(font2);
        clbCancel = new QCommandLinkButton(confirmDialogBoxClass);
        clbCancel->setObjectName(QString::fromUtf8("clbCancel"));
        clbCancel->setGeometry(QRect(450, 0, 95, 55));
        clbCancel->setFocusPolicy(Qt::NoFocus);
        clbCancel->setIconSize(QSize(85, 40));
        clbCancel->setAutoDefault(false);
        clbTestSignature = new QCommandLinkButton(confirmDialogBoxClass);
        clbTestSignature->setObjectName(QString::fromUtf8("clbTestSignature"));
        clbTestSignature->setGeometry(QRect(90, 270, 176, 71));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Segoe UI"));
        font3.setPointSize(12);
        font3.setBold(true);
        font3.setItalic(false);
        font3.setUnderline(false);
        font3.setWeight(75);
        font3.setStrikeOut(false);
        clbTestSignature->setFont(font3);
        clbTestSignature->setCursor(QCursor(Qt::PointingHandCursor));
        clbTestSignature->setFocusPolicy(Qt::NoFocus);
        clbTestSignature->setIconSize(QSize(166, 65));
        clbDoNotTestSignature = new QCommandLinkButton(confirmDialogBoxClass);
        clbDoNotTestSignature->setObjectName(QString::fromUtf8("clbDoNotTestSignature"));
        clbDoNotTestSignature->setGeometry(QRect(280, 270, 176, 71));
        clbDoNotTestSignature->setFont(font3);
        clbDoNotTestSignature->setCursor(QCursor(Qt::PointingHandCursor));
        clbDoNotTestSignature->setFocusPolicy(Qt::NoFocus);
        clbDoNotTestSignature->setIconSize(QSize(166, 65));

        retranslateUi(confirmDialogBoxClass);

        QMetaObject::connectSlotsByName(confirmDialogBoxClass);
    } // setupUi

    void retranslateUi(QDialog *confirmDialogBoxClass)
    {
        confirmDialogBoxClass->setWindowTitle(QApplication::translate("confirmDialogBoxClass", "e-ID Quick Installer", 0, QApplication::UnicodeUTF8));
        lblExclamationMark->setText(QApplication::translate("confirmDialogBoxClass", "Uitroepteken !", 0, QApplication::UnicodeUTF8));
        lblOnlyTested->setText(QApplication::translate("confirmDialogBoxClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:12pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">De handtekening wordt enkel getest, niet opgeslagen of gebruikt voor andere doeleinden.</p></body></html>", 0, QApplication::UnicodeUTF8));
        lblWarning->setText(QApplication::translate("confirmDialogBoxClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:12pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Let op : </span>als je de verkeerde pincode ingeeft,  kun je de elektronische identiteitskaart blokkeren.</p></body></html>", 0, QApplication::UnicodeUTF8));
        lblTitle->setText(QApplication::translate("confirmDialogBoxClass", "Handtekening testen", 0, QApplication::UnicodeUTF8));
        clbTestSignature->setText(QApplication::translate("confirmDialogBoxClass", "Testen", 0, QApplication::UnicodeUTF8));
        clbDoNotTestSignature->setText(QApplication::translate("confirmDialogBoxClass", "Niet testen", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(confirmDialogBoxClass);
    } // retranslateUi

};

namespace Ui {
    class confirmDialogBoxClass: public Ui_confirmDialogBoxClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIRMDIALOGBOX_H
