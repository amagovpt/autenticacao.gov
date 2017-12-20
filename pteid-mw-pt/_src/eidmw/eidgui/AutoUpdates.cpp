/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2011-2017 Caixa Mágica Software.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.
 *
 * Author: Luis Medinas    <luis.medinas@caixamagica.pt>
 * Author: Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 **************************************************************************** */

#include <QtGui>
#include <QtNetwork>
#include <QProgressDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegExp>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QLabel>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

#include "eidlib.h"
#include "AutoUpdates.h"
#include "httpwindow.h"

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include "verinfo.h"
#include <QSysInfo>
#include <QNetworkProxy>
#else
#include "pteidversions.h"
#endif

using namespace eIDMW;


std::string remoteversion = "https://svn.gov.pt/projects/ccidadao/repository/middleware-offline/tags/builds/lastversion/version.txt";
std::string serverurl = "https://svn.gov.pt/projects/ccidadao/repository/middleware-offline/tags/builds/lastversion/";

std::string WINDOWS32 = "PteidMW-Basic.msi";
std::string WINDOWS64 = "PteidMW-Basic-x64.msi";
std::string MAC_OS = "pteid-mw.pkg";
std::string DEBIAN32 = "pteid-mw_debian_i386.deb";
std::string DEBIAN64 = "pteid-mw_debian_amd64.deb";
std::string UBUNTU_14_32 = "pteid-mw_ubuntu14_i386.deb";
std::string UBUNTU_16_32 = "pteid-mw_ubuntu16_i386.deb";
std::string UBUNTU_14_64 = "pteid-mw_ubuntu14_amd64.deb";
std::string UBUNTU_16_64 = "pteid-mw_ubuntu16_amd64.deb";
std::string FEDORA32 = "pteid-mw-fedora.i386.rpm";
std::string FEDORA64 = "pteid-mw-fedora.x86_64.rpm";
std::string SUSE32 = "pteid-mw-suse.i586.rpm";
std::string SUSE64 = "pteid-mw-suse.x86_64.rpm";


AutoUpdates::AutoUpdates(QWidget *parent)
: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
	QString ddtitle = tr("Software Update");
	statusLabel = new QLabel(tr("Do you want to check for updates?"));

	cancelButton = new QPushButton(tr("Cancel"));
	cancelButton->setDefault(true);
	downloadButton = new QPushButton(tr("Yes"));
	downloadButton->setAutoDefault(false);


	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(downloadButton, QDialogButtonBox::RejectRole);


	progressDialog = new QProgressDialog(this);

	connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadFile()));
	progressDialog->reset();

	topLayout = new QHBoxLayout();

	mainLayout = new QVBoxLayout();

	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(statusLabel);
	mainLayout->addWidget(buttonBox);
	setLayout(mainLayout);

	const QIcon app_icon = QIcon(":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png");
	setWindowIcon(app_icon);
	setWindowTitle(ddtitle);

}

AutoUpdates::~AutoUpdates()
{
	delete cancelButton;
	delete downloadButton;
	delete buttonBox;
	delete statusLabel;

	delete topLayout;
	delete mainLayout;
}

void AutoUpdates::cancelDownload()
{

	httpRequestAborted = true;
	reply->abort();

}

