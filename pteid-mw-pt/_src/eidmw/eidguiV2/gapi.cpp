/*-****************************************************************************

 * Copyright (C) 2017-2022 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2018-2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2019-2022 José Pinto - <jose.pinto@caixamagica.pt>
 * Copyright (C) 2023 Nuno Romeu Lopes - <nuno.lopes@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "gapi.h"
#include <QString>
#include <QVector>
#include <QDate>
#include <QRegExp>
#include <cstdio>
#include <QQuickImageProvider>
#include <QDesktopServices>
#include <QPrinter>
#include <QPrinterInfo>
#include <QWindow>
#include <QUuid>
#include <QProcess>
#include <QtGlobal>
#include "qpainter.h"

#include "CMDSignature.h"
#include "cmdErrors.h"
#include "SigContainer.h"

// SCAP
#include "scapsettings.h"
#include "scapclient.h"
#include "scaperrors.h"
#include "scapcache.h"
#include "pdfsignatureutils.h"

#include "stdsoap2.h"
#include "credentials.h"
#include "eidguiV2Credentials.h"
#include "Config.h"
#include "Util.h"
#include "StringOps.h"
#include "MiscUtil.h"
#include "CurlUtil.h"
#include "proxyinfo.h"
#include "concurrent.h"

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#include "pteidversions.h"

#include <curl/curl.h>

#include <algorithm>

using namespace eIDMW;

#define TRIES_LEFT_ERROR 1000
#define TRIES_LEFT_MAX 3

static bool g_cleaningCallback = false;
static int g_runningCallback = 0;

/*
	GAPI - Graphic Application Programming Interface
*/

//
// The write callback function to curl requests. At the moment this functions
// does nothing other than return the ammount of bytes. This allows
//
size_t GAPI::write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	Q_UNUSED(ptr);
	Q_UNUSED(userdata);
	return size * nmemb;
}

bool GAPI::isMacOSApplicationFolder() const
{
#ifdef __APPLE__
	QString currentAppFolder = QCoreApplication::applicationDirPath();
	QString executableInsidePackage = "/Contents/MacOS";
	QString macOSInstallationPath = "/Applications/Autenticação.gov.app";
	currentAppFolder.chop(executableInsidePackage.length());
	return currentAppFolder == macOSInstallationPath;
#else
	return false;
#endif
}

const char *GAPI::telemetryActionToString(TelemetryAction action) {
	switch (action) {
	case TelemetryAction::Startup:
		return "app/startup/";
	case TelemetryAction::Accepted:
		return "app/accepted/";
	case TelemetryAction::Denied:
		return "app/denied/";
	case TelemetryAction::SignCC:
		return "app/sign/cc/";
	case TelemetryAction::SignCMD:
		return "app/sign/cmd/";
	case TelemetryAction::SignCCScap:
		return "app/sign/cc/scap/";
	case TelemetryAction::SignCMDScap:
		return "app/sign/cmd/scap/";
	case TelemetryAction::PrintPDF:
		return "app/printpdf/";
	default:
		return "app/unknown/";
	}
}

#ifdef __APPLE__
// Uses sysctl to find if the process is being translated by Rosetta
int processIsTranslated() {
	int ret = 0;
	size_t size = sizeof(ret);
	if (sysctlbyname("sysctl.proc_translated", &ret, &size, NULL, 0) == -1) {
		if (errno == ENOENT)
			return 0;
		return -1;
	}
	return ret;
}
#endif

// Obtains the system Architecture
QString getSystemArchitecture() {
	QString system_architecture = QSysInfo::currentCpuArchitecture().toStdString().c_str();

#ifdef __APPLE__
	system_architecture = processIsTranslated() ? "arm64" : "x86_64";
#endif

	return system_architecture;
}

GAPI::TelemetryStatus GAPI::getTelemetryStatus() {
	return static_cast<TelemetryStatus>(m_Settings.getTelemetryStatus());
}

void GAPI::setTelemetryStatus(TelemetryStatus status) { m_Settings.setTelemetryStatus(static_cast<long>(status)); }

void GAPI::enableTelemetry() {
	if (getTelemetryStatus() == TelemetryStatus::Enabled)
		return;

	setTelemetryStatus(TelemetryStatus::RetryEnable);
	Concurrent::run(this, &GAPI::doUpdateTelemetry, TelemetryAction::Accepted);
}

void GAPI::disableTelemetry() {
	const auto tel_status = getTelemetryStatus();
	// If we never sent an "accepted" update sucessefully, we don't need to send a "denied/disabled" update
	if (tel_status == TelemetryStatus::RetryEnable || tel_status == TelemetryStatus::Disabled) {
		m_Settings.setTelemetryStatus(TelemetryStatus::Disabled);
		return;
	}

	m_Settings.setTelemetryStatus(TelemetryStatus::RetryDisable); // trying to send denied telemetry action
	Concurrent::run(this, &GAPI::doUpdateTelemetry, TelemetryAction::Denied);
}

QString getOSName() {

#ifdef __linux__
	// Access the os-release file of the host OS
	QFile file("/run/host/os-release");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return QSysInfo::prettyProductName();
	}

	QTextStream in(&file);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine();
		if (line.startsWith("PRETTY_NAME=")) {
			// Remove the initial part of the line and leading/trailing quotes
			QString prettyName = line.mid(QString("PRETTY_NAME=").length()).trimmed();
			if (prettyName.startsWith('"') && prettyName.endsWith('"')) {
				prettyName = prettyName.mid(1, prettyName.length() - 2);
			}
			return prettyName;
		}
	}
	return "Linux (non-conforming distro)";
#else
	return QSysInfo::prettyProductName();
#endif
}

//
// Telemetry is updated by making requests to pre-defined endpoints on a
// remote server.
//
void GAPI::doUpdateTelemetry(TelemetryAction action) {
	// Request timeout in seconds
	const long TLMT_REQUEST_TIMEOUT = 5;
	//
	// Create a new guid once per system.
	//
	const auto telemetry_id = m_Settings.getTelemetryId();
	if (telemetry_id.compare("0") == 0) {
		const auto new_telemetry_id = QUuid::createUuid().toString(QUuid::Id128);
		m_Settings.setTelemetryId(new_telemetry_id);
	}

	const auto telemetry_host = m_Settings.getTelemetryHost();
	if (!telemetry_host.startsWith("http")) {
		qDebug() << "Wrong value for telemetry_host config! It should begin with http(s) protocol.";
	}

	//
	// Initiate curl
	//
	auto curl = curl_easy_init();
	if (curl) {
		//
		// The endpoint URL represents the action performed by the client
		// URL : protocol://<hostname>/<action>/
		//
		QString url = telemetry_host;
		// Support telemetry_host without trailing slash
		if (!url.endsWith('/')) {
			url += '/';
		}
		url += telemetryActionToString(action);
		url += "?tel_id=";
		url += m_Settings.getTelemetryId();
		std::string url_std_string = url.toStdString();
		curl_easy_setopt(curl, CURLOPT_URL, url_std_string.c_str());

		// Obtains System Architecture
		g_systemArchitecture = (g_systemArchitecture.isEmpty()) ? getSystemArchitecture() : g_systemArchitecture;

		// Create user-agent header with the following structure
		// AutenticacaoGov/<version> (<OS Name> <OS version> <OS System Architecture>)
		QString user_agent = QString(TEL_APP_USER_AGENT) + PTEID_PRODUCT_VERSION + " (" +
							 getOSName().toStdString().c_str() + " " + g_systemArchitecture + ")";
		curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.toStdString().c_str());
		// Validate TLS server certificate using our certificate bundle
		std::string cacerts_file =
			utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR)) + "/cacerts.pem";
		curl_easy_setopt(curl, CURLOPT_CAINFO, cacerts_file.c_str());

		//
		// Custom writefunction callback to pipe the request result so
		// it does not print to the console/log
		//
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		// We shouldn't block the telemetry request thread for too long
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, TLMT_REQUEST_TIMEOUT);

		bool using_proxy = applyProxyConfigToCurl(curl, url_std_string);

		curl_easy_perform(curl);

		//
		// Retrieve response status code
		//
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		qDebug() << "updateTelemetry HTTP status:" << http_code << "for URL:" << url << "Using proxy?: " << using_proxy;
		if (action == TelemetryAction::Accepted || action == TelemetryAction::Denied) {
			setTelemetryStatus(
				http_code == 200
					? (action == TelemetryAction::Accepted ? TelemetryStatus::Enabled : TelemetryStatus::Disabled)
					: (action == TelemetryAction::Accepted ? TelemetryStatus::RetryEnable
														   : TelemetryStatus::RetryDisable));
		}

		//
		// Cleanup curl
		//
		curl_easy_cleanup(curl);
		curl = NULL;
	}
}

//
// Telemetry updates are non-blocking, does not produce any logs nor do they
// throw any exceptions. They are completly silent for both the developer and
// user.
//
void GAPI::updateTelemetry(TelemetryAction action) {
	const auto tel_status = getTelemetryStatus();
	if (tel_status == TelemetryStatus::Enabled)
		//
		// Run telemetry updates on different thread so it does not block the GUI
		// thread no matter how long the request manages to take.
		//
		Concurrent::run(this, &GAPI::doUpdateTelemetry, action);
}

static std::string get_scap_app_id() {
	PTEID_Config app_id_config(PTEID_PARAM_GENERAL_SCAP_APPID);
	std::string appid = app_id_config.getString();

	if (appid.empty()) {
		appid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
		app_id_config.setString(appid.c_str());
	}

	return appid;
}

GAPI::GAPI(GUISettings &settings, QObject *parent) : QObject(parent), m_Settings(settings) {
	image_provider = new PhotoImageProvider();
	image_provider_pdf = new PDFPreviewImageProvider();
	m_mainWnd = NULL;
	m_qml_engine = NULL;

	// Get the configured CMD account details
	std::string cmd_user_id = CMDCredentials::getCMDBasicAuthUserId(EIDGUIV2_CMD_BASIC_AUTH_USERID);
	std::string cmd_password = CMDCredentials::getCMDBasicAuthPassword(EIDGUIV2_CMD_BASIC_AUTH_PASSWORD);
	std::string cmd_app_id = CMDCredentials::getCMDBasicAuthAppId(EIDGUIV2_CMD_BASIC_AUTH_APPID);

	PTEID_CMDSignatureClient::setCredentials(cmd_user_id.c_str(), cmd_password.c_str(), cmd_app_id.c_str());

#ifdef WIN32
	m_cmdCertificates = new eIDMW::CMDCertificates(cmd_user_id.c_str(), cmd_password.c_str(), cmd_app_id.c_str());
#endif
	m_addressLoaded = false;
	m_shortcutFlag = ShortcutIdNone;

	// Create callbacks for all readers at the startup
	setEventCallbacks();

	m_cmd_client = new PTEID_CMDSignatureClient();
	m_cmd_client->setMobileNumberCaching(true);

	//----------------------------------
	// set a timer to check if the number of card readers is changed
	//----------------------------------
	m_timerReaderList = new QTimer(this);
	connect(m_timerReaderList, SIGNAL(timeout()), this, SLOT(updateReaderList()));
	m_timerReaderList->start(TIMERREADERLIST);

	ScapCredentials scap_credentials = {SCAP_BASIC_AUTH_USERID, SCAP_BASIC_AUTH_PASSWORD, get_scap_app_id()};
	m_scap_client = new ScapClient(scap_credentials);
}

void GAPI::initTranslation() {

	QString appPath = QCoreApplication::applicationDirPath();
	m_Settings.setExePath(appPath);
	QString CurrLng = m_Settings.getGuiLanguageString();
	if (LoadTranslationFile(CurrLng) == false) {
		emit signalLanguageChangedError();
	}
}

QString GAPI::loadCountryName(const QString &threeLetterCode, const QString &language) {
	QString filePath = ":/country_codes.json";
	QFile file;
	QString useLanguage = language == "nl" ? "pt" : "en";
	file.setFileName(filePath);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QString val = file.readAll();
	file.close();
	QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
	QJsonObject jsonObj = doc.object();
	QJsonValue threeLetterTranslation = jsonObj.value(threeLetterCode);
	if (threeLetterTranslation == QJsonValue::Undefined)
		return threeLetterCode;

	QJsonObject threeLetterTranslationObj = threeLetterTranslation.toObject();
	QJsonValue valueTranslated = threeLetterTranslationObj.value(useLanguage);
	if(valueTranslated == QJsonValue::Undefined)
		return threeLetterTranslationObj.value("en").toString();

	return valueTranslated.toString();
}

bool GAPI::LoadTranslationFile(QString NewLanguage) {
	QString strTranslationFile;
	QString translations_dir;
	strTranslationFile = QString("eidmw_") + NewLanguage;

#ifdef __APPLE__
	translations_dir = m_Settings.getExePath() + "/../Resources/";
#else
	translations_dir = m_Settings.getExePath() + "/";
#endif

	qDebug() << "C++: GAPI LoadTranslationFile" << strTranslationFile << translations_dir;

	if (!m_translator.load(strTranslationFile, translations_dir)) {
		// this should not happen, since we've built the menu with the translation filenames
		strTranslationFile = QString("eidmw_") + STR_DEF_GUILANGUAGE;
		// try load default translation file
		qDebug() << "C++: GAPI LoadTranslationFile" << strTranslationFile << translations_dir;
		if (!m_translator.load(strTranslationFile, translations_dir)) {
			// this should not happen too, since we've built the menu with the translation filenames
			qDebug() << "C++: GAPI Load Default Translation File Error";
			return false;
		}
		qDebug() << "C++: GAPI Loaded Default Translation File";
		qApp->installTranslator(&m_translator);
		return false;
	}
	//------------------------------------
	// install the translator object and load the .qm file for
	// the given language.
	//------------------------------------
	qApp->installTranslator(&m_translator);
	return true;
}

WindowGeometry *GAPI::getWndGeometry() {
#ifdef WIN32
	return NULL;
#else
	if (m_mainWnd == NULL) {
		qDebug() << "C++: GAPI getWndGeometry: Failed to get reference to main window";
		return NULL;
	}

	m_wndGeometry.x = m_mainWnd->geometry().x();
	m_wndGeometry.y = m_mainWnd->geometry().y();
	m_wndGeometry.width = m_mainWnd->geometry().width();
	m_wndGeometry.height = m_mainWnd->geometry().height();

	return &m_wndGeometry;
#endif
}

QString GAPI::getDataCardIdentifyValue(IDInfoKey key) { return m_data[key]; }

QString GAPI::getDataICAOValue(ICAOInfoKey key) { return m_icaoData[key];}

void GAPI::setDataCardIdentify(QMap<IDInfoKey, QString> data) {

	qDebug() << "C++: setDataCardIdentify ";

	m_data = data;
	emit signalCardDataChanged();
}

void GAPI::setDataCardICAO(QMap<ICAOInfoKey, QString>  data){
	qDebug() << "C++: setDataCardICAO ";

	m_icaoData = data;
	emit signalICAODataChanged();
}

bool isExpiredDate(const char *strDate) {
	if (strDate == NULL)
		return false;

	QDate qDate = QDate::fromString(strDate, "dd MM yyyy");
	QDate curDate = QDate::currentDate();

	if (curDate > qDate)
		return true;
	return false;
}

QString GAPI::getAddressField(AddressInfoKey key) { return m_addressData[key]; }

#define BEGIN_TRY_CATCH try {

#define END_TRY_CATCH                                                                                                  \
	}                                                                                                                  \
	catch (PTEID_ExNoReader &) {                                                                                       \
		qDebug() << "No card reader found !";                                                                          \
		emit signalCardAccessError(NoReaderFound);                                                                     \
	}                                                                                                                  \
	catch (PTEID_ExNoCardPresent &) {                                                                                  \
		qDebug() << "No card present.";                                                                                \
		emit signalCardAccessError(NoCardFound);                                                                       \
	}                                                                                                                  \
	catch (PTEID_ExBatchSignatureFailed & e) {                                                                         \
		qDebug() << "Batch signature failed at index: " << e.GetFailedSignatureIndex();                                \
		emitErrorSignal(__FUNCTION__, e.GetError(), e.GetFailedSignatureIndex());                                      \
	}                                                                                                                  \
	catch (PTEID_Exception & e) {                                                                                      \
		emitErrorSignal(__FUNCTION__, e.GetError());                                                                   \
	}

void GAPI::handleRemoteAddressErrors(long errorCode) {
	if (errorCode == EIDMW_REMOTEADDR_CONNECTION_ERROR) {
		emit signalRemoteAddressError(AddressConnectionError);
	} else if (errorCode == EIDMW_REMOTEADDR_SERVER_ERROR) {
		emit signalRemoteAddressError(AddressServerError);
	} else if (errorCode == EIDMW_REMOTEADDR_CONNECTION_TIMEOUT) {
		emit signalRemoteAddressError(AddressConnectionTimeout);
	} else if (errorCode == EIDMW_REMOTEADDR_SMARTCARD_ERROR) {
		emit signalRemoteAddressError(AddressSmartcardError);
	} else if (errorCode == EIDMW_REMOTEADDR_CERTIFICATE_ERROR) {
		emit signalRemoteAddressError(AddressServerCertificateError);
	} else if (errorCode == EIDMW_REMOTEADDR_EXPIRED || errorCode == EIDMW_REMOTEADDR_REVOKED) {
		emit signalRemoteAddressError(CardCertificateError);
	} else if (errorCode == EIDMW_REMOTEADDR_INVALID_STATE) {
		emit signalRemoteAddressError(AddressInvalidStateError);
	} else if (errorCode == EIDMW_REMOTEADDR_UNKNOWN_ERROR) {
		emit signalRemoteAddressError(AddressUnknownError);
	}
}

void GAPI::emitErrorSignal(const char *caller_function, long errorCode, int index) {
	// The SOD-related error codes have values in a contiguous range
	if (errorCode >= EIDMW_SOD_UNEXPECTED_VALUE && errorCode <= EIDMW_SOD_ERR_ACTIVE_AUTHENTICATION) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "SOD exception! Error code (see strings in eidErrors.h): %08lx\n",
				  errorCode);
		emit signalCardAccessError(SodCardReadError);
	} else if (errorCode == EIDMW_TIMESTAMP_ERROR) {
		emit signalPdfSignSuccess(SignMessageTimestampFailed);
	} else if (errorCode == EIDMW_LTV_ERROR) {
		emit signalPdfSignSuccess(SignMessageLtvFailed);
	} else if (errorCode == EIDMW_PERMISSION_DENIED) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Permission denied error in %s", caller_function);
		emit signalPdfSignFail(SignFilePermissionFailed, index);
	} else if (errorCode == EIDMW_PDF_INVALID_ERROR) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "PDF invalid error in %s", caller_function);
		emit signalPdfSignFail(PDFFileInvalid, index);
	} else if (errorCode == EIDMW_PDF_UNSUPPORTED_ERROR) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "PDF unsupported error in %s", caller_function);
		emit signalPdfSignFail(PDFFileUnsupported, index);
	} else if (errorCode == EIDMW_ERR_PIN_CANCEL) {
		emit signalCardAccessError(CardUserPinCancel);
	} else if (errorCode == EIDMW_ERR_PIN_BLOCKED) {
		emit signalCardAccessError(PinBlocked);
	} else if (errorCode == EIDMW_ERR_INCOMPATIBLE_READER) {
		emit signalCardAccessError(IncompatibleReader);
	} else if (errorCode == EIDMW_ERR_TIMEOUT) {
		emit signalCardAccessError(CardPinTimeout);
	} else if (errorCode == EIDMW_ERR_OP_CANCEL) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Operation cancelled by user");
		emit signalOperationCanceledByUser();
	} else if (errorCode >= EIDMW_REMOTEADDR_CONNECTION_ERROR && errorCode <= EIDMW_REMOTEADDR_UNKNOWN_ERROR) {
		handleRemoteAddressErrors(errorCode);
	} else if (errorCode == EIDMW_ERR_FILE_TOO_LONG) {
		emit signalPdfSignFail(TooLongFilename, index);
	} else if (errorCode == EIDMW_ERR_FILE_OP_FL_SYSTEM || errorCode == EIDMW_ERR_FILE_OP_FL_PROCESS) {
		emit signalPdfSignFail(TooManyOpenFiles, index);
	} else if (errorCode == EIDMW_ERR_FILE_NO_SPC) {
		emit signalPdfSignFail(FileNoSpace, index);
	} else if (errorCode == EIDMW_ERR_FILE_READ_ONLY) {
		emit signalPdfSignFail(FileReadOnly, index);
	} else {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "Generic eidlib exception in %s! Error code (see strings in eidErrors.h): %08lx", caller_function,
				  errorCode);
		/* Discard the 0xe1d0 prefix */
		unsigned long user_error = errorCode & 0x0000FFFF;
		QString msgError = QString("%1\n").arg(user_error);
		emit signalGenericError(msgError);
	}
}

