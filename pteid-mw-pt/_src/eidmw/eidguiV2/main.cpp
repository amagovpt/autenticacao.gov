#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "appcontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");
    //QQuickStyle::setStyle("Universal");
    //QQuickStyle::setStyle("Default");

    QQmlApplicationEngine engine;
    AppController controler;

    QQmlContext* ctx = engine.rootContext();
        ctx->setContextProperty("controler", &controler);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
