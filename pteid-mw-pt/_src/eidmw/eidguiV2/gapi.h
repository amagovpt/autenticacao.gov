/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2018-2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef GAPI_H
#define GAPI_H

#include <QQuickImageProvider>
#include <QQmlEngine>
#include <QObject>
#include <QMap>
#include <QTimer>
#include <QDebug>
#include <QtQml>
#include <QPixmap>
#include <poppler-qt5.h>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include "Settings.h"
#include "certificates.h"
#include "cmdCertificates.h"
#include <unordered_map>

//MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"
#include "PDFSignature.h"

#include "CMDSignature.h"
#include "cmdErrors.h"

#include "scapsignature.h"
#include "../dialogs/dialogs.h"

#include "OAuthAttributes.h"
#include "AttributeFactory.h"

#define SUCCESS_EXIT_CODE 0
#define RESTART_EXIT_CODE  1

/* For filenames we need to maintain latin-1 or UTF-8 native encoding */
//This macro's argument is a QString

//#define getPlatformNativeString(s) s.toLatin1().constData()

#define getPlatformNativeString(s) s.toUtf8().constData()


/*
    GAPI - Graphic Application Programming Interface

*/

class GAPI;

//***********************************************
// callback data class
// This class can be used to store whatever info we
// need from the callback function
//***********************************************
class CallBackData
{
public:
    CallBackData( void )
        : m_readerName()
        , m_mainWnd()
        , m_cardID(0)
    {
    }
    CallBackData( const char* pReaderName, GAPI* pGAPI )
        : m_readerName(pReaderName)
        , m_mainWnd(pGAPI)
        , m_cardID(0)
    {
    }
    virtual ~CallBackData( void )
    {
    }
    QString const& getReaderName()
    {
        return m_readerName;
    }
    void setReaderName( QString const& readerName)
    {
        m_readerName = readerName;
    }
    void setReaderName( const char* readerName)
    {
        m_readerName = readerName;
    }
    GAPI* getMainWnd( void )
    {
        return m_mainWnd;
    }
private:
    QString m_readerName;
    GAPI*   m_mainWnd;
public:
    unsigned long   m_cardID;
};

typedef QMap<QString,unsigned long> tCallBackHandles;
typedef QMap<QString,CallBackData*> tCallBackData;

class PhotoImageProvider: public QQuickImageProvider 
{
public:
    PhotoImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    void setPixmap(QPixmap &pixmap) { p = pixmap; }

private:
    QPixmap p;
};

struct PrintParams {
public:
    QString outputFile;
    bool isBasicInfo;
    bool isAddicionalInfo;
    bool isAddress;
    bool isNotes;
    bool isPrintDate;
    bool isSign;
};

struct SignParams {
public:
    QList<QString> loadedFilePaths;
    QString outputFile;
    int page;
    double coord_x;
    double coord_y;
    QString reason;
    QString location;
    bool isTimestamp;
    bool isLtv;
    bool isSmallSignature;
};

struct CmdParams {
public:
    QString mobileNumber;
    QString secret_code;
};

struct CmdSignedFileDetails {
public:
    QString signedCMDFile;
    QString citizenName;
    QString citizenId;
};

struct SCAPSignParams {
public:
    QString inputPDF;
    QString outputPDF;
    int page; 
	double location_x;
	double location_y;
	QString reason;
    QString location;
    bool isTimestamp;
	bool isLtv;
    QList<int> attribute_index;
};

class PDFPreviewImageProvider: public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    PDFPreviewImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) { }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    //Returns page size in postscript points
    QSize getPageSize(int page);
    
    void closeDoc(QString filePath);
    void closeAllDocs();
signals:
    Q_SIGNAL void signalPdfSourceChanged(int pdfWidth, int pdfHeight);
