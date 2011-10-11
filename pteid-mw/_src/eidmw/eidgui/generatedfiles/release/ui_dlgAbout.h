/********************************************************************************
** Form generated from reading ui file 'dlgAbout.ui'
**
** Created: Sun 31. Jul 19:32:55 2011
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DLGABOUT_H
#define UI_DLGABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dlgAbout
{
public:
    QLabel *lblIcon;
    QWidget *layoutWidget;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *btnOK;
    QSpacerItem *spacerItem1;
    QLabel *lblVersion;
    QLabel *label;
    QLabel *label_2;
    QLabel *lblInfoTop;

    void setupUi(QDialog *dlgAbout)
    {
        if (dlgAbout->objectName().isEmpty())
            dlgAbout->setObjectName(QString::fromUtf8("dlgAbout"));
        dlgAbout->setWindowModality(Qt::WindowModal);
        dlgAbout->resize(500, 140);
        dlgAbout->setMaximumSize(QSize(500, 140));
        dlgAbout->setModal(true);
        lblIcon = new QLabel(dlgAbout);
        lblIcon->setObjectName(QString::fromUtf8("lblIcon"));
        lblIcon->setGeometry(QRect(20, 10, 113, 79));
        lblIcon->setAutoFillBackground(false);
        lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/images/Images/a-z_identiteitskaart_logo.png")));
        layoutWidget = new QWidget(dlgAbout);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 100, 483, 27));
        hboxLayout = new QHBoxLayout(layoutWidget);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        spacerItem = new QSpacerItem(211, 25, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        btnOK = new QPushButton(layoutWidget);
        btnOK->setObjectName(QString::fromUtf8("btnOK"));

        hboxLayout->addWidget(btnOK);

        spacerItem1 = new QSpacerItem(181, 25, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);

        lblVersion = new QLabel(dlgAbout);
        lblVersion->setObjectName(QString::fromUtf8("lblVersion"));
        lblVersion->setGeometry(QRect(340, 10, 121, 22));
        label = new QLabel(dlgAbout);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(141, 39, 349, 23));
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(dlgAbout);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(141, 68, 349, 22));
        label_2->setAlignment(Qt::AlignCenter);
        lblInfoTop = new QLabel(dlgAbout);
        lblInfoTop->setObjectName(QString::fromUtf8("lblInfoTop"));
        lblInfoTop->setGeometry(QRect(150, 10, 181, 22));
        lblInfoTop->setMaximumSize(QSize(200, 100));
        lblInfoTop->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        retranslateUi(dlgAbout);
        QObject::connect(btnOK, SIGNAL(clicked()), dlgAbout, SLOT(accept()));

        QMetaObject::connectSlotsByName(dlgAbout);
    } // setupUi

    void retranslateUi(QDialog *dlgAbout)
    {
        dlgAbout->setWindowTitle(QApplication::translate("dlgAbout", "Identity Card: About", 0, QApplication::UnicodeUTF8));
        lblIcon->setText(QString());
        btnOK->setText(QApplication::translate("dlgAbout", "&OK", 0, QApplication::UnicodeUTF8));
        lblVersion->setText(QString());
        label->setText(QApplication::translate("dlgAbout", "Copyright \302\251 2009-2011", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("dlgAbout", "Caixa M\303\241gica Software", 0, QApplication::UnicodeUTF8));
        lblInfoTop->setText(QApplication::translate("dlgAbout", "Portuguese eID Runtime", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(dlgAbout);
    } // retranslateUi

};

namespace Ui {
    class dlgAbout: public Ui_dlgAbout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGABOUT_H