void GAPI::getAddressFile() {
	qDebug() << "C++: getAddressFile()";
	bool m_foreign;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance getAddressFile");

	BEGIN_TRY_CATCH;

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	PTEID_Address &addressFile = card->getAddr();

	if (!addressFile.isNationalAddress()) {
		qDebug() << "Is foreign citizen";
		m_foreign = true;
		m_addressData[Foreigncountry] = QString::fromUtf8(addressFile.getForeignCountry());
		m_addressData[Foreignaddress] = QString::fromUtf8(addressFile.getForeignAddress());
		m_addressData[Foreigncity] = QString::fromUtf8(addressFile.getForeignCity());
		m_addressData[Foreignregion] = QString::fromUtf8(addressFile.getForeignRegion());
		m_addressData[Foreignlocality] = QString::fromUtf8(addressFile.getForeignLocality());
		m_addressData[Foreignpostalcode] = QString::fromUtf8(addressFile.getForeignPostalCode());
	} else {
		qDebug() << "Is national citizen";
		m_foreign = false;
		m_addressData[District] = QString::fromUtf8(addressFile.getDistrict());
		m_addressData[Municipality] = QString::fromUtf8(addressFile.getMunicipality());
		m_addressData[Parish] = QString::fromUtf8(addressFile.getCivilParish());
		m_addressData[Streettype] = QString::fromUtf8(addressFile.getStreetType());
		m_addressData[Streetname] = QString::fromUtf8(addressFile.getStreetName());
		m_addressData[Buildingtype] = QString::fromUtf8(addressFile.getBuildingType());
		m_addressData[Doorno] = QString::fromUtf8(addressFile.getDoorNo());
		m_addressData[Floor] = QString::fromUtf8(addressFile.getFloor());
		m_addressData[Side] = QString::fromUtf8(addressFile.getSide());
		m_addressData[Locality] = QString::fromUtf8(addressFile.getLocality());
		m_addressData[Place] = QString::fromUtf8(addressFile.getPlace());
		m_addressData[Zip4] = QString::fromUtf8(addressFile.getZip4());
		m_addressData[Zip3] = QString::fromUtf8(addressFile.getZip3());
		m_addressData[PostalLocality] = QString::fromUtf8(addressFile.getPostalLocality());
	}
	emit signalAddressLoaded(m_foreign);

	END_TRY_CATCH
}

void GAPI::doSaveCardPhoto(QString outputFile) {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	PTEID_ByteArray &photo = card->getID().getPhotoObj().getphoto();

	QImage image_photo;
	image_photo.loadFromData(photo.GetBytes(), photo.Size(), "PNG");

	bool success = image_photo.save(outputFile);
	emit signalSaveCardPhotoFinished(success);
}

// Only applicable to CC1
void GAPI::getPersoDataFile() {

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance getPersoDataFile");

	BEGIN_TRY_CATCH;

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	try {
		emit signalPersoDataLoaded(QString(card->readPersonalNotes()));
	} catch (PTEID_Exception &e) {
		if (e.GetError() == EIDMW_ERR_NOT_SUPPORTED) {
			signalPersonalDataNotSupported();
		} else
			throw;
	};

	END_TRY_CATCH
}

// Only applicable to CC1
void GAPI::setPersoDataFile(const QString &text) {
	qDebug() << "setPersoDataFile() called";

	int auth_pin_res = doVerifyAuthPin("");
	if (auth_pin_res == 0 || auth_pin_res == TRIES_LEFT_ERROR) {
		return;
	}

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);

	QByteArray notes = text.toUtf8();
	const unsigned char *p_notes = reinterpret_cast<const unsigned char *>(notes.constData());
	const PTEID_ByteArray ba_notes(p_notes, notes.size() + 1);

	try {
		card->writePersonalNotes(ba_notes);
	} catch (PTEID_Exception&) {
		qDebug() << "Error writing personal notes!";
		emit signalSetPersoDataFile(tr("STR_POPUP_ERROR"), tr("STR_PERSONAL_NOTES_ERROR"), false);
		return;
	}

	qDebug() << "Personal notes successfully written!";
	emit signalSetPersoDataFile(tr("STR_POPUP_SUCESS"), tr("STR_PERSONAL_NOTES_SUCESS"), true);
}

void GAPI::verifyAuthPin(QString pin_value) { Concurrent::run(this, &GAPI::doVerifyAuthPin, pin_value); }

