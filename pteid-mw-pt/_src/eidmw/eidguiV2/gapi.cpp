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

    PTEID_EIDCard &card = getCardInstance();

    PTEID_Address &addressFile = card.getAddr();

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

    PTEID_EIDCard &card = getCardInstance();

    emit signalPersoDataLoaded(QString(card.readPersonalNotes()));

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
            unsigned char *pucData = (unsigned char *)calloc( ulSize, sizeof(char) );

            const PTEID_ByteArray oData( (const unsigned char *)pucData, ulSize);
            Card.writePersonalNotes(oData);
            free(pucData);
        }
        qDebug() << "Personal notes successfully written!" ;
        emit signalSetPersoDataFile("Success","Personal notes successfully written!");

    } catch (PTEID_Exception& e) {
        qDebug() << "Error writing personal notes!" ;
        emit signalSetPersoDataFile("Error","Error writing personal notes!");
    }

}

unsigned int GAPI::verifyAuthPin(QString pin_value) {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & auth_pin = card.getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
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

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & auth_pin = card.getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
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

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & sign_pin = card.getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
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

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & sign_pin = card.getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
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

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & address_pin = card.getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
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

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & address_pin = card.getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
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

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & auth_pin = card.getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
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

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & sign_pin = card.getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
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
    QString support_string = tr("Please try again. If this error persists, please have your"
                                " process number and error code ready, and contact the"
                                " Citizen Card support line at telephone number +351 211 950 500 or e-mail cartaodecidadao@irn.mj.pt.");

    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AddressChange op finished with error code 0x%08x", code);

    switch(code)
    {
    case 0:
        error_msg = tr("Address Confirmed successfully.");
        break;
        //The error code for connection error is common between SAM and OTP
    case EIDMW_OTP_CONNECTION_ERROR:
        error_msg = tr("Connection Error") + "\n\n" +
                tr("Please make sure you are connected to the Internet");
        break;

    case 1121:
    case 1122:
        error_msg = tr("Error in the Address Change operation!") + "\n\n" + tr("Please make sure you typed the correct process number and confirmation code");
        sam_error_code = code;
        break;
    case EIDMW_SAM_UNCONFIRMED_CHANGE:
        error_msg = tr("Address change process is incomplete!") + "\n\n" + tr("The address is changed in the card but not confirmed by the State central services");
        break;
    case EIDMW_SSL_PROTOCOL_ERROR:
        error_msg = tr("Error in the Address Change operation!") + "\n\n" + tr("Please make sure you have a valid authentication certificate");
        break;
    default:
        //Make sure we only show the user error codes from the SAM service and not some weird pteid exception error code
        if (code > 1100 && code < 3500)
            sam_error_code = code;
        error_msg = tr("Error in the Address Change operation!");
        break;
    }

    if (sam_error_code != 0)
    {
        error_msg += "\n\n" + tr("Error code = ") + QString::number(sam_error_code);
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
    QString support_string = tr("Please try again. If this error persists, please have your"
                                " process number and error code ready, and contact the"
                                " CMD support line at telephone number +351 xxx xxx xxx or e-mail xxxx@xxx.pt.");

    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "CMD signature op finished with error code 0x%08x", code);

    switch(code)
    {
    case 0:
        error_msg = tr("Assinatura com Chave Móvel Digital com sucesso.");
        break;
        //The error code for connection error is common between SAM and OTP
    default:
        //Make sure we only show the user error codes from the SAM service and not some weird pteid exception error code
        sam_error_code = code;
        error_msg = tr("Error in the Chave Móvel Digital Login operation!");
        break;
    }

    if (sam_error_code != 0)
    {
        error_msg += "\n\n";
        error_msg += "Error code = " + QString::number(sam_error_code);
    }

    if (code != 0)
        error_msg += "\n\n" + support_string;

    qDebug() << error_msg;
    signalUpdateProgressStatus(error_msg);
}

