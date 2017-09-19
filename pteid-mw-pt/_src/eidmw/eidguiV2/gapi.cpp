#include "gapi.h"
#include <QString>
#include <QDate>
#include <QtConcurrent>
#include <cstdio>
#include <QQuickImageProvider>
#include <QPrinter>
#include "qpainter.h"


#include "CMDSignature.h"
#include "cmdErrors.h"

//SCAP
#include "scapsignature.h"
#include "SCAPServices/SCAPH.h"
//#include "ASService/ASServiceH.h"   
//#include "PDFSignature/envStub.h"


using namespace eIDMW;

#define TRIES_LEFT_ERROR    1000

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

}

void GAPI::initTranslation() {

    GenPur::UI_LANGUAGE CurrLng   = m_Settings.getGuiLanguageCode();
    if (LoadTranslationFile(CurrLng)==false){
        emit signalLanguageChangedError();
    }
}

bool GAPI::LoadTranslationFile(GenPur::UI_LANGUAGE NewLanguage)
{

    QString strTranslationFile;
    strTranslationFile = QString("eidmw_") + GenPur::getLanguage(NewLanguage);

    qDebug() << "C++: GAPI LoadTranslationFile" << strTranslationFile;

    if (!m_translator.load(strTranslationFile))
    {
        // this should not happen, since we've built the menu with the translation filenames
        qDebug() << "C++: GAPI LoadTranslationFile Error";
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

void GAPI::testUpdateCardIdentify(int timerValue) {

    qDebug() << "C++: testUpdateCardIdentify";

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this,
            SLOT(triggerTestUpdateCardIdentify()));
    timer->start(timerValue);
}

void GAPI::triggerTestUpdateCardIdentify() {

    static char i = 0;

    m_data[Givenname] = "Joana Ovilia " + QString::number(i++);

    setDataCardIdentify(m_data);
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
    fprintf(stderr, "SOD exception! Error code (see strings in eidErrors.h): %08lx\n", e.GetError()); \
    emit signalCardAccessError(SodCardReadError); \
    } \
    else \
{ \
    fprintf(stderr, "Generic eidlib exception! Error code (see strings in eidErrors.h): %08lx\n", e.GetError()); \
    emit signalCardAccessError(CardUnknownError); \
    } \
    }

void GAPI::getAddressFile() {
    qDebug() << "C++: getAddressFile()";

    BEGIN_TRY_CATCH;

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    PTEID_Address &addressFile = card->getAddr();

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

    emit signalAddressLoaded();

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
            unsigned long ulSize = 1000;
            unsigned char *pucData = (unsigned char *)calloc(ulSize, sizeof(unsigned char));

            const PTEID_ByteArray oData( (const unsigned char *)pucData, ulSize);
            Card.writePersonalNotes(oData);
            free(pucData);
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

    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AddressChange op finished with error code 0x%08x", code);

    switch(code)
    {
    case 0:
        error_msg = tr("STR_CHANGE_ADDRESS_SUCESS");
        break;
        //The error code for connection error is common between SAM and OTP
    case EIDMW_OTP_CONNECTION_ERROR:
        error_msg = tr("STR_CONNECTION ERROR") + "\n\n" +
                tr("STR_VERIFY_INTERNET");
        break;

    case 1121:
    case 1122:
        error_msg = tr("STR_CHANGE_ADDRESS_ERROR") + "\n\n" + tr("STR_CHANGE_ADDRESS_CHECK_PROCESS_NUMBER");
        sam_error_code = code;
        break;
    case EIDMW_SAM_UNCONFIRMED_CHANGE:
        error_msg = tr("STR_CHANGE_ADDRESS_ERROR_INCOMPLETE") + "\n\n" + tr("STR_CHANGE_ADDRESS_NOT_CONFIRMED");
        break;
    case EIDMW_SSL_PROTOCOL_ERROR:
        error_msg = tr("STR_CHANGE_ADDRESS_ERROR") + "\n\n" + tr("STR_CHANGE_ADDRESS_CHECK_AUTHENTICATION_CERTIFICATE");
        break;
    default:
        //Make sure we only show the user error codes from the SAM service and not some weird pteid exception error code
        if (code > 1100 && code < 3500)
            sam_error_code = code;
        error_msg = tr("STR_CHANGE_ADDRESS_ERROR");
        break;
    }

    if (sam_error_code != 0)
    {
        error_msg += "\n\n" + tr("STR_ERROR_CODE = ") + QString::number(sam_error_code);
    }

    if (code != 0)
        error_msg += "\n\n" + support_string;

    qDebug() << error_msg;
    signalUpdateProgressStatus(error_msg);

    //TO-DO: Reload card information in case of successful address change
}

