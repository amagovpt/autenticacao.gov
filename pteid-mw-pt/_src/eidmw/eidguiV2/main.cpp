#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include "appcontroller.h"
#include "filesavedialog.h"
#include "gapi.h"
#include "eidlib.h"
using namespace eIDMW;

int main(int argc, char *argv[])
{
    int retValue = 0;
    QApplication app(argc, argv);

    // Set app icon
    app.setWindowIcon(QIcon(":/favicon.ico"));

    QQuickStyle::setStyle("Material");
    //QQuickStyle::setStyle("Universal");
    //QQuickStyle::setStyle("Default");

    QQmlApplicationEngine engine;

    // AppController init
    AppController controller;
    PTEID_InitSDK();

    // GAPI init
    GAPI gapi;
    GAPI::declareQMLTypes();

    if (argc == 3 && strcmp(argv[1], "advanced") == 0) {
        QString inputPdf(argv[2]);
        gapi.setShortcutFlag(1);
        if (QFileInfo::exists(inputPdf))
            gapi.setShortcutPDFPath(inputPdf);
    }

    // Embedding C++ Objects into QML with Context Properties
    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("gapi", &gapi);
    ctx->setContextProperty("controler", &controller);
    ctx->setContextProperty("image_provider_pdf", gapi.image_provider_pdf);

    qmlRegisterType<FileSaveDialog>("eidguiV2", 1, 0, "FileSaveDialog");

    engine.addImageProvider("myimageprovider", gapi.buildImageProvider());
    engine.addImageProvider("pdfpreview_imageprovider", gapi.buildPdfImageProvider());

    // Load main QML file
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    retValue = app.exec();

    PTEID_ReleaseSDK();

    return retValue;
}