unsigned int GAPI::doVerifyAuthPin(QString pin_value) {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doVerifyAuthPin");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &auth_pin = card->getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
	auth_pin.verifyPin(pin_value.toLatin1().data(), tries_left, true, getWndGeometry());

	if (tries_left == 0) {
		qDebug() << "WARNING: Auth PIN blocked!" << tries_left;
	}

	END_TRY_CATCH

	emit signalTestPinFinished(tries_left, AuthPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::getTriesLeftAuthPin() {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	if (!m_is_contactless || m_pace_auth_state == PaceAuthenticated) {
		Concurrent::run(this, &GAPI::doGetTriesLeftAuthPin);
	} else {
		performPACEWithCache(CardOperation::GetAuthPin);
	}
}
unsigned int GAPI::doGetTriesLeftAuthPin() {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doGetTriesLeftAuthPin");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &auth_pin = card->getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
	tries_left = auth_pin.getTriesLeft();

	if (tries_left == 0) {
		qDebug() << "WARNING: Auth PIN blocked!" << tries_left;
	}

	END_TRY_CATCH

	emit signalTriesLeftPinFinished(tries_left, AuthPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::verifySignPin(QString pin_value) { Concurrent::run(this, &GAPI::doVerifySignPin, pin_value); }
unsigned int GAPI::doVerifySignPin(QString pin_value) {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doVerifySignPin");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &sign_pin = card->getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
	sign_pin.verifyPin(pin_value.toLatin1().data(), tries_left, true, getWndGeometry());

	if (tries_left == 0) {
		qDebug() << "WARNING: Sign PIN blocked!" << tries_left;
	}

	END_TRY_CATCH

	emit signalTestPinFinished(tries_left, SignPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::getTriesLeftSignPin() {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	if (!m_is_contactless || m_pace_auth_state == PaceAuthenticated) {
		Concurrent::run(this, &GAPI::doGetTriesLeftSignPin);
	} else {
		performPACEWithCache(CardOperation::GetAuthPin);
	}
}
unsigned int GAPI::doGetTriesLeftSignPin() {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doGetTriesLeftSignPin");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &sign_pin = card->getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
	tries_left = sign_pin.getTriesLeft();

	if (tries_left == 0) {
		qDebug() << "WARNING: Sign PIN blocked!" << tries_left;
	}

	END_TRY_CATCH

	emit signalTriesLeftPinFinished(tries_left, SignPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::startPACEAuthentication(QString pace_can, CardOperation op) {
	Concurrent::run(this, &GAPI::doStartPACEAuthentication, pace_can, op);
}

void GAPI::doStartPACEAuthentication(QString pace_can, CardOperation op) {

	PTEID_EIDCard *card = NULL;
	ICAO_Card *cardIcao = NULL;
	BEGIN_TRY_CATCH

	getCardInstance(card);
	if (card == NULL) {
		if (CardOperation::ICAOData) {
			try {
				unsigned long ReaderCount = ReaderSet.readerCount();
				PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance Card Reader count =  %ld", ReaderCount);
				unsigned long ReaderIdx = 0;
				unsigned long tempReaderIndex = 0;
				ReaderCount = ReaderSet.readerCount();
				if (ReaderCount == 0) {
					return;
				}

				for (ReaderIdx = 0; ReaderIdx < ReaderCount; ReaderIdx++) {
					PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(ReaderIdx);
					if (readerContext.isCardPresent()) {
						tempReaderIndex = ReaderIdx;
						break;
					}
				}

				PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(tempReaderIndex);
				PTEID_CardType cardType = readerContext.getCardType();
				if (cardType != ICAO_CARDTYPE_MRTD)
					return;

				cardIcao = &readerContext.getICAOCard();

			} catch (PTEID_Exception err) {
				qDebug() << "Error while trying to create ICAO Card object! error: " << Qt::hex << err.GetError();
			}
		}
	}

	std::string can_str = pace_can.toStdString();
	try {
		if (card)
			card->initPaceAuthentication(can_str.c_str(), can_str.size(),
										 PTEID_CardPaceSecretType::PTEID_CARD_SECRET_CAN);
		else if (cardIcao)
			cardIcao->initPaceAuthentication(can_str.c_str(), can_str.size(),
											 PTEID_CardPaceSecretType::PTEID_CARD_SECRET_CAN); // or mrz
		emit signalPaceSuccess();
	} catch (PTEID_PACE_ERROR &e) {
		PaceError err;
		switch (e.GetError()) {
		case EIDMW_PACE_ERR_BAD_TOKEN: 
			err = PaceError::PaceBadToken;
			deleteCAN();
			break;
		case EIDMW_PACE_ERR_NOT_INITIALIZED:
			err = PaceError::PaceUnutilized;
			break;
		default:
			err = PaceError::PaceUnknown;
			break;
		}
		emit signalErrorPace(err);
		return;
	}

	m_pace_auth_state = PaceAuthenticated;
	// Cache correct CAN value
	if (m_Settings.getEnablePteidCANCache()) {
		saveCAN(can_str.c_str());
	}

	switch (op) {
	case IdentityData:
		finishLoadingCardData(card);
		break;
	case SignCertificateData:
		finishLoadingSignCertData(card);
		break;
	case ValidateCertificate:
		validateCertificates();
		break;
	case ReadCertDetails:
		startfillCertificateList();
		break;
	case DoAddress:
		verifyAddressPin("", false);
		break;
	case GetAuthPin:
		getTriesLeftAuthPin();
		break;
	case GetSignPin:
		getTriesLeftSignPin();
		break;
	case GetAddressPin:
		getTriesLeftAddressPin();
		break;
	case ICAOData:
		connectToICAOCard();
		break;
	}
	END_TRY_CATCH
}

void GAPI::verifyAddressPin(QString pin_value, bool forceVerify) {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);

	if (!m_is_contactless || m_pace_auth_state == PaceAuthenticated) {
		Concurrent::run(this, &GAPI::doVerifyAddressPin, pin_value, forceVerify);
	} else {
		performPACEWithCache(CardOperation::DoAddress);
	}
}

unsigned int GAPI::doVerifyAddressPin(QString pin_value, bool forceVerify) {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doVerifyAddressPin forceVerify: %s",
			  forceVerify ? "yes" : "no");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &address_pin = card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
	if (!forceVerify && address_pin.isVerified()) {
		tries_left = TRIES_LEFT_MAX;
	} else {
		address_pin.verifyPin(pin_value.toLatin1().data(), tries_left, true, getWndGeometry());

		if (tries_left == 0) {
			qDebug() << "WARNING: Address PIN blocked!" << tries_left;
		}
	}

	END_TRY_CATCH

	emit signalTestPinFinished(tries_left, AddressPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::getTriesLeftAddressPin() {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	if (!m_is_contactless || m_pace_auth_state == PaceAuthenticated) {
		Concurrent::run(this, &GAPI::doGetTriesLeftAddressPin);
	} else {
		performPACEWithCache(CardOperation::GetAuthPin);
	}
}
unsigned int GAPI::doGetTriesLeftAddressPin() {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doGetTriesLeftAddressPin");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &address_pin = card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
	tries_left = address_pin.getTriesLeft();

	if (tries_left == 0) {
		qDebug() << "WARNING: Address PIN blocked!" << tries_left;
	}

	END_TRY_CATCH

	emit signalTriesLeftPinFinished(tries_left, AddressPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::changeAuthPin(QString currentPin, QString newPin) {
	Concurrent::run(this, &GAPI::doChangeAuthPin, currentPin, newPin);
}
unsigned int GAPI::doChangeAuthPin(QString currentPin, QString newPin) {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doChangeAuthPin");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &auth_pin = card->getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);
	auth_pin.changePin(currentPin.toLatin1().data(), newPin.toLatin1().data(), tries_left, "", true, getWndGeometry());

	if (tries_left == 0) {
		qDebug() << "WARNING: Auth PIN blocked!" << tries_left;
	}

	END_TRY_CATCH

	emit signalModifyPinFinished(tries_left, AuthPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::changeSignPin(QString currentPin, QString newPin) {
	Concurrent::run(this, &GAPI::doChangeSignPin, currentPin, newPin);
}
unsigned int GAPI::doChangeSignPin(QString currentPin, QString newPin) {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doChangeSignPin");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &sign_pin = card->getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);
	sign_pin.changePin(currentPin.toLatin1().data(), newPin.toLatin1().data(), tries_left, "", true, getWndGeometry());

	if (tries_left == 0) {
		qDebug() << "WARNING: Sign PIN blocked!" << tries_left;
	}

	END_TRY_CATCH

	emit signalModifyPinFinished(tries_left, SignPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::showSignCMDDialog(long error_code) {
	QString message;
	QString support_string = tr("STR_CMD_ERROR_MSG");
	QString urlLink = tr("STR_LINK_SUPPORT");

	switch (error_code) {
	case 0:
		message = tr("STR_CMD_SUCESS");
		break;
	case SOAP_EOF:
		message = tr("STR_CMD_TIMEOUT_ERROR");
		break;
	case EIDMW_ERR_CMD_INACTIVE_ACCOUNT:
		urlLink = tr("STR_URL_AUTENTICACAO_GOT_PT");
		message = tr("STR_CMD_GET_CERTIFICATE_ERROR");
		support_string = "";
		break;
	case HTTP_PROXY_AUTH_REQUIRED:
		message = tr("STR_CMD_PROXY_AUTH_ERROR");
		break;
	case SOAP_TCP_ERROR:
	case EIDMW_ERR_CMD_CONNECTION:
		urlLink = tr("STR_LINK_MANUAL_PROXY");
		message =
			tr("STR_CONNECTION_ERROR") + "<br><br>" + tr("STR_VERIFY_INTERNET") + "<br>" + tr("STR_PROXY_WARNING");
		break;
	case EIDMW_ERR_CMD_SERVICE:
		message = tr("STR_CMD_SERVICE_FAIL");
		break;
	case EIDMW_ERR_CMD_INVALID_CODE:
		message = tr("STR_CMD_INVALID_OTP");
		break;
	case EIDMW_ERR_CMD_INVALID_PIN:
		message = tr("STR_CMD_INVALID_PIN");
		break;
	case SOAP_ERR_INACTIVE_SERVICE:
		message = tr("STR_CMD_INACTIVE_SERVICE");
		break;
	case EIDMW_PERMISSION_DENIED:
		message = tr("STR_SIGN_FILE_PERMISSION_FAIL");
		break;
	case EIDMW_TIMESTAMP_ERROR:
		message = tr("STR_CMD_SUCESS") + " " + tr("STR_TIME_STAMP_FAILED");
		break;
	case EIDMW_LTV_ERROR:
		message = tr("STR_CMD_SUCESS") + " " + tr("STR_LTV_FAILED");
		break;
	case EIDMW_ERR_OP_CANCEL:
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "CMD signature was canceled by the user.", error_code);
		return;
	case EIDMW_ERR_FILE_TOO_LONG:
		message = tr("STR_SIGN_FILE_TOO_LONG_FILENAME");
		break;
	case EIDMW_ERR_FILE_OP_FL_SYSTEM:
	case EIDMW_ERR_FILE_OP_FL_PROCESS:
		message = tr("STR_SIGN_FILE_TOO_MANY_OPEN_FILES");
		break;
	case EIDMW_ERR_FILE_NO_SPC:
		message = tr("STR_SIGN_FILE_NO_SPACE");
		break;
	case EIDMW_ERR_FILE_READ_ONLY:
		message = tr("STR_SIGN_FILE_READ_ONLY");
		break;
	case EIDMW_ERR_CMD_CHANGE_PIN_AUTH:
		message = tr("STR_CMD_NEEDS_CHANGE_PIN");
		support_string = "";
		urlLink = tr("STR_URL_AUTENTICACAO_GOT_PT");
		break;
	case EIDMW_ERR_CMD_CHANGE_PIN_SIGN:
		message = tr("STR_CMD_MUST_CHANGE_PIN");
		support_string = "";
		urlLink = tr("STR_URL_AUTENTICACAO_GOT_PT");
		break;
	default:
		message = tr("STR_CMD_LOGIN_ERROR");
		break;
	}

	if (error_code != 0) {
		// If there is error show message screen
		if (error_code == EIDMW_TIMESTAMP_ERROR || error_code == EIDMW_LTV_ERROR) {
			signalUpdateProgressStatus(message);
		} else if (error_code == EIDMW_ERR_CMD_CONNECTION) {
			signalUpdateProgressStatus(tr("STR_POPUP_ERROR") + "!");
			signalShowMessage(message, urlLink);
		} else {
			message += "<br><br>" + support_string;
			signalUpdateProgressStatus(tr("STR_POPUP_ERROR") + "!");
			signalShowMessage(message, urlLink);
		}
	} else {
		// Success
		signalUpdateProgressStatus(message);
	}

	if (error_code == 0 || error_code == EIDMW_TIMESTAMP_ERROR || error_code == EIDMW_LTV_ERROR) {
		PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui", "Successful CMD signature - Returned error code: 0x%08x",
				  error_code);
		emit signalOpenFile();
	} else {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Failed CMD signature - Returned error code: 0x%08x", error_code);
	}

	emit signalSignatureFinished();
	qDebug() << "Show Sign CMD Dialog - Error code: " << error_code << "Message: " << message;
}

void GAPI::changeAddressPin(QString currentPin, QString newPin) {
	Concurrent::run(this, &GAPI::doChangeAddressPin, currentPin, newPin);
}
unsigned int GAPI::doChangeAddressPin(QString currentPin, QString newPin) {
	unsigned long tries_left = TRIES_LEFT_ERROR;
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doChangeAddressPin");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return TRIES_LEFT_ERROR;

	PTEID_Pin &address_pin = card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
	address_pin.changePin(currentPin.toLatin1().data(), newPin.toLatin1().data(), tries_left, "", true,
						  getWndGeometry());

	if (tries_left == 0) {
		qDebug() << "WARNING: Address PIN blocked!" << tries_left;
	}

	END_TRY_CATCH

	emit signalModifyPinFinished(tries_left, AddressPin);
	// QML default types don't include long
	return (unsigned int)tries_left;
}

void GAPI::addressChangeCallback(void *instance, int value) {
	qDebug() << "addressChangeCallback";
	GAPI *gapi = (GAPI *)(instance);
	gapi->signalUpdateProgressBar(value);
}

/*Store a pointer to our QMLEngine and register the signal that when triggered will register
  the main window as parent in pteidDialogs */
void GAPI::storeQmlEngine(QQmlApplicationEngine *engine) {
	m_qml_engine = engine;
	connect(m_qml_engine, SIGNAL(objectCreated(QObject *, const QUrl &)), this,
			SLOT(setAppAsDlgParent(QObject *, const QUrl &)));
}

void GAPI::doSignCMD(PTEID_PDFSignature &pdf_signature, SignParams &signParams) {

	long ret = -1;

	PTEID_CMDSignatureClient *client = m_cmd_client;

	const int page = signParams.page;
	const double coord_x = signParams.coord_x;
	const double coord_y = signParams.coord_y;
	const std::string location = signParams.location.toStdString();
	const std::string reason = signParams.reason.toStdString();
	const std::string outputFile = signParams.outputFile.toStdString();

	try {
		ret = client->SignPDF(pdf_signature, page, coord_x, coord_y, location.c_str(), reason.c_str(),
							  outputFile.c_str());
	} catch (PTEID_Exception &e) {
		ret = e.GetError();

		if (ret == EIDMW_ERR_OP_CANCEL) {
			emit signalCanceledSignature();
		} else {
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "doSignCMD",
					  "Caught exception in PTEID_CMDSignatureClient::SignPDF. Error code: %08x", e.GetError());
		}
	}

	showSignCMDDialog(ret);

	if (ret == 0)
		updateTelemetry(TelemetryAction::SignCMD);
}

void GAPI::doSignXADESWithCMD(SignParams &params, bool isASIC) {
	BEGIN_TRY_CATCH

	std::string output_file = getPlatformNativeString(params.outputFile);

	std::vector<std::string> filenames_tmp;
	for (const QString &filename : params.loadedFilePaths) {
		filenames_tmp.push_back(getPlatformNativeString(filename));
	}

	std::vector<const char *> files_to_sign;
	for (const std::string &filename : filenames_tmp) {
		files_to_sign.push_back(filename.c_str());
	}

	unsigned int file_count = params.loadedFilePaths.count();

	PTEID_SignatureLevel level = PTEID_LEVEL_BASIC;
	if (params.isLtv) {
		level = PTEID_LEVEL_LTV;
	} else if (params.isTimestamp) {
		level = PTEID_LEVEL_TIMESTAMP;
	}

	PTEID_CMDSignatureClient *client = m_cmd_client;

	long ret = -1;
	try {
		if (isASIC)
			client->SignASiC(files_to_sign[0], level);
		else
			client->SignXades(output_file.c_str(), &files_to_sign[0], file_count, level);

		emit signalPdfSignSuccess(SignMessageOK);
	} catch (PTEID_Exception &e) {
		ret = e.GetError();
		if (ret == EIDMW_ERR_OP_CANCEL) {
			emit signalCanceledSignature();
		} else if (isASIC) {
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "doSignXADESWithCMD",
					  "Caught exception in PTEID_CMDSignatureClient::SignASiC. Error code: %08x", e.GetError());
		} else {
			PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "doSignXADESWithCMD",
					  "Caught exception in PTEID_CMDSignatureClient::SignXades. Error code: %08x", e.GetError());
		}
		showSignCMDDialog(ret);
	}

	//  emit signalPdfSignSuccess(SignMessageOK);

	END_TRY_CATCH
}

void GAPI::signCMD(QList<QString> loadedFilePaths, QString outputFile, int page, double coord_x, double coord_y,
				   QString reason, QString location, bool isTimestamp, bool isLTV, bool isSmall, bool isLastPage) {

	SignParams signParams = {loadedFilePaths, outputFile,  page,  coord_x, coord_y, reason,
							 location,		  isTimestamp, isLTV, isSmall, false};

	PTEID_PDFSignature *pdf_signature = new PTEID_PDFSignature();

	if (loadedFilePaths.size() == 1) {
		pdf_signature->setFileSigning((char *)getPlatformNativeString(loadedFilePaths.first()));
	} else {
		// batch signature
		for (QString filepath : loadedFilePaths) {
			pdf_signature->addToBatchSigning((char *)getPlatformNativeString(filepath), isLastPage);
		}
	}

	if (signParams.isTimestamp) {
		if (signParams.isLtv) {
			pdf_signature->setSignatureLevel(PTEID_LEVEL_LTV);
		} else {
			pdf_signature->setSignatureLevel(PTEID_LEVEL_TIMESTAMP);
		}
	}

	if (signParams.isSmallSignature)
		pdf_signature->enableSmallSignatureFormat();

	pdf_signature->setCustomSealSize(m_seal_width, m_seal_height);

	if (useCustomSignature()) {
		const PTEID_ByteArray imageData(reinterpret_cast<const unsigned char *>(m_jpeg_scaled_data.data()),
										static_cast<unsigned long>(m_jpeg_scaled_data.size()));
		pdf_signature->setCustomImage(const_cast<unsigned char *>(imageData.GetBytes()), imageData.Size());
	}

	Concurrent::run(this, &GAPI::doSignCMD, *pdf_signature, signParams);
}

QString GAPI::getCardActivation() {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance getCardActivation");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return "Error getting Card Activation";

	PTEID_EId &eid_file = card->getID();

	PTEID_Certificates &certificates = card->getCertificates();

	int certificateStatus = PTEID_CERTIF_STATUS_UNKNOWN;

	certificateStatus = certificates.getCert(PTEID_Certificate::CITIZEN_AUTH).getStatus();

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "PTEID_Certificate::getStatus() returned: %d", certificateStatus);

	// If state active AND validity not expired AND certificate active
	if (card->isActive() && !isExpiredDate(eid_file.getValidityEndDate()) &&
		certificateStatus == PTEID_CERTIF_STATUS_VALID) {
		return QString(tr("STR_CARD_ACTIVE_AND_VALID"));
	}
	// Else If state active AND validity not expired AND certificate validation error
	else if (card->isActive() && !isExpiredDate(eid_file.getValidityEndDate())) {
		// Network access errors (OCSP/CRL)
		if (certificateStatus == PTEID_CERTIF_STATUS_CONNECT) {
			QString msg(tr("STR_CARD_CONNECTION_ERROR"));
			if (m_Settings.isProxyConfigured())
				msg.append(" ").append(tr("STR_VERIFY_PROXY"));
			return msg;
		} else if (certificateStatus == PTEID_CERTIF_STATUS_ERROR || certificateStatus == PTEID_CERTIF_STATUS_ISSUER ||
				   certificateStatus == PTEID_CERTIF_STATUS_UNKNOWN)
			return QString(tr("STR_CARD_VALIDATION_ERROR"));
		else if (certificateStatus == PTEID_CERTIF_STATUS_SUSPENDED || certificateStatus == PTEID_CERTIF_STATUS_REVOKED)
			return QString(tr("STR_CARD_CANCELED"));
		else if (certificateStatus == PTEID_CERTIF_STATUS_EXPIRED)
			return QString(tr("STR_CARD_EXPIRED_CERT"));
	}
	// Else If state active AND validity expired
	else if (card->isActive() && isExpiredDate(eid_file.getValidityEndDate())) {
		return QString(tr("STR_CARD_EXPIRED"));
	}
	// Else If state not active
	else if (!card->isActive()) {
		return QString(tr("STR_CARD_NOT_ACTIVE"));
	}
	END_TRY_CATCH

	return QString(tr("STR_CARD_STATUS_FAIL"));
}

QPixmap PhotoImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
	if (p.count(id) == 0){
		qDebug() << "PhotoImageProvider: wrong id requested - " << id;
		return QPixmap();
	}

	if (!requestedSize.isValid()) {
		if (requestedSize.height() > p[id].height() || requestedSize.width() > p[id].width()) {
			qDebug() << "PhotoImageProvider: Invalid requestedSize - " << requestedSize;
			return QPixmap();
		}
	}

	size->setWidth(p[id].width());
	size->setHeight(p[id].height());

	return p[id];
}

void GAPI::startPrintPDF(QString outputFile, bool isBasicInfo, bool isAdditionalInfo, bool isAddress, bool isNotes,
						 bool isPrintDate, bool isSign, bool isTimestamp, bool isLtv) {
	PrintParams base_params = {outputFile, isBasicInfo, isAdditionalInfo, isAddress, isNotes, isPrintDate, isSign};

	PrintParamsWithSignature params = {base_params, isTimestamp, isLtv};
	Concurrent::run(this, &GAPI::doPrintPDF, params);
}

void GAPI::startPrint(QString outputFile, bool isBasicInfo, bool isAdditionalInfo, bool isAddress, bool isNotes,
					  bool isPrintDate, bool isSign) {

	if (QPrinterInfo::availablePrinters().size() == 0) {
		qDebug() << "No Printers Available";
		emit signalPrinterPrintFail(NoPrinterAvailable);
		return;
	}

	PrintParams params = {outputFile, isBasicInfo, isAdditionalInfo, isAddress, isNotes, isPrintDate, isSign};

	QPrinter printer;
	bool res = false;
	long addressError = 0;

	printer.setDocName("CartaoCidadao.pdf");
	QPrintDialog *dlg = new QPrintDialog(&printer);
	if (dlg->exec() == QDialog::Accepted) {
		BEGIN_TRY_CATCH;
		// Print PDF not Signed
		res = drawpdf(printer, params, addressError);
		if (params.outputFile.toUtf8().size() > 0) {
			qDebug() << "Printing to PDF file";
			if (res) {
				emit signalPdfPrintSucess();
			} else if (addressError >= EIDMW_REMOTEADDR_CONNECTION_ERROR &&
					   addressError <= EIDMW_REMOTEADDR_UNKNOWN_ERROR) {
				handleRemoteAddressErrors(addressError);
			} else {
				emit signalPdfPrintFail();
			}
		} else {
			qDebug() << "Printing to a printer";
			if (res) {
				emit signalPrinterPrintSucess();
			} else if (addressError >= EIDMW_REMOTEADDR_CONNECTION_ERROR &&
					   addressError <= EIDMW_REMOTEADDR_UNKNOWN_ERROR) {
				handleRemoteAddressErrors(addressError);
			} else {
				emit signalPdfPrintFail();
			}
		}
		END_TRY_CATCH
	}
}

bool GAPI::doSignPrintPDF(QString &file_to_sign, QString &outputsign, bool isTimestamp, bool isLtv) {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doSignPrintPDF");
	BEGIN_TRY_CATCH

	PTEID_PDFSignature sig_handler(getPlatformNativeString(file_to_sign));
	if (isTimestamp) {
		if (isLtv) {
			sig_handler.setSignatureLevel(PTEID_LEVEL_LTV);
		} else {
			sig_handler.setSignatureLevel(PTEID_LEVEL_TIMESTAMP);
		}
	}

	PTEID_SigningDeviceFactory &factory = PTEID_SigningDeviceFactory::instance();
	PTEID_SigningDevice &device = factory.getSigningDevice();

	try {
		device.SignPDF(sig_handler, 0, 0, false, "", "", getPlatformNativeString(outputsign));
	} catch (PTEID_Exception &e) {
		if (device.getDeviceType() == PTEID_SigningDeviceType::CMD) {
			if (e.GetError() != EIDMW_ERR_OP_CANCEL) {
				showSignCMDDialog(e.GetError());
				return false;
			}
		}
		throw;
	}

	return true;

	END_TRY_CATCH

	return false;
}

void GAPI::doPrintPDF(PrintParamsWithSignature &params) {

	PrintParams base_params = params.base_params;
	qDebug() << "doPrintPDF! outputFile =" << base_params.outputFile << "isBasicInfo =" << base_params.isBasicInfo
			 << "isAdditionalInfo =" << base_params.isAdditionalInfo << "isAddress =" << base_params.isAddress
			 << "isNotes =" << base_params.isNotes << "isPrintDate =" << base_params.isPrintDate
			 << "isSign =" << base_params.isSign;
	if (base_params.isSign) {
		qDebug() << "SignatureParams: " << "isTimestamp =" << params.isTimestamp << "isLtv =" << params.isLtv;
	}

	QString pdffiletmp;
	QPrinter pdf_printer;
	QString nativepdftmp;
	QString originalOutputFile;
	bool res = false;
	long addressError = 0;
	BEGIN_TRY_CATCH;
	if (base_params.isSign) {
		// Print and sign PDF
		pdffiletmp = QDir::tempPath();
		pdffiletmp.append("/CartaoCidadao.pdf");
		nativepdftmp = QDir::toNativeSeparators(pdffiletmp);

		originalOutputFile = base_params.outputFile;
		base_params.outputFile = nativepdftmp;
		res = drawpdf(pdf_printer, base_params, addressError);

		if (!res) {
			if (addressError >= EIDMW_REMOTEADDR_CONNECTION_ERROR && addressError <= EIDMW_REMOTEADDR_UNKNOWN_ERROR) {
				handleRemoteAddressErrors(addressError);
			} else {
				emit signalPdfPrintFail();
			}
			return;
		}

		QFuture<bool> new_thread = Concurrent::run(this, &GAPI::doSignPrintPDF, nativepdftmp, originalOutputFile,
												   params.isTimestamp, params.isLtv);

		res = new_thread.result();
		// Emit signal if success but if false a popup about some error is already sent
		if (res)
			emit signalPdfPrintSignSucess();
	} else {
		// Print PDF not Signed
		res = drawpdf(pdf_printer, base_params, addressError);
		if (res) {
			emit signalPdfPrintSucess();
		} else if (addressError >= EIDMW_REMOTEADDR_CONNECTION_ERROR &&
				   addressError <= EIDMW_REMOTEADDR_UNKNOWN_ERROR) {
			handleRemoteAddressErrors(addressError);
		} else {
			emit signalPdfPrintFail();
		}
	}

	updateTelemetry(TelemetryAction::PrintPDF);

	END_TRY_CATCH
}

static QPen black_pen;
static QPen blue_pen;

double GAPI::drawSingleField(QPainter &painter, double pos_x, double pos_y, QString name, QString value,
							 double line_length, int field_margin, bool is_bounded_rect, double bound_width) {
	painter.setPen(blue_pen);

	// apply scale factor to default values
	if (field_margin == 15)
		field_margin *= print_scale_factor;
	if (bound_width == 360)
		bound_width *= print_scale_factor;
	if (field_margin == 0) {
		line_length -= 15 * print_scale_factor;
	}

	painter.drawText(QPointF(pos_x + field_margin, pos_y), name);
	pos_y += 7 * print_scale_factor;

	painter.drawLine(QPointF(pos_x + field_margin, pos_y), QPointF(pos_x + (line_length - field_margin), pos_y));
	painter.setPen(black_pen);

	if (is_bounded_rect) {
		int flags = Qt::TextWordWrap | Qt::TextWrapAnywhere;
		int textFlags = Qt::TextWordWrap;
		QFontMetricsF fm = painter.fontMetrics();
		QRectF bounding_rect = fm.boundingRect(
			QRectF(pos_x + field_margin, pos_y, bound_width - 2 * field_margin, 500 * print_scale_factor), flags,
			value);

		painter.drawText(bounding_rect.adjusted(0, 0, 0, 0), textFlags, value);
		pos_y += bounding_rect.height() + 15 * print_scale_factor;
	} else {
		pos_y += 15 * print_scale_factor;
		painter.drawText(QPointF(pos_x + field_margin, pos_y), value);
	}

	return pos_y;
}

void GAPI::drawSectionHeader(QPainter &painter, double pos_x, double pos_y, QString section_name) {
	QFont header_font("DIN Light");
	header_font.setPointSize(11);
	QFont regular_font("DIN Medium");
	regular_font.setPointSize(12);

	painter.setFont(header_font);

	QColor light_grey(233, 233, 233);
	painter.setBrush(light_grey);
	painter.setPen(light_grey);

	painter.drawRoundedRect(QRectF(pos_x, pos_y, 250 * print_scale_factor, 30 * print_scale_factor),
							10.0 * print_scale_factor, 10.0 * print_scale_factor);
	painter.setPen(black_pen);

	painter.drawText(pos_x + 20 * print_scale_factor, pos_y + 20 * print_scale_factor, section_name);

	painter.setFont(regular_font);
}

void GAPI::drawPrintingDate(QPainter &painter, QString printing_date) {
	QFont date_font("DIN Medium");
	date_font.setPointSize(8);
	date_font.setBold(false);
	QFont regular_font("DIN Medium");
	regular_font.setPointSize(12);

	printing_date += " " + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
	painter.setFont(date_font);
	painter.drawText(QRectF(painter.device()->width() - 225 * print_scale_factor,
							painter.device()->height() - 25 * print_scale_factor, 200 * print_scale_factor,
							100 * print_scale_factor),
					 Qt::TextWordWrap, printing_date);

	painter.setFont(regular_font);
}

QPixmap loadHeader() { return QPixmap(":/images/pdf_document_header.png"); }

QString getTextFromLines(QStringList lines, int start, int stop) {
	QStringList text_lines;

	// make sure doesn't access a invalid index
	if (stop > lines.length()) {
		stop = lines.length();
	}

	for (int i = start; i < stop; i++) {
		text_lines.append(lines.at(i));
	}

	return text_lines.join("\n");
}

double GAPI::checkNewPageAndPrint(QPrinter &printer, QPainter &painter, double current_y, double remaining_height,
								  double max_height, bool print_date, QString date_label) {
	if (current_y + remaining_height > max_height) {
		printer.newPage();
		current_y = 50 * print_scale_factor;
		if (print_date) {
			drawPrintingDate(painter, date_label);
		}
	}
	return current_y;
}

bool GAPI::drawpdf(QPrinter &printer, PrintParams params, long &addressError) {
	qDebug() << "drawpdf PrintParams: outputFile =" << params.outputFile << "isBasicInfo =" << params.isBasicInfo
			 << " isAdditionalInfo =" << params.isAdditionalInfo << "isAddress =" << params.isAddress
			 << " isNotes =" << params.isNotes << " isPrintDate =" << params.isPrintDate;

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance drawpdf");

	BEGIN_TRY_CATCH;

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return false;

	card->doSODCheck(true); // Enable SOD checking
	PTEID_EId &eid_file = card->getID();

	// QPrinter printer;
	print_scale_factor = printer.resolution() / 96.0;

	printer.setColorMode(QPrinter::Color);
	printer.setPageSize(QPageSize(QPageSize::A4));

	if (params.outputFile.toUtf8().size() > 0) {
		printer.setOutputFileName(params.outputFile.toUtf8().data());
		qDebug() << "Printing PDF";
	} else {
		qDebug() << "Printing to default printer";
	}

	// TODO: Add custom fonts
	// addFonts();

	// Start drawing
	QPainter painter;
	bool res = false;
	res = painter.begin(&printer);
	if (res == false) {
		qDebug() << "Start drawing return error: " << res;
		return false;
	}

	// gives a bit of left margin
	double page_margin = 33.5 * print_scale_factor;
	double pos_x = page_margin, pos_y = 0;
	int field_margin = 15 * print_scale_factor;

	double page_height = painter.device()->height();
	double page_width = painter.device()->width();
	double full_page = (page_width - 2 * page_margin);
	double half_page = full_page / 2;
	double third_of_page = (page_width - 2 * page_margin) / 3;

	// Font setup
	QFont din_font("DIN Medium");
	din_font.setPixelSize(18 * print_scale_factor);

	//  Include header as png pixmap
	QPixmap header = loadHeader();
	QRectF headerImgRect = QRectF(pos_x, pos_y, 359.0 * print_scale_factor, 75.0 * print_scale_factor);
	painter.drawPixmap(headerImgRect, header, QRect(0.0, 0.0, header.width(), header.height()));

	//  //Alternative using the QtSVG module, not enabled for now because the rendering is far from perfect
	// QSvgRenderer renderer(QString("C:\\Users\\agrr\\Desktop\\GMC_logo.svg"));
	// std::cout << renderer.defaultSize().width() << "x" << renderer.defaultSize().height() << std::endl;

	// renderer.render(&painter, QRect(pos_x, pos_y, 504, 132));
	// renderer.render(&painter, QRect(pos_x, pos_y, 250, 120));

	pos_y += headerImgRect.height() + field_margin;
	black_pen = painter.pen();

	blue_pen.setColor(QColor(78, 138, 190));
	painter.setPen(blue_pen);

	int line_length = 487 * print_scale_factor;
	// Horizontal separator below the CC logo
	painter.drawLine(QPointF(pos_x, pos_y), QPointF(pos_x + line_length, pos_y));
	pos_y += 25 * print_scale_factor;

	// Change text color
	blue_pen = painter.pen();

	const double LINE_HEIGHT = 45.0 * print_scale_factor;

	//    din_font.setBold(true);
	painter.setFont(din_font);

	painter.drawText(QPointF(pos_x, pos_y), tr("STR_PERSONAL_DATA"));

	pos_y += field_margin;

	painter.setPen(black_pen);
	// Reset font
	din_font.setPixelSize(10 * print_scale_factor);
	din_font.setBold(false);
	painter.setFont(din_font);

	pos_y += 30 * print_scale_factor;

	if (params.isPrintDate) {
		drawPrintingDate(painter, tr("STR_PRINTED_ON"));
	}

	if (params.isBasicInfo) {

		drawSectionHeader(painter, pos_x, pos_y, tr("STR_BASIC_INFORMATION"));

		// Load photo into a QPixmap
		PTEID_ByteArray &photo = eid_file.getPhotoObj().getphoto();

		// Image
		QPixmap pixmap_photo;
		pixmap_photo.loadFromData(photo.GetBytes(), photo.Size(), "PNG");

		const int img_height = 200 * print_scale_factor;

		// Scale height if needed
		QPixmap scaled = pixmap_photo.scaledToHeight(img_height, Qt::SmoothTransformation);

		painter.drawPixmap(QPointF(pos_x + 500 * print_scale_factor, pos_y - 80 * print_scale_factor), scaled);

		pos_y += 50 * print_scale_factor;

		double new_pos_y = drawSingleField(painter, pos_x, pos_y, tr("STR_GIVEN_NAME"),
										   QString::fromUtf8(eid_file.getGivenName()), half_page, 0, true, half_page);
		pos_y = (std::max)(new_pos_y, pos_y + LINE_HEIGHT);

		new_pos_y = drawSingleField(painter, pos_x, pos_y, tr("STR_SURNAME"), QString::fromUtf8(eid_file.getSurname()),
									half_page, 0, true, half_page);
		pos_y = (std::max)(new_pos_y, pos_y + LINE_HEIGHT);

		double f_col = half_page / 2;
		drawSingleField(painter, pos_x, pos_y, tr("STR_GENDER"), QString::fromUtf8(eid_file.getGender()), f_col, 0);
		drawSingleField(painter, pos_x + f_col, pos_y, tr("STR_HEIGHT"), QString::fromUtf8(eid_file.getHeight()),
						f_col);
		drawSingleField(painter, pos_x + 2 * f_col, pos_y, tr("STR_NATIONALITY"),
						QString::fromUtf8(eid_file.getNationality()), f_col);
		drawSingleField(painter, pos_x + 3 * f_col, pos_y, tr("STR_DATE_OF_BIRTH"),
						QString::fromUtf8(eid_file.getDateOfBirth()), f_col);
		pos_y += LINE_HEIGHT;

		drawSingleField(painter, pos_x, pos_y, tr("STR_DOCUMENT_NUMBER"),
						QString::fromUtf8(eid_file.getDocumentNumber()), half_page, 0);
		drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_VALIDITY_DATE"),
						QString::fromUtf8(eid_file.getValidityEndDate()), half_page);
		pos_y += LINE_HEIGHT;

		double new_height_left = drawSingleField(painter, pos_x, pos_y, tr("STR_FATHER"),
												 QString::fromUtf8(eid_file.getGivenNameFather()) + " " +
													 QString::fromUtf8(eid_file.getSurnameFather()),
												 half_page, 0, true, half_page);
		double new_height_right = drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_MOTHER"),
												  QString::fromUtf8(eid_file.getGivenNameMother()) + " " +
													  QString::fromUtf8(eid_file.getSurnameMother()),
												  half_page, field_margin, true, half_page);
		pos_y = (std::max)((std::max)(new_height_left, new_height_right), pos_y + LINE_HEIGHT);

		new_pos_y =
			drawSingleField(painter, pos_x, pos_y, tr("STR_NOTES"),
							QString::fromUtf8(eid_file.getAccidentalIndications()), half_page, 0, true, page_width);

		pos_y = (std::max)(new_pos_y, pos_y + LINE_HEIGHT);
	}

	if (params.isAdditionalInfo) {
		// height of this section, if spacing between fields change update this value
		double min_section_height = 235 * print_scale_factor;
		pos_y = checkNewPageAndPrint(printer, painter, pos_y, min_section_height, page_height, params.isPrintDate,
									 tr("STR_PRINTED_ON"));

		drawSectionHeader(painter, pos_x, pos_y, tr("STR_ADDITIONAL_INFORMATION"));
		pos_y += 50 * print_scale_factor;

		drawSingleField(painter, pos_x, pos_y, tr("STR_VAT_NUM"), QString::fromUtf8(eid_file.getTaxNo()), third_of_page,
						0);
		drawSingleField(painter, pos_x + third_of_page, pos_y, tr("STR_SOCIAL_SECURITY_NUM"),
						QString::fromUtf8(eid_file.getSocialSecurityNumber()), third_of_page);
		drawSingleField(painter, pos_x + 2 * third_of_page, pos_y, tr("STR_NATIONAL_HEALTH_NUM"),
						QString::fromUtf8(eid_file.getHealthNumber()), third_of_page);
		pos_y += LINE_HEIGHT;

		drawSingleField(painter, pos_x, pos_y, tr("STR_DELIVERY_ENTITY"),
						QString::fromUtf8(eid_file.getIssuingEntity()), half_page, 0, true, half_page);
		drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_DELIVERY_DATE"),
						QString::fromUtf8(eid_file.getValidityBeginDate()), half_page);
		pos_y += LINE_HEIGHT;

		drawSingleField(painter, pos_x, pos_y, tr("STR_DOCUMENT_TYPE"), QString::fromUtf8(eid_file.getDocumentType()),
						half_page, 0, true, half_page);
		drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_DELIVERY_LOCATION"),
						QString::fromUtf8(eid_file.getLocalofRequest()), half_page, field_margin, true, half_page);
		pos_y += LINE_HEIGHT;

		drawSingleField(painter, pos_x, pos_y, tr("STR_CARD_VERSION"), QString::fromUtf8(eid_file.getDocumentVersion()),
						half_page, 0);
		/*
		drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_CARD_STATE"),
			getCardActivation(), half_page, field_margin, true, half_page);
		*/
		pos_y += 50 * print_scale_factor;
	}

	if (params.isAddress) {
		// height of this section, if spacing between fields change update this value
		double min_section_height = 350 * print_scale_factor;
		pos_y = checkNewPageAndPrint(printer, painter, pos_y, min_section_height, page_height, params.isPrintDate,
									 tr("STR_PRINTED_ON"));

		drawSectionHeader(painter, pos_x, pos_y, tr("STR_ADDRESS"));
		pos_y += 50 * print_scale_factor;

		try {
			PTEID_Address &addressFile = card->getAddr();

			if (addressFile.isNationalAddress()) {
				double new_height_left =
					drawSingleField(painter, pos_x, pos_y, tr("STR_DISTRICT"),
									QString::fromUtf8(addressFile.getDistrict()), half_page, 0, true, half_page);
				double new_height_right = drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_MUNICIPALITY"),
														  QString::fromUtf8(addressFile.getMunicipality()), half_page,
														  field_margin, true, half_page);
				pos_y = (std::max)((std::max)(new_height_left, new_height_right), pos_y + LINE_HEIGHT);

				new_height_left =
					drawSingleField(painter, pos_x, pos_y, tr("STR_CIVIL_PARISH"),
									QString::fromUtf8(addressFile.getCivilParish()), full_page, 0, true, page_width);
				pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

				drawSingleField(painter, pos_x, pos_y, tr("STR_STREET_TYPE"),
								QString::fromUtf8(addressFile.getStreetType()), full_page, 0, true, page_width);
				pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

				new_height_left =
					drawSingleField(painter, pos_x, pos_y, tr("STR_STREET_NAME"),
									QString::fromUtf8(addressFile.getStreetName()), full_page, 0, true, page_width);
				pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

				new_height_left = drawSingleField(painter, pos_x, pos_y, tr("STR_HOUSE_BUILDING_NUM"),
												  QString::fromUtf8(addressFile.getDoorNo()), third_of_page, 0);
				new_height_right = drawSingleField(painter, pos_x + third_of_page, pos_y, tr("STR_FLOOR"),
												   QString::fromUtf8(addressFile.getFloor()), third_of_page);
				double new_height = drawSingleField(painter, pos_x + 2 * third_of_page, pos_y, tr("STR_SIDE"),
													QString::fromUtf8(addressFile.getSide()), third_of_page);
				pos_y = (std::max)(new_height,
								   (std::max)(new_height_left, (std::max)(new_height_left, pos_y + LINE_HEIGHT)));

				drawSingleField(painter, pos_x, pos_y, tr("STR_PLACE"), QString::fromUtf8(addressFile.getPlace()),
								half_page, 0, true, half_page);
				new_height_left = drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_LOCALITY"),
												  QString::fromUtf8(addressFile.getLocality()), half_page, field_margin,
												  true, half_page);

				pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

				drawSingleField(painter, pos_x, pos_y, tr("STR_ZIP_CODE"),
								QString::fromUtf8(addressFile.getZip4()) + "-" +
									QString::fromUtf8(addressFile.getZip3()),
								half_page / 2, 0);
				new_height_left = drawSingleField(painter, pos_x + (half_page / 2), pos_y, tr("STR_POSTAL_LOCALITY"),
												  QString::fromUtf8(addressFile.getPostalLocality()),
												  (3 * half_page) / 2, field_margin, true, (3 * half_page) / 2);
				pos_y = (std::max)(new_height_left, pos_y + LINE_HEIGHT);

			} else {
				/* Foreign Address*/
				drawSingleField(painter, pos_x, pos_y, tr("STR_FOREIGN_COUNTRY"),
								QString::fromUtf8(addressFile.getForeignCountry()), third_of_page, 0);
				drawSingleField(painter, pos_x + third_of_page, pos_y, tr("STR_FOREIGN_REGION"),
								QString::fromUtf8(addressFile.getForeignRegion()), third_of_page);
				drawSingleField(painter, pos_x + 2 * third_of_page, pos_y, tr("STR_FOREIGN_CITY"),
								QString::fromUtf8(addressFile.getForeignCity()), third_of_page);

				pos_y += LINE_HEIGHT;

				drawSingleField(painter, pos_x, pos_y, tr("STR_FOREIGN_LOCALITY"),
								QString::fromUtf8(addressFile.getForeignLocality()), half_page, 0, true, half_page);
				drawSingleField(painter, pos_x + half_page, pos_y, tr("STR_FOREIGN_POSTAL_CODE"),
								QString::fromUtf8(addressFile.getForeignPostalCode()), half_page, field_margin, true,
								half_page);

				pos_y += LINE_HEIGHT;

				drawSingleField(painter, pos_x, pos_y, tr("STR_FOREIGN_ADDRESS"),
								QString::fromUtf8(addressFile.getForeignAddress()), half_page, 0, true, page_width);
			}
			pos_y += 80 * print_scale_factor;
		} catch (PTEID_Exception &e) {
			long errorCode = e.GetError();
			if (errorCode >= EIDMW_REMOTEADDR_CONNECTION_ERROR && errorCode <= EIDMW_REMOTEADDR_UNKNOWN_ERROR) {
				qDebug() << "Address exception captured in drawpdf!";
				addressError = e.GetError();
				return false;
			} else {
				throw e;
			}
		}
	}

	if (params.isNotes) {
		QString perso_data;

		perso_data = QString(card->readPersonalNotes());

		if (perso_data.size() > 0) {
			pos_x = 30 * print_scale_factor; // gives a bit of left margin
			pos_y += 50 * print_scale_factor;

			pos_y =
				checkNewPageAndPrint(printer, painter, pos_y, 0, page_height, params.isPrintDate, tr("STR_PRINTED_ON"));

			drawSectionHeader(painter, pos_x, pos_y, tr("STR_PERSONAL_NOTES"));

			pos_y += 50 * print_scale_factor;

			#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
				QStringList lines = perso_data.split("\n", Qt::KeepEmptyParts);
			#else 
				QStringList lines = perso_data.split("\n", QString::KeepEmptyParts);
			#endif

			const int TEXT_LINE_HEIGHT = 20 * print_scale_factor;

			int line_count = lines.length();
			double height_to_print = TEXT_LINE_HEIGHT * line_count;
			int line_index_start = 0;
			int line_index_stop = 0;
			int completed_lines = 0;
			double notes_width = painter.device()->width() - 2 * page_margin;

			QFontMetrics fontMetrics(din_font);
			while (completed_lines < line_count) {
				int page_remaining_space = static_cast<int>(page_height - pos_y);

				line_index_stop = line_index_start + (page_remaining_space / TEXT_LINE_HEIGHT);

				if (page_remaining_space < 50 * print_scale_factor && height_to_print > 0) {
					printer.newPage();
					pos_y = 50 * print_scale_factor;
					drawSectionHeader(painter, pos_x, pos_y, tr("STR_PERSONAL_NOTES"));
					pos_y += 50 * print_scale_factor;
					if (params.isPrintDate) {
						drawPrintingDate(painter, tr("STR_PRINTED_ON"));
					}
					// not enough area recalculate lines needed
					page_remaining_space = static_cast<int>(page_height - pos_y);
					line_index_stop = line_index_start + (page_remaining_space / TEXT_LINE_HEIGHT);
				}

				QString text = getTextFromLines(lines, line_index_start, line_index_stop);

				double diff = line_index_stop - line_index_start;
				double height = diff * TEXT_LINE_HEIGHT;

				QRectF bounding_rect = fontMetrics.boundingRect(text);
				QRectF textRect(pos_x, pos_y, notes_width, height);
				painter.drawText(textRect, Qt::TextWrapAnywhere, text, &bounding_rect);
				height_to_print -= height;
				pos_y += height;
				completed_lines += diff;
				line_index_start = line_index_stop;
			}
		}
	}
	// Finish drawing/printing
	painter.end();
	END_TRY_CATCH
	return true;
}

void GAPI::startSigningPDF(QString loadedFilePath, QString outputFile, int page, double coord_x, double coord_y,
						   QString reason, QString location, bool isTimestamp, bool isLTV, bool isSmall) {
	// TODO: refactor startSigningPDF/startSigningBatchPDF
	QList<QString> loadedFilePaths = {loadedFilePath};
	SignParams params = {loadedFilePaths, outputFile,  page,  coord_x, coord_y, reason,
						 location,		  isTimestamp, isLTV, isSmall, false};
	QFuture<void> future = Concurrent::run(this, &GAPI::doSignPDF, params);
}

void GAPI::startSigningBatchPDF(QList<QString> loadedFileBatchPath, QString outputFile, int page, double coord_x,
								double coord_y, QString reason, QString location, bool isTimestamp, bool isLTV,
								bool isSmall, bool isLastPage) {

	SignParams params = {loadedFileBatchPath, outputFile, page,	   coord_x,	  coord_y, reason, location,
						 isTimestamp,		  isLTV,	  isSmall, isLastPage};

	QFuture<void> future = Concurrent::run(this, &GAPI::doSignBatchPDF, params);
}

int GAPI::getPDFpageCount(QString loadedFilePath) {

	PTEID_PDFSignature sig_handler(getPlatformNativeString(loadedFilePath));

	int pageCount = sig_handler.getPageCount();

	return pageCount;
}

void GAPI::closePdfPreview(QString filePath) { image_provider_pdf->closeDoc(filePath); }
void GAPI::closeAllPdfPreviews() {
	qDebug() << "closeAllPdfPreviews";
	image_provider_pdf->closeAllDocs();
}

void GAPI::startSigningXADES(QString loadedFilePath, QString outputFile, bool isTimestamp, bool isLTV, bool isASIC) {

	QFuture<void> future =
		Concurrent::run(this, &GAPI::doSignXADES, loadedFilePath, outputFile, isTimestamp, isLTV, isASIC);
}

void GAPI::startSigningBatchXADES(QList<QString> loadedFileBatchPath, QString outputFile, bool isTimestamp,
								  bool isLTV) {

	SignParams params = {loadedFileBatchPath, outputFile, 0, 0, 0, "", "", isTimestamp, isLTV, 0, 0};

	QFuture<void> future = Concurrent::run(this, &GAPI::doSignBatchXADES, params);
}

void GAPI::doSignBatchXADES(SignParams &params) {
	qDebug() << "doSignBatchXADES! loadedFilePath = " << params.loadedFilePaths << " outputFile = " << params.outputFile
			 << "page = " << params.page << "coord_x" << params.coord_x << "coord_y" << params.coord_y
			 << "reason = " << params.reason << "location = " << params.location
			 << "isTimestamp = " << params.isTimestamp << "isLtv = " << params.isLtv;

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doSignBatchXADES");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	QVector<const char *> files_to_sign;

	for (int i = 0; i < params.loadedFilePaths.count(); i++) {
		files_to_sign.push_back(strdup(getPlatformNativeString(params.loadedFilePaths[i])));
	}

	QByteArray tempOutputFile = getPlatformNativeString(params.outputFile);

	PTEID_SignatureLevel level = PTEID_LEVEL_BASIC;
	if (params.isLtv) {
		level = PTEID_LEVEL_LTV;
	} else if (params.isTimestamp) {
		level = PTEID_LEVEL_TIMESTAMP;
	}
	card->SignXades(tempOutputFile.constData(), files_to_sign.data(), params.loadedFilePaths.count(), level);

	emit signalPdfSignSuccess(SignMessageOK);

	END_TRY_CATCH
}

void GAPI::startSigningXADESWithCMD(QList<QString> inputFiles, QString outputFile, bool isTimestamp, bool isLTV,
									bool isASIC) {

	SignParams params = {inputFiles, outputFile, 0, 0, 0, "", "", isTimestamp, isLTV, 0, 0};

	QFuture<void> future = Concurrent::run(this, &GAPI::doSignXADESWithCMD, params, isASIC);
}

bool GAPI::isASiC(const QString &filename) {
	std::string filename_utf8 = filename.toUtf8().constData();
	bool isASiC = false;

	try {
		isASiC = SigContainer::isValidASiC(filename_utf8.c_str());
	} catch (const PTEID_Exception &e) {
		emitErrorSignal(__FUNCTION__, e.GetError());
	}

	return isASiC;
}

QStringList GAPI::listFilesInASiC(const QString &filename) {
	QStringList files;

	try {
		PTEID_ASICContainer container(filename.toStdString().c_str());
		long file_count = container.countInputFiles();

		for (long i = 0; i < file_count; ++i) {
			files.append(container.getInputFile(i));
		}

	} catch (const PTEID_Exception &e) {
		emitErrorSignal(__FUNCTION__, e.GetError());
	}

	return files;
}

void GAPI::extractFileFromASiC(const QString &container_path, const QString &filename) {
	Concurrent::run(this, &GAPI::doExtractFileFromASiC, container_path, filename);
}

void GAPI::doExtractFileFromASiC(const QString &container_path, const QString &filename) {
	QString out_dir = QDir::tempPath();
	QString complete_path = QDir::toNativeSeparators(out_dir + "/" + filename);

	try {
		PTEID_ASICContainer container(container_path.toStdString().c_str());
		container.extract(filename.toStdString().c_str(), out_dir.toStdString().c_str());
		QDesktopServices::openUrl("file:///" + complete_path);
	} catch (const PTEID_Exception &e) {
		emitErrorSignal(__FUNCTION__, e.GetError());
	}
}

bool GAPI::isDirectory(QString path) {
	QFileInfo fi(path);
	return fi.isDir();
}

bool GAPI::isFile(QString path) {
	QFileInfo fi(path);
	return fi.isFile();
}

// Checks if the given file status or path corresponds to an existing file or directory.
bool GAPI::fileExists(QString path) {
	QFileInfo check_file(path);
	return check_file.exists();
}

QList<QString> GAPI::getFilesFromDirectory(QString path) {
	QList<QString> allFiles;
	if (isDirectory(path)) {
		QDir dir(path);
		if (dir.exists() && !dir.isEmpty()) {
			QFileInfoList list =
				dir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);
			for (int i = 0; i < list.size(); ++i) {
				QFileInfo fi = list.at(i);
				allFiles.append(fi.absoluteFilePath());
			}
		}
	}
	qDebug() << "QSTring files " << allFiles;
	return allFiles;
}