void AutoUpdates::startRequest(QUrl url)
{
	
    //10 second timeout
    int network_timeout = 10000;

	reply = qnam.get(QNetworkRequest(url));

    QTimer::singleShot(network_timeout, this, SLOT(cancelDownload()));

	connect(reply, SIGNAL(finished()),
			this, SLOT(httpFinished()));
	connect(reply, SIGNAL(readyRead()),
			this, SLOT(httpReadyRead()));
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
			this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void AutoUpdates::downloadFile()
{
	url = remoteversion.c_str();

	QFileInfo fileInfo(url.path());
	QString fileName = fileInfo.fileName();
	if (fileName.isEmpty())
	{
		QMessageBox::critical(this, tr("Software Update"),
				tr("Unable to download the update please check your Network Connection.")
		.arg(fileName).arg(file->errorString()));
	}

	std::string tmpfile;
	tmpfile.append(QDir::tempPath().toStdString());
	tmpfile.append("/");
	tmpfile.append(fileName.toStdString());

	file = new QFile(QString::fromUtf8((tmpfile.c_str())));
	if (!file->open(QIODevice::WriteOnly)) {
		QMessageBox::critical(this, tr("Software Update"),
				tr("Unable to save the file %1: %2.")
		.arg(fileName).arg(file->errorString()));
		delete file;
		file = 0;
		return;
	}

	progressDialog->setWindowTitle(tr("Software Update"));
	progressDialog->setLabelText(tr("Checking for newer versions"));
	downloadButton->setEnabled(false);

	// schedule the request
	httpRequestAborted = false;
	startRequest(url);
}

void AutoUpdates::httpFinished()
{

	if (httpRequestAborted) {
		if (file) {
			file->close();
			file->remove();
			delete file;
			file = 0;
		}

		QMessageBox::critical(this, tr("Software Update"),
				tr("Download failed. Please check your Network Connection."));

		reply->deleteLater();
		progressDialog->hide();

		this->close();
		return;
	}

	progressDialog->hide();
	file->flush();
	file->close();

	QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (reply->error()) {
		file->remove();
		QMessageBox::critical(this, tr("Software Update"),
				tr("Download failed. Please check your Network Connection.") );

		downloadButton->setEnabled(true);

        QString strLog = QString("AutoUpdates:: Download failed: ");
        strLog += reply->url().toString();
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", strLog.toStdString().c_str() );

	} else if (!redirectionTarget.isNull()) {
		QUrl newUrl = url.resolved(redirectionTarget.toUrl());
		if (QMessageBox::question(this, tr("Software Update"),
				tr("Redirect to %1 ?").arg(newUrl.toString()),
				QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			url = newUrl;
			reply->deleteLater();
			file->open(QIODevice::WriteOnly);
			file->resize(0);
			startRequest(url);
			return;
		}
	} else {
		downloadButton->setEnabled(true);
	}
	VerifyUpdates(filedata);
	reply->deleteLater();
	reply = 0;
	delete file;
	file = 0;
}

void AutoUpdates::httpReadyRead()
{
	// this slot gets called everytime the QNetworkReply has new data.
	// We read all of its new data and write it into the file.
	// That way we use less RAM than when reading it at the finished()
	// signal of the QNetworkReply

	QByteArray data = reply->readAll();
	QString qsdata(data);
	filedata = qsdata.toStdString();

	//Write to file
	if (file)
		file->write(data);
}

void AutoUpdates::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
	if (httpRequestAborted)
		return;

	progressDialog->setMaximum(totalBytes);
	progressDialog->setValue(bytesRead);
}

int compareVersions(PteidVersion v1, PteidVersion v2)
{

	unsigned int ret = 0;
	ret = v2.major - v1.major;
	if (ret != 0)
		return ret;
	ret = v2.minor - v1.minor;
	if (ret != 0)
		return ret;
	ret = v2.release - v1.release;
	return ret;
}


bool AutoUpdates::VerifyUpdates(std::string filedata)
{
	std::string distrover;
	std::string archver;

#ifdef WIN32
	QString filename = QCoreApplication::arguments().at(0);
	CFileVersionInfo VerInfo;
	char version[256];
	if(VerInfo.Open(filename.toLatin1()))
	{
		VerInfo.QueryStringValue(VI_STR_FILEVERSION, version);
	}
	QString ver = QString::fromLatin1(version);

#else
	QString ver (WIN_GUI_VERSION_STRING);
#endif

	QString remote_data(filedata.c_str());
	//Only consider the first line of version.txt
	remote_data = remote_data.left(remote_data.indexOf('\n'));
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "%s: Local version: %s", __FUNCTION__, ver.toUtf8().constData());
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "%s: Remote version: %s", __FUNCTION__, remote_data.toUtf8().constData());

	QStringList list1 = ver.split(",");

	QStringList list2 = remote_data.split(",");

	if (list2.size() < 3)
	{
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
			"%s: Wrong data returned from server or Proxy HTML error!", __FUNCTION__);

		this->close();
		return false;
	}

	//Parse local version into PteidVersion
	PteidVersion local_version;
	local_version.major = list1.at(0).toInt();
	local_version.minor = list1.at(1).toInt();
	local_version.release = list1.at(2).toInt();

	//Parse remote version into PteidVersion
	PteidVersion remote_version;
	remote_version.major = list2.at(0).toInt();
	remote_version.minor = list2.at(1).toInt();
	remote_version.release = list2.at(2).toInt();

	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "%s: Parsed versions: Remote= %d.%d.%d Local= %d.%d.%d", 
		__FUNCTION__,
		remote_version.major, remote_version.minor, remote_version.release, 
		local_version.major, local_version.minor, local_version.release);

	if (compareVersions(local_version, remote_version) > 0)
	{
		this->close();

		distrover = VerifyOS("distro");
        archver = VerifyOS("arch");
        ChooseVersion(distrover, archver);
        return true;
	}
	else {

		QMessageBox msgBoxnoupdates(QMessageBox::Information, tr("Software Update"),
			       	tr("No updates available at the moment"), 0, this);
		msgBoxnoupdates.exec();
		this->close();
		return false;
	}
}