private:
    void doCloseDoc(QString filePath);
    void doCloseAllDocs();
    QPixmap renderPdf(int page,const QSize &requestedSize);
    QPixmap renderPDFPage(unsigned int page);
    std::unordered_map<std::string, Poppler::Document *> m_docs; // all loaded pdfs (remain open)
    std::string m_filePath; // file in preview
    QMutex renderMutex;
};


class GAPI : public QObject
{
    #define TIMERREADERLIST 5000
    Q_OBJECT
    Q_PROPERTY(QMap<IDInfoKey, QString> m_data
               NOTIFY signalCardDataChanged)
    Q_PROPERTY(QMap<AddressInfoKey, QString> m_addressData NOTIFY signalAddressLoaded)
    Q_PROPERTY(QString persoData MEMBER m_persoData NOTIFY signalPersoDataLoaded)
    Q_PROPERTY(bool isAddressLoaded READ isAddressLoaded WRITE setAddressLoaded NOTIFY signalAddressLoadedChanged)

public:
    explicit GAPI(QObject *parent = 0);

    //bool isIdentityLoaded;
    //bool isPersoDateLoaded;
    //bool isAddressLoaded;

    enum IDInfoKey { Documenttype, Documentversion, Surname, Givenname, Sex, Height, Nationality, Birthdate, Documentnum, Validitybegindate, Validityenddate,
                     NIC, NIF, NISS, NSNS, IssuingEntity, PlaceOfRequest, Country, Father, Mother, AccidentalIndications };

    enum AddressInfoKey { District, Municipality, Parish, Streettype, Streetname, Buildingtype, Doorno, Floor, Side, Locality, Place, Zip4, Zip3, PostalLocality,
                          Foreigncountry, Foreignaddress, Foreigncity, Foreignregion, Foreignlocality, Foreignpostalcode};

    enum CardAccessError { NoReaderFound, NoCardFound, CardUnknownCard, PinBlocked, SodCardReadError, CardUserPinCancel, CardUnknownError, CardPinTimeout, IncompatibleReader };

    enum SignMessage { SignMessageOK, SignMessageTimestampFailed, SignMessageLtvFailed, SignFilePermissionFailed, PDFFileUnsupported};

    enum PrintMessage {NoPrinterAvailable};

    enum eCustomEventType { ET_UNKNOWN, ET_CARD_CHANGED, ET_CARD_REMOVED };

    enum AutoUpdateMessage {GenericError, NoUpdatesAvailable, DownloadFailed, DownloadCancelled, LinuxNotSupported, UpdatesAvailable,
                           UnableSaveFile, InstallFailed, NetworkError};

    enum AutoUpdateType {AutoUpdateNoExist, AutoUpdateApp, AutoUpdateCerts, AutoUpdateNews};

    enum ScapPdfSignResult { ScapTimeOutError, ScapGenericError, ScapAttributesExpiredError, ScapZeroAttributesError,
                             ScapNotValidAttributesError, ScapClockError, ScapSecretKeyError, ScapMultiEntityError, ScapSucess };

    enum ScapAttrType {ScapAttrEntities, ScapAttrCompanies, ScapAttrAll};
    enum ScapAttrDescription {ScapAttrDescriptionShort, ScapAttrDescriptionLong};

    enum PinUsage { AuthPin, SignPin, AddressPin };

    enum CmdDialogClass { Sign, RegisterCert, AskToRegisterCert };

    enum ShortcutId { ShortcutIdNone, ShortcutIdSignSimple, ShortcutIdSignAdvanced};

    enum SignLevel { LevelBasic, LevelTimestamp, LevelLTV };

