/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

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
    const char * default_sam_server = NULL;

	SingleApplication app(argc, argv);

    PTEID_InitSDK();

	// GUISettings init
	GUISettings settings;
	// AppController init
	AppController controller(settings);

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
        PTEID_Config::SetTestMode(true);
        default_sam_server = strdup(sam_server.getString());
        sam_server.setString("pki.teste.cartaodecidadao.pt:443");
        settings.setTestMode( true );
        qDebug() << "Starting App in test mode";
    }
    else
    {
        //Force production mode
        PTEID_Config::SetTestMode(false);
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
