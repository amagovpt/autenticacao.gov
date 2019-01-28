#include "gapi.h"
#include <QString>
#include <QVector>
#include <QDate>
#include <QtConcurrent>
#include <cstdio>
#include <QQuickImageProvider>
#include <QPrinter>
#include <QPrinterInfo>
#include "qpainter.h"

#include "CMDSignature.h"
#include "cmdErrors.h"

//SCAP
#include "scapsignature.h"
#include "SCAP-services-v3/SCAPH.h"
//#include "ASService/ASServiceH.h"   
//#include "PDFSignature/envStub.h"

#include "ScapSettings.h"

using namespace eIDMW;

#define TRIES_LEFT_ERROR    1000

static  bool g_cleaningCallback=false;
static  int g_runningCallback=0;

/*
    GAPI - Graphic Application Programming Interface
*/

GAPI::GAPI(QObject *parent) :
    QObject(parent) {
    image_provider = new PhotoImageProvider();
    image_provider_pdf = new PDFPreviewImageProvider();


    cmd_signature = new eIDMW::CMDSignature();
    cmd_pdfSignature = new eIDMW::PTEID_PDFSignature();
    m_addressLoaded = false;
    m_shortcutFlag = 0;

    // Create callbacks for all readers at the startup
    setEventCallbacks();

    //----------------------------------
    // set a timer to check if the number of card readers is changed
    //----------------------------------
    m_timerReaderList = new QTimer(this);
    connect(m_timerReaderList, SIGNAL(timeout()), this, SLOT(updateReaderList()));
    m_timerReaderList->start(TIMERREADERLIST);
}


CMDProxyInfo GAPI::buildProxyInfo() {
    ProxyInfo proxyinfo;

    CMDProxyInfo cmd_proxyinfo;

    if (proxyinfo.getProxyHost().size() > 0) {
        cmd_proxyinfo.host = proxyinfo.getProxyHost().toStdString();
        cmd_proxyinfo.port = proxyinfo.getProxyPort().toLong();

        if (proxyinfo.getProxyUser().size() > 0) {
            cmd_proxyinfo.user = proxyinfo.getProxyUser().toStdString();
            cmd_proxyinfo.pwd = proxyinfo.getProxyPwd().toStdString();
        }
    }

    qDebug() << "buildProxyInfo is returning host= " << QString::fromStdString(cmd_proxyinfo.host) << " port= " << cmd_proxyinfo.port;
    qDebug() << "buildProxyInfo proxy authentication: " <<  (cmd_proxyinfo.user.size() > 0 ? "YES" : "NO" );
    return cmd_proxyinfo;
}

void GAPI::initTranslation() {

    QString     appPath = QCoreApplication::applicationDirPath();
    m_Settings.setExePath(appPath);
    QString CurrLng   = m_Settings.getGuiLanguageString();
    if (LoadTranslationFile(CurrLng)==false){
        emit signalLanguageChangedError();
    }
}

long GAPI::getFileSize(QString filePath) {
    QFile file(filePath);
      if (!file.exists()) {
          qDebug() << "GAPI::getFileSize() - File missing: " << filePath;
          return 1;
      }
      return file.size();
}

bool GAPI::LoadTranslationFile(QString NewLanguage)
{
    QString strTranslationFile;
    strTranslationFile = QString("eidmw_") + NewLanguage;

    qDebug() << "C++: GAPI LoadTranslationFile" << strTranslationFile << m_Settings.getExePath();

    if (!m_translator.load(strTranslationFile,m_Settings.getExePath()+"/"))
    {
        // this should not happen, since we've built the menu with the translation filenames
        strTranslationFile = QString("eidmw_") + STR_DEF_GUILANGUAGE;
        //try load default translation file
        qDebug() << "C++: AppController GAPI" << strTranslationFile << m_Settings.getExePath();
        if (!m_translator.load(strTranslationFile,m_Settings.getExePath()+"/"))
        {
            // this should not happen too, since we've built the menu with the translation filenames
            qDebug() << "C++: AppController Load Default Translation File Error";
            return false;
        }
        qDebug() << "C++: AppController Loaded Default Translation File";
        qApp->installTranslator(&m_translator);
        return false;
    }
    //------------------------------------
    // install the translator object and load the .qm file for
    // the given language.
    //------------------------------------
    qApp->installTranslator(&m_translator);
    return true;
}

QString GAPI::getDataCardIdentifyValue(IDInfoKey key) {

    qDebug() << "C++: getDataCardIdentifyValue ";

    return m_data[key];
}

void GAPI::setDataCardIdentify(QMap<IDInfoKey, QString> data) {

    qDebug() << "C++: setDataCardIdentify ";

    m_data = data;
    emit signalCardDataChanged();
}

bool isExpiredDate(const char * strDate) {
    if (strDate == NULL)
        return false;

    QDate qDate = QDate::fromString(strDate, "dd MM yyyy");
    QDate curDate = QDate::currentDate();

    if ( curDate > qDate )
        return true;
    return false;
}

QString GAPI::getAddressField(AddressInfoKey key) {
    return m_addressData[key];
}

