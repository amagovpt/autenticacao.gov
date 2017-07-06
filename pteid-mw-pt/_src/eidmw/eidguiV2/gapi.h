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

    // GAPI Test Functions:
    gapi.fillDataCardIdentifyDummy();
    gapi.testUpdateCardIdentify(1000);
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

public:
    explicit GAPI(QObject *parent = 0);

    enum IDInfoKey { Documenttype, Documentversion, Surname, Givenname, Sex, Height, Nationality, Birthdate, Documentnum, Validitybegindate, Validityenddate, 
          NIC, NIF, NISS, NSNS, IssuingEntity, PlaceOfRequest, Country, Father, Mother, AccidentalIndications };

    enum AddressInfoKey { District, Municipality, Parish, Streettype, Streetname, Buildingtype, Doorno, Floor, side, locality, place, zip4, zip3, postalLocality};

    Q_ENUMS(IDInfoKey)
    Q_ENUMS(AddressInfoKey)

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
    
    void startCardReading();
    void startReadingPersoNotes();
    void verifyAddressPin(QString &pin);
    QString getCardActivation();
    QString getDataCardIdentifyValue(GAPI::IDInfoKey stringValue);

signals:
    // Signal from GAPI to Gui
    // Notify about Card Identify changed
    void signalCardDataChanged();
    void signalPersoDataLoaded(QString persoNotes);

private:
    void setDataCardIdentify(QMap<GAPI::IDInfoKey, QString> m_data);
    void connectToCard();
    void getPersoDataFile();

    // Data Card Identify map
    QMap<GAPI::IDInfoKey, QString> m_data;
    //Don't free this!, we release ownership to the QMLEngine in buildImageProvider()
    PhotoImageProvider *image_provider;

private slots:
    // Test functions
    void triggerTestUpdateCardIdentify();
};
#endif // GAPI_H
