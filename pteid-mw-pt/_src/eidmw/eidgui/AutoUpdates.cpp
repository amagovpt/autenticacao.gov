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
#else
#include "pteidversions.h"
#endif

using namespace eIDMW;

std::string ddtitle ("Cartão de Cidadão");
std::string serverurl = "http://people.caixamagica.pt/lmedinas/autoupdates/";
std::string remoteversion = "http://people.caixamagica.pt/lmedinas/autoupdates/versioninferior.txt";

AutoUpdates::AutoUpdates(QWidget *parent)
: QDialog(parent)
{
	statusLabel = new QLabel(tr("Do you want to check for updates ?"));

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

	setWindowTitle(QString::fromUtf8(ddtitle.c_str()));
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
		QMessageBox::information(this, QString::fromUtf8(ddtitle.c_str()),
				tr("Unable to download the update please check your Network Connection.")
		.arg(fileName).arg(file->errorString()));
	}

	std::string tmpfile;
	tmpfile.append(QDir::tempPath().toStdString());
	tmpfile.append("/");
	tmpfile.append(fileName.toStdString());

	file = new QFile(QString::fromUtf8((tmpfile.c_str())));
	if (!file->open(QIODevice::WriteOnly)) {
		QMessageBox::information(this, QString::fromUtf8(ddtitle.c_str()),
				tr("Unable to save the file %1: %2.")
		.arg(fileName).arg(file->errorString()));
		delete file;
		file = 0;
		return;
	}

	progressDialog->setWindowTitle(QString::fromUtf8(ddtitle.c_str()));
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

	VerifyUpdates(filedata);
	progressDialog->hide();
	file->flush();
	file->close();

	QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (reply->error()) {
		file->remove();
		QMessageBox::information(this, QString::fromUtf8(ddtitle.c_str()),
				tr("Download failed: %1.")
		.arg(reply->errorString()));
		downloadButton->setEnabled(true);
	} else if (!redirectionTarget.isNull()) {
		QUrl newUrl = url.resolved(redirectionTarget.toUrl());
		if (QMessageBox::question(this, QString::fromUtf8(ddtitle.c_str()),
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

	if (file)
		file->write(reply->readAll());
}

void AutoUpdates::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
	if (httpRequestAborted)
		return;

	progressDialog->setMaximum(totalBytes);
	progressDialog->setValue(bytesRead);
}

bool AutoUpdates::VerifyUpdates(std::string filedata)
{
    	std::string distrover;
	std::string archver;
	QString strVersion (WIN_GUI_VERSION_STRING);
	std::string ver = strVersion.toStdString();
	ver.replace(2,1,"");
	ver.replace(3,1,"");
	ver.replace(3,1,"");
	ver.replace(4,6, "");

	double localverd = atof(ver.c_str());
	//printf ("value %f\n", localverd);

	std::string remoteversion = filedata;
	double remoteversiond = atof(remoteversion.c_str());

	//return true;
	if (localverd < remoteversiond)
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
	} else {
		std::string titlenoup = "Actualizações";
		std::string infotextnoup = "Não existem Actualizações de momento!";

		QMessageBox msgBoxnoupdates(QMessageBox::Information, QString::fromUtf8(titlenoup.c_str()), QString::fromUtf8(infotextnoup.c_str()), 0, this);
		msgBoxnoupdates.exec();
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
#ifdef WIN32

	//check if it's Windows 32 or 64 bits

#else
	std::string distrostr;
	std::string archstr;

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

	if (param == "distro")
		return distrostr;
	else
		return archstr;

#endif

}

void AutoUpdates::ChooseVersion(std::string distro, std::string arch)
{
	std::string downloadurl;
	std::string pkgname;
	downloadurl.append(serverurl);
	pkgname.append("pteid-mw");

	if (arch == "x86_64")
	{
		if (distro == "debian")
		{
			pkgname.append("_1.0.1svn1522-1");
			pkgname.append("_amd64.deb");
			downloadurl.append(pkgname);
			HttpWindow httpWin(downloadurl, distro);
			httpWin.show();
			httpWin.exec();
			//delete httpWin;
		}
		else if (distro == "fedora")
		{
			pkgname.append("-1.0.1.1522-2.1");
			pkgname.append(".x86_64.rpm");
			downloadurl.append(pkgname);
			HttpWindow httpWin(downloadurl, distro);
			httpWin.show();
			httpWin.exec();
		}
		else if (distro == "suse")
		{
		    	pkgname.append("-1.0.1.1522-2.1");
		    	pkgname.append(".x86_64.rpm");
		    	downloadurl.append(pkgname);
		    	HttpWindow httpWin(downloadurl, distro);
		    	httpWin.show();
		    	httpWin.exec();
		}

	} else {
		//32bits
		if (distro == "debian")
		{
			pkgname.append("_1.0.1svn1522-1");
			pkgname.append("_i386.deb");
			downloadurl.append(pkgname);
			HttpWindow httpWin(downloadurl, distro);
			httpWin.show();
			httpWin.exec();
		}
		else if (distro == "fedora")
		{
			pkgname.append("-1.0.1.1522-2.1");
			pkgname.append(".i386.rpm");
			downloadurl.append(pkgname);
			HttpWindow httpWin(downloadurl, distro);
			httpWin.show();
			httpWin.exec();
		}
		else if (distro == "suse")
		{
		    	pkgname.append("-1.0.1.1522-2.1");
		    	pkgname.append(".i586.rpm");
		    	downloadurl.append(pkgname);
		    	HttpWindow httpWin(downloadurl, distro);
		    	httpWin.show();
		    	httpWin.exec();
		}
	}

}

