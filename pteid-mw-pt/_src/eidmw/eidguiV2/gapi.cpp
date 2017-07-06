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

//TODO: emit signal for errors
void GAPI::getPersoDataFile() {
    try
    {
        //TODO: we should store the readerContext or EIDCard reference in the GAPI object
        PTEID_ReaderContext& readerContext = ReaderSet.getReader();

        if (!readerContext.isCardPresent())
        {
            qDebug() << "No card found in the reader!" << endl;
            return;
        }

        PTEID_EIDCard &card = readerContext.getEIDCard();
        emit signalPersoDataLoaded(QString(card.readPersonalNotes()));
    }
    catch (PTEID_Exception &e)
    {
        qDebug() << "getPersoDataFile(): Generic MW exception ! Error code: " << e.GetError() ;
    }

    
}

QString GAPI::getCardActivation() {
    try
    {
        //TODO: we should store the readerContext or EIDCard reference in the GAPI object
        PTEID_ReaderContext& readerContext = ReaderSet.getReader();

        if (!readerContext.isCardPresent())
        {
            qDebug() << "No card found in the reader!" << endl;
            return QString();
        }

        PTEID_EIDCard &card = readerContext.getEIDCard();
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
    }
    catch (PTEID_Exception &e)
    {
        qDebug() << "getCardActivation(): Generic MW exception ! Error code: " << e.GetError() ;
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

void GAPI::connectToCard() {

    try
    {
        PTEID_ReaderContext& readerContext = ReaderSet.getReader();
        
        qDebug() << "Using Card Reader: " << readerContext.getName();

        if (!readerContext.isCardPresent())
        {
            qDebug() << "No card found in the reader!" << endl;
            return;
        }

        PTEID_EIDCard &card = readerContext.getEIDCard();
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
    catch (PTEID_ExNoReader &e)
    {
        qDebug() << "No card reader found !";
    }
    catch (PTEID_Exception &e)
    {
        qDebug() << "connectToCard: Generic MW exception ! Error code: " << e.GetError() ;
    }

    //TODO: emit signal for errors
}