void GAPI::doSignXADES(QString loadedFilePath, QString outputFile, bool isTimestamp, bool isLTV, bool isASIC) {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doSignXADES");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	const char *files_to_sign[1];
	QByteArray tempLoadedFilePath = getPlatformNativeString(loadedFilePath);
	files_to_sign[0] = tempLoadedFilePath.constData();

	QByteArray tempOutputFile = getPlatformNativeString(outputFile);

	PTEID_SignatureLevel level = PTEID_LEVEL_BASIC;
	if (isLTV) {
		level = PTEID_LEVEL_LTV;
	} else if (isTimestamp) {
		level = PTEID_LEVEL_TIMESTAMP;
	}
	if (isASIC) {
		card->SignASiC(files_to_sign[0], level);
	} else {
		card->SignXades(tempOutputFile.constData(), files_to_sign, 1, level);
	}

	emit signalPdfSignSuccess(SignMessageOK);

	END_TRY_CATCH
}

void GAPI::doSignPDF(SignParams &params) {

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doSignPDF");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	QString fullInputPath = params.loadedFilePaths[0];
	PTEID_PDFSignature sig_handler(getPlatformNativeString(fullInputPath));

	if (params.isTimestamp) {
		if (params.isLtv) {
			sig_handler.setSignatureLevel(PTEID_LEVEL_LTV);
		} else {
			sig_handler.setSignatureLevel(PTEID_LEVEL_TIMESTAMP);
		}
	}

	if (params.isSmallSignature) {
		sig_handler.enableSmallSignatureFormat();
	}

	sig_handler.setCustomSealSize(m_seal_width, m_seal_height);

	if (useCustomSignature()) {
		const PTEID_ByteArray imageData(reinterpret_cast<const unsigned char *>(m_jpeg_scaled_data.data()),
										static_cast<unsigned long>(m_jpeg_scaled_data.size()));
		sig_handler.setCustomImage(imageData);
	}
	card->SignPDF(sig_handler, params.page, params.coord_x, params.coord_y, params.location.toUtf8().data(),
				  params.reason.toUtf8().data(), getPlatformNativeString(params.outputFile));

	emit signalPdfSignSuccess(SignMessageOK);

	updateTelemetry(TelemetryAction::SignCC);

	END_TRY_CATCH
}

