#include "gapi.h"
#include <QString>
#include <QDate>
#include <QtConcurrent>
#include <QQuickImageProvider>

using namespace eIDMW;

/*
    GAPI - Graphic Application Programming Interface
*/

GAPI::GAPI(QObject *parent) :
    QObject(parent) {
        image_provider = new PhotoImageProvider();
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

bool GAPI::verifyAddressPin(QString pin_value) {
    unsigned long tries_left = 0;

    PTEID_EIDCard &card = getCardInstance();

    PTEID_Pin & address_pin = card.getPins().getPinByPinRef(PTEID_ADDRESS_PIN_ID);
    bool ret = address_pin.verifyPin(pin_value.toLatin1().data(), tries_left);

    if (tries_left == 0) {
        qDebug() << "Address PIN blocked!";
    }

    return ret;
}

QString GAPI::getCardActivation() {

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

void GAPI::startCardReading() {
 QFuture<void> future = QtConcurrent::run(this, &GAPI::connectToCard);

}

void GAPI::startReadingPersoNotes() {
    QFuture<void> future = QtConcurrent::run(this, &GAPI::getPersoDataFile);
}

PTEID_EIDCard & GAPI::getCardInstance() {
    try
    {
        PTEID_ReaderContext& readerContext = ReaderSet.getReader();
        qDebug() << "Using Card Reader: " << readerContext.getName();

        if (!readerContext.isCardPresent())
        {
            qDebug() << "No card found in the reader!" << endl;
            //TODO: add specific error code for "No card found" error
            emit cardAcessError();
        }
        PTEID_EIDCard &card = readerContext.getEIDCard();

        return card;
    }
    catch (PTEID_ExNoReader &)
    {
        qDebug() << "No card reader found !";
    }
    catch (PTEID_Exception &e)
    {
        qDebug() << "getCardInstance: Generic MW exception ! Error code: " << e.GetError() ;
    }

    emit cardAcessError();
}

void GAPI::connectToCard() {

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

}

