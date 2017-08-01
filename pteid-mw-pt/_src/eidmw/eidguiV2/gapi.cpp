#include "gapi.h"
#include <QString>
#include <QDate>
#include <QtConcurrent>
#include <cstdio>
#include <QQuickImageProvider>

using namespace eIDMW;

#define TRIES_LEFT_ERROR    1000

/*
    GAPI - Graphic Application Programming Interface
*/

GAPI::GAPI(QObject *parent) :
    QObject(parent) {
    image_provider = new PhotoImageProvider();
    image_provider_pdf = new PDFPreviewImageProvider();
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
    fprintf(stderr, "Generic eidlib exception! Error code (see strings in eidErrors.h): %08lx\n", e.GetError()); \
    emit signalCardAccessError(CardUnknownError); \
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

void GAPI::startSigningPDF(QString loadedFilePath, QString outputFile, int page, double coord_x, double coord_y,
                           QString reason, QString location, double isTimestamp, double isSmall) {

    SignParams params = {loadedFilePath, outputFile, page, coord_x, coord_y, reason, location, isTimestamp, isSmall};
    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignPDF, params);

}

void GAPI::startSigningXADES(QString loadedFilePath, QString outputFile, double isTimestamp) {
    QFuture<void> future =
            QtConcurrent::run(this, &GAPI::doSignXADES, loadedFilePath, outputFile, isTimestamp);

}

QString skipFileURL(QString input) {
    const char * needle = "file://";
    if (input.startsWith(needle))
    {
        return input.mid(strlen(needle));
    }
    else {
        return input;
    }
}


void GAPI::doSignXADES(QString loadedFilePath, QString outputFile, double isTimestamp) {
    BEGIN_TRY_CATCH

            PTEID_EIDCard &card = getCardInstance();

    const char *files_to_sign[1];
    files_to_sign[0] = loadedFilePath.toUtf8().constData();

    if (isTimestamp > 0)
        card.SignXadesT(outputFile.toUtf8().constData(), files_to_sign, 1);
    else
        card.SignXades(outputFile.toUtf8().constData(), files_to_sign, 1);

    END_TRY_CATCH

            emit signalPdfSignSucess();
}

void GAPI::doSignPDF(SignParams &params) {

    BEGIN_TRY_CATCH

            PTEID_EIDCard &card = getCardInstance();
    QString fullInputPath = "/" + params.loadedFilePath;
    QString fullOutputPath = skipFileURL(params.outputFile);
    PTEID_PDFSignature sig_handler(fullInputPath.toUtf8().data());

    if (params.isTimestamp > 0)
        sig_handler.enableTimestamp();
    if (params.isSmallSignature > 0)
        sig_handler.enableSmallSignatureFormat();

    card.SignPDF(sig_handler, params.page, params.coord_x, params.coord_y,
                 params.location.toUtf8().data(), params.reason.toUtf8().data(), fullOutputPath.toUtf8().data());

    END_TRY_CATCH

            emit signalPdfSignSucess();
}

QPixmap PDFPreviewImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "PDFPreviewImageProvider received request for: " << id;
    qDebug() << "PDFPreviewImageProvider received request for: "
             << requestedSize.width() << " - " << requestedSize.height();
    QStringList strList = id.split("?");

    //This should work on Windows too with QT APIs...
    QString pdf_path = "/" + strList.at(0);

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

void GAPI::startReadingPersoNotes() {
    QFuture<void> future = QtConcurrent::run(this, &GAPI::getPersoDataFile);
}

void GAPI::startReadingAddress() {
    QtConcurrent::run(this, &GAPI::getAddressFile);
}

PTEID_EIDCard & GAPI::getCardInstance() {

    PTEID_ReaderContext& readerContext = ReaderSet.getReader();
    qDebug() << "Using Card Reader: " << readerContext.getName();
    return readerContext.getEIDCard();
}

void GAPI::connectToCard() {

    BEGIN_TRY_CATCH

            PTEID_EIDCard &card = getCardInstance();
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