    Q_ENUMS(ScapPdfSignResult)
    Q_ENUMS(ScapAttrType)
    Q_ENUMS(ScapAttrDescription)
    Q_ENUMS(CardAccessError)
    Q_ENUMS(eCustomEventType)
    Q_ENUMS(IDInfoKey)
    Q_ENUMS(AddressInfoKey)
    Q_ENUMS(UI_LANGUAGE)
    Q_ENUMS(SignMessage)
    Q_ENUMS(PrintMessage)
    Q_ENUMS(AutoUpdateMessage)
    Q_ENUMS(AutoUpdateType)
    Q_ENUMS(PinUsage)
    Q_ENUMS(CmdDialogClass)
    Q_ENUMS(SignatureLevel)
    Q_ENUMS(ShortcutId)

    bool isAddressLoaded() {return m_addressLoaded; }

    QQuickImageProvider * buildImageProvider() { return image_provider; }
    QQuickImageProvider * buildPdfImageProvider() { return image_provider_pdf; }

    PDFPreviewImageProvider * image_provider_pdf;

    // Do not forget to declare your class to the QML system.
    static void declareQMLTypes() {
        qmlRegisterType<GAPI>("eidguiV2", 1, 0, "GAPI");
    }
    GUISettings    m_Settings;
    CERTIFICATES    m_Certificates;
    CMDCertificates *m_cmdCertificates;

public slots:
    // Slots to Gui request values
    QVariantList getRetReaderList(void);
    int getReaderIndex(void);
    void setReaderByUser(unsigned long setReaderIndex);
    void resetReaderSelected(void) {selectedReaderIndex =  -1; }
    signed int returnReaderSelected(void) {return selectedReaderIndex; }
    void setAddressLoaded(bool addressLoaded) {m_addressLoaded = addressLoaded; }
    void startCardReading();
    void startSavingCardPhoto(QString outputFile);
    int getStringByteLength(QString text);
    void startReadingPersoNotes();
    void startWritingPersoNotes(QString text);
    void startReadingAddress();
    void startPrintPDF(QString outputFile, bool isBasicInfo,bool isAdditionalInfo,
                       bool isAddress,bool isNotes,bool isPrintDate,bool isSign);
    void startPrint(QString outputFile, bool isBasicInfo,bool isAdditionalInfo,
                       bool isAddress, bool isNotes, bool isPrintDate, bool isSign);
    //This method should be used by basic and advanced signature modes
    void startSigningPDF(QString loadedFilePath, QString outputFile, int page, double coord_x, double coord_y,
                         QString reason, QString location, bool isTimestamp, bool isLtv, bool isSmall);
    void startSigningBatchPDF(QList<QString> loadedFileBatchPath, QString outputFile, int page, double coord_x, double coord_y,
                         QString reason, QString location, bool isTimestamp, bool isLtv, bool isSmall);
    int getPDFpageCount(QString loadedFilePath);
    void closePdfPreview(QString filePath);
    void closeAllPdfPreviews();

	void startSigningXADES(QString loadedFilePath, QString outputFile, bool isTimestamp);
	void startSigningBatchXADES(QList<QString> loadedFileBatchPath, QString outputFile, bool isTimestamp);

    //This flag is used to start the application in specific signature subpage
    void setShortcutFlag(ShortcutId value) { m_shortcutFlag = value; }
    void addShortcutPath(QString path) { m_shortcutPaths.append(path.replace(QChar('\\'), QChar('/'))); };
    void setShortcutLocation(QString location) { m_shortcutLocation = location; }
    void setShortcutReason(QString reason) { m_shortcutReason = reason; }
    void setShortcutTsa(bool useTsa) { m_shortcutTsa = useTsa; }
    void setShortcutOutput(QString output) { m_shortcutOutput = output; }
    int getShortcutFlag() { return m_shortcutFlag; }
    QList<QString> getShortcutPaths() { return m_shortcutPaths; }
    QString getShortcutLocation() { return m_shortcutLocation; }
    QString getShortcutReason() { return m_shortcutReason; }
    QString getShortcutOutput() { return m_shortcutOutput; }
    QString getAbsolutePath(QString path);
    bool getShortcutTsa() { return m_shortcutTsa; }

