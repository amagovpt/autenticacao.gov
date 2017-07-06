#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include "appcontroller.h"
#include "gapi.h"
#include "eidlib.h"
using namespace eIDMW;

int main(int argc, char *argv[])
{
    int retValue = 0;
    QGuiApplication app(argc, argv);

    // Set app icon
    app.setWindowIcon(QIcon(":/favicon.ico"));

    QQuickStyle::setStyle("Material");
    //QQuickStyle::setStyle("Universal");
    //QQuickStyle::setStyle("Default");

    QQmlApplicationEngine engine;

    // AppController init
    AppController controller;

    // GAPI init
    GAPI gapi;
    GAPI::declareQMLTypes();

    // GAPI Test Functions
    //gapi.fillDataCardIdentifyDummy();

    //TODO: We shouldn't really try to connect on startup ...
    //gapi.connectToCard();

    // Embedding C++ Objects into QML with Context Properties
    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("gapi", &gapi);
    ctx->setContextProperty("controler", &controller);

    engine.addImageProvider("myimageprovider", gapi.buildImageProvider());

    // Load main QML file
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    retValue = app.exec();

    PTEID_ReleaseSDK();

    return retValue;
}
