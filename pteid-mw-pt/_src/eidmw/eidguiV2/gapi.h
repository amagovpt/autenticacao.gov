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

//MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"

#include "CMDSignature.h"
#include "cmdErrors.h"

#include "scapsignature.h"

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
    QString loadedFilePath; QString outputFile;
    int page;
    double coord_x;
    double coord_y;
    QString reason;
    QString location;
    bool isTimestamp;
    bool isSmallSignature;
};

struct CmdSignParams {
public:
    QString mobileNumber;
    QString secret_code;
    QString loadedFilePath;
    QString outputFile;
    int page;
    double coord_x;
    double coord_y;
    QString reason;
    QString location;
    double isTimestamp;
    double isSmallSignature;
};

struct CmdSignedFileDetails {
public:
    QString signedCMDFile;
    QString citizenName;
    QString citizenId;
};

struct SignBatchParams {
public:
    QList<QString> loadedFileBatchPath; QString outputFile;
    int page;
    double coord_x;
    double coord_y;
    QString reason;
    QString location;
    bool isTimestamp;
    bool isSmallSignature;
};

struct SCAPSignParams {
public:
    QString inputPDF;
    QString outputPDF;
    int page; double location_x; double location_y;
    QString location; QString reason;
    int ltv;
    QList<int> attribute_index;
};

class PDFPreviewImageProvider: public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    PDFPreviewImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap), m_doc(NULL) { }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    //Returns page size in postscript points
    QSize getPageSize(int page);

signals:
    Q_SIGNAL void signalPdfSourceChanged(int pdfWidth, int pdfHeight);
private:
    QPixmap renderPdf(int page,const QSize &requestedSize);
    QPixmap renderPDFPage(unsigned int page);
    Poppler::Document *m_doc;
    QString m_filePath;
};


class GAPI : public QObject
{
    #define SCAP_SERVICE_ERROR_CODE -10
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

    enum CardAccessError { NoReaderFound, NoCardFound, PinBlocked, CardReadError, SodCardReadError, CardUserPinCancel, CardUnknownError, CardPinTimeout };

    enum SignMessage { SignMessageOK, SignMessageTimestampFailed, SignFilePermissionFailed, PDFFileUnsupported};

    enum PrintMessage {NoPrinterAvailable};

    enum eCustomEventType { ET_UNKNOWN, ET_CARD_CHANGED, ET_CARD_REMOVED };

    enum AutoUpdateMessage {GenericError, NoUpdatesAvailable, DownloadFailed, DownloadCancelled, LinuxNotSupported, UpdatesAvailable,
                           UnableSaveFile, InstallFailed, NetworkError};

    enum ScapPdfSignResult { ScapTimeOutError, ScapGenericError, ScapAttributesExpiredError, ScapZeroAttributesError,
                             ScapNotValidAttributesError, ScapSucess };
    Q_ENUMS(ScapPdfSignResult)
    Q_ENUMS(CardAccessError)
    Q_ENUMS(eCustomEventType)
    Q_ENUMS(IDInfoKey)
    Q_ENUMS(AddressInfoKey)
    Q_ENUMS(UI_LANGUAGE)
    Q_ENUMS(SignMessage)
    Q_ENUMS(PrintMessage)
    Q_ENUMS(AutoUpdateMessage)


    bool isAddressLoaded() {return m_addressLoaded; }

    QQuickImageProvider * buildImageProvider() { return image_provider; }
    QQuickImageProvider * buildPdfImageProvider() { return image_provider_pdf; }

    PDFPreviewImageProvider * image_provider_pdf;

    //This flag is used to start the application in specific subpage
    void setShortcutFlag(int value) { m_shortcutFlag = value; }
    void setShortcutPDFPath(QString &inputPdf) { m_shortcutInputPDF = inputPdf ;}

