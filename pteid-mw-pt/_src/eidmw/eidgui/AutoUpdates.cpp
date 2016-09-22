/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2011-2012 Caixa Mágica Software.
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
 * Author: Luis Medinas <luis.medinas@caixamagica.pt>
 **************************************************************************** */

#include <QtGui>
#include <QtNetwork>

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

std::string serverurl = "https://svn.gov.pt/projects/ccidadao/repository/middleware-offline/tags/builds/lastversion/";
std::string remoteversion = "https://svn.gov.pt/projects/ccidadao/repository/middleware-offline/tags/builds/lastversion/version.txt";

std::string WINDOWS32 = "PteidMW-Basic.msi";
std::string WINDOWS64 = "PteidMW-Basic-x64.msi";
std::string OSX32 = "pteidgui.dmg";
std::string OSX64 = "pteidgui.dmg";
std::string DEBIAN32 = "pteid-mw_debian_i386.deb";
std::string DEBIAN64 = "pteid-mw_debian_amd64.deb";
std::string UBUNTU32 = "pteid-mw_ubuntu_i386.deb";
std::string UBUNTU64 = "pteid-mw_ubuntu_amd64.deb";
std::string FEDORA32 = "pteid-mw-fedora.i386.rpm";
std::string FEDORA64 = "pteid-mw-fedora.x86_64.rpm";
std::string SUSE32 = "pteid-mw-suse.i586.rpm";
std::string SUSE64 = "pteid-mw-suse.x86_64.rpm";
std::string MANDRIVA32 = "pteid-mw-mandriva.i586.rpm";
std::string MANDRIVA64 = "pteid-mw-mandriva.x86_64.rpm";

struct PteidVersion
{
int major;
int minor;
int release;
};


AutoUpdates::AutoUpdates(QWidget *parent)
: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
	QString ddtitle = tr("Auto-update");
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

	QHBoxLayout *topLayout = new QHBoxLayout;

	QVBoxLayout *mainLayout = new QVBoxLayout;
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
}

void AutoUpdates::cancelDownload()
{
	httpRequestAborted = true;
	reply->abort();
}
void AutoUpdates::startRequest(QUrl url)
{
	eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
	eIDMW::PTEID_Config config2(eIDMW::PTEID_PARAM_PROXY_PORT);

    std::string proxy_host = config.getString();
    long proxy_port = config2.getLong();

	if (!proxy_host.empty())
	{
		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(QString::fromStdString(proxy_host));
		proxy.setPort(proxy_port);

		//proxy.setUser("username");
		//proxy.setPassword("password");
		QNetworkProxy::setApplicationProxy(proxy);
	}

	reply = qnam.get(QNetworkRequest(url));
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
		QMessageBox::information(this, tr("Auto-update"),
				tr("Unable to download the update please check your Network Connection.")
		.arg(fileName).arg(file->errorString()));
	}

	std::string tmpfile;
	tmpfile.append(QDir::tempPath().toStdString());
	tmpfile.append("/");
	tmpfile.append(fileName.toStdString());

	file = new QFile(QString::fromUtf8((tmpfile.c_str())));
	if (!file->open(QIODevice::WriteOnly)) {
		QMessageBox::information(this, tr("Auto-update"),
				tr("Unable to save the file %1: %2.")
		.arg(fileName).arg(file->errorString()));
		delete file;
		file = 0;
		return;
	}

	progressDialog->setWindowTitle(tr("Auto-update"));
	progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));
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
		reply->deleteLater();
		progressDialog->hide();
		return;
	}

	progressDialog->hide();
	file->flush();
	file->close();

	QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (reply->error()) {
		file->remove();
		QMessageBox::information(this, tr("Auto-update"),
				tr("Download failed: %1.")
		.arg(reply->errorString()));
		downloadButton->setEnabled(true);
	} else if (!redirectionTarget.isNull()) {
		QUrl newUrl = url.resolved(redirectionTarget.toUrl());
		if (QMessageBox::question(this, tr("Auto-update"),
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
	QString ver = QString::fromAscii(version);

#else

	QString ver (WIN_GUI_VERSION_STRING);
#endif

	QStringList list1 = ver.split(",");
	
	QStringList list2 = QString(filedata.c_str()).split(",");

	if (list2.size() < 3)
	{
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", 
			"AutoUpdates::VerifyUpdates: Wrong data returned from server or Proxy HTML error!");
		
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

	//return true;
	if (compareVersions(local_version, remote_version) > 0)
	{
		this->close();
#ifdef WIN32
		distrover = VerifyOS("distro", false);
#else
		distrover = VerifyOS("distro", true);
#endif
        archver = VerifyOS("arch", false);
        ChooseVersion(distrover, archver);
        return true;
	} 
	else {

		QMessageBox msgBoxnoupdates(QMessageBox::Information, tr("Auto-update"),
			       	tr("No updates available at the moment"), 0, this);
		msgBoxnoupdates.exec();
		this->close();
		return false;
	}
}

bool AutoUpdates::FileExists(const char *filename)
{
	std::ifstream ifile(filename);
	ifile.close();
	return ifile;
}

std::string AutoUpdates::VerifyOS(std::string param, bool runscript)
{
	std::string distrostr;
	std::string archstr;
#ifdef WIN32
	//check if it's Windows 32 or 64 bits
	distrostr = "windows";

	if( QSysInfo::WordSize == 64 )
		archstr = "x86_64";
	else
		archstr = "i386";

#elif __APPLE__
    //check if it's OSX 32 or 64 bits
    distrostr = "osx";

    if( QSysInfo::WordSize == 64 )
        archstr = "x86_64";
    else
        archstr = "i386";
#else

	if (runscript)
	{
		int sysret;
		sysret = system("/usr/local/bin/pteidlinuxversion.pl");
		if (sysret != 0)
			PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Problem trying to run pteidlinuxversion.pl");
	}

	try
	{
		if (FileExists("/tmp/linuxversion"))
		{
			std::ifstream linuxversion("/tmp/linuxversion");
			std::string line;

			while(std::getline(linuxversion, line))
			{
				std::stringstream   linestream(line);
				std::string         value;

				getline(linestream,value, ';');
				std::istringstream distro(value);
				distro >> distrostr;

				getline(linestream, value, ';');
				std::istringstream arch(value);
				arch >> archstr;
			}
		}
	} catch (...) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "can't open file /tmp/linuxversion");
	}

#endif
	if (param == "distro")
		return distrostr;
	else
		return archstr;
}