#define BEGIN_TRY_CATCH  \
    try					  \
{

#define END_TRY_CATCH    \
    }                    \
    catch (PTEID_ExNoReader &) \
{                           \
    qDebug() << "No card reader found !"; \
    emit signalCardAccessError(NoReaderFound); \
    }     \
    catch (PTEID_ExNoCardPresent &) \
{     \
    qDebug() << "No card present."; \
    emit signalCardAccessError(NoCardFound); \
    }     \
    catch (PTEID_Exception &e) \
{ \
    long errorCode = e.GetError(); \
    if (errorCode >= EIDMW_SOD_UNEXPECTED_VALUE && \
    errorCode <= EIDMW_SOD_ERR_VERIFY_SOD_SIGN) \
{ \
    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "SOD exception! Error code (see strings in eidErrors.h): %08lx\n", e.GetError()); \
    emit signalCardAccessError(SodCardReadError); \
    } else if (errorCode == EIDMW_TIMESTAMP_ERROR) \
{ \
    emit signalPdfSignSucess(SignMessageTimestampFailed); \
    } else if (errorCode == EIDMW_PERMISSION_DENIED) \
{ \
    emit signalPdfSignFail(SignFilePermissionFailed); \
    } else if (errorCode == EIDMW_PDF_UNSUPPORTED_ERROR) \
{ \
	emit signalPdfSignFail(PDFFileUnsupported); \
    } else if (errorCode == EIDMW_ERR_PIN_CANCEL) \
{ \
    emit signalCardAccessError(CardUserPinCancel); \
    } \
    else if (errorCode == EIDMW_ERR_PIN_BLOCKED) \
{ \
    emit signalCardAccessError(PinBlocked); \
    } \
    else if (errorCode == EIDMW_ERR_TIMEOUT) \
{ \
    emit signalCardAccessError(CardPinTimeout); \
    } \
    else \
{ \
    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Generic eidlib exception! Error code (see strings in eidErrors.h): %08lx\n", e.GetError()); \
    /* Discard the 0xe1d0 prefix */                       \
    unsigned long user_error = e.GetError() & 0x0000FFFF; \
    QString msgError = QString("%1\n").arg(user_error); \
    emit signalGenericError(msgError); \
    } \
    }

void GAPI::getAddressFile() {
    qDebug() << "C++: getAddressFile()";
    bool m_foreign;

    BEGIN_TRY_CATCH;

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    PTEID_Address &addressFile = card->getAddr();

    if (!addressFile.isNationalAddress())
    {
        qDebug() << "Is foreign citizen";
        m_foreign = true;
        m_addressData[Foreigncountry] = QString::fromUtf8(addressFile.getForeignCountry());
        m_addressData[Foreignaddress] = QString::fromUtf8(addressFile.getForeignAddress());
        m_addressData[Foreigncity] = QString::fromUtf8(addressFile.getForeignCity());
        m_addressData[Foreignregion] = QString::fromUtf8(addressFile.getForeignRegion());
        m_addressData[Foreignlocality] = QString::fromUtf8(addressFile.getForeignLocality());
        m_addressData[Foreignpostalcode] = QString::fromUtf8(addressFile.getForeignPostalCode());
    }
    else
    {
        qDebug() << "Is national citizen";
        m_foreign = false;
        m_addressData[District] = QString::fromUtf8(addressFile.getDistrict());
        m_addressData[Municipality] = QString::fromUtf8(addressFile.getMunicipality());
        m_addressData[Parish] = QString::fromUtf8(addressFile.getCivilParish());
        m_addressData[Streettype] = QString::fromUtf8(addressFile.getStreetType());
        m_addressData[Streetname] = QString::fromUtf8(addressFile.getStreetName());
        m_addressData[Buildingtype] = QString::fromUtf8(addressFile.getBuildingType());
        m_addressData[Doorno] = QString::fromUtf8(addressFile.getDoorNo());
        m_addressData[Floor] = QString::fromUtf8(addressFile.getFloor());
        m_addressData[Side] = QString::fromUtf8(addressFile.getSide());
        m_addressData[Locality] = QString::fromUtf8(addressFile.getLocality());
        m_addressData[Place] = QString::fromUtf8(addressFile.getPlace());
        m_addressData[Zip4] = QString::fromUtf8(addressFile.getZip4());
        m_addressData[Zip3] = QString::fromUtf8(addressFile.getZip3());
        m_addressData[PostalLocality] = QString::fromUtf8(addressFile.getPostalLocality());
    }
    emit signalAddressLoaded(m_foreign);

    END_TRY_CATCH
}

void GAPI::getPersoDataFile() {

    qDebug() << "getPersoDataFile() called";

    BEGIN_TRY_CATCH;

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    emit signalPersoDataLoaded(QString(card->readPersonalNotes()));

    END_TRY_CATCH

}

void GAPI::setPersoDataFile(QString text) {

    qDebug() << "setPersoDataFile() called";

    int testAuthPin = verifyAuthPin("");

    if( testAuthPin == 0 || testAuthPin == TRIES_LEFT_ERROR) {
        return;
    }

    try {
        QString TxtPersoDataString = text.toUtf8();;

        PTEID_ReaderContext &ReaderContext  = ReaderSet.getReader();
        PTEID_EIDCard	 &Card	= ReaderContext.getEIDCard();

	if ( TxtPersoDataString.toStdString().size() > 0 ){
            const PTEID_ByteArray oData(reinterpret_cast<const unsigned char*> (TxtPersoDataString.toStdString().c_str()), (TxtPersoDataString.toStdString().size() + 1) );
            Card.writePersonalNotes(oData);
        }
        else {
            Card.clearPersonalNotes();
        }
        qDebug() << "Personal notes successfully written!" ;
        emit signalSetPersoDataFile(tr("STR_POPUP_SUCESS"),tr("STR_PERSONAL_NOTES_SUCESS"));

    } catch (PTEID_Exception& e) {
        qDebug() << "Error writing personal notes!" ;
        emit signalSetPersoDataFile(tr("STR_POPUP_ERROR"),tr("STR_PERSONAL_NOTES_ERROR"));
    }

}

unsigned int GAPI::verifyAuthPin(QString pin_value) {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & auth_pin = card->getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
    auth_pin.verifyPin(pin_value.toLatin1().data(), tries_left);

    if (tries_left == 0) {
        qDebug() << "WARNING: Auth PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

            //QML default types don't include long
            return (unsigned int)tries_left;
}

unsigned int GAPI::getTriesLeftAuthPin() {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & auth_pin = card->getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
    tries_left = auth_pin.getTriesLeft();

    if (tries_left == 0) {
        qDebug() << "WARNING: Auth PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

    //QML default types don't include long
    return (unsigned int)tries_left;
}

unsigned int GAPI::verifySignPin(QString pin_value) {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & sign_pin = card->getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
    sign_pin.verifyPin(pin_value.toLatin1().data(), tries_left);

    if (tries_left == 0) {
        qDebug() << "WARNING: Sign PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

            //QML default types don't include long
            return (unsigned int)tries_left;
}

unsigned int GAPI::getTriesLeftSignPin() {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & sign_pin = card->getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
    tries_left = sign_pin.getTriesLeft();

    if (tries_left == 0) {
        qDebug() << "WARNING: Sign PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

            //QML default types don't include long
            return (unsigned int)tries_left;
}

unsigned int GAPI::verifyAddressPin(QString pin_value) {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & address_pin = card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
    address_pin.verifyPin(pin_value.toLatin1().data(), tries_left);

    if (tries_left == 0) {
        qDebug() << "WARNING: Address PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

            //QML default types don't include long
            return (unsigned int)tries_left;
}

unsigned int GAPI::getTriesLeftAddressPin() {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & address_pin = card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
    tries_left = address_pin.getTriesLeft();

    if (tries_left == 0) {
        qDebug() << "WARNING: Address PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

            //QML default types don't include long
            return (unsigned int)tries_left;
}

unsigned int GAPI::changeAuthPin(QString currentPin, QString newPin) {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & auth_pin = card->getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
    auth_pin.changePin(currentPin.toLatin1().data(), newPin.toLatin1().data(),tries_left,"");

    if (tries_left == 0) {
        qDebug() << "WARNING: Auth PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

            //QML default types don't include long
            return (unsigned int)tries_left;
}

unsigned int GAPI::changeSignPin(QString currentPin, QString newPin) {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & sign_pin = card->getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
    sign_pin.changePin(currentPin.toLatin1().data(), newPin.toLatin1().data(),tries_left,"");

    if (tries_left == 0) {
        qDebug() << "WARNING: Sign PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

    //QML default types don't include long
    return (unsigned int)tries_left;
}
void GAPI::showChangeAddressDialog(long code)
{
    QString error_msg;
    long sam_error_code = 0;
    QString support_string = tr("STR_CHANGE_ADDRESS_ERROR_MSG");
    QString support_string_wait_5min = tr("STR_CHANGE_ADDRESS_WAIT_5MIN_ERROR_MSG");

    PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "AddressChange op finished with error code 0x%08x", code);

    switch(code)
    {
    case 0:
        error_msg = tr("STR_CHANGE_ADDRESS_SUCESS");
        break;
        //The error code for connection error is common between SAM and OTP
    case EIDMW_OTP_CONNECTION_ERROR:
        error_msg = "<b>" + tr("STR_CHANGE_ADDRESS_ERROR") + "</b><br><br>" + tr("STR_CONNECTION_ERROR") + "<br><br>" +
                tr("STR_VERIFY_INTERNET");
        break;

    case EIDMW_OTP_CERTIFICATE_ERROR:
        error_msg = "<b>" + tr("STR_CHANGE_ADDRESS_ERROR") + "</b><br><br>" + tr("STR_CONNECTION_ERROR") + "<br><br>" +
                tr("STR_VERIFY_APP_UPDATE");
        break;

    case EIDMW_SAM_PROXY_AUTH_FAILED:
        error_msg = "<b>" + tr("STR_CHANGE_ADDRESS_ERROR") + "</b><br><br>" + tr("STR_CONNECTION_ERROR") + "<br><br>" +
                    tr("STR_PROXY_AUTH_FAILED");
        break;
    case EIDMW_SAM_PROXY_UNSUPPORTED:
        error_msg = "<b>" + tr("STR_CHANGE_ADDRESS_ERROR") + "</b><br><br>" + tr("STR_CONNECTION_ERROR") + "<br><br>" +
                    tr("STR_PROXY_UNSUPPORTED");
        break;

    case 1121:
    case 1122:
        error_msg = "<b>" + tr("STR_CHANGE_ADDRESS_ERROR") + "</b><br><br>" + tr("STR_CHANGE_ADDRESS_CHECK_PROCESS_NUMBER");
        sam_error_code = code;
        break;
    case EIDMW_SAM_UNCONFIRMED_CHANGE:
        error_msg = "<b>" + tr("STR_CHANGE_ADDRESS_ERROR") + "</b><br><br>" + tr("STR_CHANGE_ADDRESS_ERROR_INCOMPLETE") + "<br><br>" + tr("STR_CHANGE_ADDRESS_NOT_CONFIRMED");
        break;
    case EIDMW_SSL_PROTOCOL_ERROR:
        error_msg = "<b>" + tr("STR_CHANGE_ADDRESS_ERROR") + "</b><br><br>" + tr("STR_CHANGE_ADDRESS_CHECK_AUTHENTICATION_CERTIFICATE");
        break;
    default:
        //Make sure we only show the user error codes from the SAM service and not some weird pteid exception error code
        if (code > 1100 && code < 3500)
            sam_error_code = code;
        error_msg = "<b>" + tr("STR_CHANGE_ADDRESS_ERROR") + "</b><br><br>" + "\n\n";
        break;
    }

    if (sam_error_code != 0)
    {
        error_msg += "<br><br>" + tr("STR_ERROR_CODE") + QString::number(sam_error_code);
    }

    if(code == EIDMW_SAM_UNCONFIRMED_CHANGE){
        error_msg += "<br><br>" + support_string_wait_5min;
    } else if (code != 0){
        error_msg += "<br><br>" + support_string;
    }

    qDebug() << error_msg;
    signalUpdateProgressStatus(error_msg);

    //TODO: Reload card information in case of successful address change
}

void GAPI::showSignCMDDialog(long code)
{
    QString error_msg;
    QString support_string = tr("STR_CMD_ERROR_MSG");

    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "CMD signature op finished with error code 0x%08x", code);

    switch(code)
    {
        case 0:
            error_msg = tr("STR_CMD_SUCESS");
            break;
        case SCAP_SERVICE_ERROR_CODE:
            error_msg = tr("STR_SCAP_SIGNATURE_ERROR");
            break;
        case -1:
            error_msg = tr("STR_CMD_TIMEOUT_ERROR");
            break;
        case ERR_GET_CERTIFICATE:
            error_msg = tr("STR_CMD_GET_CERTIFICATE_ERROR");
            break;
        case SOAP_TCP_ERROR:
            error_msg = tr("STR_CONNECTION_ERROR") + "<br><br>" +
                    tr("STR_VERIFY_INTERNET");
            break;
        case SOAP_ERR_SERVICE_FAIL:
            error_msg = tr("STR_CMD_SERVICE_FAIL");
            break;
        case SOAP_ERR_INVALID_PIN:
            error_msg = tr("STR_CMD_INVALID_PIN");
            break;  
        case SOAP_ERR_INVALID_OTP:
            error_msg = tr("STR_CMD_INVALID_OTP");
            break;        
        case SOAP_ERR_OTP_VALIDATION_ERROR:
            error_msg = tr("STR_CMD_OTP_VALIDATION_ERROR");
            break;
        case SOAP_ERR_INACTIVE_SERVICE:
            error_msg = tr("STR_CMD_INACTIVE_SERVICE");
            break;
        default:
            error_msg = tr("STR_CMD_LOGIN_ERROR");
            break;
    }

    if (code != 0)
        error_msg += "<br><br>" + support_string;

    qDebug() << error_msg;
    signalUpdateProgressStatus(error_msg);
}

unsigned int GAPI::changeAddressPin(QString currentPin, QString newPin) {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return TRIES_LEFT_ERROR;

    PTEID_Pin & address_pin = card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
    address_pin.changePin(currentPin.toLatin1().data(), newPin.toLatin1().data(),tries_left,"");

    if (tries_left == 0) {
        qDebug() << "WARNING: Address PIN blocked!" + tries_left;
    }

    END_TRY_CATCH

            //QML default types don't include long
            return (unsigned int)tries_left;
}

void GAPI::addressChangeCallback(void *instance, int value)
{
    qDebug() << "addressChangeCallback";
    GAPI * gapi = (GAPI*) (instance);
    gapi->signalUpdateProgressBar(value);
}

void GAPI::doChangeAddress(const char *process, const char *secret_code)
{
    qDebug() << "DoChangeAddress!";

    try
    {
        PTEID_EIDCard * card = NULL;
        getCardInstance(card);
        if (card == NULL) return;

        card->ChangeAddress((char *)secret_code, (char*)process, &GAPI::addressChangeCallback, (void*)this);
    }
    catch(PTEID_Exception & exception)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Caught exception in EidCard.ChangeAddress()... closing progressBar");
        this->signalUpdateProgressBar(100);
        this->addressChangeFinished(exception.GetError());

        free((char *)process);
        free((char *)secret_code);
        return;
    }

    free((char *)process);
    free((char *)secret_code);
    this->addressChangeFinished(0);
}

void GAPI::changeAddress(QString process, QString secret_code)
{
    qDebug() << "ChangeAddress! process = " + process + " secret_code = " + secret_code;

    signalUpdateProgressStatus(tr("STR_CHANGING_ADDRESS"));

    connect(this, SIGNAL(addressChangeFinished(long)),
            this, SLOT(showChangeAddressDialog(long)), Qt::UniqueConnection);

    char *processUtf8 = strdup(process.toUtf8().constData());
    char *secret_codeUtf8 = strdup(secret_code.toUtf8().constData());
    QtConcurrent::run(this, &GAPI::doChangeAddress, processUtf8, secret_codeUtf8);
}

QString GAPI::translateCMDErrorCode(int errorCode) {
   QString errorMsg;

   switch (errorCode) {
     case SOAP_TCP_ERROR:
       errorMsg = tr("STR_CONNECTION_ERROR") + "\n\n" +
                tr("STR_VERIFY_INTERNET");
       break;
   }

   return errorMsg;
}

void GAPI::doOpenSignCMD(CMDSignature *cmd_signature, CmdSignParams &params)
{
    qDebug() << "doOpenSignCMD! MobileNumber = " << params.mobileNumber << " secret_code = " << params.secret_code <<
                " loadedFilePath = " << params.loadedFilePath << " outputFile = " << params.outputFile <<
                "page = " << params.page << "coord_x" << params.coord_x << "coord_y" << params.coord_y <<
                "reason = " << params.reason << "location = " << params.location;

    int ret = 0;

    try {
        signalUpdateProgressBar(25);
        CMDProxyInfo proxyInfo = buildProxyInfo();
        ret = cmd_signature->signOpen(proxyInfo, params.mobileNumber.toStdString(), params.secret_code.toStdString(),
                                      params.page,
                                      params.coord_x, params.coord_y,
                                      params.location.toUtf8().data(), params.reason.toUtf8().data(),
                                      getPlatformNativeString(params.outputFile));

        if ( ret != 0 ) {
            qDebug() << "signOpen failed! - ret: " << ret << endl;
     
            signCMDFinished(ret);
            signalUpdateProgressBar(100);
            return;
        }


    } catch (PTEID_Exception &e) {
        qDebug() << "Caught exception in some SDK method. Error code: " << hex << e.GetError() << endl;
        signalUpdateProgressBar(100);
    }

    signalUpdateProgressBar(50);
    signalUpdateProgressStatus(tr("STR_CMD_LOGIN_SUCESS"));
    emit signalOpenCMDSucess();
}

void GAPI::doCloseSignCMD(CMDSignature *cmd_signature, QString sms_token)
{
    qDebug() << "doCloseSignCMD! " << "sms_token = " << sms_token;

    int ret = 0;
    std::string local_sms_token = sms_token.toUtf8().data();

    try {
        signalUpdateProgressBar(75);
        ret = cmd_signature->signClose(local_sms_token);
        if ( ret != 0 ) {
            qDebug() << "signClose failed!" << endl;
            signCMDFinished(ret);
            signalUpdateProgressBar(100);
            return;
        }

    } catch (PTEID_Exception &e) {
        qDebug() << "Caught exception in some SDK method. Error code: " << hex << e.GetError() << endl;
    }

    signCMDFinished(ret);
    signalUpdateProgressBar(100);
    emit signalCloseCMDSucess();
}

void GAPI::doCloseSignCMDWithSCAP(CMDSignature *cmd_signature, QString sms_token, QList<int> attribute_list) {
        qDebug() << "doCloseSignCMDWithSCAP! " << "sms_token = " << sms_token
          <<"attribute_list size: " << attribute_list.size();

    int ret = 0;
    std::string local_sms_token = sms_token.toUtf8().data();

    try {
        signalUpdateProgressBar(65);
        ret = cmd_signature->signClose(local_sms_token );
        if ( ret != 0 ) {
            qDebug() << "signClose failed!" << endl;
            signCMDFinished(ret);
            signalUpdateProgressBar(100);
            return;
        }

    } catch (PTEID_Exception &e) {
        qDebug() << "Caught exception in some SDK method. Error code: " << hex << e.GetError() << endl;
    }

    signalUpdateProgressStatus(tr("STR_CMD_SIGNING_SCAP"));

    signalUpdateProgressBar(80);

    //Do SCAP Signature
    std::vector<int> attrs;
    for (unsigned int i = 0; i!= attribute_list.size(); i++) {
        attrs.push_back(attribute_list.at(i));
    }

    //See details of this
    CmdSignedFileDetails cmd_details;
    cmd_details.signedCMDFile = m_scap_params.inputPDF;

    cmd_details.citizenName = cmd_signature->getCertificateCitizenName();
    //The method returns something like "BI123456789";
    cmd_details.citizenId = QString(cmd_signature->getCertificateCitizenID()+2);

    scapServices.executeSCAPWithCMDSignature(this, m_scap_params.outputPDF, m_scap_params.page,
                m_scap_params.location_x, m_scap_params.location_y,
                m_scap_params.location, m_scap_params.reason, 0, attrs, cmd_details);

    //TODO: reset the m_scap_params struct
}


QString generateTempFile() {
    QTemporaryFile tempFile;

    if (!tempFile.open()) {
        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "generateTempFile", "SCAP Signature error: Error creating temporary file");
        return "";
    }

    return tempFile.fileName();
}

void GAPI::signOpenScapWithCMD(QString mobileNumber, QString secret_code, QString loadedFilePath,
                   QString outputFile, int page, double coord_x, double coord_y,
                   QString reason, QString location) {

    qDebug() << "signOpenScapWithCMD! MobileNumber = " + mobileNumber + " secret_code = " + secret_code +
                " reason = " + reason + " location = " + location +
                " loadedFilePath = " + loadedFilePath + " outputFile = " + outputFile +
                "page = " + page + "coord_x" + coord_x + "coord_y" + coord_y;

    signalUpdateProgressStatus(tr("STR_CMD_CONNECTING"));

    connect(this, SIGNAL(signCMDFinished(long)),
            this, SLOT(showSignCMDDialog(long)), Qt::UniqueConnection);

    //Final params for the SCAP signature (visible PDF signature params)  
    m_scap_params.outputPDF = outputFile;
    m_scap_params.inputPDF = generateTempFile();
    m_scap_params.page = page;
    m_scap_params.location_x = coord_x;
    m_scap_params.location_y = coord_y;
    m_scap_params.location = location;
    m_scap_params.reason = reason;


    CmdSignParams params;

    //Invisible CMD signature
    params.secret_code = secret_code;
    params.mobileNumber = mobileNumber;
    params.loadedFilePath = loadedFilePath;
    params.outputFile = m_scap_params.inputPDF;
    params.page = 0;
    params.coord_x = -1;
    params.coord_y = -1;
    params.location = location;
    params.reason = reason;
    params.isTimestamp = 0;
    params.isSmallSignature = 0;

    QString fullInputPath = params.loadedFilePath;

    cmd_pdfSignature->setFileSigning((char *)getPlatformNativeString(fullInputPath));

    cmd_signature->set_pdf_handler(cmd_pdfSignature);
    QtConcurrent::run(this, &GAPI::doOpenSignCMD, cmd_signature, params);

}

void GAPI::signOpenCMD(QString mobileNumber, QString secret_code, QString loadedFilePath,
                   QString outputFile, int page, double coord_x, double coord_y,
                   QString reason, QString location, double isTimestamp, double isSmall)
{
    qDebug() << "signOpenCMD! MobileNumber = " + mobileNumber + " secret_code = " + secret_code +
                " loadedFilePath = " + loadedFilePath + " outputFile = " + outputFile +
                "page = " + page + "coord_x" + coord_x + "coord_y" + coord_y +
                "reason = " + reason + "location = " + location +
                "isTimestamp = " +  isTimestamp + "isSmall = " + isSmall;

    signalUpdateProgressStatus(tr("STR_CMD_CONNECTING"));

    connect(this, SIGNAL(signCMDFinished(long)),
            this, SLOT(showSignCMDDialog(long)), Qt::UniqueConnection);

    CmdSignParams params = {mobileNumber, secret_code,loadedFilePath,outputFile,
                            page,coord_x,coord_y,reason,location,isTimestamp,isSmall};

    QString fullInputPath = params.loadedFilePath;

    cmd_pdfSignature->setFileSigning((char *)getPlatformNativeString(fullInputPath));

    if (params.isTimestamp > 0)
        cmd_pdfSignature->enableTimestamp();
    if (params.isSmallSignature > 0)
        cmd_pdfSignature->enableSmallSignatureFormat();

    if(useCustomSignature()) {
        const PTEID_ByteArray imageData(reinterpret_cast<const unsigned char *>(m_jpeg_scaled_data.data()), static_cast<unsigned long>(m_jpeg_scaled_data.size()));
        cmd_pdfSignature->setCustomImage(imageData);
    }

    cmd_signature->set_pdf_handler(cmd_pdfSignature);
    QtConcurrent::run(this, &GAPI::doOpenSignCMD, cmd_signature, params);
}

void GAPI::signCloseCMD(QString sms_token, QList<int> attribute_list)
{
    qDebug() << "signCloseCMD! sms_token = " + sms_token;

    signalUpdateProgressStatus(tr("STR_CMD_SENDING_CODE"));

    if (attribute_list.size() > 0)
        QtConcurrent::run(this, &GAPI::doCloseSignCMDWithSCAP, cmd_signature, sms_token, attribute_list);

    else
        QtConcurrent::run(this, &GAPI::doCloseSignCMD, cmd_signature, sms_token);
}

QString GAPI::getCardActivation() {
    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return "Error getting Card Activation";

    PTEID_EId &eid_file = card->getID();

    PTEID_Certificates&	 certificates	= card->getCertificates();

    int certificateStatus = PTEID_CERTIF_STATUS_UNKNOWN;

    certificateStatus = certificates.getCert(PTEID_Certificate::CITIZEN_AUTH).getStatus();

    // If state active AND validity not expired AND certificate active
    if(card->isActive() && !isExpiredDate(eid_file.getValidityEndDate())
            && certificateStatus == PTEID_CERTIF_STATUS_VALID)
    {
        return QString(tr("STR_CARD_ACTIVE_AND_VALID"));
    }
    // Else If state active AND validity not expired AND certificate error
    else if(card->isActive() && !isExpiredDate(eid_file.getValidityEndDate())
            && (certificateStatus == PTEID_CERTIF_STATUS_CONNECT || certificateStatus == PTEID_CERTIF_STATUS_ERROR
            || certificateStatus == PTEID_CERTIF_STATUS_ISSUER || certificateStatus == PTEID_CERTIF_STATUS_UNKNOWN))
    {
        return QString(tr("STR_CARD_CONNECTION_ERROR"));
    }
    //Else If state active AND validity not expired AND certificate suspended or revoked
    else if(card->isActive() && !isExpiredDate(eid_file.getValidityEndDate())
            && (certificateStatus == PTEID_CERTIF_STATUS_SUSPENDED || certificateStatus == PTEID_CERTIF_STATUS_REVOKED))
    {
        return QString(tr("STR_CARD_CANCELED"));
    }
    //Else If state active AND validity expired
    else if(card->isActive() && isExpiredDate(eid_file.getValidityEndDate()))
    {
        return QString(tr("STR_CARD_EXPIRED"));
    }
    //Else If state not active
    else if(!card->isActive())
    {
        return QString(tr("STR_CARD_NOT_ACTIVE"));
    }
    END_TRY_CATCH
}

QPixmap PhotoImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (id != "photo.png") {
        qDebug() << "PhotoImageProvider: wrong id requested - " << id;
        return QPixmap();
    }

    if (!requestedSize.isValid())
    {
        if (requestedSize.height() > p.height() || requestedSize.width() > p.width())
        {
            qDebug() << "PhotoImageProvider: Invalid requestedSize - " << requestedSize;
            return QPixmap();
        }
    }

    size->setWidth(p.width());
    size->setHeight(p.height());

    return p;
}

void GAPI::startPrintPDF(QString outputFile, bool isBasicInfo, bool isAddicionalInfo,
                         bool isAddress, bool isNotes, bool isPrintDate, bool isSign) {

    PrintParams params = {outputFile, isBasicInfo, isAddicionalInfo, isAddress, isNotes, isPrintDate, isSign};
    QtConcurrent::run(this, &GAPI::doPrintPDF, params);
}

void GAPI::startPrint(QString outputFile, bool isBasicInfo, bool isAddicionalInfo,
                         bool isAddress, bool isNotes, bool isPrintDate, bool isSign) {

    if (QPrinterInfo::availablePrinters().size() == 0) {
        qDebug() << "No Printers Available";
        emit signalPrinterPrintFail(NoPrinterAvailable);
        return;
    }

    PrintParams params = {outputFile, isBasicInfo, isAddicionalInfo, isAddress, isNotes, isPrintDate, isSign};

    QPrinter printer;
    bool res = false;

    printer.setDocName("CartaoCidadao.pdf");
    QPrintDialog *dlg = new QPrintDialog(&printer);
    if(dlg->exec() == QDialog::Accepted) {
         qDebug() << "QPrintDialog! Accepted";
         BEGIN_TRY_CATCH;
         // Print PDF not Signed
         res = drawpdf(printer, params);

         if ( params.outputFile.toUtf8().size() > 0){
             qDebug() << "Create PDF";
             if (res) {
                 emit signalPdfPrintSucess();
             }else{
                 emit signalPdfPrintFail();
             }
         }else{
             qDebug() << "Printing to a printer";
             if (res) {
                 emit signalPrinterPrintSucess();
             }else{
                 emit signalPrinterPrintFail();
             }
         }
         END_TRY_CATCH
    }
}

bool GAPI::doSignPrintPDF(QString &file_to_sign, QString &outputsign) {

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;

    getCardInstance(card);
    if (card == NULL) return false;

    PTEID_PDFSignature sig_handler(getPlatformNativeString(file_to_sign));

    card->SignPDF(sig_handler, 0, 0, false, "", "", getPlatformNativeString(outputsign));

    return true;

    END_TRY_CATCH

            return false;
}

void GAPI::doPrintPDF(PrintParams &params) {

    qDebug() << "doPrintPDF! outputFile = " << params.outputFile <<
                "isBasicInfo = " << params.isBasicInfo << "isAddicionalInfo" << params.isAddicionalInfo << "isAddress"
             << params.isAddress << "isNotes = " << params.isNotes << "isPrintDate = " << params.isPrintDate << "isSign = " << params.isSign;

    QString pdffiletmp;
    QPrinter pdf_printer;
    QString nativepdftmp;
    QString originalOutputFile;
    bool res = false;

    BEGIN_TRY_CATCH;
    if (params.isSign)
    {
        // Print PDF Signed
        pdffiletmp = QDir::tempPath();
        pdffiletmp.append("/CartaoCidadao.pdf");
        nativepdftmp = QDir::toNativeSeparators(pdffiletmp);
        originalOutputFile = params.outputFile;
        params.outputFile = nativepdftmp;
        res = drawpdf(pdf_printer, params);
        if (!res)
        {
            emit signalPdfPrintFail();
            return;
        }

        QFuture<bool> new_thread = QtConcurrent::run(this, &GAPI::doSignPrintPDF, nativepdftmp,
                                                     originalOutputFile);

        res = new_thread.result();
        // Emit signal if success but if false a popup about some error is already sent
        if (res)
            emit signalPdfPrintSignSucess();
    } else {
        // Print PDF not Signed
        res = drawpdf(pdf_printer, params);
        if (res) {
            emit signalPdfPrintSucess();
        }else{
            emit signalPdfPrintFail();
        }
    }
    END_TRY_CATCH
}

static QPen black_pen;
static QPen blue_pen;

double drawSingleField(QPainter &painter, double pos_x, double pos_y, QString name, QString value, double line_length, int field_margin=15, bool is_bounded_rect=false, double bound_width=360)
{
    painter.setPen(blue_pen);

    if (field_margin == 0){
        line_length -= 15;
    }

    painter.drawText(QPointF(pos_x + field_margin, pos_y), name);
    pos_y += 7;

    painter.drawLine(QPointF(pos_x + field_margin, pos_y), QPointF(pos_x + (line_length - field_margin) , pos_y));
    painter.setPen(black_pen);

    if (is_bounded_rect){
        int flags = Qt::TextWordWrap | Qt::TextWrapAnywhere;
        int textFlags = Qt::TextWordWrap;
        QFontMetricsF fm = painter.fontMetrics();
        QRectF bounding_rect = fm.boundingRect(QRectF(pos_x + field_margin, pos_y, bound_width -  2 * field_margin ,  500), flags, value);

        painter.drawText(bounding_rect.adjusted(0, 0, 0, 0), textFlags, value);
        pos_y += bounding_rect.height() + 15;
    } else {
        pos_y += 15;
        painter.drawText(QPointF(pos_x + field_margin, pos_y), value);
    }

    return pos_y;
}

void drawSectionHeader(QPainter &painter, double pos_x, double pos_y, QString section_name)
{
    QFont header_font("DIN Light");
    header_font.setPointSize(11);
    QFont regular_font("DIN Medium");
    regular_font.setPointSize(12);

    painter.setFont(header_font);

    QColor light_grey(233, 233, 233);
    painter.setBrush(light_grey);
    painter.setPen(light_grey);

    painter.drawRoundedRect(QRectF(pos_x, pos_y, 250, 30), 10.0, 10.0);
    painter.setPen(black_pen);

    painter.drawText(pos_x+20, pos_y+20, section_name);

    painter.setFont(regular_font);
}

void drawPrintingDate(QPainter &painter, QString printing_date){
    QFont date_font("DIN Medium");
    date_font.setPointSize(8);
    date_font.setBold(false);
    QFont regular_font("DIN Medium");
    regular_font.setPointSize(12);

    printing_date += " " +  QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    painter.setFont(date_font);
    painter.drawText(QRectF(painter.device()->width() - 225, painter.device()->height() - 25, 200, 100), Qt::TextWordWrap, printing_date);

    painter.setFont(regular_font);
}

QPixmap loadHeader()
{
    return QPixmap(":/images/pdf_document_header.png");
}

QString getTextFromLines(QStringList lines, int start, int stop){
    QStringList text_lines;

    //make sure doesn't access a invalid index
    if (stop > lines.length())
    {
        stop = lines.length();
    }

    for (int i = start; i < stop; i++)
    {
         text_lines.append(lines.at(i));
    }

    return text_lines.join("\n");
}

double checkNewPageAndPrint(QPrinter &printer, QPainter &painter, double current_y, double remaining_height, double max_height, bool print_date=false, QString date_label=""){
    if (current_y + remaining_height > max_height){
        printer.newPage();
        current_y = 50;
        if (print_date)
        {
            drawPrintingDate(painter,  date_label);
        }
    }
    return current_y;
}

bool GAPI::drawpdf(QPrinter &printer, PrintParams params)
{
    qDebug() << "drawpdf! outputFile = " << params.outputFile <<
                "isBasicInfo = " << params.isBasicInfo << "isAddicionalInfo" << params.isAddicionalInfo << "isAddress"
             << params.isAddress << "isNotes = " << params.isNotes << "isPrintDate = " << params.isPrintDate << "isSign = " << params.isSign;
    //gives a bit of left margin
    double page_margin = 33.5;
    double pos_x = page_margin, pos_y = 0;
    bool res = false;
    int field_margin = 15;

    BEGIN_TRY_CATCH;

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return false;

    card->doSODCheck(true); //Enable SOD checking
    PTEID_EId &eid_file = card->getID();

    //QPrinter printer;
    printer.setResolution(96);
    printer.setColorMode(QPrinter::Color);
    printer.setPaperSize(QPrinter::A4);

    if ( params.outputFile.toUtf8().size() > 0){
        printer.setOutputFileName(params.outputFile.toUtf8().data());
        qDebug() << "Printing PDF";
    }else{
        qDebug() << "Printing to default printer";
    }

    //TODO: Add custom fonts
    //addFonts();

    //Start drawing
    QPainter painter;
    res = painter.begin(&printer);
    if(res == false){
        qDebug() << "Start drawing return error: " <<  res;
        return false;
    }
    double page_height = painter.device()->height();
    double page_width = painter.device()->width();
    double half_page = (page_width - 2 * page_margin) / 2;
    double third_of_page = (page_width - 2 * page_margin) / 3;

    //Font setup
    QFont din_font("DIN Medium");
    din_font.setPixelSize(18);

    //  Include header as png pixmap
    QPixmap header = loadHeader();

    painter.drawPixmap(QPointF(pos_x, pos_y), header);

    //  //Alternative using the QtSVG module, not enabled for now because the rendering is far from perfect
    //QSvgRenderer renderer(QString("C:\\Users\\agrr\\Desktop\\GMC_logo.svg"));
    //std::cout << renderer.defaultSize().width() << "x" << renderer.defaultSize().height() << std::endl;

    //renderer.render(&painter, QRect(pos_x, pos_y, 504, 132));
    //renderer.render(&painter, QRect(pos_x, pos_y, 250, 120));

    pos_y += header.height() + field_margin;
    black_pen = painter.pen();

    blue_pen.setColor(QColor(78, 138, 190));
    painter.setPen(blue_pen);

    int line_length = 487;
    //Horizontal separator below the CC logo
    painter.drawLine(QPointF(pos_x, pos_y), QPointF(pos_x+line_length, pos_y));
    pos_y += 25;

    //Change text color
    blue_pen = painter.pen();

    const double LINE_HEIGHT = 45.0;

    //    din_font.setBold(true);
    painter.setFont(din_font);

    painter.drawText(QPointF(pos_x, pos_y), tr("STR_PERSONAL_DATA"));

    pos_y += field_margin;
    int circle_radius = 6;

    //Draw 4 blue circles
    painter.setBrush(QColor(78, 138, 190));
    painter.drawEllipse(QPointF(pos_x+10, pos_y), circle_radius, circle_radius);
    painter.drawEllipse(QPointF(pos_x+40, pos_y), circle_radius, circle_radius);
    painter.drawEllipse(QPointF(pos_x+70, pos_y), circle_radius, circle_radius);
    painter.drawEllipse(QPointF(pos_x+100, pos_y), circle_radius, circle_radius);

    painter.setPen(black_pen);
    //Reset font
    din_font.setPixelSize(10);
    din_font.setBold(false);
    painter.setFont(din_font);

    pos_y += 30;

    if (params.isPrintDate)
    {
        drawPrintingDate(painter,  tr("STR_PRINTED_ON"));
    }

    if (params.isBasicInfo)
    {

        drawSectionHeader(painter, pos_x, pos_y, tr("STR_BASIC_INFORMATION"));

        //Load photo into a QPixmap
        PTEID_ByteArray& photo = eid_file.getPhotoObj().getphoto();

        //Image
        QPixmap pixmap_photo;
        pixmap_photo.loadFromData(photo.GetBytes(), photo.Size(), "PNG");

        const int img_height = 200;

        //Scale height if needed
        QPixmap scaled = pixmap_photo.scaledToHeight(img_height, Qt::SmoothTransformation);

        painter.drawPixmap(QPointF(pos_x + 500, pos_y-80), scaled);

        pos_y += 50;

        double new_pos_y = drawSingleField(painter, pos_x, pos_y, tr("STR_GIVEN_NAME"),
                                           QString::fromUtf8(eid_file.getGivenName()), half_page, 0, true, half_page);
        pos_y = (std::max)(new_pos_y, pos_y + LINE_HEIGHT);

        new_pos_y = drawSingleField(painter, pos_x, pos_y, tr("STR_SURNAME"),
                                    QString::fromUtf8(eid_file.getSurname()), half_page, 0, true, half_page);
        pos_y = (std::max)(new_pos_y, pos_y + LINE_HEIGHT);

        double f_col = half_page / 2;
        drawSingleField(painter, pos_x, pos_y, tr("STR_GENDER"), QString::fromUtf8(eid_file.getGender()), f_col, 0);
        drawSingleField(painter, pos_x + f_col, pos_y, tr("STR_HEIGHT"), QString::fromUtf8(eid_file.getHeight()), f_col);
        drawSingleField(painter, pos_x + 2 * f_col, pos_y, tr("STR_NATIONALITY"),
                        QString::fromUtf8(eid_file.getNationality()), f_col);
        drawSingleField(painter, pos_x + 3 * f_col, pos_y, tr("STR_DATE_OF_BIRTH"),
                        QString::fromUtf8(eid_file.getDateOfBirth()), f_col);
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_DOCUMENT_NUMBER"), QString::fromUtf8(eid_file.getDocumentNumber()), half_page, 0);
        drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_VALIDITY_DATE"),
                        QString::fromUtf8(eid_file.getValidityEndDate()), half_page);
        pos_y += LINE_HEIGHT;

        double new_height_left = drawSingleField(painter, pos_x, pos_y, tr("STR_FATHER"),
                                                 QString::fromUtf8(eid_file.getGivenNameFather()) + " " +
                                                 QString::fromUtf8(eid_file.getSurnameFather()), half_page, 0, true, half_page);
        double new_height_right = drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_MOTHER"),
                                                  QString::fromUtf8(eid_file.getGivenNameMother()) + " " +
                                                  QString::fromUtf8(eid_file.getSurnameMother()),  half_page, field_margin, true, half_page);
        pos_y = (std::max)((std::max)(new_height_left, new_height_right), pos_y + LINE_HEIGHT);

        new_pos_y = drawSingleField(painter, pos_x, pos_y, tr("STR_NOTES"),
                                    QString::fromUtf8(eid_file.getAccidentalIndications()), half_page, 0, true, page_width);

        pos_y = (std::max)(new_pos_y, pos_y + LINE_HEIGHT);
    }

    if (params.isAddicionalInfo)
    {
        //height of this section, if spacing between fields change update this value
        double min_section_height = 235;
        pos_y = checkNewPageAndPrint(printer, painter, pos_y, min_section_height, page_height, params.isPrintDate, tr("STR_PRINTED_ON"));

        drawSectionHeader(painter, pos_x, pos_y, tr("STR_ADDITIONAL_INFORMATION"));
        pos_y += 50;

        drawSingleField(painter, pos_x, pos_y, tr("STR_VAT_NUM"),
                        QString::fromUtf8(eid_file.getTaxNo()), third_of_page, 0);
        drawSingleField(painter, pos_x + third_of_page, pos_y, tr("STR_SOCIAL_SECURITY_NUM"),
                        QString::fromUtf8(eid_file.getSocialSecurityNumber()), third_of_page);
        drawSingleField(painter, pos_x + 2 * third_of_page, pos_y, tr("STR_NATIONAL_HEALTH_NUM"),
                        QString::fromUtf8(eid_file.getHealthNumber()), third_of_page);
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_DELIVERY_ENTITY"),
                        QString::fromUtf8(eid_file.getIssuingEntity()), half_page, 0, true, half_page);
        drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_DELIVERY_DATE"),
                        QString::fromUtf8(eid_file.getValidityBeginDate()), half_page);
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_DOCUMENT_TYPE"),
                        QString::fromUtf8(eid_file.getDocumentType()), half_page, 0, true, half_page);
        drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_DELIVERY_LOCATION"),
                        QString::fromUtf8(eid_file.getLocalofRequest()), half_page, field_margin, true, half_page);
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_CARD_VERSION"),
                        QString::fromUtf8(eid_file.getDocumentVersion()), half_page, 0);
        drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_CARD_STATE"),
                        getCardActivation(), half_page, field_margin, true, half_page);
        pos_y += 50;
    }

    if (params.isAddress)
    {
        //height of this section, if spacing between fields change update this value
        double min_section_height = 350;
        pos_y = checkNewPageAndPrint(printer, painter, pos_y, min_section_height, page_height, params.isPrintDate, tr("STR_PRINTED_ON"));

        drawSectionHeader(painter, pos_x, pos_y, tr("STR_ADDRESS"));
        pos_y += 50;

        PTEID_Address &addressFile = card->getAddr();

        if (addressFile.isNationalAddress()){
            double new_height_left = drawSingleField(painter, pos_x, pos_y, tr("STR_DISTRICT"),
                                                     QString::fromUtf8(addressFile.getDistrict()), half_page, 0, true, half_page);
            double new_height_right = drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_MUNICIPALITY"),
                                                      QString::fromUtf8(addressFile.getMunicipality()), half_page,
                                                      field_margin, true, half_page);
            pos_y = (std::max)((std::max)(new_height_left, new_height_right), pos_y + LINE_HEIGHT);

            new_height_left = drawSingleField(painter, pos_x, pos_y, tr("STR_CIVIL_PARISH"),
                                              QString::fromUtf8(addressFile.getCivilParish()), half_page, 0, true, page_width);
            pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

            drawSingleField(painter, pos_x, pos_y, tr("STR_STREET_TYPE"),
                            QString::fromUtf8(addressFile.getStreetType()), (half_page / 2), 0, true, (half_page / 2));

            new_height_left = drawSingleField(painter, pos_x + (half_page / 2), pos_y, tr("STR_STREET_NAME"),
                                              QString::fromUtf8(addressFile.getStreetName()), 3 * (half_page / 2),
                                              field_margin, true, 3 * (half_page / 2));
            pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

            new_height_left = drawSingleField(painter, pos_x, pos_y, tr("STR_HOUSE_BUILDING_NUM"),
                            QString::fromUtf8(addressFile.getDoorNo()), third_of_page, 0);
            new_height_right = drawSingleField(painter, pos_x + third_of_page, pos_y, tr("STR_FLOOR"),
                            QString::fromUtf8(addressFile.getFloor()), third_of_page);
            double new_height = drawSingleField(painter, pos_x + 2 * third_of_page, pos_y, tr("STR_SIDE"),
                            QString::fromUtf8(addressFile.getSide()), third_of_page);
            pos_y = (std::max)(new_height, (std::max)(new_height_left, (std::max)(new_height_left, pos_y + LINE_HEIGHT)));

            drawSingleField(painter, pos_x, pos_y, tr("STR_PLACE"),
                            QString::fromUtf8(addressFile.getPlace()), half_page, 0, true, half_page);
            new_height_left = drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_LOCALITY"),
                            QString::fromUtf8(addressFile.getLocality()), half_page, field_margin, true, half_page);

            pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

            drawSingleField(painter, pos_x, pos_y, tr("STR_ZIP_CODE"),
                            QString::fromUtf8(addressFile.getZip4()) + "-" + QString::fromUtf8(addressFile.getZip3()), half_page/2, 0);
            new_height_left = drawSingleField(painter, pos_x + (half_page / 2), pos_y, tr("STR_POSTAL_LOCALITY"),
                                              QString::fromUtf8(addressFile.getPostalLocality()),
                                              (3 * half_page) / 2, field_margin, true, (3 * half_page) / 2);
            pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

        }else{
            /* Foreign Address*/
            drawSingleField(painter, pos_x, pos_y, tr("STR_FOREIGN_COUNTRY"),
                            QString::fromUtf8(addressFile.getForeignCountry()), third_of_page, 0);
            drawSingleField(painter, pos_x + third_of_page, pos_y, tr("STR_FOREIGN_REGION"),
                            QString::fromUtf8(addressFile.getForeignRegion()), third_of_page);
            drawSingleField(painter, pos_x + 2 * third_of_page, pos_y, tr("STR_FOREIGN_CITY"),
                            QString::fromUtf8(addressFile.getForeignCity()), third_of_page);

            pos_y += LINE_HEIGHT;

            drawSingleField(painter, pos_x, pos_y, tr("STR_FOREIGN_LOCALITY"),
                            QString::fromUtf8(addressFile.getForeignLocality()), half_page, 0, true, half_page);
            drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_FOREIGN_POSTAL_CODE"),
                            QString::fromUtf8(addressFile.getForeignPostalCode()), half_page, field_margin, true, half_page);

            pos_y += LINE_HEIGHT;

            drawSingleField(painter, pos_x, pos_y, tr("STR_FOREIGN_ADDRESS"),
                            QString::fromUtf8(addressFile.getForeignAddress()), half_page, 0, true, page_width);
        }
        pos_y += 80;
    }

    if (params.isNotes)
    {
        QString perso_data;

        perso_data = QString(card->readPersonalNotes());

        if (perso_data.size() > 0)
        {
            pos_x = 30; //gives a bit of left margin
            pos_y += 50;

            pos_y = checkNewPageAndPrint(printer, painter, pos_y, 0, page_height, params.isPrintDate, tr("STR_PRINTED_ON"));

            drawSectionHeader(painter, pos_x, pos_y, tr("STR_PERSONAL_NOTES"));

            pos_y += 50;

            QStringList lines = perso_data.split("\n", QString::KeepEmptyParts);
            for (int j = 0; j < lines.count(); j++) {
                QString line = lines.at(j);
                if (!line.contains(" ")) {
                    int num_of_chars = line.count();
                    int chars_in_page = 75; //only 75 characters fit in the width of a A4 page
                    if(num_of_chars > chars_in_page) { //a really big word without spaces
                        for(int i = 1; i <= num_of_chars / chars_in_page; i++) {
                            int n = (i * chars_in_page) - 1;
                            line.insert(n, " "); //add whitespaces in such word to enforce WordWrap
                        }
                    }
                    lines.replace(j, line);
                }
             }

            const int TEXT_LINE_HEIGHT = 20;

            int line_count = lines.length();
            int height_to_print = TEXT_LINE_HEIGHT * line_count;
            int line_index_start = 0;
            int line_index_stop = 0;
            int completed_lines = 0;

            while (completed_lines < line_count){
                int page_remaining_space = static_cast<int>(page_height - pos_y);

                line_index_stop = line_index_start + (page_remaining_space / TEXT_LINE_HEIGHT);

                if (page_remaining_space < 50 && height_to_print > 0)
                {
                    printer.newPage();
                    pos_y = 50;
                    drawSectionHeader(painter, pos_x, pos_y, tr("STR_PERSONAL_NOTES"));
                    pos_y += 50;
                    if (params.isPrintDate)
                    {
                        drawPrintingDate(painter,  tr("STR_PRINTED_ON"));
                    }
                    // not enough area recalculate lines needed
                    page_remaining_space = static_cast<int>(page_height - pos_y);
                    line_index_stop = line_index_start + (page_remaining_space / TEXT_LINE_HEIGHT);
                }

                QString text = getTextFromLines(lines, line_index_start, line_index_stop);

                int diff = line_index_stop - line_index_start;

                painter.drawText(static_cast<int>(pos_x), static_cast<int>(pos_y), painter.device()->width() - 60,
                                 diff * TEXT_LINE_HEIGHT, Qt::TextWordWrap, text);

                height_to_print -= diff * TEXT_LINE_HEIGHT;
                pos_y += diff * TEXT_LINE_HEIGHT;
                completed_lines += diff;
                line_index_start = line_index_stop;
            }
        }
    }
    //Finish drawing/printing
    painter.end();
    END_TRY_CATCH
    return true;
}

