/********************************************************************************
** Form generated from reading ui file 'dlgOptions.ui'
**
** Created: Sun 31. Jul 19:32:55 2011
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DLGOPTIONS_H
#define UI_DLGOPTIONS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_dlgOptions
{
public:
    QGroupBox *groupBox_2;
    QCheckBox *chbAutoCardReading;
    QCheckBox *chbWinAutoStart;
    QCheckBox *chbStartMinimized;
    QGroupBox *groupBox;
    QCheckBox *chbRegCert;
    QCheckBox *chbRemoveCert;
    QGroupBox *groupBox_3;
    QCheckBox *chbUseKeyPad;
    QCheckBox *chbShowNotification;
    QGroupBox *groupBox_4;
    QComboBox *cmbCardReader;
    QGroupBox *grpCertValidation;
    QGroupBox *grpOCSP;
    QRadioButton *rbOCSPNotUsed;
    QRadioButton *rbOCSPOptional;
    QRadioButton *rbOCSPMandatory;
    QGroupBox *grpCRL;
    QRadioButton *rbCRLNotUsed;
    QRadioButton *rbCRLOptional;
    QRadioButton *rbCRLMandatory;
    QPushButton *okButton;
    QCheckBox *chbShowPicture;
    QCheckBox *chbShowToolbar;

    void setupUi(QDialog *dlgOptions)
    {
        if (dlgOptions->objectName().isEmpty())
            dlgOptions->setObjectName(QString::fromUtf8("dlgOptions"));
        dlgOptions->resize(600, 389);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dlgOptions->sizePolicy().hasHeightForWidth());
        dlgOptions->setSizePolicy(sizePolicy);
        dlgOptions->setMaximumSize(QSize(600, 417));
        dlgOptions->setModal(true);
        groupBox_2 = new QGroupBox(dlgOptions);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 110, 281, 111));
        chbAutoCardReading = new QCheckBox(groupBox_2);
        chbAutoCardReading->setObjectName(QString::fromUtf8("chbAutoCardReading"));
        chbAutoCardReading->setGeometry(QRect(10, 20, 261, 19));
        chbWinAutoStart = new QCheckBox(groupBox_2);
        chbWinAutoStart->setObjectName(QString::fromUtf8("chbWinAutoStart"));
        chbWinAutoStart->setGeometry(QRect(10, 80, 261, 19));
        chbStartMinimized = new QCheckBox(groupBox_2);
        chbStartMinimized->setObjectName(QString::fromUtf8("chbStartMinimized"));
        chbStartMinimized->setGeometry(QRect(10, 50, 261, 19));
        groupBox = new QGroupBox(dlgOptions);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(300, 9, 291, 91));
        chbRegCert = new QCheckBox(groupBox);
        chbRegCert->setObjectName(QString::fromUtf8("chbRegCert"));
        chbRegCert->setGeometry(QRect(10, 20, 271, 19));
        chbRemoveCert = new QCheckBox(groupBox);
        chbRemoveCert->setObjectName(QString::fromUtf8("chbRemoveCert"));
        chbRemoveCert->setGeometry(QRect(10, 50, 271, 19));
        groupBox_3 = new QGroupBox(dlgOptions);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 230, 281, 101));
        chbUseKeyPad = new QCheckBox(groupBox_3);
        chbUseKeyPad->setObjectName(QString::fromUtf8("chbUseKeyPad"));
        chbUseKeyPad->setGeometry(QRect(10, 50, 261, 19));
        chbShowNotification = new QCheckBox(groupBox_3);
        chbShowNotification->setObjectName(QString::fromUtf8("chbShowNotification"));
        chbShowNotification->setGeometry(QRect(10, 20, 261, 19));
        groupBox_4 = new QGroupBox(dlgOptions);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 10, 281, 91));
        cmbCardReader = new QComboBox(groupBox_4);
        cmbCardReader->setObjectName(QString::fromUtf8("cmbCardReader"));
        cmbCardReader->setGeometry(QRect(10, 20, 251, 22));
        grpCertValidation = new QGroupBox(dlgOptions);
        grpCertValidation->setObjectName(QString::fromUtf8("grpCertValidation"));
        grpCertValidation->setGeometry(QRect(300, 110, 291, 221));
        grpOCSP = new QGroupBox(grpCertValidation);
        grpOCSP->setObjectName(QString::fromUtf8("grpOCSP"));
        grpOCSP->setGeometry(QRect(10, 20, 271, 91));
        rbOCSPNotUsed = new QRadioButton(grpOCSP);
        rbOCSPNotUsed->setObjectName(QString::fromUtf8("rbOCSPNotUsed"));
        rbOCSPNotUsed->setGeometry(QRect(10, 20, 231, 19));
        rbOCSPNotUsed->setChecked(true);
        rbOCSPOptional = new QRadioButton(grpOCSP);
        rbOCSPOptional->setObjectName(QString::fromUtf8("rbOCSPOptional"));
        rbOCSPOptional->setGeometry(QRect(10, 40, 231, 19));
        rbOCSPMandatory = new QRadioButton(grpOCSP);
        rbOCSPMandatory->setObjectName(QString::fromUtf8("rbOCSPMandatory"));
        rbOCSPMandatory->setGeometry(QRect(10, 60, 221, 19));
        grpCRL = new QGroupBox(grpCertValidation);
        grpCRL->setObjectName(QString::fromUtf8("grpCRL"));
        grpCRL->setGeometry(QRect(10, 120, 271, 91));
        rbCRLNotUsed = new QRadioButton(grpCRL);
        rbCRLNotUsed->setObjectName(QString::fromUtf8("rbCRLNotUsed"));
        rbCRLNotUsed->setGeometry(QRect(10, 20, 221, 19));
        rbCRLNotUsed->setChecked(true);
        rbCRLOptional = new QRadioButton(grpCRL);
        rbCRLOptional->setObjectName(QString::fromUtf8("rbCRLOptional"));
        rbCRLOptional->setGeometry(QRect(10, 40, 231, 19));
        rbCRLMandatory = new QRadioButton(grpCRL);
        rbCRLMandatory->setObjectName(QString::fromUtf8("rbCRLMandatory"));
        rbCRLMandatory->setGeometry(QRect(10, 60, 231, 19));
        okButton = new QPushButton(dlgOptions);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setGeometry(QRect(500, 350, 77, 26));
        chbShowPicture = new QCheckBox(dlgOptions);
        chbShowPicture->setObjectName(QString::fromUtf8("chbShowPicture"));
        chbShowPicture->setEnabled(false);
        chbShowPicture->setGeometry(QRect(10, 340, 261, 19));
        chbShowToolbar = new QCheckBox(dlgOptions);
        chbShowToolbar->setObjectName(QString::fromUtf8("chbShowToolbar"));
        chbShowToolbar->setEnabled(false);
        chbShowToolbar->setGeometry(QRect(10, 360, 261, 19));

        retranslateUi(dlgOptions);
        QObject::connect(okButton, SIGNAL(clicked()), dlgOptions, SLOT(accept()));

        QMetaObject::connectSlotsByName(dlgOptions);
    } // setupUi

    void retranslateUi(QDialog *dlgOptions)
    {
        dlgOptions->setWindowTitle(QApplication::translate("dlgOptions", "Options", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("dlgOptions", "Startup", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        chbAutoCardReading->setToolTip(QApplication::translate("dlgOptions", "determins whether the application should start reading the card from the moment it's detected in the cartdreader or not", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        chbAutoCardReading->setText(QApplication::translate("dlgOptions", "Automatic card reading", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        chbWinAutoStart->setStatusTip(QApplication::translate("dlgOptions", "Determins whether this application should start when windows starts or not", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        chbWinAutoStart->setText(QApplication::translate("dlgOptions", "Activate on Windows startup", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        chbStartMinimized->setToolTip(QApplication::translate("dlgOptions", "Determins whether the mainwindow should stay hidden on startup or not (Tray Icon will stay available)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        chbStartMinimized->setText(QApplication::translate("dlgOptions", "Start minimized", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("dlgOptions", "Certificates", 0, QApplication::UnicodeUTF8));
        chbRegCert->setText(QApplication::translate("dlgOptions", "Register certificates on insert", 0, QApplication::UnicodeUTF8));
        chbRemoveCert->setText(QApplication::translate("dlgOptions", "Remove certificates on close", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("dlgOptions", "Layout", 0, QApplication::UnicodeUTF8));
        chbUseKeyPad->setText(QApplication::translate("dlgOptions", "Use Virtual KeyPad", 0, QApplication::UnicodeUTF8));
        chbShowNotification->setText(QApplication::translate("dlgOptions", "Show notification", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("dlgOptions", "Card reader", 0, QApplication::UnicodeUTF8));
        grpCertValidation->setTitle(QApplication::translate("dlgOptions", "Certificate Validation", 0, QApplication::UnicodeUTF8));
        grpOCSP->setTitle(QApplication::translate("dlgOptions", "OCSP", 0, QApplication::UnicodeUTF8));
        rbOCSPNotUsed->setText(QApplication::translate("dlgOptions", "Not Used", 0, QApplication::UnicodeUTF8));
        rbOCSPOptional->setText(QApplication::translate("dlgOptions", "Optional", 0, QApplication::UnicodeUTF8));
        rbOCSPMandatory->setText(QApplication::translate("dlgOptions", "Mandatory", 0, QApplication::UnicodeUTF8));
        grpCRL->setTitle(QApplication::translate("dlgOptions", "CRL", 0, QApplication::UnicodeUTF8));
        rbCRLNotUsed->setText(QApplication::translate("dlgOptions", "Not Used", 0, QApplication::UnicodeUTF8));
        rbCRLOptional->setText(QApplication::translate("dlgOptions", "Optional", 0, QApplication::UnicodeUTF8));
        rbCRLMandatory->setText(QApplication::translate("dlgOptions", "Mandatory", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("dlgOptions", "OK", 0, QApplication::UnicodeUTF8));
        chbShowPicture->setText(QApplication::translate("dlgOptions", "Show picture", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        chbShowToolbar->setToolTip(QApplication::translate("dlgOptions", "Determins whether the toolbar gets shown or not", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        chbShowToolbar->setText(QApplication::translate("dlgOptions", "Show toolbar", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(dlgOptions);
    } // retranslateUi

};

namespace Ui {
    class dlgOptions: public Ui_dlgOptions {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGOPTIONS_H