    /* SCAP Methods  */
    void startGettingEntities();
    void startGettingCompanyAttributes(bool useOAuth);
    void startLoadingAttributesFromCache(int scapAttrType, bool isShortDescription);
    void startRemovingAttributesFromCache(int scapAttrType);
    void startGettingEntityAttributes(QList<int> entity_index, bool useOAuth);
    void startPingSCAP();

    void startSigningSCAP(QString inputPdf, QString outputPDF, int page, double location_x, double location_y,
                          QString location, QString reason, bool isTimestamp, bool isLtv, QList<int> attribute_index);

    void abortSCAPWithCMD(); // close the listing server

    //Returns page size in postscript points
    QSize getPageSize(int page) { return image_provider_pdf->getPageSize(page); };
    void verifyAuthPin(QString pin);
    void getTriesLeftAuthPin();
    void verifySignPin(QString pin);
    void getTriesLeftSignPin();
    void verifyAddressPin(QString pin);
    void getTriesLeftAddressPin();

    unsigned int doGetTriesLeftAuthPin();
    unsigned int doGetTriesLeftSignPin();
    unsigned int doGetTriesLeftAddressPin();
    unsigned int doVerifyAuthPin(QString pin);
    unsigned int doVerifySignPin(QString pin);
    unsigned int doVerifyAddressPin(QString pin);
    unsigned int doChangeAuthPin(QString currentPin, QString newPin);
    unsigned int doChangeSignPin(QString currentPin, QString newPin);
    unsigned int doChangeAddressPin(QString currentPin, QString newPin);

    void changeAuthPin(QString currentPin, QString newPin);
    void changeSignPin(QString currentPin, QString newPin);
    void changeAddressPin(QString currentPin, QString newPin);

    void changeAddress(QString process, QString secret_code);
    void doChangeAddress(const char *process, const char *secret_code);
    void cancelCMDSign();
    void cancelCMDRegisterCert();
    void signOpenCMD(QString mobileNumber, QString secret_code, QList<QString> loadedFilePath,
                  QString outputFile, int page, double coord_x, double coord_y, QString reason, QString location,
                 bool isTimestamp, bool isLTV, bool isSmall);
    void signCloseCMD(QString sms_token, QList<int> attribute_list);
    void doOpenSignCMD(CMDSignature *cmd_signature, CmdParams &cmdParams, SignParams &signParams);
    void doCloseSignCMD(CMDSignature *cmd_signature, QString sms_token);
    void doCloseSignCMDWithSCAP(CMDSignature *cmd_signature, QString sms_token, QList<int> attribute_list);
    void signOpenScapWithCMD(QString mobileNumber, QString secret_code, QList<QString> loadedFilePaths,
                       QString outputFile, int page, double coord_x, double coord_y,
                       QString reason, QString location, bool isTimestamp, bool isLtv);
    void sendSmsCmd(CmdDialogClass dialogType);
    void doSendSmsCmd(CmdDialogClass dialogType);

    static void addressChangeCallback(void *, int);
    void showChangeAddressDialog(long code);
    void showSignCMDDialog(long error_code);
    bool checkCMDSupport();

    QString getCardActivation();
    QString getDataCardIdentifyValue(GAPI::IDInfoKey key);
    QString getAddressField(GAPI::AddressInfoKey key);

    void setEventCallbacks( void );
    void startfillCertificateList ( void );
    void viewCardCertificate(QString issuedBy, QString issuedTo);
    void exportCardCertificate(QString issuedBy, QString issuedTo, QString outputPath);
    void startGetCardActivation ( void );

    void initTranslation();

    QString getCachePath(void);
    bool customSignImageExist(void);
    void customSignRemove(void);