void GAPI::showSignCMDDialog(long code)
{
    QString error_msg;
    long sam_error_code = 0;
    QString support_string = tr("STR_CMD_ERROR_MSG");

    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "CMD signature op finished with error code 0x%08x", code);

    switch(code)
    {
    case 0:
        error_msg = tr("STR_CMD_SUCESS");
        break;
        //The error code for connection error is common between SAM and OTP
    default:
        //Make sure we only show the user error codes from the SAM service and not some weird pteid exception error code
        sam_error_code = code;
        error_msg = tr("STR_CMD_LOGIN_ERROR");
        break;
    }

    if (sam_error_code != 0)
    {
        error_msg += "\n\n" + tr("STR_ERROR_CODE = ") + QString::number(sam_error_code);
    }

    if (code != 0)
        error_msg += "\n\n" + support_string;

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

void GAPI::doOpenSignCMD(CMDSignature *cmd_signature, CmdSignParams &params)
{
    qDebug() << "doOpenSignCMD! MobileNumber = " << params.mobileNumber << " secret_code = " << params.secret_code <<
                " loadedFilePath = " << params.loadedFilePath << " outputFile = " << params.outputFile <<
                "page = " << params.page << "coord_x" << params.coord_x << "coord_y" << params.coord_y <<
                "reason = " << params.reason << "location = " << params.location;

    int ret = 0;

    try {
        signalUpdateProgressBar(25);
        ret = cmd_signature->signOpen( params.mobileNumber.toStdString(), params.secret_code.toStdString(),
                                      params.page,
                                      params.coord_x, params.coord_y,
                                      params.location.toUtf8().data(), params.reason.toUtf8().data(),
                                      params.outputFile.toUtf8().data());

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
        ret = cmd_signature->signClose( local_sms_token );
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

    cmd_pdfSignature->setFileSigning(fullInputPath.toUtf8().data());


    if (params.isTimestamp > 0)
        cmd_pdfSignature->enableTimestamp();
    if (params.isSmallSignature > 0)
        cmd_pdfSignature->enableSmallSignatureFormat();

    cmd_signature->set_pdf_handler(cmd_pdfSignature);
    QtConcurrent::run(this, &GAPI::doOpenSignCMD, cmd_signature, params);
}

void GAPI::signCloseCMD(QString sms_token)
{
    qDebug() << "signCloseCMD! sms_token = " + sms_token;

    signalUpdateProgressStatus(tr("STR_CMD_SENDING_CODE"));

    connect(this, SIGNAL(signCMDFinished(long)),
            this, SLOT(showSignCMDDialog(long)), Qt::UniqueConnection);

    QtConcurrent::run(this, &GAPI::doCloseSignCMD, cmd_signature, sms_token);
}

QString GAPI::getCardActivation() {
    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return "Error getting Card Activation";

    PTEID_EId &eid_file = card->getID();

    if (isExpiredDate(eid_file.getValidityEndDate())) {
        return QString(tr("STR_CARD_EXPIRED"));
    }
    else if (!card->isActive()) {
        return QString(tr("STR_CARD_NOT_ACTIVE"));
    }
    else {
        return QString(tr("STR_CARD_HAS_BEEN_ACTIVATED"));
    }

    END_TRY_CATCH

            return QString();
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

void GAPI::startPrintPDF(QString outputFile, double isBasicInfo,double isAddicionalInfo,
                         double isAddress,double isNotes,double isSign) {

    PrintParams params = {outputFile, isBasicInfo, isAddicionalInfo, isAddress, isNotes, isSign};
    QtConcurrent::run(this, &GAPI::doPrintPDF, params);
}

void GAPI::startPrint(QString outputFile, double isBasicInfo,double isAddicionalInfo,
                         double isAddress,double isNotes,double isSign) {

    PrintParams params = {outputFile, isBasicInfo, isAddicionalInfo, isAddress, isNotes, isSign};

    QPrinter printer;
    bool res = false;

    printer.setDocName("CartaoCidadao_signed.pdf");
    QPrintDialog *dlg = new QPrintDialog(&printer);
    if(dlg->exec() == QDialog::Accepted) {
         qDebug() << "QPrintDialog! Accepted";
         BEGIN_TRY_CATCH;
         // Print PDF not Signed
         res = drawpdf(printer, params);
         if (res) {
             emit signalPdfPrintSucess();
         }else{
             emit signalPdfPrintFail();
         }
         END_TRY_CATCH
    }
}

bool GAPI::doSignPrintPDF(QString &file_to_sign, QString &outputsign) {

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;

    getCardInstance(card);
    if (card == NULL) return false;

    PTEID_PDFSignature sig_handler(file_to_sign.toUtf8().data());

    card->SignPDF(sig_handler, 0, 0, false, "", "", outputsign.toUtf8().data());

    return true;

    END_TRY_CATCH

            return false;
}

void GAPI::doPrintPDF(PrintParams &params) {

    qDebug() << "doPrintPDF! outputFile = " << params.outputFile <<
                "isBasicInfo = " << params.isBasicInfo << "isAddicionalInfo" << params.isAddicionalInfo << "isAddress"
             << params.isAddress << "isNotes = " << params.isNotes << "isSign = " << params.isSign;

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

void drawSingleField(QPainter &painter, double pos_x, double pos_y, QString name, QString value, bool single_column=false)
{
    int line_length = single_column ? 300 : 180;

    painter.setPen(blue_pen);
    painter.drawText(QPointF(pos_x, pos_y), name);
    pos_y += 7;

    painter.drawLine(QPointF(pos_x, pos_y), QPointF(pos_x+line_length, pos_y));
    pos_y += 15;
    painter.setPen(black_pen);
    painter.drawText(QPointF(pos_x, pos_y), value);
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

QPixmap loadHeader()
{
    return QPixmap(":/images/pdf_document_header.png");
}

bool GAPI::drawpdf(QPrinter &printer, PrintParams params)
{
    qDebug() << "drawpdf! outputFile = " << params.outputFile <<
                "isBasicInfo = " << params.isBasicInfo << "isAddicionalInfo" << params.isAddicionalInfo << "isAddress"
             << params.isAddress << "isNotes = " << params.isNotes << "isSign = " << params.isSign;

    double pos_x = 0, pos_y = 0;
    bool res = false;
    int sections_to_print = 0;

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

    pos_y += header.height() + 15;
    black_pen = painter.pen();

    blue_pen.setColor(QColor(78, 138, 190));
    painter.setPen(blue_pen);

    int line_length = 487;
    //Horizontal separator below the CC logo
    painter.drawLine(QPointF(pos_x, pos_y), QPointF(pos_x+line_length, pos_y));
    pos_y += 25;

    //Change text color
    blue_pen = painter.pen();

    const int COLUMN_WIDTH = 220;
    const int LINE_HEIGHT = 45;

    //    din_font.setBold(true);
    painter.setFont(din_font);

    painter.drawText(QPointF(pos_x, pos_y), tr("STR_PERSONAL_DATA"));

    pos_y += 15;
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

    if (params.isBasicInfo)
    {

        drawSectionHeader(painter, pos_x, pos_y, tr("STR_BASIC_INFORMATION"));
        sections_to_print++;

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

        drawSingleField(painter, pos_x, pos_y, tr("STR_GIVEN_NAME"), QString::fromUtf8(eid_file.getGivenName()), true);

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_SURNAME"), QString::fromUtf8(eid_file.getSurname()), true);

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_GENDER"), QString::fromUtf8(eid_file.getGender()));
        drawSingleField(painter, pos_x + COLUMN_WIDTH, pos_y, tr("STR_HEIGHT"), QString::fromUtf8(eid_file.getHeight()));
        drawSingleField(painter, pos_x + COLUMN_WIDTH*2, pos_y, tr("STR_DATE_OF_BIRTH"),
                        QString::fromUtf8(eid_file.getDateOfBirth()));

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_DOCUMENT_NUMBER"), QString::fromUtf8(eid_file.getDocumentNumber()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("STR_VALIDITY_DATE"),
                        QString::fromUtf8(eid_file.getValidityEndDate()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_COUNTRY"), QString::fromUtf8(eid_file.getCountry()));

        pos_y += LINE_HEIGHT;
        drawSingleField(painter, pos_x, pos_y, tr("STR_FATHER"), QString::fromUtf8(eid_file.getGivenNameFather()) + " " +
                        QString::fromUtf8(eid_file.getSurnameFather()), true);
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_MOTHER"), QString::fromUtf8(eid_file.getGivenNameMother()) + " " +
                        QString::fromUtf8(eid_file.getSurnameMother()), true);
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_NOTES"),
                        QString::fromUtf8(eid_file.getAccidentalIndications()), true);

        pos_y += 50;
    }

    if (params.isAddicionalInfo)
    {
        drawSectionHeader(painter, pos_x, pos_y, tr("STR_ADDITIONAL_INFORMATION"));
        sections_to_print++;
        pos_y += 50;

        drawSingleField(painter, pos_x, pos_y, tr("STR_VAT_NUM"), QString::fromUtf8(eid_file.getTaxNo()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("STR_SOCIAL_SECURITY_NUM"),
                        QString::fromUtf8(eid_file.getSocialSecurityNumber()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_NATIONAL_HEALTH_NUM"),
                        QString::fromUtf8(eid_file.getHealthNumber()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_CARD_VERSION"), QString::fromUtf8(eid_file.getDocumentVersion()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("STR_DELIVERY_DATE"),
                        QString::fromUtf8(eid_file.getValidityBeginDate()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_DELIVERY_ENTITY"),
                        QString::fromUtf8(eid_file.getIssuingEntity()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_CARD_STATE"), getCardActivation());
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_DOCUMENT_TYPE"),
                        QString::fromUtf8(eid_file.getDocumentType()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_DELIVERY_LOCATION"),
                        QString::fromUtf8(eid_file.getLocalofRequest()));

        pos_y += 50;
    }

    if (params.isAddress)
    {
        drawSectionHeader(painter, pos_x, pos_y, tr("STR_ADDRESS"));
        sections_to_print++;
        pos_y += 50;

        PTEID_Address &addressFile = card->getAddr();

        // TODO: Foreign address

        drawSingleField(painter, pos_x, pos_y, tr("STR_DISTRICT"),
                        QString::fromUtf8(addressFile.getDistrict()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("STR_MUNICIPALY"),
                        QString::fromUtf8(addressFile.getMunicipality()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_CICIL_PARISH"),
                        QString::fromUtf8(addressFile.getCivilParish()));

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_AB_STREET_TYPE"),
                        QString::fromUtf8(addressFile.getAbbrStreetType()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("STR_STREET_TYPE"),
                        QString::fromUtf8(addressFile.getStreetType()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_STREET_NAME"),
                        QString::fromUtf8(addressFile.getStreetName()));

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_AB_BUILDING_TYPE"),
                        QString::fromUtf8(addressFile.getAbbrBuildingType()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("STR_BUILDING_TYPE"),
                        QString::fromUtf8(addressFile.getBuildingType()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_HOUSE_BUILDING_NUM"),
                        QString::fromUtf8(addressFile.getDoorNo()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_FLOOR"),
                        QString::fromUtf8(addressFile.getFloor()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("STR_SIDE"),
                        QString::fromUtf8(addressFile.getSide()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_PLACE"),
                        QString::fromUtf8(addressFile.getPlace()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_ZIP_CODE_4"),
                        QString::fromUtf8(addressFile.getZip4()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("STR_ZIP_CODE_3"),
                        QString::fromUtf8(addressFile.getZip3()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("STR_POSTAL_LOCALITY"),
                        QString::fromUtf8(addressFile.getPostalLocality()));

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("STR_LOCALITY"),
                        QString::fromUtf8(addressFile.getLocality()));

        pos_y += 80;
    }

    if (params.isNotes)
    {
        QString perso_data;
        perso_data = QString(card->readPersonalNotes());

        if (perso_data.size() > 0)
        {
            //Force a page-break before PersoData only if we're drawing all the sections
            if (sections_to_print == 3)
            {
                printer.newPage();
                pos_y = 0;
            }
            pos_x = 0;


            drawSectionHeader(painter, pos_x, pos_y, tr("STR_PERSONAL_NOTES"));

            pos_y += 75;
            painter.drawText(QRectF(pos_x, pos_y, 700, 700), Qt::TextWordWrap, perso_data);
        }
    }

    //Finish drawing/printing
    painter.end();
    END_TRY_CATCH
    return true;
}

void GAPI::startSigningPDF(QString loadedFilePath, QString outputFile, int page, double coord_x, double coord_y,
                           QString reason, QString location, double isTimestamp, double isSmall) {

    SignParams params = {loadedFilePath, outputFile, page, coord_x, coord_y, reason, location, isTimestamp, isSmall};
    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignPDF, params);

}

void GAPI::startSigningBatchPDF(QList<QString> loadedFileBatchPath, QString outputFile, int page, double coord_x, double coord_y,
                                QString reason, QString location, double isTimestamp, double isSmall) {

    SignBatchParams params = {loadedFileBatchPath, outputFile, page, coord_x, coord_y, reason, location, isTimestamp, isSmall};

    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignBatchPDF, params);
}

unsigned int GAPI::getPDFpageCount(QString loadedFilePath) {

    PTEID_PDFSignature sig_handler(loadedFilePath.toUtf8().data());

    int pageCount = sig_handler.getPageCount();

    return (unsigned int) pageCount;
}

void GAPI::startSigningXADES(QString loadedFilePath, QString outputFile, double isTimestamp) {
    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignXADES, loadedFilePath, outputFile, isTimestamp);

}

void GAPI::doSignXADES(QString loadedFilePath, QString outputFile, double isTimestamp) {
    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    const char *files_to_sign[1];
    QByteArray tempLoadedFilePath= loadedFilePath.toUtf8();
    files_to_sign[0] = tempLoadedFilePath.constData();

    QByteArray tempOutputFile= outputFile.toUtf8();

    if (isTimestamp > 0)
        card->SignXadesT(tempOutputFile.constData(), files_to_sign, 1);
    else
        card->SignXades(tempOutputFile.constData(), files_to_sign, 1);

    emit signalPdfSignSucess();

    END_TRY_CATCH
}

void GAPI::doSignPDF(SignParams &params) {

    BEGIN_TRY_CATCH

    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL) return;

    QString fullInputPath = params.loadedFilePath;
    PTEID_PDFSignature sig_handler(fullInputPath.toUtf8().data());

    if (params.isTimestamp > 0)
        sig_handler.enableTimestamp();
    if (params.isSmallSignature > 0)
        sig_handler.enableSmallSignatureFormat();

    card->SignPDF(sig_handler, params.page, params.coord_x, params.coord_y,
                 params.location.toUtf8().data(), params.reason.toUtf8().data(),
                 params.outputFile.toUtf8().data());

    emit signalPdfSignSucess();

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
        sig_handler->addToBatchSigning( fullInputPath.toUtf8().data() , false );
    }

    if (params.isTimestamp > 0)
        sig_handler->enableTimestamp();
    if (params.isSmallSignature > 0)
        sig_handler->enableSmallSignatureFormat();

    card->SignPDF(*sig_handler, params.page, params.coord_x, params.coord_y,
                 params.location.toUtf8().data(), params.reason.toUtf8().data(),
                 params.outputFile.toUtf8().data());

    emit signalPdfSignSucess();

    END_TRY_CATCH
}

QPixmap PDFPreviewImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "PDFPreviewImageProvider received request for: " << id;
    qDebug() << "PDFPreviewImageProvider received request for: "
             << requestedSize.width() << " - " << requestedSize.height();
    QStringList strList = id.split("?");

    //This should work on Windows too with QT APIs...
    QString pdf_path = strList.at(0);

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

    //TODO: Hardcoded image size...
    //int img_height = 420;
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

void GAPI::startReadingPersoNotes() {
    QFuture<void> future = QtConcurrent::run(this, &GAPI::getPersoDataFile);
}

void GAPI::startReadingAddress() {
    QtConcurrent::run(this, &GAPI::getAddressFile);
}

void GAPI::startLoadingAttributesFromCache(bool isCompanies, bool isShortDescription) {
    QtConcurrent::run(this, &GAPI::getSCAPAttributesFromCache, isCompanies, isShortDescription);
}

void GAPI::startSigningSCAP(QString inputPDF, QString outputPDF, int page, int location_x, int location_y,
                            int ltv, QList<int> attribute_index) {
    SCAPSignParams signParams = {inputPDF, outputPDF, page, location_x, location_y,
                            ltv, attribute_index};


    QtConcurrent::run(this, &GAPI::doSignSCAP, signParams);

}


void GAPI::doSignSCAP(SCAPSignParams params) {

    std::vector<int> attrs;
    for (unsigned int i = 0; i!= params.attribute_index.size(); i++) {
        attrs.push_back(params.attribute_index.at(i));
    }

    scapServices.executeSCAPSignature(params.inputPDF, params.outputPDF, params.page,
                params.location_x, params.location_y, params.ltv, attrs);

}


void GAPI::getSCAPEntities() {
    
    QList<QString> attributeSuppliers;
    std::vector<ns3__AttributeSupplierType *> entities = scapServices.getAttributeSuppliers();

    for (unsigned int i = 0; i!=entities.size(); i++)
        attributeSuppliers.append(QString::fromStdString(entities.at(i)->Name));
    
    //TODO: emit signal for error
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
            ns3__MainAttributeType * mainAttributeObject = signatureObject->union_ObjectType.ns3__Attribute->MainAttribute;

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

void GAPI::getSCAPEntityAttributes(QList<int> entityIDs) {

    QVariantMap attribute_map;
    PTEID_EIDCard * card = NULL;
    getCardInstance(card);
    if (card == NULL){
        emit signalEntityAttributesLoadedError();
        return;
    }

    std::vector<int> supplier_ids;
    
    int supplier_id;
    foreach (supplier_id, entityIDs) {
        supplier_ids.push_back(supplier_id);
    }

    std::vector<ns2__AttributesType *> attributes = scapServices.getAttributes(*card, supplier_ids);

    if (attributes.size() == 0)
    {
        emit signalEntityAttributesLoadedError();
        return;
    }

    for(uint i = 0; i < attributes.size() ; i++)
    {
       std::string attrSupplier = attributes.at(i)->ATTRSupplier->Name;
       std::vector<std::string> childAttributes = getChildAttributes(attributes.at(i), false);

       if (childAttributes.size() == 0) {
        qDebug() << "Zero child attributes in AttributeResponseValues!";
        emit signalEntityAttributesLoadedError();
        return;   
       }

       if (childAttributes.size() > 1)
       {
           qDebug() << "TODO: multiple attributes from the same supplier is not supported yet...";
       }

       attribute_map.insert(QString::fromStdString(attrSupplier),
                            QString::fromStdString(childAttributes.at(0)));
    }

    emit signalEntityAttributesLoaded(attribute_map);
}


void GAPI::getSCAPCompanyAttributes() {

    PTEID_EIDCard * card = NULL;
    QVariantMap attribute_map;
    getCardInstance(card);
    if (card == NULL){
        emit signalCompanyAttributesLoadedError();
        return;
    }

    std::vector<int> supplierIDs;

    std::vector<ns2__AttributesType *> attributes = scapServices.getAttributes(*card, supplierIDs);

    if (attributes.size() == 0)
    {
        emit signalCompanyAttributesLoadedError();
        return;
    }

    for(uint i = 0; i < attributes.size() ; i++)
    {
       std::string attrSupplier = attributes.at(i)->ATTRSupplier->Name;
       std::vector<std::string> childAttributes = getChildAttributes(attributes.at(i), false);

       PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Attribute from supplier: %s containing %d child attributes", attrSupplier.c_str(), childAttributes.size()) ;

       if (childAttributes.size() == 0) {
        qDebug() << "Zero child attributes in AttributeResponseValues!";
        emit signalCompanyAttributesLoadedError();
        return; 
       }

       if (childAttributes.size() > 1)
       {
           qDebug() << "TODO: multiple attributes from the same supplier is not supported yet...";
       }

       attribute_map.insert(QString::fromStdString(attrSupplier),
                            QString::fromStdString(childAttributes.at(0)));
    }

    emit signalCompanyAttributesLoaded(attribute_map);
}

void GAPI::getSCAPAttributesFromCache(bool isCompanies, bool isShortDescription) {

    PTEID_EIDCard * card = NULL;
    QVariantMap attribute_map;

    getCardInstance(card);
    if (card == NULL)
        return;

    std::vector<ns2__AttributesType *> attributes = scapServices.loadAttributesFromCache(*card, isCompanies);

    for(uint i = 0; i < attributes.size() ; i++)
    {
       std::string attrSupplier = attributes.at(i)->ATTRSupplier->Name;
       std::vector<std::string> childAttributes = getChildAttributes(attributes.at(i), isShortDescription);

       if (childAttributes.size() > 1)
       {
           qDebug() << "TODO: multiple attributes from the same supplier is not supported yet...";
       }

       attribute_map.insert(QString::fromStdString(attrSupplier),
                            QString::fromStdString(childAttributes.at(0)));
    }
    if (isCompanies)
        emit signalCompanyAttributesLoaded(attribute_map);
    else
        emit signalEntityAttributesLoaded(attribute_map);
}

void GAPI::getCardInstance(PTEID_EIDCard * &new_card) {

    try
    {
        unsigned long ReaderEndIdx = ReaderSet.readerCount();
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

                if (readerContext.isCardPresent())
                {
                    bCardPresent = true;
                    CardIdx++;
                    tempReaderIndex = ReaderIdx;
                }
            }
            // Test if Card Reader was previously selected
            if(selectedReaderIndex != -1){ // Card Reader was previously selected
                PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(selectedReaderIndex);
                PTEID_CardType CardType = readerContext.getCardType();
                lastFoundCardType = CardType;
                switch (CardType)
                {
                case PTEID_CARDTYPE_IAS07:
                {
                    try
                    {
                        PTEID_EIDCard& Card = readerContext.getEIDCard();
                        new_card = &Card;
                    }
                    catch (PTEID_ExCardBadType const& e)
                    {
                        QString errcode;
                        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed %s", e.GetError());
                        errcode = errcode.setNum(e.GetError());
                    }
                }
                    break;
                case PTEID_CARDTYPE_UNKNOWN:
                default:
                    break;
                }
            }else{ //Card Reader was not previously selected
                if(CardIdx == 1){
                    PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(tempReaderIndex);
                    PTEID_CardType CardType = readerContext.getCardType();
                    lastFoundCardType = CardType;
                    switch (CardType)
                    {
                    case PTEID_CARDTYPE_IAS07:
                    {
                        try
                        {
                            PTEID_EIDCard& Card = readerContext.getEIDCard();
                            new_card = &Card;
                        }
                        catch (PTEID_ExCardBadType const& e)
                        {
                            PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed %s", e.GetError());
                        }
                    }
                        break;
                    case PTEID_CARDTYPE_UNKNOWN:
                    default:
                        break;
                    }
                }
                if(CardIdx > 1 && selectedReaderIndex == -1){
                    emit signalReaderContext("STR_WARNING","STR_MULTI_CARD");
                    selectedReaderIndex = -1;
                }else{
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
    catch (PTEID_ExCardBadType e)
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
    catch (PTEID_ExReaderSetChanged e)
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
    //------------------------------------
    // TODO: cleanup the callback data
    //------------------------------------

    try
    {
        PTEID_ReaderContext& readerContext = ReaderSet.getReaderByName(pCallBackData->getReaderName().toLatin1());

        //------------------------------------
        // is card retracted from reader?
        //------------------------------------
        if (!readerContext.isCardPresent())
        {

            //------------------------------------
            // TODO: remove the certificates
            //------------------------------------

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

            return;
        }
        //------------------------------------
        // is card inserted ?
        //------------------------------------
        if (readerContext.isCardChanged(pCallBackData->m_cardID))
        {
            //------------------------------------
            // send an event to the main app to show the popup message
            //------------------------------------
            pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_CHANGED);
            pCallBackData->getMainWnd()->setAddressLoaded(false);
            pCallBackData->getMainWnd()->resetReaderSelected();
        }
    }
    catch (...)
    {
        emit pCallBackData->getMainWnd()->signalCardAccessError(GAPI::CardUnknownError);
        // we catch ALL exceptions. This is because otherwise the thread throwing the exception stops
    }
}

void GAPI::setEventCallbacks( void )
{
    //----------------------------------------
    // for all the readers, create a callback such we can know
    // afterwards, which reader called us
    //----------------------------------------

    //------------------------------------
    // TODO:     update the readerlist
    //------------------------------------

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

void GAPI::buildTree(PTEID_Certificate &cert, bool &bEx, QVariantMap &certificatesMap)
{
    QVariantMap certificatesMapChildren;
    static int level = 0;

    if (cert.isRoot())
    {
        certificatesMapChildren.insert("OwnerName", cert.getOwnerName());
        certificatesMapChildren.insert("IssuerName", cert.getIssuerName());
        certificatesMapChildren.insert("ValidityBegin", cert.getValidityBegin());
        certificatesMapChildren.insert("ValidityEnd", cert.getValidityEnd());
        certificatesMapChildren.insert("KeyLength", QString::number(cert.getKeyLength()));
        certificatesMapChildren.insert("Status", cert.getStatus());

        certificatesMap.insert("level" + QString::number(level),certificatesMapChildren);
        certificatesMap.insert("levelCount",level+1);
        level = 0;
    }
    else
    {
        certificatesMapChildren.insert("OwnerName", cert.getOwnerName());
        certificatesMapChildren.insert("IssuerName", cert.getIssuerName());
        certificatesMapChildren.insert("ValidityBegin", cert.getValidityBegin());
        certificatesMapChildren.insert("ValidityEnd", cert.getValidityEnd());
        certificatesMapChildren.insert("KeyLength", QString::number(cert.getKeyLength()));
        certificatesMapChildren.insert("Status", cert.getStatus());

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