void GAPI::startSigningPDF(QString loadedFilePath, QString outputFile, int page, double coord_x, double coord_y,
                           QString reason, QString location, bool isTimestamp, bool isSmall) {

    SignParams params = {loadedFilePath, outputFile, page, coord_x, coord_y, reason, location, isTimestamp, isSmall};
    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignPDF, params);

}

void GAPI::startSigningBatchPDF(QList<QString> loadedFileBatchPath, QString outputFile, int page, double coord_x, double coord_y,
                                QString reason, QString location, bool isTimestamp, bool isSmall) {

    SignBatchParams params = {loadedFileBatchPath, outputFile, page, coord_x, coord_y, reason, location, isTimestamp, isSmall};

    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignBatchPDF, params);
}

int GAPI::getPDFpageCount(QString loadedFilePath) {

    PTEID_PDFSignature sig_handler(getPlatformNativeString(loadedFilePath));

    int pageCount = sig_handler.getPageCount();

    return pageCount;
}

void GAPI::startSigningXADES(QString loadedFilePath, QString outputFile, bool isTimestamp) {
    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignXADES, loadedFilePath, outputFile, isTimestamp);

}

void GAPI::startSigningBatchXADES(QList<QString> loadedFileBatchPath, QString outputFile, bool isTimestamp) {

    SignBatchParams params = {loadedFileBatchPath, outputFile, 0, 0, 0, "", "", isTimestamp, 0};

    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignBatchXADES, params);
}