    void updateReaderList( void );
    void setUseCustomSignature (bool UseCustomSignature);
    bool getUseCustomSignature(void);
    bool saveCustomImageToCache(QString url);
    void setRegCertValue(bool bRegCert);
    void setRemoveCertValue(bool bRemoveCert);
    bool getRegCertValue(void);
    bool getRemoveCertValue(void);
    void registerCMDCertOpen(QString mobileNumber, QString pin);
    void registerCMDCertClose(QString otp);
#ifdef WIN32
    QVariantList getRegisteredCmdPhoneNumbers();
#endif
    bool areRootCACertsInstalled();
    void installRootCACert();

    void cancelDownload();
    void httpFinished();

    void quitApplication(bool restart = false);
    void forgetAllCertificates( void );
    void forgetCertificates(QString const& reader);

    void setAppAsDlgParent();

    // used to check if a path is a directory or a file from QML
    bool isDirectory(QString path);
    bool isFile(QString path);
    bool fileExists(QString path);
    QList<QString> getFilesFromDirectory(QString path);

signals:
    // Signal from GAPI to Gui
    // Notify about Card Identify changed
    void signalReaderContext();
    void signalSetReaderComboIndex(long selected_reader);
    void signalCardDataChanged();
    void signalAddressLoaded(bool m_foreign);
    void signalCardAccessError(int error_code);
    void signalGenericError(const QString error_code);
    void signalSaveCardPhotoFinished(bool success);
    void signalPersoDataLoaded(const QString& persoNotes);
    void signalAddressLoadedChanged();
    void signalPdfSignSucess(int error_code);
    void signalPdfSignFail(int error_code);
    void signalUpdateProgressBar(int value);
    void signalUpdateProgressStatus(const QString statusMessage);
    void addressChangeFinished(long return_code);
    void signCMDFinished(long error_code);
    void signalValidateOtp();
    void signalShowLoadAttrButton();
    void signalShowMessage(QString msg);
    void signalOpenFile();
    void signalCardChanged(const int error_code);
    void signalSetPersoDataFile(const QString titleMessage, const QString statusMessage, bool success);
    void signalCertificatesChanged(const QVariantMap certificatesMap);
    void signalCertificatesFail();
    void signalInstalledRootCACert(bool successful);
    void signalShowCardActivation(QString statusMessage);
    void signalTestPinFinished(int triesLeft, int pin);
    void signalModifyPinFinished(int triesLeft, int pin);
    void signalTriesLeftPinFinished(int triesLeft, int pin);
    void signalBeginOAuth();
    void signalEndOAuth(int oauthResult);
        
    //SCAP signals
    void signalSCAPEntitiesLoaded(const QList<QString> entitiesList);
    void signalSCAPServiceFail(int pdfsignresult);
    void signalSCAPDefinitionsServiceFail(int pdfsignresult, bool isCompany);
    void signalSCAPServiceTimeout();
    void signalSCAPPingFail();
    void signalSCAPPingSuccess();
    void signalCompanyAttributesLoaded(const QList<QString> attribute_list);
    void signalEntityAttributesLoaded(const QList<QString> attribute_list);
    void signalAttributesLoaded(const QList<QString> attribute_list);
    void signalCompanyAttributesLoadedError();
    void signalEntityAttributesLoadedError();
    void signalPdfPrintSucess();
    void signalPrinterPrintSucess();
    void signalPdfPrintSignSucess();
    void signalPdfPrintFail();
    void signalPrinterPrintFail();
    void signalPrinterPrintFail(int error_code);
    void signalLanguageChangedError();
    void signalRemoveSCAPAttributesSucess(int scapAttrType);
    void signalRemoveSCAPAttributesFail(int scapAttrType);
    void signalCacheNotReadable(int scapAttrType);
	void signalCacheNotWritable();
	void signalCacheFolderNotCreated();

    // Import Certificates
    void signalImportCertificatesFail();
    void signalRemoveCertificatesFail();

