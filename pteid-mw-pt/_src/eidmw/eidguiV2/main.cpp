#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include "appcontroller.h"
#include "filesavedialog.h"
#include "gapi.h"
#include "eidlib.h"
#include "singleapplication.h"

using namespace eIDMW;

int main(int argc, char *argv[])
{
    int retValue = 0;
    bool test_mode = false;
    const char * default_sam_server = NULL;

    SingleApplication app(argc, argv);

    // Set app icon
    app.setWindowIcon(QIcon(":/appicon.ico"));

    QQuickStyle::setStyle("Material");
    //QQuickStyle::setStyle("Universal");
    //QQuickStyle::setStyle("Default");

    QQmlApplicationEngine engine;

    // GUISettings init
    GUISettings settings;
    // AppController init
    AppController controller(settings);

    PTEID_InitSDK();
    PTEID_Config sam_server(PTEID_PARAM_GENERAL_SAM_SERVER);

    if (argc == 2 && strcmp(argv[1], "-test") == 0)
    {
        test_mode = true;
        default_sam_server = strdup(sam_server.getString());
        sam_server.setString("pki.teste.cartaodecidadao.pt:443");
        settings.setTestMode( test_mode );
        if(test_mode)
            qDebug() << "Starting App in test mode";
    }
    else
    {
        //Force production mode
        sam_server.setString("pki.cartaodecidadao.pt:443");
    }

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

    // Each starting instance will make the running instance to restore.
    QObject::connect(
        &app,
        &SingleApplication::instanceStarted,
        &controller,
        &AppController::restoreScreen);

    retValue = app.exec();

    PTEID_ReleaseSDK();

    return retValue;
}