void GAPI::doSignBatchPDF(SignParams &params) {

	qDebug() << "doSignBatchPDF! loadedFilePath = " << params.loadedFilePaths << " outputFile = " << params.outputFile
			 << "page = " << params.page << "coord_x" << params.coord_x << "coord_y" << params.coord_y
			 << "reason = " << params.reason << "location = " << params.location
			 << " isLastPage = " << params.isLastPage;

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doSignBatchPDF");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	PTEID_PDFSignature *sig_handler;
	sig_handler = new PTEID_PDFSignature();

	for (int i = 0; i < params.loadedFilePaths.count(); i++) {
		qDebug() << params.loadedFilePaths[i];
		QString fullInputPath = params.loadedFilePaths[i];
		sig_handler->addToBatchSigning((char *)getPlatformNativeString(fullInputPath), params.isLastPage);
	}

	if (params.isTimestamp) {
		if (params.isLtv) {
			sig_handler->setSignatureLevel(PTEID_LEVEL_LTV);
		} else {
			sig_handler->setSignatureLevel(PTEID_LEVEL_TIMESTAMP);
		}
	}

	if (params.isSmallSignature) {
		sig_handler->enableSmallSignatureFormat();
	}

	sig_handler->setCustomSealSize(m_seal_width, m_seal_height);

	if (useCustomSignature()) {
		const PTEID_ByteArray imageData(reinterpret_cast<const unsigned char *>(m_jpeg_scaled_data.data()),
										static_cast<unsigned long>(m_jpeg_scaled_data.size()));
		sig_handler->setCustomImage(imageData);
	}

	card->SignPDF(*sig_handler, params.page, params.coord_x, params.coord_y, params.location.toUtf8().data(),
				  params.reason.toUtf8().data(), getPlatformNativeString(params.outputFile));

	emit signalPdfSignSuccess(SignMessageOK);

	END_TRY_CATCH
}

QPixmap PDFPreviewImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
	qDebug() << "PDFPreviewImageProvider received request for (width height): " << requestedSize.width() << " - "
			 << requestedSize.height();
	QStringList strList = id.split("?");

	QString pdf_path = QUrl::fromPercentEncoding(strList.at(0).toUtf8());
	m_filePath = pdf_path.toStdString();

	// URL param ?page=xx
	unsigned int page = (unsigned int)strList.at(1).split("=").at(1).toInt();

	if (m_docs.find(m_filePath) == m_docs.end()) {

		Poppler::Document *newdoc = Poppler::Document::load(pdf_path);
		if (!newdoc) {
			qDebug() << "Failed to load PDF file";
			return QPixmap();
		}
		m_docs.insert(std::pair<std::string, Poppler::Document *>(m_filePath, newdoc));

		newdoc->setRenderHint(Poppler::Document::TextAntialiasing, true);
		newdoc->setRenderHint(Poppler::Document::Antialiasing, true);
		newdoc->setRenderBackend(Poppler::Document::RenderBackend::SplashBackend);
	}

	if (m_file_id != id) {
		m_cache = renderPDFPage(page);
		m_file_id = id;
	}

	QPixmap scaled =
		m_cache.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	size->setHeight(scaled.height());
	size->setWidth(scaled.width());

	emit signalPdfSourceChanged(m_cache.width());

	return scaled;
}

QSize PDFPreviewImageProvider::getPageSize(int page) {
	Poppler::Page *popplerPage = m_docs.at(m_filePath)->page(page - 1);
	return popplerPage->pageSize();
}

QPixmap PDFPreviewImageProvider::renderPDFPage(unsigned int page) {
	renderMutex.lock();
	if (m_docs.empty()) {
		qDebug() << "Pdf to be rendered was unloaded!";
		return QPixmap();
	}
	// Document starts at page 0 in the poppler-qt5 API
	Poppler::Page *popplerPage = m_docs.at(m_filePath)->page(page - 1);

	if (popplerPage == NULL) {
		qDebug() << "Failed to get page object: " << page;
		return QPixmap();
	}

	const double resX = 300.0;
	const double resY = 300.0;
	QImage image = popplerPage->renderToImage(resX, resY);

	delete popplerPage;
	renderMutex.unlock();

	if (!image.isNull()) {
		return QPixmap::fromImage(image);
	} else {
		qDebug() << "Error rendering PDF page to image!";
		return QPixmap();
	}
}

void PDFPreviewImageProvider::closeDoc(QString filePath) {
	Concurrent::run(this, &PDFPreviewImageProvider::doCloseDoc, filePath);
}
void PDFPreviewImageProvider::closeAllDocs() { Concurrent::run(this, &PDFPreviewImageProvider::doCloseAllDocs); }
void PDFPreviewImageProvider::doCloseDoc(QString filePath) {
	renderMutex.lock();
	std::string filePathStd = filePath.toStdString();
	if (m_docs.find(filePathStd) != m_docs.end()) {
		delete m_docs.at(filePathStd);
		m_docs.erase(filePathStd);
	}
	renderMutex.unlock();
}
void PDFPreviewImageProvider::doCloseAllDocs() {
	renderMutex.lock();
	std::unordered_map<std::string, Poppler::Document *>::iterator it = m_docs.begin();
	while (it != m_docs.end()) {
		delete it->second;
		it++;
	}
	m_docs.clear();
	renderMutex.unlock();
}

bool GAPI::isNotesSupported() {
	BEGIN_TRY_CATCH;

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return false;

	try {
		// try read personal notes
		card->readPersonalNotes();
	} catch (PTEID_Exception &e) {
		if (e.GetError() == EIDMW_ERR_NOT_SUPPORTED)
			return false;
		else
			throw;
	};

	END_TRY_CATCH

	return true;
}

void GAPI::startCardReading() {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "StartCardReading connectToCard");
	QFuture<void> future = Concurrent::run(this, &GAPI::connectToCard);
}

void GAPI::startCardICAOReading() {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "StartCardReading connectToICAOCard");
	QFuture<void> future = Concurrent::run(this, &GAPI::connectToICAOCard);
}


void GAPI::startSavingCardPhoto(QString outputFile) {
	QFuture<void> future = Concurrent::run(this, &GAPI::doSaveCardPhoto, outputFile);
}

void GAPI::startWritingPersoNotes(const QString &text) {
	QFuture<void> future = Concurrent::run(this, &GAPI::setPersoDataFile, text);
}

int GAPI::getStringByteLength(const QString &text) {

	return (int)text.toStdString().size() + 1; // +1 for the NULL terminator
}

void GAPI::startGettingEntities() { Concurrent::run(this, &GAPI::getSCAPEntities); }

void GAPI::startGettingCompanyAttributes(bool useOauth) {
	Concurrent::run(this, &GAPI::getSCAPCompanyAttributes, useOauth);
}

void GAPI::startGettingEntityAttributes(QList<QString> entities_names, bool useOAuth) {
	Concurrent::run(this, &GAPI::getSCAPEntityAttributes, entities_names, useOAuth);
}

void GAPI::startReadingPersoNotes() { QFuture<void> future = Concurrent::run(this, &GAPI::getPersoDataFile); }

void GAPI::startReadingAddress() { Concurrent::run(this, &GAPI::getAddressFile); }

void GAPI::startLoadingAttributesFromCache(bool showSubAttributes) {
	Concurrent::run(this, &GAPI::getSCAPAttributesFromCache, showSubAttributes);
}

void GAPI::startRemovingAttributesFromCache() { Concurrent::run(this, &GAPI::removeSCAPAttributesFromCache); }

void GAPI::startRemoveCANCache() { Concurrent::run(this, &GAPI::removeCANCache); }
void GAPI::removeCANCache() {
	qDebug() << "removeCANCache";
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "CAN", "Remove CAN Cache");
	deleteCAN();
	emit signalRemoveCANCacheSucess();
}

void GAPI::startSigningSCAP(QList<QString> inputPDFs, QString outputPDF, int page, double location_x, double location_y,
							QString location, QString reason, bool isTimestamp, bool isLtv, bool isLastPage,
							QList<QString> attribute_ids, bool useProfessionalName) {

	SCAPSignParams signParams = {inputPDFs, outputPDF,	 page,	location_x, location_y,	  reason,
								 location,	isTimestamp, isLtv, isLastPage, useProfessionalName, attribute_ids};

	Concurrent::run(this, &GAPI::doSignSCAP, signParams, false);
}

void GAPI::signScapWithCMD(QList<QString> inputPDFs, QString outputPDF, QList<QString> attribute_ids, int page,
						   double location_x, double location_y, QString reason, QString location, bool isTimestamp,
						   bool isLtv, bool isLastPage, bool useProfessionalName) {

	SCAPSignParams signParams = {inputPDFs, outputPDF,	 page,	location_x, location_y,	  reason,
								 location,	isTimestamp, isLtv, isLastPage, useProfessionalName, attribute_ids};

	Concurrent::run(this, &GAPI::doSignSCAP, signParams, true);
}

static std::vector<ScapAttribute> get_attributes_by_ids(const std::vector<std::string> &ids) {
	const auto all_attributes = ScapClient::readAttributeCache().data();

	std::vector<ScapAttribute> result;
	for (const ScapAttribute &a : all_attributes) {
		if (std::find(ids.begin(), ids.end(), a.unique_id) != ids.end()) {
			result.push_back(a);
		}
	}

	return result;
}

const std::string SCAP_FUNCIONARIOS_ENTITY_NAME = "SCAP Funcionários";

template <typename T> bool GAPI::handleScapError(const ScapResult<T> &result, bool isCompany, bool isEmployee) {
	QList<QString> error_data;
	for (const std::string &provider : result.error_data()) {
		error_data.append(QString::fromStdString(provider));
	}

	bool update_attributes = false;
	ScapError error = result.error();
	if (error == ScapError::generic) {
		emit signalSCAPServiceFail(ScapGenericError, isCompany);
	} else if (error == ScapError::connection) {
		emit signalSCAPConnectionFailed();
	} else if (error == ScapError::proxy_auth) {
		emit signalSCAPProxyAuthRequired();
	} else if (error == ScapError::possibly_proxy) {
		emit signalSCAPPossibleProxyMisconfigured();
	} else if (error == ScapError::sign_cmd_connection) {
		showSignCMDDialog(EIDMW_ERR_CMD_CONNECTION);
	} else if (error == ScapError::sign_cmd_invalid_code) {
		showSignCMDDialog(EIDMW_ERR_CMD_INVALID_CODE);
	} else if (error == ScapError::sign_cmd_invalid_account) {
		showSignCMDDialog(EIDMW_ERR_CMD_INACTIVE_ACCOUNT);
	} else if (error == ScapError::sign_cmd_generic) {
		showSignCMDDialog(EIDMW_ERR_CMD_SERVICE);
	} else if (error == ScapError::sign_timestamp) {
		emit signalPdfSignSuccess(SignMessageTimestampFailed);
	} else if (error == ScapError::sign_ltv) {
		emit signalPdfSignSuccess(SignMessageLtvFailed);
	} else if (error == ScapError::sign_unsupported_pdf) {
		if (!error_data.empty()) {
			// error on batch signature comes with failed filename
			emit signalPdfBatchSignFail(PDFFileUnsupported, error_data.first());
		} else {
			emit signalPdfSignFail(PDFFileUnsupported, -1);
		}
	} else if (error == ScapError::sign_permission_denied) {
		if (!error_data.empty()) {
			// error on batch signature comes with failed filename
			emit signalPdfBatchSignFail(SignFilePermissionFailed, error_data.first());
		} else {
			emit signalPdfSignFail(SignFilePermissionFailed, -1);
		}
	} else if (error == ScapError::sign_pin_blocked) {
		emit signalCardAccessError(PinBlocked);
	} else if (error == ScapError::sign_pin_cancel) {
		emit signalCardAccessError(CardUserPinCancel);
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature", "Operation canceled.");
	} else if (error == ScapError::sign_cancel) {
		emit signalCanceledSignature();
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature", "Operation canceled.");
	} else if (error == ScapError::oauth_cancelled) {
		emit signalEndOAuth(OAuthCancelled);
	} else if (error == ScapError::oauth_timeout) {
		emit signalEndOAuth(OAuthTimeoutError);
	} else if (error == ScapError::oauth_connection) {
		emit signalEndOAuth(OAuthConnectionError);
	} else if (error == ScapError::oauth_failure) {
		emit signalEndOAuth(OAuthGenericError);
	} else if (error == ScapError::timeout) {
		emit signalSCAPServiceFail(ScapTimeOutError, isCompany);
	} else if (error == ScapError::incomplete_response) {
		emit signalSCAPIncompleteResponse(error_data);
		update_attributes = true;
	} else if (error == ScapError::no_attributes) {
		// the new scap service does not provide a proper way to determine if getting
		// employee attributes succeeded or failed
		if (isEmployee) {
			error_data.push_back(QString::fromStdString(SCAP_FUNCIONARIOS_ENTITY_NAME));
		}

		emit signalSCAPNoAttributesResponse(error_data, isCompany);
		update_attributes = true;
	} else if (error == ScapError::expired_attributes) {
		emit signalSCAPServiceFail(ScapAttributesExpiredError, isCompany);
	} else if (error == ScapError::invalid_attributes) {
		emit signalSCAPServiceFail(ScapNotValidAttributesError, isCompany);
	} else if (error == ScapError::bad_secret_key) {
		emit signalSCAPServiceFail(ScapSecretKeyError, isCompany);
	} else if (error == ScapError::clock) {
		emit signalSCAPServiceFail(ScapClockError, isCompany);
	} else if (error == ScapError::bad_credentials) {
		emit signalSCAPBadCredentials();
	} else if (error == ScapError::cache_removed_legacy) {
		emit signalCacheRemovedLegacy();
	} else if (error == ScapError::cache_read_failure) {
		emit signalCacheNotReadable();
	} else if (error == ScapError::cache_write_failure) {
		emit signalCacheNotWritable();
	} else {
		emit signalSCAPServiceFail(ScapGenericError, isCompany);
	}

	return update_attributes;
}

void GAPI::doSignSCAP(const SCAPSignParams &params, bool isCMD) {
	BEGIN_TRY_CATCH

	PTEID_SigningDevice *device = NULL;
	if (isCMD) {
		device = m_cmd_client;
	} else {
		PTEID_EIDCard *card = NULL;
		getCardInstance(card);
		device = card;
	}

	std::vector<std::string> ids;
	foreach (QString id, params.attribute_ids) {
		ids.push_back(id.toStdString());
	}

	std::vector<ScapAttribute> attributes = get_attributes_by_ids(ids);

	PTEID_SignatureLevel level = PTEID_LEVEL_BASIC;
	if (params.isLtv) {
		level = PTEID_LEVEL_LTV;
	} else if (params.isTimestamp) {
		level = PTEID_LEVEL_TIMESTAMP;
	}

	SealGeometry seal_geometry = {static_cast<unsigned int>(m_seal_width), static_cast<unsigned int>(m_seal_height),
								  params.location_x, params.location_y};

	bool visible = seal_geometry.x != -1 && seal_geometry.y != -1;

	std::vector<std::string> input;
	foreach (QString file_name, params.inputPDFs) {
		input.push_back(file_name.toStdString());
	}

	PDFSignatureInfo signature_info = {input,
									   params.outputPDF.toStdString(),
									   params.location.toStdString(),
									   params.reason.toStdString(),
									   level,
									   visible,
									   params.page,
									   params.isLastPage,
									   seal_geometry,
									   useCustomSignature(),
									   params.useProfessionalName,
									   reinterpret_cast<unsigned char *>(m_jpeg_scaled_data.data()),
									   static_cast<unsigned long>(m_jpeg_scaled_data.size())};

	const auto result = m_scap_client->sign(device, signature_info, attributes);
	if (result.is_error()) {
		handleScapError(result);
		return;
	}

	emit signalPdfSignSuccess(SignMessageOK);

	updateTelemetry(isCMD ? TelemetryAction::SignCMDScap : TelemetryAction::SignCCScap);

	END_TRY_CATCH
}

