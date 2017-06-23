#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include "appcontroller.h"
#include "gapi.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Set app icon
    app.setWindowIcon(QIcon(":/favicon.ico"));

    QQuickStyle::setStyle("Material");
    //QQuickStyle::setStyle("Universal");
    //QQuickStyle::setStyle("Default");

    QQmlApplicationEngine engine;

    // AppController init
    AppController controler;

    // GAPI init
    GAPI gapi;

    // GAPI Test Functions
    gapi.fillDataCardIdentifyDummy();

    // Embedding C++ Objects into QML with Context Properties
    QQmlContext* ctx = engine.rootContext();
        ctx->setContextProperty("gapi", &gapi);
        ctx->setContextProperty("controler", &controler);

    // Load main QMl file
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
