/********************************************************************************
** Form generated from reading ui file 'ezinstaller.ui'
**
** Created: Sun 31. Jul 19:34:10 2011
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_EZINSTALLER_H
#define UI_EZINSTALLER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCommandLinkButton>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QStackedWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ezInstallerClass
{
public:
    QLabel *lblPicEidBackground;
    QStackedWidget *stackedWidget;
    QWidget *welcomePage;
    QFrame *frame;
    QCommandLinkButton *clbFrancais;
    QCommandLinkButton *clbNederlands;
    QLabel *lblRemoveReader;
    QWidget *pageProgress;
    QProgressBar *progressBar;
    QTextEdit *teProgress;
    QLineEdit *leProgress;
    QLabel *lblTitleInstallSoftware;
    QLabel *label_3;
    QCommandLinkButton *clbViewDetails;
    QWidget *pageReaderImages;
    QLabel *lblTitleInstallSoftware_2;
    QLabel *lblConnectReaders;
    QLabel *lblPicUnconnectedReader;
    QWidget *pageConnectedReader;
    QLabel *lblPicCardReaderConnected;
    QLabel *lblTitleInstallSoftware_3;
    QLabel *lblConnectedReader;
    QWidget *pageeIDinserted;
    QLabel *lblTitleInstallSoftware_4;
    QLabel *lbleIDInserted;
    QLabel *lblPicCardReaderCardInserted;
    QWidget *pageVieweIDData;
    QLabel *lblReadingCard;
    QLabel *lblPhoto;
    QLineEdit *leName;
    QLineEdit *leFirstname;
    QLineEdit *lePlaceOfBirth;
    QLineEdit *leDateOfBirth;
    QLineEdit *leNationalNumber;
    QLabel *lblIconSummary_3;
    QLabel *lblTextSummary_4;
    QWidget *pageSummary;
    QLabel *lblTitleSummaryPage;
    QLabel *lblIconSummary_1;
    QLabel *lblIconSummary_2;
    QLabel *lblTextSummary_1;
    QLabel *lblTextSummary_2;
    QLabel *lblTextSummary_3;
    QWidget *pageReport;
    QTextEdit *teReport;
    QLabel *lblPicEID;
    QCommandLinkButton *clbBack;
    QCommandLinkButton *clbCancel;
    QLabel *lblPicM;
    QLabel *lblPicR;
    QLabel *lblPicL;
    QLabel *lblInstallSoft;
    QLabel *lblConnectReader;
    QLabel *lblViewData;
    QCommandLinkButton *clbSaveAsPdf;
    QCommandLinkButton *clbClose;
    QCommandLinkButton *clbOpenReport;

    void setupUi(QDialog *ezInstallerClass)
    {
        if (ezInstallerClass->objectName().isEmpty())
            ezInstallerClass->setObjectName(QString::fromUtf8("ezInstallerClass"));
        ezInstallerClass->resize(800, 600);
        ezInstallerClass->setMinimumSize(QSize(800, 600));
        ezInstallerClass->setMaximumSize(QSize(800, 600));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(45, 30, 255, 0));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        QBrush brush2(QColor(255, 255, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        ezInstallerClass->setPalette(palette);
        ezInstallerClass->setAutoFillBackground(true);
        lblPicEidBackground = new QLabel(ezInstallerClass);
        lblPicEidBackground->setObjectName(QString::fromUtf8("lblPicEidBackground"));
        lblPicEidBackground->setGeometry(QRect(176, 105, 613, 445));
        stackedWidget = new QStackedWidget(ezInstallerClass);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(0, 160, 800, 351));
        stackedWidget->setFocusPolicy(Qt::NoFocus);
        stackedWidget->setFrameShape(QFrame::Box);
        stackedWidget->setFrameShadow(QFrame::Plain);
        stackedWidget->setLineWidth(0);
        welcomePage = new QWidget();
        welcomePage->setObjectName(QString::fromUtf8("welcomePage"));
        frame = new QFrame(welcomePage);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, -10, 801, 351));
        QPalette palette1;
        frame->setPalette(palette1);
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Plain);
        frame->setLineWidth(0);
        clbFrancais = new QCommandLinkButton(frame);
        clbFrancais->setObjectName(QString::fromUtf8("clbFrancais"));
        clbFrancais->setGeometry(QRect(430, 170, 294, 110));
        QPalette palette2;
        QBrush brush3(QColor(255, 170, 0, 0));
        brush3.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::AlternateBase, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush3);
        clbFrancais->setPalette(palette2);
        QFont font;
        font.setFamily(QString::fromUtf8("Segoe UI"));
        font.setPointSize(14);
        font.setBold(true);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(75);
        font.setStrikeOut(false);
        clbFrancais->setFont(font);
        clbFrancais->setCursor(QCursor(Qt::PointingHandCursor));
        clbFrancais->setFocusPolicy(Qt::TabFocus);
#ifndef QT_NO_ACCESSIBILITY
        clbFrancais->setAccessibleName(QString::fromUtf8("Bouton Installez le logiciel eID"));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        clbFrancais->setAccessibleDescription(QString::fromUtf8(""));
#endif // QT_NO_ACCESSIBILITY
        clbFrancais->setIconSize(QSize(294, 94));
        clbFrancais->setAutoDefault(false);
        clbFrancais->setFlat(false);
        clbNederlands = new QCommandLinkButton(frame);
        clbNederlands->setObjectName(QString::fromUtf8("clbNederlands"));
        clbNederlands->setGeometry(QRect(60, 165, 294, 110));
        clbNederlands->setFont(font);
        clbNederlands->setCursor(QCursor(Qt::PointingHandCursor));
        clbNederlands->setFocusPolicy(Qt::TabFocus);
#ifndef QT_NO_ACCESSIBILITY
        clbNederlands->setAccessibleName(QString::fromUtf8("Installeer de eID software knop"));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        clbNederlands->setAccessibleDescription(QString::fromUtf8(""));
#endif // QT_NO_ACCESSIBILITY
        clbNederlands->setIconSize(QSize(294, 94));
        clbNederlands->setAutoDefault(false);
        clbNederlands->setFlat(false);
        lblRemoveReader = new QLabel(frame);
        lblRemoveReader->setObjectName(QString::fromUtf8("lblRemoveReader"));
        lblRemoveReader->setGeometry(QRect(20, 280, 761, 71));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblRemoveReader->sizePolicy().hasHeightForWidth());
        lblRemoveReader->setSizePolicy(sizePolicy);
        QPalette palette3;
        QBrush brush4(QColor(0, 128, 64, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::Text, brush4);
        palette3.setBrush(QPalette::Inactive, QPalette::Text, brush4);
        QBrush brush5(QColor(127, 127, 127, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Disabled, QPalette::Text, brush5);
        lblRemoveReader->setPalette(palette3);
        QFont font1;
        font1.setFamily(QString::fromUtf8("Tahoma"));
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        lblRemoveReader->setFont(font1);
        lblRemoveReader->setMouseTracking(true);
        lblRemoveReader->setFocusPolicy(Qt::StrongFocus);
        lblRemoveReader->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        lblRemoveReader->setWordWrap(false);
        stackedWidget->addWidget(welcomePage);
        pageProgress = new QWidget();
        pageProgress->setObjectName(QString::fromUtf8("pageProgress"));
        progressBar = new QProgressBar(pageProgress);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(90, 140, 631, 23));
        progressBar->setMouseTracking(true);
        progressBar->setFocusPolicy(Qt::StrongFocus);
        progressBar->setValue(0);
        progressBar->setOrientation(Qt::Horizontal);
        teProgress = new QTextEdit(pageProgress);
        teProgress->setObjectName(QString::fromUtf8("teProgress"));
        teProgress->setGeometry(QRect(20, 170, 751, 171));
        QPalette palette4;
        QBrush brush6(QColor(0, 0, 0, 255));
        brush6.setStyle(Qt::NoBrush);
        palette4.setBrush(QPalette::Active, QPalette::Base, brush6);
        QBrush brush7(QColor(0, 0, 0, 255));
        brush7.setStyle(Qt::NoBrush);
        palette4.setBrush(QPalette::Inactive, QPalette::Base, brush7);
        QBrush brush8(QColor(0, 0, 0, 255));
        brush8.setStyle(Qt::NoBrush);
        palette4.setBrush(QPalette::Disabled, QPalette::Base, brush8);
        teProgress->setPalette(palette4);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Tahoma"));
        font2.setPointSize(9);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setUnderline(false);
        font2.setWeight(50);
        font2.setStrikeOut(false);
        teProgress->setFont(font2);
        teProgress->setReadOnly(true);
        leProgress = new QLineEdit(pageProgress);
        leProgress->setObjectName(QString::fromUtf8("leProgress"));
        leProgress->setGeometry(QRect(90, 110, 621, 20));
        QPalette palette5;
        QBrush brush9(QColor(255, 255, 255, 0));
        brush9.setStyle(Qt::SolidPattern);
        palette5.setBrush(QPalette::Active, QPalette::Light, brush9);
        palette5.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette5.setBrush(QPalette::Inactive, QPalette::Light, brush9);
        palette5.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette5.setBrush(QPalette::Disabled, QPalette::Light, brush9);
        palette5.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        leProgress->setPalette(palette5);
        QFont font3;
        font3.setFamily(QString::fromUtf8("Tahoma"));
        font3.setPointSize(9);
        font3.setBold(true);
        font3.setItalic(false);
        font3.setUnderline(false);
        font3.setWeight(75);
        font3.setStrikeOut(false);
        leProgress->setFont(font3);
        leProgress->setFocusPolicy(Qt::NoFocus);
        leProgress->setAutoFillBackground(true);
        leProgress->setFrame(false);
        leProgress->setReadOnly(true);
        lblTitleInstallSoftware = new QLabel(pageProgress);
        lblTitleInstallSoftware->setObjectName(QString::fromUtf8("lblTitleInstallSoftware"));
        lblTitleInstallSoftware->setGeometry(QRect(20, 20, 261, 41));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Tahoma"));
        font4.setPointSize(14);
        font4.setBold(true);
        font4.setItalic(false);
        font4.setUnderline(false);
        font4.setWeight(75);
        font4.setStrikeOut(false);
        lblTitleInstallSoftware->setFont(font4);
        label_3 = new QLabel(pageProgress);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 60, 341, 31));
        label_3->setFont(font2);
        label_3->setFocusPolicy(Qt::StrongFocus);
        clbViewDetails = new QCommandLinkButton(pageProgress);
        clbViewDetails->setObjectName(QString::fromUtf8("clbViewDetails"));
        clbViewDetails->setGeometry(QRect(610, 20, 171, 71));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush10(QColor(118, 116, 108, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette6.setBrush(QPalette::Disabled, QPalette::WindowText, brush10);
        clbViewDetails->setPalette(palette6);
        QFont font5;
        font5.setFamily(QString::fromUtf8("Segoe UI"));
        font5.setPointSize(9);
        font5.setBold(true);
        font5.setItalic(false);
        font5.setUnderline(false);
        font5.setWeight(75);
        font5.setStrikeOut(false);
        clbViewDetails->setFont(font5);
        clbViewDetails->setFocusPolicy(Qt::NoFocus);
        clbViewDetails->setIconSize(QSize(281, 51));
        stackedWidget->addWidget(pageProgress);
        pageReaderImages = new QWidget();
        pageReaderImages->setObjectName(QString::fromUtf8("pageReaderImages"));
        lblTitleInstallSoftware_2 = new QLabel(pageReaderImages);
        lblTitleInstallSoftware_2->setObjectName(QString::fromUtf8("lblTitleInstallSoftware_2"));
        lblTitleInstallSoftware_2->setGeometry(QRect(20, 20, 231, 41));
        lblTitleInstallSoftware_2->setFont(font4);
        lblTitleInstallSoftware_2->setFocusPolicy(Qt::StrongFocus);
        lblConnectReaders = new QLabel(pageReaderImages);
        lblConnectReaders->setObjectName(QString::fromUtf8("lblConnectReaders"));
        lblConnectReaders->setGeometry(QRect(150, 70, 551, 131));
        QFont font6;
        font6.setFamily(QString::fromUtf8("Tahoma"));
        font6.setPointSize(13);
        font6.setBold(false);
        font6.setItalic(false);
        font6.setUnderline(false);
        font6.setWeight(50);
        font6.setStrikeOut(false);
        lblConnectReaders->setFont(font6);
        lblConnectReaders->setFocusPolicy(Qt::StrongFocus);
        lblConnectReaders->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lblConnectReaders->setWordWrap(true);
        lblPicUnconnectedReader = new QLabel(pageReaderImages);
        lblPicUnconnectedReader->setObjectName(QString::fromUtf8("lblPicUnconnectedReader"));
        lblPicUnconnectedReader->setGeometry(QRect(80, 60, 708, 288));
        QFont font7;
        font7.setFamily(QString::fromUtf8("Tahoma"));
        font7.setPointSize(14);
        font7.setBold(false);
        font7.setItalic(false);
        font7.setUnderline(false);
        font7.setWeight(50);
        font7.setStrikeOut(false);
        lblPicUnconnectedReader->setFont(font7);
        lblPicUnconnectedReader->setScaledContents(true);
        lblPicUnconnectedReader->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop|Qt::AlignVCenter);
        stackedWidget->addWidget(pageReaderImages);
        pageConnectedReader = new QWidget();
        pageConnectedReader->setObjectName(QString::fromUtf8("pageConnectedReader"));
        lblPicCardReaderConnected = new QLabel(pageConnectedReader);
        lblPicCardReaderConnected->setObjectName(QString::fromUtf8("lblPicCardReaderConnected"));
        lblPicCardReaderConnected->setGeometry(QRect(20, 60, 768, 327));
        lblPicCardReaderConnected->setScaledContents(true);
        lblTitleInstallSoftware_3 = new QLabel(pageConnectedReader);
        lblTitleInstallSoftware_3->setObjectName(QString::fromUtf8("lblTitleInstallSoftware_3"));
        lblTitleInstallSoftware_3->setGeometry(QRect(20, 20, 231, 41));
        lblTitleInstallSoftware_3->setFont(font4);
        lblTitleInstallSoftware_3->setFocusPolicy(Qt::StrongFocus);
        lblConnectedReader = new QLabel(pageConnectedReader);
        lblConnectedReader->setObjectName(QString::fromUtf8("lblConnectedReader"));
        lblConnectedReader->setGeometry(QRect(150, 70, 551, 131));
        lblConnectedReader->setFont(font6);
        lblConnectedReader->setFocusPolicy(Qt::TabFocus);
        lblConnectedReader->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lblConnectedReader->setWordWrap(true);
        stackedWidget->addWidget(pageConnectedReader);
        pageeIDinserted = new QWidget();
        pageeIDinserted->setObjectName(QString::fromUtf8("pageeIDinserted"));
        lblTitleInstallSoftware_4 = new QLabel(pageeIDinserted);
        lblTitleInstallSoftware_4->setObjectName(QString::fromUtf8("lblTitleInstallSoftware_4"));
        lblTitleInstallSoftware_4->setGeometry(QRect(20, 20, 231, 41));
        lblTitleInstallSoftware_4->setFont(font4);
        lblTitleInstallSoftware_4->setFocusPolicy(Qt::StrongFocus);
        lbleIDInserted = new QLabel(pageeIDinserted);
        lbleIDInserted->setObjectName(QString::fromUtf8("lbleIDInserted"));
        lbleIDInserted->setGeometry(QRect(150, 70, 551, 131));
        lbleIDInserted->setFont(font6);
        lbleIDInserted->setFocusPolicy(Qt::StrongFocus);
        lbleIDInserted->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbleIDInserted->setWordWrap(true);
        lblPicCardReaderCardInserted = new QLabel(pageeIDinserted);
        lblPicCardReaderCardInserted->setObjectName(QString::fromUtf8("lblPicCardReaderCardInserted"));
        lblPicCardReaderCardInserted->setGeometry(QRect(136, 60, 652, 288));
        lblPicCardReaderCardInserted->setScaledContents(true);
        stackedWidget->addWidget(pageeIDinserted);
        pageVieweIDData = new QWidget();
        pageVieweIDData->setObjectName(QString::fromUtf8("pageVieweIDData"));
        pageVieweIDData->setFocusPolicy(Qt::NoFocus);
        lblReadingCard = new QLabel(pageVieweIDData);
        lblReadingCard->setObjectName(QString::fromUtf8("lblReadingCard"));
        lblReadingCard->setGeometry(QRect(180, 10, 491, 61));
        QFont font8;
        font8.setFamily(QString::fromUtf8("Tahoma"));
        font8.setPointSize(12);
        font8.setBold(false);
        font8.setItalic(false);
        font8.setUnderline(false);
        font8.setWeight(50);
        font8.setStrikeOut(false);
        lblReadingCard->setFont(font8);
        lblReadingCard->setFocusPolicy(Qt::StrongFocus);
        lblReadingCard->setAcceptDrops(true);
        lblReadingCard->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lblReadingCard->setWordWrap(true);
        lblPhoto = new QLabel(pageVieweIDData);
        lblPhoto->setObjectName(QString::fromUtf8("lblPhoto"));
        lblPhoto->setGeometry(QRect(40, 80, 121, 171));
        lblPhoto->setFrameShape(QFrame::Box);
        lblPhoto->setScaledContents(true);
        lblPhoto->setWordWrap(true);
        leName = new QLineEdit(pageVieweIDData);
        leName->setObjectName(QString::fromUtf8("leName"));
        leName->setGeometry(QRect(180, 71, 581, 30));
        QPalette palette7;
        QBrush brush11(QColor(40, 61, 106, 255));
        brush11.setStyle(Qt::SolidPattern);
        palette7.setBrush(QPalette::Active, QPalette::WindowText, brush11);
        palette7.setBrush(QPalette::Active, QPalette::Text, brush11);
        QBrush brush12(QColor(40, 61, 106, 0));
        brush12.setStyle(Qt::SolidPattern);
        palette7.setBrush(QPalette::Active, QPalette::Base, brush12);
        palette7.setBrush(QPalette::Inactive, QPalette::WindowText, brush11);
        palette7.setBrush(QPalette::Inactive, QPalette::Text, brush11);
        palette7.setBrush(QPalette::Inactive, QPalette::Base, brush12);
        palette7.setBrush(QPalette::Disabled, QPalette::WindowText, brush10);
        palette7.setBrush(QPalette::Disabled, QPalette::Text, brush10);
        palette7.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        leName->setPalette(palette7);
        QFont font9;
        font9.setFamily(QString::fromUtf8("Tahoma"));
        font9.setPointSize(12);
        font9.setBold(true);
        font9.setItalic(false);
        font9.setUnderline(false);
        font9.setWeight(75);
        font9.setStrikeOut(false);
        leName->setFont(font9);
        leName->setReadOnly(true);
        leFirstname = new QLineEdit(pageVieweIDData);
        leFirstname->setObjectName(QString::fromUtf8("leFirstname"));
        leFirstname->setGeometry(QRect(180, 106, 581, 30));
        QPalette palette8;
        palette8.setBrush(QPalette::Active, QPalette::Text, brush11);
        palette8.setBrush(QPalette::Active, QPalette::Base, brush9);
        palette8.setBrush(QPalette::Inactive, QPalette::Text, brush11);
        palette8.setBrush(QPalette::Inactive, QPalette::Base, brush9);
        palette8.setBrush(QPalette::Disabled, QPalette::Text, brush10);
        palette8.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        leFirstname->setPalette(palette8);
        leFirstname->setFont(font9);
        leFirstname->setReadOnly(true);
        lePlaceOfBirth = new QLineEdit(pageVieweIDData);
        lePlaceOfBirth->setObjectName(QString::fromUtf8("lePlaceOfBirth"));
        lePlaceOfBirth->setGeometry(QRect(180, 141, 581, 30));
        QPalette palette9;
        palette9.setBrush(QPalette::Active, QPalette::Text, brush11);
        palette9.setBrush(QPalette::Active, QPalette::Base, brush9);
        palette9.setBrush(QPalette::Inactive, QPalette::Text, brush11);
        palette9.setBrush(QPalette::Inactive, QPalette::Base, brush9);
        palette9.setBrush(QPalette::Disabled, QPalette::Text, brush10);
        palette9.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        lePlaceOfBirth->setPalette(palette9);
        lePlaceOfBirth->setFont(font9);
        lePlaceOfBirth->setReadOnly(true);
        leDateOfBirth = new QLineEdit(pageVieweIDData);
        leDateOfBirth->setObjectName(QString::fromUtf8("leDateOfBirth"));
        leDateOfBirth->setGeometry(QRect(180, 176, 581, 30));
        QPalette palette10;
        palette10.setBrush(QPalette::Active, QPalette::Text, brush11);
        palette10.setBrush(QPalette::Active, QPalette::Base, brush9);
        palette10.setBrush(QPalette::Inactive, QPalette::Text, brush11);
        palette10.setBrush(QPalette::Inactive, QPalette::Base, brush9);
        palette10.setBrush(QPalette::Disabled, QPalette::Text, brush10);
        palette10.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        leDateOfBirth->setPalette(palette10);
        leDateOfBirth->setFont(font9);
        leDateOfBirth->setReadOnly(true);
        leNationalNumber = new QLineEdit(pageVieweIDData);
        leNationalNumber->setObjectName(QString::fromUtf8("leNationalNumber"));
        leNationalNumber->setGeometry(QRect(180, 211, 581, 30));
        QPalette palette11;
        palette11.setBrush(QPalette::Active, QPalette::Text, brush11);
        palette11.setBrush(QPalette::Active, QPalette::Base, brush9);
        palette11.setBrush(QPalette::Inactive, QPalette::Text, brush11);
        palette11.setBrush(QPalette::Inactive, QPalette::Base, brush9);
        palette11.setBrush(QPalette::Disabled, QPalette::Text, brush10);
        palette11.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        leNationalNumber->setPalette(palette11);
        leNationalNumber->setFont(font9);
        leNationalNumber->setReadOnly(true);
        lblIconSummary_3 = new QLabel(pageVieweIDData);
        lblIconSummary_3->setObjectName(QString::fromUtf8("lblIconSummary_3"));
        lblIconSummary_3->setGeometry(QRect(40, 270, 71, 72));
        lblTextSummary_4 = new QLabel(pageVieweIDData);
        lblTextSummary_4->setObjectName(QString::fromUtf8("lblTextSummary_4"));
        lblTextSummary_4->setGeometry(QRect(180, 280, 581, 61));
        lblTextSummary_4->setFont(font8);
        lblTextSummary_4->setFocusPolicy(Qt::StrongFocus);
        lblTextSummary_4->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lblTextSummary_4->setWordWrap(true);
        stackedWidget->addWidget(pageVieweIDData);
        pageSummary = new QWidget();
        pageSummary->setObjectName(QString::fromUtf8("pageSummary"));
        lblTitleSummaryPage = new QLabel(pageSummary);
        lblTitleSummaryPage->setObjectName(QString::fromUtf8("lblTitleSummaryPage"));
        lblTitleSummaryPage->setGeometry(QRect(20, 10, 231, 41));
        lblTitleSummaryPage->setFont(font4);
        lblIconSummary_1 = new QLabel(pageSummary);
        lblIconSummary_1->setObjectName(QString::fromUtf8("lblIconSummary_1"));
        lblIconSummary_1->setGeometry(QRect(30, 60, 71, 72));
        lblIconSummary_2 = new QLabel(pageSummary);
        lblIconSummary_2->setObjectName(QString::fromUtf8("lblIconSummary_2"));
        lblIconSummary_2->setGeometry(QRect(30, 150, 71, 72));
        lblTextSummary_1 = new QLabel(pageSummary);
        lblTextSummary_1->setObjectName(QString::fromUtf8("lblTextSummary_1"));
        lblTextSummary_1->setGeometry(QRect(120, 60, 641, 81));
        lblTextSummary_1->setFont(font8);
        lblTextSummary_1->setFocusPolicy(Qt::StrongFocus);
        lblTextSummary_1->setWordWrap(true);
        lblTextSummary_2 = new QLabel(pageSummary);
        lblTextSummary_2->setObjectName(QString::fromUtf8("lblTextSummary_2"));
        lblTextSummary_2->setGeometry(QRect(120, 150, 641, 101));
        lblTextSummary_2->setFont(font8);
        lblTextSummary_2->setFocusPolicy(Qt::StrongFocus);
        lblTextSummary_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lblTextSummary_2->setWordWrap(true);
        lblTextSummary_3 = new QLabel(pageSummary);
        lblTextSummary_3->setObjectName(QString::fromUtf8("lblTextSummary_3"));
        lblTextSummary_3->setGeometry(QRect(100, 310, 641, 51));
        lblTextSummary_3->setFont(font8);
        lblTextSummary_3->setFocusPolicy(Qt::StrongFocus);
        lblTextSummary_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lblTextSummary_3->setWordWrap(true);
        stackedWidget->addWidget(pageSummary);
        pageReport = new QWidget();
        pageReport->setObjectName(QString::fromUtf8("pageReport"));
        teReport = new QTextEdit(pageReport);
        teReport->setObjectName(QString::fromUtf8("teReport"));
        teReport->setGeometry(QRect(13, 33, 771, 311));
        teReport->setReadOnly(true);
        stackedWidget->addWidget(pageReport);
        lblPicEID = new QLabel(ezInstallerClass);
        lblPicEID->setObjectName(QString::fromUtf8("lblPicEID"));
        lblPicEID->setGeometry(QRect(92, 32, 72, 53));
        clbBack = new QCommandLinkButton(ezInstallerClass);
        clbBack->setObjectName(QString::fromUtf8("clbBack"));
        clbBack->setEnabled(false);
        clbBack->setGeometry(QRect(7, 20, 72, 72));
        clbBack->setBaseSize(QSize(62, 62));
        QPalette palette12;
        QBrush brush13(QColor(0, 85, 0, 0));
        brush13.setStyle(Qt::SolidPattern);
        palette12.setBrush(QPalette::Active, QPalette::WindowText, brush13);
        palette12.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette12.setBrush(QPalette::Active, QPalette::Text, brush13);
        palette12.setBrush(QPalette::Active, QPalette::ButtonText, brush13);
        palette12.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette12.setBrush(QPalette::Active, QPalette::Window, brush1);
        QBrush brush14(QColor(170, 0, 0, 0));
        brush14.setStyle(Qt::SolidPattern);
        palette12.setBrush(QPalette::Active, QPalette::Highlight, brush14);
        palette12.setBrush(QPalette::Active, QPalette::HighlightedText, brush14);
        palette12.setBrush(QPalette::Active, QPalette::AlternateBase, brush3);
        palette12.setBrush(QPalette::Inactive, QPalette::WindowText, brush13);
        palette12.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::Text, brush13);
        palette12.setBrush(QPalette::Inactive, QPalette::ButtonText, brush13);
        palette12.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette12.setBrush(QPalette::Inactive, QPalette::Highlight, brush14);
        palette12.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush14);
        palette12.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush3);
        palette12.setBrush(QPalette::Disabled, QPalette::WindowText, brush13);
        palette12.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::Text, brush13);
        palette12.setBrush(QPalette::Disabled, QPalette::ButtonText, brush13);
        palette12.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette12.setBrush(QPalette::Disabled, QPalette::Highlight, brush14);
        palette12.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush14);
        palette12.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush3);
        clbBack->setPalette(palette12);
        clbBack->setCursor(QCursor(Qt::PointingHandCursor));
        clbBack->setFocusPolicy(Qt::TabFocus);
        clbBack->setAutoFillBackground(false);
        clbBack->setIconSize(QSize(62, 62));
        clbBack->setAutoDefault(false);
        clbBack->setFlat(false);
        clbCancel = new QCommandLinkButton(ezInstallerClass);
        clbCancel->setObjectName(QString::fromUtf8("clbCancel"));
        clbCancel->setGeometry(QRect(642, -3, 95, 55));
        clbCancel->setCursor(QCursor(Qt::PointingHandCursor));
        clbCancel->setFocusPolicy(Qt::TabFocus);
        clbCancel->setIconSize(QSize(85, 40));
        clbCancel->setAutoDefault(false);
        lblPicM = new QLabel(ezInstallerClass);
        lblPicM->setObjectName(QString::fromUtf8("lblPicM"));
        lblPicM->setGeometry(QRect(270, 105, 260, 56));
        lblPicM->setScaledContents(true);
        lblPicR = new QLabel(ezInstallerClass);
        lblPicR->setObjectName(QString::fromUtf8("lblPicR"));
        lblPicR->setGeometry(QRect(530, 105, 259, 79));
        lblPicR->setScaledContents(true);
        lblPicL = new QLabel(ezInstallerClass);
        lblPicL->setObjectName(QString::fromUtf8("lblPicL"));
        lblPicL->setGeometry(QRect(11, 105, 259, 79));
        lblInstallSoft = new QLabel(ezInstallerClass);
        lblInstallSoft->setObjectName(QString::fromUtf8("lblInstallSoft"));
        lblInstallSoft->setGeometry(QRect(40, 100, 221, 61));
        QPalette palette13;
        palette13.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette13.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette13.setBrush(QPalette::Disabled, QPalette::WindowText, brush10);
        lblInstallSoft->setPalette(palette13);
        QFont font10;
        font10.setFamily(QString::fromUtf8("Tahoma"));
        font10.setPointSize(13);
        font10.setBold(true);
        font10.setItalic(false);
        font10.setUnderline(false);
        font10.setWeight(75);
        font10.setStrikeOut(false);
        lblInstallSoft->setFont(font10);
        lblInstallSoft->setFocusPolicy(Qt::NoFocus);
        lblInstallSoft->setAlignment(Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        lblConnectReader = new QLabel(ezInstallerClass);
        lblConnectReader->setObjectName(QString::fromUtf8("lblConnectReader"));
        lblConnectReader->setGeometry(QRect(290, 100, 221, 61));
        QPalette palette14;
        palette14.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette14.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette14.setBrush(QPalette::Disabled, QPalette::WindowText, brush10);
        lblConnectReader->setPalette(palette14);
        lblConnectReader->setFont(font10);
        lblConnectReader->setAlignment(Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        lblViewData = new QLabel(ezInstallerClass);
        lblViewData->setObjectName(QString::fromUtf8("lblViewData"));
        lblViewData->setGeometry(QRect(560, 100, 221, 61));
        QPalette palette15;
        palette15.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette15.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette15.setBrush(QPalette::Disabled, QPalette::WindowText, brush10);
        lblViewData->setPalette(palette15);
        lblViewData->setFont(font10);
        lblViewData->setAlignment(Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        clbSaveAsPdf = new QCommandLinkButton(ezInstallerClass);
        clbSaveAsPdf->setObjectName(QString::fromUtf8("clbSaveAsPdf"));
        clbSaveAsPdf->setGeometry(QRect(10, 510, 176, 71));
        QFont font11;
        font11.setFamily(QString::fromUtf8("Segoe UI"));
        font11.setPointSize(12);
        font11.setBold(true);
        font11.setItalic(false);
        font11.setUnderline(false);
        font11.setWeight(75);
        font11.setStrikeOut(false);
        clbSaveAsPdf->setFont(font11);
        clbSaveAsPdf->setCursor(QCursor(Qt::PointingHandCursor));
        clbSaveAsPdf->setFocusPolicy(Qt::TabFocus);
        clbSaveAsPdf->setIconSize(QSize(166, 65));
        clbSaveAsPdf->setAutoDefault(false);
        clbClose = new QCommandLinkButton(ezInstallerClass);
        clbClose->setObjectName(QString::fromUtf8("clbClose"));
        clbClose->setGeometry(QRect(580, 510, 176, 75));
        clbClose->setFont(font11);
        clbClose->setCursor(QCursor(Qt::PointingHandCursor));
        clbClose->setFocusPolicy(Qt::TabFocus);
        clbClose->setIconSize(QSize(166, 65));
        clbClose->setAutoDefault(false);
        clbOpenReport = new QCommandLinkButton(ezInstallerClass);
        clbOpenReport->setObjectName(QString::fromUtf8("clbOpenReport"));
        clbOpenReport->setGeometry(QRect(10, 510, 176, 75));
        clbOpenReport->setFont(font11);
        clbOpenReport->setCursor(QCursor(Qt::PointingHandCursor));
        clbOpenReport->setFocusPolicy(Qt::TabFocus);
        clbOpenReport->setIconSize(QSize(166, 65));
        clbOpenReport->setAutoDefault(false);
        QWidget::setTabOrder(clbBack, clbCancel);
        QWidget::setTabOrder(clbCancel, clbNederlands);
        QWidget::setTabOrder(clbNederlands, clbFrancais);
        QWidget::setTabOrder(clbFrancais, label_3);
        QWidget::setTabOrder(label_3, progressBar);
        QWidget::setTabOrder(progressBar, teProgress);
        QWidget::setTabOrder(teProgress, lblTitleInstallSoftware_2);
        QWidget::setTabOrder(lblTitleInstallSoftware_2, lblTitleInstallSoftware_3);
        QWidget::setTabOrder(lblTitleInstallSoftware_3, lblReadingCard);
        QWidget::setTabOrder(lblReadingCard, lblConnectedReader);
        QWidget::setTabOrder(lblConnectedReader, leName);
        QWidget::setTabOrder(leName, leFirstname);
        QWidget::setTabOrder(leFirstname, lePlaceOfBirth);
        QWidget::setTabOrder(lePlaceOfBirth, leDateOfBirth);
        QWidget::setTabOrder(leDateOfBirth, leNationalNumber);
        QWidget::setTabOrder(leNationalNumber, lblTextSummary_4);
        QWidget::setTabOrder(lblTextSummary_4, teReport);
        QWidget::setTabOrder(teReport, lblRemoveReader);
        QWidget::setTabOrder(lblRemoveReader, lblConnectReaders);
        QWidget::setTabOrder(lblConnectReaders, lblTitleInstallSoftware_4);
        QWidget::setTabOrder(lblTitleInstallSoftware_4, lbleIDInserted);
        QWidget::setTabOrder(lbleIDInserted, lblTextSummary_1);
        QWidget::setTabOrder(lblTextSummary_1, lblTextSummary_2);
        QWidget::setTabOrder(lblTextSummary_2, lblTextSummary_3);
        QWidget::setTabOrder(lblTextSummary_3, clbOpenReport);
        QWidget::setTabOrder(clbOpenReport, clbSaveAsPdf);
        QWidget::setTabOrder(clbSaveAsPdf, clbClose);

        retranslateUi(ezInstallerClass);

        stackedWidget->setCurrentIndex(5);


        QMetaObject::connectSlotsByName(ezInstallerClass);
    } // setupUi

    void retranslateUi(QDialog *ezInstallerClass)
    {
        ezInstallerClass->setWindowTitle(QApplication::translate("ezInstallerClass", "e-ID Quick Installer", 0, QApplication::UnicodeUTF8));
        lblPicEidBackground->setText(QApplication::translate("ezInstallerClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        clbFrancais->setText(QApplication::translate("ezInstallerClass", "Installez le logiciel eID", 0, QApplication::UnicodeUTF8));
        clbNederlands->setText(QApplication::translate("ezInstallerClass", "Installeer de eID software", 0, QApplication::UnicodeUTF8));
        lblRemoveReader->setText(QApplication::translate("ezInstallerClass", "xxx", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        progressBar->setAccessibleName(QApplication::translate("ezInstallerClass", "vooruitgangsindicator", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        progressBar->setStyleSheet(QApplication::translate("ezInstallerClass", "background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 green, stop: 1 white);", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblTitleInstallSoftware->setAccessibleName(QApplication::translate("ezInstallerClass", "eID software installeren", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblTitleInstallSoftware->setText(QApplication::translate("ezInstallerClass", "eID software installeren", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        label_3->setAccessibleName(QApplication::translate("ezInstallerClass", "Even geduld terwijl de eID software ge\303\257nstalleerd wordt...", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        label_3->setText(QApplication::translate("ezInstallerClass", "Even geduld terwijl de eID software ge\303\257nstalleerd wordt...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        clbViewDetails->setAccessibleName(QApplication::translate("ezInstallerClass", "Toon details", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        clbViewDetails->setText(QApplication::translate("ezInstallerClass", "Toon details", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblTitleInstallSoftware_2->setAccessibleName(QApplication::translate("ezInstallerClass", "Kaartlezer verbinden", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblTitleInstallSoftware_2->setText(QApplication::translate("ezInstallerClass", "Kaartlezer verbinden", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblConnectReaders->setAccessibleName(QApplication::translate("ezInstallerClass", "Verbind nu je kaartlezer(s) met de computer", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblConnectReaders->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:14pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Tahoma'; font-size:12pt; font-weight:600;\">De eID software is succesvol ge\303\257nstalleerd</span><span style=\" font-family:'Tahoma'; font-size:12pt;\">. De Quick Installer zal vervolgens controleren of de kaartlezer correct functioneert.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Tahoma'; font-size:12pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-"
                        "indent:0; text-indent:0px;\"><span style=\" font-family:'Tahoma'; font-size:12pt; font-weight:600;\">Verbind nu je kaartlezer(s) met de computer.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblPicUnconnectedReader->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:14pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:12pt;\">Hier moet een afbeelding komen van een computer + kaartlezer losgekoppeld.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblPicCardReaderConnected->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:12pt;\">Hier moet een afbeelding komen van een computer + kaartlezer aangekoppeld.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblTitleInstallSoftware_3->setAccessibleName(QApplication::translate("ezInstallerClass", "Kaart uitlezen", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblTitleInstallSoftware_3->setText(QApplication::translate("ezInstallerClass", "Kaart uitlezen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblConnectedReader->setAccessibleName(QApplication::translate("ezInstallerClass", "Steek de elektronische identiteitskaart in een kaartlezer.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblConnectedReader->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:14pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">De kaartlezer is succesvol met de computer verbonden.</span><span style=\" font-size:12pt;\"> </span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Steek de elektronische identiteitskaart in een kaartlezer.</span></p"
                        "></body></html>", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblTitleInstallSoftware_4->setAccessibleName(QApplication::translate("ezInstallerClass", "Kaart lezen", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblTitleInstallSoftware_4->setText(QApplication::translate("ezInstallerClass", "Kaart lezen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lbleIDInserted->setAccessibleName(QApplication::translate("ezInstallerClass", "De Quick Installer heeft de elektronische identiteitskaart gevonden en is nu klaar om de gegevens te lezen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lbleIDInserted->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:14pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:9pt; font-weight:400;\"><span style=\" font-size:12pt; font-weight:600;\">De Quick Installer heeft de elektronische identiteitskaart gevonden</span><span style=\" font-size:12pt;\"> en is nu klaar om de gegevens te lezen. </span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt; font-weight:400;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt; f"
                        "ont-weight:400;\">Klik op <span style=\" font-weight:600;\">Volgende</span>.</p></body></html>", 0, QApplication::UnicodeUTF8));
        lblPicCardReaderCardInserted->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:12pt;\">Hier moet een afbeelding komen van een computer + kaartlezer aangekoppeld + eID erin.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblReadingCard->setAccessibleName(QApplication::translate("ezInstallerClass", "De kaartlezer werkt! De Quick Installer leest nu de belangrijkste gegevens van je identiteitskaart:", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblReadingCard->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; font-weight:600;\">De kaartlezer werkt</span><span style=\" font-size:14pt;\">! De Quick Installer leest nu de belangrijkste gegevens van je identiteitskaart:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblIconSummary_3->setText(QString());
        lblTextSummary_4->setText(QString());
#ifndef QT_NO_ACCESSIBILITY
        lblTitleSummaryPage->setAccessibleName(QApplication::translate("ezInstallerClass", "Installatie voltooien", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        lblTitleSummaryPage->setAccessibleDescription(QString());
#endif // QT_NO_ACCESSIBILITY
        lblTitleSummaryPage->setText(QApplication::translate("ezInstallerClass", "Installatie voltooien", 0, QApplication::UnicodeUTF8));
        lblIconSummary_1->setText(QApplication::translate("ezInstallerClass", "Icon1", 0, QApplication::UnicodeUTF8));
        lblIconSummary_2->setText(QApplication::translate("ezInstallerClass", "Icon2", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblTextSummary_1->setAccessibleName(QApplication::translate("ezInstallerClass", "De Quick Installer kan de identiteitskaart niet vinden. Keer terug naar de vorige pagina en probeer het nogmaals. Als dat niet werkt kun je de servicedesk contacteren.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        lblTextSummary_1->setAccessibleDescription(QString());
#endif // QT_NO_ACCESSIBILITY
        lblTextSummary_1->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">De Quick Installer kan de identiteitskaart niet vinden</span>. Keer terug naar de vorige pagina en probeer het nogmaals. Als dat niet werkt kun je de servicedesk contacteren.</p></body></html>", 0, QApplication::UnicodeUTF8));
        lblTextSummary_2->setText(QApplication::translate("ezInstallerClass", "TextLabel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblTextSummary_3->setAccessibleName(QApplication::translate("ezInstallerClass", "Open het rapport en contacteer de Servicedesk om het probleem op te lossen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        lblTextSummary_3->setAccessibleDescription(QString());
#endif // QT_NO_ACCESSIBILITY
        lblTextSummary_3->setText(QApplication::translate("ezInstallerClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:12pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Open het rapport en contacteer de Servicedesk om het probleem op te lossen.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        teReport->setAccessibleName(QApplication::translate("ezInstallerClass", "test rapport", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        clbBack->setAccessibleName(QApplication::translate("ezInstallerClass", "terug knop", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        clbBack->setStyleSheet(QApplication::translate("ezInstallerClass", "color: rgba(0, 85, 0, 0);\n"
"background-color: rgb(45, 30, 255,0);\n"
"alternate-background-color: rgb(255, 170, 0,0);\n"
"border-color: rgba(255, 255, 255, 0);\n"
"border-top-color: rgba(255, 255, 255, 0);\n"
"border-right-color: rgba(255, 255, 255, 0);\n"
"border-left-color: rgba(255, 255, 255, 0);\n"
"border-bottom-color: rgba(0, 170, 255, 0);\n"
"gridline-color: rgba(170, 0, 0, 0);\n"
"selection-color: rgba(170, 0, 0, 0);\n"
"selection-background-color: rgba(170, 0, 0, 0);\n"
"\n"
"", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        clbCancel->setAccessibleName(QApplication::translate("ezInstallerClass", "annuleer knop", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        lblInstallSoft->setAccessibleName(QApplication::translate("ezInstallerClass", "eID software installeren", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblInstallSoft->setText(QApplication::translate("ezInstallerClass", "eID software installeren", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblConnectReader->setAccessibleName(QApplication::translate("ezInstallerClass", "Kaartlezer verbinden", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblConnectReader->setText(QApplication::translate("ezInstallerClass", "Kaartlezer verbinden", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        lblViewData->setAccessibleName(QApplication::translate("ezInstallerClass", "Gegevens bekijken", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        lblViewData->setAccessibleDescription(QApplication::translate("ezInstallerClass", "Gegevens bekijken", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        lblViewData->setText(QApplication::translate("ezInstallerClass", "Gegevens bekijken", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        clbSaveAsPdf->setAccessibleName(QApplication::translate("ezInstallerClass", "Opslaan als pdf knop", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        clbSaveAsPdf->setText(QApplication::translate("ezInstallerClass", "Opslaan als pdf", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        clbClose->setAccessibleName(QApplication::translate("ezInstallerClass", "Sluit knop", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        clbClose->setText(QApplication::translate("ezInstallerClass", "Sluit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        clbOpenReport->setAccessibleName(QApplication::translate("ezInstallerClass", "Open rapport knop", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        clbOpenReport->setAccessibleDescription(QString());
#endif // QT_NO_ACCESSIBILITY
        clbOpenReport->setText(QApplication::translate("ezInstallerClass", "Open rapport", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ezInstallerClass);
    } // retranslateUi

};

namespace Ui {
    class ezInstallerClass: public Ui_ezInstallerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EZINSTALLER_H