void GAPI::doSignBatchXADES(SignBatchParams &params) {
    qDebug() << "doSignBatchXADES! loadedFilePath = " << params.loadedFileBatchPath << " outputFile = " << params.outputFile <<
                "page = " << params.page << "coord_x" << params.coord_x << "coord_y" << params.coord_y <<
                "reason = " << params.reason << "location = " << params.location << "isTimestamp = " <<  params.isTimestamp;

    BEGIN_TRY_CATCH

        PTEID_EIDCard * card = NULL;
        getCardInstance(card);
        if (card == NULL) return;

        QVector<const char *> files_to_sign;

        for( int i = 0; i < params.loadedFileBatchPath.count(); i++ ){
            files_to_sign.push_back(strdup(getPlatformNativeString(params.loadedFileBatchPath[i])));
        }

        QByteArray tempOutputFile = getPlatformNativeString(params.outputFile);

        if (params.isTimestamp)
            card->SignXadesT(tempOutputFile.constData(), files_to_sign.data(), params.loadedFileBatchPath.count());
        else
            card->SignXades(tempOutputFile.constData(), files_to_sign.data(), params.loadedFileBatchPath.count());

        emit signalPdfSignSucess(SignMessageOK);

    END_TRY_CATCH
}

void GAPI::doSignXADES(QString loadedFilePath, QString outputFile, bool isTimestamp) {
    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    const char *files_to_sign[1];
    QByteArray tempLoadedFilePath = getPlatformNativeString(loadedFilePath);
    files_to_sign[0] = tempLoadedFilePath.constData();

    QByteArray tempOutputFile = getPlatformNativeString(outputFile);

    if (isTimestamp)
        card->SignXadesT(tempOutputFile.constData(), files_to_sign, 1);
    else
        card->SignXades(tempOutputFile.constData(), files_to_sign, 1);

    emit signalPdfSignSucess(SignMessageOK);

    END_TRY_CATCH
}

