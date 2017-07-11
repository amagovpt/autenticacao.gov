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

//MW libraries
#include "eidlib.h"
#include "eidlibException.h"

/*
    GAPI - Graphic Application Programming Interface

    // GAPI init
    GAPI gapi;

*/

class PhotoImageProvider: public QQuickImageProvider 
{
public:
    PhotoImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    void setPixmap(QPixmap &pixmap) { p = pixmap; }

private:
    QPixmap p;
};

class GAPI : public QObject
{
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

    enum AddressInfoKey { District, Municipality, Parish, Streettype, Streetname, Buildingtype, Doorno, Floor, Side, Locality, Place, Zip4, Zip3, PostalLocality};

    Q_ENUMS(IDInfoKey)
    Q_ENUMS(AddressInfoKey)

    bool isAddressLoaded() {return m_addressLoaded; }

    QQuickImageProvider * buildImageProvider() { return image_provider; }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

    // Public Method to Test GAPI
    //void fillDataCardIdentifyDummy();
    void testUpdateCardIdentify(int timerValue);

    // Do not forget to declare your class to the QML system.
    static void declareQMLTypes() {
        qmlRegisterType<GAPI>("eidguiV2", 1, 0, "GAPI");
    }

public slots:
    // Slots to Gui request values
    void setAddressLoaded(bool addressLoaded) {m_addressLoaded = addressLoaded; }
    void startCardReading();
    void startReadingPersoNotes();
    void startReadingAddress();
    unsigned int verifyAddressPin(QString pin);
    QString getCardActivation();
    QString getDataCardIdentifyValue(GAPI::IDInfoKey key);
    QString getAddressField(GAPI::AddressInfoKey key);

signals:
    // Signal from GAPI to Gui
    // Notify about Card Identify changed
    void signalCardDataChanged();
    void signalAddressLoaded();
    void cardAcessError();
    void signalPersoDataLoaded(const QString& persoNotes);
    void signalAddressLoadedChanged();

private:
    void setDataCardIdentify(QMap<GAPI::IDInfoKey, QString> m_data);
    void connectToCard();
    void getPersoDataFile();
    void getAddressFile();
    eIDMW::PTEID_EIDCard & getCardInstance();

    // Data Card Identify map
    QMap<GAPI::IDInfoKey, QString> m_data;
    QMap<GAPI::AddressInfoKey, QString> m_addressData;
    //Don't free this!, we release ownership to the QMLEngine in buildImageProvider()
    PhotoImageProvider *image_provider;
    QString m_persoData;
    bool m_addressLoaded;

private slots:
    // Test functions
    void triggerTestUpdateCardIdentify();
};
#endif // GAPI_H