void GAPI::getSCAPEntities() {
	const auto result = m_scap_client->getAttributeProviders();
	if (result.is_error()) {
		handleScapError(result);
		return;
	}

	const auto &providers = result.data();
	if (providers.size() == 0) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature", "getAttributeProviders returned zero");
		emit signalSCAPServiceFail(ScapGenericError, false);
		return;
	}

	QList<QString> provider_names;
	for (size_t i = 0; i < providers.size(); ++i) {
		// temporary workaround: compare uriId of employee providers, they seem to come with the wrong type
		if (providers[i].type == "EMPLOYEE" || providers[i].uriId == "http://interop.gov.pt/SCAP/FAF") { // hammer time
			provider_names.append(QString::fromStdString(SCAP_FUNCIONARIOS_ENTITY_NAME));
		} else {
			provider_names.append(QString::fromStdString(providers[i].name));
		}
	}

	emit signalSCAPEntitiesLoaded(provider_names);
}

bool isSpecialValidity(QString value) {

	// Search AAAA/MM/DD, AAAA-MM-DD, AAAA MM DD
	QRegExp dateFormat("(9999|99|12|31)(/|-| *)(9999|99|12|31)(/|-| *)(9999|99|12|31)");

	return dateFormat.indexIn(value) != -1;
}

static std::map<std::string, std::vector<std::string>>
format_scap_attr_strings(const std::vector<ScapAttribute> &attributes, bool showSubAttributes) {
	std::map<std::string, std::vector<std::string>> result;

	if (attributes.size() == 0) {
		return result;
	}

	// In the SCAP settings page we show sub-attributes but don't need to split citizen name and attribute
	const std::string separator = showSubAttributes ? " - " : " [at_name] ";

	for (size_t i = 0; i < attributes.size(); ++i) {
		const ScapAttribute &attribute = attributes[i];

		std::string provider_name = attribute.provider.name;
		std::string attr_string = attribute.citizen_name + separator;
		attr_string += attribute.description + " ";

		if (showSubAttributes) {
			if (!attribute.sub_attributes.empty()) {
				attr_string += "(";
			}

			for (auto it = attribute.sub_attributes.begin(); it != attribute.sub_attributes.end(); ++it) {
				// if (!isSpecialValidity(QString::fromStdString(it->value))) { //TODO: review this function, it is
				// matching things it shouldn't...
				attr_string += it->description + ": " + it->value;
				//}
				attr_string += (std::next(it) != attribute.sub_attributes.end()) ? ", " : ")";
			}

			if (attribute.provider.type == "EMPLOYEE") {
				provider_name = SCAP_FUNCIONARIOS_ENTITY_NAME;
			}
		} else {
			// send id of each attribute to signature page
			// using an uncommon sequence to split id from attribute description , for now
			attr_string += " [id] " + attribute.unique_id;
		}

		result[provider_name].push_back(attr_string);
	}

	return result;
}

static std::vector<ScapProvider> filter_providers_by_names(const std::vector<ScapProvider> &providers,
														   const std::vector<std::string> &names) {
	std::vector<ScapProvider> result;

	for (const ScapProvider &p : providers) {
		if (std::find(names.begin(), names.end(), p.name) != names.end()) {
			result.push_back(p);
		}
	}

	return result;
}

static QVariantMap map_of_string_vectors_to_qvariant_map(const std::map<std::string, std::vector<std::string>> &map) {
	QVariantMap result;

	for (const auto &x : map) {
		QList<QString> list;
		for (const auto &s : x.second) {
			list.append(QString::fromStdString(s));
		}

		result.insert(QString::fromStdString(x.first), QVariant::fromValue(list));
	}

	return result;
}

static std::vector<ScapAttribute> filter_attributes_by_type(const std::vector<ScapAttribute> &attributes,
															const std::vector<std::string> &types) {
	std::vector<ScapAttribute> result;

	for (const ScapAttribute &attribute : attributes) {
		if (std::find(types.begin(), types.end(), attribute.provider.type) != types.end()) {
			result.push_back(attribute);
		}
	}

	return result;
}

static std::pair<QVariantMap, QVariantMap> split_attributes_by_type(const std::vector<ScapAttribute> &all_attributes,
																	bool showSubAttributes) {
	std::vector<ScapAttribute> institution = filter_attributes_by_type(all_attributes, {"INSTITUTION", "EMPLOYEE"});
	std::vector<ScapAttribute> enterprise = filter_attributes_by_type(all_attributes, {"ENTERPRISE"});

	const auto institution_map = format_scap_attr_strings(institution, showSubAttributes);
	const auto enterprise_map = format_scap_attr_strings(enterprise, showSubAttributes);

	QVariantMap institutions_result = map_of_string_vectors_to_qvariant_map(institution_map);
	QVariantMap enterprise_result = map_of_string_vectors_to_qvariant_map(enterprise_map);

	return std::make_pair(institutions_result, enterprise_result);
}

void GAPI::getSCAPEntityAttributes(QList<QString> provider_names, bool useOAuth) {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "getSCAPEntityAttributes");

	PTEID_EIDCard *card = NULL;
	if (!useOAuth) {
		getCardInstance(card);
	}

	const auto get_providers_result = m_scap_client->getAttributeProviders();
	if (get_providers_result.is_error()) {
		handleScapError(get_providers_result);
		return;
	}

	std::vector<std::string> names;
	foreach (QString id, provider_names) {
		names.push_back(id.toStdString());
	}

	const auto &all_providers = get_providers_result.data();
	std::vector<ScapProvider> providers = filter_providers_by_names(all_providers, names);

	bool employee = std::find(names.begin(), names.end(), SCAP_FUNCIONARIOS_ENTITY_NAME) != names.end();

	const auto result = m_scap_client->getCitizenAttributes(card, providers, false, employee);
	bool update_attributes = (!result.is_error()) || handleScapError(result, false, employee);

	if (update_attributes) {
		const auto &attributes = result.data();
		const auto split_attributes = split_attributes_by_type(attributes, true);
		emit signalAttributesLoaded(split_attributes.first, split_attributes.second);
	}
}

void GAPI::getSCAPCompanyAttributes(bool useOAuth) {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "getSCAPCompanyAttributes");

	PTEID_EIDCard *card = NULL;
	if (!useOAuth) {
		getCardInstance(card);
	}

	const auto result = m_scap_client->getCitizenAttributes(card, {}, true, false);
	bool update_attributes = (!result.is_error()) || handleScapError(result, true);

	if (update_attributes) {
		const auto &attributes = result.data();
		const auto split_attributes = split_attributes_by_type(attributes, true);
		emit signalAttributesLoaded(split_attributes.first, split_attributes.second);
	}
}

bool GAPI::isAttributeExpired(std::string &date, std::string &supplier) {
	QRegExp dateFormat("^\\d{4}-\\d{2}-\\d{2}$"); // xsd:date format
	if (date.empty() || dateFormat.indexIn(date.c_str()) == -1) {
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui",
				  "%s: Bad date format in validity of attribute supplied by: '%s' -> '%s'", __FUNCTION__,
				  supplier.c_str(), date.c_str());
		return false;
	}

	std::string today;
	const char *format = "%Y-%m-%d"; // xsd:date format
	CTimestampUtil::getTimestamp(today, 0L, format);
	bool isExpired = today.compare(date) > 0; // today is after validity -> expired
	if (isExpired) {
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "%s: Attribute supplied by: '%s' may be expired -> '%s'",
				  __FUNCTION__, supplier.c_str(), date.c_str());
	}

	return isExpired;
}

void GAPI::getSCAPAttributesFromCache(bool showSubAttributes) {
	qDebug() << "getSCAPAttributesFromCache";
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "getSCAPAttributesFromCache");

	const auto all_attributes = ScapClient::readAttributeCache();
	if (all_attributes.is_error()) {
		handleScapError(all_attributes);
		return;
	}

	const auto split_attributes = split_attributes_by_type(all_attributes.data(), showSubAttributes);
	emit signalAttributesLoaded(split_attributes.first, split_attributes.second);

#if 0
    if (!possiblyExpiredSuppliers.empty()) {
        possiblyExpiredSuppliers.removeDuplicates();
        emit signalAttributesPossiblyExpired(possiblyExpiredSuppliers);
    }
#endif
}

void GAPI::removeSCAPAttributesFromCache() {
	qDebug() << "removeSCAPAttributesFromCache";
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "Remove SCAP Attributes From Cache");

	// Nothing more to do if cache dir doesn't exist - TODO: return success if cache does not exist
	if (m_scap_client->clearAttributeCache())
		emit signalRemoveSCAPAttributesSucess();
	else
		emit signalRemoveSCAPAttributesFail();
}

void GAPI::abortSCAPWithCMD() { m_scap_client->cancelOAuth(); }

void GAPI::getCardInstance(PTEID_EIDCard *&new_card) {

	try {
		unsigned long ReaderCount = ReaderSet.readerCount();
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance Card Reader count =  %ld", ReaderCount);
		unsigned long ReaderIdx = 0;
		long CardIdx = 0;
		unsigned long tempReaderIndex = 0;

		if (ReaderCount == 0) {
			emit signalCardAccessError(NoReaderFound);
		} else {

			// Count number of cards
			for (ReaderIdx = 0; ReaderIdx < ReaderCount; ReaderIdx++) {
				PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(ReaderIdx);
				try {
					if (readerContext.isCardPresent()) {
						CardIdx++;
						tempReaderIndex = ReaderIdx;
					}
				} catch (PTEID_Exception &e) {
					unsigned long err = e.GetError();
					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Failed to check is Card Present 0x%08x\n", err);
				}
			}
			// Test if Card Reader was previously selected
			if (selectedReaderIndex != -1) {
				// Card Reader was previously selected
				PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(selectedReaderIndex);
				PTEID_CardType CardType = readerContext.getCardType();
				qDebug() << "Card Reader was previously selected CardType:" << CardType;
				PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Card Reader was previously selected CardType: %d",
						  CardType);
				switch (CardType) {
				case PTEID_CARDTYPE_IAS07:
				case PTEID_CARDTYPE_IAS5: {
					PTEID_EIDCard &Card = readerContext.getEIDCard();
					new_card = &Card;
					if (readerContext.getCardContactInterface() == PTEID_CARD_CONTACTLESS)
						m_is_contactless = true;

					break;
				}
				case PTEID_CARDTYPE_UNKNOWN: {
					selectedReaderIndex = -1;
					emit signalCardAccessError(CardUnknownCard);
					break;
				}
				default:
					break;
				}
			} else { // Card Reader was not previously selected
				if (CardIdx == 1) {
					PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(tempReaderIndex);
					PTEID_CardType CardType = readerContext.getCardType();
					qDebug() << "Card Reader was not previously selected CardType:" << CardType;
					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Card Reader was not previously selected CardType: %d",
							  CardType);
					switch (CardType) {
					case PTEID_CARDTYPE_IAS07:
					case PTEID_CARDTYPE_IAS5: {
						PTEID_EIDCard &Card = readerContext.getEIDCard();
						new_card = &Card;
						if (readerContext.getCardContactInterface() == PTEID_CARD_CONTACTLESS)
							m_is_contactless = true;
						break;
					}

					case PTEID_CARDTYPE_UNKNOWN: {
						selectedReaderIndex = -1;
						emit signalCardAccessError(CardUnknownCard);
						break;
					}
					default:
						break;
					}
				} else if (CardIdx > 1) // Card Reader was not previously selected
				{
					qDebug() << "Card Reader was not previously selected. Ask user to select card";
					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui",
							  "Card Reader was not previously selected. "
							  "Ask user to select card");
					emit signalReaderContext(); // Ask user to select card
				} else {
					emit signalCardAccessError(NoCardFound);
				}
			}
		}
	} catch (PTEID_ExParamRange &e) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance failed with error code 0x%08x", e.GetError());
		emit signalCardAccessError(CardUnknownError);
	} catch (PTEID_ExNoCardPresent &e) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance failed with error code 0x%08x", e.GetError());
		emit signalCardAccessError(NoCardFound);
	} catch (PTEID_ExCardChanged &e) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance failed with error code 0x%08x", e.GetError());
		emit signalCardAccessError(CardUnknownError);
	} catch (PTEID_ExBadTransaction &e) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance failed with error code 0x%08x", e.GetError());
		emit signalCardAccessError(CardUnknownError);
	} catch (PTEID_ExCertNoRoot &e) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance failed with error code 0x%08x", e.GetError());
		emit signalCardAccessError(CardUnknownError);
	} catch (PTEID_Exception &e) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance failed with error code 0x%08x", e.GetError());
		emit signalCardAccessError(CardUnknownError);
	}
}

void GAPI::setReaderByUser(unsigned long setReaderIndex) {

	qDebug() << "GAPI::setReader!" << setReaderIndex;

	if (setReaderIndex >= ReaderSet.readerCount()) {
		setReaderIndex = ReaderSet.readerCount() - 1;
	}
	selectedReaderIndex = setReaderIndex;
}

QVariantList GAPI::getRetReaderList() {
	qDebug() << "GAPI::getRetReaderList!";

	QVariantList list;

	try {
		const char *const *ReaderList = ReaderSet.readerList();

		for (unsigned long Idx = 0; Idx < ReaderSet.readerCount(); Idx++) {
			const char *readerName = ReaderList[Idx];
			list << readerName;
		}
		if (selectedReaderIndex >= 0) {
			if ((unsigned long)selectedReaderIndex >= ReaderSet.readerCount()) {
				selectedReaderIndex = ReaderSet.readerCount() - 1;
			}
			emit signalSetReaderComboIndex(selectedReaderIndex);
		}

	} catch (PTEID_Exception&) {
		qDebug() << "Error getRetReaderList!";
	}

	return list;
}

bool GAPI::hasOnlyICAO() {
	unsigned long ReaderCount = 0;
	unsigned long ReaderIdx = 0;
	unsigned long tempReaderIndex = 0;
	PTEID_CardType cardType = PTEID_CARDTYPE_UNKNOWN;
	try {
		ReaderCount = ReaderSet.readerCount();
		if (ReaderCount == 0) {
			return false;
		}

		for (ReaderIdx = 0; ReaderIdx < ReaderCount; ReaderIdx++) {
			PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (readerContext.isCardPresent()) {
				tempReaderIndex = ReaderIdx;
				break;
			}
		}

		PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(tempReaderIndex);

		cardType = readerContext.getCardType();
	} catch (PTEID_Exception err) {
		qDebug() << "Not able to get card type! Error code: " << Qt::hex << err.GetError();
	}
	return cardType == ICAO_CARDTYPE_MRTD;
}

int GAPI::getReaderIndex(void) {
	qDebug() << "GAPI::getReaderIndex!" << selectedReaderIndex;
	if (selectedReaderIndex >= 0) {

		return selectedReaderIndex;
	}
	return 0;
}

bool GAPI::hasICAO() {
	unsigned long ReaderCount = 0;
	unsigned long ReaderIdx = 0;
	unsigned long tempReaderIndex = 0;
	bool hasIcao = false;
	try {
		ReaderCount = ReaderSet.readerCount();
		if (ReaderCount == 0) {
			return false;
		}

		for (ReaderIdx = 0; ReaderIdx < ReaderCount; ReaderIdx++) {
			PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (readerContext.isCardPresent()) {
				tempReaderIndex = ReaderIdx;
				break;
			}
		}

		PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(tempReaderIndex);
		auto &icaoCard = readerContext.getICAOCard();
		hasIcao = true;
	} catch (PTEID_Exception err) {
		qDebug() << "Card failed to read as ICAO doc. Error code: " << Qt::hex << err.GetError();
	}

	return hasIcao;
}

void GAPI::finishLoadingICAOCardData(ICAO_Card *card) {

	if (card == NULL) {
		qDebug() << "This ICAO card doesn't exist!";
		return;
	}

	PTEID_ICAO_DG1 *dg = card->readDataGroup1();

	if (dg == NULL) {
		qDebug() << "Couldn't read data group 1!";
		return;
	}

	qDebug() << "C++: loading ICAO Card Data";

	QMap<ICAOInfoKey, QString> cardData;

	cardData[DocumentCode] = QString::fromUtf8(dg->documentCode());
	cardData[IssuingState] = QString::fromUtf8(dg->issuingState());
	QString optionalData;
	if (dg->serialNumberCheckDigit() == -1) {
		optionalData = dg->optionalDataLine1();
	}

	cardData[DocumentNumber] =
		QString::fromUtf8(dg->documentNumber()) + (optionalData.size() > 0 ? " " + optionalData : "");
	cardData[OptionalDataLine1] = QString::fromUtf8(dg->optionalDataLine1());
	cardData[DateOfBirth] = convertDate(QString::fromUtf8(dg->dateOfBirth()));
	cardData[Gender] = QString(QChar(dg->sex()));
	cardData[DateOfExpiry] = convertDate(QString::fromUtf8(dg->dateOfExpiry()));
	cardData[Nat] = QString::fromUtf8(dg->nationality());
	cardData[OptionalDataLine2] = QString::fromUtf8(dg->optionalDataLine2());
	cardData[FullName] =
		QString::fromUtf8(dg->secondaryIdentifier()) + " " + QString::fromUtf8(dg->primaryIdentifier());
	cardData[IsNameFromMRZ] = "True";
	cardData[IsPassport] = QString::fromStdString(dg->isPassport() ? "True" : "False");

	// Load the photo from PTEID_ICAO_DG2	
	PTEID_ICAO_DG2* dg2 = card->readDataGroup2();
	if (!dg2->biometricInstances().empty()) {
		qDebug() << "Trying to process DG2";
		std::vector<PTEID_BiometricInfomation*> biometric_instances = dg2->biometricInstances();
		PTEID_BiometricInfomation* instance = biometric_instances.at(0);
		PTEID_FaceInfo* faceInfo = instance->faceInfo();
		PTEID_FaceInfoData* faceInfoData = faceInfo->faceInfoData().at(0);
		unsigned short image_data_type = faceInfoData->imgDataType();
		qDebug() << "DG2 image data type: " << image_data_type;
		QPixmap image_photo;
		if (image_data_type == 1) {
			// JPEG2000, which needs to be converted to PNG
			PTEID_ByteArray photoRawData = faceInfoData->photoRawDataPNG();
			image_photo.loadFromData(photoRawData.GetBytes(), photoRawData.Size(), "PNG");
		}
        else if (image_data_type == 0) {
			// JPG
			PTEID_ByteArray photoRawData = faceInfoData->photoRawData();
			image_photo.loadFromData(photoRawData.GetBytes(), photoRawData.Size(), "JPG");
		}
		// Updates image provider
		image_provider->setPixmap("photoICAO.png", image_photo);
    }

	auto availableDgs = card->getAvailableDatagroups();
	auto it = std::find(availableDgs.begin(), availableDgs.end(), PTEID_DATA_GROUP_ID_DG11);
	if (it != availableDgs.end()) {
		try {
			PTEID_ICAO_DG11 *dg11 = card->readDataGroup11();
			//All the DG11 tags are optional so we need to check for presence of fullname
			if (dg11 != NULL && strlen(dg11->fullName()) > 0) {
				cardData[FullName] = dg11->fullName();
				cardData[IsNameFromMRZ] = "False";
			}
		} catch (PTEID_Exception &err) {
			qDebug() << "Error while trying to read DG11! Error code: " << Qt::hex << err.GetError();
		}
	}

	setDataCardICAO(cardData);
}