void GAPI::doSignPDF(SignParams &params) {

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    QString fullInputPath = params.loadedFilePath;
    PTEID_PDFSignature sig_handler(getPlatformNativeString(fullInputPath));

    if (params.isTimestamp)
        sig_handler.enableTimestamp();
    if (params.isSmallSignature)
        sig_handler.enableSmallSignatureFormat();

	if(useCustomSignature()) {
        const PTEID_ByteArray imageData(reinterpret_cast<const unsigned char *>(m_jpeg_scaled_data.data()), static_cast<unsigned long>(m_jpeg_scaled_data.size()));
        sig_handler.setCustomImage(imageData);
	}
    card->SignPDF(sig_handler, params.page, params.coord_x, params.coord_y,
                    params.location.toUtf8().data(), params.reason.toUtf8().data(),
                    getPlatformNativeString(params.outputFile));

    emit signalPdfSignSucess(SignMessageOK);

    END_TRY_CATCH
}

void GAPI::doSignBatchPDF(SignBatchParams &params) {

    qDebug() << "doSignBatchPDF! loadedFilePath = " << params.loadedFileBatchPath << " outputFile = " << params.outputFile <<
                "page = " << params.page << "coord_x" << params.coord_x << "coord_y" << params.coord_y <<
                "reason = " << params.reason << "location = " << params.location;
    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    PTEID_PDFSignature *sig_handler;
    sig_handler = new PTEID_PDFSignature();

    for( int i = 0; i < params.loadedFileBatchPath.count(); i++ ){
        qDebug() << params.loadedFileBatchPath[i];
        QString fullInputPath = params.loadedFileBatchPath[i];
        sig_handler->addToBatchSigning((char *)getPlatformNativeString(fullInputPath),
                                       params.page == 0 ? true : false);
    }

    if (params.isTimestamp)
        sig_handler->enableTimestamp();
    if (params.isSmallSignature)
        sig_handler->enableSmallSignatureFormat();

    if(useCustomSignature()) {
        const PTEID_ByteArray imageData(reinterpret_cast<const unsigned char *>(m_jpeg_scaled_data.data()), static_cast<unsigned long>(m_jpeg_scaled_data.size()));
        sig_handler->setCustomImage(imageData);
    }

    card->SignPDF(*sig_handler, params.page, params.coord_x, params.coord_y,
        params.location.toUtf8().data(), params.reason.toUtf8().data(),
                 getPlatformNativeString(params.outputFile));

    emit signalPdfSignSucess(SignMessageOK);

    END_TRY_CATCH
}

QPixmap PDFPreviewImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "PDFPreviewImageProvider received request for: " << id;
    qDebug() << "PDFPreviewImageProvider received request for: "
             << requestedSize.width() << " - " << requestedSize.height();
    QStringList strList = id.split("?");

    QString pdf_path = QUrl::fromPercentEncoding(strList.at(0).toUtf8());

    //URL param ?page=xx
    unsigned int page = (unsigned int) strList.at(1).split("=").at(1).toInt();

    if (m_doc == NULL || m_filePath != pdf_path) {

        Poppler::Document *newdoc = Poppler::Document::load(pdf_path);
        if (!newdoc) {
            qDebug() << "Failed to load PDF file";
            return QPixmap();
        }
        m_filePath = pdf_path;

        //TODO: Close a previous document
        //closeDocument();

        m_doc = newdoc;

        m_doc->setRenderHint(Poppler::Document::TextAntialiasing, true);
        m_doc->setRenderHint(Poppler::Document::Antialiasing, true);
        m_doc->setRenderBackend(Poppler::Document::RenderBackend::SplashBackend);
    }

    QPixmap pTest = renderPDFPage(page);
    qDebug() << "PDFPreviewImageProvider sending signal signalPdfSourceChanged width : "
             << pTest.width()<< " - height : " << pTest.height();
    emit signalPdfSourceChanged(pTest.width(),pTest.height());

    QFuture<QPixmap> future = QtConcurrent::run(this, &PDFPreviewImageProvider::renderPdf, page, requestedSize);

    QPixmap p = future.result();
    size->setHeight(p.height());
    size->setWidth(p.width());

    return p;
}