unsigned int GAPI::changeAddressPin(QString currentPin, QString newPin) {
    unsigned long tries_left = TRIES_LEFT_ERROR;

    BEGIN_TRY_CATCH

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & address_pin = card.getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
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

    PTEID_EIDCard& Card = getCardInstance();
    try
    {
        Card.ChangeAddress((char *)secret_code, (char*)process, &GAPI::addressChangeCallback, (void*)this);
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

    signalUpdateProgressStatus(tr("Mudando a morada no Cartão..."));

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
    signalUpdateProgressStatus("Login com sucesso. Aguarde a recepção do código enviado por sms!");
    emit signalOpenCMDSucess();
}

void GAPI::doCloseSignCMD(CMDSignature *cmd_signature, QString sms_token)
{
    qDebug() << "doCloseSignCMD! " << "sms_token = " << sms_token;

    int ret = 0;
    std::string local_sms_token = sms_token.toUtf8().data();
    //std::string local_sms_token =  "111111";

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

    signalUpdateProgressStatus("Conectando com o servidor");

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

    signalUpdateProgressStatus("Enviando código de confirmação para o servidor");

    connect(this, SIGNAL(signCMDFinished(long)),
            this, SLOT(showSignCMDDialog(long)), Qt::UniqueConnection);

    QtConcurrent::run(this, &GAPI::doCloseSignCMD, cmd_signature, sms_token);
}

QString GAPI::getCardActivation() {
    BEGIN_TRY_CATCH

            PTEID_EIDCard &card = getCardInstance();
    PTEID_EId &eid_file = card.getID();

    if (isExpiredDate(eid_file.getValidityEndDate())) {
        return QString("The Citizen Card is expired");
    }
    else if (!card.isActive()) {
        return QString("The Citizen Card is not active");
    }
    else {
        return QString("The Citizen Card has been activated");
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

            PTEID_EIDCard &card = getCardInstance();
    PTEID_PDFSignature sig_handler(file_to_sign.toUtf8().data());

    card.SignPDF(sig_handler, 0, 0, false, "", "", outputsign.toUtf8().data());

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

    PTEID_EIDCard &card = getCardInstance();
    card.doSODCheck(true); //Enable SOD checking
    PTEID_EId &eid_file = card.getID();

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

    painter.drawText(QPointF(pos_x, pos_y), tr("PERSONAL DATA"));

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

        drawSectionHeader(painter, pos_x, pos_y, tr("BASIC INFORMATION"));
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

        drawSingleField(painter, pos_x, pos_y, tr("Given Name(s)"), QString::fromUtf8(eid_file.getGivenName()), true);

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Surname(s)"), QString::fromUtf8(eid_file.getSurname()), true);

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Gender"), QString::fromUtf8(eid_file.getGender()));
        drawSingleField(painter, pos_x + COLUMN_WIDTH, pos_y, tr("Height"), QString::fromUtf8(eid_file.getHeight()));
        drawSingleField(painter, pos_x + COLUMN_WIDTH*2, pos_y, tr("Date of birth"),
                        QString::fromUtf8(eid_file.getDateOfBirth()));

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Document Number"), QString::fromUtf8(eid_file.getDocumentNumber()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Validity Date"),
                        QString::fromUtf8(eid_file.getValidityEndDate()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Country"), QString::fromUtf8(eid_file.getCountry()));

        pos_y += LINE_HEIGHT;
        drawSingleField(painter, pos_x, pos_y, tr("Father"), QString::fromUtf8(eid_file.getGivenNameFather()) + " " +
                        QString::fromUtf8(eid_file.getSurnameFather()), true);
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Mother"), QString::fromUtf8(eid_file.getGivenNameMother()) + " " +
                        QString::fromUtf8(eid_file.getSurnameMother()), true);
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Notes"),
                        QString::fromUtf8(eid_file.getAccidentalIndications()), true);

        pos_y += 50;
    }

    if (params.isAddicionalInfo)
    {
        drawSectionHeader(painter, pos_x, pos_y, tr("ADDITIONAL INFORMATION"));
        sections_to_print++;
        pos_y += 50;

        drawSingleField(painter, pos_x, pos_y, tr("VAT identification no."), QString::fromUtf8(eid_file.getTaxNo()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Social Security no."),
                        QString::fromUtf8(eid_file.getSocialSecurityNumber()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("National Health System no."),
                        QString::fromUtf8(eid_file.getHealthNumber()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Card Version"), QString::fromUtf8(eid_file.getDocumentVersion()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Delivery Date"),
                        QString::fromUtf8(eid_file.getValidityBeginDate()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Delivery Entity"),
                        QString::fromUtf8(eid_file.getIssuingEntity()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Card State"), getCardActivation());
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Document type"),
                        QString::fromUtf8(eid_file.getDocumentType()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Delivery Location"),
                        QString::fromUtf8(eid_file.getLocalofRequest()));

        pos_y += 50;
    }

    if (params.isAddress)
    {
        drawSectionHeader(painter, pos_x, pos_y, tr("ADDRESS"));
        sections_to_print++;
        pos_y += 50;

        PTEID_Address &addressFile = card.getAddr();

        // TODO: Foreign address

        drawSingleField(painter, pos_x, pos_y, tr("District"),
                        QString::fromUtf8(addressFile.getDistrict()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Municipality"),
                        QString::fromUtf8(addressFile.getMunicipality()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Civil Parish"),
                        QString::fromUtf8(addressFile.getCivilParish()));

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Ab. street type"),
                        QString::fromUtf8(addressFile.getAbbrStreetType()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Street type"),
                        QString::fromUtf8(addressFile.getStreetType()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Street Name"),
                        QString::fromUtf8(addressFile.getStreetName()));

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Ab. Building Type"),
                        QString::fromUtf8(addressFile.getAbbrBuildingType()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Building Type"),
                        QString::fromUtf8(addressFile.getBuildingType()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("House/building no."),
                        QString::fromUtf8(addressFile.getDoorNo()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Floor"),
                        QString::fromUtf8(addressFile.getFloor()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Side"),
                        QString::fromUtf8(addressFile.getSide()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Place"),
                        QString::fromUtf8(addressFile.getPlace()));
        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Zip Code 4"),
                        QString::fromUtf8(addressFile.getZip4()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Zip Code 3"),
                        QString::fromUtf8(addressFile.getZip3()));
        drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Postal Locality"),
                        QString::fromUtf8(addressFile.getPostalLocality()));

        pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Locality"),
                        QString::fromUtf8(addressFile.getLocality()));

        pos_y += 80;
    }

    if (params.isNotes)
    {
        QString perso_data;
        perso_data = QString(card.readPersonalNotes());

        if (perso_data.size() > 0)
        {
            //Force a page-break before PersoData only if we're drawing all the sections
            if (sections_to_print == 3)
            {
                printer.newPage();
                pos_y = 0;
            }
            pos_x = 0;


            drawSectionHeader(painter, pos_x, pos_y, tr("PERSONAL NOTES"));

            pos_y += 75;
            painter.drawText(QRectF(pos_x, pos_y, 700, 700), Qt::TextWordWrap, perso_data);
        }
    }

    //Finish drawing/printing
    painter.end();
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

            PTEID_EIDCard &card = getCardInstance();

    const char *files_to_sign[1];
    QByteArray tempLoadedFilePath= loadedFilePath.toUtf8();
    files_to_sign[0] = tempLoadedFilePath.constData();

    QByteArray tempOutputFile= outputFile.toUtf8();

    if (isTimestamp > 0)
        card.SignXadesT(tempOutputFile.constData(), files_to_sign, 1);
    else
        card.SignXades(tempOutputFile.constData(), files_to_sign, 1);

    emit signalPdfSignSucess();

    END_TRY_CATCH
}

void GAPI::doSignPDF(SignParams &params) {

    BEGIN_TRY_CATCH

            PTEID_EIDCard &card = getCardInstance();
    QString fullInputPath = params.loadedFilePath;
    PTEID_PDFSignature sig_handler(fullInputPath.toUtf8().data());

    if (params.isTimestamp > 0)
        sig_handler.enableTimestamp();
    if (params.isSmallSignature > 0)
        sig_handler.enableSmallSignatureFormat();

    card.SignPDF(sig_handler, params.page, params.coord_x, params.coord_y,
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

            PTEID_EIDCard &card = getCardInstance();

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

    card.SignPDF(*sig_handler, params.page, params.coord_x, params.coord_y,
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

int GAPI::getStringByteLenght(QString text) {

    int strLenght;

    strLenght = text.toStdString().size() + 1; // '\0' should be considered as a byte

    return strLenght;
}

void GAPI::startReadingPersoNotes() {
    QFuture<void> future = QtConcurrent::run(this, &GAPI::getPersoDataFile);
}

void GAPI::startReadingAddress() {
    QtConcurrent::run(this, &GAPI::getAddressFile);
}

PTEID_EIDCard & GAPI::getCardInstance() {

    PTEID_ReaderContext& readerContext = ReaderSet.getReader();
    qDebug() << "Using Card Reader: " << readerContext.getName();

    PTEID_EIDCard &ref = readerContext.getEIDCard();
    ref.doSODCheck(true); //Enable SOD checking

    return ref;
}

void GAPI::connectToCard() {

    BEGIN_TRY_CATCH

            PTEID_EIDCard &card = getCardInstance();

    card.doSODCheck(true); //Enable SOD checking
    PTEID_EId &eid_file = card.getID();

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
            pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_REMOVED);
            pCallBackData->getMainWnd()->setAddressLoaded(false);

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
        }
    }
    catch (PTEID_ExBadTransaction& e)
    {
        long err = e.GetError();
        err = err;
    }
    catch (...)
    {
        int x=0;
        x++;
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

        if(certificatesMap.contains("level" + QString::number( level ))){
            certificatesMap.insert("levelB" + QString::number(level),certificatesMapChildren);
        }else{
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

            PTEID_EIDCard &card = getCardInstance();

    PTEID_Certificates&	 certificates	= card.getCertificates();

    // TODO: In test mode with production card or in production mode with test card
    // this cause a segment fault

    certificatesMap.clear();
    buildTree(certificates.getCert(PTEID_Certificate::CITIZEN_AUTH), noIssuer,certificatesMap);
    buildTree(certificates.getCert(PTEID_Certificate::CITIZEN_SIGN), noIssuer,certificatesMap);

    emit signalCertificatesChanged(certificatesMap);
    END_TRY_CATCH
}