QString GAPI::convertDate(const QString& date) {
    if (date.length() != 6) {
        return "Invalid date format";
    }

    QString year = date.mid(0, 2);  // YY
    QString month = date.mid(2, 2); // MM
    QString day = date.mid(4, 2);   // DD

    return QString("%1-%2-%3").arg(day, month, year);
}

void GAPI::finishLoadingCardData(PTEID_EIDCard *card) {
	card->doSODCheck(true); // Enable SOD checking
	PTEID_EId &eid_file = card->getID();

	qDebug() << "C++: loading Card Data";

	QMap<IDInfoKey, QString> cardData;

	cardData[Surname] = QString::fromUtf8(eid_file.getSurname());
	cardData[Givenname] = QString::fromUtf8(eid_file.getGivenName());
	cardData[Sex] = QString::fromUtf8(eid_file.getGender());
	cardData[Height] = QString::fromUtf8(eid_file.getHeight());
	cardData[Country] = QString::fromUtf8(eid_file.getCountry());
	cardData[Birthdate] = QString::fromUtf8(eid_file.getDateOfBirth());
	cardData[Father] =
		QString::fromUtf8(eid_file.getGivenNameFather()) + " " + QString::fromUtf8(eid_file.getSurnameFather());
	cardData[Mother] =
		QString::fromUtf8(eid_file.getGivenNameMother()) + " " + QString::fromUtf8(eid_file.getSurnameMother());
	cardData[AccidentalIndications] = QString::fromUtf8(eid_file.getAccidentalIndications());
	cardData[Documenttype] = QString::fromUtf8(eid_file.getDocumentType());
	cardData[Documentnum] = QString::fromUtf8(eid_file.getDocumentNumber());
	cardData[Documentversion] = QString::fromUtf8(eid_file.getDocumentVersion());
	cardData[Nationality] = QString::fromUtf8(eid_file.getNationality());
	cardData[Validityenddate] = QString::fromUtf8(eid_file.getValidityEndDate());
	cardData[Validitybegindate] = QString::fromUtf8(eid_file.getValidityBeginDate());
	cardData[PlaceOfRequest] = QString::fromUtf8(eid_file.getLocalofRequest());
	cardData[IssuingEntity] = QString::fromUtf8(eid_file.getIssuingEntity());
	cardData[NISS] = QString::fromUtf8(eid_file.getSocialSecurityNumber());
	cardData[NSNS] = QString::fromUtf8(eid_file.getHealthNumber());
	cardData[NIF] = QString::fromUtf8(eid_file.getTaxNo());
	cardData[NIC] = QString::fromUtf8(eid_file.getCivilianIdNumber());

	// Load photo into a QPixmap
	PTEID_ByteArray &photo = eid_file.getPhotoObj().getphoto();

	QPixmap image_photo;
	image_photo.loadFromData(photo.GetBytes(), photo.Size(), "PNG");

	image_provider->setPixmap("photo.png",image_photo);

	// All data loaded: we can emit the signal to QML
	setDataCardIdentify(cardData);

	// Load certificates here
	for (long unsigned int i = 0; i < ReaderSet.readerCount(); i++) {
		auto &reader = ReaderSet.getReaderByNum(i);
		if (reader.isCardPresent() && reader.getCardContactInterface() == PTEID_CARD_CONTACTLESS) {
			QString readerName = reader.getName();
			bool imported = m_Certificates.ImportCertificates(readerName);
			if (!imported) {
				PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "finishLoadingCardData", "ImportCertificates failed!");
				emit signalImportCertificatesFail();
			}
		}
	}
}

void GAPI::performPACEWithCache(CardOperation op) {
	if (!m_Settings.getEnablePteidCANCache()) {
		emit signalContactlessCANNeeded();
		return;
	}

	const int CAN_LENGTH = 6;
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Reading cached CAN");

	std::string cached_can = getCANFromCache();
	if (cached_can.size() == CAN_LENGTH) {
		QString pace_can = QString::fromStdString(cached_can);
		Concurrent::run(this, &GAPI::doStartPACEAuthentication, pace_can, op);
	} else {
		if (cached_can.size() > 0) {
			deleteCAN();
		}
		emit signalContactlessCANNeeded();
	}
}

void GAPI::connectToCard() {

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance connectToCard");

	BEGIN_TRY_CATCH
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;
	if (!m_is_contactless || m_pace_auth_state == PaceAuthenticated) {
		finishLoadingCardData(card);
	} else {
		performPACEWithCache(CardOperation::IdentityData);
	}

	END_TRY_CATCH
}

void GAPI::connectToICAOCard() {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance connectToICAOCard");
	ICAO_Card* card = NULL;
	unsigned long ReaderCount = ReaderSet.readerCount();
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "getCardInstance Card Reader count =  %ld", ReaderCount);
	unsigned long ReaderIdx = 0;
	unsigned long tempReaderIndex = 0;

	if (ReaderCount == 0) {
		emit signalCardAccessError(NoReaderFound);
	}
	
	for(ReaderIdx = 0; ReaderIdx < ReaderCount; ReaderIdx++) {
		PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(ReaderIdx);
		if (readerContext.isCardPresent()) {
			tempReaderIndex = ReaderIdx;
			break;
		}
	}

	PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(tempReaderIndex);
	PTEID_CardType cardType = readerContext.getCardType();

	if (cardType == ICAO_CARDTYPE_MRTD || cardType == PTEID_CARDTYPE_IAS5) {
		card = &readerContext.getICAOCard();
	}
	if (card == NULL)
		return;
	std::string masterListFile =
		utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR)) + "/IcaoMasterList.ml";
	card->loadMasterList(masterListFile.c_str());
	if (m_pace_auth_state == PaceAuthenticated) {
		finishLoadingICAOCardData(card);
	} else {
		performPACEWithCache(ICAOData);
		return;
	}
}

//****************************************************
// Callback function used by the Readercontext to notify insertion/removal of a card
// The callback comes at:
// - startup
// - insertion of a card
// - removal of a card
// - add/remove of a cardreader
// When a card is inserted we post a signal to the GUI telling that
// a new card is inserted.
//****************************************************
void cardEventCallback(long lRet, unsigned long ulState, CallBackData *pCallBackData) {
	g_runningCallback++;
	Q_UNUSED(lRet);
	try {
		PTEID_ReaderContext &readerContext = ReaderSet.getReaderByName(pCallBackData->getReaderName().toLatin1());

		unsigned int cardState = (unsigned int)ulState & 0x0000FFFF;
		unsigned int eventCounter = ((unsigned int)ulState) >> 16;

		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback",
				  "Card Event received for reader %s: cardState: %u Event Counter: %u",
				  pCallBackData->getReaderName().toUtf8().constData(), cardState, eventCounter);

		//------------------------------------
		// is card removed from the reader?
		//------------------------------------
		if (!readerContext.isCardPresent()) {

			if (pCallBackData->getMainWnd()->m_Settings.getRemoveCert()) {
				PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "Will try to RemoveCertificates...");
				bool bImported =
					pCallBackData->getMainWnd()->m_Certificates.RemoveCertificates(pCallBackData->getReaderName());

				if (!bImported) {
					qDebug() << "RemoveCertificates fail";

					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "RemoveCertificates failed!");
					emit pCallBackData->getMainWnd()->signalRemoveCertificatesFail();
				}
			}

			//------------------------------------
			// send an event to the main app to show the popup message
			//------------------------------------
			if (pCallBackData->getMainWnd()->returnReaderSelected() != -1) {
				pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_CHANGED);
			} else {
				pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_REMOVED);
			}
			pCallBackData->getMainWnd()->setAddressLoaded(false);
			pCallBackData->getMainWnd()->resetReaderSelected();
			pCallBackData->getMainWnd()->resetContactlessState();

			g_runningCallback--;
			return;
		}
		//------------------------------------
		// is card inserted ?
		//------------------------------------
		else if (readerContext.isCardChanged(pCallBackData->m_cardID)) {
			//------------------------------------
			// send an event to the main app to show the popup message
			//------------------------------------
			PTEID_CardType cardType = readerContext.getCardType();
			if (cardType == ICAO_CARDTYPE_MRTD) {
				pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_ICAO);
			} else {
				pCallBackData->getMainWnd()->signalCardChanged(GAPI::ET_CARD_CHANGED);
			}
			pCallBackData->getMainWnd()->setAddressLoaded(false);
			pCallBackData->getMainWnd()->resetReaderSelected();

			//------------------------------------
			// register certificates when needed
			//------------------------------------
			if (pCallBackData->getMainWnd()->m_Settings.getRegCert()) {
				switch (cardType) {
				case PTEID_CARDTYPE_IAS101:
				case PTEID_CARDTYPE_IAS07:
				case PTEID_CARDTYPE_IAS5: {
					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "Will try to ImportCertificates...");
					auto &reader = ReaderSet.getReaderByName(pCallBackData->getReaderName().toLatin1().data());
					if (reader.isCardPresent() && reader.getCardContactInterface() == PTEID_CARD_CONTACTLESS) {
						PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback",
								  "Postponing ImportCertificates until PACE authentication...");
						break;
					}

					// We can't read serial number before PACE on contactless interface
					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Card inserted with serial number: %s",
							  readerContext.getEIDCard().getVersionInfo().getSerialNumber());

					bool bImported =
						pCallBackData->getMainWnd()->m_Certificates.ImportCertificates(pCallBackData->getReaderName());

					if (!bImported) {
						PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eventCallback", "ImportCertificates failed!");
						emit pCallBackData->getMainWnd()->signalImportCertificatesFail();
					}
					break;
				}
				case ICAO_CARDTYPE_MRTD:
					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback","ICAO CARD inserted");
					break;
				case PTEID_CARDTYPE_UNKNOWN:
					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "unknown Card - skipping ImportCertificates()");
					break;
				}
			}
		} else {
			PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eventCallback", "Event ignored");
		}
	} catch (...) {
		emit pCallBackData->getMainWnd()->signalCardAccessError(GAPI::CardUnknownError);
		// we catch ALL exceptions. This is because otherwise the thread throwing the exception stops
	}
	g_runningCallback--;
}

//*****************************************************
// update the readerlist. In case a reader is added to the machine
// at runtime.
//*****************************************************
void GAPI::updateReaderList(void) {
	//----------------------------------------------------
	// check if the number of readers is changed
	//----------------------------------------------------
	try {
		if (ReaderSet.isReadersChanged()) {
			stopAllEventCallbacks();
			ReaderSet.releaseReaders();

			if (0 == ReaderSet.readerCount()) {
				emit signalCardAccessError(NoReaderFound);
			}
		}
		if (0 == m_callBackHandles.size()) {
			setEventCallbacks();
		}
	} catch (...) {
		stopAllEventCallbacks();
		ReaderSet.releaseReaders();
	}
}
void GAPI::setEventCallbacks(void) {
	//----------------------------------------
	// for all the readers, create a callback such we can know
	// afterwards, which reader called us
	//----------------------------------------

	try {
		unsigned long maxcount = ReaderSet.readerCount(true);
		for (unsigned long Ix = 0; Ix < maxcount; Ix++) {
			void (*fCallback)(long lRet, unsigned long ulState, void *pCBData);

			const char *readerName = ReaderSet.getReaderName(Ix);
			PTEID_ReaderContext &readerContext = ReaderSet.getReaderByNum(Ix);
			CallBackData *pCBData = new CallBackData(readerName, this);

			fCallback = (void (*)(long, unsigned long, void *)) & cardEventCallback;

			m_callBackHandles[readerName] = readerContext.SetEventCallback(fCallback, pCBData);
			m_callBackData[readerName] = pCBData;
		}
	} catch (PTEID_Exception&)  {
		emit signalCardChanged(ET_UNKNOWN);
	}
}

//*****************************************************
// stop the event callbacks and delete the corresponding callback data
// objects.
//*****************************************************
void GAPI::stopAllEventCallbacks(void) {

	for (tCallBackHandles::iterator it = m_callBackHandles.begin(); it != m_callBackHandles.end(); it++) {
		PTEID_ReaderContext &readerContext = ReaderSet.getReaderByName(it.key().toLatin1());
		unsigned long handle = it.value();
		readerContext.StopEventCallback(handle);
	}
	m_callBackHandles.clear();
	cleanupCallbackData();
}

//*****************************************************
// cleanup the callback data
//*****************************************************
void GAPI::cleanupCallbackData(void) {

	while (g_runningCallback) {
#ifdef WIN32
		::Sleep(100);
#else
		::usleep(100000);
#endif
	}

	g_cleaningCallback = true;

	for (tCallBackData::iterator it = m_callBackData.begin(); it != m_callBackData.end(); it++) {
		CallBackData *pCallbackData = it.value();
		delete pCallbackData;
	}
	m_callBackData.clear();
	g_cleaningCallback = false;
}

/* findCardCertificate returns the index i of the certificate to be obtained with certificates.getCert(i).
	This is because the copy constructor for PTEID_Certificate is not implemented.
*/
int GAPI::findCardCertificate(QString issuedBy, QString issuedTo) {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL) {
		qDebug() << "Error: findCardCertificate should not be called if card is not inserted!";
		return -1;
	}
	PTEID_Certificates &certificates = card->getCertificates();
	unsigned long nCerts = certificates.countAll();
	for (unsigned long i = 0; i < nCerts; i++) {
		PTEID_Certificate &cert = certificates.getCert(i);
		std::string issuerName = cert.getIssuerName();
		std::string ownerName = cert.getOwnerName();
		if (issuerName == issuedBy.toUtf8().constData() && ownerName == issuedTo.toUtf8().constData()) {
			return i;
		}
	}
	return -1;
}
void GAPI::viewCardCertificate(QString issuedBy, QString issuedTo) {
#ifdef WIN32
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL) {
		qDebug() << "Error: viewCardCertificate should not be called if card is not inserted!";
		return;
	}
	PTEID_Certificates &certificates = card->getCertificates();
	unsigned long certIdx = findCardCertificate(issuedBy, issuedTo);
	if (certIdx < 0) {
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "viewCardCertificate: could not find certificate!");
		return;
	}
	PTEID_Certificate &cert = certificates.getCert(certIdx);
	PTEID_ByteArray certData = cert.getCertData();
	PCCERT_CONTEXT pCertContext =
		CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());
	if (!pCertContext) {
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "viewCardCertificate: Error creating cert context: %d",
				  GetLastError());
		return;
	}

	HWND appWindow = GetForegroundWindow();
	CryptUIDlgViewContext(CERT_STORE_CERTIFICATE_CONTEXT, pCertContext, appWindow, NULL, 0, NULL);
	if (pCertContext) {
		CertFreeCertificateContext(pCertContext);
	}
#else
	Q_UNUSED(issuedBy);
	Q_UNUSED(issuedTo);
#endif
}
void GAPI::exportCardCertificate(QString issuedBy, QString issuedTo, QString outputPath) {
	Concurrent::run(this, &GAPI::doExportCardCertificate, issuedBy, issuedTo, outputPath);
}
void GAPI::doExportCardCertificate(QString issuedBy, QString issuedTo, QString outputPath) {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL) {
		qDebug() << "Error: inspectCardCertificate should not be called if card is not inserted!";
		emit signalExportCertificates(false);
		return;
	}
	PTEID_Certificates &certificates = card->getCertificates();
	int certIdx = findCardCertificate(issuedBy, issuedTo);
	if (certIdx < 0) {
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "doExportCardCertificate: could not find certificate");
		emit signalExportCertificates(false);
		return;
	}
	unsigned long ulCertIdx = certIdx;
	PTEID_Certificate &cert = certificates.getCert(ulCertIdx);
	PTEID_ByteArray certData;
	cert.getFormattedData(certData);

	QFile file(outputPath);
	file.open(QIODevice::WriteOnly);
	int bytesWritten = file.write((const char *)certData.GetBytes(), certData.Size());
	file.close();
	if (bytesWritten < 0){
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "doExportCardCertificate: could not write certificate");
		emit signalExportCertificates(false);
		return;
	}
	unsigned long ulBytesWritten = bytesWritten;
	emit signalExportCertificates(ulBytesWritten == certData.Size());
}

void GAPI::buildTree(PTEID_Certificate &cert, bool &bEx, QVariantMap &certificatesMap) {
	QVariantMap certificatesMapChildren;
	static int level = 0;
	static int status = PTEID_CERTIF_STATUS_UNKNOWN;

	if (cert.isRoot()) {
		certificatesMapChildren.insert("OwnerName", cert.getOwnerName());
		certificatesMapChildren.insert("IssuerName", cert.getIssuerName());
		certificatesMapChildren.insert("ValidityBegin", cert.getValidityBegin());
		certificatesMapChildren.insert("ValidityEnd", cert.getValidityEnd());
		certificatesMapChildren.insert("KeyLength", QString::number(cert.getKeyLength()));
		certificatesMapChildren.insert("SerialNumber", cert.getSerialNumber());
		if (status != PTEID_CERTIF_STATUS_CONNECT && status != PTEID_CERTIF_STATUS_ERROR) {
			status = cert.getStatus(true, false);
		}
		certificatesMapChildren.insert("Status", status);

		certificatesMap.insert("level" + QString::number(level), certificatesMapChildren);
		certificatesMap.insert("levelCount", level + 1);
		level = 0;
		status = PTEID_CERTIF_STATUS_UNKNOWN;
	} else {
		certificatesMapChildren.insert("OwnerName", cert.getOwnerName());
		certificatesMapChildren.insert("IssuerName", cert.getIssuerName());
		certificatesMapChildren.insert("ValidityBegin", cert.getValidityBegin());
		certificatesMapChildren.insert("ValidityEnd", cert.getValidityEnd());
		certificatesMapChildren.insert("KeyLength", QString::number(cert.getKeyLength()));
		certificatesMapChildren.insert("SerialNumber", cert.getSerialNumber());

		if (status != PTEID_CERTIF_STATUS_CONNECT && status != PTEID_CERTIF_STATUS_ERROR) {
			status = cert.getStatus(true, false);
		}
		certificatesMapChildren.insert("Status", status);

		if (certificatesMap.contains("level" + QString::number(level))) {
			certificatesMap.insert("levelB" + QString::number(level), certificatesMapChildren);
		} else {
			certificatesMap.insert("level" + QString::number(level), certificatesMapChildren);
		}

		level++;

		try {
			buildTree(cert.getIssuer(), bEx, certificatesMap);
		} catch (PTEID_ExCertNoIssuer &) {
			bEx = true;
		}
	}
}

void GAPI::startfillCertificateList(void) {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	if (!m_is_contactless || m_pace_auth_state == PaceAuthenticated) {
		Concurrent::run(this, &GAPI::fillCertificateList);
	} else {
		performPACEWithCache(CardOperation::ReadCertDetails);
	}
}

void GAPI::startGetCardActivation(void) { Concurrent::run(this, &GAPI::getCertificateAuthStatus); }