QPixmap PDFPreviewImageProvider::renderPdf(int page,const QSize &requestedSize) {

    QPixmap p = renderPDFPage(page).scaled(requestedSize.width(), requestedSize.height(),
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return p;
}

QSize PDFPreviewImageProvider::getPageSize(int page) {

    Poppler::Page *popplerPage = m_doc->page(page-1);

    return popplerPage->pageSize();
}

QPixmap PDFPreviewImageProvider::renderPDFPage(unsigned int page)
{
    // Document starts at page 0 in the poppler-qt5 API
    Poppler::Page *popplerPage = m_doc->page(page-1);

    //TODO: Test the resolution on Windows
    const double resX = 120.0;
    const double resY = 120.0;
    if (popplerPage == NULL)
    {
        qDebug() << "Failed to get page object: " << page;
        return QPixmap();
    }

    QImage image = popplerPage->renderToImage(resX, resY, -1, -1, -1, -1, Poppler::Page::Rotate0);
    //DEBUG
    //image.save("/tmp/pteid_preview.png");

    delete popplerPage;

    if (!image.isNull()) {
        return QPixmap::fromImage(image);
    } else {
        qDebug() << "Error rendering PDF page to image!";
        return QPixmap();
    }

}


void GAPI::startCardReading() {
    QFuture<void> future = QtConcurrent::run(this, &GAPI::connectToCard);

}

void GAPI::startWritingPersoNotes(QString text) {
    QFuture<void> future = QtConcurrent::run(this, &GAPI::setPersoDataFile, text);
}

int GAPI::getStringByteLength(QString text) {

    return text.toStdString().size() + 1; // '\0' should be considered as a byte
}

void GAPI::startGettingEntities() {
    QtConcurrent::run(this, &GAPI::getSCAPEntities);
}

void GAPI::startGettingCompanyAttributes() {
    QtConcurrent::run(this, &GAPI::getSCAPCompanyAttributes);
}

void GAPI::startGettingEntityAttributes(QList<int> entities_index) {
    QtConcurrent::run(this, &GAPI::getSCAPEntityAttributes, entities_index);
}

void GAPI::startPingSCAP() {

    // schedule the request
    httpRequestAborted = httpRequestSuccess = false;

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) {
        return;
    }

    const char * as_endpoint = "/CCC-REST/rest/scap/pingSCAP";

    // Get Endpoint from settings
    ScapSettings settings;
    std::string port = settings.getScapServerPort().toStdString();
    std::string sup_endpoint = std::string("https://")
            + settings.getScapServerHost().toStdString() + ":" + port + as_endpoint;

    url = sup_endpoint.c_str();

    eIDMW::PTEID_Config config_pacfile(eIDMW::PTEID_PARAM_PROXY_PACFILE);
    const char * pacfile_url = config_pacfile.getString();

    if (pacfile_url != NULL && strlen(pacfile_url) > 0)
    {
        m_pac_url = QString(pacfile_url);
    }

    eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
    eIDMW::PTEID_Config config_port(eIDMW::PTEID_PARAM_PROXY_PORT);
    eIDMW::PTEID_Config config_username(eIDMW::PTEID_PARAM_PROXY_USERNAME);
    eIDMW::PTEID_Config config_pwd(eIDMW::PTEID_PARAM_PROXY_PWD);

    std::string proxy_host = config.getString();
    std::string proxy_username = config_username.getString();
    std::string proxy_pwd = config_pwd.getString();
    long proxy_port = config_port.getLong();

    //10 second timeout
    int network_timeout = 10000;

    if (!proxy_host.empty() && proxy_port != 0)
    {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "PingSCAP: using manual proxy config");
        qDebug() << "C++: PingSCAP: using manual proxy config";
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(QString::fromStdString(proxy_host));
        proxy.setPort(proxy_port);

        if (!proxy_username.empty())
        {
            proxy.setUser(QString::fromStdString(proxy_username));
            proxy.setPassword(QString::fromStdString(proxy_pwd));
        }

        QNetworkProxy::setApplicationProxy(proxy);
    }
    else if (!m_pac_url.isEmpty())
    {
        std::string proxy_port_str;
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "PingSCAP: using system proxy config");
        qDebug() << "C++: PingSCAP: using system proxy config";
        PTEID_GetProxyFromPac(m_pac_url.toUtf8().constData(),
                              url.toString().toUtf8().constData(), &proxy_host, &proxy_port_str);
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(QString::fromStdString(proxy_host));
        proxy.setPort(atol(proxy_port_str.c_str()));
        QNetworkProxy::setApplicationProxy(proxy);
    }

    reply = qnam.get(QNetworkRequest(url));

    QTimer::singleShot(network_timeout, this, SLOT(cancelDownload()));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
}

void GAPI::cancelDownload()
{
    if(!httpRequestSuccess && !httpRequestAborted){
        qDebug() << "C++: signalSCAPPingFail";
        httpRequestAborted = true;
        httpRequestSuccess = false;
        emit signalSCAPPingFail();
        reply->deleteLater();
    }
}

void GAPI::httpFinished()
{
    qDebug() << "C++: httpFinished";
    if(!httpRequestSuccess && !httpRequestAborted){

        if (reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ) != 200) {
            qDebug() << "C++: reply error";
            httpRequestAborted = true;
            httpRequestSuccess = false;
            emit signalSCAPPingFail();
            QString strLog = QString("PingSCAP:: Http request failed to: ");
            strLog += reply->url().toString();
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", strLog.toStdString().c_str() );
        } else {
            qDebug() << "C++: signalSCAPPingSuccess";
            httpRequestAborted = false;
            httpRequestSuccess = true;
            emit signalSCAPPingSuccess();
        }
        reply->deleteLater();
    }
}

void GAPI::startReadingPersoNotes() {
    QFuture<void> future = QtConcurrent::run(this, &GAPI::getPersoDataFile);
}

void GAPI::startReadingAddress() {
    QtConcurrent::run(this, &GAPI::getAddressFile);
}

void GAPI::startLoadingAttributesFromCache(int isCompanies, bool isShortDescription) {
    QtConcurrent::run(this, &GAPI::getSCAPAttributesFromCache, isCompanies, isShortDescription);
}

void GAPI::startRemovingAttributesFromCache(int isCompanies) {
    QtConcurrent::run(this, &GAPI::removeSCAPAttributesFromCache, isCompanies);
}

void GAPI::startSigningSCAP(QString inputPDF, QString outputPDF, int page, double location_x,
                            double location_y, QString location, QString reason, int ltv,
                            QList<int> attribute_index) {

    SCAPSignParams signParams = {inputPDF, outputPDF, page, location_x, location_y,
                                 location, reason,ltv, attribute_index};

    QtConcurrent::run(this, &GAPI::doSignSCAP, signParams);
}

void GAPI::doSignSCAP(SCAPSignParams params) {

    std::vector<int> attrs;
    for (unsigned int i = 0; i!= params.attribute_index.size(); i++) {
        attrs.push_back(params.attribute_index.at(i));
    }

    scapServices.executeSCAPSignature(this, params.inputPDF, params.outputPDF, params.page,
                params.location_x, params.location_y, params.location, params.reason,
                                      params.ltv, attrs);
}

void GAPI::getSCAPEntities() {

    QList<QString> attributeSuppliers;
    std::vector<ns3__AttributeSupplierType *> entities = scapServices.getAttributeSuppliers();

    if(entities.size() == 0){
        emit signalSCAPDefinitionsServiceFail(ScapGenericError, false);
        return;
    }

    for (unsigned int i = 0; i!=entities.size(); i++)
        attributeSuppliers.append(QString::fromStdString(entities.at(i)->Name));
    
    emit signalSCAPEntitiesLoaded(attributeSuppliers);
}

std::vector<std::string> getChildAttributes(ns2__AttributesType *attributes, bool isShortDescription) {
    std::vector<std::string> childrensList;

    std::vector<ns5__SignatureType *> signatureAttributeList = attributes->SignedAttributes->ns3__SignatureAttribute;

    for (uint i = 0; i < signatureAttributeList.size(); i++) {
        ns5__SignatureType * signatureType = signatureAttributeList.at(i);
        if(signatureType->ns5__Object.size() > 0)
        {
            ns5__ObjectType * signatureObject = signatureType->ns5__Object.at(0);
            ns3__PersonalDataType * personalDataObject = signatureObject->union_ObjectType.ns3__Attribute->PersonalData;
            ns3__MainAttributeType * mainAttributeObject = signatureObject->union_ObjectType.ns3__Attribute->MainAttribute;

            std::string name = personalDataObject->Name;
            childrensList.push_back(name.c_str());

            std::string description = mainAttributeObject->Description->c_str();
            if (!isShortDescription) {

                QString subAttributes(" (");
                QString subAttributesValues;
                for(uint subAttributePos = 0; subAttributePos < mainAttributeObject->SubAttributeList->SubAttribute.size(); subAttributePos++){
                    ns3__SubAttributeType * subAttribute = mainAttributeObject->SubAttributeList->SubAttribute.at(subAttributePos);

                    QString subDescription(subAttribute->Description->c_str());
                    QString subValue(subAttribute->Value->c_str());
                    subAttributesValues.append(subDescription + ": " + subValue + ", ");
                }
                // Chop 2 to remove last 2 chars (', ')
                subAttributesValues.chop(2);
                subAttributes.append(subAttributesValues + ")");

                qDebug() << "Sub attributes : " << subAttributes;
                description += subAttributes.toStdString();
            }
            
            childrensList.push_back(description.c_str());
        }
    }
    return childrensList;
}

void GAPI::initScapAppId(){
    ScapSettings settings;
    if(settings.getAppID()==""){
        QString appIDstring;
        QString request_uuid = QUuid::createUuid().toString();
        appIDstring = request_uuid.midRef(1, request_uuid.size() - 2).toString();
        settings.setAppID(appIDstring);
        removeSCAPAttributesFromCache(0);
    }
}

void GAPI::getSCAPEntityAttributes(QList<int> entityIDs) {

    QList<QString> attribute_list;
    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) {
        emit signalEntityAttributesLoadedError();
        return;
    }

    initScapAppId();

    std::vector<int> supplier_ids;
    
    int supplier_id;
    foreach (supplier_id, entityIDs) {
        supplier_ids.push_back(supplier_id);
    }
    if (!prepareSCAPCache()){
        return;
    }
    std::vector<ns2__AttributesType *> attributes = scapServices.getAttributes(this, *card, supplier_ids);

    if (attributes.size() == 0) {
        return;
    }

    for(uint i = 0; i < attributes.size() ; i++) {

       std::string attrSupplier = attributes.at(i)->ATTRSupplier->Name;
       std::vector<std::string> childAttributes = getChildAttributes(attributes.at(i), false);

       if (childAttributes.size() == 0) {
        qDebug() << "Zero child attributes in AttributeResponseValues!";
        emit signalEntityAttributesLoadedError();
        return;   
       }

       for(uint j = 0; j < childAttributes.size() ; j=j+2) {
           attribute_list.append(QString::fromStdString(attrSupplier));
           attribute_list.append(QString::fromStdString(childAttributes.at(j)));
           attribute_list.append(QString::fromStdString(childAttributes.at(j+1)));
       }
    }

    emit signalEntityAttributesLoaded(attribute_list);
}


void GAPI::getSCAPCompanyAttributes() {

    PTEID_EIDCard * card = NULL;
    QList<QString> attribute_list;
    getCardInstance(card);
    if (card == NULL){
        emit signalCompanyAttributesLoadedError();
        return;
    }

    initScapAppId();

    std::vector<int> supplierIDs;
    if (!prepareSCAPCache()) {
        return;
    }
    std::vector<ns2__AttributesType *> attributes = scapServices.getAttributes(this, *card, supplierIDs);

    if (attributes.size() == 0)
    {
        return;
    }

    for(uint i = 0; i < attributes.size() ; i++)
    {
       //Skip malformed AttributeResponseValues element
       if (attributes.at(i)->ATTRSupplier == NULL) {
            continue;
       }
       std::string attrSupplier = attributes.at(i)->ATTRSupplier->Name;
       std::vector<std::string> childAttributes = getChildAttributes(attributes.at(i), false);

       PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Attribute from supplier: %s containing %d child attributes", attrSupplier.c_str(), childAttributes.size()) ;

       if (childAttributes.size() == 0) {
         qDebug() << "Zero child attributes in AttributeResponseValues!";
         emit signalCompanyAttributesLoadedError();
         return;
       }

       for(uint j = 0; j < childAttributes.size() ; j=j+2) {
          attribute_list.append(QString::fromStdString(attrSupplier));
          attribute_list.append(QString::fromStdString(childAttributes.at(j)));
          attribute_list.append(QString::fromStdString(childAttributes.at(j+1)));
       }
    }

    emit signalCompanyAttributesLoaded(attribute_list);
}


