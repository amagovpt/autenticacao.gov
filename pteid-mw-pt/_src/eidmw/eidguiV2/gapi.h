#ifndef GAPI_H
#define GAPI_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QDebug>

/*
    GAPI - Graphic Application Programming Interface

    // GAPI init
    GAPI gapi;

    // GAPI Test Functions:
    gapi.fillDataCardIdentifyDummy();
    gapi.testUpdateCardIdentify(1000);
*/

class GAPI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMap<QString, QString> m_data
               READ getDataCardIdentify
               WRITE setDataCardIdentify
               NOTIFY signalCardIdentifyChanged)

public:
    explicit GAPI(QObject *parent = 0);

    // Public Method to get Data Card Identify
    QMap<QString, QString> getDataCardIdentify();

    // Public Method to set Data Card Identify
    void setDataCardIdentify(QMap<QString, QString> m_data);

    // Public Method to Test GAPI
    void fillDataCardIdentifyDummy();
    void testUpdateCardIdentify(int timerValue);

public slots:
    // Slot to Gui request values
    QString getDataCardIdentifyValue(QString stringValue);

signals:
    // Signal from GAPI to Gui
    // Notify about Card Identify changed
    void signalCardIdentifyChanged();

private:
    // Data Card Identify map
    QMap<QString, QString> m_data;

private slots:
    // Test functions
    void triggerTestUpdateCardIdentify();
};
#endif // GAPI_H
