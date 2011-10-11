/********************************************************************************
** Form generated from reading ui file 'mainwnd.ui'
**
** Created: Sun 31. Jul 19:32:55 2011
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
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
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *vboxLayout;
    QWidget *SplashWidget;
    QGridLayout *gridLayout1;
    QSpacerItem *spacerItem;
    QSpacerItem *spacerItem1;
    QLabel *lblSplashImg_4;
    QSpacerItem *spacerItem2;
    QSpacerItem *spacerItem3;
    QWidget *page_2;
    QVBoxLayout *vboxLayout1;
    QTabWidget *tabWidget_Identity;
    QWidget *tabIdentity;
    QLabel *lblIdentity_Head2_2;
    QLabel *lblIdentity_Head2_4;
    QLabel *lblIdentity_Head2_1;
    QLabel *lblIdentity_Head1_2;
    QLabel *lblIdentity_Head1_3;
    QLabel *lblIdentity_Head1_4;
    QLabel *lblIdentity_Head2_3;
    QLineEdit *txtIdentity_Name;
    QLabel *lblIdentity_GivenNames;
    QLineEdit *txtIdentity_GivenNames;
    QLabel *lblIdentity_BirthDate;
    QLineEdit *txtIdentity_BirthDate;
    QLabel *lblIdentity_Height;
    QLineEdit *txtIdentity_Height;
    QLabel *lblIdentity_ValidFrom_Until;
    QLineEdit *txtIdentity_ValidFrom_Until;
    QLabel *lblIdentity_Parents;
    QLabel *lblIdentity_Head1_1;
    QLineEdit *txtIdentity_Nationality;
    QLabel *lblIdentity_Sex;
    QLineEdit *txtIdentity_Sex;
    QLabel *lblIdentity_Name;
    QLabel *lblIdentity_Nationality;
    QLabel *lblIdentity_ImgPerson;
    QLabel *lblIdentity_DocumentNumber;
    QLineEdit *txtIdentity_DocumentNumber;
    QLineEdit *txtIdentity_Parents;
    QLineEdit *txtIdentity_Country;
    QLabel *lblIdentity_Country;
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
    QLabel *lblIdentity_SocialSecurityNo;
    QLineEdit *txtIdentityExtra_SocialSecurityNo;
    QLabel *lblIdentity_Remarks;
    QLineEdit *txtIdentityExtra_Remarks1;
    QLineEdit *txtIdentityExtra_Remarks2;
    QLineEdit *txtIdentityExtra_Remarks3;
    QLineEdit *txtIdentityExtra_TaxNo;
    QLabel *lblIdentity_TaxNo;
    QLabel *lblChip_Number;
    QLineEdit *txtIdentityExtra_ChipNumber;
    QLineEdit *txtIdentityExtra_Card_Number;
    QLabel *lblCard_Number;
    QLineEdit *txtIdentityExtra_ValidFrom_Until;
    QLabel *lblCard_ValidFrom_Until;
    QLineEdit *txtIdentityExtra_HealthNo;
    QLabel *lblIdentity_HeathNo;
    QLabel *lblIdentity_Title;
    QLineEdit *txtIdentityExtra_Title;
    QLineEdit *txtIdentityExtra_CardVersion;
    QLabel *lblIdentity_CardVersion;
    QLabel *lblIdentity_DocumentType;
    QLineEdit *txtIdentityExtra_DocumentType;
    QLineEdit *txtIdentityExtra_IssuingEntity;
    QLabel *lblIdentity_IssuingEntity;
    QLineEdit *txtSpecialStatus;
    QLabel *lblSpecialStatus;
    QLabel *lblIdentity_PlaceOfIssue;
    QLineEdit *txtIdentityExtra_PlaceOfIssue;
    QWidget *tabAddress;
    QLabel *lblAddress_StreetType2;
    QLineEdit *txtAddress_StreetType2;
    QLabel *lblAddress_Municipality;
    QLineEdit *txtAddress_Municipality;
    QLineEdit *txtAddress_CivilParish;
    QLabel *lblAddress_CivilParish;
    QLineEdit *txtAddress_District;
    QLabel *lblAddress_District;
    QLineEdit *txtAddress_StreetName;
    QLabel *lblAddress_StreetName;
    QLineEdit *txtAddress_BuildingType2;
    QLabel *lblAddress_BuildingType2;
    QLineEdit *txtAddress_DoorNo;
    QLabel *lblAddress_DoorNo;
    QLineEdit *txtAddress_Side;
    QLabel *lblAddress_Side;
    QLabel *lblAddress_Locality;
    QLineEdit *txtAddress_Locality;
    QLabel *lblAddress_Zip4;
    QLineEdit *txtAddress_Zip4;
    QLineEdit *txtAddress_Floor;
    QLabel *lblAddress_Floor;
    QLineEdit *txtAddress_BuildingType1;
    QLabel *lblAddress_BuildingType1;
    QLabel *lblAddress_StreetType1;
    QLineEdit *txtAddress_StreetType1;
    QLineEdit *txtAddress_Zip3;
    QLabel *lblAddress_Zip3;
    QLabel *lblAddress_Place;
    QLineEdit *txtAddress_Place;
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
    QGridLayout *gridLayout2;
    QGroupBox *grbPIN_VersionInfo;
    QGridLayout *gridLayout3;
    QTableWidget *tblCardAndPIN;
    QGroupBox *grbPIN_PINInfo;
    QGridLayout *gridLayout4;
    QTreeWidget *treePIN;
    QVBoxLayout *vboxLayout2;
    QHBoxLayout *hboxLayout;
    QLabel *lblPIN_Name;
    QLabel *lblPIN_ID;
    QHBoxLayout *hboxLayout1;
    QLineEdit *txtPIN_Name;
    QLineEdit *txtPIN_ID;
    QLabel *lblPIN_Status;
    QLineEdit *txtPIN_Status;
    QSpacerItem *spacerItem4;
    QLabel *lblPIN_InfoAdd;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem5;
    QPushButton *btnPIN_Test;
    QPushButton *btnPIN_Change;
    QWidget *tabCertificates;
    QGridLayout *gridLayout5;
    QTreeWidget *treeCert;
    QGridLayout *gridLayout6;
    QLabel *lblCert_Owner;
    QLineEdit *txtCert_Owner;
    QLabel *lblCert_Issuer;
    QLineEdit *txtCert_Issuer;
    QLabel *lblCert_KeyLenght;
    QLineEdit *txtCert_KeyLenght;
    QLabel *lblCert_ValidFrom;
    QLabel *lblCert_ValidUntil;
    QLineEdit *txtCert_ValidFrom;
    QLineEdit *txtCert_ValidUntil;
    QLabel *lblCert_Status;
    QLineEdit *txtCert_Status;
    QPushButton *btnOCSPCheck;
    QSpacerItem *spacerItem6;
    QLabel *lblCert_InfoAdd;
    QHBoxLayout *hboxLayout3;
    QSpacerItem *spacerItem7;
    QPushButton *btnCert_Register;
    QPushButton *btnCert_Details;
    QWidget *tabPersoData;
    QPlainTextEdit *txtPersoData;
    QWidget *tabInfo;
    QGridLayout *gridLayout7;
    QLabel *lblInfo_Img1;
    QLabel *lblInfo_Top;
    QTableWidget *tblInfo;
    QLabel *lblInfo_bottom;
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
        MainWnd->resize(800, 695);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWnd->sizePolicy().hasHeightForWidth());
        MainWnd->setSizePolicy(sizePolicy);
        MainWnd->setAutoFillBackground(true);
        actionE_xit = new QAction(MainWnd);
        actionE_xit->setObjectName(QString::fromUtf8("actionE_xit"));
        QIcon icon;
        icon.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Exit.png")), QIcon::Normal, QIcon::Off);
        actionE_xit->setIcon(icon);
        actionReload_eID = new QAction(MainWnd);
        actionReload_eID->setObjectName(QString::fromUtf8("actionReload_eID"));
        QIcon icon1;
        icon1.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Chip.png")), QIcon::Normal, QIcon::Off);
        actionReload_eID->setIcon(icon1);
        actionSave_eID = new QAction(MainWnd);
        actionSave_eID->setObjectName(QString::fromUtf8("actionSave_eID"));
        QIcon icon2;
        icon2.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Save.png")), QIcon::Normal, QIcon::Off);
        actionSave_eID->setIcon(icon2);
        actionSave_eID_as = new QAction(MainWnd);
        actionSave_eID_as->setObjectName(QString::fromUtf8("actionSave_eID_as"));
        QIcon icon3;
        icon3.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/save_as.png")), QIcon::Normal, QIcon::Off);
        actionSave_eID_as->setIcon(icon3);
        actionOpen_eID = new QAction(MainWnd);
        actionOpen_eID->setObjectName(QString::fromUtf8("actionOpen_eID"));
        QIcon icon4;
        icon4.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Open.png")), QIcon::Normal, QIcon::Off);
        actionOpen_eID->setIcon(icon4);
        actionClear = new QAction(MainWnd);
        actionClear->setObjectName(QString::fromUtf8("actionClear"));
        QIcon icon5;
        icon5.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Clear.png")), QIcon::Normal, QIcon::Off);
        actionClear->setIcon(icon5);
        actionPrint_eID = new QAction(MainWnd);
        actionPrint_eID->setObjectName(QString::fromUtf8("actionPrint_eID"));
        QIcon icon6;
        icon6.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Print.png")), QIcon::Normal, QIcon::Off);
        actionPrint_eID->setIcon(icon6);
        actionPrinter_Settings = new QAction(MainWnd);
        actionPrinter_Settings->setObjectName(QString::fromUtf8("actionPrinter_Settings"));
        actionPrinter_Settings->setVisible(false);
        actionOptions = new QAction(MainWnd);
        actionOptions->setObjectName(QString::fromUtf8("actionOptions"));
        QIcon icon7;
        icon7.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Options.png")), QIcon::Normal, QIcon::Off);
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
        icon8.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Help.png")), QIcon::Normal, QIcon::Off);
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
        icon9.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Zoom_IN.png")), QIcon::Normal, QIcon::Off);
        actionZoom_In->setIcon(icon9);
        actionZoom_In->setVisible(true);
        actionZoom_Out = new QAction(MainWnd);
        actionZoom_Out->setObjectName(QString::fromUtf8("actionZoom_Out"));
        QIcon icon10;
        icon10.addPixmap(QPixmap(QString::fromUtf8(":/images/Images/Icons/Zoom_OUT.png")), QIcon::Normal, QIcon::Off);
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
        centralwidget = new QWidget(MainWnd);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        vboxLayout = new QVBoxLayout(page);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        SplashWidget = new QWidget(page);
        SplashWidget->setObjectName(QString::fromUtf8("SplashWidget"));
        gridLayout1 = new QGridLayout(SplashWidget);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout1->addItem(spacerItem, 0, 1, 1, 1);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem1, 1, 0, 1, 1);

        lblSplashImg_4 = new QLabel(SplashWidget);
        lblSplashImg_4->setObjectName(QString::fromUtf8("lblSplashImg_4"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lblSplashImg_4->sizePolicy().hasHeightForWidth());
        lblSplashImg_4->setSizePolicy(sizePolicy1);
        lblSplashImg_4->setMinimumSize(QSize(128, 92));
        lblSplashImg_4->setFrameShape(QFrame::NoFrame);
        lblSplashImg_4->setPixmap(QPixmap(QString::fromUtf8(":/images/Images/a-z_identiteitskaart_logo.png")));
        lblSplashImg_4->setAlignment(Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout1->addWidget(lblSplashImg_4, 1, 1, 1, 1);

        spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem2, 1, 2, 1, 1);

        spacerItem3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout1->addItem(spacerItem3, 2, 1, 1, 1);


        vboxLayout->addWidget(SplashWidget);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        vboxLayout1 = new QVBoxLayout(page_2);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        tabWidget_Identity = new QTabWidget(page_2);
        tabWidget_Identity->setObjectName(QString::fromUtf8("tabWidget_Identity"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(tabWidget_Identity->sizePolicy().hasHeightForWidth());
        tabWidget_Identity->setSizePolicy(sizePolicy2);
        tabWidget_Identity->setSizeIncrement(QSize(0, 0));
        QFont font;
        tabWidget_Identity->setFont(font);
        tabWidget_Identity->setFocusPolicy(Qt::StrongFocus);
        tabWidget_Identity->setTabShape(QTabWidget::Rounded);
        tabIdentity = new QWidget();
        tabIdentity->setObjectName(QString::fromUtf8("tabIdentity"));
        tabIdentity->setFocusPolicy(Qt::NoFocus);
        lblIdentity_Head2_2 = new QLabel(tabIdentity);
        lblIdentity_Head2_2->setObjectName(QString::fromUtf8("lblIdentity_Head2_2"));
        lblIdentity_Head2_2->setGeometry(QRect(197, 32, 180, 16));
        lblIdentity_Head2_2->setFont(font);
        lblIdentity_Head2_2->setTextFormat(Qt::PlainText);
        lblIdentity_Head2_4 = new QLabel(tabIdentity);
        lblIdentity_Head2_4->setObjectName(QString::fromUtf8("lblIdentity_Head2_4"));
        lblIdentity_Head2_4->setGeometry(QRect(570, 32, 180, 16));
        lblIdentity_Head2_4->setFont(font);
        lblIdentity_Head2_4->setTextFormat(Qt::PlainText);
        lblIdentity_Head2_1 = new QLabel(tabIdentity);
        lblIdentity_Head2_1->setObjectName(QString::fromUtf8("lblIdentity_Head2_1"));
        lblIdentity_Head2_1->setGeometry(QRect(10, 32, 181, 16));
        lblIdentity_Head2_1->setFont(font);
        lblIdentity_Head2_1->setTextFormat(Qt::PlainText);
        lblIdentity_Head1_2 = new QLabel(tabIdentity);
        lblIdentity_Head1_2->setObjectName(QString::fromUtf8("lblIdentity_Head1_2"));
        lblIdentity_Head1_2->setGeometry(QRect(200, 0, 180, 30));
        lblIdentity_Head1_2->setFont(font);
        lblIdentity_Head1_2->setTextFormat(Qt::PlainText);
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
        txtIdentity_Name = new QLineEdit(tabIdentity);
        txtIdentity_Name->setObjectName(QString::fromUtf8("txtIdentity_Name"));
        txtIdentity_Name->setGeometry(QRect(222, 79, 529, 16));
        txtIdentity_Name->setFont(font);
        txtIdentity_Name->setFrame(false);
        txtIdentity_Name->setReadOnly(true);
        lblIdentity_GivenNames = new QLabel(tabIdentity);
        lblIdentity_GivenNames->setObjectName(QString::fromUtf8("lblIdentity_GivenNames"));
        lblIdentity_GivenNames->setGeometry(QRect(9, 101, 207, 16));
        lblIdentity_GivenNames->setFont(font);
        txtIdentity_GivenNames = new QLineEdit(tabIdentity);
        txtIdentity_GivenNames->setObjectName(QString::fromUtf8("txtIdentity_GivenNames"));
        txtIdentity_GivenNames->setGeometry(QRect(222, 101, 529, 16));
        txtIdentity_GivenNames->setFont(font);
        txtIdentity_GivenNames->setFrame(false);
        txtIdentity_GivenNames->setAlignment(Qt::AlignLeading);
        txtIdentity_GivenNames->setReadOnly(true);
        lblIdentity_BirthDate = new QLabel(tabIdentity);
        lblIdentity_BirthDate->setObjectName(QString::fromUtf8("lblIdentity_BirthDate"));
        lblIdentity_BirthDate->setGeometry(QRect(222, 123, 417, 20));
        lblIdentity_BirthDate->setFont(font);
        txtIdentity_BirthDate = new QLineEdit(tabIdentity);
        txtIdentity_BirthDate->setObjectName(QString::fromUtf8("txtIdentity_BirthDate"));
        txtIdentity_BirthDate->setGeometry(QRect(222, 151, 417, 16));
        txtIdentity_BirthDate->setFont(font);
        txtIdentity_BirthDate->setFrame(false);
        txtIdentity_BirthDate->setReadOnly(true);
        lblIdentity_Height = new QLabel(tabIdentity);
        lblIdentity_Height->setObjectName(QString::fromUtf8("lblIdentity_Height"));
        lblIdentity_Height->setGeometry(QRect(222, 210, 341, 20));
        lblIdentity_Height->setFont(font);
        txtIdentity_Height = new QLineEdit(tabIdentity);
        txtIdentity_Height->setObjectName(QString::fromUtf8("txtIdentity_Height"));
        txtIdentity_Height->setGeometry(QRect(222, 230, 341, 20));
        txtIdentity_Height->setFont(font);
        txtIdentity_Height->setFrame(false);
        txtIdentity_Height->setReadOnly(true);
        lblIdentity_ValidFrom_Until = new QLabel(tabIdentity);
        lblIdentity_ValidFrom_Until->setObjectName(QString::fromUtf8("lblIdentity_ValidFrom_Until"));
        lblIdentity_ValidFrom_Until->setGeometry(QRect(10, 347, 240, 16));
        txtIdentity_ValidFrom_Until = new QLineEdit(tabIdentity);
        txtIdentity_ValidFrom_Until->setObjectName(QString::fromUtf8("txtIdentity_ValidFrom_Until"));
        txtIdentity_ValidFrom_Until->setGeometry(QRect(10, 367, 240, 16));
        txtIdentity_ValidFrom_Until->setMinimumSize(QSize(240, 0));
        txtIdentity_ValidFrom_Until->setFrame(false);
        txtIdentity_ValidFrom_Until->setReadOnly(true);
        lblIdentity_Parents = new QLabel(tabIdentity);
        lblIdentity_Parents->setObjectName(QString::fromUtf8("lblIdentity_Parents"));
        lblIdentity_Parents->setGeometry(QRect(350, 347, 240, 16));
        lblIdentity_Head1_1 = new QLabel(tabIdentity);
        lblIdentity_Head1_1->setObjectName(QString::fromUtf8("lblIdentity_Head1_1"));
        lblIdentity_Head1_1->setGeometry(QRect(10, 0, 181, 30));
        lblIdentity_Head1_1->setFont(font);
        lblIdentity_Head1_1->setTextFormat(Qt::PlainText);
        txtIdentity_Nationality = new QLineEdit(tabIdentity);
        txtIdentity_Nationality->setObjectName(QString::fromUtf8("txtIdentity_Nationality"));
        txtIdentity_Nationality->setGeometry(QRect(290, 170, 270, 28));
        txtIdentity_Nationality->setFont(font);
        txtIdentity_Nationality->setFrame(false);
        txtIdentity_Nationality->setReadOnly(true);
        lblIdentity_Sex = new QLabel(tabIdentity);
        lblIdentity_Sex->setObjectName(QString::fromUtf8("lblIdentity_Sex"));
        lblIdentity_Sex->setGeometry(QRect(650, 120, 101, 20));
        lblIdentity_Sex->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        txtIdentity_Sex = new QLineEdit(tabIdentity);
        txtIdentity_Sex->setObjectName(QString::fromUtf8("txtIdentity_Sex"));
        txtIdentity_Sex->setGeometry(QRect(645, 151, 106, 16));
        txtIdentity_Sex->setFrame(false);
        txtIdentity_Sex->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        txtIdentity_Sex->setReadOnly(true);
        lblIdentity_Name = new QLabel(tabIdentity);
        lblIdentity_Name->setObjectName(QString::fromUtf8("lblIdentity_Name"));
        lblIdentity_Name->setGeometry(QRect(9, 79, 207, 16));
        lblIdentity_Name->setFont(font);
        lblIdentity_Name->setFrameShape(QFrame::NoFrame);
        lblIdentity_Nationality = new QLabel(tabIdentity);
        lblIdentity_Nationality->setObjectName(QString::fromUtf8("lblIdentity_Nationality"));
        lblIdentity_Nationality->setGeometry(QRect(222, 175, 253, 28));
        lblIdentity_Nationality->setFont(font);
        lblIdentity_Nationality->setWordWrap(true);
        lblIdentity_ImgPerson = new QLabel(tabIdentity);
        lblIdentity_ImgPerson->setObjectName(QString::fromUtf8("lblIdentity_ImgPerson"));
        lblIdentity_ImgPerson->setEnabled(true);
        lblIdentity_ImgPerson->setGeometry(QRect(30, 130, 50, 51));
        QSizePolicy sizePolicy3(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(lblIdentity_ImgPerson->sizePolicy().hasHeightForWidth());
        lblIdentity_ImgPerson->setSizePolicy(sizePolicy3);
        lblIdentity_ImgPerson->setMinimumSize(QSize(50, 50));
        lblIdentity_ImgPerson->setMaximumSize(QSize(400, 400));
        lblIdentity_ImgPerson->setAutoFillBackground(false);
        lblIdentity_ImgPerson->setFrameShape(QFrame::NoFrame);
        lblIdentity_ImgPerson->setScaledContents(true);
        lblIdentity_ImgPerson->setAlignment(Qt::AlignCenter);
        lblIdentity_DocumentNumber = new QLabel(tabIdentity);
        lblIdentity_DocumentNumber->setObjectName(QString::fromUtf8("lblIdentity_DocumentNumber"));
        lblIdentity_DocumentNumber->setGeometry(QRect(222, 260, 341, 20));
        lblIdentity_DocumentNumber->setFont(font);
        txtIdentity_DocumentNumber = new QLineEdit(tabIdentity);
        txtIdentity_DocumentNumber->setObjectName(QString::fromUtf8("txtIdentity_DocumentNumber"));
        txtIdentity_DocumentNumber->setGeometry(QRect(222, 290, 341, 20));
        txtIdentity_DocumentNumber->setFont(font);
        txtIdentity_DocumentNumber->setFrame(false);
        txtIdentity_DocumentNumber->setReadOnly(true);
        txtIdentity_Parents = new QLineEdit(tabIdentity);
        txtIdentity_Parents->setObjectName(QString::fromUtf8("txtIdentity_Parents"));
        txtIdentity_Parents->setGeometry(QRect(350, 367, 240, 16));
        txtIdentity_Parents->setMinimumSize(QSize(240, 0));
        txtIdentity_Parents->setFrame(false);
        txtIdentity_Parents->setReadOnly(true);
        txtIdentity_Country = new QLineEdit(tabIdentity);
        txtIdentity_Country->setObjectName(QString::fromUtf8("txtIdentity_Country"));
        txtIdentity_Country->setGeometry(QRect(570, 204, 176, 16));
        sizePolicy.setHeightForWidth(txtIdentity_Country->sizePolicy().hasHeightForWidth());
        txtIdentity_Country->setSizePolicy(sizePolicy);
        txtIdentity_Country->setFrame(false);
        txtIdentity_Country->setReadOnly(true);
        lblIdentity_Country = new QLabel(tabIdentity);
        lblIdentity_Country->setObjectName(QString::fromUtf8("lblIdentity_Country"));
        lblIdentity_Country->setGeometry(QRect(570, 170, 176, 28));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(lblIdentity_Country->sizePolicy().hasHeightForWidth());
        lblIdentity_Country->setSizePolicy(sizePolicy4);
        tabWidget_Identity->addTab(tabIdentity, QString());
        tabForeigners = new QWidget();
        tabForeigners->setObjectName(QString::fromUtf8("tabForeigners"));
        tabForeigners->setFocusPolicy(Qt::NoFocus);
        txtForeigners_Card_Number = new QLineEdit(tabForeigners);
        txtForeigners_Card_Number->setObjectName(QString::fromUtf8("txtForeigners_Card_Number"));
        txtForeigners_Card_Number->setGeometry(QRect(463, 11, 288, 16));
        sizePolicy.setHeightForWidth(txtForeigners_Card_Number->sizePolicy().hasHeightForWidth());
        txtForeigners_Card_Number->setSizePolicy(sizePolicy);
        txtForeigners_Card_Number->setFont(font);
        txtForeigners_Card_Number->setFrame(false);
        txtForeigners_Card_Number->setReadOnly(true);
        label = new QLabel(tabForeigners);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(37, 61, 176, 20));
        txtForeigners_Name = new QLineEdit(tabForeigners);
        txtForeigners_Name->setObjectName(QString::fromUtf8("txtForeigners_Name"));
        txtForeigners_Name->setGeometry(QRect(219, 63, 532, 16));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(1);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(txtForeigners_Name->sizePolicy().hasHeightForWidth());
        txtForeigners_Name->setSizePolicy(sizePolicy5);
        txtForeigners_Name->setFrame(false);
        txtForeigners_Name->setReadOnly(true);
        txtForeigners_GivenNames = new QLineEdit(tabForeigners);
        txtForeigners_GivenNames->setObjectName(QString::fromUtf8("txtForeigners_GivenNames"));
        txtForeigners_GivenNames->setGeometry(QRect(219, 89, 532, 16));
        sizePolicy.setHeightForWidth(txtForeigners_GivenNames->sizePolicy().hasHeightForWidth());
        txtForeigners_GivenNames->setSizePolicy(sizePolicy);
        txtForeigners_GivenNames->setFrame(false);
        txtForeigners_GivenNames->setAlignment(Qt::AlignLeading);
        txtForeigners_GivenNames->setReadOnly(true);
        label_2 = new QLabel(tabForeigners);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(37, 113, 176, 20));
        txtForeigners_ValidTot = new QLineEdit(tabForeigners);
        txtForeigners_ValidTot->setObjectName(QString::fromUtf8("txtForeigners_ValidTot"));
        txtForeigners_ValidTot->setGeometry(QRect(219, 115, 532, 16));
        sizePolicy.setHeightForWidth(txtForeigners_ValidTot->sizePolicy().hasHeightForWidth());
        txtForeigners_ValidTot->setSizePolicy(sizePolicy);
        txtForeigners_ValidTot->setFrame(false);
        txtForeigners_ValidTot->setReadOnly(true);
        label_3 = new QLabel(tabForeigners);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(37, 139, 211, 20));
        txtForeigners_PlaceOfIssue = new QLineEdit(tabForeigners);
        txtForeigners_PlaceOfIssue->setObjectName(QString::fromUtf8("txtForeigners_PlaceOfIssue"));
        txtForeigners_PlaceOfIssue->setGeometry(QRect(260, 141, 491, 20));
        sizePolicy.setHeightForWidth(txtForeigners_PlaceOfIssue->sizePolicy().hasHeightForWidth());
        txtForeigners_PlaceOfIssue->setSizePolicy(sizePolicy);
        txtForeigners_PlaceOfIssue->setFrame(false);
        txtForeigners_PlaceOfIssue->setReadOnly(true);
        label_4 = new QLabel(tabForeigners);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(219, 165, 532, 20));
        lblForeigners_ImgPerson = new QLabel(tabForeigners);
        lblForeigners_ImgPerson->setObjectName(QString::fromUtf8("lblForeigners_ImgPerson"));
        lblForeigners_ImgPerson->setGeometry(QRect(63, 191, 150, 270));
        sizePolicy2.setHeightForWidth(lblForeigners_ImgPerson->sizePolicy().hasHeightForWidth());
        lblForeigners_ImgPerson->setSizePolicy(sizePolicy2);
        lblForeigners_ImgPerson->setMinimumSize(QSize(150, 270));
        lblForeigners_ImgPerson->setFrameShape(QFrame::NoFrame);
        lblForeigners_ImgPerson->setScaledContents(true);
        lblForeigners_ImgPerson->setAlignment(Qt::AlignCenter);
        txtForeigners_CardType = new QLineEdit(tabForeigners);
        txtForeigners_CardType->setObjectName(QString::fromUtf8("txtForeigners_CardType"));
        txtForeigners_CardType->setGeometry(QRect(219, 193, 532, 16));
        sizePolicy.setHeightForWidth(txtForeigners_CardType->sizePolicy().hasHeightForWidth());
        txtForeigners_CardType->setSizePolicy(sizePolicy);
        txtForeigners_CardType->setFrame(false);
        txtForeigners_CardType->setReadOnly(true);
        lblForeigners_Footer_1 = new QLabel(tabForeigners);
        lblForeigners_Footer_1->setObjectName(QString::fromUtf8("lblForeigners_Footer_1"));
        lblForeigners_Footer_1->setGeometry(QRect(219, 413, 532, 20));
        sizePolicy4.setHeightForWidth(lblForeigners_Footer_1->sizePolicy().hasHeightForWidth());
        lblForeigners_Footer_1->setSizePolicy(sizePolicy4);
        lblForeigners_Footer_2 = new QLabel(tabForeigners);
        lblForeigners_Footer_2->setObjectName(QString::fromUtf8("lblForeigners_Footer_2"));
        lblForeigners_Footer_2->setGeometry(QRect(219, 439, 532, 20));
        sizePolicy4.setHeightForWidth(lblForeigners_Footer_2->sizePolicy().hasHeightForWidth());
        lblForeigners_Footer_2->setSizePolicy(sizePolicy4);
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
        QSizePolicy sizePolicy6(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(lblSis_Sex->sizePolicy().hasHeightForWidth());
        lblSis_Sex->setSizePolicy(sizePolicy6);
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
        lblIdentity_SocialSecurityNo = new QLabel(tabIdentity_extention);
        lblIdentity_SocialSecurityNo->setObjectName(QString::fromUtf8("lblIdentity_SocialSecurityNo"));
        lblIdentity_SocialSecurityNo->setGeometry(QRect(230, 50, 201, 28));
        sizePolicy.setHeightForWidth(lblIdentity_SocialSecurityNo->sizePolicy().hasHeightForWidth());
        lblIdentity_SocialSecurityNo->setSizePolicy(sizePolicy);
        txtIdentityExtra_SocialSecurityNo = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_SocialSecurityNo->setObjectName(QString::fromUtf8("txtIdentityExtra_SocialSecurityNo"));
        txtIdentityExtra_SocialSecurityNo->setGeometry(QRect(230, 84, 201, 16));
        sizePolicy5.setHeightForWidth(txtIdentityExtra_SocialSecurityNo->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_SocialSecurityNo->setSizePolicy(sizePolicy5);
        txtIdentityExtra_SocialSecurityNo->setFrame(false);
        txtIdentityExtra_SocialSecurityNo->setReadOnly(true);
        lblIdentity_Remarks = new QLabel(tabIdentity_extention);
        lblIdentity_Remarks->setObjectName(QString::fromUtf8("lblIdentity_Remarks"));
        lblIdentity_Remarks->setGeometry(QRect(510, 280, 176, 28));
        sizePolicy4.setHeightForWidth(lblIdentity_Remarks->sizePolicy().hasHeightForWidth());
        lblIdentity_Remarks->setSizePolicy(sizePolicy4);
        txtIdentityExtra_Remarks1 = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_Remarks1->setObjectName(QString::fromUtf8("txtIdentityExtra_Remarks1"));
        txtIdentityExtra_Remarks1->setGeometry(QRect(510, 320, 176, 16));
        sizePolicy.setHeightForWidth(txtIdentityExtra_Remarks1->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_Remarks1->setSizePolicy(sizePolicy);
        txtIdentityExtra_Remarks1->setFrame(false);
        txtIdentityExtra_Remarks1->setReadOnly(true);
        txtIdentityExtra_Remarks2 = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_Remarks2->setObjectName(QString::fromUtf8("txtIdentityExtra_Remarks2"));
        txtIdentityExtra_Remarks2->setGeometry(QRect(510, 340, 176, 16));
        sizePolicy.setHeightForWidth(txtIdentityExtra_Remarks2->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_Remarks2->setSizePolicy(sizePolicy);
        txtIdentityExtra_Remarks2->setFrame(false);
        txtIdentityExtra_Remarks2->setReadOnly(true);
        txtIdentityExtra_Remarks3 = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_Remarks3->setObjectName(QString::fromUtf8("txtIdentityExtra_Remarks3"));
        txtIdentityExtra_Remarks3->setGeometry(QRect(510, 370, 176, 16));
        sizePolicy.setHeightForWidth(txtIdentityExtra_Remarks3->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_Remarks3->setSizePolicy(sizePolicy);
        txtIdentityExtra_Remarks3->setFrame(false);
        txtIdentityExtra_Remarks3->setReadOnly(true);
        txtIdentityExtra_TaxNo = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_TaxNo->setObjectName(QString::fromUtf8("txtIdentityExtra_TaxNo"));
        txtIdentityExtra_TaxNo->setGeometry(QRect(9, 84, 201, 16));
        sizePolicy5.setHeightForWidth(txtIdentityExtra_TaxNo->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_TaxNo->setSizePolicy(sizePolicy5);
        txtIdentityExtra_TaxNo->setFrame(false);
        txtIdentityExtra_TaxNo->setReadOnly(true);
        lblIdentity_TaxNo = new QLabel(tabIdentity_extention);
        lblIdentity_TaxNo->setObjectName(QString::fromUtf8("lblIdentity_TaxNo"));
        lblIdentity_TaxNo->setGeometry(QRect(9, 50, 201, 28));
        sizePolicy.setHeightForWidth(lblIdentity_TaxNo->sizePolicy().hasHeightForWidth());
        lblIdentity_TaxNo->setSizePolicy(sizePolicy);
        lblChip_Number = new QLabel(tabIdentity_extention);
        lblChip_Number->setObjectName(QString::fromUtf8("lblChip_Number"));
        lblChip_Number->setGeometry(QRect(360, 340, 353, 16));
        sizePolicy.setHeightForWidth(lblChip_Number->sizePolicy().hasHeightForWidth());
        lblChip_Number->setSizePolicy(sizePolicy);
        txtIdentityExtra_ChipNumber = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_ChipNumber->setObjectName(QString::fromUtf8("txtIdentityExtra_ChipNumber"));
        txtIdentityExtra_ChipNumber->setGeometry(QRect(360, 367, 353, 16));
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy7.setHorizontalStretch(2);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(txtIdentityExtra_ChipNumber->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_ChipNumber->setSizePolicy(sizePolicy7);
        txtIdentityExtra_ChipNumber->setMinimumSize(QSize(100, 0));
        txtIdentityExtra_ChipNumber->setFrame(false);
        txtIdentityExtra_ChipNumber->setReadOnly(true);
        txtIdentityExtra_Card_Number = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_Card_Number->setObjectName(QString::fromUtf8("txtIdentityExtra_Card_Number"));
        txtIdentityExtra_Card_Number->setGeometry(QRect(420, 474, 353, 16));
        sizePolicy.setHeightForWidth(txtIdentityExtra_Card_Number->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_Card_Number->setSizePolicy(sizePolicy);
        txtIdentityExtra_Card_Number->setFrame(false);
        txtIdentityExtra_Card_Number->setReadOnly(true);
        lblCard_Number = new QLabel(tabIdentity_extention);
        lblCard_Number->setObjectName(QString::fromUtf8("lblCard_Number"));
        lblCard_Number->setGeometry(QRect(420, 440, 353, 28));
        sizePolicy4.setHeightForWidth(lblCard_Number->sizePolicy().hasHeightForWidth());
        lblCard_Number->setSizePolicy(sizePolicy4);
        txtIdentityExtra_ValidFrom_Until = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_ValidFrom_Until->setObjectName(QString::fromUtf8("txtIdentityExtra_ValidFrom_Until"));
        txtIdentityExtra_ValidFrom_Until->setGeometry(QRect(420, 530, 353, 16));
        sizePolicy.setHeightForWidth(txtIdentityExtra_ValidFrom_Until->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_ValidFrom_Until->setSizePolicy(sizePolicy);
        txtIdentityExtra_ValidFrom_Until->setFrame(false);
        txtIdentityExtra_ValidFrom_Until->setReadOnly(true);
        lblCard_ValidFrom_Until = new QLabel(tabIdentity_extention);
        lblCard_ValidFrom_Until->setObjectName(QString::fromUtf8("lblCard_ValidFrom_Until"));
        lblCard_ValidFrom_Until->setGeometry(QRect(420, 496, 353, 28));
        sizePolicy4.setHeightForWidth(lblCard_ValidFrom_Until->sizePolicy().hasHeightForWidth());
        lblCard_ValidFrom_Until->setSizePolicy(sizePolicy4);
        txtIdentityExtra_HealthNo = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_HealthNo->setObjectName(QString::fromUtf8("txtIdentityExtra_HealthNo"));
        txtIdentityExtra_HealthNo->setGeometry(QRect(470, 84, 201, 16));
        sizePolicy5.setHeightForWidth(txtIdentityExtra_HealthNo->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_HealthNo->setSizePolicy(sizePolicy5);
        txtIdentityExtra_HealthNo->setFrame(false);
        txtIdentityExtra_HealthNo->setReadOnly(true);
        lblIdentity_HeathNo = new QLabel(tabIdentity_extention);
        lblIdentity_HeathNo->setObjectName(QString::fromUtf8("lblIdentity_HeathNo"));
        lblIdentity_HeathNo->setGeometry(QRect(470, 50, 201, 28));
        sizePolicy.setHeightForWidth(lblIdentity_HeathNo->sizePolicy().hasHeightForWidth());
        lblIdentity_HeathNo->setSizePolicy(sizePolicy);
        lblIdentity_Title = new QLabel(tabIdentity_extention);
        lblIdentity_Title->setObjectName(QString::fromUtf8("lblIdentity_Title"));
        lblIdentity_Title->setGeometry(QRect(20, 334, 176, 28));
        sizePolicy4.setHeightForWidth(lblIdentity_Title->sizePolicy().hasHeightForWidth());
        lblIdentity_Title->setSizePolicy(sizePolicy4);
        txtIdentityExtra_Title = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_Title->setObjectName(QString::fromUtf8("txtIdentityExtra_Title"));
        txtIdentityExtra_Title->setGeometry(QRect(20, 370, 176, 16));
        sizePolicy.setHeightForWidth(txtIdentityExtra_Title->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_Title->setSizePolicy(sizePolicy);
        txtIdentityExtra_Title->setFrame(false);
        txtIdentityExtra_Title->setReadOnly(true);
        txtIdentityExtra_CardVersion = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_CardVersion->setObjectName(QString::fromUtf8("txtIdentityExtra_CardVersion"));
        txtIdentityExtra_CardVersion->setGeometry(QRect(9, 164, 201, 16));
        sizePolicy5.setHeightForWidth(txtIdentityExtra_CardVersion->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_CardVersion->setSizePolicy(sizePolicy5);
        txtIdentityExtra_CardVersion->setFrame(false);
        txtIdentityExtra_CardVersion->setReadOnly(true);
        lblIdentity_CardVersion = new QLabel(tabIdentity_extention);
        lblIdentity_CardVersion->setObjectName(QString::fromUtf8("lblIdentity_CardVersion"));
        lblIdentity_CardVersion->setGeometry(QRect(9, 130, 201, 28));
        sizePolicy.setHeightForWidth(lblIdentity_CardVersion->sizePolicy().hasHeightForWidth());
        lblIdentity_CardVersion->setSizePolicy(sizePolicy);
        lblIdentity_DocumentType = new QLabel(tabIdentity_extention);
        lblIdentity_DocumentType->setObjectName(QString::fromUtf8("lblIdentity_DocumentType"));
        lblIdentity_DocumentType->setGeometry(QRect(240, 130, 201, 28));
        sizePolicy.setHeightForWidth(lblIdentity_DocumentType->sizePolicy().hasHeightForWidth());
        lblIdentity_DocumentType->setSizePolicy(sizePolicy);
        txtIdentityExtra_DocumentType = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_DocumentType->setObjectName(QString::fromUtf8("txtIdentityExtra_DocumentType"));
        txtIdentityExtra_DocumentType->setGeometry(QRect(240, 164, 201, 16));
        sizePolicy5.setHeightForWidth(txtIdentityExtra_DocumentType->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_DocumentType->setSizePolicy(sizePolicy5);
        txtIdentityExtra_DocumentType->setFrame(false);
        txtIdentityExtra_DocumentType->setReadOnly(true);
        txtIdentityExtra_IssuingEntity = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_IssuingEntity->setObjectName(QString::fromUtf8("txtIdentityExtra_IssuingEntity"));
        txtIdentityExtra_IssuingEntity->setGeometry(QRect(470, 164, 201, 16));
        sizePolicy5.setHeightForWidth(txtIdentityExtra_IssuingEntity->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_IssuingEntity->setSizePolicy(sizePolicy5);
        txtIdentityExtra_IssuingEntity->setFrame(false);
        txtIdentityExtra_IssuingEntity->setReadOnly(true);
        lblIdentity_IssuingEntity = new QLabel(tabIdentity_extention);
        lblIdentity_IssuingEntity->setObjectName(QString::fromUtf8("lblIdentity_IssuingEntity"));
        lblIdentity_IssuingEntity->setGeometry(QRect(470, 130, 201, 28));
        sizePolicy.setHeightForWidth(lblIdentity_IssuingEntity->sizePolicy().hasHeightForWidth());
        lblIdentity_IssuingEntity->setSizePolicy(sizePolicy);
        txtSpecialStatus = new QLineEdit(tabIdentity_extention);
        txtSpecialStatus->setObjectName(QString::fromUtf8("txtSpecialStatus"));
        txtSpecialStatus->setGeometry(QRect(70, 466, 201, 16));
        sizePolicy.setHeightForWidth(txtSpecialStatus->sizePolicy().hasHeightForWidth());
        txtSpecialStatus->setSizePolicy(sizePolicy);
        txtSpecialStatus->setFrame(false);
        txtSpecialStatus->setReadOnly(true);
        lblSpecialStatus = new QLabel(tabIdentity_extention);
        lblSpecialStatus->setObjectName(QString::fromUtf8("lblSpecialStatus"));
        lblSpecialStatus->setGeometry(QRect(70, 430, 201, 28));
        sizePolicy4.setHeightForWidth(lblSpecialStatus->sizePolicy().hasHeightForWidth());
        lblSpecialStatus->setSizePolicy(sizePolicy4);
        lblIdentity_PlaceOfIssue = new QLabel(tabIdentity_extention);
        lblIdentity_PlaceOfIssue->setObjectName(QString::fromUtf8("lblIdentity_PlaceOfIssue"));
        lblIdentity_PlaceOfIssue->setGeometry(QRect(9, 206, 176, 28));
        sizePolicy.setHeightForWidth(lblIdentity_PlaceOfIssue->sizePolicy().hasHeightForWidth());
        lblIdentity_PlaceOfIssue->setSizePolicy(sizePolicy);
        txtIdentityExtra_PlaceOfIssue = new QLineEdit(tabIdentity_extention);
        txtIdentityExtra_PlaceOfIssue->setObjectName(QString::fromUtf8("txtIdentityExtra_PlaceOfIssue"));
        txtIdentityExtra_PlaceOfIssue->setGeometry(QRect(15, 240, 260, 16));
        sizePolicy5.setHeightForWidth(txtIdentityExtra_PlaceOfIssue->sizePolicy().hasHeightForWidth());
        txtIdentityExtra_PlaceOfIssue->setSizePolicy(sizePolicy5);
        txtIdentityExtra_PlaceOfIssue->setSizeIncrement(QSize(0, 0));
        txtIdentityExtra_PlaceOfIssue->setFrame(false);
        txtIdentityExtra_PlaceOfIssue->setReadOnly(true);
        tabWidget_Identity->addTab(tabIdentity_extention, QString());
        tabAddress = new QWidget();
        tabAddress->setObjectName(QString::fromUtf8("tabAddress"));
        lblAddress_StreetType2 = new QLabel(tabAddress);
        lblAddress_StreetType2->setObjectName(QString::fromUtf8("lblAddress_StreetType2"));
        lblAddress_StreetType2->setGeometry(QRect(70, 240, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_StreetType2->sizePolicy().hasHeightForWidth());
        lblAddress_StreetType2->setSizePolicy(sizePolicy4);
        txtAddress_StreetType2 = new QLineEdit(tabAddress);
        txtAddress_StreetType2->setObjectName(QString::fromUtf8("txtAddress_StreetType2"));
        txtAddress_StreetType2->setGeometry(QRect(70, 280, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_StreetType2->sizePolicy().hasHeightForWidth());
        txtAddress_StreetType2->setSizePolicy(sizePolicy);
        txtAddress_StreetType2->setFrame(false);
        txtAddress_StreetType2->setReadOnly(true);
        lblAddress_Municipality = new QLabel(tabAddress);
        lblAddress_Municipality->setObjectName(QString::fromUtf8("lblAddress_Municipality"));
        lblAddress_Municipality->setGeometry(QRect(70, 90, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_Municipality->sizePolicy().hasHeightForWidth());
        lblAddress_Municipality->setSizePolicy(sizePolicy4);
        txtAddress_Municipality = new QLineEdit(tabAddress);
        txtAddress_Municipality->setObjectName(QString::fromUtf8("txtAddress_Municipality"));
        txtAddress_Municipality->setGeometry(QRect(70, 130, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_Municipality->sizePolicy().hasHeightForWidth());
        txtAddress_Municipality->setSizePolicy(sizePolicy);
        txtAddress_Municipality->setFrame(false);
        txtAddress_Municipality->setReadOnly(true);
        txtAddress_CivilParish = new QLineEdit(tabAddress);
        txtAddress_CivilParish->setObjectName(QString::fromUtf8("txtAddress_CivilParish"));
        txtAddress_CivilParish->setGeometry(QRect(70, 210, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_CivilParish->sizePolicy().hasHeightForWidth());
        txtAddress_CivilParish->setSizePolicy(sizePolicy);
        txtAddress_CivilParish->setFrame(false);
        txtAddress_CivilParish->setReadOnly(true);
        lblAddress_CivilParish = new QLabel(tabAddress);
        lblAddress_CivilParish->setObjectName(QString::fromUtf8("lblAddress_CivilParish"));
        lblAddress_CivilParish->setGeometry(QRect(70, 170, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_CivilParish->sizePolicy().hasHeightForWidth());
        lblAddress_CivilParish->setSizePolicy(sizePolicy4);
        txtAddress_District = new QLineEdit(tabAddress);
        txtAddress_District->setObjectName(QString::fromUtf8("txtAddress_District"));
        txtAddress_District->setGeometry(QRect(70, 60, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_District->sizePolicy().hasHeightForWidth());
        txtAddress_District->setSizePolicy(sizePolicy);
        txtAddress_District->setFrame(false);
        txtAddress_District->setReadOnly(true);
        lblAddress_District = new QLabel(tabAddress);
        lblAddress_District->setObjectName(QString::fromUtf8("lblAddress_District"));
        lblAddress_District->setGeometry(QRect(70, 20, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_District->sizePolicy().hasHeightForWidth());
        lblAddress_District->setSizePolicy(sizePolicy4);
        txtAddress_StreetName = new QLineEdit(tabAddress);
        txtAddress_StreetName->setObjectName(QString::fromUtf8("txtAddress_StreetName"));
        txtAddress_StreetName->setGeometry(QRect(320, 280, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_StreetName->sizePolicy().hasHeightForWidth());
        txtAddress_StreetName->setSizePolicy(sizePolicy);
        txtAddress_StreetName->setFrame(false);
        txtAddress_StreetName->setReadOnly(true);
        lblAddress_StreetName = new QLabel(tabAddress);
        lblAddress_StreetName->setObjectName(QString::fromUtf8("lblAddress_StreetName"));
        lblAddress_StreetName->setGeometry(QRect(320, 240, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_StreetName->sizePolicy().hasHeightForWidth());
        lblAddress_StreetName->setSizePolicy(sizePolicy4);
        txtAddress_BuildingType2 = new QLineEdit(tabAddress);
        txtAddress_BuildingType2->setObjectName(QString::fromUtf8("txtAddress_BuildingType2"));
        txtAddress_BuildingType2->setGeometry(QRect(70, 360, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_BuildingType2->sizePolicy().hasHeightForWidth());
        txtAddress_BuildingType2->setSizePolicy(sizePolicy);
        txtAddress_BuildingType2->setFrame(false);
        txtAddress_BuildingType2->setReadOnly(true);
        lblAddress_BuildingType2 = new QLabel(tabAddress);
        lblAddress_BuildingType2->setObjectName(QString::fromUtf8("lblAddress_BuildingType2"));
        lblAddress_BuildingType2->setGeometry(QRect(70, 320, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_BuildingType2->sizePolicy().hasHeightForWidth());
        lblAddress_BuildingType2->setSizePolicy(sizePolicy4);
        txtAddress_DoorNo = new QLineEdit(tabAddress);
        txtAddress_DoorNo->setObjectName(QString::fromUtf8("txtAddress_DoorNo"));
        txtAddress_DoorNo->setGeometry(QRect(320, 360, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_DoorNo->sizePolicy().hasHeightForWidth());
        txtAddress_DoorNo->setSizePolicy(sizePolicy);
        txtAddress_DoorNo->setFrame(false);
        txtAddress_DoorNo->setReadOnly(true);
        lblAddress_DoorNo = new QLabel(tabAddress);
        lblAddress_DoorNo->setObjectName(QString::fromUtf8("lblAddress_DoorNo"));
        lblAddress_DoorNo->setGeometry(QRect(320, 320, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_DoorNo->sizePolicy().hasHeightForWidth());
        lblAddress_DoorNo->setSizePolicy(sizePolicy4);
        txtAddress_Side = new QLineEdit(tabAddress);
        txtAddress_Side->setObjectName(QString::fromUtf8("txtAddress_Side"));
        txtAddress_Side->setGeometry(QRect(70, 430, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_Side->sizePolicy().hasHeightForWidth());
        txtAddress_Side->setSizePolicy(sizePolicy);
        txtAddress_Side->setFrame(false);
        txtAddress_Side->setReadOnly(true);
        lblAddress_Side = new QLabel(tabAddress);
        lblAddress_Side->setObjectName(QString::fromUtf8("lblAddress_Side"));
        lblAddress_Side->setGeometry(QRect(70, 390, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_Side->sizePolicy().hasHeightForWidth());
        lblAddress_Side->setSizePolicy(sizePolicy4);
        lblAddress_Locality = new QLabel(tabAddress);
        lblAddress_Locality->setObjectName(QString::fromUtf8("lblAddress_Locality"));
        lblAddress_Locality->setGeometry(QRect(320, 390, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_Locality->sizePolicy().hasHeightForWidth());
        lblAddress_Locality->setSizePolicy(sizePolicy4);
        txtAddress_Locality = new QLineEdit(tabAddress);
        txtAddress_Locality->setObjectName(QString::fromUtf8("txtAddress_Locality"));
        txtAddress_Locality->setGeometry(QRect(320, 430, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_Locality->sizePolicy().hasHeightForWidth());
        txtAddress_Locality->setSizePolicy(sizePolicy);
        txtAddress_Locality->setFrame(false);
        txtAddress_Locality->setReadOnly(true);
        lblAddress_Zip4 = new QLabel(tabAddress);
        lblAddress_Zip4->setObjectName(QString::fromUtf8("lblAddress_Zip4"));
        lblAddress_Zip4->setGeometry(QRect(540, 390, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_Zip4->sizePolicy().hasHeightForWidth());
        lblAddress_Zip4->setSizePolicy(sizePolicy4);
        txtAddress_Zip4 = new QLineEdit(tabAddress);
        txtAddress_Zip4->setObjectName(QString::fromUtf8("txtAddress_Zip4"));
        txtAddress_Zip4->setGeometry(QRect(540, 430, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_Zip4->sizePolicy().hasHeightForWidth());
        txtAddress_Zip4->setSizePolicy(sizePolicy);
        txtAddress_Zip4->setFrame(false);
        txtAddress_Zip4->setReadOnly(true);
        txtAddress_Floor = new QLineEdit(tabAddress);
        txtAddress_Floor->setObjectName(QString::fromUtf8("txtAddress_Floor"));
        txtAddress_Floor->setGeometry(QRect(540, 360, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_Floor->sizePolicy().hasHeightForWidth());
        txtAddress_Floor->setSizePolicy(sizePolicy);
        txtAddress_Floor->setFrame(false);
        txtAddress_Floor->setReadOnly(true);
        lblAddress_Floor = new QLabel(tabAddress);
        lblAddress_Floor->setObjectName(QString::fromUtf8("lblAddress_Floor"));
        lblAddress_Floor->setGeometry(QRect(540, 320, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_Floor->sizePolicy().hasHeightForWidth());
        lblAddress_Floor->setSizePolicy(sizePolicy4);
        txtAddress_BuildingType1 = new QLineEdit(tabAddress);
        txtAddress_BuildingType1->setObjectName(QString::fromUtf8("txtAddress_BuildingType1"));
        txtAddress_BuildingType1->setGeometry(QRect(540, 280, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_BuildingType1->sizePolicy().hasHeightForWidth());
        txtAddress_BuildingType1->setSizePolicy(sizePolicy);
        txtAddress_BuildingType1->setFrame(false);
        txtAddress_BuildingType1->setReadOnly(true);
        lblAddress_BuildingType1 = new QLabel(tabAddress);
        lblAddress_BuildingType1->setObjectName(QString::fromUtf8("lblAddress_BuildingType1"));
        lblAddress_BuildingType1->setGeometry(QRect(540, 240, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_BuildingType1->sizePolicy().hasHeightForWidth());
        lblAddress_BuildingType1->setSizePolicy(sizePolicy4);
        lblAddress_StreetType1 = new QLabel(tabAddress);
        lblAddress_StreetType1->setObjectName(QString::fromUtf8("lblAddress_StreetType1"));
        lblAddress_StreetType1->setGeometry(QRect(320, 170, 201, 28));
        sizePolicy4.setHeightForWidth(lblAddress_StreetType1->sizePolicy().hasHeightForWidth());
        lblAddress_StreetType1->setSizePolicy(sizePolicy4);
        txtAddress_StreetType1 = new QLineEdit(tabAddress);
        txtAddress_StreetType1->setObjectName(QString::fromUtf8("txtAddress_StreetType1"));
        txtAddress_StreetType1->setGeometry(QRect(320, 210, 201, 16));
        sizePolicy.setHeightForWidth(txtAddress_StreetType1->sizePolicy().hasHeightForWidth());
        txtAddress_StreetType1->setSizePolicy(sizePolicy);
        txtAddress_StreetType1->setFrame(false);
        txtAddress_StreetType1->setReadOnly(true);
        txtAddress_Zip3 = new QLineEdit(tabAddress);
        txtAddress_Zip3->setObjectName(QString::fromUtf8("txtAddress_Zip3"));
        txtAddress_Zip3->setGeometry(QRect(320, 130, 176, 16));
        sizePolicy.setHeightForWidth(txtAddress_Zip3->sizePolicy().hasHeightForWidth());
        txtAddress_Zip3->setSizePolicy(sizePolicy);
        txtAddress_Zip3->setFrame(false);
        txtAddress_Zip3->setReadOnly(true);
        lblAddress_Zip3 = new QLabel(tabAddress);
        lblAddress_Zip3->setObjectName(QString::fromUtf8("lblAddress_Zip3"));
        lblAddress_Zip3->setGeometry(QRect(320, 90, 176, 28));
        sizePolicy4.setHeightForWidth(lblAddress_Zip3->sizePolicy().hasHeightForWidth());
        lblAddress_Zip3->setSizePolicy(sizePolicy4);
        lblAddress_Place = new QLabel(tabAddress);
        lblAddress_Place->setObjectName(QString::fromUtf8("lblAddress_Place"));
        lblAddress_Place->setGeometry(QRect(310, 20, 176, 28));
        sizePolicy4.setHeightForWidth(lblAddress_Place->sizePolicy().hasHeightForWidth());
        lblAddress_Place->setSizePolicy(sizePolicy4);
        txtAddress_Place = new QLineEdit(tabAddress);
        txtAddress_Place->setObjectName(QString::fromUtf8("txtAddress_Place"));
        txtAddress_Place->setGeometry(QRect(310, 60, 176, 16));
        sizePolicy.setHeightForWidth(txtAddress_Place->sizePolicy().hasHeightForWidth());
        txtAddress_Place->setSizePolicy(sizePolicy);
        txtAddress_Place->setFrame(false);
        txtAddress_Place->setReadOnly(true);
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
        sizePolicy5.setHeightForWidth(txtForeignersExtra_BirthDate->sizePolicy().hasHeightForWidth());
        txtForeignersExtra_BirthDate->setSizePolicy(sizePolicy5);
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
        sizePolicy.setHeightForWidth(txtForeignersExtra_NationalNumber->sizePolicy().hasHeightForWidth());
        txtForeignersExtra_NationalNumber->setSizePolicy(sizePolicy);
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
        sizePolicy.setHeightForWidth(txtForeignerSpecialStatus->sizePolicy().hasHeightForWidth());
        txtForeignerSpecialStatus->setSizePolicy(sizePolicy);
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
        QSizePolicy sizePolicy8(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy8.setHorizontalStretch(4);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(lblSisExtra_Card_ValidFrom_Until->sizePolicy().hasHeightForWidth());
        lblSisExtra_Card_ValidFrom_Until->setSizePolicy(sizePolicy8);
        txtSisExtra_ValidFrom_Until = new QLineEdit(tabSis_Back);
        txtSisExtra_ValidFrom_Until->setObjectName(QString::fromUtf8("txtSisExtra_ValidFrom_Until"));
        txtSisExtra_ValidFrom_Until->setGeometry(QRect(258, 63, 493, 16));
        QSizePolicy sizePolicy9(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy9.setHorizontalStretch(4);
        sizePolicy9.setVerticalStretch(0);
        sizePolicy9.setHeightForWidth(txtSisExtra_ValidFrom_Until->sizePolicy().hasHeightForWidth());
        txtSisExtra_ValidFrom_Until->setSizePolicy(sizePolicy9);
        txtSisExtra_ValidFrom_Until->setFrame(false);
        txtSisExtra_ValidFrom_Until->setReadOnly(true);
        tabWidget_Identity->addTab(tabSis_Back, QString());
        tabCardPin = new QWidget();
        tabCardPin->setObjectName(QString::fromUtf8("tabCardPin"));
        tabCardPin->setFocusPolicy(Qt::NoFocus);
        gridLayout2 = new QGridLayout(tabCardPin);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        grbPIN_VersionInfo = new QGroupBox(tabCardPin);
        grbPIN_VersionInfo->setObjectName(QString::fromUtf8("grbPIN_VersionInfo"));
        QSizePolicy sizePolicy10(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy10.setHorizontalStretch(0);
        sizePolicy10.setVerticalStretch(2);
        sizePolicy10.setHeightForWidth(grbPIN_VersionInfo->sizePolicy().hasHeightForWidth());
        grbPIN_VersionInfo->setSizePolicy(sizePolicy10);
        gridLayout3 = new QGridLayout(grbPIN_VersionInfo);
        gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
        tblCardAndPIN = new QTableWidget(grbPIN_VersionInfo);
        tblCardAndPIN->setObjectName(QString::fromUtf8("tblCardAndPIN"));
        tblCardAndPIN->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblCardAndPIN->setShowGrid(false);

        gridLayout3->addWidget(tblCardAndPIN, 0, 0, 1, 1);


        gridLayout2->addWidget(grbPIN_VersionInfo, 0, 0, 1, 1);

        grbPIN_PINInfo = new QGroupBox(tabCardPin);
        grbPIN_PINInfo->setObjectName(QString::fromUtf8("grbPIN_PINInfo"));
        sizePolicy10.setHeightForWidth(grbPIN_PINInfo->sizePolicy().hasHeightForWidth());
        grbPIN_PINInfo->setSizePolicy(sizePolicy10);
        gridLayout4 = new QGridLayout(grbPIN_PINInfo);
        gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
        treePIN = new QTreeWidget(grbPIN_PINInfo);
        treePIN->setObjectName(QString::fromUtf8("treePIN"));

        gridLayout4->addWidget(treePIN, 0, 0, 1, 1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        lblPIN_Name = new QLabel(grbPIN_PINInfo);
        lblPIN_Name->setObjectName(QString::fromUtf8("lblPIN_Name"));
        sizePolicy4.setHeightForWidth(lblPIN_Name->sizePolicy().hasHeightForWidth());
        lblPIN_Name->setSizePolicy(sizePolicy4);

        hboxLayout->addWidget(lblPIN_Name);

        lblPIN_ID = new QLabel(grbPIN_PINInfo);
        lblPIN_ID->setObjectName(QString::fromUtf8("lblPIN_ID"));
        sizePolicy4.setHeightForWidth(lblPIN_ID->sizePolicy().hasHeightForWidth());
        lblPIN_ID->setSizePolicy(sizePolicy4);

        hboxLayout->addWidget(lblPIN_ID);


        vboxLayout2->addLayout(hboxLayout);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        txtPIN_Name = new QLineEdit(grbPIN_PINInfo);
        txtPIN_Name->setObjectName(QString::fromUtf8("txtPIN_Name"));
        txtPIN_Name->setReadOnly(true);

        hboxLayout1->addWidget(txtPIN_Name);

        txtPIN_ID = new QLineEdit(grbPIN_PINInfo);
        txtPIN_ID->setObjectName(QString::fromUtf8("txtPIN_ID"));
        txtPIN_ID->setReadOnly(true);

        hboxLayout1->addWidget(txtPIN_ID);


        vboxLayout2->addLayout(hboxLayout1);

        lblPIN_Status = new QLabel(grbPIN_PINInfo);
        lblPIN_Status->setObjectName(QString::fromUtf8("lblPIN_Status"));
        sizePolicy4.setHeightForWidth(lblPIN_Status->sizePolicy().hasHeightForWidth());
        lblPIN_Status->setSizePolicy(sizePolicy4);

        vboxLayout2->addWidget(lblPIN_Status);

        txtPIN_Status = new QLineEdit(grbPIN_PINInfo);
        txtPIN_Status->setObjectName(QString::fromUtf8("txtPIN_Status"));
        txtPIN_Status->setReadOnly(true);

        vboxLayout2->addWidget(txtPIN_Status);

        spacerItem4 = new QSpacerItem(431, 31, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout2->addItem(spacerItem4);

        lblPIN_InfoAdd = new QLabel(grbPIN_PINInfo);
        lblPIN_InfoAdd->setObjectName(QString::fromUtf8("lblPIN_InfoAdd"));
        sizePolicy4.setHeightForWidth(lblPIN_InfoAdd->sizePolicy().hasHeightForWidth());
        lblPIN_InfoAdd->setSizePolicy(sizePolicy4);
        lblPIN_InfoAdd->setWordWrap(true);

        vboxLayout2->addWidget(lblPIN_InfoAdd);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        spacerItem5 = new QSpacerItem(251, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem5);

        btnPIN_Test = new QPushButton(grbPIN_PINInfo);
        btnPIN_Test->setObjectName(QString::fromUtf8("btnPIN_Test"));
        btnPIN_Test->setFont(font);

        hboxLayout2->addWidget(btnPIN_Test);

        btnPIN_Change = new QPushButton(grbPIN_PINInfo);
        btnPIN_Change->setObjectName(QString::fromUtf8("btnPIN_Change"));
        btnPIN_Change->setFont(font);

        hboxLayout2->addWidget(btnPIN_Change);


        vboxLayout2->addLayout(hboxLayout2);


        gridLayout4->addLayout(vboxLayout2, 0, 1, 1, 1);


        gridLayout2->addWidget(grbPIN_PINInfo, 1, 0, 1, 1);

        tabWidget_Identity->addTab(tabCardPin, QString());
        tabCertificates = new QWidget();
        tabCertificates->setObjectName(QString::fromUtf8("tabCertificates"));
        tabCertificates->setFocusPolicy(Qt::NoFocus);
        gridLayout5 = new QGridLayout(tabCertificates);
        gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
        treeCert = new QTreeWidget(tabCertificates);
        treeCert->setObjectName(QString::fromUtf8("treeCert"));
        QSizePolicy sizePolicy11(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy11.setHorizontalStretch(20);
        sizePolicy11.setVerticalStretch(0);
        sizePolicy11.setHeightForWidth(treeCert->sizePolicy().hasHeightForWidth());
        treeCert->setSizePolicy(sizePolicy11);
        treeCert->setMinimumSize(QSize(300, 0));
        treeCert->setSortingEnabled(true);

        gridLayout5->addWidget(treeCert, 0, 0, 2, 1);

        gridLayout6 = new QGridLayout();
        gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));
        lblCert_Owner = new QLabel(tabCertificates);
        lblCert_Owner->setObjectName(QString::fromUtf8("lblCert_Owner"));
        sizePolicy4.setHeightForWidth(lblCert_Owner->sizePolicy().hasHeightForWidth());
        lblCert_Owner->setSizePolicy(sizePolicy4);

        gridLayout6->addWidget(lblCert_Owner, 0, 0, 1, 4);

        txtCert_Owner = new QLineEdit(tabCertificates);
        txtCert_Owner->setObjectName(QString::fromUtf8("txtCert_Owner"));
        txtCert_Owner->setReadOnly(true);

        gridLayout6->addWidget(txtCert_Owner, 1, 0, 1, 4);

        lblCert_Issuer = new QLabel(tabCertificates);
        lblCert_Issuer->setObjectName(QString::fromUtf8("lblCert_Issuer"));
        sizePolicy4.setHeightForWidth(lblCert_Issuer->sizePolicy().hasHeightForWidth());
        lblCert_Issuer->setSizePolicy(sizePolicy4);

        gridLayout6->addWidget(lblCert_Issuer, 2, 0, 1, 4);

        txtCert_Issuer = new QLineEdit(tabCertificates);
        txtCert_Issuer->setObjectName(QString::fromUtf8("txtCert_Issuer"));
        txtCert_Issuer->setReadOnly(true);

        gridLayout6->addWidget(txtCert_Issuer, 3, 0, 1, 4);

        lblCert_KeyLenght = new QLabel(tabCertificates);
        lblCert_KeyLenght->setObjectName(QString::fromUtf8("lblCert_KeyLenght"));
        sizePolicy4.setHeightForWidth(lblCert_KeyLenght->sizePolicy().hasHeightForWidth());
        lblCert_KeyLenght->setSizePolicy(sizePolicy4);

        gridLayout6->addWidget(lblCert_KeyLenght, 4, 0, 1, 4);

        txtCert_KeyLenght = new QLineEdit(tabCertificates);
        txtCert_KeyLenght->setObjectName(QString::fromUtf8("txtCert_KeyLenght"));
        txtCert_KeyLenght->setReadOnly(true);

        gridLayout6->addWidget(txtCert_KeyLenght, 5, 0, 1, 4);

        lblCert_ValidFrom = new QLabel(tabCertificates);
        lblCert_ValidFrom->setObjectName(QString::fromUtf8("lblCert_ValidFrom"));
        sizePolicy4.setHeightForWidth(lblCert_ValidFrom->sizePolicy().hasHeightForWidth());
        lblCert_ValidFrom->setSizePolicy(sizePolicy4);

        gridLayout6->addWidget(lblCert_ValidFrom, 6, 0, 1, 1);

        lblCert_ValidUntil = new QLabel(tabCertificates);
        lblCert_ValidUntil->setObjectName(QString::fromUtf8("lblCert_ValidUntil"));
        sizePolicy4.setHeightForWidth(lblCert_ValidUntil->sizePolicy().hasHeightForWidth());
        lblCert_ValidUntil->setSizePolicy(sizePolicy4);

        gridLayout6->addWidget(lblCert_ValidUntil, 6, 2, 1, 1);

        txtCert_ValidFrom = new QLineEdit(tabCertificates);
        txtCert_ValidFrom->setObjectName(QString::fromUtf8("txtCert_ValidFrom"));
        txtCert_ValidFrom->setReadOnly(true);

        gridLayout6->addWidget(txtCert_ValidFrom, 7, 0, 1, 2);

        txtCert_ValidUntil = new QLineEdit(tabCertificates);
        txtCert_ValidUntil->setObjectName(QString::fromUtf8("txtCert_ValidUntil"));
        txtCert_ValidUntil->setReadOnly(true);

        gridLayout6->addWidget(txtCert_ValidUntil, 7, 2, 1, 2);

        lblCert_Status = new QLabel(tabCertificates);
        lblCert_Status->setObjectName(QString::fromUtf8("lblCert_Status"));
        sizePolicy4.setHeightForWidth(lblCert_Status->sizePolicy().hasHeightForWidth());
        lblCert_Status->setSizePolicy(sizePolicy4);

        gridLayout6->addWidget(lblCert_Status, 8, 0, 1, 4);

        txtCert_Status = new QLineEdit(tabCertificates);
        txtCert_Status->setObjectName(QString::fromUtf8("txtCert_Status"));
        txtCert_Status->setReadOnly(true);

        gridLayout6->addWidget(txtCert_Status, 9, 0, 1, 3);

        btnOCSPCheck = new QPushButton(tabCertificates);
        btnOCSPCheck->setObjectName(QString::fromUtf8("btnOCSPCheck"));
        btnOCSPCheck->setFont(font);

        gridLayout6->addWidget(btnOCSPCheck, 9, 3, 1, 1);

        spacerItem6 = new QSpacerItem(20, 108, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout6->addItem(spacerItem6, 10, 1, 1, 1);

        lblCert_InfoAdd = new QLabel(tabCertificates);
        lblCert_InfoAdd->setObjectName(QString::fromUtf8("lblCert_InfoAdd"));
        sizePolicy4.setHeightForWidth(lblCert_InfoAdd->sizePolicy().hasHeightForWidth());
        lblCert_InfoAdd->setSizePolicy(sizePolicy4);
        lblCert_InfoAdd->setWordWrap(true);

        gridLayout6->addWidget(lblCert_InfoAdd, 11, 0, 1, 4);


        gridLayout5->addLayout(gridLayout6, 0, 1, 1, 1);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setSpacing(6);
        hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
        spacerItem7 = new QSpacerItem(20, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout3->addItem(spacerItem7);

        btnCert_Register = new QPushButton(tabCertificates);
        btnCert_Register->setObjectName(QString::fromUtf8("btnCert_Register"));
        btnCert_Register->setFont(font);

        hboxLayout3->addWidget(btnCert_Register);

        btnCert_Details = new QPushButton(tabCertificates);
        btnCert_Details->setObjectName(QString::fromUtf8("btnCert_Details"));
        btnCert_Details->setFont(font);

        hboxLayout3->addWidget(btnCert_Details);


        gridLayout5->addLayout(hboxLayout3, 1, 1, 1, 1);

        tabWidget_Identity->addTab(tabCertificates, QString());
        tabPersoData = new QWidget();
        tabPersoData->setObjectName(QString::fromUtf8("tabPersoData"));
        txtPersoData = new QPlainTextEdit(tabPersoData);
        txtPersoData->setObjectName(QString::fromUtf8("txtPersoData"));
        txtPersoData->setGeometry(QRect(10, 10, 721, 481));
        tabWidget_Identity->addTab(tabPersoData, QString());
        tabInfo = new QWidget();
        tabInfo->setObjectName(QString::fromUtf8("tabInfo"));
        tabInfo->setFocusPolicy(Qt::NoFocus);
        gridLayout7 = new QGridLayout(tabInfo);
        gridLayout7->setObjectName(QString::fromUtf8("gridLayout7"));
        lblInfo_Img1 = new QLabel(tabInfo);
        lblInfo_Img1->setObjectName(QString::fromUtf8("lblInfo_Img1"));
        QSizePolicy sizePolicy12(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy12.setHorizontalStretch(1);
        sizePolicy12.setVerticalStretch(0);
        sizePolicy12.setHeightForWidth(lblInfo_Img1->sizePolicy().hasHeightForWidth());
        lblInfo_Img1->setSizePolicy(sizePolicy12);
        lblInfo_Img1->setPixmap(QPixmap(QString::fromUtf8(":/images/Images/a-z_identiteitskaart_logo.png")));
        lblInfo_Img1->setScaledContents(false);

        gridLayout7->addWidget(lblInfo_Img1, 0, 0, 1, 1);

        lblInfo_Top = new QLabel(tabInfo);
        lblInfo_Top->setObjectName(QString::fromUtf8("lblInfo_Top"));
        sizePolicy8.setHeightForWidth(lblInfo_Top->sizePolicy().hasHeightForWidth());
        lblInfo_Top->setSizePolicy(sizePolicy8);

        gridLayout7->addWidget(lblInfo_Top, 0, 1, 1, 1);

        tblInfo = new QTableWidget(tabInfo);
        tblInfo->setObjectName(QString::fromUtf8("tblInfo"));
        tblInfo->setSelectionMode(QAbstractItemView::NoSelection);
        tblInfo->setShowGrid(false);

        gridLayout7->addWidget(tblInfo, 1, 0, 1, 2);

        lblInfo_bottom = new QLabel(tabInfo);
        lblInfo_bottom->setObjectName(QString::fromUtf8("lblInfo_bottom"));
        lblInfo_bottom->setAlignment(Qt::AlignCenter);

        gridLayout7->addWidget(lblInfo_bottom, 2, 0, 1, 2);

        tabWidget_Identity->addTab(tabInfo, QString());

        vboxLayout1->addWidget(tabWidget_Identity);

        stackedWidget->addWidget(page_2);

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);

        MainWnd->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWnd);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 25));
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
        toolBar->addAction(actionZoom_Out);

        retranslateUi(MainWnd);

        stackedWidget->setCurrentIndex(1);
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
        lblSplashImg_4->setText(QString());
#ifndef QT_NO_ACCESSIBILITY
        tabWidget_Identity->setAccessibleName(QApplication::translate("MainWnd", "card contents", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        tabIdentity->setAccessibleName(QApplication::translate("MainWnd", "Tab Identity", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblIdentity_Head2_2->setText(QApplication::translate("MainWnd", "CARTAO DE CIDADAO", 0, QApplication::UnicodeUTF8));
        lblIdentity_Head2_4->setText(QString());
        lblIdentity_Head2_1->setText(QApplication::translate("MainWnd", "txtIdentityCard_1", 0, QApplication::UnicodeUTF8));
        lblIdentity_Head1_2->setText(QApplication::translate("MainWnd", "PORTUGAL", 0, QApplication::UnicodeUTF8));
        lblIdentity_Head1_3->setText(QString());
        lblIdentity_Head1_4->setText(QString());
        lblIdentity_Head2_3->setText(QString());
        lblIdentity_GivenNames->setText(QApplication::translate("MainWnd", "Voornamen / <i>Given names</i>", 0, QApplication::UnicodeUTF8));
        lblIdentity_BirthDate->setText(QApplication::translate("MainWnd", "Geboorteplaats en -datum / <i>Place and date of Birth</i>", 0, QApplication::UnicodeUTF8));
        lblIdentity_Height->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Altura / <span style=\" font-style:italic;\">Card No</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblIdentity_ValidFrom_Until->setText(QApplication::translate("MainWnd", "Geldig van - tot / <i>Valid from - until</i>", 0, QApplication::UnicodeUTF8));
        lblIdentity_Parents->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Filia\303\247\303\243o / <span style=\" font-style:italic;\">Parents</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblIdentity_Head1_1->setText(QApplication::translate("MainWnd", "PORTUGAL", 0, QApplication::UnicodeUTF8));
        lblIdentity_Sex->setText(QApplication::translate("MainWnd", "Geslacht / <i>Sex</i> ", 0, QApplication::UnicodeUTF8));
        lblIdentity_Name->setText(QApplication::translate("MainWnd", "Naam / <i>Name</i>", 0, QApplication::UnicodeUTF8));
        lblIdentity_Nationality->setText(QApplication::translate("MainWnd", "Nationaliteit<br><i>Nationality</i>", 0, QApplication::UnicodeUTF8));
        lblIdentity_ImgPerson->setText(QString());
        lblIdentity_DocumentNumber->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">N\302\272 Documento / <span style=\" font-style:italic;\">Document Number</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtIdentity_Country->setStyleSheet(QString());
        lblIdentity_Country->setStyleSheet(QString());
        lblIdentity_Country->setText(QApplication::translate("MainWnd", "Land<br><i>Country</i>", 0, QApplication::UnicodeUTF8));
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
        lblIdentity_SocialSecurityNo->setStyleSheet(QString());
        lblIdentity_SocialSecurityNo->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Identificatienummer<br /><span style=\" font-style:italic;\">Social Security No.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_SocialSecurityNo->setStyleSheet(QString());
        lblIdentity_Remarks->setText(QApplication::translate("MainWnd", "Opmerkingen<br><i>Remarks</i>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_Remarks1->setStyleSheet(QString());
        txtIdentityExtra_Remarks2->setStyleSheet(QString());
        txtIdentityExtra_Remarks3->setStyleSheet(QString());
        txtIdentityExtra_TaxNo->setStyleSheet(QString());
        lblIdentity_TaxNo->setStyleSheet(QString());
        lblIdentity_TaxNo->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Identificatienummer<br /><span style=\" font-style:italic;\">Tax No.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblChip_Number->setStyleSheet(QString());
        lblChip_Number->setText(QApplication::translate("MainWnd", "Chip nummer / <i>Chip number</i>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_ChipNumber->setStyleSheet(QString());
        txtIdentityExtra_Card_Number->setStyleSheet(QString());
        lblCard_Number->setStyleSheet(QString());
        lblCard_Number->setText(QApplication::translate("MainWnd", "Kaart nummer / <i>Card number</i>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_ValidFrom_Until->setStyleSheet(QString());
        lblCard_ValidFrom_Until->setStyleSheet(QString());
        lblCard_ValidFrom_Until->setText(QApplication::translate("MainWnd", "Geldig van - tot / <i>Valid from - until</i>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_HealthNo->setStyleSheet(QString());
        lblIdentity_HeathNo->setStyleSheet(QString());
        lblIdentity_HeathNo->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Identificatienummer<br /><span style=\" font-style:italic;\">Health No.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblIdentity_Title->setText(QApplication::translate("MainWnd", "Titel<br><i>Title</i>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_Title->setStyleSheet(QString());
        txtIdentityExtra_CardVersion->setStyleSheet(QString());
        lblIdentity_CardVersion->setStyleSheet(QString());
        lblIdentity_CardVersion->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Identificatienummer<br /><span style=\" font-style:italic;\">Card Version</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblIdentity_DocumentType->setStyleSheet(QString());
        lblIdentity_DocumentType->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Identificatienummer<br /><span style=\" font-style:italic;\">Document Type</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_DocumentType->setStyleSheet(QString());
        txtIdentityExtra_IssuingEntity->setStyleSheet(QString());
        lblIdentity_IssuingEntity->setStyleSheet(QString());
        lblIdentity_IssuingEntity->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Identificatienummer<br /><span style=\" font-style:italic;\">Issuing Entity</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtSpecialStatus->setStyleSheet(QString());
        lblSpecialStatus->setStyleSheet(QString());
        lblSpecialStatus->setText(QApplication::translate("MainWnd", "Special Status", 0, QApplication::UnicodeUTF8));
        lblIdentity_PlaceOfIssue->setStyleSheet(QString());
        lblIdentity_PlaceOfIssue->setText(QApplication::translate("MainWnd", "Plaats van afgifte<br><i>Place of issue</i>", 0, QApplication::UnicodeUTF8));
        txtIdentityExtra_PlaceOfIssue->setStyleSheet(QString());
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabIdentity_extention), QApplication::translate("MainWnd", "Identity &Extra", 0, QApplication::UnicodeUTF8));
        lblAddress_StreetType2->setStyleSheet(QString());
        lblAddress_StreetType2->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Street Type2</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_StreetType2->setStyleSheet(QString());
        lblAddress_Municipality->setStyleSheet(QString());
        lblAddress_Municipality->setText(QApplication::translate("MainWnd", "Gemeente <br> <i>Municipality</i>", 0, QApplication::UnicodeUTF8));
        txtAddress_Municipality->setStyleSheet(QString());
        txtAddress_CivilParish->setStyleSheet(QString());
        lblAddress_CivilParish->setStyleSheet(QString());
        lblAddress_CivilParish->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Civil Parish</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_District->setStyleSheet(QString());
        lblAddress_District->setStyleSheet(QString());
        lblAddress_District->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">National District</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_StreetName->setStyleSheet(QString());
        lblAddress_StreetName->setStyleSheet(QString());
        lblAddress_StreetName->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Street Name</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_BuildingType2->setStyleSheet(QString());
        lblAddress_BuildingType2->setStyleSheet(QString());
        lblAddress_BuildingType2->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br />Building Type2</p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_DoorNo->setStyleSheet(QString());
        lblAddress_DoorNo->setStyleSheet(QString());
        lblAddress_DoorNo->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br />Door No</p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_Side->setStyleSheet(QString());
        lblAddress_Side->setStyleSheet(QString());
        lblAddress_Side->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Side</span></p></body></html>", 0, QApplication::UnicodeUTF8));
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
        txtAddress_Floor->setStyleSheet(QString());
        lblAddress_Floor->setStyleSheet(QString());
        lblAddress_Floor->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br />Floor</p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_BuildingType1->setStyleSheet(QString());
        lblAddress_BuildingType1->setStyleSheet(QString());
        lblAddress_BuildingType1->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br />Building Type1</p></body></html>", 0, QApplication::UnicodeUTF8));
        lblAddress_StreetType1->setStyleSheet(QString());
        lblAddress_StreetType1->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Straat <br /><span style=\" font-style:italic;\">Street Type 1</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_StreetType1->setStyleSheet(QString());
        txtAddress_Zip3->setStyleSheet(QString());
        lblAddress_Zip3->setStyleSheet(QString());
        lblAddress_Zip3->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Postcode<br /><span style=\" font-style:italic;\">Zip3</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblAddress_Place->setStyleSheet(QString());
        lblAddress_Place->setText(QApplication::translate("MainWnd", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Postcode<br /><span style=\" font-style:italic;\">Place</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        txtAddress_Place->setStyleSheet(QString());
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
        grbPIN_VersionInfo->setStyleSheet(QString());
        grbPIN_VersionInfo->setTitle(QApplication::translate("MainWnd", "Version Info", 0, QApplication::UnicodeUTF8));
        grbPIN_PINInfo->setStyleSheet(QString());
        grbPIN_PINInfo->setTitle(QApplication::translate("MainWnd", "PIN Info", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treePIN->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWnd", "PINs", 0, QApplication::UnicodeUTF8));
        lblPIN_Name->setText(QApplication::translate("MainWnd", "PIN  Name", 0, QApplication::UnicodeUTF8));
        lblPIN_ID->setText(QApplication::translate("MainWnd", "PIN  ID", 0, QApplication::UnicodeUTF8));
        lblPIN_Status->setText(QApplication::translate("MainWnd", "PIN  Status", 0, QApplication::UnicodeUTF8));
        lblPIN_InfoAdd->setText(QApplication::translate("MainWnd", "When you give a wrong PIN number 3 times, the connected services will be blocked. If so please contact your local goverment inctance. Press \"Cancel\" if you are unscertain.", 0, QApplication::UnicodeUTF8));
        btnPIN_Test->setText(QApplication::translate("MainWnd", "Test PIN", 0, QApplication::UnicodeUTF8));
        btnPIN_Change->setText(QApplication::translate("MainWnd", "Change PIN", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabCardPin), QApplication::translate("MainWnd", "Card && &PIN", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabCertificates->setAccessibleName(QApplication::translate("MainWnd", "Tab Certificates", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        QTreeWidgetItem *___qtreewidgetitem1 = treeCert->headerItem();
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWnd", "Certificates Overview", 0, QApplication::UnicodeUTF8));
        lblCert_Owner->setText(QApplication::translate("MainWnd", "Owner", 0, QApplication::UnicodeUTF8));
        lblCert_Issuer->setText(QApplication::translate("MainWnd", "Issuer", 0, QApplication::UnicodeUTF8));
        lblCert_KeyLenght->setText(QApplication::translate("MainWnd", "Key Lenght", 0, QApplication::UnicodeUTF8));
        txtCert_KeyLenght->setStyleSheet(QString());
        lblCert_ValidFrom->setText(QApplication::translate("MainWnd", "Valid From", 0, QApplication::UnicodeUTF8));
        lblCert_ValidUntil->setText(QApplication::translate("MainWnd", "Valid Until", 0, QApplication::UnicodeUTF8));
        lblCert_Status->setText(QApplication::translate("MainWnd", "OCSP Status", 0, QApplication::UnicodeUTF8));
        btnOCSPCheck->setText(QApplication::translate("MainWnd", "OCSP check", 0, QApplication::UnicodeUTF8));
        lblCert_InfoAdd->setText(QApplication::translate("MainWnd", "To use your identity card with applications like Internet Explorer, Microsoft Offfice,OpenOffice.org, Adobe Reader, Outlook, ... you first have to register the card in Windows. Select a certificate and click the \"Register\" button to register your certificates", 0, QApplication::UnicodeUTF8));
        btnCert_Register->setText(QApplication::translate("MainWnd", "&Register", 0, QApplication::UnicodeUTF8));
        btnCert_Details->setText(QApplication::translate("MainWnd", "&Details >>", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabCertificates), QApplication::translate("MainWnd", "&Certificates", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabPersoData), QApplication::translate("MainWnd", "Bloco de Notas", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        tabInfo->setAccessibleName(QApplication::translate("MainWnd", "Tab Info", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblInfo_Img1->setText(QString());
        lblInfo_Top->setText(QApplication::translate("MainWnd", "Portuguese eID Runtime", 0, QApplication::UnicodeUTF8));
        lblInfo_bottom->setText(QApplication::translate("MainWnd", "Developed by the Portuguese Government", 0, QApplication::UnicodeUTF8));
        tabWidget_Identity->setTabText(tabWidget_Identity->indexOf(tabInfo), QApplication::translate("MainWnd", "In&fo", 0, QApplication::UnicodeUTF8));
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