void GAPI::getSCAPAttributesFromCache(int queryType, bool isShortDescription) {

    PTEID_EIDCard * card = NULL;
    std::vector<ns2__AttributesType *> attributes;
    QList<QString> attribute_list;

    //Card is only required for loading entities and companies seperately...
    //This is a hack to support loading attributes without NIC in the context of CMD 
    if (queryType < 2) {
        getCardInstance(card);
        if (card == NULL)
            return;

        if (queryType == 0 )
            attributes = scapServices.loadAttributesFromCache(*card, false);
        else if (queryType == 1 )
            attributes = scapServices.loadAttributesFromCache(*card, true);    
    }
    else if (queryType == 2) {
        attributes = scapServices.reloadAttributesFromCache();
    }

    for(uint i = 0; i < attributes.size() ; i++) {
        //Skip malformed AttributeResponseValues element
       if (attributes.at(i)->ATTRSupplier == NULL) {
            continue;
       }
       std::string attrSupplier = attributes.at(i)->ATTRSupplier->Name;
       std::vector<std::string> childAttributes = getChildAttributes(attributes.at(i), isShortDescription);

       for(uint j = 0; j < childAttributes.size() ; j=j+2) {
          attribute_list.append(QString::fromStdString(attrSupplier));
          attribute_list.append(QString::fromStdString(childAttributes.at(j)));
          attribute_list.append(QString::fromStdString(childAttributes.at(j+1)));
       }
    }

    if (queryType == 0)
        emit signalEntityAttributesLoaded(attribute_list);
    else if (queryType == 1)
        emit signalCompanyAttributesLoaded(attribute_list);
    else if (queryType == 2)
        emit signalAttributesLoaded(attribute_list);
}

void GAPI::removeSCAPAttributesFromCache(int isCompanies) {

    qDebug() << "removeSCAPAttributesFromCache : " << isCompanies;

    ScapSettings settings;
    QString scapCacheDir = settings.getCacheDir() + "/scap_attributes/";
    QDir dir(scapCacheDir);
    bool has_read_permissions = true;

    // Delete SCAP secretkey to get a new one
    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card != NULL) {
        QString citizenNIC(card->getID().getCivilianIdNumber());
        settings.setSecretKey("",citizenNIC);
    }

#ifdef WIN32
    extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
    qt_ntfs_permission_lookup++; // turn ntfs checking (allows isReadable and isWritable)
#endif
    if(!dir.isReadable())
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "No read permissions: SCAP cache directory!");
        qDebug() << "C++: Cache folder does not have read permissions! ";
        has_read_permissions = false;
        emit signalCacheNotReadable(isCompanies);
    }
#ifdef WIN32
    qt_ntfs_permission_lookup--; // turn ntfs permissions lookup off for performance
#endif

    bool error_code = false;
    error_code = scapServices.removeAttributesFromCache();

    if (!has_read_permissions)
        return;

    if (error_code == true)
        emit signalRemoveSCAPAttributesSucess(isCompanies);
    else
        emit signalRemoveSCAPAttributesFail(isCompanies);
}

bool GAPI::prepareSCAPCache() {
    ScapSettings settings;
    QString s_scapCacheDir = settings.getCacheDir() + "/scap_attributes/";
    QFileInfo scapCacheDir(s_scapCacheDir);
    QDir scapCache(s_scapCacheDir);
    bool hasPermissions = true;
    // Tries to create if does not exist
    if (!scapCache.mkpath(s_scapCacheDir)) {	
        qDebug() << "couldn't create SCAP cache folder";
        emit signalCacheFolderNotCreated();
        hasPermissions = false;
    }
#ifdef WIN32
    extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
    qt_ntfs_permission_lookup++; // turn ntfs checking (allows isReadable and isWritable)
#endif
    if (!scapCacheDir.isWritable()) {
        qDebug() << "SCAP cache not writable";
        emit signalCacheNotWritable();
        hasPermissions = false;
    }
    if (!scapCacheDir.isReadable()) {
        qDebug() << "SCAP cache not readable";
        emit signalCacheNotReadable(0); // isCompanies parameter not used
        hasPermissions = false;
    }
#ifdef WIN32
    qt_ntfs_permission_lookup--; // turn ntfs permissions lookup off for performance
#endif
    return hasPermissions;
}

void GAPI::getCardInstance(PTEID_EIDCard * &new_card) {

    try
    { 
        unsigned long ReaderEndIdx = ReaderSet.readerCount();
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Card Reader count =  %ld", ReaderEndIdx);
        long ReaderIdx = 0;
        long CardIdx = 0;
        long tempReaderIndex = 0;

        if ( ReaderEndIdx == 0 ) {
            emit signalCardAccessError(NoReaderFound);
        } else {
            bool bCardPresent = false;
            PTEID_CardType lastFoundCardType = PTEID_CARDTYPE_UNKNOWN;

            // Count number of cards
            for (ReaderIdx = 0; ReaderIdx < ReaderEndIdx; ReaderIdx++)
            {
                PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(ReaderIdx);
                try
                {
                    if (readerContext.isCardPresent())
                    {
                        bCardPresent = true;
                        CardIdx++;
                        tempReaderIndex = ReaderIdx;
                    }
                }
                catch(PTEID_Exception &e){
                    unsigned long err = e.GetError();
                    PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui",
                              "Failed to check is Card Present 0x%08x\n", err);
                }
            }
            // Test if Card Reader was previously selected
            if(selectedReaderIndex != -1)
            {
                PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(selectedReaderIndex);
                PTEID_CardType CardType = readerContext.getCardType();
                lastFoundCardType = CardType;
                qDebug()<< "Card Reader was previously selected CardType:" << CardType;
                PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui",
                          "Card Reader was previously selected CardType: %d", CardType);
                switch (CardType)
                {
                case PTEID_CARDTYPE_IAS07:
                {
                    PTEID_EIDCard& Card = readerContext.getEIDCard();
                    new_card = &Card;
                    break;
                }
                    
                case PTEID_CARDTYPE_UNKNOWN:
                default:
                    break;
                }
            }
            else
            { //Card Reader was not previously selected
                if(CardIdx == 1)
                {
                    PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(tempReaderIndex);
                    PTEID_CardType CardType = readerContext.getCardType();
                    lastFoundCardType = CardType;
                    qDebug()<< "Card Reader was not previously selected CardType:" << CardType;
                    PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui",
                              "Card Reader was not previously selected CardType: %d", CardType);
                    switch (CardType)
                    {
                        case PTEID_CARDTYPE_IAS07:
                        {
                            PTEID_EIDCard& Card = readerContext.getEIDCard();
                            new_card = &Card;
                            break;
                        }
                            
                        case PTEID_CARDTYPE_UNKNOWN:
                        default:
                            break;
                    }
                }
                else if(CardIdx > 1) //Card Reader was not previously selected
                {
                    qDebug()<< "Card Reader was not previously selected. Ask user to select card";
                    PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Card Reader was not previously selected. "
                                                               "Ask user to select card");
                    emit signalReaderContext(); // Ask user to select card
                }
                else
                {
                    if (!bCardPresent)
                    {
                        emit signalCardAccessError(NoCardFound);
                    }
                    else if (CardIdx == 1 && lastFoundCardType == PTEID_CARDTYPE_UNKNOWN)
                    {
                        selectedReaderIndex = -1;
                        emit signalCardAccessError(CardUnknownError);
                    }
                    else if (CardIdx == 1 && lastFoundCardType != PTEID_CARDTYPE_UNKNOWN)
                    {
                        selectedReaderIndex = tempReaderIndex;
                    }
                }
            }
        }
    }
    catch (PTEID_ExParamRange e)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed with error code 0x%08x", e.GetError());
        emit signalCardAccessError(CardUnknownError);
    }
    catch (PTEID_ExNoCardPresent e)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed with error code 0x%08x", e.GetError());
        emit signalCardAccessError(NoCardFound);
    }
    catch (PTEID_ExCardChanged e)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed with error code 0x%08x", e.GetError());
        emit signalCardAccessError(CardUnknownError);
    }
    catch (PTEID_ExBadTransaction& e)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed with error code 0x%08x", e.GetError());
        emit signalCardAccessError(CardUnknownError);
    }
    catch (PTEID_ExCertNoRoot& e)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed with error code 0x%08x", e.GetError());
        emit signalCardAccessError(CardUnknownError);
    }
    catch (PTEID_Exception e)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed with error code 0x%08x", e.GetError());
        emit signalCardAccessError(CardUnknownError);
    }
}

void GAPI::setReaderByUser(unsigned long setReaderIndex){

    qDebug() << "AppController GAPI::setReader!" << setReaderIndex;

    if (setReaderIndex >= ReaderSet.readerCount()) {
        setReaderIndex = ReaderSet.readerCount() - 1;
    }
    selectedReaderIndex = setReaderIndex;
}

QVariantList GAPI::getRetReaderList()
{
    qDebug() << "AppController GAPI::getRetReaderList!" ;

    QVariantList list;

    try {
        const char* const* ReaderList = ReaderSet.readerList();

        for (unsigned long Idx=0; Idx < ReaderSet.readerCount(); Idx++)
        {
            const char* readerName = ReaderList[Idx];
            list << readerName;
        }
        if ( selectedReaderIndex >= 0 ) {
            if ((unsigned long)selectedReaderIndex >= ReaderSet.readerCount()) {
                selectedReaderIndex = ReaderSet.readerCount() - 1;
            }
            emit signalSetReaderComboIndex(selectedReaderIndex);
        }


    }catch (PTEID_Exception& e) {
        qDebug() << "Error getRetReaderList!" ;
    }

    return list;
}

int GAPI::getReaderIndex(void)
{
    qDebug() << "AppController GAPI::geReaderIndex!" << selectedReaderIndex;
    if ( selectedReaderIndex >= 0 ) {

       return selectedReaderIndex;
    }
    return 0;
}

void GAPI::connectToCard() {

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    card->doSODCheck(true); //Enable SOD checking
    PTEID_EId &eid_file = card->getID();

    qDebug() << "C++: loading Card Data";

    QMap<IDInfoKey, QString> cardData;

    cardData[Surname] = QString::fromUtf8(eid_file.getSurname());
    cardData[Givenname] = QString::fromUtf8(eid_file.getGivenName());
    cardData[Sex] =       QString::fromUtf8(eid_file.getGender());
    cardData[Height] = QString::fromUtf8(eid_file.getHeight());
    cardData[Country] = QString::fromUtf8(eid_file.getCountry());
    cardData[Birthdate] = QString::fromUtf8(eid_file.getDateOfBirth());
    cardData[Father] = QString::fromUtf8(eid_file.getGivenNameFather()) + " " +
            QString::fromUtf8(eid_file.getSurnameFather());
    cardData[Mother] = QString::fromUtf8(eid_file.getGivenNameMother()) + " " +
            QString::fromUtf8(eid_file.getSurnameMother());
    cardData[AccidentalIndications]  = QString::fromUtf8(eid_file.getAccidentalIndications());
    cardData[Documenttype] = QString::fromUtf8(eid_file.getDocumentType());
    cardData[Documentnum] = QString::fromUtf8(eid_file.getDocumentNumber());
    cardData[Documentversion] = QString::fromUtf8(eid_file.getDocumentVersion());
    cardData[Nationality] = QString::fromUtf8(eid_file.getNationality());
    cardData[Validityenddate] = QString::fromUtf8(eid_file.getValidityEndDate());
    cardData[Validitybegindate] = QString::fromUtf8(eid_file.getValidityBeginDate());
    cardData[PlaceOfRequest] = QString::fromUtf8(eid_file.getLocalofRequest());
    cardData[IssuingEntity] = QString::fromUtf8(eid_file.getIssuingEntity());
    cardData[NISS] = QString::fromUtf8(eid_file.getSocialSecurityNumber());
    cardData[NSNS] = QString::fromUtf8(eid_file.getHealthNumber());
    cardData[NIF]  = QString::fromUtf8(eid_file.getTaxNo());

    //Load photo into a QPixmap
    PTEID_ByteArray& photo = eid_file.getPhotoObj().getphoto();

    QPixmap image_photo;
    image_photo.loadFromData(photo.GetBytes(), photo.Size(), "PNG");

    image_provider->setPixmap(image_photo);

    //All data loaded: we can emit the signal to QML
    setDataCardIdentify(cardData);

    END_TRY_CATCH
}

