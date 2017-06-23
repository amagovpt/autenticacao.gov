#include "gapi.h"

/*
    GAPI - Graphic Application Programming Interface
*/

GAPI::GAPI(QObject *parent) :
    QObject(parent)
{
}

QString GAPI::getDataCardIdentifyValue(QString stringValue){

    qDebug() << "C++: getDataCardIdentifyValue ";

    return m_data[stringValue];
}

QMap<QString, QString> GAPI::getDataCardIdentify(){

    qDebug() << "C++: getDataCardIdentify ";

    return m_data;
}

void GAPI::setDataCardIdentify(QMap<QString, QString> data){

    qDebug() << "C++: setDataCardIdentify ";

    m_data = data;
    emit signalCardIdentifyChanged();
}

void GAPI::testUpdateCardIdentify(int timerValue) {

    qDebug() << "C++: testUpdateCardIdentify";

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this,
            SLOT(triggerTestUpdateCardIdentify()));
    timer->start(timerValue);
}

void GAPI::triggerTestUpdateCardIdentify(){

    static char i = 0;

    m_data["name"] = "Joana Ovilia " + QString::number(i++);

    setDataCardIdentify(m_data);
}

void GAPI::fillDataCardIdentifyDummy(){

    qDebug() << "C++: fillDataDummy";

    QMap<QString, QString> testData;

    testData["name"] = "\
Joana Ovilia Maria Manuela Diana Rute Manuel Maria"; // Max. 50 chars
    testData["givenname"] = "\
Ribeiro Martins Sousa Costadas Mender Cavaco Soares Meireles\
Ribeiro Martins Sousa Costadas Mender Cavao Soares"; // Max. 110 chars
    testData["sex"] = "F";
    testData["height"] = "1,77";
    testData["nationality"] = "PRT";
    testData["birthdate"] = "25 12 1980";
    testData["documentnumber"] = "12433954";
    testData["card_validuntil"] = "31 12 2019";
    testData["country"] = "PRT";
    testData["father"] = "\
João Ribeiro Martins Sousa Costadas Mender Cavaco Soares Meireles";
    testData["mother"] = "\
Maria Ribeiro Martins Sousa Costadas Mender Cavaco Soares Meireles";
    testData["ACCIDENTALINDICATIONS"] = "Não sabe assinar";

    setDataCardIdentify(testData);
}
