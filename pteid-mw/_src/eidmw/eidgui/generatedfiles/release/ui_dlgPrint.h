/********************************************************************************
** Form generated from reading ui file 'dlgPrint.ui'
**
** Created: Sun 31. Jul 19:32:55 2011
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DLGPRINT_H
#define UI_DLGPRINT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_dlgPrint
{
public:
    QGridLayout *gridLayout;
    QGridLayout *gridLayout1;
    QTextEdit *paperview;
    QSpacerItem *spacerItem;
    QPushButton *pbCancel;
    QPushButton *pbPrint;

    void setupUi(QDialog *dlgPrint)
    {
        if (dlgPrint->objectName().isEmpty())
            dlgPrint->setObjectName(QString::fromUtf8("dlgPrint"));
        dlgPrint->resize(645, 743);
        QFont font;
        font.setPointSize(11);
        dlgPrint->setFont(font);
        const QIcon icon = QIcon(QString::fromUtf8("Images/Icons/Print.png"));
        dlgPrint->setWindowIcon(icon);
        dlgPrint->setModal(true);
        gridLayout = new QGridLayout(dlgPrint);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout1 = new QGridLayout();
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        paperview = new QTextEdit(dlgPrint);
        paperview->setObjectName(QString::fromUtf8("paperview"));
        paperview->setFont(font);
        paperview->setReadOnly(true);

        gridLayout1->addWidget(paperview, 0, 0, 1, 3);

        spacerItem = new QSpacerItem(451, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem, 1, 0, 1, 1);

        pbCancel = new QPushButton(dlgPrint);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));

        gridLayout1->addWidget(pbCancel, 1, 1, 1, 1);

        pbPrint = new QPushButton(dlgPrint);
        pbPrint->setObjectName(QString::fromUtf8("pbPrint"));

        gridLayout1->addWidget(pbPrint, 1, 2, 1, 1);


        gridLayout->addLayout(gridLayout1, 0, 0, 1, 1);


        retranslateUi(dlgPrint);

        QMetaObject::connectSlotsByName(dlgPrint);
    } // setupUi

    void retranslateUi(QDialog *dlgPrint)
    {
        dlgPrint->setWindowTitle(QApplication::translate("dlgPrint", "Print", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("dlgPrint", "Cancel", 0, QApplication::UnicodeUTF8));
        pbPrint->setText(QApplication::translate("dlgPrint", "Print", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(dlgPrint);
    } // retranslateUi

};

namespace Ui {
    class dlgPrint: public Ui_dlgPrint {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGPRINT_H
