#include "gapi.h"
#include <QString>
#include <QDate>
#include <QtConcurrent>
#include <cstdio>
#include <QQuickImageProvider>

using namespace eIDMW;

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

void GAPI::getPersoDataFile() {

    qDebug() << "getPersoDataFile() called";
    PTEID_EIDCard &card = getCardInstance();

    emit signalPersoDataLoaded(QString(card.readPersonalNotes()));

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
        fprintf(stderr, "Generic eidlib exception! Error code (see strings in eidErrors.h): %08lx", e.GetError()); \
        emit signalCardAccessError(CardUnknownError); \
    }

void GAPI::getAddressFile() {
    qDebug() << "C++: getAddressFile()";
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
}

unsigned int GAPI::verifyAddressPin(QString pin_value) {
    unsigned long tries_left = 0;

    BEGIN_TRY_CATCH

    PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & address_pin = card.getPins().getPinByPinRef(PTEID_ADDRESS_PIN_ID);
    address_pin.verifyPin(pin_value.toLatin1().data(), tries_left);

    if (tries_left == 0) {
        qDebug() << "WARNING: Address PIN blocked!";
    }

    END_TRY_CATCH

    //QML default types don't include long
    return (unsigned int)tries_left;
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
    qDebug() << "PDFPreviewImageProvider received request for: " << requestedSize.height() << " - " << requestedSize.width();
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
    QPixmap p = renderPDFPage(page).scaledToHeight(requestedSize.height(), Qt::SmoothTransformation);
    size->setHeight(p.height());
    size->setWidth(p.width());
    return p;
}

QPixmap PDFPreviewImageProvider::renderPDFPage(unsigned int page)
{
    // Document starts at page 0 in the poppler-qt5 API
    Poppler::Page *popplerPage = m_doc->page(page-1);

    //TODO: Test the resolution on Windows
    const double resX = 40.0;
    const double resY = 40.0;
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