    void signalExportCertificates(bool success);

private:
    bool LoadTranslationFile( QString NewLanguage );
    void setDataCardIdentify(QMap<GAPI::IDInfoKey, QString> m_data);
    void connectToCard();
    void getSCAPEntities();
    void getSCAPCompanyAttributes(bool OAuth);
    QString translateCMDErrorCode(int errorCode);

    //scapAttrType : 0 = Entities, 1 = Companies, 2 = All Attributes
    void getSCAPAttributesFromCache(int scapAttrType, bool isShortDescription);
    void removeSCAPAttributesFromCache(int scapAttrType);
    bool prepareSCAPCache(int scapAttrType);
    void getSCAPEntityAttributes(QList<int> entityIDs, bool useOAuth);
    void doCancelCMDSign();
    void doCancelCMDRegisterCert();
    void doSignSCAP(SCAPSignParams params);
    void getPersoDataFile();
    void setPersoDataFile(QString text);
    void doSaveCardPhoto(QString outputFile);
    void getAddressFile();
    void doSignPDF(SignParams &params);
    bool doSignPrintPDF(QString &file_to_sign, QString &outputsign);
    void doPrintPDF(PrintParams &params);
    void doPrint(PrintParams &params);
    bool drawpdf(QPrinter &printer, PrintParams params);
    void doSignBatchPDF(SignParams &params);
	void doSignXADES(QString loadedFilePath, QString outputFile, bool isTimestamp);
    void doSignBatchXADES(SignParams &params);
    void buildTree(eIDMW::PTEID_Certificate &cert, bool &bEx, QVariantMap &certificatesMap);
    void fillCertificateList (void );
    void getCertificateAuthStatus(void );
    int findCardCertificate(QString issuedBy, QString issuedTo);
    void doExportCardCertificate(QString issuedBy, QString issuedTo, QString outputPath);
    void getCardInstance(PTEID_EIDCard *&new_card);
    bool useCustomSignature(void);
    void stopAllEventCallbacks(void);
    void cleanupCallbackData(void);
    void initScapAppId(void);
    void doRegisterCMDCertOpen(QString mobileNumber, QString pin);
    void doRegisterCMDCertClose(QString otp);
    void doInstallRootCACert();
    void drawSectionHeader(QPainter &painter, double pos_x, double pos_y, QString section_name);
    void drawPrintingDate(QPainter &painter, QString printing_date);
    double checkNewPageAndPrint(QPrinter &printer, QPainter &painter, double current_y, double remaining_height, double max_height, bool print_date = false, QString date_label = "");
    double drawSingleField(QPainter &painter, double pos_x, double pos_y, QString name, QString value, double line_length, int field_margin = 15, bool is_bounded_rect = false, double bound_width = 360);

    // Data Card Identify map
    QMap<GAPI::IDInfoKey, QString> m_data;
    QMap<GAPI::AddressInfoKey, QString> m_addressData;
    //Don't free this!, we release ownership to the QMLEngine in buildImageProvider()
    PhotoImageProvider *image_provider;

    CMDSignature *cmd_signature;
    std::vector<PTEID_PDFSignature *> cmd_pdfSignatures;
    ScapServices scapServices;
    SCAPSignParams m_scap_params;

    QString m_persoData;
    bool m_addressLoaded;
    ShortcutId m_shortcutFlag;
    QList<QString> m_shortcutPaths;
    QString m_shortcutLocation;
    QString m_shortcutReason;
    QString m_shortcutOutput;
    bool m_shortcutTsa = false;
    signed int selectedReaderIndex = -1;
    double print_scale_factor = 1;

    tCallBackHandles		m_callBackHandles;
    tCallBackData			m_callBackData;

    QImage m_custom_image;
    QByteArray m_jpeg_scaled_data;

    QTimer* m_timerReaderList;

    QUrl url;
    QNetworkProxy proxy;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QString m_pac_url;
    bool httpRequestAborted;
    bool httpRequestSuccess;

protected:
    QTranslator m_translator;
};
#endif // GAPI_H
