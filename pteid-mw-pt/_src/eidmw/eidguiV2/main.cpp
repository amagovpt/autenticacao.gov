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
#include <QDesktopWidget> 

using namespace eIDMW;

int main(int argc, char *argv[])
{
    int retValue = 0;
    bool test_mode = false;
    const char * default_sam_server = NULL;
	
    // GUISettings init
    GUISettings settings;
    // AppController init
    AppController controller(settings);

    double scale = 1;
    int scaleSetting = settings.getApplicationScale();
    bool useSystemScale = settings.getUseSystemScale();
    if (!useSystemScale)
    {
        scale = 1 + 0.25 * scaleSetting; // scale levels are in 25% increments
    }
    else
    {
        //QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); //Uncomment after migration to Qt 5.12
        QApplication temp(argc, argv);
#ifdef WIN32
        // Delete this block after migration to Qt 5.12
        // QApplication::desktop() requires the creation of a QApplication first, so a temp is created. 
        // The setting of the env var must be done before the application instance is contructed
        scale = QApplication::desktop()->logicalDpiX() / 96.0; // Default Windows PPI

        // set scale setting to the system scale so it can be displayed in the application settings
        // convert to scale levels used in the application settings
        int scaleLevel = round((scale - 1) / 0.25);
        settings.setApplicationScale(scaleLevel);
#else
        // System scale in application can only be configured in Windows
#endif
    }
    std::string scaleAsString = std::to_string(scale);
    QByteArray scaleAsQByteArray(scaleAsString.c_str(), (int)scaleAsString.length());
    qDebug() << "Application scaling: " << scale;
    if(!qputenv("QT_SCALE_FACTOR", scaleAsQByteArray))
        qDebug() << "Erro QT_SCALE_FACTOR";

	SingleApplication app(argc, argv);

    PTEID_InitSDK();


	if (settings.getGraphicsAccel()){
		qDebug() << "C++: Starting App with graphics acceleration";
    }else{
		qDebug() << "C++: Starting App without graphics acceleration";
		app.setAttribute(Qt::AA_UseSoftwareOpenGL);
	}

	// Set app icon
	app.setWindowIcon(QIcon(":/appicon.ico"));

	QQuickStyle::setStyle("Material");
	//QQuickStyle::setStyle("Universal");
	//QQuickStyle::setStyle("Default");

	QQmlApplicationEngine engine;

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

    // Load translation files
    gapi.initTranslation();
    controller.initTranslation();

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