    // Do not forget to declare your class to the QML system.
    static void declareQMLTypes() {
        qmlRegisterType<GAPI>("eidguiV2", 1, 0, "GAPI");
    }
    GUISettings    m_Settings;
    CERTIFICATES    m_Certificates;

public slots:
    // Slots to Gui request values
    QVariantList getRetReaderList(void);
    int getReaderIndex(void);
    void setReaderByUser(unsigned long setReaderIndex);
    void resetReaderSelected(void) {selectedReaderIndex =  -1; }
    signed int returnReaderSelected(void) {return selectedReaderIndex; }
    void setAddressLoaded(bool addressLoaded) {m_addressLoaded = addressLoaded; }
    void startCardReading();
    int getStringByteLength(QString text);
    long getFileSize(QString filePath);
    void startReadingPersoNotes();
    void startWritingPersoNotes(QString text);
    void startReadingAddress();
    int getShortcutFlag() {return m_shortcutFlag; }
	QString getShortcutInputPDF() {	return m_shortcutInputPDF.replace(QChar('\\'), QChar('/')); }
    void startPrintPDF(QString outputFile, bool isBasicInfo,bool isAdditionalInfo,
                       bool isAddress,bool isNotes,bool isPrintDate,bool isSign);
    void startPrint(QString outputFile, bool isBasicInfo,bool isAdditionalInfo,
                       bool isAddress, bool isNotes, bool isPrintDate, bool isSign);
    //This method should be used by basic and advanced signature modes
    void startSigningPDF(QString loadedFilePath, QString outputFile, int page, double coord_x, double coord_y,
                         QString reason, QString location, bool isTimestamp, bool isSmall);
    void startSigningBatchPDF(QList<QString> loadedFileBatchPath, QString outputFile, int page, double coord_x, double coord_y,
                         QString reason, QString location, bool isTimestamp, bool isSmall);
    int getPDFpageCount(QString loadedFilePath);

	void startSigningXADES(QString loadedFilePath, QString outputFile, bool isTimestamp);
	void startSigningBatchXADES(QList<QString> loadedFileBatchPath, QString outputFile, bool isTimestamp);

    /* SCAP Methods  */
    void startGettingEntities();
    void startGettingCompanyAttributes();
    void startLoadingAttributesFromCache(int isCompanies, bool isShortDescription);
    void startRemovingAttributesFromCache(int isCompanies);
    void startGettingEntityAttributes(QList<int> entity_index);
    void startPingSCAP();

    void startSigningSCAP(QString inputPdf, QString outputPDF, int page, double location_x, double location_y,
                          QString location, QString reason, int ltv, QList<int> attribute_index);

    //Returns page size in postscript points
    QSize getPageSize(int page) { return image_provider_pdf->getPageSize(page); };
    unsigned int verifyAuthPin(QString pin);
    unsigned int getTriesLeftAuthPin();
    unsigned int verifySignPin(QString pin);
    unsigned int getTriesLeftSignPin();
    unsigned int verifyAddressPin(QString pin);
    unsigned int getTriesLeftAddressPin();

    unsigned int changeAuthPin(QString currentPin, QString newPin);
    unsigned int changeSignPin(QString currentPin, QString newPin);
    unsigned int changeAddressPin(QString currentPin, QString newPin);

    void changeAddress(QString process, QString secret_code);
    void doChangeAddress(const char *process, const char *secret_code);
    void signOpenCMD(QString mobileNumber, QString secret_code, QString loadedFilePath,
                  QString outputFile, int page, double coord_x, double coord_y, QString reason, QString location,
                 double isTimestamp, double isSmall);
    void signCloseCMD(QString sms_token, QList<int> attribute_list);
    void doOpenSignCMD(CMDSignature *cmd_signature, CmdSignParams &params);
    void doCloseSignCMD(CMDSignature *cmd_signature, QString sms_token);
    void doCloseSignCMDWithSCAP(CMDSignature *cmd_signature, QString sms_token, QList<int> attribute_list);
    void signOpenScapWithCMD(QString mobileNumber, QString secret_code, QString loadedFilePath,
                       QString outputFile, int page, double coord_x, double coord_y,
                       QString reason, QString location);

    static void addressChangeCallback(void *, int);
    void showChangeAddressDialog(long code);
    void showSignCMDDialog(long code);

    QString getCardActivation();
    QString getDataCardIdentifyValue(GAPI::IDInfoKey key);
    QString getAddressField(GAPI::AddressInfoKey key);

    void setEventCallbacks( void );
    void startfillCertificateList ( void );
    void startGetCardActivation ( void );

    void initTranslation();

    QString getCachePath(void);
    bool customSignImageExist(void);
    void customSignRemove(void);

