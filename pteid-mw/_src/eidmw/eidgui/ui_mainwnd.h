/********************************************************************************
** Form generated from reading UI file 'mainwnd.ui'
**
** Created: Fri Oct 14 14:58:09 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWND_H
#define UI_MAINWND_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWnd
{
public:
    QAction *actionE_xit;
    QAction *actionReload_eID;
    QAction *actionSave_eID;
    QAction *actionSave_eID_as;
    QAction *actionOpen_eID;
    QAction *actionClear;
    QAction *actionPrint_eID;
    QAction *actionPrinter_Settings;
    QAction *actionOptions;
    QAction *actionEnglish;
    QAction *actionNederlands;
    QAction *actionHelp;
    QAction *actionAbout;
    QAction *actionShow_Toolbar;
    QAction *actionPINRequest;
    QAction *actionPINChange;
    QAction *actionPINRenew;
    QAction *actionZoom_In;
    QAction *actionZoom_Out;
    QAction *actionDisplay_Pinpad_Info;
    QAction *actionClose_Pinpad_Info;
    QAction *actionIdentity_Card;
    QAction *actionSIS_Card;
    QAction *actionKids_Card;
    QAction *actionForeigner_Card;
    QAction *actionSave_PersoData;
    QWidget *centralwidget;
    QWidget *widget;
    QWidget *widget_3;
    QStackedWidget *stackedWidget;
    QWidget *page_Wellcome;
    QRadioButton *radioButton;
    QWidget *page_Identity;
    QLineEdit *txtIdentity_GivenNames;
    QLineEdit *txtIdentity_Name;
    QLineEdit *txtIdentity_Sex;
    QLineEdit *txtIdentity_Height;
    QLineEdit *txtIdentity_Country;
    QLineEdit *txtIdentity_Nationality;
    QLineEdit *txtIdentity_BirthDate;
    QLabel *lblIdentity_ImgPerson;
    QLineEdit *txtIdentity_Parents;
    QLineEdit *txtIdentity_DocumentNumber;
    QLineEdit *txtIdentity_ValidFrom_Until;
    QWidget *page_IdentityExtra;
    QLineEdit *txtIdentityExtra_SocialSecurityNo;
    QLineEdit *txtIdentityExtra_TaxNo;
    QLineEdit *txtIdentityExtra_CardVersion;
    QLineEdit *txtIdentityExtra_DocumentType;
    QLineEdit *txtIdentityExtra_LocalofRequest;
    QLineEdit *txtIdentityExtra_HealthNo;
    QLineEdit *txtIdentityExtra_IssuingEntity;
    QLineEdit *txtIdentityExtra_Validate;
    QLineEdit *txtSpecialStatus;
    QLineEdit *txtIdentityExtra_ChipNumber;
    QLabel *lblChip_Number;
    QLineEdit *txtIdentityExtra_Card_Number;
    QLineEdit *txtIdentityExtra_ValidFrom_Until;
    QWidget *page_Address;
    QLabel *lblAddress_District;
    QLineEdit *txtAddress_District;
    QLabel *lblAddress_Place;
    QLineEdit *txtAddress_Place;
    QLabel *lblAddress_Municipality;
    QLineEdit *txtAddress_Municipality;
    QLabel *lblAddress_Zip3;
    QLineEdit *txtAddress_Zip3;
    QLabel *lblAddress_PostalLocality;
    QLineEdit *txtAddress_PostalLocality;
    QLabel *lblAddress_CivilParish;
    QLineEdit *txtAddress_CivilParish;
    QLabel *lblAddress_StreetType1;
    QLineEdit *txtAddress_StreetType1;
    QLabel *lblAddress_StreetType2;
    QLineEdit *txtAddress_StreetType2;
    QLabel *lblAddress_StreetName;
    QLineEdit *txtAddress_StreetName;
    QLabel *lblAddress_BuildingType1;
    QLineEdit *txtAddress_BuildingType1;
    QLabel *lblAddress_BuildingType2;
    QLineEdit *txtAddress_BuildingType2;
    QLabel *lblAddress_DoorNo;
    QLineEdit *txtAddress_DoorNo;
    QLabel *lblAddress_Floor;
    QLineEdit *txtAddress_Floor;
    QLabel *lblAddress_Side;
    QLineEdit *txtAddress_Side;
    QLabel *lblAddress_Locality;
    QLineEdit *txtAddress_Locality;
    QLabel *lblAddress_Zip4;
    QLineEdit *txtAddress_Zip4;
    QWidget *page_Certificates;
    QTreeWidget *treeCert;
    QLabel *lblCert_Owner;
    QLineEdit *txtCert_Owner;
    QLabel *lblCert_Issuer;
    QLineEdit *txtCert_Issuer;
    QLabel *lblCert_KeyLenght;
    QLineEdit *txtCert_KeyLenght;
    QLabel *lblCert_ValidFrom;
    QLineEdit *txtCert_ValidFrom;
    QLabel *lblCert_ValidUntil;
    QLineEdit *txtCert_ValidUntil;
    QLabel *lblCert_Status;
    QLineEdit *txtCert_Status;
    QPushButton *btnOCSPCheck;
    QTabWidget *tabWidget_Identity;
    QWidget *tabIdentity;
    QLabel *lblIdentity_Head2_4;
    QLabel *lblIdentity_Head1_3;
    QLabel *lblIdentity_Head1_4;
    QLabel *lblIdentity_Head2_3;
    QWidget *tabForeigners;
    QLineEdit *txtForeigners_Card_Number;
    QLabel *label;
    QLineEdit *txtForeigners_Name;
    QLineEdit *txtForeigners_GivenNames;
    QLabel *label_2;
    QLineEdit *txtForeigners_ValidTot;
    QLabel *label_3;
    QLineEdit *txtForeigners_PlaceOfIssue;
    QLabel *label_4;
    QLabel *lblForeigners_ImgPerson;
    QLineEdit *txtForeigners_CardType;
    QLabel *lblForeigners_Footer_1;
    QLabel *lblForeigners_Footer_2;
    QWidget *tabSis;
    QLineEdit *txtSis_SocialSecurityNumber;
    QLineEdit *txtSis_Name;
    QLineEdit *txtSis_GivenNames;
    QLineEdit *txtSis_BirthDate;
    QLabel *lblSis_Sex;
    QLineEdit *txtSis_LogicalNumber;
    QLineEdit *txtSis_ValidFrom;
    QWidget *tabIdentity_extention;
    QWidget *tabAddress;
    QWidget *tabForeigners_Back;
    QLineEdit *txtForeignersExtra_Nationality;
    QLineEdit *txtForeignersExtra_Sex;
    QLineEdit *txtForeignersExtra_Remarks1;
    QLineEdit *txtForeignersExtra_Remarks2;
    QLineEdit *txtForeignersExtra_ChipNumber;
    QLabel *lblForeignersExtra_Nationality;
    QLabel *lblForeignersExtra_Sex;
    QLabel *lblForeignersExtra_Remarks1;
    QLabel *lblForeignersExtra_ChipNumber;
    QLineEdit *txtForeignersExtra_BirthDate;
    QLabel *lblForeignersExtra_NationalNumber;
    QLineEdit *txtForeignersExtra_NationalNumber;
    QLabel *lblForeignersExtra_BirthDate;
    QLineEdit *txtForeignersExtra_Remarks3;
    QLineEdit *txtForeignerSpecialStatus;
    QLabel *lblForeignersExtra_street;
    QLineEdit *txtForeignersExtra_Adress_Street;
    QLabel *lblForeignersExtra_PostalCode;
    QLineEdit *txtForeignersExtra_Adress_PostalCode;
    QLineEdit *txtForeignersExtra_Adress_Muncipality;
    QLabel *lblForeignersExtra_municipality;
    QLineEdit *txtForeignersExtra_Adress_Country;
    QLabel *lblForeignersExtra_country;
    QWidget *tabSis_Back;
    QLabel *lblSisExtra_Card_ValidFrom_Until;
    QLineEdit *txtSisExtra_ValidFrom_Until;
    QWidget *tabCardPin;
    QGridLayout *_2;
    QGroupBox *grbPIN_PINInfo;
    QGridLayout *_4;
    QVBoxLayout *_5;
    QHBoxLayout *_6;
    QHBoxLayout *_7;
    QSpacerItem *spacerItem;
    QHBoxLayout *_8;
    QSpacerItem *spacerItem1;
    QWidget *tabCertificates;
    QGridLayout *_9;
    QHBoxLayout *_11;
    QSpacerItem *spacerItem2;
    QWidget *tabPersoData;
    QWidget *tabInfo;
    QGridLayout *_12;
    QLabel *lblInfo_Img1;
    QLabel *lblInfo_Top;
    QTableWidget *tblInfo;
    QLabel *lblInfo_bottom;
    QWidget *page_PINCodes;
    QTableWidget *tblCardAndPIN;
    QGroupBox *grbPIN_VersionInfo;
    QGridLayout *_3;
    QLabel *lblPIN_Name;
    QLineEdit *txtPIN_Name;
    QLabel *lblPIN_ID;
    QLineEdit *txtPIN_ID;
    QLabel *lblPIN_Status;
    QLineEdit *txtPIN_Status;
    QWidget *layoutWidget;
    QGridLayout *_10;
    QSpacerItem *spacerItem3;
    QLabel *lblCert_InfoAdd;
    QTreeWidget *treePIN;
    QLabel *lblPIN_InfoAdd;
    QPushButton *btnPIN_Test;
    QPushButton *btnPIN_Change;
    QPushButton *btnCert_Register;
    QPushButton *btnCert_Details;
    QWidget *page_Notes;
    QPlainTextEdit *txtPersoData;
    QPushButton *btnPersoDataSave;
    QWidget *widget_2;
    QPushButton *btnSelectTab_Identity;
    QPushButton *btnSelectTab_Identity_Extra;
    QPushButton *btnSelectTab_Address;
    QPushButton *btnSelectTab_Certificates;
    QPushButton *btnSelectTab_PinCodes;
    QPushButton *btnSelectTab_Notes;
    QWidget *widget_Toolbar;
    QPushButton *button_menu_card;
    QPushButton *button_menu_tools;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menu_Tools;
    QMenu *menuLanguage;
    QMenu *menuHelp;
    QToolBar *toolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWnd)
    {
        if (MainWnd->objectName().isEmpty())
            MainWnd->setObjectName(QString::fromUtf8("MainWnd"));
        MainWnd->setWindowModality(Qt::NonModal);
        MainWnd->resize(1300, 1000);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWnd->sizePolicy().hasHeightForWidth());
        MainWnd->setSizePolicy(sizePolicy);
        MainWnd->setAutoFillBackground(false);
        actionE_xit = new QAction(MainWnd);
        actionE_xit->setObjectName(QString::fromUtf8("actionE_xit"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/Images/Icons/Exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionE_xit->setIcon(icon);
        actionReload_eID = new QAction(MainWnd);
        actionReload_eID->setObjectName(QString::fromUtf8("actionReload_eID"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/Images/Icons/Chip.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReload_eID->setIcon(icon1);
        actionSave_eID = new QAction(MainWnd);
        actionSave_eID->setObjectName(QString::fromUtf8("actionSave_eID"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/Images/Icons/Save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_eID->setIcon(icon2);
        actionSave_eID_as = new QAction(MainWnd);
        actionSave_eID_as->setObjectName(QString::fromUtf8("actionSave_eID_as"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/Images/Icons/save_as.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_eID_as->setIcon(icon3);
        actionOpen_eID = new QAction(MainWnd);
        actionOpen_eID->setObjectName(QString::fromUtf8("actionOpen_eID"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/Images/Icons/Open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_eID->setIcon(icon4);
        actionClear = new QAction(MainWnd);
        actionClear->setObjectName(QString::fromUtf8("actionClear"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/images/Images/Icons/Clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClear->setIcon(icon5);
        actionPrint_eID = new QAction(MainWnd);
        actionPrint_eID->setObjectName(QString::fromUtf8("actionPrint_eID"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/images/Images/Icons/Print.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrint_eID->setIcon(icon6);
        actionPrinter_Settings = new QAction(MainWnd);
        actionPrinter_Settings->setObjectName(QString::fromUtf8("actionPrinter_Settings"));
        actionPrinter_Settings->setVisible(false);
        actionOptions = new QAction(MainWnd);
        actionOptions->setObjectName(QString::fromUtf8("actionOptions"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/images/Images/Icons/Options.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOptions->setIcon(icon7);
        actionEnglish = new QAction(MainWnd);
        actionEnglish->setObjectName(QString::fromUtf8("actionEnglish"));
        actionEnglish->setCheckable(true);
        actionNederlands = new QAction(MainWnd);
        actionNederlands->setObjectName(QString::fromUtf8("actionNederlands"));
        actionNederlands->setCheckable(true);
        actionHelp = new QAction(MainWnd);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionAbout = new QAction(MainWnd);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/images/Images/Icons/Help.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon8);
        actionShow_Toolbar = new QAction(MainWnd);
        actionShow_Toolbar->setObjectName(QString::fromUtf8("actionShow_Toolbar"));
        actionShow_Toolbar->setCheckable(true);
        actionShow_Toolbar->setChecked(true);
        actionPINRequest = new QAction(MainWnd);
        actionPINRequest->setObjectName(QString::fromUtf8("actionPINRequest"));
        actionPINChange = new QAction(MainWnd);
        actionPINChange->setObjectName(QString::fromUtf8("actionPINChange"));
        actionPINRenew = new QAction(MainWnd);
        actionPINRenew->setObjectName(QString::fromUtf8("actionPINRenew"));
        actionPINRenew->setEnabled(true);
        actionZoom_In = new QAction(MainWnd);
        actionZoom_In->setObjectName(QString::fromUtf8("actionZoom_In"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/images/Images/Icons/Zoom_IN.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_In->setIcon(icon9);
        actionZoom_In->setVisible(true);
        actionZoom_Out = new QAction(MainWnd);
        actionZoom_Out->setObjectName(QString::fromUtf8("actionZoom_Out"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/images/Images/Icons/Zoom_OUT.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZoom_Out->setIcon(icon10);
        actionZoom_Out->setVisible(true);
        actionDisplay_Pinpad_Info = new QAction(MainWnd);
        actionDisplay_Pinpad_Info->setObjectName(QString::fromUtf8("actionDisplay_Pinpad_Info"));
        actionClose_Pinpad_Info = new QAction(MainWnd);
        actionClose_Pinpad_Info->setObjectName(QString::fromUtf8("actionClose_Pinpad_Info"));
        actionIdentity_Card = new QAction(MainWnd);
        actionIdentity_Card->setObjectName(QString::fromUtf8("actionIdentity_Card"));
        actionSIS_Card = new QAction(MainWnd);
        actionSIS_Card->setObjectName(QString::fromUtf8("actionSIS_Card"));
        actionKids_Card = new QAction(MainWnd);
        actionKids_Card->setObjectName(QString::fromUtf8("actionKids_Card"));
        actionForeigner_Card = new QAction(MainWnd);
        actionForeigner_Card->setObjectName(QString::fromUtf8("actionForeigner_Card"));
        actionSave_PersoData = new QAction(MainWnd);
        actionSave_PersoData->setObjectName(QString::fromUtf8("actionSave_PersoData"));
        centralwidget = new QWidget(MainWnd);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(0, 120, 1282, 761));
        widget_3 = new QWidget(widget);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        widget_3->setGeometry(QRect(10, 10, 1241, 821));
        stackedWidget = new QStackedWidget(widget_3);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(0, 0, 830, 671));
        stackedWidget->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/additional_data.png);"));
        page_Wellcome = new QWidget();
        page_Wellcome->setObjectName(QString::fromUtf8("page_Wellcome"));
        radioButton = new QRadioButton(page_Wellcome);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setGeometry(QRect(340, 90, 116, 22));
        stackedWidget->addWidget(page_Wellcome);
        page_Identity = new QWidget();
        page_Identity->setObjectName(QString::fromUtf8("page_Identity"));
        page_Identity->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/Page_background_Identity.png);"));
        txtIdentity_GivenNames = new QLineEdit(page_Identity);
        txtIdentity_GivenNames->setObjectName(QString::fromUtf8("txtIdentity_GivenNames"));
        txtIdentity_GivenNames->setGeometry(QRect(23, 88, 376, 20));
        QFont font;
        txtIdentity_GivenNames->setFont(font);
        txtIdentity_GivenNames->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_GivenNames->setFrame(false);
        txtIdentity_GivenNames->setAlignment(Qt::AlignLeading);
        txtIdentity_GivenNames->setReadOnly(true);
        txtIdentity_Name = new QLineEdit(page_Identity);
        txtIdentity_Name->setObjectName(QString::fromUtf8("txtIdentity_Name"));
        txtIdentity_Name->setGeometry(QRect(23, 40, 197, 20));
        txtIdentity_Name->setFont(font);
        txtIdentity_Name->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_Name->setFrame(false);
        txtIdentity_Name->setReadOnly(true);
        txtIdentity_Sex = new QLineEdit(page_Identity);
        txtIdentity_Sex->setObjectName(QString::fromUtf8("txtIdentity_Sex"));
        txtIdentity_Sex->setGeometry(QRect(23, 147, 64, 20));
        txtIdentity_Sex->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_Sex->setFrame(false);
        txtIdentity_Sex->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        txtIdentity_Sex->setReadOnly(true);
        txtIdentity_Height = new QLineEdit(page_Identity);
        txtIdentity_Height->setObjectName(QString::fromUtf8("txtIdentity_Height"));
        txtIdentity_Height->setGeometry(QRect(110, 147, 53, 20));
        txtIdentity_Height->setFont(font);
        txtIdentity_Height->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_Height->setFrame(false);
        txtIdentity_Height->setReadOnly(true);
        txtIdentity_Country = new QLineEdit(page_Identity);
        txtIdentity_Country->setObjectName(QString::fromUtf8("txtIdentity_Country"));
        txtIdentity_Country->setGeometry(QRect(23, 264, 195, 20));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(txtIdentity_Country->sizePolicy().hasHeightForWidth());
        txtIdentity_Country->setSizePolicy(sizePolicy1);
        txtIdentity_Country->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_Country->setFrame(false);
        txtIdentity_Country->setReadOnly(true);
        txtIdentity_Nationality = new QLineEdit(page_Identity);
        txtIdentity_Nationality->setObjectName(QString::fromUtf8("txtIdentity_Nationality"));
        txtIdentity_Nationality->setGeometry(QRect(185, 147, 90, 20));
        txtIdentity_Nationality->setFont(font);
        txtIdentity_Nationality->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_Nationality->setFrame(false);
        txtIdentity_Nationality->setReadOnly(true);
        txtIdentity_BirthDate = new QLineEdit(page_Identity);
        txtIdentity_BirthDate->setObjectName(QString::fromUtf8("txtIdentity_BirthDate"));
        txtIdentity_BirthDate->setGeometry(QRect(298, 147, 101, 20));
        txtIdentity_BirthDate->setFont(font);
        txtIdentity_BirthDate->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_BirthDate->setFrame(false);
        txtIdentity_BirthDate->setReadOnly(true);
        lblIdentity_ImgPerson = new QLabel(page_Identity);
        lblIdentity_ImgPerson->setObjectName(QString::fromUtf8("lblIdentity_ImgPerson"));
        lblIdentity_ImgPerson->setEnabled(true);
        lblIdentity_ImgPerson->setGeometry(QRect(420, 0, 131, 121));
        QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lblIdentity_ImgPerson->sizePolicy().hasHeightForWidth());
        lblIdentity_ImgPerson->setSizePolicy(sizePolicy2);
        lblIdentity_ImgPerson->setMinimumSize(QSize(50, 50));
        lblIdentity_ImgPerson->setMaximumSize(QSize(400, 400));
        lblIdentity_ImgPerson->setAutoFillBackground(false);
        lblIdentity_ImgPerson->setStyleSheet(QString::fromUtf8(""));
        lblIdentity_ImgPerson->setFrameShape(QFrame::NoFrame);
        lblIdentity_ImgPerson->setScaledContents(true);
        lblIdentity_ImgPerson->setAlignment(Qt::AlignCenter);
        txtIdentity_Parents = new QLineEdit(page_Identity);
        txtIdentity_Parents->setObjectName(QString::fromUtf8("txtIdentity_Parents"));
        txtIdentity_Parents->setGeometry(QRect(23, 314, 361, 48));
        txtIdentity_Parents->setMinimumSize(QSize(240, 0));
        txtIdentity_Parents->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_Parents->setFrame(false);
        txtIdentity_Parents->setReadOnly(true);
        txtIdentity_DocumentNumber = new QLineEdit(page_Identity);
        txtIdentity_DocumentNumber->setObjectName(QString::fromUtf8("txtIdentity_DocumentNumber"));
        txtIdentity_DocumentNumber->setGeometry(QRect(23, 210, 140, 20));
        txtIdentity_DocumentNumber->setFont(font);
        txtIdentity_DocumentNumber->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_DocumentNumber->setFrame(false);
        txtIdentity_DocumentNumber->setReadOnly(true);
        txtIdentity_ValidFrom_Until = new QLineEdit(page_Identity);
        txtIdentity_ValidFrom_Until->setObjectName(QString::fromUtf8("txtIdentity_ValidFrom_Until"));
        txtIdentity_ValidFrom_Until->setGeometry(QRect(186, 210, 87, 20));
        txtIdentity_ValidFrom_Until->setMinimumSize(QSize(0, 0));
        txtIdentity_ValidFrom_Until->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg-Blue.png);"));
        txtIdentity_ValidFrom_Until->setFrame(false);
        txtIdentity_ValidFrom_Until->setReadOnly(true);
        stackedWidget->addWidget(page_Identity);
        lblIdentity_ImgPerson->raise();
        txtIdentity_GivenNames->raise();
        txtIdentity_Name->raise();
        txtIdentity_Sex->raise();
        txtIdentity_Height->raise();
        txtIdentity_Country->raise();
        txtIdentity_Nationality->raise();
        txtIdentity_BirthDate->raise();
        txtIdentity_Parents->raise();
        txtIdentity_DocumentNumber->raise();
        txtIdentity_ValidFrom_Until->raise();
        page_IdentityExtra = new QWidget();
        page_IdentityExtra->setObjectName(QString::fromUtf8("page_IdentityExtra"));
        txtIdentityExtra_SocialSecurityNo = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_SocialSecurityNo->setObjectName(QString::fromUtf8("txtIdentityExtra_SocialSecurityNo"));
        txtIdentityExtra_SocialSecurityNo->setGeometry(QRect(190, 60, 131, 16));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(txtIdentityExtra_SocialSecurityNo->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_SocialSecurityNo->setSizePolicy(sizePolicy3);
        txtIdentityExtra_SocialSecurityNo->setFrame(false);
        txtIdentityExtra_SocialSecurityNo->setReadOnly(true);
        txtIdentityExtra_TaxNo = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_TaxNo->setObjectName(QString::fromUtf8("txtIdentityExtra_TaxNo"));
        txtIdentityExtra_TaxNo->setGeometry(QRect(30, 60, 121, 16));
        sizePolicy3.setHeightForWidth(txtIdentityExtra_TaxNo->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_TaxNo->setSizePolicy(sizePolicy3);
        txtIdentityExtra_TaxNo->setFrame(false);
        txtIdentityExtra_TaxNo->setReadOnly(true);
        txtIdentityExtra_CardVersion = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_CardVersion->setObjectName(QString::fromUtf8("txtIdentityExtra_CardVersion"));
        txtIdentityExtra_CardVersion->setGeometry(QRect(30, 160, 141, 16));
        sizePolicy3.setHeightForWidth(txtIdentityExtra_CardVersion->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_CardVersion->setSizePolicy(sizePolicy3);
        txtIdentityExtra_CardVersion->setFrame(false);
        txtIdentityExtra_CardVersion->setReadOnly(true);
        txtIdentityExtra_DocumentType = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_DocumentType->setObjectName(QString::fromUtf8("txtIdentityExtra_DocumentType"));
        txtIdentityExtra_DocumentType->setGeometry(QRect(190, 280, 201, 16));
        sizePolicy3.setHeightForWidth(txtIdentityExtra_DocumentType->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_DocumentType->setSizePolicy(sizePolicy3);
        txtIdentityExtra_DocumentType->setFrame(false);
        txtIdentityExtra_DocumentType->setReadOnly(true);
        txtIdentityExtra_LocalofRequest = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_LocalofRequest->setObjectName(QString::fromUtf8("txtIdentityExtra_LocalofRequest"));
        txtIdentityExtra_LocalofRequest->setGeometry(QRect(20, 380, 321, 16));
        sizePolicy3.setHeightForWidth(txtIdentityExtra_LocalofRequest->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_LocalofRequest->setSizePolicy(sizePolicy3);
        txtIdentityExtra_LocalofRequest->setSizeIncrement(QSize(0, 0));
        txtIdentityExtra_LocalofRequest->setFrame(false);
        txtIdentityExtra_LocalofRequest->setReadOnly(true);
        txtIdentityExtra_HealthNo = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_HealthNo->setObjectName(QString::fromUtf8("txtIdentityExtra_HealthNo"));
        txtIdentityExtra_HealthNo->setGeometry(QRect(350, 60, 201, 16));
        sizePolicy3.setHeightForWidth(txtIdentityExtra_HealthNo->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_HealthNo->setSizePolicy(sizePolicy3);
        txtIdentityExtra_HealthNo->setFrame(false);
        txtIdentityExtra_HealthNo->setReadOnly(true);
        txtIdentityExtra_IssuingEntity = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_IssuingEntity->setObjectName(QString::fromUtf8("txtIdentityExtra_IssuingEntity"));
        txtIdentityExtra_IssuingEntity->setGeometry(QRect(30, 280, 141, 16));
        sizePolicy3.setHeightForWidth(txtIdentityExtra_IssuingEntity->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_IssuingEntity->setSizePolicy(sizePolicy3);
        txtIdentityExtra_IssuingEntity->setFrame(false);
        txtIdentityExtra_IssuingEntity->setReadOnly(true);
        txtIdentityExtra_Validate = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_Validate->setObjectName(QString::fromUtf8("txtIdentityExtra_Validate"));
        txtIdentityExtra_Validate->setGeometry(QRect(30, 320, 381, 20));
        sizePolicy3.setHeightForWidth(txtIdentityExtra_Validate->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_Validate->setSizePolicy(sizePolicy3);
        txtIdentityExtra_Validate->setSizeIncrement(QSize(0, 0));
        txtIdentityExtra_Validate->setFrame(false);
        txtIdentityExtra_Validate->setReadOnly(true);
        txtSpecialStatus = new QLineEdit(page_IdentityExtra);
        txtSpecialStatus->setObjectName(QString::fromUtf8("txtSpecialStatus"));
        txtSpecialStatus->setGeometry(QRect(520, 180, 201, 16));
        sizePolicy1.setHeightForWidth(txtSpecialStatus->sizePolicy().hasHeightForWidth());
        txtSpecialStatus->setSizePolicy(sizePolicy1);
        txtSpecialStatus->setFrame(false);
        txtSpecialStatus->setReadOnly(true);
        txtIdentityExtra_ChipNumber = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_ChipNumber->setObjectName(QString::fromUtf8("txtIdentityExtra_ChipNumber"));
        txtIdentityExtra_ChipNumber->setGeometry(QRect(420, 130, 353, 16));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(2);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(txtIdentityExtra_ChipNumber->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_ChipNumber->setSizePolicy(sizePolicy4);
        txtIdentityExtra_ChipNumber->setMinimumSize(QSize(100, 0));
        txtIdentityExtra_ChipNumber->setFrame(false);
        txtIdentityExtra_ChipNumber->setReadOnly(true);
        lblChip_Number = new QLabel(page_IdentityExtra);
        lblChip_Number->setObjectName(QString::fromUtf8("lblChip_Number"));
        lblChip_Number->setGeometry(QRect(420, 100, 353, 16));
        sizePolicy1.setHeightForWidth(lblChip_Number->sizePolicy().hasHeightForWidth());
        lblChip_Number->setSizePolicy(sizePolicy1);
        txtIdentityExtra_Card_Number = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_Card_Number->setObjectName(QString::fromUtf8("txtIdentityExtra_Card_Number"));
        txtIdentityExtra_Card_Number->setGeometry(QRect(410, 240, 353, 16));
        sizePolicy1.setHeightForWidth(txtIdentityExtra_Card_Number->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_Card_Number->setSizePolicy(sizePolicy1);
        txtIdentityExtra_Card_Number->setFrame(false);
        txtIdentityExtra_Card_Number->setReadOnly(true);
        txtIdentityExtra_ValidFrom_Until = new QLineEdit(page_IdentityExtra);
        txtIdentityExtra_ValidFrom_Until->setObjectName(QString::fromUtf8("txtIdentityExtra_ValidFrom_Until"));
        txtIdentityExtra_ValidFrom_Until->setGeometry(QRect(190, 160, 131, 16));
        sizePolicy1.setHeightForWidth(txtIdentityExtra_ValidFrom_Until->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_ValidFrom_Until->setSizePolicy(sizePolicy1);
        txtIdentityExtra_ValidFrom_Until->setFrame(false);
        txtIdentityExtra_ValidFrom_Until->setReadOnly(true);
        stackedWidget->addWidget(page_IdentityExtra);
        page_Address = new QWidget();
        page_Address->setObjectName(QString::fromUtf8("page_Address"));
        lblAddress_District = new QLabel(page_Address);
        lblAddress_District->setObjectName(QString::fromUtf8("lblAddress_District"));
        lblAddress_District->setGeometry(QRect(60, 20, 201, 28));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(lblAddress_District->sizePolicy().hasHeightForWidth());
        lblAddress_District->setSizePolicy(sizePolicy5);
        txtAddress_District = new QLineEdit(page_Address);
        txtAddress_District->setObjectName(QString::fromUtf8("txtAddress_District"));
        txtAddress_District->setGeometry(QRect(60, 60, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_District->sizePolicy().hasHeightForWidth());
        txtAddress_District->setSizePolicy(sizePolicy1);
        txtAddress_District->setFrame(false);
        txtAddress_District->setReadOnly(true);
        lblAddress_Place = new QLabel(page_Address);
        lblAddress_Place->setObjectName(QString::fromUtf8("lblAddress_Place"));
        lblAddress_Place->setGeometry(QRect(330, 20, 176, 28));
        sizePolicy5.setHeightForWidth(lblAddress_Place->sizePolicy().hasHeightForWidth());
        lblAddress_Place->setSizePolicy(sizePolicy5);
        txtAddress_Place = new QLineEdit(page_Address);
        txtAddress_Place->setObjectName(QString::fromUtf8("txtAddress_Place"));
        txtAddress_Place->setGeometry(QRect(330, 70, 176, 16));
        sizePolicy1.setHeightForWidth(txtAddress_Place->sizePolicy().hasHeightForWidth());
        txtAddress_Place->setSizePolicy(sizePolicy1);
        txtAddress_Place->setFrame(false);
        txtAddress_Place->setReadOnly(true);
        lblAddress_Municipality = new QLabel(page_Address);
        lblAddress_Municipality->setObjectName(QString::fromUtf8("lblAddress_Municipality"));
        lblAddress_Municipality->setGeometry(QRect(70, 90, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_Municipality->sizePolicy().hasHeightForWidth());
        lblAddress_Municipality->setSizePolicy(sizePolicy5);
        txtAddress_Municipality = new QLineEdit(page_Address);
        txtAddress_Municipality->setObjectName(QString::fromUtf8("txtAddress_Municipality"));
        txtAddress_Municipality->setGeometry(QRect(70, 130, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_Municipality->sizePolicy().hasHeightForWidth());
        txtAddress_Municipality->setSizePolicy(sizePolicy1);
        txtAddress_Municipality->setFrame(false);
        txtAddress_Municipality->setReadOnly(true);
        lblAddress_Zip3 = new QLabel(page_Address);
        lblAddress_Zip3->setObjectName(QString::fromUtf8("lblAddress_Zip3"));
        lblAddress_Zip3->setGeometry(QRect(330, 100, 176, 28));
        sizePolicy5.setHeightForWidth(lblAddress_Zip3->sizePolicy().hasHeightForWidth());
        lblAddress_Zip3->setSizePolicy(sizePolicy5);
        txtAddress_Zip3 = new QLineEdit(page_Address);
        txtAddress_Zip3->setObjectName(QString::fromUtf8("txtAddress_Zip3"));
        txtAddress_Zip3->setGeometry(QRect(320, 130, 176, 16));
        sizePolicy1.setHeightForWidth(txtAddress_Zip3->sizePolicy().hasHeightForWidth());
        txtAddress_Zip3->setSizePolicy(sizePolicy1);
        txtAddress_Zip3->setFrame(false);
        txtAddress_Zip3->setReadOnly(true);
        lblAddress_PostalLocality = new QLabel(page_Address);
        lblAddress_PostalLocality->setObjectName(QString::fromUtf8("lblAddress_PostalLocality"));
        lblAddress_PostalLocality->setGeometry(QRect(540, 100, 176, 28));
        sizePolicy5.setHeightForWidth(lblAddress_PostalLocality->sizePolicy().hasHeightForWidth());
        lblAddress_PostalLocality->setSizePolicy(sizePolicy5);
        txtAddress_PostalLocality = new QLineEdit(page_Address);
        txtAddress_PostalLocality->setObjectName(QString::fromUtf8("txtAddress_PostalLocality"));
        txtAddress_PostalLocality->setGeometry(QRect(540, 140, 176, 16));
        sizePolicy1.setHeightForWidth(txtAddress_PostalLocality->sizePolicy().hasHeightForWidth());
        txtAddress_PostalLocality->setSizePolicy(sizePolicy1);
        txtAddress_PostalLocality->setFrame(false);
        txtAddress_PostalLocality->setReadOnly(true);
        lblAddress_CivilParish = new QLabel(page_Address);
        lblAddress_CivilParish->setObjectName(QString::fromUtf8("lblAddress_CivilParish"));
        lblAddress_CivilParish->setGeometry(QRect(70, 170, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_CivilParish->sizePolicy().hasHeightForWidth());
        lblAddress_CivilParish->setSizePolicy(sizePolicy5);
        txtAddress_CivilParish = new QLineEdit(page_Address);
        txtAddress_CivilParish->setObjectName(QString::fromUtf8("txtAddress_CivilParish"));
        txtAddress_CivilParish->setGeometry(QRect(70, 220, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_CivilParish->sizePolicy().hasHeightForWidth());
        txtAddress_CivilParish->setSizePolicy(sizePolicy1);
        txtAddress_CivilParish->setFrame(false);
        txtAddress_CivilParish->setReadOnly(true);
        lblAddress_StreetType1 = new QLabel(page_Address);
        lblAddress_StreetType1->setObjectName(QString::fromUtf8("lblAddress_StreetType1"));
        lblAddress_StreetType1->setGeometry(QRect(320, 180, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_StreetType1->sizePolicy().hasHeightForWidth());
        lblAddress_StreetType1->setSizePolicy(sizePolicy5);
        txtAddress_StreetType1 = new QLineEdit(page_Address);
        txtAddress_StreetType1->setObjectName(QString::fromUtf8("txtAddress_StreetType1"));
        txtAddress_StreetType1->setGeometry(QRect(320, 220, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_StreetType1->sizePolicy().hasHeightForWidth());
        txtAddress_StreetType1->setSizePolicy(sizePolicy1);
        txtAddress_StreetType1->setFrame(false);
        txtAddress_StreetType1->setReadOnly(true);
        lblAddress_StreetType2 = new QLabel(page_Address);
        lblAddress_StreetType2->setObjectName(QString::fromUtf8("lblAddress_StreetType2"));
        lblAddress_StreetType2->setGeometry(QRect(70, 250, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_StreetType2->sizePolicy().hasHeightForWidth());
        lblAddress_StreetType2->setSizePolicy(sizePolicy5);
        txtAddress_StreetType2 = new QLineEdit(page_Address);
        txtAddress_StreetType2->setObjectName(QString::fromUtf8("txtAddress_StreetType2"));
        txtAddress_StreetType2->setGeometry(QRect(70, 290, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_StreetType2->sizePolicy().hasHeightForWidth());
        txtAddress_StreetType2->setSizePolicy(sizePolicy1);
        txtAddress_StreetType2->setFrame(false);
        txtAddress_StreetType2->setReadOnly(true);
        lblAddress_StreetName = new QLabel(page_Address);
        lblAddress_StreetName->setObjectName(QString::fromUtf8("lblAddress_StreetName"));
        lblAddress_StreetName->setGeometry(QRect(300, 250, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_StreetName->sizePolicy().hasHeightForWidth());
        lblAddress_StreetName->setSizePolicy(sizePolicy5);
        txtAddress_StreetName = new QLineEdit(page_Address);
        txtAddress_StreetName->setObjectName(QString::fromUtf8("txtAddress_StreetName"));
        txtAddress_StreetName->setGeometry(QRect(290, 290, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_StreetName->sizePolicy().hasHeightForWidth());
        txtAddress_StreetName->setSizePolicy(sizePolicy1);
        txtAddress_StreetName->setFrame(false);
        txtAddress_StreetName->setReadOnly(true);
        lblAddress_BuildingType1 = new QLabel(page_Address);
        lblAddress_BuildingType1->setObjectName(QString::fromUtf8("lblAddress_BuildingType1"));
        lblAddress_BuildingType1->setGeometry(QRect(500, 250, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_BuildingType1->sizePolicy().hasHeightForWidth());
        lblAddress_BuildingType1->setSizePolicy(sizePolicy5);
        txtAddress_BuildingType1 = new QLineEdit(page_Address);
        txtAddress_BuildingType1->setObjectName(QString::fromUtf8("txtAddress_BuildingType1"));
        txtAddress_BuildingType1->setGeometry(QRect(510, 290, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_BuildingType1->sizePolicy().hasHeightForWidth());
        txtAddress_BuildingType1->setSizePolicy(sizePolicy1);
        txtAddress_BuildingType1->setFrame(false);
        txtAddress_BuildingType1->setReadOnly(true);
        lblAddress_BuildingType2 = new QLabel(page_Address);
        lblAddress_BuildingType2->setObjectName(QString::fromUtf8("lblAddress_BuildingType2"));
        lblAddress_BuildingType2->setGeometry(QRect(80, 320, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_BuildingType2->sizePolicy().hasHeightForWidth());
        lblAddress_BuildingType2->setSizePolicy(sizePolicy5);
        txtAddress_BuildingType2 = new QLineEdit(page_Address);
        txtAddress_BuildingType2->setObjectName(QString::fromUtf8("txtAddress_BuildingType2"));
        txtAddress_BuildingType2->setGeometry(QRect(70, 360, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_BuildingType2->sizePolicy().hasHeightForWidth());
        txtAddress_BuildingType2->setSizePolicy(sizePolicy1);
        txtAddress_BuildingType2->setFrame(false);
        txtAddress_BuildingType2->setReadOnly(true);
        lblAddress_DoorNo = new QLabel(page_Address);
        lblAddress_DoorNo->setObjectName(QString::fromUtf8("lblAddress_DoorNo"));
        lblAddress_DoorNo->setGeometry(QRect(300, 310, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_DoorNo->sizePolicy().hasHeightForWidth());
        lblAddress_DoorNo->setSizePolicy(sizePolicy5);
        txtAddress_DoorNo = new QLineEdit(page_Address);
        txtAddress_DoorNo->setObjectName(QString::fromUtf8("txtAddress_DoorNo"));
        txtAddress_DoorNo->setGeometry(QRect(290, 360, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_DoorNo->sizePolicy().hasHeightForWidth());
        txtAddress_DoorNo->setSizePolicy(sizePolicy1);
        txtAddress_DoorNo->setFrame(false);
        txtAddress_DoorNo->setReadOnly(true);
        lblAddress_Floor = new QLabel(page_Address);
        lblAddress_Floor->setObjectName(QString::fromUtf8("lblAddress_Floor"));
        lblAddress_Floor->setGeometry(QRect(510, 320, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_Floor->sizePolicy().hasHeightForWidth());
        lblAddress_Floor->setSizePolicy(sizePolicy5);
        txtAddress_Floor = new QLineEdit(page_Address);
        txtAddress_Floor->setObjectName(QString::fromUtf8("txtAddress_Floor"));
        txtAddress_Floor->setGeometry(QRect(490, 360, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_Floor->sizePolicy().hasHeightForWidth());
        txtAddress_Floor->setSizePolicy(sizePolicy1);
        txtAddress_Floor->setFrame(false);
        txtAddress_Floor->setReadOnly(true);
        lblAddress_Side = new QLabel(page_Address);
        lblAddress_Side->setObjectName(QString::fromUtf8("lblAddress_Side"));
        lblAddress_Side->setGeometry(QRect(80, 380, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_Side->sizePolicy().hasHeightForWidth());
        lblAddress_Side->setSizePolicy(sizePolicy5);
        txtAddress_Side = new QLineEdit(page_Address);
        txtAddress_Side->setObjectName(QString::fromUtf8("txtAddress_Side"));
        txtAddress_Side->setGeometry(QRect(70, 420, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_Side->sizePolicy().hasHeightForWidth());
        txtAddress_Side->setSizePolicy(sizePolicy1);
        txtAddress_Side->setFrame(false);
        txtAddress_Side->setReadOnly(true);
        lblAddress_Locality = new QLabel(page_Address);
        lblAddress_Locality->setObjectName(QString::fromUtf8("lblAddress_Locality"));
        lblAddress_Locality->setGeometry(QRect(290, 380, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_Locality->sizePolicy().hasHeightForWidth());
        lblAddress_Locality->setSizePolicy(sizePolicy5);
        txtAddress_Locality = new QLineEdit(page_Address);
        txtAddress_Locality->setObjectName(QString::fromUtf8("txtAddress_Locality"));
        txtAddress_Locality->setGeometry(QRect(290, 420, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_Locality->sizePolicy().hasHeightForWidth());
        txtAddress_Locality->setSizePolicy(sizePolicy1);
        txtAddress_Locality->setFrame(false);
        txtAddress_Locality->setReadOnly(true);
        lblAddress_Zip4 = new QLabel(page_Address);
        lblAddress_Zip4->setObjectName(QString::fromUtf8("lblAddress_Zip4"));
        lblAddress_Zip4->setGeometry(QRect(510, 390, 201, 28));
        sizePolicy5.setHeightForWidth(lblAddress_Zip4->sizePolicy().hasHeightForWidth());
        lblAddress_Zip4->setSizePolicy(sizePolicy5);
        txtAddress_Zip4 = new QLineEdit(page_Address);
        txtAddress_Zip4->setObjectName(QString::fromUtf8("txtAddress_Zip4"));
        txtAddress_Zip4->setGeometry(QRect(490, 420, 201, 16));
        sizePolicy1.setHeightForWidth(txtAddress_Zip4->sizePolicy().hasHeightForWidth());
        txtAddress_Zip4->setSizePolicy(sizePolicy1);
        txtAddress_Zip4->setFrame(false);
        txtAddress_Zip4->setReadOnly(true);
        stackedWidget->addWidget(page_Address);
        page_Certificates = new QWidget();
        page_Certificates->setObjectName(QString::fromUtf8("page_Certificates"));
        treeCert = new QTreeWidget(page_Certificates);
        treeCert->setObjectName(QString::fromUtf8("treeCert"));
        treeCert->setGeometry(QRect(20, 10, 300, 251));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(20);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(treeCert->sizePolicy().hasHeightForWidth());
        treeCert->setSizePolicy(sizePolicy6);
        treeCert->setMinimumSize(QSize(300, 0));
        treeCert->setSortingEnabled(true);
        lblCert_Owner = new QLabel(page_Certificates);
        lblCert_Owner->setObjectName(QString::fromUtf8("lblCert_Owner"));
        lblCert_Owner->setGeometry(QRect(330, 20, 291, 20));
        sizePolicy5.setHeightForWidth(lblCert_Owner->sizePolicy().hasHeightForWidth());
        lblCert_Owner->setSizePolicy(sizePolicy5);
        txtCert_Owner = new QLineEdit(page_Certificates);
        txtCert_Owner->setObjectName(QString::fromUtf8("txtCert_Owner"));
        txtCert_Owner->setGeometry(QRect(330, 40, 401, 27));
        txtCert_Owner->setReadOnly(true);
        lblCert_Issuer = new QLabel(page_Certificates);
        lblCert_Issuer->setObjectName(QString::fromUtf8("lblCert_Issuer"));
        lblCert_Issuer->setGeometry(QRect(340, 80, 291, 20));
        sizePolicy5.setHeightForWidth(lblCert_Issuer->sizePolicy().hasHeightForWidth());
        lblCert_Issuer->setSizePolicy(sizePolicy5);
        txtCert_Issuer = new QLineEdit(page_Certificates);
        txtCert_Issuer->setObjectName(QString::fromUtf8("txtCert_Issuer"));
        txtCert_Issuer->setGeometry(QRect(340, 100, 261, 27));
        txtCert_Issuer->setReadOnly(true);
        lblCert_KeyLenght = new QLabel(page_Certificates);
        lblCert_KeyLenght->setObjectName(QString::fromUtf8("lblCert_KeyLenght"));
        lblCert_KeyLenght->setGeometry(QRect(330, 140, 151, 20));
        sizePolicy5.setHeightForWidth(lblCert_KeyLenght->sizePolicy().hasHeightForWidth());
        lblCert_KeyLenght->setSizePolicy(sizePolicy5);
        txtCert_KeyLenght = new QLineEdit(page_Certificates);
        txtCert_KeyLenght->setObjectName(QString::fromUtf8("txtCert_KeyLenght"));
        txtCert_KeyLenght->setGeometry(QRect(340, 160, 521, 27));
        txtCert_KeyLenght->setReadOnly(true);
        lblCert_ValidFrom = new QLabel(page_Certificates);
        lblCert_ValidFrom->setObjectName(QString::fromUtf8("lblCert_ValidFrom"));
        lblCert_ValidFrom->setGeometry(QRect(330, 200, 341, 20));
        sizePolicy5.setHeightForWidth(lblCert_ValidFrom->sizePolicy().hasHeightForWidth());
        lblCert_ValidFrom->setSizePolicy(sizePolicy5);
        txtCert_ValidFrom = new QLineEdit(page_Certificates);
        txtCert_ValidFrom->setObjectName(QString::fromUtf8("txtCert_ValidFrom"));
        txtCert_ValidFrom->setGeometry(QRect(340, 220, 146, 27));
        txtCert_ValidFrom->setReadOnly(true);
        lblCert_ValidUntil = new QLabel(page_Certificates);
        lblCert_ValidUntil->setObjectName(QString::fromUtf8("lblCert_ValidUntil"));
        lblCert_ValidUntil->setGeometry(QRect(530, 200, 241, 20));
        sizePolicy5.setHeightForWidth(lblCert_ValidUntil->sizePolicy().hasHeightForWidth());
        lblCert_ValidUntil->setSizePolicy(sizePolicy5);
        txtCert_ValidUntil = new QLineEdit(page_Certificates);
        txtCert_ValidUntil->setObjectName(QString::fromUtf8("txtCert_ValidUntil"));
        txtCert_ValidUntil->setGeometry(QRect(520, 220, 181, 27));
        txtCert_ValidUntil->setReadOnly(true);
        lblCert_Status = new QLabel(page_Certificates);
        lblCert_Status->setObjectName(QString::fromUtf8("lblCert_Status"));
        lblCert_Status->setGeometry(QRect(330, 260, 451, 20));
        sizePolicy5.setHeightForWidth(lblCert_Status->sizePolicy().hasHeightForWidth());
        lblCert_Status->setSizePolicy(sizePolicy5);
        txtCert_Status = new QLineEdit(page_Certificates);
        txtCert_Status->setObjectName(QString::fromUtf8("txtCert_Status"));
        txtCert_Status->setGeometry(QRect(320, 300, 171, 27));
        txtCert_Status->setReadOnly(true);
        btnOCSPCheck = new QPushButton(page_Certificates);
        btnOCSPCheck->setObjectName(QString::fromUtf8("btnOCSPCheck"));
        btnOCSPCheck->setGeometry(QRect(530, 280, 402, 27));
        btnOCSPCheck->setFont(font);
        tabWidget_Identity = new QTabWidget(page_Certificates);
        tabWidget_Identity->setObjectName(QString::fromUtf8("tabWidget_Identity"));
        tabWidget_Identity->setGeometry(QRect(40, 130, 1141, 571));
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(tabWidget_Identity->sizePolicy().hasHeightForWidth());
        tabWidget_Identity->setSizePolicy(sizePolicy7);
        tabWidget_Identity->setSizeIncrement(QSize(0, 0));
        tabWidget_Identity->setFont(font);
        tabWidget_Identity->setFocusPolicy(Qt::StrongFocus);
        tabWidget_Identity->setTabShape(QTabWidget::Rounded);
        tabIdentity = new QWidget();
        tabIdentity->setObjectName(QString::fromUtf8("tabIdentity"));
        tabIdentity->setFocusPolicy(Qt::NoFocus);
        lblIdentity_Head2_4 = new QLabel(tabIdentity);
        lblIdentity_Head2_4->setObjectName(QString::fromUtf8("lblIdentity_Head2_4"));
        lblIdentity_Head2_4->setGeometry(QRect(570, 32, 180, 16));
        lblIdentity_Head2_4->setFont(font);
        lblIdentity_Head2_4->setTextFormat(Qt::PlainText);
        lblIdentity_Head1_3 = new QLabel(tabIdentity);
        lblIdentity_Head1_3->setObjectName(QString::fromUtf8("lblIdentity_Head1_3"));
        lblIdentity_Head1_3->setGeometry(QRect(380, 0, 181, 30));
        lblIdentity_Head1_3->setFont(font);
        lblIdentity_Head1_3->setTextFormat(Qt::PlainText);
        lblIdentity_Head1_4 = new QLabel(tabIdentity);
        lblIdentity_Head1_4->setObjectName(QString::fromUtf8("lblIdentity_Head1_4"));
        lblIdentity_Head1_4->setGeometry(QRect(570, 0, 180, 30));
        lblIdentity_Head1_4->setFont(font);
        lblIdentity_Head1_4->setTextFormat(Qt::PlainText);
        lblIdentity_Head2_3 = new QLabel(tabIdentity);
        lblIdentity_Head2_3->setObjectName(QString::fromUtf8("lblIdentity_Head2_3"));
        lblIdentity_Head2_3->setGeometry(QRect(383, 32, 181, 16));
        lblIdentity_Head2_3->setFont(font);
        lblIdentity_Head2_3->setTextFormat(Qt::PlainText);
        tabWidget_Identity->addTab(tabIdentity, QString());
        tabForeigners = new QWidget();
        tabForeigners->setObjectName(QString::fromUtf8("tabForeigners"));
        tabForeigners->setFocusPolicy(Qt::NoFocus);
        txtForeigners_Card_Number = new QLineEdit(tabForeigners);
        txtForeigners_Card_Number->setObjectName(QString::fromUtf8("txtForeigners_Card_Number"));
        txtForeigners_Card_Number->setGeometry(QRect(463, 11, 288, 16));
        sizePolicy1.setHeightForWidth(txtForeigners_Card_Number->sizePolicy().hasHeightForWidth());
        txtForeigners_Card_Number->setSizePolicy(sizePolicy1);
        txtForeigners_Card_Number->setFont(font);
        txtForeigners_Card_Number->setFrame(false);
        txtForeigners_Card_Number->setReadOnly(true);
        label = new QLabel(tabForeigners);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(37, 61, 176, 20));
        txtForeigners_Name = new QLineEdit(tabForeigners);
        txtForeigners_Name->setObjectName(QString::fromUtf8("txtForeigners_Name"));
        txtForeigners_Name->setGeometry(QRect(219, 63, 532, 16));
        sizePolicy3.setHeightForWidth(txtForeigners_Name->sizePolicy().hasHeightForWidth());
        txtForeigners_Name->setSizePolicy(sizePolicy3);
        txtForeigners_Name->setFrame(false);
        txtForeigners_Name->setReadOnly(true);
        txtForeigners_GivenNames = new QLineEdit(tabForeigners);
        txtForeigners_GivenNames->setObjectName(QString::fromUtf8("txtForeigners_GivenNames"));
        txtForeigners_GivenNames->setGeometry(QRect(219, 89, 532, 16));
        sizePolicy1.setHeightForWidth(txtForeigners_GivenNames->sizePolicy().hasHeightForWidth());
        txtForeigners_GivenNames->setSizePolicy(sizePolicy1);
        txtForeigners_GivenNames->setFrame(false);
        txtForeigners_GivenNames->setAlignment(Qt::AlignLeading);
        txtForeigners_GivenNames->setReadOnly(true);
        label_2 = new QLabel(tabForeigners);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(37, 113, 176, 20));
        txtForeigners_ValidTot = new QLineEdit(tabForeigners);
        txtForeigners_ValidTot->setObjectName(QString::fromUtf8("txtForeigners_ValidTot"));
        txtForeigners_ValidTot->setGeometry(QRect(219, 115, 532, 16));
        sizePolicy1.setHeightForWidth(txtForeigners_ValidTot->sizePolicy().hasHeightForWidth());
        txtForeigners_ValidTot->setSizePolicy(sizePolicy1);
        txtForeigners_ValidTot->setFrame(false);
        txtForeigners_ValidTot->setReadOnly(true);
        label_3 = new QLabel(tabForeigners);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(37, 139, 211, 20));
        txtForeigners_PlaceOfIssue = new QLineEdit(tabForeigners);
        txtForeigners_PlaceOfIssue->setObjectName(QString::fromUtf8("txtForeigners_PlaceOfIssue"));
        txtForeigners_PlaceOfIssue->setGeometry(QRect(260, 141, 491, 20));
        sizePolicy1.setHeightForWidth(txtForeigners_PlaceOfIssue->sizePolicy().hasHeightForWidth());
        txtForeigners_PlaceOfIssue->setSizePolicy(sizePolicy1);
        txtForeigners_PlaceOfIssue->setFrame(false);
        txtForeigners_PlaceOfIssue->setReadOnly(true);
        label_4 = new QLabel(tabForeigners);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(219, 165, 532, 20));
        lblForeigners_ImgPerson = new QLabel(tabForeigners);
        lblForeigners_ImgPerson->setObjectName(QString::fromUtf8("lblForeigners_ImgPerson"));
        lblForeigners_ImgPerson->setGeometry(QRect(63, 191, 150, 270));
        sizePolicy7.setHeightForWidth(lblForeigners_ImgPerson->sizePolicy().hasHeightForWidth());
        lblForeigners_ImgPerson->setSizePolicy(sizePolicy7);
        lblForeigners_ImgPerson->setMinimumSize(QSize(150, 270));
        lblForeigners_ImgPerson->setFrameShape(QFrame::NoFrame);
        lblForeigners_ImgPerson->setScaledContents(true);
        lblForeigners_ImgPerson->setAlignment(Qt::AlignCenter);
        txtForeigners_CardType = new QLineEdit(tabForeigners);
        txtForeigners_CardType->setObjectName(QString::fromUtf8("txtForeigners_CardType"));
        txtForeigners_CardType->setGeometry(QRect(219, 193, 532, 16));
        sizePolicy1.setHeightForWidth(txtForeigners_CardType->sizePolicy().hasHeightForWidth());
        txtForeigners_CardType->setSizePolicy(sizePolicy1);
        txtForeigners_CardType->setFrame(false);
        txtForeigners_CardType->setReadOnly(true);
        lblForeigners_Footer_1 = new QLabel(tabForeigners);
        lblForeigners_Footer_1->setObjectName(QString::fromUtf8("lblForeigners_Footer_1"));
        lblForeigners_Footer_1->setGeometry(QRect(219, 413, 532, 20));
        sizePolicy5.setHeightForWidth(lblForeigners_Footer_1->sizePolicy().hasHeightForWidth());
        lblForeigners_Footer_1->setSizePolicy(sizePolicy5);
        lblForeigners_Footer_2 = new QLabel(tabForeigners);
        lblForeigners_Footer_2->setObjectName(QString::fromUtf8("lblForeigners_Footer_2"));
        lblForeigners_Footer_2->setGeometry(QRect(219, 439, 532, 20));
        sizePolicy5.setHeightForWidth(lblForeigners_Footer_2->sizePolicy().hasHeightForWidth());
        lblForeigners_Footer_2->setSizePolicy(sizePolicy5);
        tabWidget_Identity->addTab(tabForeigners, QString());
        tabSis = new QWidget();
        tabSis->setObjectName(QString::fromUtf8("tabSis"));
        tabSis->setFocusPolicy(Qt::NoFocus);
        txtSis_SocialSecurityNumber = new QLineEdit(tabSis);
        txtSis_SocialSecurityNumber->setObjectName(QString::fromUtf8("txtSis_SocialSecurityNumber"));
        txtSis_SocialSecurityNumber->setGeometry(QRect(286, 71, 465, 16));
        txtSis_SocialSecurityNumber->setFrame(false);
        txtSis_SocialSecurityNumber->setAlignment(Qt::AlignLeading);
        txtSis_SocialSecurityNumber->setReadOnly(true);
        txtSis_Name = new QLineEdit(tabSis);
        txtSis_Name->setObjectName(QString::fromUtf8("txtSis_Name"));
        txtSis_Name->setGeometry(QRect(286, 155, 465, 16));
        txtSis_Name->setFrame(false);
        txtSis_Name->setReadOnly(true);
        txtSis_GivenNames = new QLineEdit(tabSis);
        txtSis_GivenNames->setObjectName(QString::fromUtf8("txtSis_GivenNames"));
        txtSis_GivenNames->setGeometry(QRect(286, 177, 465, 16));
        txtSis_GivenNames->setFrame(false);
        txtSis_GivenNames->setAlignment(Qt::AlignLeading);
        txtSis_GivenNames->setReadOnly(true);
        txtSis_BirthDate = new QLineEdit(tabSis);
        txtSis_BirthDate->setObjectName(QString::fromUtf8("txtSis_BirthDate"));
        txtSis_BirthDate->setGeometry(QRect(286, 201, 152, 16));
        txtSis_BirthDate->setFrame(false);
        txtSis_BirthDate->setAlignment(Qt::AlignLeading);
        txtSis_BirthDate->setReadOnly(true);
        lblSis_Sex = new QLabel(tabSis);
        lblSis_Sex->setObjectName(QString::fromUtf8("lblSis_Sex"));
        lblSis_Sex->setGeometry(QRect(444, 199, 20, 20));
        QSizePolicy sizePolicy8(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(lblSis_Sex->sizePolicy().hasHeightForWidth());
        lblSis_Sex->setSizePolicy(sizePolicy8);
        lblSis_Sex->setMaximumSize(QSize(20, 20));
        lblSis_Sex->setPixmap(QPixmap(QString::fromUtf8(":/images/Images/Male.png")));
        lblSis_Sex->setScaledContents(true);
        lblSis_Sex->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        txtSis_LogicalNumber = new QLineEdit(tabSis);
        txtSis_LogicalNumber->setObjectName(QString::fromUtf8("txtSis_LogicalNumber"));
        txtSis_LogicalNumber->setGeometry(QRect(9, 374, 245, 16));
        txtSis_LogicalNumber->setFrame(false);
        txtSis_LogicalNumber->setAlignment(Qt::AlignLeading);
        txtSis_LogicalNumber->setReadOnly(true);
        txtSis_ValidFrom = new QLineEdit(tabSis);
        txtSis_ValidFrom->setObjectName(QString::fromUtf8("txtSis_ValidFrom"));
        txtSis_ValidFrom->setGeometry(QRect(566, 374, 185, 16));
        txtSis_ValidFrom->setFrame(false);
        txtSis_ValidFrom->setAlignment(Qt::AlignLeading);
        txtSis_ValidFrom->setReadOnly(true);
        tabWidget_Identity->addTab(tabSis, QString());
        tabIdentity_extention = new QWidget();
        tabIdentity_extention->setObjectName(QString::fromUtf8("tabIdentity_extention"));
        tabIdentity_extention->setFocusPolicy(Qt::NoFocus);
        tabWidget_Identity->addTab(tabIdentity_extention, QString());
        tabAddress = new QWidget();
        tabAddress->setObjectName(QString::fromUtf8("tabAddress"));
        tabWidget_Identity->addTab(tabAddress, QString());
        tabForeigners_Back = new QWidget();
        tabForeigners_Back->setObjectName(QString::fromUtf8("tabForeigners_Back"));
        tabForeigners_Back->setFocusPolicy(Qt::NoFocus);
        txtForeignersExtra_Nationality = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Nationality->setObjectName(QString::fromUtf8("txtForeignersExtra_Nationality"));
        txtForeignersExtra_Nationality->setGeometry(QRect(173, 74, 576, 16));
        txtForeignersExtra_Nationality->setFrame(false);
        txtForeignersExtra_Nationality->setReadOnly(true);
        txtForeignersExtra_Sex = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Sex->setObjectName(QString::fromUtf8("txtForeignersExtra_Sex"));
        txtForeignersExtra_Sex->setGeometry(QRect(173, 96, 576, 16));
        txtForeignersExtra_Sex->setFrame(false);
        txtForeignersExtra_Sex->setReadOnly(true);
        txtForeignersExtra_Remarks1 = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Remarks1->setObjectName(QString::fromUtf8("txtForeignersExtra_Remarks1"));
        txtForeignersExtra_Remarks1->setGeometry(QRect(170, 200, 571, 20));
        txtForeignersExtra_Remarks1->setFrame(false);
        txtForeignersExtra_Remarks1->setReadOnly(true);
        txtForeignersExtra_Remarks2 = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Remarks2->setObjectName(QString::fromUtf8("txtForeignersExtra_Remarks2"));
        txtForeignersExtra_Remarks2->setGeometry(QRect(173, 222, 576, 16));
        txtForeignersExtra_Remarks2->setFrame(false);
        txtForeignersExtra_Remarks2->setReadOnly(true);
        txtForeignersExtra_ChipNumber = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_ChipNumber->setObjectName(QString::fromUtf8("txtForeignersExtra_ChipNumber"));
        txtForeignersExtra_ChipNumber->setGeometry(QRect(141, 318, 609, 16));
        txtForeignersExtra_ChipNumber->setFrame(false);
        txtForeignersExtra_ChipNumber->setReadOnly(true);
        lblForeignersExtra_Nationality = new QLabel(tabForeigners_Back);
        lblForeignersExtra_Nationality->setObjectName(QString::fromUtf8("lblForeignersExtra_Nationality"));
        lblForeignersExtra_Nationality->setGeometry(QRect(57, 74, 110, 16));
        lblForeignersExtra_Nationality->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lblForeignersExtra_Sex = new QLabel(tabForeigners_Back);
        lblForeignersExtra_Sex->setObjectName(QString::fromUtf8("lblForeignersExtra_Sex"));
        lblForeignersExtra_Sex->setGeometry(QRect(57, 96, 110, 16));
        lblForeignersExtra_Sex->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lblForeignersExtra_Remarks1 = new QLabel(tabForeigners_Back);
        lblForeignersExtra_Remarks1->setObjectName(QString::fromUtf8("lblForeignersExtra_Remarks1"));
        lblForeignersExtra_Remarks1->setGeometry(QRect(57, 198, 110, 16));
        lblForeignersExtra_Remarks1->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);
        lblForeignersExtra_ChipNumber = new QLabel(tabForeigners_Back);
        lblForeignersExtra_ChipNumber->setObjectName(QString::fromUtf8("lblForeignersExtra_ChipNumber"));
        lblForeignersExtra_ChipNumber->setGeometry(QRect(56, 318, 79, 16));
        lblForeignersExtra_ChipNumber->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txtForeignersExtra_BirthDate = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_BirthDate->setObjectName(QString::fromUtf8("txtForeignersExtra_BirthDate"));
        txtForeignersExtra_BirthDate->setGeometry(QRect(188, 52, 561, 20));
        sizePolicy3.setHeightForWidth(txtForeignersExtra_BirthDate->sizePolicy().hasHeightForWidth());
        txtForeignersExtra_BirthDate->setSizePolicy(sizePolicy3);
        txtForeignersExtra_BirthDate->setMinimumSize(QSize(0, 0));
        txtForeignersExtra_BirthDate->setFrame(false);
        txtForeignersExtra_BirthDate->setReadOnly(true);
        lblForeignersExtra_NationalNumber = new QLabel(tabForeigners_Back);
        lblForeignersExtra_NationalNumber->setObjectName(QString::fromUtf8("lblForeignersExtra_NationalNumber"));
        lblForeignersExtra_NationalNumber->setGeometry(QRect(56, 296, 79, 16));
        lblForeignersExtra_NationalNumber->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txtForeignersExtra_NationalNumber = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_NationalNumber->setObjectName(QString::fromUtf8("txtForeignersExtra_NationalNumber"));
        txtForeignersExtra_NationalNumber->setGeometry(QRect(141, 296, 609, 16));
        sizePolicy1.setHeightForWidth(txtForeignersExtra_NationalNumber->sizePolicy().hasHeightForWidth());
        txtForeignersExtra_NationalNumber->setSizePolicy(sizePolicy1);
        txtForeignersExtra_NationalNumber->setSizeIncrement(QSize(0, 0));
        txtForeignersExtra_NationalNumber->setFrame(false);
        txtForeignersExtra_NationalNumber->setReadOnly(true);
        lblForeignersExtra_BirthDate = new QLabel(tabForeigners_Back);
        lblForeignersExtra_BirthDate->setObjectName(QString::fromUtf8("lblForeignersExtra_BirthDate"));
        lblForeignersExtra_BirthDate->setGeometry(QRect(46, 52, 141, 20));
        lblForeignersExtra_BirthDate->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txtForeignersExtra_Remarks3 = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Remarks3->setObjectName(QString::fromUtf8("txtForeignersExtra_Remarks3"));
        txtForeignersExtra_Remarks3->setGeometry(QRect(170, 250, 576, 16));
        txtForeignersExtra_Remarks3->setFrame(false);
        txtForeignersExtra_Remarks3->setReadOnly(true);
        txtForeignerSpecialStatus = new QLineEdit(tabForeigners_Back);
        txtForeignerSpecialStatus->setObjectName(QString::fromUtf8("txtForeignerSpecialStatus"));
        txtForeignerSpecialStatus->setGeometry(QRect(170, 280, 571, 20));
        sizePolicy1.setHeightForWidth(txtForeignerSpecialStatus->sizePolicy().hasHeightForWidth());
        txtForeignerSpecialStatus->setSizePolicy(sizePolicy1);
        txtForeignerSpecialStatus->setFrame(false);
        txtForeignerSpecialStatus->setReadOnly(true);
        lblForeignersExtra_street = new QLabel(tabForeigners_Back);
        lblForeignersExtra_street->setObjectName(QString::fromUtf8("lblForeignersExtra_street"));
        lblForeignersExtra_street->setGeometry(QRect(65, 120, 101, 20));
        lblForeignersExtra_street->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txtForeignersExtra_Adress_Street = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Adress_Street->setObjectName(QString::fromUtf8("txtForeignersExtra_Adress_Street"));
        txtForeignersExtra_Adress_Street->setGeometry(QRect(170, 120, 581, 20));
        txtForeignersExtra_Adress_Street->setAcceptDrops(false);
        txtForeignersExtra_Adress_Street->setFrame(false);
        txtForeignersExtra_Adress_Street->setReadOnly(true);
        lblForeignersExtra_PostalCode = new QLabel(tabForeigners_Back);
        lblForeignersExtra_PostalCode->setObjectName(QString::fromUtf8("lblForeignersExtra_PostalCode"));
        lblForeignersExtra_PostalCode->setGeometry(QRect(75, 150, 91, 20));
        lblForeignersExtra_PostalCode->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txtForeignersExtra_Adress_PostalCode = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Adress_PostalCode->setObjectName(QString::fromUtf8("txtForeignersExtra_Adress_PostalCode"));
        txtForeignersExtra_Adress_PostalCode->setGeometry(QRect(170, 150, 113, 20));
        txtForeignersExtra_Adress_PostalCode->setFrame(false);
        txtForeignersExtra_Adress_PostalCode->setReadOnly(true);
        txtForeignersExtra_Adress_Muncipality = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Adress_Muncipality->setObjectName(QString::fromUtf8("txtForeignersExtra_Adress_Muncipality"));
        txtForeignersExtra_Adress_Muncipality->setGeometry(QRect(380, 150, 281, 20));
        txtForeignersExtra_Adress_Muncipality->setFrame(false);
        txtForeignersExtra_Adress_Muncipality->setReadOnly(true);
        lblForeignersExtra_municipality = new QLabel(tabForeigners_Back);
        lblForeignersExtra_municipality->setObjectName(QString::fromUtf8("lblForeignersExtra_municipality"));
        lblForeignersExtra_municipality->setGeometry(QRect(300, 150, 71, 16));
        lblForeignersExtra_municipality->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txtForeignersExtra_Adress_Country = new QLineEdit(tabForeigners_Back);
        txtForeignersExtra_Adress_Country->setObjectName(QString::fromUtf8("txtForeignersExtra_Adress_Country"));
        txtForeignersExtra_Adress_Country->setGeometry(QRect(170, 170, 113, 20));
        txtForeignersExtra_Adress_Country->setFrame(false);
        txtForeignersExtra_Adress_Country->setReadOnly(true);
        lblForeignersExtra_country = new QLabel(tabForeigners_Back);
        lblForeignersExtra_country->setObjectName(QString::fromUtf8("lblForeignersExtra_country"));
        lblForeignersExtra_country->setGeometry(QRect(110, 170, 51, 16));
        lblForeignersExtra_country->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        tabWidget_Identity->addTab(tabForeigners_Back, QString());
        tabSis_Back = new QWidget();
        tabSis_Back->setObjectName(QString::fromUtf8("tabSis_Back"));
        tabSis_Back->setFocusPolicy(Qt::NoFocus);
        lblSisExtra_Card_ValidFrom_Until = new QLabel(tabSis_Back);
        lblSisExtra_Card_ValidFrom_Until->setObjectName(QString::fromUtf8("lblSisExtra_Card_ValidFrom_Until"));
        lblSisExtra_Card_ValidFrom_Until->setGeometry(QRect(9, 63, 243, 16));
        QSizePolicy sizePolicy9(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy9.setHorizontalStretch(4);
        sizePolicy9.setVerticalStretch(0);
        sizePolicy9.setHeightForWidth(lblSisExtra_Card_ValidFrom_Until->sizePolicy().hasHeightForWidth());
        lblSisExtra_Card_ValidFrom_Until->setSizePolicy(sizePolicy9);
        txtSisExtra_ValidFrom_Until = new QLineEdit(tabSis_Back);
        txtSisExtra_ValidFrom_Until->setObjectName(QString::fromUtf8("txtSisExtra_ValidFrom_Until"));
        txtSisExtra_ValidFrom_Until->setGeometry(QRect(258, 63, 493, 16));
        QSizePolicy sizePolicy10(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy10.setHorizontalStretch(4);
        sizePolicy10.setVerticalStretch(0);
        sizePolicy10.setHeightForWidth(txtSisExtra_ValidFrom_Until->sizePolicy().hasHeightForWidth());
        txtSisExtra_ValidFrom_Until->setSizePolicy(sizePolicy10);
        txtSisExtra_ValidFrom_Until->setFrame(false);
        txtSisExtra_ValidFrom_Until->setReadOnly(true);
        tabWidget_Identity->addTab(tabSis_Back, QString());
        tabCardPin = new QWidget();
        tabCardPin->setObjectName(QString::fromUtf8("tabCardPin"));
        tabCardPin->setFocusPolicy(Qt::NoFocus);
        _2 = new QGridLayout(tabCardPin);
        _2->setObjectName(QString::fromUtf8("_2"));
        grbPIN_PINInfo = new QGroupBox(tabCardPin);
        grbPIN_PINInfo->setObjectName(QString::fromUtf8("grbPIN_PINInfo"));
        QSizePolicy sizePolicy11(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy11.setHorizontalStretch(0);
        sizePolicy11.setVerticalStretch(2);
        sizePolicy11.setHeightForWidth(grbPIN_PINInfo->sizePolicy().hasHeightForWidth());
        grbPIN_PINInfo->setSizePolicy(sizePolicy11);
        _4 = new QGridLayout(grbPIN_PINInfo);
        _4->setObjectName(QString::fromUtf8("_4"));
        _5 = new QVBoxLayout();
        _5->setObjectName(QString::fromUtf8("_5"));
        _6 = new QHBoxLayout();
        _6->setObjectName(QString::fromUtf8("_6"));

        _5->addLayout(_6);

        _7 = new QHBoxLayout();
        _7->setObjectName(QString::fromUtf8("_7"));

        _5->addLayout(_7);

        spacerItem = new QSpacerItem(431, 31, QSizePolicy::Minimum, QSizePolicy::Expanding);

        _5->addItem(spacerItem);

        _8 = new QHBoxLayout();
        _8->setObjectName(QString::fromUtf8("_8"));
        spacerItem1 = new QSpacerItem(251, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        _8->addItem(spacerItem1);


        _5->addLayout(_8);


        _4->addLayout(_5, 0, 0, 1, 1);


        _2->addWidget(grbPIN_PINInfo, 0, 0, 1, 1);

        tabWidget_Identity->addTab(tabCardPin, QString());
        tabCertificates = new QWidget();
        tabCertificates->setObjectName(QString::fromUtf8("tabCertificates"));
        tabCertificates->setFocusPolicy(Qt::NoFocus);
        _9 = new QGridLayout(tabCertificates);
        _9->setObjectName(QString::fromUtf8("_9"));
        _11 = new QHBoxLayout();
        _11->setSpacing(6);
        _11->setObjectName(QString::fromUtf8("_11"));
        spacerItem2 = new QSpacerItem(20, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        _11->addItem(spacerItem2);


        _9->addLayout(_11, 1, 0, 1, 1);

        tabWidget_Identity->addTab(tabCertificates, QString());
        tabPersoData = new QWidget();
        tabPersoData->setObjectName(QString::fromUtf8("tabPersoData"));
        tabWidget_Identity->addTab(tabPersoData, QString());
        tabInfo = new QWidget();
        tabInfo->setObjectName(QString::fromUtf8("tabInfo"));
        tabInfo->setFocusPolicy(Qt::NoFocus);
        _12 = new QGridLayout(tabInfo);
        _12->setObjectName(QString::fromUtf8("_12"));
        lblInfo_Img1 = new QLabel(tabInfo);
        lblInfo_Img1->setObjectName(QString::fromUtf8("lblInfo_Img1"));
        QSizePolicy sizePolicy12(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy12.setHorizontalStretch(1);
        sizePolicy12.setVerticalStretch(0);
        sizePolicy12.setHeightForWidth(lblInfo_Img1->sizePolicy().hasHeightForWidth());
        lblInfo_Img1->setSizePolicy(sizePolicy12);
        lblInfo_Img1->setPixmap(QPixmap(QString::fromUtf8(":/images/Images/a-z_identiteitskaart_logo.png")));
        lblInfo_Img1->setScaledContents(false);

        _12->addWidget(lblInfo_Img1, 0, 0, 1, 1);

        lblInfo_Top = new QLabel(tabInfo);
        lblInfo_Top->setObjectName(QString::fromUtf8("lblInfo_Top"));
        sizePolicy9.setHeightForWidth(lblInfo_Top->sizePolicy().hasHeightForWidth());
        lblInfo_Top->setSizePolicy(sizePolicy9);

        _12->addWidget(lblInfo_Top, 0, 1, 1, 1);

        tblInfo = new QTableWidget(tabInfo);
        tblInfo->setObjectName(QString::fromUtf8("tblInfo"));
        tblInfo->setSelectionMode(QAbstractItemView::NoSelection);
        tblInfo->setShowGrid(false);

        _12->addWidget(tblInfo, 1, 0, 1, 2);

        lblInfo_bottom = new QLabel(tabInfo);
        lblInfo_bottom->setObjectName(QString::fromUtf8("lblInfo_bottom"));
        lblInfo_bottom->setAlignment(Qt::AlignCenter);

        _12->addWidget(lblInfo_bottom, 2, 0, 1, 2);

        tabWidget_Identity->addTab(tabInfo, QString());
        stackedWidget->addWidget(page_Certificates);
        page_PINCodes = new QWidget();
        page_PINCodes->setObjectName(QString::fromUtf8("page_PINCodes"));
        tblCardAndPIN = new QTableWidget(page_PINCodes);
        tblCardAndPIN->setObjectName(QString::fromUtf8("tblCardAndPIN"));
        tblCardAndPIN->setGeometry(QRect(300, 60, 221, 101));
        tblCardAndPIN->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblCardAndPIN->setShowGrid(false);
        grbPIN_VersionInfo = new QGroupBox(page_PINCodes);
        grbPIN_VersionInfo->setObjectName(QString::fromUtf8("grbPIN_VersionInfo"));
        grbPIN_VersionInfo->setGeometry(QRect(300, 30, 161, 16));
        sizePolicy11.setHeightForWidth(grbPIN_VersionInfo->sizePolicy().hasHeightForWidth());
        grbPIN_VersionInfo->setSizePolicy(sizePolicy11);
        _3 = new QGridLayout(grbPIN_VersionInfo);
        _3->setObjectName(QString::fromUtf8("_3"));
        lblPIN_Name = new QLabel(page_PINCodes);
        lblPIN_Name->setObjectName(QString::fromUtf8("lblPIN_Name"));
        lblPIN_Name->setGeometry(QRect(540, 30, 151, 17));
        sizePolicy5.setHeightForWidth(lblPIN_Name->sizePolicy().hasHeightForWidth());
        lblPIN_Name->setSizePolicy(sizePolicy5);
        txtPIN_Name = new QLineEdit(page_PINCodes);
        txtPIN_Name->setObjectName(QString::fromUtf8("txtPIN_Name"));
        txtPIN_Name->setGeometry(QRect(540, 60, 141, 27));
        txtPIN_Name->setReadOnly(true);
        lblPIN_ID = new QLabel(page_PINCodes);
        lblPIN_ID->setObjectName(QString::fromUtf8("lblPIN_ID"));
        lblPIN_ID->setGeometry(QRect(690, 30, 131, 17));
        sizePolicy5.setHeightForWidth(lblPIN_ID->sizePolicy().hasHeightForWidth());
        lblPIN_ID->setSizePolicy(sizePolicy5);
        txtPIN_ID = new QLineEdit(page_PINCodes);
        txtPIN_ID->setObjectName(QString::fromUtf8("txtPIN_ID"));
        txtPIN_ID->setGeometry(QRect(690, 60, 191, 27));
        txtPIN_ID->setReadOnly(true);
        lblPIN_Status = new QLabel(page_PINCodes);
        lblPIN_Status->setObjectName(QString::fromUtf8("lblPIN_Status"));
        lblPIN_Status->setGeometry(QRect(540, 100, 231, 20));
        sizePolicy5.setHeightForWidth(lblPIN_Status->sizePolicy().hasHeightForWidth());
        lblPIN_Status->setSizePolicy(sizePolicy5);
        txtPIN_Status = new QLineEdit(page_PINCodes);
        txtPIN_Status->setObjectName(QString::fromUtf8("txtPIN_Status"));
        txtPIN_Status->setGeometry(QRect(540, 130, 231, 27));
        txtPIN_Status->setReadOnly(true);
        layoutWidget = new QWidget(page_PINCodes);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 30, 261, 301));
        _10 = new QGridLayout(layoutWidget);
        _10->setObjectName(QString::fromUtf8("_10"));
        _10->setContentsMargins(0, 0, 0, 0);
        spacerItem3 = new QSpacerItem(20, 108, QSizePolicy::Minimum, QSizePolicy::Expanding);

        _10->addItem(spacerItem3, 0, 1, 1, 1);

        lblCert_InfoAdd = new QLabel(layoutWidget);
        lblCert_InfoAdd->setObjectName(QString::fromUtf8("lblCert_InfoAdd"));
        sizePolicy5.setHeightForWidth(lblCert_InfoAdd->sizePolicy().hasHeightForWidth());
        lblCert_InfoAdd->setSizePolicy(sizePolicy5);
        lblCert_InfoAdd->setWordWrap(true);

        _10->addWidget(lblCert_InfoAdd, 1, 0, 1, 3);

        treePIN = new QTreeWidget(page_PINCodes);
        treePIN->setObjectName(QString::fromUtf8("treePIN"));
        treePIN->setGeometry(QRect(300, 180, 221, 131));
        lblPIN_InfoAdd = new QLabel(page_PINCodes);
        lblPIN_InfoAdd->setObjectName(QString::fromUtf8("lblPIN_InfoAdd"));
        lblPIN_InfoAdd->setGeometry(QRect(530, 170, 351, 91));
        sizePolicy5.setHeightForWidth(lblPIN_InfoAdd->sizePolicy().hasHeightForWidth());
        lblPIN_InfoAdd->setSizePolicy(sizePolicy5);
        lblPIN_InfoAdd->setWordWrap(true);
        btnPIN_Test = new QPushButton(page_PINCodes);
        btnPIN_Test->setObjectName(QString::fromUtf8("btnPIN_Test"));
        btnPIN_Test->setGeometry(QRect(540, 260, 85, 27));
        btnPIN_Test->setFont(font);
        btnPIN_Change = new QPushButton(page_PINCodes);
        btnPIN_Change->setObjectName(QString::fromUtf8("btnPIN_Change"));
        btnPIN_Change->setGeometry(QRect(630, 260, 94, 27));
        btnPIN_Change->setFont(font);
        btnCert_Register = new QPushButton(page_PINCodes);
        btnCert_Register->setObjectName(QString::fromUtf8("btnCert_Register"));
        btnCert_Register->setGeometry(QRect(20, 330, 85, 27));
        btnCert_Register->setFont(font);
        btnCert_Details = new QPushButton(page_PINCodes);
        btnCert_Details->setObjectName(QString::fromUtf8("btnCert_Details"));
        btnCert_Details->setGeometry(QRect(120, 330, 85, 27));
        btnCert_Details->setFont(font);
        stackedWidget->addWidget(page_PINCodes);
        page_Notes = new QWidget();
        page_Notes->setObjectName(QString::fromUtf8("page_Notes"));
        txtPersoData = new QPlainTextEdit(page_Notes);
        txtPersoData->setObjectName(QString::fromUtf8("txtPersoData"));
        txtPersoData->setGeometry(QRect(60, 10, 741, 371));
        btnPersoDataSave = new QPushButton(page_Notes);
        btnPersoDataSave->setObjectName(QString::fromUtf8("btnPersoDataSave"));
        btnPersoDataSave->setGeometry(QRect(720, 390, 97, 27));
        stackedWidget->addWidget(page_Notes);
        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setGeometry(QRect(10, 80, 1000, 38));
        widget_2->setMaximumSize(QSize(1000, 100));
        widget_2->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg_SelectTabTool.png);"));
        btnSelectTab_Identity = new QPushButton(widget_2);
        btnSelectTab_Identity->setObjectName(QString::fromUtf8("btnSelectTab_Identity"));
        btnSelectTab_Identity->setGeometry(QRect(0, -3, 145, 43));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(12);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(9);
        btnSelectTab_Identity->setFont(font1);
        btnSelectTab_Identity->setStyleSheet(QString::fromUtf8("color: rgb(108, 108, 108);\n"
"font: 75 12pt \"Arial\";\n"
"background-image: url(:/images/Images/alpha.png);"));
        btnSelectTab_Identity->setFlat(true);
        btnSelectTab_Identity_Extra = new QPushButton(widget_2);
        btnSelectTab_Identity_Extra->setObjectName(QString::fromUtf8("btnSelectTab_Identity_Extra"));
        btnSelectTab_Identity_Extra->setGeometry(QRect(145, -3, 145, 43));
        btnSelectTab_Identity_Extra->setStyleSheet(QString::fromUtf8("color: rgb(108, 108, 108);\n"
"font: 75 12pt \"Arial\";\n"
"background-image: url(:/images/Images/alpha.png);"));
        btnSelectTab_Identity_Extra->setFlat(true);
        btnSelectTab_Address = new QPushButton(widget_2);
        btnSelectTab_Address->setObjectName(QString::fromUtf8("btnSelectTab_Address"));
        btnSelectTab_Address->setGeometry(QRect(285, -3, 130, 43));
        btnSelectTab_Address->setStyleSheet(QString::fromUtf8("color: rgb(108, 108, 108);\n"
"font: 75 12pt \"Arial\";\n"
"background-image: url(:/images/Images/alpha.png);"));
        btnSelectTab_Address->setFlat(true);
        btnSelectTab_Certificates = new QPushButton(widget_2);
        btnSelectTab_Certificates->setObjectName(QString::fromUtf8("btnSelectTab_Certificates"));
        btnSelectTab_Certificates->setGeometry(QRect(420, -3, 130, 43));
        btnSelectTab_Certificates->setStyleSheet(QString::fromUtf8("color: rgb(108, 108, 108);\n"
"font: 75 12pt \"Arial\";\n"
"background-image: url(:/images/Images/alpha.png);"));
        btnSelectTab_Certificates->setFlat(true);
        btnSelectTab_PinCodes = new QPushButton(widget_2);
        btnSelectTab_PinCodes->setObjectName(QString::fromUtf8("btnSelectTab_PinCodes"));
        btnSelectTab_PinCodes->setGeometry(QRect(570, -3, 145, 43));
        btnSelectTab_PinCodes->setStyleSheet(QString::fromUtf8("color: rgb(108, 108, 108);\n"
"font: 75 12pt \"Arial\";\n"
"background-image: url(:/images/Images/alpha.png);"));
        btnSelectTab_PinCodes->setFlat(true);
        btnSelectTab_Notes = new QPushButton(widget_2);
        btnSelectTab_Notes->setObjectName(QString::fromUtf8("btnSelectTab_Notes"));
        btnSelectTab_Notes->setGeometry(QRect(720, -3, 135, 43));
        btnSelectTab_Notes->setStyleSheet(QString::fromUtf8("color: rgb(108, 108, 108);\n"
"font: 75 12pt \"Arial\";\n"
"background-image: url(:/images/Images/alpha.png);"));
        btnSelectTab_Notes->setFlat(true);
        widget_Toolbar = new QWidget(centralwidget);
        widget_Toolbar->setObjectName(QString::fromUtf8("widget_Toolbar"));
        widget_Toolbar->setGeometry(QRect(0, 0, 741, 61));
        widget_Toolbar->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/bg_Toolbar.png);"));
        button_menu_card = new QPushButton(widget_Toolbar);
        button_menu_card->setObjectName(QString::fromUtf8("button_menu_card"));
        button_menu_card->setGeometry(QRect(0, 0, 131, 61));
        button_menu_card->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/buttons/button_card.png);"));
        button_menu_card->setFlat(true);
        button_menu_tools = new QPushButton(widget_Toolbar);
        button_menu_tools->setObjectName(QString::fromUtf8("button_menu_tools"));
        button_menu_tools->setGeometry(QRect(140, 0, 151, 61));
        button_menu_tools->setStyleSheet(QString::fromUtf8("background-image: url(:/images/Images/buttons/button_tools.png);"));
        button_menu_tools->setFlat(true);
        MainWnd->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWnd);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1300, 25));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Tools = new QMenu(menubar);
        menu_Tools->setObjectName(QString::fromUtf8("menu_Tools"));
        menuLanguage = new QMenu(menubar);
        menuLanguage->setObjectName(QString::fromUtf8("menuLanguage"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWnd->setMenuBar(menubar);
        toolBar = new QToolBar(MainWnd);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setEnabled(true);
        toolBar->setMovable(false);
        toolBar->setFloatable(false);
        MainWnd->addToolBar(Qt::TopToolBarArea, toolBar);
        statusBar = new QStatusBar(MainWnd);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWnd->setStatusBar(statusBar);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_Tools->menuAction());
        menubar->addAction(menuLanguage->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menu_File->addAction(actionReload_eID);
        menu_File->addAction(actionClear);
        menu_File->addSeparator();
        menu_File->addAction(actionOpen_eID);
        menu_File->addAction(actionSave_eID);
        menu_File->addAction(actionSave_eID_as);
        menu_File->addSeparator();
        menu_File->addAction(actionPrint_eID);
        menu_File->addAction(actionPrinter_Settings);
        menu_File->addSeparator();
        menu_File->addAction(actionE_xit);
        menu_Tools->addSeparator();
        menu_Tools->addAction(actionOptions);
        menu_Tools->addSeparator();
        menu_Tools->addAction(actionZoom_In);
        menu_Tools->addAction(actionZoom_Out);
        menuHelp->addAction(actionAbout);
        toolBar->addAction(actionReload_eID);
        toolBar->addAction(actionClear);
        toolBar->addSeparator();
        toolBar->addAction(actionOpen_eID);
        toolBar->addAction(actionSave_eID);
        toolBar->addAction(actionPrint_eID);
        toolBar->addSeparator();
        toolBar->addAction(actionZoom_In);

        retranslateUi(MainWnd);

        stackedWidget->setCurrentIndex(2);
        tabWidget_Identity->setCurrentIndex(9);


        QMetaObject::connectSlotsByName(MainWnd);
    } // setupUi

    void retranslateUi(QMainWindow *MainWnd)
    {
        MainWnd->setWindowTitle(QApplication::translate("MainWnd", "PTeID-NG", 0, QApplication::UnicodeUTF8));
        actionE_xit->setText(QApplication::translate("MainWnd", "E&xit", 0, QApplication::UnicodeUTF8));
        actionReload_eID->setText(QApplication::translate("MainWnd", "&Reload eID", 0, QApplication::UnicodeUTF8));
        actionReload_eID->setIconText(QApplication::translate("MainWnd", "Reload eID", 0, QApplication::UnicodeUTF8));
        actionSave_eID->setText(QApplication::translate("MainWnd", "&Save eID", 0, QApplication::UnicodeUTF8));
        actionSave_eID->setShortcut(QApplication::translate("MainWnd", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionSave_eID_as->setText(QApplication::translate("MainWnd", "Save eID as...", 0, QApplication::UnicodeUTF8));
        actionOpen_eID->setText(QApplication::translate("MainWnd", "&Open eID", 0, QApplication::UnicodeUTF8));
        actionOpen_eID->setShortcut(QApplication::translate("MainWnd", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionClear->setText(QApplication::translate("MainWnd", "&Clear", 0, QApplication::UnicodeUTF8));
        actionPrint_eID->setText(QApplication::translate("MainWnd", "Print eID", 0, QApplication::UnicodeUTF8));
        actionPrint_eID->setShortcut(QApplication::translate("MainWnd", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionPrinter_Settings->setText(QApplication::translate("MainWnd", "Printer Settings", 0, QApplication::UnicodeUTF8));
        actionOptions->setText(QApplication::translate("MainWnd", "&Options", 0, QApplication::UnicodeUTF8));
        actionEnglish->setText(QApplication::translate("MainWnd", "&English", 0, QApplication::UnicodeUTF8));
        actionNederlands->setText(QApplication::translate("MainWnd", "&Portuguese", 0, QApplication::UnicodeUTF8));
        actionHelp->setText(QApplication::translate("MainWnd", "&Help / Manual", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWnd", "&About", 0, QApplication::UnicodeUTF8));
        actionShow_Toolbar->setText(QApplication::translate("MainWnd", "&Show Toolbar", 0, QApplication::UnicodeUTF8));
        actionPINRequest->setText(QApplication::translate("MainWnd", "Test PIN", 0, QApplication::UnicodeUTF8));
        actionPINChange->setText(QApplication::translate("MainWnd", "Change PIN", 0, QApplication::UnicodeUTF8));
        actionPINRenew->setText(QApplication::translate("MainWnd", "PIN renew (PUK)", 0, QApplication::UnicodeUTF8));
        actionZoom_In->setText(QApplication::translate("MainWnd", "Zoom &In", 0, QApplication::UnicodeUTF8));
        actionZoom_In->setShortcut(QApplication::translate("MainWnd", "+", 0, QApplication::UnicodeUTF8));
        actionZoom_Out->setText(QApplication::translate("MainWnd", "&Zoom Out", 0, QApplication::UnicodeUTF8));
        actionZoom_Out->setShortcut(QApplication::translate("MainWnd", "-", 0, QApplication::UnicodeUTF8));
        actionDisplay_Pinpad_Info->setText(QApplication::translate("MainWnd", "Display pinpad info", 0, QApplication::UnicodeUTF8));
        actionClose_Pinpad_Info->setText(QApplication::translate("MainWnd", "Close pinpad info", 0, QApplication::UnicodeUTF8));
        actionIdentity_Card->setText(QApplication::translate("MainWnd", "&Identity Card", 0, QApplication::UnicodeUTF8));
        actionSIS_Card->setText(QApplication::translate("MainWnd", "SIS Card", 0, QApplication::UnicodeUTF8));
        actionSIS_Card->setIconText(QApplication::translate("MainWnd", "SIS Card", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSIS_Card->setToolTip(QApplication::translate("MainWnd", "SIS Card", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionKids_Card->setText(QApplication::translate("MainWnd", "Kids Card", 0, QApplication::UnicodeUTF8));
        actionForeigner_Card->setText(QApplication::translate("MainWnd", "Foreigner Card", 0, QApplication::UnicodeUTF8));
        actionSave_PersoData->setText(QApplication::translate("MainWnd", "Save PersoData", 0, QApplication::UnicodeUTF8));
        radioButton->setText(QApplication::translate("MainWnd", "Initial Screen", 0, QApplication::UnicodeUTF8));
        lblIdentity_ImgPerson->setText(QString());
        txtIdentityExtra_SocialSecurityNo->setStyleSheet(QString());
        txtIdentityExtra_TaxNo->setStyleSheet(QString());
        txtIdentityExtra_CardVersion->setStyleSheet(QString());
        txtIdentityExtra_DocumentType->setStyleSheet(QString());
        txtIdentityExtra_LocalofRequest->setStyleSheet(QString());
        txtIdentityExtra_HealthNo->setStyleSheet(QString());
        txtIdentityExtra_IssuingEntity->setStyleSheet(QString());
        txtIdentityExtra_Validate->setStyleSheet(QString());
        txtSpecialStatus->setStyleSheet(QString());
        txtIdentityExtra_ChipNumber->setStyleSheet(QString());
        lblChip_Number->setStyleSheet(QString());
        lblChip_Number->setText(QApplication::translate("MainWnd", "Chip nummer / <i>Chip number</i>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_Card_Number->setStyleSheet(QString());
        txtIdentityExtra_ValidFrom_Until->setStyleSheet(QString());
        lblAddress_District->setStyleSheet(QString());
        lblAddress_District->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">National District</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_District->setStyleSheet(QString());
        lblAddress_Place->setStyleSheet(QString());
        lblAddress_Place->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Postcode<br /><span style=\" font-style:italic;\">Place</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_Place->setStyleSheet(QString());
        lblAddress_Municipality->setStyleSheet(QString());
        lblAddress_Municipality->setText(QApplication::translate("MainWnd", "Gemeente <br> <i>Municipality</i>", 0, QApplication::UnicodeUTF8));
        txtAddress_Municipality->setStyleSheet(QString());
        lblAddress_Zip3->setStyleSheet(QString());
        lblAddress_Zip3->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Postcode<br /><span style=\" font-style:italic;\">Zip3</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_Zip3->setStyleSheet(QString());
        lblAddress_PostalLocality->setStyleSheet(QString());
        lblAddress_PostalLocality->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">PostalLocality<br /><span style=\" font-style:italic;\">PostalLocality</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_PostalLocality->setStyleSheet(QString());
        lblAddress_CivilParish->setStyleSheet(QString());
        lblAddress_CivilParish->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Civil Parish</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_CivilParish->setStyleSheet(QString());
        lblAddress_StreetType1->setStyleSheet(QString());
        lblAddress_StreetType1->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Street Type 1</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_StreetType1->setStyleSheet(QString());
        lblAddress_StreetType2->setStyleSheet(QString());
        lblAddress_StreetType2->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Street Type2</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_StreetType2->setStyleSheet(QString());
        lblAddress_StreetName->setStyleSheet(QString());
        lblAddress_StreetName->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Street Name</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_StreetName->setStyleSheet(QString());
        lblAddress_BuildingType1->setStyleSheet(QString());
        lblAddress_BuildingType1->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br />Building Type1</p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_BuildingType1->setStyleSheet(QString());
        lblAddress_BuildingType2->setStyleSheet(QString());
        lblAddress_BuildingType2->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br />Building Type2</p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_BuildingType2->setStyleSheet(QString());
        lblAddress_DoorNo->setStyleSheet(QString());
        lblAddress_DoorNo->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br />Door No</p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_DoorNo->setStyleSheet(QString());
        lblAddress_Floor->setStyleSheet(QString());
        lblAddress_Floor->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br />Floor</p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_Floor->setStyleSheet(QString());
        lblAddress_Side->setStyleSheet(QString());
        lblAddress_Side->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Side</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_Side->setStyleSheet(QString());
        lblAddress_Locality->setStyleSheet(QString());
        lblAddress_Locality->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Locality</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_Locality->setStyleSheet(QString());
        lblAddress_Zip4->setStyleSheet(QString());
        lblAddress_Zip4->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Zip4</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_Zip4->setStyleSheet(QString());
        QTreeWidgetItem *___qtreewidgetitem = treeCert->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWnd", "Certificates Overview", 0, QApplication::UnicodeUTF8));
        lblCert_Owner->setText(QApplication::translate("MainWnd", "Owner", 0, QApplication::UnicodeUTF8));
        lblCert_Issuer->setText(QApplication::translate("MainWnd", "Issuer", 0, QApplication::UnicodeUTF8));
        lblCert_KeyLenght->setText(QApplication::translate("MainWnd", "Key Lenght", 0, QApplication::UnicodeUTF8));
        txtCert_KeyLenght->setStyleSheet(QString());
        lblCert_ValidFrom->setText(QApplication::translate("MainWnd", "Valid From", 0, QApplication::UnicodeUTF8));
        lblCert_ValidUntil->setText(QApplication::translate("MainWnd", "Valid Until", 0, QApplication::UnicodeUTF8));
        lblCert_Status->setText(QApplication::translate("MainWnd", "OCSP Status", 0, QApplication::UnicodeUTF8));
        btnOCSPCheck->setText(QApplication::translate("MainWnd", "OCSP check", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabWidget_Identity->setAccessibleName(QApplication::translate("MainWnd", "card contents", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        tabIdentity->setAccessibleName(QApplication::translate("MainWnd", "Tab Identity", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblIdentity_Head2_4->setText(QString());
        lblIdentity_Head1_3->setText(QString());
        lblIdentity_Head1_4->setText(QString());
        lblIdentity_Head2_3->setText(QString());
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabIdentity), QApplication::translate("MainWnd", "&Identity", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabForeigners->setAccessibleName(QApplication::translate("MainWnd", "Tab Foreigners", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        label->setText(QApplication::translate("MainWnd", "Name", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWnd", "Valid until", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWnd", "Issuing place and begin of validity", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWnd", "Type of title", 0, QApplication::UnicodeUTF8));
        lblForeigners_ImgPerson->setText(QString());
        lblForeigners_Footer_1->setText(QApplication::translate("MainWnd", "Belgische verblijfstitel - Titre de s\303\251jour belge", 0, QApplication::UnicodeUTF8));
        lblForeigners_Footer_2->setText(QApplication::translate("MainWnd", "Belgischer Aufenthaltstitel - Belgian residence permit", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabForeigners), QApplication::translate("MainWnd", "Foreigners", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabSis->setAccessibleName(QApplication::translate("MainWnd", "Tab Sis", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabSis), QApplication::translate("MainWnd", "Sis", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabIdentity_extention->setAccessibleName(QApplication::translate("MainWnd", "Tab Identity Extra", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabIdentity_extention), QApplication::translate("MainWnd", "Identity &Extra", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabAddress), QApplication::translate("MainWnd", "Address", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabForeigners_Back->setAccessibleName(QApplication::translate("MainWnd", "Tab Foreigners Extra", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        txtForeignersExtra_Nationality->setStyleSheet(QString());
        txtForeignersExtra_Sex->setStyleSheet(QString());
        txtForeignersExtra_Remarks1->setStyleSheet(QString());
        txtForeignersExtra_Remarks2->setStyleSheet(QString());
        lblForeignersExtra_Nationality->setStyleSheet(QString());
        lblForeignersExtra_Nationality->setText(QApplication::translate("MainWnd", "Nationality", 0, QApplication::UnicodeUTF8));
        lblForeignersExtra_Sex->setStyleSheet(QString());
        lblForeignersExtra_Sex->setText(QApplication::translate("MainWnd", "Sex", 0, QApplication::UnicodeUTF8));
        lblForeignersExtra_Remarks1->setStyleSheet(QString());
        lblForeignersExtra_Remarks1->setText(QApplication::translate("MainWnd", "Remarks", 0, QApplication::UnicodeUTF8));
        lblForeignersExtra_ChipNumber->setStyleSheet(QString());
        lblForeignersExtra_ChipNumber->setText(QApplication::translate("MainWnd", "Chip number", 0, QApplication::UnicodeUTF8));
        txtForeignersExtra_BirthDate->setStyleSheet(QString());
        lblForeignersExtra_NationalNumber->setStyleSheet(QString());
        lblForeignersExtra_NationalNumber->setText(QApplication::translate("MainWnd", "National Number", 0, QApplication::UnicodeUTF8));
        txtForeignersExtra_NationalNumber->setStyleSheet(QString());
        lblForeignersExtra_BirthDate->setStyleSheet(QString());
        lblForeignersExtra_BirthDate->setText(QApplication::translate("MainWnd", "Date and place of birth", 0, QApplication::UnicodeUTF8));
        txtForeignersExtra_Remarks3->setStyleSheet(QString());
        txtForeignerSpecialStatus->setStyleSheet(QString());
        lblForeignersExtra_street->setText(QApplication::translate("MainWnd", "Street", 0, QApplication::UnicodeUTF8));
        lblForeignersExtra_PostalCode->setText(QApplication::translate("MainWnd", "Postal code", 0, QApplication::UnicodeUTF8));
        lblForeignersExtra_municipality->setText(QApplication::translate("MainWnd", "Municipality", 0, QApplication::UnicodeUTF8));
        lblForeignersExtra_country->setText(QApplication::translate("MainWnd", "Country", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabForeigners_Back), QApplication::translate("MainWnd", "Foreigners Extra", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabSis_Back->setAccessibleName(QApplication::translate("MainWnd", "Tab Sis Extra", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblSisExtra_Card_ValidFrom_Until->setText(QApplication::translate("MainWnd", "Geldig van - tot / <i>Valid from - until</i>", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabSis_Back), QApplication::translate("MainWnd", "Sis Extra", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabCardPin->setAccessibleName(QApplication::translate("MainWnd", "Tab Card and Pin", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        grbPIN_PINInfo->setStyleSheet(QString());
        grbPIN_PINInfo->setTitle(QApplication::translate("MainWnd", "PIN Info", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabCardPin), QApplication::translate("MainWnd", "Card && &PIN", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabCertificates->setAccessibleName(QApplication::translate("MainWnd", "Tab Certificates", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabCertificates), QApplication::translate("MainWnd", "&Certificates", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabPersoData), QApplication::translate("MainWnd", "Bloco de Notas", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabInfo->setAccessibleName(QApplication::translate("MainWnd", "Tab Info", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblInfo_Img1->setText(QString());
        lblInfo_Top->setText(QApplication::translate("MainWnd", "Portuguese eID Runtime", 0, QApplication::UnicodeUTF8));
        lblInfo_bottom->setText(QApplication::translate("MainWnd", "Developed by the Portuguese Government", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabInfo), QApplication::translate("MainWnd", "In&fo", 0, QApplication::UnicodeUTF8));
        grbPIN_VersionInfo->setStyleSheet(QString());
        grbPIN_VersionInfo->setTitle(QApplication::translate("MainWnd", "Version Info", 0, QApplication::UnicodeUTF8));
        lblPIN_Name->setText(QApplication::translate("MainWnd", "PIN  Name", 0, QApplication::UnicodeUTF8));
        lblPIN_ID->setText(QApplication::translate("MainWnd", "PIN  ID", 0, QApplication::UnicodeUTF8));
        lblPIN_Status->setText(QApplication::translate("MainWnd", "PIN  Status", 0, QApplication::UnicodeUTF8));
        lblCert_InfoAdd->setText(QApplication::translate("MainWnd", "To use your identity card with applications like Internet Explorer, Microsoft Offfice,OpenOffice.org, Adobe Reader, Outlook, ... you first have to register the card in Windows. Select a certificate and click the \"Register\" button to register your certificates", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem1 = treePIN->headerItem();
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWnd", "PINs", 0, QApplication::UnicodeUTF8));
        lblPIN_InfoAdd->setText(QApplication::translate("MainWnd", "When you give a wrong PIN number 3 times, the connected services will be blocked. If so please contact your local goverment inctance. Press \"Cancel\" if you are unscertain.", 0, QApplication::UnicodeUTF8));
        btnPIN_Test->setText(QApplication::translate("MainWnd", "Test PIN", 0, QApplication::UnicodeUTF8));
        btnPIN_Change->setText(QApplication::translate("MainWnd", "Change PIN", 0, QApplication::UnicodeUTF8));
        btnCert_Register->setText(QApplication::translate("MainWnd", "&Register", 0, QApplication::UnicodeUTF8));
        btnCert_Details->setText(QApplication::translate("MainWnd", "&Details >>", 0, QApplication::UnicodeUTF8));
        btnPersoDataSave->setText(QApplication::translate("MainWnd", "Save", 0, QApplication::UnicodeUTF8));
        btnSelectTab_Identity->setText(QApplication::translate("MainWnd", "Identifica\303\247\303\243o Civil", 0, QApplication::UnicodeUTF8));
        btnSelectTab_Identity_Extra->setText(QApplication::translate("MainWnd", "Outros Dados", 0, QApplication::UnicodeUTF8));
        btnSelectTab_Address->setText(QApplication::translate("MainWnd", "Morada", 0, QApplication::UnicodeUTF8));
        btnSelectTab_Certificates->setText(QApplication::translate("MainWnd", "Certificados", 0, QApplication::UnicodeUTF8));
        btnSelectTab_PinCodes->setText(QApplication::translate("MainWnd", "C\303\263digos PIN", 0, QApplication::UnicodeUTF8));
        btnSelectTab_Notes->setText(QApplication::translate("MainWnd", "Notes", 0, QApplication::UnicodeUTF8));
        button_menu_card->setText(QString());
        button_menu_tools->setText(QString());
        menu_File->setTitle(QApplication::translate("MainWnd", "&Card", 0, QApplication::UnicodeUTF8));
        menu_Tools->setTitle(QApplication::translate("MainWnd", "&Tools", 0, QApplication::UnicodeUTF8));
        menuLanguage->setTitle(QApplication::translate("MainWnd", "Language", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWnd", "Help", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWnd", "toolBar", 0, QApplication::UnicodeUTF8));
        toolBar->setStyleSheet(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWnd: public Ui_MainWnd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWND_H
