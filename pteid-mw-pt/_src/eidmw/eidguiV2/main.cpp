/*-****************************************************************************

 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <QApplication>
#include <QDateTime>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include <QProcess>
#include <QFontDatabase>
#include "appcontroller.h"
#include "gapi.h"
#include "eidlib.h"
#include "singleapplication.h"
#include <QDesktopWidget>
#include <QCommandLineParser>
#include "pteidversions.h"
#include "LogBase.h"

#include <stdio.h>

#include <iostream>

using namespace eIDMW;

const char *signDescription =
	"Open Signature submenu with preset fields. PDF file argument list is mandatory.\n   --tsa\n    Enable timestamp "
	"option\n   -d,--destino=STRING\n    Set destination folder for signatures\n"
	"   -m,--motivo=STRING\n    Set the signature reason field\n   -l,--localidade=STRING\n    Set the signature "
	"location field\n";

int parseCommandlineAppArguments(QCommandLineParser *parser, GUISettings *settings) {

	QString modeDescription("Mode of the application. Possible values are:\n ");
	modeDescription.append("\"\" (empty): default mode\n");
	modeDescription.append("sign [OPTIONS] PDF_FILE...: ").append(signDescription);
	parser->addPositionalArgument("mode", modeDescription);

	const QCommandLineOption helpOption(QStringList() << "h" << "help", "Displays help on commandline options");
	const QCommandLineOption versionOption = parser->addVersionOption();
	parser->addOption(helpOption);

	// Graphics rendering options
	const QCommandLineOption softwareModeOption(QStringList() << "s" << "software",
												"Graphics rendering with Software (OpenGL)");
	parser->addOption(softwareModeOption);
	const QCommandLineOption hardwareModeOption(QStringList() << "c" << "card",
												"Graphics rendering with Hardware (Video card)");
	parser->addOption(hardwareModeOption);
#ifdef WIN32
	const QCommandLineOption direct3dModeOption(QStringList() << "w" << "direct3d",
												"Graphics rendering with Software (Direct3D)");
	parser->addOption(direct3dModeOption);
#endif
	const QCommandLineOption testModeOption(
		QStringList() << "t" << "test",
		"Enable test mode. It accepts only test cards and uses address reading test server");
	parser->addOption(testModeOption);

	parser->parse(QCoreApplication::arguments());

	// Default is production mode
	PTEID_Config::SetTestMode(false);

	if (parser->isSet(testModeOption)) {
		PTEID_Config::SetTestMode(true);
		settings->setTestMode(true);
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "Starting App in test mode");
	}

	if (parser->isSet(versionOption)) {
		parser->showVersion();
	}

	if (parser->isSet(helpOption)) {
		parser->showHelp();
	}

	int tGraphicsAccel = settings->getGraphicsAccel();
	if (parser->isSet(hardwareModeOption)) {
		if (tGraphicsAccel != OPENGL_HARDWARE) {
			settings->setAccelGraphics(OPENGL_HARDWARE);
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui",
					  "Command line option set OPENGL_HARDWARE. Current option : %d", tGraphicsAccel);
			return RESTART_EXIT_CODE;
		}
	} else if (parser->isSet(softwareModeOption)) {
		if (tGraphicsAccel != OPENGL_SOFTWARE) {
			settings->setAccelGraphics(OPENGL_SOFTWARE);
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui",
					  "Command line option set OPENGL_SOFTWARE. Current option : %d", tGraphicsAccel);
			return RESTART_EXIT_CODE;
		}
	}
#ifdef WIN32
	else if (parser->isSet(direct3dModeOption)) {
		qDebug() << "C++:  Command line option to OPENGL_DIRECT3D";
		if (tGraphicsAccel != OPENGL_DIRECT3D) {
			settings->setAccelGraphics(OPENGL_DIRECT3D);
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui",
					  "Command line option set OPENGL_DIRECT3D. Current option : %d", tGraphicsAccel);
			return RESTART_EXIT_CODE;
		}
	}
#endif
	return SUCCESS_EXIT_CODE;
}

FILE *handleSTDErr() {
	std::wstring location;
	eIDMW::CLogger::instance().getFileFromStdErr(location);
	QString path = QString::fromWCharArray(location.c_str());

	FILE *file = NULL;

	file = freopen(path.toStdString().c_str(), "a", stderr);

	std::cerr << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ").toStdString() << "stderr start"
			  << std::endl;
	return file;
}

void parseCommandlineGuiArguments(QCommandLineParser *parser, GAPI *gapi) {

	QStringList args = parser->positionalArguments();
	const QString mode = args.isEmpty() ? QString() : args.first();
	if (mode == "") {
		if (!parser->parse(QCoreApplication::arguments())) {
			qDebug() << "ERROR: default no-arguments mode: " << parser->errorText().toStdString().c_str();
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "parseCommandlineGuiArguments: default no-arguments mode: %s",
					  parser->errorText().toStdString().c_str());
			return;
		}
	}
	// sign is the correct option. signAdvanced and signSimple are legacy options
	else if (mode == "sign" || mode == "signAdvanced" || mode == "signSimple") {
		parser->clearPositionalArguments();
		parser->addPositionalArgument(mode, signDescription);
		QString inputDescription("File  or list of files to be loaded for signing.");
		parser->addPositionalArgument("input", inputDescription);

		const QCommandLineOption timestampingOption("tsa", "Check timestamping");
		const QCommandLineOption reasonOption(QStringList() << "m" << "motivo", "Set default reason", "reason");
		const QCommandLineOption locationOption(QStringList() << "l" << "localidade", "Set default location",
												"location");

		parser->addOption(timestampingOption);
		parser->addOption(reasonOption);
		parser->addOption(locationOption);

		const QCommandLineOption outputOption(QStringList() << "d" << "destino", "Set output folder", "output");
		parser->addOption(outputOption);

		if (!parser->parse(QCoreApplication::arguments())) {
			qDebug() << "ERROR: " << mode << ": " << parser->errorText().toStdString().c_str();
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "parseCommandlineGuiArguments: %s: %s",
					  mode.toStdString().c_str(), parser->errorText().toStdString().c_str());
			return;
		}
		// some option values were interpreted as positional args before. call this again
		args = parser->positionalArguments();
		// Input Files
		if (args.size() < 2) {
			qDebug() << "ERROR: " << mode << ": " << "No input files were provided.";
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "parseCommandlineGuiArguments: %s: No input files were provided.", mode.toStdString().c_str());
			return;
		}

		for (int i = 1; i < args.size(); i++) {
			if (QFileInfo::exists(args[i])) {
				qDebug() << "Adding file: " << args[i];
				gapi->addShortcutPath(args[i]);
			} else {
				qDebug() << "ERROR: Cannot load file for signature. File/folder does not exist: " << args[i];
				PTEID_LOG(
					eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui",
					"parseCommandlineGuiArguments: Cannot load file for signature. File/folder does not exist: %s",
					args[i].toStdString().c_str());
				return;
			}
		}
		// Shortcuts and options
		gapi->setShortcutFlag(GAPI::ShortcutIdSign);
		gapi->setShortcutTsa(parser->isSet(timestampingOption));
		gapi->setShortcutReason(parser->value(reasonOption));
		gapi->setShortcutLocation(parser->value(locationOption));

		// Output dir
		if (parser->isSet(outputOption) && !QFileInfo::exists(parser->value(outputOption))) {
			qDebug() << "ERROR: File/folder does not exist for output folder: " << parser->value(outputOption);
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "parseCommandlineGuiArguments: File/folder does not exist for output folder: %s",
					  parser->value(outputOption).toStdString().c_str());
			gapi->setShortcutFlag(GAPI::ShortcutIdNone);
			return;
		}
		gapi->setShortcutOutput(parser->value(outputOption));
	} else {
		qDebug() << "ERROR: Unknown mode: " << mode;
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "parseCommandlineGuiArguments: Unknown mode: %s",
				  mode.toStdString().c_str());
		return;
	}
}

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	QString src_file = context.file ? context.file : "";
	PTEID_LogLevel msgLvl;

	switch (type) {
	case QtDebugMsg:
		msgLvl = eIDMW::PTEID_LOG_LEVEL_DEBUG;
		break;
	case QtInfoMsg:
		msgLvl = eIDMW::PTEID_LOG_LEVEL_INFO;
		break;
	case QtWarningMsg:
		msgLvl = eIDMW::PTEID_LOG_LEVEL_WARNING;
		break;
	case QtFatalMsg:
	case QtCriticalMsg:
	default:
		msgLvl = eIDMW::PTEID_LOG_LEVEL_CRITICAL;
		break;
	}

	QString message = src_file.isEmpty() ? msg : QString("%1:%2 - %3").arg(src_file).arg(context.line).arg(msg);

	PTEID_LOG(msgLvl, "QT-messages", message.toStdString().c_str());
}

int main(int argc, char *argv[]) {
	int retValue = SUCCESS_EXIT_CODE;

	// GUISettings init
	GUISettings settings;
	AppController::initApplicationScale();

	// AppController init
	AppController controller(settings);

	int tGraphicsAccel = settings.getGraphicsAccel();
	if (tGraphicsAccel == OPENGL_SOFTWARE) {
		qDebug() << "C++: Starting App with software graphics acceleration";
		QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
	}
#ifdef WIN32
	else if (tGraphicsAccel == OPENGL_DIRECT3D) {
		qDebug() << "C++: Starting App with ANGLE emulation for OpenGL";
		QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
	}
#endif
	else {
		qDebug() << "C++: Starting App with hardware graphics acceleration";
	}

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "eidgui", "OpenGL option : %d", tGraphicsAccel);

	FILE *errFile = handleSTDErr();
	SingleApplication app(argc, argv);
	app.setOrganizationName("Portuguese State");
	// Needs to be defined before commandline arguments are processed for --version
	app.setApplicationVersion(settings.getGuiVersion() + " - " + REVISION_NUM_STRING + " [ " + REVISION_HASH_STRING +
							  " ]");

	qInstallMessageHandler(customMessageHandler);

	// Parse command line arguments
	QCommandLineParser parser;
	retValue = parseCommandlineAppArguments(&parser, &settings);

	if (retValue == SUCCESS_EXIT_CODE) {

		PTEID_InitSDK();

		// Set app icon
		app.setWindowIcon(QIcon(":/appicon.ico"));

		// GAPI init
		GAPI gapi(settings);
		GAPI::declareQMLTypes();

		parseCommandlineGuiArguments(&parser, &gapi);

		QQuickStyle::setStyle("Material");
		// QQuickStyle::setStyle("Universal");
		// QQuickStyle::setStyle("Default");

		QQmlApplicationEngine *engine = new QQmlApplicationEngine();

		// Embedding C++ Objects into QML with Context Properties
		QQmlContext *ctx = engine->rootContext();
		ctx->setContextProperty("gapi", &gapi);
		ctx->setContextProperty("controler", &controller);
		ctx->setContextProperty("image_provider_pdf", gapi.image_provider_pdf);

		engine->addImageProvider("myimageprovider", gapi.buildImageProvider());
		engine->addImageProvider("pdfpreview_imageprovider", gapi.buildPdfImageProvider());

		gapi.storeQmlEngine(engine);

		// Load translation files
		gapi.initTranslation();
		controller.initTranslation();
#ifdef __linux__
		int font_id = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath() +
														"/../share/pteid-mw/fonts/Lato-Regular.ttf");
		qDebug() << "Font id: " << font_id;
#endif

		// Load main QML file
		engine->load(QUrl(MAIN_QML_PATH));

		// Each starting instance will make the running instance to restore.
		QObject::connect(&app, &SingleApplication::instanceStarted, &controller, &AppController::restoreScreen);

		retValue = app.exec();

		PTEID_ReleaseSDK();
	}

	if (retValue == RESTART_EXIT_CODE) {
		QProcess proc;
		QString program = QCoreApplication::arguments().at(0);
		QStringList arguments;
		if (QCoreApplication::arguments().size() > 1) {
			arguments = QCoreApplication::arguments();
			arguments.removeFirst();
		}
		if (!proc.startDetached(program, arguments)) {
			qDebug() << "Error restarting application: could not start process.";
		}
	}
	if (errFile != NULL) {
		fclose(errFile);
	}

	return retValue;
}