    void updateReaderList( void );
    void setUseCustomSignature (bool UseCustomSignature);
    bool getUseCustomSignature(void);
    void setRegCertValue(bool bRegCert);
    void setRemoveCertValue(bool bRemoveCert);
    bool getRegCertValue(void);
    bool getRemoveCertValue(void);

    void cancelDownload();
    void httpFinished();

    void quitApplication();
    void forgetAllCertificates( void );
    void forgetCertificates(QString const& reader);

signals:
    // Signal from GAPI to Gui
    // Notify about Card Identify changed
    void signalReaderContext();
    void signalSetReaderComboIndex(long selected_reader);
    void signalCardDataChanged();
    void signalAddressLoaded(bool m_foreign);
    void signalCardAccessError(int error_code);
    void signalGenericError(const QString error_code);
    void signalPersoDataLoaded(const QString& persoNotes);
    void signalAddressLoadedChanged();
    void signalPdfSignSucess(int error_code);
    void signalPdfSignFail(int error_code);
    void signalUpdateProgressBar(int value);
    void signalUpdateProgressStatus(const QString statusMessage);
    void addressChangeFinished(long return_code);
    void signCMDFinished(long return_code);
    void signalOpenCMDSucess();
    void signalCloseCMDSucess();
    void signalCardChanged(const int error_code);
    void signalSetPersoDataFile(const QString titleMessage, const QString statusMessage);
    void signalCertificatesChanged(const QVariantMap certificatesMap);
    void signalShowCardActivation(QString statusMessage);
        
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
    void signalRemoveSCAPAttributesSucess(int isCompanies);
    void signalRemoveSCAPAttributesFail(int isCompanies);
    void signalCacheNotReadable(int isCompanies);
	void signalCacheNotWritable();
	void signalCacheFolderNotCreated();

    // Import Certificates
    void signalImportCertificatesFail();
    void signalRemoveCertificatesFail();

private:
    bool LoadTranslationFile( QString NewLanguage );
    void setDataCardIdentify(QMap<GAPI::IDInfoKey, QString> m_data);
    void connectToCard();
    void getSCAPEntities();
    void getSCAPCompanyAttributes();
    QString translateCMDErrorCode(int errorCode);

    //querytype - 0 = Entities, 1 = Companies, 2 = All Attributes 
    void getSCAPAttributesFromCache(int queryType, bool isShortDescription);
    //querytype - 0 = Entities, 1 = Companies
    void removeSCAPAttributesFromCache(int queryType);
	bool prepareSCAPCache();
    void getSCAPEntityAttributes(QList<int> entityIDs);
    void doSignSCAP(SCAPSignParams params);
    void getPersoDataFile();
    void setPersoDataFile(QString text);
    void getAddressFile();
    void doSignPDF(SignParams &params);
    bool doSignPrintPDF(QString &file_to_sign, QString &outputsign);
    void doPrintPDF(PrintParams &params);
    void doPrint(PrintParams &params);
    bool drawpdf(QPrinter &printer, PrintParams params);
    void doSignBatchPDF(SignBatchParams &params);
	void doSignXADES(QString loadedFilePath, QString outputFile, bool isTimestamp);
    void doSignBatchXADES(SignBatchParams &params);
    void buildTree(eIDMW::PTEID_Certificate &cert, bool &bEx, QVariantMap &certificatesMap);
    void fillCertificateList (void );
    void getCertificateAuthStatus(void );
    void getCardInstance(PTEID_EIDCard *&new_card);
    bool useCustomSignature(void);
    void stopAllEventCallbacks(void);
    void cleanupCallbackData(void);
    void initScapAppId(void);
    CMDProxyInfo buildProxyInfo();

    // Data Card Identify map
    QMap<GAPI::IDInfoKey, QString> m_data;
    QMap<GAPI::AddressInfoKey, QString> m_addressData;
    //Don't free this!, we release ownership to the QMLEngine in buildImageProvider()
    PhotoImageProvider *image_provider;

    CMDSignature *cmd_signature;
    PTEID_PDFSignature *cmd_pdfSignature;
    ScapServices scapServices;
    SCAPSignParams m_scap_params;

    QString m_persoData;
    bool m_addressLoaded;
    int m_shortcutFlag;
    QString m_shortcutInputPDF;
    signed int selectedReaderIndex = -1;

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
