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
#ifndef HTTPWINDOW_H
#define HTTPWINDOW_H

#include <QDialog>
#include <QtGui>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QUrl>
#include <iostream>
#include <QString>

class QDialogButtonBox;
class QFile;
class QLabel;
class QLineEdit;
class QProgressDialog;
class QPushButton;
class QAuthenticator;
class QNetworkReply;


class HttpWindow : public QDialog
{
	Q_OBJECT

public:
	HttpWindow(std::string uri, std::string distro, QWidget *parent = 0);
	~HttpWindow();
	void startRequest(QUrl url);

private slots:
	void downloadFile();
	void cancelDownload();
	void httpFinished();
	void httpReadyRead();
	void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
    QString GetReleaseNotes();

	QLabel *statusLabel;
	QProgressDialog *progressDialog;
	QPushButton *downloadButton;
	QPushButton *cancelButton;
	QDialogButtonBox *buttonBox;
	QVBoxLayout *mainLayout;
	QHBoxLayout *topLayout;
	QTextEdit *textEditor;

	QUrl url;
	QNetworkAccessManager qnam;
	QNetworkReply *reply;
	QFile *file;
	int httpGetId;
	bool httpRequestAborted;

	void RunPackage(std::string pkg, std::string distro);
};

#endif
