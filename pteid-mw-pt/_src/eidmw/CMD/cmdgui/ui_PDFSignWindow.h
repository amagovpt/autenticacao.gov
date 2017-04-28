/********************************************************************************
** Form generated from reading UI file 'PDFSignWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PDFSIGNWINDOW_H
#define UI_PDFSIGNWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "mylistview.h"

QT_BEGIN_NAMESPACE

class Ui_PDFSignWindow
{
public:
    QWidget *centralwidget;
    QPushButton *button_addfile;
    MyListView *pdf_listview;
    QLabel *label_3;
    QWidget *layoutWidget;
    QVBoxLayout *vertical_top;
    QFormLayout *formLayout;
    QCheckBox *checkBox_reason;
    QLineEdit *reason_textbox;
    QCheckBox *checkBox_location;
    QLineEdit *location_textbox;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *timestamp_checkBox;
    QCheckBox *visible_checkBox;
    QCheckBox *smallsig_checkBox;
    QRadioButton *radioButton_firstpage;
    QFormLayout *formLayout_3;
    QRadioButton *radioButton_choosepage;
    QSpinBox *spinBox_page;
    QRadioButton *radioButton_lastpage;
    QPushButton *button_sign;
    QPushButton *button_cancel;
    QPushButton *button_clearFiles;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *vertical_layout_tab1;
    QTabWidget *tabWidget;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout1;
    QHBoxLayout *horizontalLayout_tab1;
    QGraphicsView *scene_view;
    QWidget *tab3;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_signature_preview;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_imgChooser;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QPushButton *pushButton_resetImage;

    void setupUi(QDialog *PDFSignWindow)
    {
        if (PDFSignWindow->objectName().isEmpty())
            PDFSignWindow->setObjectName(QStringLiteral("PDFSignWindow"));
        PDFSignWindow->resize(995, 585);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PDFSignWindow->sizePolicy().hasHeightForWidth());
        PDFSignWindow->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(PDFSignWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        centralwidget->setGeometry(QRect(10, 0, 911, 585));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        button_addfile = new QPushButton(centralwidget);
        button_addfile->setObjectName(QStringLiteral("button_addfile"));
        button_addfile->setGeometry(QRect(210, 90, 100, 25));
        QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(button_addfile->sizePolicy().hasHeightForWidth());
        button_addfile->setSizePolicy(sizePolicy2);
        pdf_listview = new MyListView(centralwidget);
        pdf_listview->setObjectName(QStringLiteral("pdf_listview"));
        pdf_listview->setGeometry(QRect(10, 30, 381, 41));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 10, 341, 16));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 117, 381, 261));
        vertical_top = new QVBoxLayout(layoutWidget);
        vertical_top->setObjectName(QStringLiteral("vertical_top"));
        vertical_top->setContentsMargins(0, 0, 0, 0);
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        formLayout->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        checkBox_reason = new QCheckBox(layoutWidget);
        checkBox_reason->setObjectName(QStringLiteral("checkBox_reason"));
        checkBox_reason->setEnabled(false);

        formLayout->setWidget(0, QFormLayout::LabelRole, checkBox_reason);

        reason_textbox = new QLineEdit(layoutWidget);
        reason_textbox->setObjectName(QStringLiteral("reason_textbox"));
        reason_textbox->setEnabled(false);

        formLayout->setWidget(0, QFormLayout::FieldRole, reason_textbox);

        checkBox_location = new QCheckBox(layoutWidget);
        checkBox_location->setObjectName(QStringLiteral("checkBox_location"));
        checkBox_location->setEnabled(false);

        formLayout->setWidget(1, QFormLayout::LabelRole, checkBox_location);

        location_textbox = new QLineEdit(layoutWidget);
        location_textbox->setObjectName(QStringLiteral("location_textbox"));
        location_textbox->setEnabled(false);

        formLayout->setWidget(1, QFormLayout::FieldRole, location_textbox);


        vertical_top->addLayout(formLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        timestamp_checkBox = new QCheckBox(layoutWidget);
        timestamp_checkBox->setObjectName(QStringLiteral("timestamp_checkBox"));
        timestamp_checkBox->setEnabled(false);

        verticalLayout_2->addWidget(timestamp_checkBox);

        visible_checkBox = new QCheckBox(layoutWidget);
        visible_checkBox->setObjectName(QStringLiteral("visible_checkBox"));
        visible_checkBox->setEnabled(false);

        verticalLayout_2->addWidget(visible_checkBox);

        smallsig_checkBox = new QCheckBox(layoutWidget);
        smallsig_checkBox->setObjectName(QStringLiteral("smallsig_checkBox"));
        smallsig_checkBox->setEnabled(false);

        verticalLayout_2->addWidget(smallsig_checkBox);

        radioButton_firstpage = new QRadioButton(layoutWidget);
        radioButton_firstpage->setObjectName(QStringLiteral("radioButton_firstpage"));
        radioButton_firstpage->setEnabled(false);

        verticalLayout_2->addWidget(radioButton_firstpage);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        formLayout_3->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout_3->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        formLayout_3->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        radioButton_choosepage = new QRadioButton(layoutWidget);
        radioButton_choosepage->setObjectName(QStringLiteral("radioButton_choosepage"));
        radioButton_choosepage->setEnabled(false);
        QSizePolicy sizePolicy3(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(radioButton_choosepage->sizePolicy().hasHeightForWidth());
        radioButton_choosepage->setSizePolicy(sizePolicy3);

        formLayout_3->setWidget(0, QFormLayout::LabelRole, radioButton_choosepage);

        spinBox_page = new QSpinBox(layoutWidget);
        spinBox_page->setObjectName(QStringLiteral("spinBox_page"));
        spinBox_page->setEnabled(false);
        sizePolicy3.setHeightForWidth(spinBox_page->sizePolicy().hasHeightForWidth());
        spinBox_page->setSizePolicy(sizePolicy3);
        spinBox_page->setMinimum(1);
        spinBox_page->setMaximum(999999);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, spinBox_page);


        verticalLayout_2->addLayout(formLayout_3);

        radioButton_lastpage = new QRadioButton(layoutWidget);
        radioButton_lastpage->setObjectName(QStringLiteral("radioButton_lastpage"));
        radioButton_lastpage->setEnabled(false);

        verticalLayout_2->addWidget(radioButton_lastpage);


        vertical_top->addLayout(verticalLayout_2);

        button_sign = new QPushButton(centralwidget);
        button_sign->setObjectName(QStringLiteral("button_sign"));
        button_sign->setEnabled(false);
        button_sign->setGeometry(QRect(210, 420, 100, 25));
        button_sign->setStyleSheet(QStringLiteral("font: bold;"));
        button_cancel = new QPushButton(centralwidget);
        button_cancel->setObjectName(QStringLiteral("button_cancel"));
        button_cancel->setGeometry(QRect(90, 420, 100, 25));
        button_clearFiles = new QPushButton(centralwidget);
        button_clearFiles->setObjectName(QStringLiteral("button_clearFiles"));
        button_clearFiles->setGeometry(QRect(90, 90, 100, 25));
        verticalLayoutWidget = new QWidget(PDFSignWindow);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(440, 10, 546, 562));
        vertical_layout_tab1 = new QVBoxLayout(verticalLayoutWidget);
        vertical_layout_tab1->setObjectName(QStringLiteral("vertical_layout_tab1"));
        vertical_layout_tab1->setSizeConstraint(QLayout::SetMinimumSize);
        vertical_layout_tab1->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(verticalLayoutWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy4);
        tabWidget->setMinimumSize(QSize(0, 560));
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout1 = new QVBoxLayout(tab_2);
        verticalLayout1->setObjectName(QStringLiteral("verticalLayout1"));
        horizontalLayout_tab1 = new QHBoxLayout();
        horizontalLayout_tab1->setObjectName(QStringLiteral("horizontalLayout_tab1"));
        scene_view = new QGraphicsView(tab_2);
        scene_view->setObjectName(QStringLiteral("scene_view"));
        QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(scene_view->sizePolicy().hasHeightForWidth());
        scene_view->setSizePolicy(sizePolicy5);
        scene_view->setMinimumSize(QSize(520, 530));

        horizontalLayout_tab1->addWidget(scene_view);


        verticalLayout1->addLayout(horizontalLayout_tab1);

        tabWidget->addTab(tab_2, QString());
        tab3 = new QWidget();
        tab3->setObjectName(QStringLiteral("tab3"));
        verticalLayoutWidget_3 = new QWidget(tab3);
        verticalLayoutWidget_3->setObjectName(QStringLiteral("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(10, 10, 535, 321));
        verticalLayout_4 = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_signature_preview = new QLabel(verticalLayoutWidget_3);
        label_signature_preview->setObjectName(QStringLiteral("label_signature_preview"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(label_signature_preview->sizePolicy().hasHeightForWidth());
        label_signature_preview->setSizePolicy(sizePolicy6);
        label_signature_preview->setAlignment(Qt::AlignLeading);

        verticalLayout_4->addWidget(label_signature_preview);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_imgChooser = new QPushButton(verticalLayoutWidget_3);
        pushButton_imgChooser->setObjectName(QStringLiteral("pushButton_imgChooser"));
        pushButton_imgChooser->setEnabled(false);
        sizePolicy.setHeightForWidth(pushButton_imgChooser->sizePolicy().hasHeightForWidth());
        pushButton_imgChooser->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pushButton_imgChooser);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(verticalLayoutWidget_3);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);


        horizontalLayout->addLayout(horizontalLayout_2);


        verticalLayout_4->addLayout(horizontalLayout);

        pushButton_resetImage = new QPushButton(verticalLayoutWidget_3);
        pushButton_resetImage->setObjectName(QStringLiteral("pushButton_resetImage"));
        pushButton_resetImage->setEnabled(false);
        sizePolicy.setHeightForWidth(pushButton_resetImage->sizePolicy().hasHeightForWidth());
        pushButton_resetImage->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(pushButton_resetImage);

        tabWidget->addTab(tab3, QString());

        vertical_layout_tab1->addWidget(tabWidget);


        retranslateUi(PDFSignWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PDFSignWindow);
    } // setupUi

    void retranslateUi(QDialog *PDFSignWindow)
    {
        PDFSignWindow->setWindowTitle(QApplication::translate("PDFSignWindow", "Chave M\303\263vel Digital - PDF Signature", 0));
        button_addfile->setText(QApplication::translate("PDFSignWindow", "Add File...", 0));
        label_3->setText(QApplication::translate("PDFSignWindow", "PDF Files to sign", 0));
        checkBox_reason->setText(QApplication::translate("PDFSignWindow", "Reason:", 0));
        checkBox_location->setText(QApplication::translate("PDFSignWindow", "Location:", 0));
        timestamp_checkBox->setText(QApplication::translate("PDFSignWindow", "Add Timestamp", 0));
        visible_checkBox->setText(QApplication::translate("PDFSignWindow", "Visible Signature?", 0));
        smallsig_checkBox->setText(QApplication::translate("PDFSignWindow", "Small signature?", 0));
        radioButton_firstpage->setText(QApplication::translate("PDFSignWindow", "First Page", 0));
        radioButton_choosepage->setText(QApplication::translate("PDFSignWindow", "Choose Page...", 0));
        radioButton_lastpage->setText(QApplication::translate("PDFSignWindow", "Last Page", 0));
        button_sign->setText(QApplication::translate("PDFSignWindow", "Sign", 0));
        button_cancel->setText(QApplication::translate("PDFSignWindow", "Cancel", 0));
        button_clearFiles->setText(QApplication::translate("PDFSignWindow", "Remove file", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("PDFSignWindow", "Location", 0));
        label_signature_preview->setText(QApplication::translate("PDFSignWindow", "Preview of the signature image", 0));
        pushButton_imgChooser->setText(QApplication::translate("PDFSignWindow", "Choose custom image", 0));
        label->setText(QApplication::translate("PDFSignWindow", "Maximum image dimension: 185x41 px", 0));
        pushButton_resetImage->setText(QApplication::translate("PDFSignWindow", "Reset image", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab3), QApplication::translate("PDFSignWindow", "Signature preview", 0));
    } // retranslateUi

};

namespace Ui {
    class PDFSignWindow: public Ui_PDFSignWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PDFSIGNWINDOW_H