void AutoUpdates::ChooseVersion(std::string distro, std::string arch)
{
	std::string downloadurl;

    eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_AUTOUPDATES_URL);
    std::string configurl = config.getString();

    if (configurl.empty())
        downloadurl.append(serverurl);
    else
        downloadurl.append(configurl);

#ifdef WIN32
	if (arch == "i386")
	{
        downloadurl.append(WINDOWS32);
		HttpWindow httpWin(downloadurl, distro);
		httpWin.show();
		httpWin.exec();
	} else {
        downloadurl.append(WINDOWS64);
		HttpWindow httpWin(downloadurl, distro);
		httpWin.show();
		httpWin.exec();
	}    
#elif __APPLE__
    	if (arch == "i386")
    	{
            downloadurl.append(OSX32);
        	HttpWindow httpWin(downloadurl, distro);
        	httpWin.show();
        	httpWin.exec();
    	} else {
            downloadurl.append(OSX64);
        	HttpWindow httpWin(downloadurl, distro);
        	httpWin.show();
        	httpWin.exec();
    	}
#else

	if (distro == "unsupported")  
	{
	  	QMessageBox msgBoxp(QMessageBox::Warning, tr("Warning"), 
			tr("Your Linux distribution is not supported by Auto-updates"), 0, this);
	  	msgBoxp.exec();
	}

    //Name of the deb/rpm the rest of the appends will be distro specific

	if (arch == "x86_64")
	{
		if (distro == "debian")
		{
            downloadurl.append(DEBIAN64);
			HttpWindow httpWin(downloadurl, distro);
			httpWin.show();
			httpWin.exec();
			//delete httpWin;
		}
        if (distro == "Ubuntu" || distro == "CaixaMagica")
        {
            downloadurl.append(UBUNTU64);
            HttpWindow httpWin(downloadurl, distro);
            httpWin.show();
            httpWin.exec();
            //delete httpWin;
        }
		else if (distro == "fedora")
        {
            downloadurl.append(FEDORA64);
			HttpWindow httpWin(downloadurl, distro);
			httpWin.show();
			httpWin.exec();
		}
		else if (distro == "suse")
		{
                downloadurl.append(SUSE64);
		    	HttpWindow httpWin(downloadurl, distro);
		    	httpWin.show();
		    	httpWin.exec();
		}
        else if (distro == "mandriva")
        {
                downloadurl.append(MANDRIVA64);
                HttpWindow httpWin(downloadurl, distro);
                httpWin.show();
                httpWin.exec();
        }

	} else {
		//32bits
		if (distro == "debian")
		{
            downloadurl.append(DEBIAN32);
			HttpWindow httpWin(downloadurl, distro);
			httpWin.show();
			httpWin.exec();
		}
        if (distro == "Ubuntu" || distro == "CaixaMagica")
        {
            downloadurl.append(UBUNTU32);
            HttpWindow httpWin(downloadurl, distro);
            httpWin.show();
            httpWin.exec();
            //delete httpWin;
        }
		else if (distro == "fedora")
		{
            downloadurl.append(FEDORA32);
			HttpWindow httpWin(downloadurl, distro);
			httpWin.show();
			httpWin.exec();
		}
		else if (distro == "suse")
		{
                downloadurl.append(SUSE32);
		    	HttpWindow httpWin(downloadurl, distro);
		    	httpWin.show();
		    	httpWin.exec();
		}
        else if (distro == "mandriva")
        {
                downloadurl.append(MANDRIVA32);
                HttpWindow httpWin(downloadurl, distro);
                httpWin.show();
                httpWin.exec();
        }
	}
#endif
}