bool AutoUpdates::FileExists(const char *filename)
{
	std::ifstream ifile(filename);
	return ifile.is_open();
}

std::string getDistroVersion(QString &content) {

	QRegExp rx = QRegExp("VERSION_ID=\"(\\d{2}).*\"");
	rx.setMinimal(true);

	rx.indexIn(content);

	QStringList list = rx.capturedTexts();

	if (list.size() > 1)
	{
		QString match = list.at(1);
		uint match_version = match.toUInt();

		if (match_version >= 16)
			return "Ubuntu16";
		else if (match_version >= 14)
			return "Ubuntu14";		
	}

	return "";
}


std::string AutoUpdates::VerifyOS(std::string param)
{
	std::string distrostr;
	std::string archstr;
	QRegExp rx;
	QStringList list;
	QString content;

    if( QSysInfo::WordSize == 64 )
        archstr = "x86_64";
    else
        archstr = "i386";

#ifdef WIN32
	distrostr = "windows";
	
#elif __APPLE__
    distrostr = "osx";
#else

    QFile osFile("/etc/os-release");
	if (!osFile.exists())
	{
		qDebug() << "Not Linux or too old distro!";
		distrostr = "unsupported";
		goto done;
	}

	if (!osFile.open(QFile::ReadOnly | QFile::Text))
		goto done; 

	rx = QRegExp("NAME=\"(.*)\"");
	content = osFile.readAll();

	rx.setMinimal(true);

	rx.indexIn(content);

	list = rx.capturedTexts();

	if (list.size() > 1)
	{
		distrostr = list.at(1).toStdString();

		if (distrostr == "Ubuntu") {
			distrostr = getDistroVersion(content);
		}
	}
#endif

	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "%s: retrieved distro = %s", __FUNCTION__, distrostr.c_str());
done:
	if (param == "distro")
		return distrostr;
	else
		return archstr;
}

void AutoUpdates::ChooseVersion(std::string distro, std::string arch)
{
	std::string downloadurl(serverurl);  

#ifdef WIN32
	if (arch == "i386") {
        downloadurl.append(WINDOWS32);
	}
	else {
        downloadurl.append(WINDOWS64);
	}
#elif __APPLE__

    downloadurl.append(MAC_OS);
#else
	if (distro == "unsupported" || (distro == "suse" && arch != "x86_64"))
	{
	  	QMessageBox msgBoxp(QMessageBox::Warning, tr("Warning"),
			tr("Your Linux distribution is not supported by Software Updates"), 0, this);
	  	msgBoxp.exec();
	}

    //Name of the deb/rpm will be distro specific
	if (arch == "x86_64")
	{
        if (distro == "Ubuntu14" || distro == "CaixaMagica")
        {
            downloadurl.append(UBUNTU_14_64);
        }
        else if (distro == "Ubuntu16") 
        {
        	downloadurl.append(UBUNTU_16_64);	
        }
		else if (distro == "fedora")
        {
            downloadurl.append(FEDORA64);
		}
		else if (distro == "suse")
		{
            downloadurl.append(SUSE64);
		}
	}
	else 
	{
		//32bits
        if (distro == "Ubuntu14" || distro == "CaixaMagica")
        {
            downloadurl.append(UBUNTU_14_32);
        }
        else if (distro == "Ubuntu16")
        {
            downloadurl.append(UBUNTU_16_32);
        }
		else if (distro == "fedora")
		{
            downloadurl.append(FEDORA32);
		}
	}
#endif	
	HttpWindow httpWin(downloadurl, distro);
	httpWin.show();
	httpWin.exec();
}