void GAPI::getCertificateAuthStatus(void) {
	qDebug() << "getCertificateAuthStatus";

	QString returnString = getCardActivation();

	emit signalShowCardActivation(returnString);
}

void GAPI::startCCSignatureCertCheck() { Concurrent::run(this, &GAPI::checkCCSignatureCert); }

void GAPI::checkCCSignatureCert() {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "checkCCSignatureCert");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	emit signalStartCheckCCSignatureCert();

	PTEID_Certificate &cert = card->getCertificates().getCert(PTEID_Certificate::CITIZEN_SIGN);
	PTEID_CertifStatus certStatus = cert.getStatus();

	if (certStatus == PTEID_CertifStatus::PTEID_CERTIF_STATUS_SUSPENDED)
		emit signalSignCertSuspended();
	else if (certStatus == PTEID_CertifStatus::PTEID_CERTIF_STATUS_REVOKED)
		emit signalSignCertRevoked();
	else
		emit signalOkSignCertificate();

	END_TRY_CATCH
}

void GAPI::fillCertificateList(void) {
	bool noIssuer = false;
	QVariantMap certificatesMap;

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance fillCertificateList");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	PTEID_Certificates &certificates = card->getCertificates();

	certificatesMap.clear();
	buildTree(certificates.getCert(PTEID_Certificate::CITIZEN_AUTH), noIssuer, certificatesMap);

	buildTree(certificates.getCert(PTEID_Certificate::CITIZEN_SIGN), noIssuer, certificatesMap);
	if (noIssuer) {
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "Certificate chain couldn't be completed!");
		qDebug() << "Certificate chain couldn't be completed!";
		emit signalCertificatesFail();
	} else {
		emit signalCertificatesChanged(certificatesMap);
	}

	END_TRY_CATCH
}

void GAPI::validateCertificates() {
	PTEID_EIDCard *card = NULL;
	getCardInstance(card);

	if (card == NULL)
		return;

	if (!m_is_contactless || m_pace_auth_state == PaceAuthenticated) {
		Concurrent::run(this, &GAPI::doValidateCertificates);
	} else {
		performPACEWithCache(CardOperation::ValidateCertificate);
	}
}

void GAPI::doValidateCertificates() {
	QVariantMap certificatesMap, certificatesMapChildren;

	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetCardInstance doValidateCertificates");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);
	if (card == NULL)
		return;

	PTEID_Certificates &certificates = card->getCertificates();

	certificatesMap.clear();
	PTEID_Certificate &auth_cert = certificates.getAuthentication();

	certificatesMapChildren.insert("OwnerName", auth_cert.getOwnerName());
	certificatesMapChildren.insert("IssuerName", auth_cert.getIssuerName());
	certificatesMapChildren.insert("ValidityBegin", auth_cert.getValidityBegin());
	certificatesMapChildren.insert("ValidityEnd", auth_cert.getValidityEnd());
	certificatesMapChildren.insert("KeyLength", QString::number(auth_cert.getKeyLength()));
	certificatesMapChildren.insert("SerialNumber", auth_cert.getSerialNumber());
	certificatesMapChildren.insert("Status", auth_cert.getStatus(false, false));
	certificatesMap.insert("level0", certificatesMapChildren);
	certificatesMapChildren.clear();

	PTEID_Certificate &sign_cert = certificates.getSignature();
	certificatesMapChildren.insert("OwnerName", sign_cert.getOwnerName());
	certificatesMapChildren.insert("IssuerName", sign_cert.getIssuerName());
	certificatesMapChildren.insert("ValidityBegin", sign_cert.getValidityBegin());
	certificatesMapChildren.insert("ValidityEnd", sign_cert.getValidityEnd());
	certificatesMapChildren.insert("KeyLength", QString::number(sign_cert.getKeyLength()));
	certificatesMapChildren.insert("SerialNumber", sign_cert.getSerialNumber());
	certificatesMapChildren.insert("Status", sign_cert.getStatus(false, false));
	certificatesMap.insert("levelB0", certificatesMapChildren);

	emit signalCertificatesChanged(certificatesMap);

	END_TRY_CATCH
}

void GAPI::finishLoadingSignCertData(PTEID_EIDCard *card) {
	PTEID_Certificate &cert = card->getCertificates().getCert(PTEID_Certificate::CITIZEN_SIGN);
	QString ownerName = cert.getOwnerName();
	QString NIC = cert.getSubjectSerialNumber();

	// remove "BI" prefix and checkdigit
	int NIC_length = 8;
	NIC.replace("BI", "");
	NIC.truncate(NIC_length);

	emit signalSignCertDataChanged(ownerName, NIC);

	if (!cert.verifyDateValidity())
		emit signalSignCertExpired();
}

void GAPI::startGettingInfoFromSignCert() { Concurrent::run(this, &GAPI::getInfoFromSignCert); }

void GAPI::getInfoFromSignCert(void) {
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "GetInfoFromSignCert");

	BEGIN_TRY_CATCH

	PTEID_EIDCard *card = NULL;
	getCardInstance(card);

	if (card == NULL)
		return;

	if (!m_is_contactless || m_pace_auth_state == PaceAuthenticated) {
		finishLoadingSignCertData(card);
	} else {
		performPACEWithCache(CardOperation::SignCertificateData);
	}

	END_TRY_CATCH
}

QStringList GAPI::getWrappedText(QString text, int maxlines, int offset) {

	std::string _text = text.toLatin1().constData();

	std::vector<std::string> result =
		wrapString(_text, m_seal_width, m_font_size, MYRIAD_BOLD, maxlines, offset * m_font_size);

	QStringList wrapped;
	for (std::string s : result)
		wrapped.append(QString::fromLatin1(s.c_str()));

	return wrapped;
}

static std::vector<std::string> string_vector_from_qstringlist(const QList<QString> &qstrings) {
	std::vector<std::string> vector_of_strings;
	foreach (QString s, qstrings) {
		vector_of_strings.push_back(s.toStdString());
	}

	return vector_of_strings;
}

QVariantList GAPI::getSCAPAttributesText(QList<QString> qstring_ids) {
	std::vector<std::string> ids = string_vector_from_qstringlist(qstring_ids);
	std::vector<ScapAttribute> attributes = get_attributes_by_ids(ids);
	std::string professionalName = get_professional_name(attributes);

	const auto formatted_strings = format_scap_seal_strings(attributes);

	QVariantList result;
	result.append(QString::fromStdString(formatted_strings.first));
	result.append(QString::fromStdString(formatted_strings.second));
	result.append(m_font_size);
	result.append(QString::fromStdString(professionalName));

	return result;
}

QString GAPI::getSCAPProviderLogo(QList<QString> qstring_ids) {
	std::vector<std::string> ids = string_vector_from_qstringlist(qstring_ids);
	std::vector<ScapAttribute> attributes = get_attributes_by_ids(ids);
	return get_scap_image_path(attributes);
}

int GAPI::getSealFontSize(bool isReduced, QString reason, QString name, bool nic, bool date, QString location,
						  QString entities, QString attributes, unsigned int width, unsigned int height) {

	FontParams wrap_parameters = calculateFontParams(
		isReduced, reason.toUtf8().constData(), name.toUtf8().constData(), nic, date, location.toUtf8().constData(),
		entities.toUtf8().constData(), attributes.toUtf8().constData(), width, height);

	m_font_size = wrap_parameters.font_size;

	// qDebug() << "Seal Font Size = " << m_font_size;

	return m_font_size;
}

QString GAPI::getCachePath(void) { return m_Settings.getPteidCachedir(); }

bool GAPI::customSignImageExist(void) {
	int IMAGE_HEIGHT = 77;
	int IMAGE_WIDTH = 351;
	QString path = m_Settings.getPteidCachedir() + "/CustomSignPicture.jpg";

	if (QFile::exists(path)) {
		QImage customImage(path);
		if (customImage.height() == IMAGE_HEIGHT && customImage.width() == IMAGE_WIDTH) {
			return true;
		}

		customSignImageRemove();
		emit signalCustomSignImageRemoved();
	}

	return false;
}

void GAPI::customSignImageRemove(void) {
	QString path = m_Settings.getPteidCachedir() + "/CustomSignPicture.jpg";
	QFile::remove(path);
}

bool GAPI::useCustomSignature(void) {
	QString customImagePath = m_Settings.getPteidCachedir() + "/CustomSignPicture.jpg";
	QFile custom_image(customImagePath);

	if (m_Settings.getUseCustomSignature() && custom_image.exists()) {
		qDebug() << "Using Custom Picture to sign a PDF";
		custom_image.open(QIODevice::ReadOnly);
		m_jpeg_scaled_data = custom_image.readAll();
		custom_image.close();
		return true;
	}

	qDebug() << "Using default Picture to CC sign";
	return false;
}

bool GAPI::saveCustomImageToCache(QString url) {
	int MAX_IMAGE_HEIGHT = 77;
	int MAX_IMAGE_WIDTH = 351;

	// remove old image from cache
	if (customSignImageExist())
		customSignImageRemove();

	QString customImagePathCache = m_Settings.getPteidCachedir() + "/CustomSignPicture.jpg";

	QUrl source(url);
	QImage custom_image;

	if (custom_image.load(source.toLocalFile(), "JPG")) {
		if (custom_image.height() == MAX_IMAGE_HEIGHT && custom_image.width() == MAX_IMAGE_WIDTH) {
			return QFile::copy(source.toLocalFile(), customImagePathCache);
		}
	} else {
		custom_image.load(source.toLocalFile());
	}

	float RECOMMENDED_RATIO = MAX_IMAGE_HEIGHT / (float)MAX_IMAGE_WIDTH;
	float ACTUAL_RATIO = custom_image.height() / (float)custom_image.width();

	// Only scale image when needed
	if (custom_image.height() != MAX_IMAGE_HEIGHT || custom_image.width() != MAX_IMAGE_WIDTH) {
		// Fit custom image to available space, keeping aspect ratio
		if (ACTUAL_RATIO >= RECOMMENDED_RATIO)
			custom_image = custom_image.scaledToHeight(MAX_IMAGE_HEIGHT, Qt::SmoothTransformation);
		else
			custom_image = custom_image.scaledToWidth(MAX_IMAGE_WIDTH, Qt::SmoothTransformation);
	}

	// Create blank image
	QImage final_img = QImage(MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT, QImage::Format_RGB32);
	final_img.fill(QColor(Qt::white).rgb());
	QPainter painter(&final_img);

	// Center scaled custom image in the white background
	int position_x = (int)((MAX_IMAGE_WIDTH / 2) - custom_image.width() / 2);
	int position_y = (int)((MAX_IMAGE_HEIGHT / 2) - custom_image.height() / 2);
	painter.drawImage(position_x, position_y, custom_image);

	// Save custom image in cache, ready to be applied in a signature
	return final_img.save(customImagePathCache, "JPG", 100);
}
void GAPI::setUseCustomSignature(bool UseCustomSignature) { m_Settings.setUseCustomSignature(UseCustomSignature); }
bool GAPI::getUseCustomSignature(void) { return m_Settings.getUseCustomSignature(); }
void GAPI::setRegCertValue(bool bRegCert) { m_Settings.setRegCert(bRegCert); }
void GAPI::setRemoveCertValue(bool bRemoveCert) { m_Settings.setRemoveCert(bRemoveCert); }
bool GAPI::getRegCertValue(void) { return m_Settings.getRegCert(); }
bool GAPI::getRemoveCertValue(void) { return m_Settings.getRemoveCert(); }
void GAPI::setUseNumId(bool UseNumId) { m_Settings.setUseNumId(UseNumId); }
bool GAPI::getUseNumId(void) { return m_Settings.getUseNumId(); }
void GAPI::setUseDate(bool UseDate) { m_Settings.setUseDate(UseDate); }
bool GAPI::getUseDate(void) { return m_Settings.getUseDate(); }
void GAPI::resizePDFSignSeal(unsigned int width, unsigned int height) {
	// qDebug() << "C++: Resize sign seal. Width: " << width << " Height:" <<height;

	m_seal_width = width;
	m_seal_height = height;
}
#ifdef WIN32
QVariantList GAPI::getRegisteredCmdPhoneNumbers() {
	QVariantList regCmdNumList;

	std::vector<std::string> phoneNums;
	m_cmdCertificates->GetRegisteredPhoneNumbers(&phoneNums);

	for (size_t i = 0; i < phoneNums.size(); i++) {
		regCmdNumList << phoneNums[i].c_str();
	}
	return regCmdNumList;
}
#endif

void GAPI::doCancelCMDRegisterCert() {
#ifdef WIN32
	m_cmdCertificates->CancelImport();
#endif
}
void GAPI::cancelCMDRegisterCert() {
#ifdef WIN32
	Concurrent::run(this, &GAPI::doCancelCMDRegisterCert);
#endif
}

void GAPI::registerCMDCertOpen(QString mobileNumber, QString pin) {
#ifdef WIN32
	Concurrent::run(this, &GAPI::doRegisterCMDCertOpen, mobileNumber, pin);
#else
	Q_UNUSED(mobileNumber);
	Q_UNUSED(pin);
#endif
}
void GAPI::doRegisterCMDCertOpen(QString mobileNumber, QString pin) {
#ifdef WIN32
	qDebug() << "Register CMD Cert of number " << mobileNumber;
	signalUpdateProgressBar(25);
	signalUpdateProgressStatus(tr("STR_CMD_CONNECTING"));
	int res = m_cmdCertificates->ImportCertificatesOpen(mobileNumber.toStdString(), pin.toStdString());

	QString error_msg;
	QString urlLink = "";
	// errors in cmdErrors.h
	switch (res) {
	case ERR_NONE:
		error_msg = tr("STR_CMD_LOGIN_SUCESS");
		signalUpdateProgressBar(50);
		signalUpdateProgressStatus(error_msg);
		emit signalValidateOtp();
		return;
	case SOAP_TCP_ERROR:
		error_msg = tr("STR_VERIFY_INTERNET");
		if (m_Settings.isProxyConfigured())
			error_msg.append(" ").append(tr("STR_VERIFY_PROXY"));
		break;
	case ERR_GET_CERTIFICATE:
		urlLink = tr("STR_URL_AUTENTICACAO_GOT_PT");
		error_msg = tr("STR_CMD_GET_CERTIFICATE_ERROR");
		break;
	default:
		urlLink = tr("STR_URL_AUTENTICACAO_GOT_PT");
		error_msg = tr("STR_CERT_REG_ERROR");
	}
	signalUpdateProgressBar(100);
	signalUpdateProgressStatus(tr("STR_POPUP_ERROR") + "!");
	emit signalShowMessage(error_msg, urlLink);
#else
	Q_UNUSED(mobileNumber);
	Q_UNUSED(pin);
#endif
}
void GAPI::registerCMDCertClose(QString otp) {
#ifdef WIN32
	Concurrent::run(this, &GAPI::doRegisterCMDCertClose, otp);
#else
	Q_UNUSED(otp);
#endif
}
void GAPI::doRegisterCMDCertClose(QString otp) {
#ifdef WIN32
	signalUpdateProgressBar(75);
	signalUpdateProgressStatus(tr("STR_CMD_SENDING_CODE"));
	int res = m_cmdCertificates->ImportCertificatesClose(otp.toStdString());

	QString top_msg = tr("STR_POPUP_ERROR") + "!";
	QString error_msg;
	QString urlLink = "";
	// errors in cmdErrors.h
	switch (res) {
	case ERR_NONE:
		top_msg = tr("STR_POPUP_SUCESS") + "!";
		error_msg = tr("STR_CERT_REG_SUCC");
		m_Settings.setAskToRegisterCmdCert(false);
		break;
	case SOAP_TCP_ERROR:
		error_msg = tr("STR_VERIFY_INTERNET");
		if (m_Settings.isProxyConfigured())
			error_msg.append(" ").append(tr("STR_VERIFY_PROXY"));
		break;
	default:
		error_msg = tr("STR_CERT_REG_ERROR");
		urlLink = tr("STR_URL_AUTENTICACAO_GOT_PT");
	}

	signalUpdateProgressBar(100);
	signalUpdateProgressStatus(top_msg);
	emit signalShowMessage(error_msg, urlLink);
#else
	Q_UNUSED(otp);
#endif
}

bool GAPI::areRootCACertsInstalled() {
#ifdef WIN32
	return Concurrent::run(&CERTIFICATES::IsNewRootCACertInstalled);
#else
	return false;
#endif
}

void GAPI::installRootCACert() {
#ifdef WIN32
	Concurrent::run(this, &GAPI::doInstallRootCACert);
#endif
}

void GAPI::doInstallRootCACert() {
#ifdef WIN32
	bool installed = m_Certificates.InstallNewRootCa();
	emit signalInstalledRootCACert(installed);
#endif
}

void GAPI::quitApplication(bool restart) {
	try {
		if (m_Settings.getRemoveCert()) {
			for (unsigned long readerCount = 0; readerCount < ReaderSet.readerCount(); readerCount++) {
				QString readerName = ReaderSet.getReaderName(readerCount);
				m_Certificates.RemoveCertificates(readerName);
			}
		}

		//-------------------------------------------------------------------
		// we must release all the certificate contexts before releasing the SDK.
		// After Release, no more calls should be done to the SDK and as such
		// noting should be done in the dtor
		//-------------------------------------------------------------------
		forgetAllCertificates();
		stopAllEventCallbacks();

	} catch (...) {
	}
	if (!Concurrent::waitForAll(30)) {
		// Threads did not finished and timeout occured: exit process
		// Some threads may not have been correctly finished.
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "eidgui", "Exiting application after timeout.");
	}
	qApp->exit((restart ? RESTART_EXIT_CODE : SUCCESS_EXIT_CODE));
}

//*****************************************************
// forget all the certificates we kept for all readers
//*****************************************************
void GAPI::forgetAllCertificates(void) {
#ifdef WIN32
	bool bRefresh = true;
	;
	for (unsigned long readerIdx = 0; readerIdx < ReaderSet.readerCount(bRefresh); readerIdx++) {
		const char *readerName = ReaderSet.getReaderByNum(readerIdx).getName();
		forgetCertificates(readerName);
	}
#endif
}

//*****************************************************
// forget all the certificates we kept for a specific reader
//*****************************************************
void GAPI::forgetCertificates(const QString &reader) {
	char readerName[256];
	readerName[0] = 0;
	if (reader.length() > 0) {
		strcpy(readerName, reader.toUtf8().data());
	}
#ifdef WIN32
	QVector<PCCERT_CONTEXT> readerCertContext = m_Certificates.m_certContexts[readerName];
	while (0 < readerCertContext.size()) {
		PCCERT_CONTEXT pContext = readerCertContext[readerCertContext.size() - 1];
		CertFreeCertificateContext(pContext);
		readerCertContext.erase(readerCertContext.end() - 1);
	}
#endif
}

void GAPI::setAppAsDlgParent(QObject *object, const QUrl &url) {
	QWindow *window = NULL;
	qDebug() << "QMLEngine objectCreated event: " << url;
	if (url.toString() == MAIN_QML_PATH && (window = qobject_cast<QWindow *>(object)) != NULL) {
#ifdef WIN32
		HWND appWindow = (HWND)window->winId();
		SetApplicationWindow(appWindow);
#else
		m_mainWnd = window;
#endif
	}
}

bool GAPI::checkCMDSupport() {
#if EIDGUIV2_CMD_SUPPORT
	return true;
#else
	return false;
#endif
}

QString GAPI::getAbsolutePath(QString path) {
	QFileInfo fileInfo(path);
	if (fileInfo.exists() && fileInfo.isRelative()) {
		return fileInfo.absoluteFilePath();
	}
	return path;
}
