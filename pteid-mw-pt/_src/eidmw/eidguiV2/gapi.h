/*-****************************************************************************

 * Copyright (C) 2017-2022 Adriano Campos - <adrianoribeirocampos@gmail.com>
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
#include "scapclient.h"
#include "cmdErrors.h"

#include "../dialogs/dialogs.h"

#define SUCCESS_EXIT_CODE 0
#define RESTART_EXIT_CODE  1

#define TEL_APP_USER_AGENT  "AutenticacaoGov/"

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
    bool isAdditionalInfo;
    bool isAddress;
    bool isNotes;
    bool isPrintDate;
    bool isSign;
    
};

struct PrintParamsWithSignature {
public:
    PrintParamsWithSignature(PrintParams &params, bool is_timestamp, bool is_ltv) {
        base_params = params;
        isTimestamp = is_timestamp;
        isLtv = is_ltv;
    }
    PrintParams base_params;
    bool isTimestamp;
    bool isLtv;
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
    bool isLastPage;
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
    QList<QString> inputPDFs;
    QString outputPDF;
    int page; 
    double location_x;
    double location_y;
    QString reason;
    QString location;
    bool isTimestamp;
    bool isLtv;
    bool isLastPage;
    QList<QString> attribute_ids;
};

struct WindowGeometry {
    int x;
    int y;
    int width;
    int height;
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
    Q_SIGNAL void signalPdfSourceChanged(double original_width);
private:
    void doCloseDoc(QString filePath);
    void doCloseAllDocs();
    QPixmap renderPDFPage(unsigned int page);
    std::unordered_map<std::string, Poppler::Document *> m_docs; // all loaded pdfs (remain open)
    std::string m_filePath; // file in preview
    QString m_file_id; // file and page in preview
    QMutex renderMutex;
    QPixmap m_cache;
};

const QString MAIN_QML_PATH("qrc:/main.qml");

void saveCAN(const char * serial, const char *can);
bool deleteCAN();
std::string getCANFromCache(const char * serial);

class GAPI : public QObject
{
    #define TIMERREADERLIST 5000
    Q_OBJECT
    Q_PROPERTY(QMap<IDInfoKey, QString> m_data
               NOTIFY signalCardDataChanged)
    Q_PROPERTY(QMap<AddressInfoKey, QString> m_addressData NOTIFY signalAddressLoaded)
    Q_PROPERTY(QString persoData MEMBER m_persoData NOTIFY signalPersoDataLoaded)

public:
    explicit GAPI(QObject *parent = 0);

    enum IDInfoKey { Documenttype, Documentversion, Surname, Givenname, Sex, Height, Nationality, Birthdate, Documentnum, Validitybegindate, Validityenddate,
                     NIC, NIF, NISS, NSNS, IssuingEntity, PlaceOfRequest, Country, Father, Mother, AccidentalIndications };

    enum AddressInfoKey { District, Municipality, Parish, Streettype, Streetname, Buildingtype, Doorno, Floor, Side, Locality, Place, Zip4, Zip3, PostalLocality,
                          Foreigncountry, Foreignaddress, Foreigncity, Foreignregion, Foreignlocality, Foreignpostalcode};

    enum CardAccessError { NoReaderFound, NoCardFound, CardUnknownCard, PinBlocked, SodCardReadError, CardUserPinCancel, CardUnknownError, CardPinTimeout, IncompatibleReader };

    enum RemoteAddressError { AddressConnectionError, AddressServerError, AddressConnectionTimeout, AddressSmartcardError, AddressServerCertificateError, CardCertificateError, AddressInvalidStateError, AddressUnknownError };

    enum SignMessage { SignMessageOK, SignMessageTimestampFailed, SignMessageLtvFailed, SignFilePermissionFailed, PDFFileUnsupported};

    enum PrintMessage {NoPrinterAvailable};

    enum eCustomEventType { ET_UNKNOWN, ET_CARD_CHANGED, ET_CARD_REMOVED };

    enum AutoUpdateMessage {GenericError, NoUpdatesAvailable, DownloadFailed, DownloadCancelled, LinuxNotSupported, UpdatesAvailable,
                           UnableSaveFile, InstallFailed, NetworkError};

    enum AutoUpdateType {AutoUpdateNoExist, AutoUpdateApp, AutoUpdateCerts, AutoUpdateNews};

    enum ScapPdfSignResult { ScapTimeOutError, ScapGenericError, ScapAttributesExpiredError, ScapZeroAttributesError,
                             ScapNotValidAttributesError, ScapClockError, ScapSecretKeyError, ScapMultiEntityError,
                             ScapSucess, ScapAttrPossiblyExpiredWarning, ScapIncompleteResponse };

    enum PinUsage { AuthPin, SignPin, AddressPin };

    enum CmdDialogClass { Sign, RegisterCert, ShowMessage, Progress};

    enum ShortcutId { ShortcutIdNone, ShortcutIdSign};

    enum SignLevel { LevelBasic, LevelTimestamp, LevelLTV };

    enum TelemetryAction { Startup, SignCC, SignCMD, SignCMDScap, SignCCScap, PrintPDF, Accepted, Denied };

    enum TelemetryStatus { RetryEnable = 1, Enabled = 2, RetryDisable = 4, Disabled = 8 };

    enum OAuthErrors {OAuthTimeoutError, OAuthGenericError, OAuthConnectionError, OAuthCancelled};

    enum PaceAuthState {PaceDefault, PaceNeeded, PaceAuthenticated};

    enum PaceError {PaceUnknown, PaceBadToken, PaceUnutilized};

    enum CardOperation {IdentityData, SignCertificateData, ValidateCertificate, ReadCertDetails, DoAddress, GetAuthPin, GetSignPin, GetAddressPin};

    Q_ENUMS(ScapPdfSignResult)
    Q_ENUMS(CardAccessError)
    Q_ENUMS(RemoteAddressError)
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
    Q_ENUMS(TelemetryAction)
    Q_ENUMS(TelemetryStatus)
    Q_ENUMS(OAuthErrors)
    Q_ENUMS(CardOperation);
    Q_ENUMS(PaceError);

    QQuickImageProvider * buildImageProvider() { return image_provider; }
    QQuickImageProvider * buildPdfImageProvider() { return image_provider_pdf; }

	void storeQmlEngine(QQmlApplicationEngine *engine);
    PDFPreviewImageProvider * image_provider_pdf;

    // Do not forget to declare your class to the QML system.
    static void declareQMLTypes() {
        qmlRegisterType<GAPI>("eidguiV2", 1, 0, "GAPI");
    }
    GUISettings    m_Settings;
    CERTIFICATES    m_Certificates;
    CMDCertificates *m_cmdCertificates;

public slots:
    // Telemetry
    void updateTelemetry(TelemetryAction action);
    void doUpdateTelemetry(TelemetryAction action);
    TelemetryStatus getTelemetryStatus();
    void setTelemetryStatus(TelemetryStatus status);
    void enableTelemetry();
    void disableTelemetry();
    const char *telemetryActionToString(TelemetryAction action);
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

    // get notes functionality support
    bool isNotesSupported();

    // Slots to Gui request values
    QVariantList getRetReaderList(void);
    int getReaderIndex(void);
    void setReaderByUser(unsigned long setReaderIndex);
    void resetReaderSelected(void) {selectedReaderIndex =  -1; }
    signed int returnReaderSelected(void) {return selectedReaderIndex; }
    void setAddressLoaded(bool addressLoaded) {m_addressLoaded = addressLoaded; }
    bool isAddressLoaded() { return m_addressLoaded; }
    void startCardReading();
    void startGettingInfoFromSignCert();
    void startCCSignatureCertCheck();
    void startSavingCardPhoto(QString outputFile);
    int getStringByteLength(QString text);
    void finishLoadingCardData(PTEID_EIDCard * card);
	void finishLoadingSignCertData(PTEID_EIDCard * card);
    void startReadingPersoNotes();
    void startWritingPersoNotes(const QString &text);
    void startReadingAddress();
    void startPrintPDF(QString outputFile, bool isBasicInfo,bool isAdditionalInfo,
                       bool isAddress,bool isNotes,bool isPrintDate,bool isSign, bool isTimestamp, bool isLtv);
    void startPrint(QString outputFile, bool isBasicInfo,bool isAdditionalInfo,
                       bool isAddress, bool isNotes, bool isPrintDate, bool isSign);
    //This method should be used by basic and advanced signature modes
    void startSigningPDF(QString loadedFilePath, QString outputFile, int page, double coord_x, double coord_y,
                         QString reason, QString location, bool isTimestamp, bool isLtv, bool isSmall);
    void startSigningBatchPDF(QList<QString> loadedFileBatchPath, QString outputFile, int page, double coord_x, double coord_y,
                         QString reason, QString location, bool isTimestamp, bool isLtv, bool isSmall, bool isLastPage);
    int getPDFpageCount(QString loadedFilePath);
    void closePdfPreview(QString filePath);
    void closeAllPdfPreviews();

    void startSigningXADES(QString loadedFilePath, QString outputFile, bool isTimestamp, 
                        bool isLTV, bool isASIC);
    void startSigningBatchXADES(QList<QString> loadedFileBatchPath, QString outputFile, 
                        bool isTimestamp, bool isLTV);
    void startSigningXADESWithCMD(QList<QString> loadedFileBatchPath, QString outputFile, 
                        bool isTimestamp, bool isLTV, bool isASIC);

    bool isASiC(const QString& filename);
    QStringList listFilesInASiC(const QString& filename);
    void extractFileFromASiC(const QString& container_path, const QString& filename);

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
    void startLoadingAttributesFromCache(bool showSubAttributes);
    void startRemovingAttributesFromCache();
	void startRemoveCANCache();
    void startGettingEntityAttributes(QList<QString> entities_ids, bool useOAuth);
    static bool isAttributeExpired(std::string& date, std::string& supplier);

    void startSigningSCAP(QList<QString> inputPdfs, QString outputPDF, int page, double location_x, double location_y,
                          QString location, QString reason, bool isTimestamp, bool isLtv, bool isLastPage, QList<QString> attribute_ids);

    void abortSCAPWithCMD(); // close the listing server
    QString getSCAPProviderLogo(QList<QString> qstring_ids);

    //Returns page size in postscript points
    QSize getPageSize(int page) { return image_provider_pdf->getPageSize(page); };
    void verifyAuthPin(QString pin);
    void getTriesLeftAuthPin();
    void verifySignPin(QString pin);
    void getTriesLeftSignPin();
    void verifyAddressPin(QString pin, bool forceVerify);
    void getTriesLeftAddressPin();

    unsigned int doGetTriesLeftAuthPin();
    unsigned int doGetTriesLeftSignPin();
    unsigned int doGetTriesLeftAddressPin();
    unsigned int doVerifyAuthPin(QString pin);
    unsigned int doVerifySignPin(QString pin);
    unsigned int doVerifyAddressPin(QString pin, bool forceVerify);
    unsigned int doChangeAuthPin(QString currentPin, QString newPin);
    unsigned int doChangeSignPin(QString currentPin, QString newPin);
    unsigned int doChangeAddressPin(QString currentPin, QString newPin);

    void changeAuthPin(QString currentPin, QString newPin);
    void changeSignPin(QString currentPin, QString newPin);
    void changeAddressPin(QString currentPin, QString newPin);

	void startPACEAuthentication(QString pace_can, CardOperation op);

    void performPACEWithCache(PTEID_EIDCard * card, CardOperation op);
    void resetContactlessState() { m_pace_auth_state = PaceDefault; m_is_contactless = false; }

    void cancelCMDRegisterCert();
    void signCMD(QList<QString> loadedFilePath, QString outputFile, int page, double coord_x,
                     double coord_y, QString reason, QString location, bool isTimestamp, bool isLTV,
                     bool isSmall, bool isLastPage);
    void doSignCMD(PTEID_PDFSignature &pdf_signature, SignParams &signParams);

    //SCAP with CMD
    void signScapWithCMD(QList<QString> loadedFilePaths, QString outputFile, QList<QString> attribute_list,
                         int page, double coord_x, double coord_y, QString reason, QString location,
                         bool isTimestamp, bool isLtv, bool isLastPage);


    static void addressChangeCallback(void *, int);
    void showSignCMDDialog(long error_code);
    bool checkCMDSupport();

    QString getCardActivation();
    QString getDataCardIdentifyValue(GAPI::IDInfoKey key);
    QString getAddressField(GAPI::AddressInfoKey key);

    void setEventCallbacks( void );
    void startfillCertificateList ( void );
    void validateCertificates();
    void doValidateCertificates();
    void viewCardCertificate(QString issuedBy, QString issuedTo);
    void exportCardCertificate(QString issuedBy, QString issuedTo, QString outputPath);
    void startGetCardActivation ( void );
	void setAppAsDlgParent(QObject *object, const QUrl &url);

    void initTranslation();

    QString getCachePath(void);
    bool customSignImageExist(void);
    void customSignImageRemove(void);

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
    void setUseNumId(bool UseNumId);
    bool getUseNumId(void);
    void setUseDate (bool UseDate);
    bool getUseDate(void);
    void resizePDFSignSeal(unsigned int height, unsigned int width);

#ifdef WIN32
    QVariantList getRegisteredCmdPhoneNumbers();
#endif
    bool areRootCACertsInstalled();
    void installRootCACert();

    void quitApplication(bool restart = false);
    void forgetAllCertificates( void );
    void forgetCertificates(QString const& reader);

    // used to check if a path is a directory or a file from QML
    bool isDirectory(QString path);
    bool isFile(QString path);
    bool fileExists(QString path);
    QList<QString> getFilesFromDirectory(QString path);

    QStringList getWrappedText(QString name, int maxlines, int offset);
    QVariantList getSCAPAttributesText(QList<QString> qstring_ids);
    int getSealFontSize(bool isReduced, QString reason, QString name, 
        bool nic, bool date, QString location, QString entities, QString attributes,
        unsigned int width, unsigned int height);

    void getCardInstance(PTEID_EIDCard *&new_card);

signals:
    // Signal from GAPI to Gui
    // Notify about Card Identify changed
    void signalReaderContext();
    void signalSetReaderComboIndex(long selected_reader);
    void signalCardDataChanged();
    void signalSignCertDataChanged(QString ownerName, QString NIC);
    void signalSignCertExpired();
    void signalSignCertSuspended();
    void signalSignCertRevoked();
    void signalAddressLoaded(bool m_foreign);
    void signalCardAccessError(int error_code);
    void signalRemoteAddressError(int error_code);
    void signalGenericError(const QString error_code);
    void signalSaveCardPhotoFinished(bool success);
    void signalPersoDataLoaded(const QString& persoNotes);
    void signalPersonalDataNotSupported();
    void signalPdfSignSuccess(int error_code);
    void signalPdfSignFail(int error_code, int index);
    void signalPdfBatchSignFail(int error_code, const QString &filename);
    void signalUpdateProgressBar(int value);
    void signalUpdateProgressStatus(const QString statusMessage);
    void signCMDFinished(long error_code);
    void signalCanceledSignature();
    void signalValidateOtp();
    void signalShowLoadAttrButton();
    void signalShowMessage(QString msg, QString urlLink);
    void signalOpenFile();
    void signalSignatureFinished();
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
    void signalCustomSignImageRemoved();
    void signalOperationCanceledByUser();
	void signalContactlessCANNeeded();

    void signalStartCheckCCSignatureCert(); // Start CC signature check with OCSP
    void signalOkSignCertificate();         // CC signature certificate OK or OCSP no response
        
    //SCAP signals
    void signalSCAPEntitiesLoaded(const QList<QString> entitiesList);
    void signalSCAPServiceFail(int pdfsignresult, bool isCompany);
    void signalSCAPIncompleteResponse(const QList<QString> failed_providers);
    void signalSCAPNoAttributesResponse(const QList<QString> failed_providers, bool enterprise);
    void signalSCAPServiceTimeout();
    void signalSCAPConnectionFailed();
    void signalSCAPProxyAuthRequired();
    void signalSCAPPossibleProxyMisconfigured();
    void signalSCAPBadCredentials();
    void signalAttributesLoaded(const QVariantMap institution_attributes, const QVariantMap enterprise_attributes);
    void signalCompanyAttributesLoadedError();
    void signalEntityAttributesLoadedError();
    void signalAttributesPossiblyExpired(const QStringList expiredSuppliers);
    void signalRemoveSCAPAttributesSucess();
    void signalRemoveSCAPAttributesFail();
    void signalCacheNotReadable();
    void signalCacheNotWritable();
    void signalCacheRemovedLegacy();
    void signalCacheFolderNotCreated();
	void signalRemoveCANCacheSucess();

    // Import Certificates
    void signalImportCertificatesFail();
    void signalRemoveCertificatesFail();

    void signalExportCertificates(bool success);

    void signalPdfPrintSucess();
    void signalPrinterPrintSucess();
    void signalPdfPrintSignSucess();
    void signalPdfPrintFail();
    void signalPrinterPrintFail();
    void signalPrinterPrintFail(int error_code);
    void signalLanguageChangedError();

    void errorPace(int error_code);
    void paceSuccess();

private:
    bool LoadTranslationFile( QString NewLanguage );
    void emitErrorSignal(const char * callerfunction, long errorCode, int index = -1);
    void setDataCardIdentify(QMap<GAPI::IDInfoKey, QString> m_data);
    void connectToCard();
    void getSCAPEntities();
    void getSCAPCompanyAttributes(bool OAuth);

    void getSCAPAttributesFromCache(bool isShortDescription);
    void removeSCAPAttributesFromCache();
	void removeCANCache();
    void getSCAPEntityAttributes(QList<QString> ids, bool useOAuth);
    void doCancelCMDRegisterCert();
    void doSignSCAP(const SCAPSignParams &params, bool isCMD);
    template<typename T>
    bool handleScapError(const ScapResult<T> &result, bool isCompany = false, bool isEmployee = false);
    void getPersoDataFile();
    void setPersoDataFile(const QString &text);
    void doSaveCardPhoto(QString outputFile);
    void getAddressFile();
    void doSignPDF(SignParams &params);
    bool doSignPrintPDF(QString &file_to_sign, QString &outputsign, bool isTimestamp, bool isLtv);
    void doPrintPDF(PrintParamsWithSignature &params);
    void doPrint(PrintParams &params);
    bool drawpdf(QPrinter &printer, PrintParams params, long &addressError);
    void doSignBatchPDF(SignParams &params);
    void doSignXADES(QString loadedFilePath, QString outputFile, bool isTimestamp, bool isLTV, bool isASIC);
    void doSignBatchXADES(SignParams &params);
    void doSignXADESWithCMD(SignParams &params, bool isASIC);
    void doExtractFileFromASiC(const QString& container_path, const QString& filename);
    void buildTree(eIDMW::PTEID_Certificate &cert, bool &bEx, QVariantMap &certificatesMap);
    void fillCertificateList (void );
    void getCertificateAuthStatus(void );
    void checkCCSignatureCert(void);
    //void checkSignatureCertValidity(void);
    void getInfoFromSignCert(void);
    int findCardCertificate(QString issuedBy, QString issuedTo);
    void doExportCardCertificate(QString issuedBy, QString issuedTo, QString outputPath);
    bool useCustomSignature(void);
    void stopAllEventCallbacks(void);
    void cleanupCallbackData(void);
    void doRegisterCMDCertOpen(QString mobileNumber, QString pin);
    void doRegisterCMDCertClose(QString otp);
    void doInstallRootCACert();
    void drawSectionHeader(QPainter &painter, double pos_x, double pos_y, QString section_name);
    void drawPrintingDate(QPainter &painter, QString printing_date);
    double checkNewPageAndPrint(QPrinter &printer, QPainter &painter, double current_y, double remaining_height, double max_height, bool print_date = false, QString date_label = "");
    double drawSingleField(QPainter &painter, double pos_x, double pos_y, QString name, QString value, double line_length, int field_margin = 15, bool is_bounded_rect = false, double bound_width = 360);
    WindowGeometry *getWndGeometry();
    void handleRemoteAddressErrors(long errorCode);
    //The 2nd function pointer param points to the function to be called after PACE auth is finished
    void doStartPACEAuthentication(QString pace_can, CardOperation op);

    // Data Card Identify map
    QMap<GAPI::IDInfoKey, QString> m_data;
    QMap<GAPI::AddressInfoKey, QString> m_addressData;
    //Don't free this!, we release ownership to the QMLEngine in buildImageProvider()
    PhotoImageProvider *image_provider;

	//Don't free this, created in main() and should live as long as the app is running
	QQmlApplicationEngine *m_qml_engine;

	PTEID_CMDSignatureClient * m_cmd_client;
    std::vector<PDFSignature *> cmd_pdfSignatures;
    ScapClient *m_scap_client;

    QString m_persoData;
    bool m_addressLoaded;
    ShortcutId m_shortcutFlag;
    QList<QString> m_shortcutPaths;
    QString m_shortcutLocation;
    QString m_shortcutReason;
    QString m_shortcutOutput;
    bool m_shortcutTsa = false;
	bool m_is_contactless = false;
    PaceAuthState m_pace_auth_state = PaceDefault;
    signed int selectedReaderIndex = -1;
    double print_scale_factor = 1;

    tCallBackHandles		m_callBackHandles;
    tCallBackData			m_callBackData;

    QByteArray m_jpeg_scaled_data;

    QTimer* m_timerReaderList;

    QUrl url;
    QString m_pac_url;

    int m_seal_width = 178;
    int m_seal_height = 90;
    int m_font_size = 8;

    WindowGeometry m_wndGeometry;
    QWindow *m_mainWnd;

protected:
    QTranslator m_translator;
};
#endif // GAPI_H