//****************************************************
// Callback function used by the Readercontext to notify insertion/removal of a card
// The callback comes at:
// - startup
// - insertion of a card
// - removal of a card
// - add/remove of a cardreader
// When a card is inserted we post a signal to the GUI telling that
// a new card is inserted.
//****************************************************
void cardEventCallback(long lRet, unsigned long ulState, CallBackData* pCallBackData)
{
    g_runningCallback++;

    try
    {
        PTEID_ReaderContext& readerContext = ReaderSet.getReaderByName(pCallBackData->getReaderName().toLatin1());

        unsigned int cardState = (unsigned int)ulState & 0x0000FFFF;
        unsigned int eventCounter = ((unsigned int)ulState) >> 16;

        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "Card Event received for reader %s: cardState: %u Event Counter: %u",
        pCallBackData->getReaderName().toUtf8().constData(), cardState, eventCounter);

        //------------------------------------
        // is card removed from the reader?
        //------------------------------------
        if (!readerContext.isCardPresent())
        {

            if (pCallBackData->getMainWnd()->m_Settings.getRemoveCert())
            {
                PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "Will try to RemoveCertificates...");
                bool bImported = pCallBackData->getMainWnd()->m_Certificates.RemoveCertificates(pCallBackData->getReaderName());

                if(!bImported) {
                    qDebug() << "RemoveCertificates fail";

                    PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "RemoveCertificates failed!");
                    emit pCallBackData->getMainWnd()->signalRemoveCertificatesFail();
                }
            }

            //------------------------------------
            // send an event to the main app to show the popup message
            //------------------------------------
            if(pCallBackData->getMainWnd()->returnReaderSelected() != -1){
                pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_CHANGED);
            }else{
                pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_REMOVED);
            }
            pCallBackData->getMainWnd()->setAddressLoaded(false);
            pCallBackData->getMainWnd()->resetReaderSelected();

            g_runningCallback--;
            return;
        }
        //------------------------------------
        // is card inserted ?
        //------------------------------------
        else if (readerContext.isCardChanged(pCallBackData->m_cardID))
        {
            //------------------------------------
            // send an event to the main app to show the popup message
            //------------------------------------
            pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_CHANGED);
            pCallBackData->getMainWnd()->setAddressLoaded(false);
            pCallBackData->getMainWnd()->resetReaderSelected();

            //------------------------------------
            // register certificates when needed
            //------------------------------------
            if (pCallBackData->getMainWnd()->m_Settings.getRegCert())
            {
                PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "Will try to ImportCertificates...");
                bool bImported = pCallBackData->getMainWnd()->m_Certificates.ImportCertificates(pCallBackData->getReaderName());

                if(!bImported) {
                    PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "ImportCertificates failed!");
                    emit pCallBackData->getMainWnd()->signalImportCertificatesFail();
                }
            }
        }
        else
        {
            PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "Event ignored");
        }
    }
    catch (...)
    {
        emit pCallBackData->getMainWnd()->signalCardAccessError(GAPI::CardUnknownError);
        // we catch ALL exceptions. This is because otherwise the thread throwing the exception stops
    }
    g_runningCallback--;
}

//*****************************************************
// update the readerlist. In case a reader is added to the machine
// at runtime.
//*****************************************************
void GAPI::updateReaderList( void )
{
    //----------------------------------------------------
    // check if the number of readers is changed
    //----------------------------------------------------
    try
    {
        if (ReaderSet.isReadersChanged())
        {
            stopAllEventCallbacks();
            ReaderSet.releaseReaders();

                    if ( 0 == ReaderSet.readerCount() ){
                        emit signalCardAccessError(NoReaderFound);
                      }
        }
        if ( 0 == m_callBackHandles.size() )
        {
            setEventCallbacks();
        }
    }
    catch(...)
    {
        stopAllEventCallbacks();
        ReaderSet.releaseReaders();
    }
}
void GAPI::setEventCallbacks( void )
{
    //----------------------------------------
    // for all the readers, create a callback such we can know
    // afterwards, which reader called us
    //----------------------------------------

    try
    {
        size_t maxcount=ReaderSet.readerCount(true);
        for (size_t Ix=0; Ix<maxcount; Ix++)
        {
            void (*fCallback)(long lRet, unsigned long ulState, void* pCBData);

            const char*			 readerName		= ReaderSet.getReaderName(Ix);
            PTEID_ReaderContext& readerContext  = ReaderSet.getReaderByNum(Ix);
            CallBackData*		 pCBData		= new CallBackData(readerName, this);

            fCallback = (void (*)(long,unsigned long,void *))&cardEventCallback;

            m_callBackHandles[readerName] = readerContext.SetEventCallback(fCallback, pCBData);
            m_callBackData[readerName]	  = pCBData;
        }
    }
    catch(PTEID_Exception& e)
    {
        emit signalCardChanged(ET_UNKNOWN);
    }
}

//*****************************************************
// stop the event callbacks and delete the corresponding callback data
// objects.
//*****************************************************
void GAPI::stopAllEventCallbacks( void )
{

    for (tCallBackHandles::iterator it = m_callBackHandles.begin()
            ; it != m_callBackHandles.end()
            ; it++
    )
    {
        PTEID_ReaderContext& readerContext = ReaderSet.getReaderByName(it.key().toLatin1());
        unsigned long handle = it.value();
        readerContext.StopEventCallback(handle);
    }
    m_callBackHandles.clear();
    cleanupCallbackData();
}

//*****************************************************
// cleanup the callback data
//*****************************************************
void GAPI::cleanupCallbackData(void)
{

    while(g_runningCallback)
    {
#ifdef WIN32
        ::Sleep(100);
#else
        ::usleep(100000);
#endif
    }

    g_cleaningCallback = true;

    for (tCallBackData::iterator it = m_callBackData.begin()
            ; it != m_callBackData.end()
            ; it++
    )
    {
        CallBackData* pCallbackData = it.value();
        delete pCallbackData;
    }
    m_callBackData.clear();
    g_cleaningCallback = false;
}

void GAPI::buildTree(PTEID_Certificate &cert, bool &bEx, QVariantMap &certificatesMap)
{
    QVariantMap certificatesMapChildren;
    static int level = 0;
    static int status = PTEID_CERTIF_STATUS_UNKNOWN;

    if (cert.isRoot())
    {
        certificatesMapChildren.insert("OwnerName", cert.getOwnerName());
        certificatesMapChildren.insert("IssuerName", cert.getIssuerName());
        certificatesMapChildren.insert("ValidityBegin", cert.getValidityBegin());
        certificatesMapChildren.insert("ValidityEnd", cert.getValidityEnd());
        certificatesMapChildren.insert("KeyLength", QString::number(cert.getKeyLength()));
        if(status != PTEID_CERTIF_STATUS_CONNECT
                && status != PTEID_CERTIF_STATUS_ERROR){
            status = cert.getStatus();
        }
        certificatesMapChildren.insert("Status", status);

        certificatesMap.insert("level" + QString::number(level),certificatesMapChildren);
        certificatesMap.insert("levelCount",level+1);
        level = 0;
        status = PTEID_CERTIF_STATUS_UNKNOWN;
    }
    else
    {
        certificatesMapChildren.insert("OwnerName", cert.getOwnerName());
        certificatesMapChildren.insert("IssuerName", cert.getIssuerName());
        certificatesMapChildren.insert("ValidityBegin", cert.getValidityBegin());
        certificatesMapChildren.insert("ValidityEnd", cert.getValidityEnd());
        certificatesMapChildren.insert("KeyLength", QString::number(cert.getKeyLength()));

        if(status != PTEID_CERTIF_STATUS_CONNECT
                && status != PTEID_CERTIF_STATUS_ERROR){
            status = cert.getStatus();
        }
        certificatesMapChildren.insert("Status", status);

        if(certificatesMap.contains("level" + QString::number(level))) {
            certificatesMap.insert("levelB" + QString::number(level),certificatesMapChildren);
        }
        else {
            certificatesMap.insert("level" + QString::number(level),certificatesMapChildren);
        }

        level++;

        try {
            buildTree(cert.getIssuer(), bEx, certificatesMap);
        } catch (PTEID_ExCertNoIssuer &ex) {
            bEx = true;
        }
    }
}

void GAPI::startfillCertificateList( void ) {
    QtConcurrent::run(this, &GAPI::fillCertificateList);
}

void GAPI::startGetCardActivation( void ) {

    QtConcurrent::run(this, &GAPI::getCertificateAuthStatus);
}

void GAPI::getCertificateAuthStatus ( void )
{
    qDebug() << "getCertificateAuthStatus";

    QString returnString = getCardActivation();

    emit signalShowCardActivation(returnString);
}

void GAPI::fillCertificateList ( void )
{
    bool noIssuer = false;
    QVariantMap certificatesMap;

    qDebug() << "fillCertificateList";

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    PTEID_Certificates&	 certificates	= card->getCertificates();

    certificatesMap.clear();
    buildTree(certificates.getCert(PTEID_Certificate::CITIZEN_AUTH), noIssuer,certificatesMap);

    buildTree(certificates.getCert(PTEID_Certificate::CITIZEN_SIGN), noIssuer,certificatesMap);
    if (noIssuer)
    {
        qDebug() << "fillCertificateList failed because certificate chain couldn't be completed!";
    }

    emit signalCertificatesChanged(certificatesMap);
    END_TRY_CATCH
}

QString GAPI::getCachePath(void){
    return m_Settings.getPteidCachedir();
}

bool GAPI::customSignImageExist(void){
    QString path = m_Settings.getPteidCachedir()+"/CustomSignPicture.png";
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists()) {
        return true;
    } else {
        return false;
    }
}

void GAPI::customSignRemove(void){
    QString path = m_Settings.getPteidCachedir()+"/CustomSignPicture.png";
    QFile file (path);
    file.remove();
}

bool GAPI::useCustomSignature(void){
    // Detect Custom Signature Image
    m_custom_image = QImage(m_Settings.getPteidCachedir()+"/CustomSignPicture.png");
    if (m_Settings.getUseCustomSignature() && !m_custom_image.isNull())
    {
        qDebug() << "Using Custom Picture to CC sign";
        QBuffer buffer(&m_jpeg_scaled_data);
        buffer.open(QIODevice::WriteOnly);
        //Save the generated image as high-quality JPEG data
        QImage generated_img (m_Settings.getPteidCachedir()+"/CustomSignPicture.png");
        QImage final_img(generated_img.size(),QImage::Format_RGB32);
        // Fill background with white
        final_img.fill(QColor(Qt::white).rgb());
        QPainter painter(&final_img);
        painter.drawImage(0, 0, generated_img);
        final_img.save(&buffer, "JPG", 100);
        return true;
    }else{
        qDebug() << "Using default Picture to CC sign";
        return false;
    }
}
void GAPI::setUseCustomSignature (bool UseCustomSignature){

    m_Settings.setUseCustomSignature(UseCustomSignature);
}
bool GAPI::getUseCustomSignature(void){
    return m_Settings.getUseCustomSignature();
}
void GAPI::setRegCertValue (bool bRegCert){

    m_Settings.setRegCert(bRegCert);
}
void GAPI::setRemoveCertValue (bool bRemoveCert){

    m_Settings.setRemoveCert(bRemoveCert);
}
bool GAPI::getRegCertValue (void){

    return m_Settings.getRegCert();
}
bool GAPI::getRemoveCertValue (void){

    return m_Settings.getRemoveCert();
}

void GAPI::quitApplication(void) {
    try
    {
        if (m_Settings.getRemoveCert())
        {
            for (unsigned long readerCount=0;readerCount<ReaderSet.readerCount();readerCount++)
            {
                QString readerName = ReaderSet.getReaderName(readerCount);
                m_Certificates.RemoveCertificates(readerName);
            }
        }

        //-------------------------------------------------------------------
        // we must release all the certificate contexts before releasing the SDK.
        // After Release, no more calls should be done to the SDK and as such
        // noting should be done in the dtor
        //-------------------------------------------------------------------
        forgetAllCertificates();
        stopAllEventCallbacks();

    }
    catch (...) {}
    qApp->quit();
}

//*****************************************************
// forget all the certificates we kept for all readers
//*****************************************************
void GAPI::forgetAllCertificates( void )
{
#ifdef WIN32
    bool bRefresh = true;;
    for (unsigned long readerIdx=0; readerIdx<ReaderSet.readerCount(bRefresh); readerIdx++)
    {
        const char* readerName = ReaderSet.getReaderByNum(readerIdx).getName();
        forgetCertificates(readerName);
    }
#endif
}

//*****************************************************
// forget all the certificates we kept for a specific reader
//*****************************************************
void GAPI::forgetCertificates(QString const& reader)
{
    char readerName[256];
    readerName[0] = 0;
    if (reader.length()>0)
    {
        strcpy(readerName, reader.toUtf8().data());
    }
#ifdef WIN32
    QVector<PCCERT_CONTEXT> readerCertContext = m_Certificates.m_certContexts[readerName];
    while (0 < readerCertContext.size())
    {
        PCCERT_CONTEXT pContext = readerCertContext[readerCertContext.size() - 1];
        CertFreeCertificateContext(pContext);
        readerCertContext.erase(readerCertContext.end() - 1);
    }
#endif
}
