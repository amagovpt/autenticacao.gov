/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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

**************************************************************************** */
#include "dlgOptions.h"
#include "eidlib.h"
#include "Settings.h"
#include <QMessageBox>

using namespace eIDMW;

dlgOptions::dlgOptions( GUISettings& settings, QWidget *parent )
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
	, m_Settings(settings)
{
	ui.setupUi(this);
	ui.chbAutoCardReading->setChecked( settings.getAutoCardReading() );
	ui.chbWinAutoStart->setChecked( settings.getAutoStartup() );


	if (!settings.getProxyHost().isEmpty())
	{
		ui.label_proxyhost->setEnabled(true);
		ui.label_proxyPort->setEnabled(true);

		ui.lineEdit_proxyHost->setText(settings.getProxyHost());
		if (settings.getProxyPort() > 0)
		{
			ui.spinBox->setValue(settings.getProxyPort());
		}
		ui.checkBox_proxy->setChecked(true);

		ui.checkBox_proxyAuth->setEnabled(true);
		ui.lineEdit_proxyHost->setEnabled(true);
		ui.spinBox->setEnabled(true);

		if (!settings.getProxyUsername().isEmpty())
		{
			ui.checkBox_proxyAuth->setChecked(true);
			ui.label_proxyUser->setEnabled(true);
			ui.label_proxyPwd->setEnabled(true);

			ui.lineEdit_proxyUser->setText(settings.getProxyUsername());
			ui.lineEdit_proxyPwd->setText(settings.getProxyPwd());
		}
		else
		{
			ui.lineEdit_proxyUser->setEnabled(false);
			ui.lineEdit_proxyPwd->setEnabled(false);
		}
	}
	else
	{
		ui.label_proxyhost->setEnabled(false);
		ui.label_proxyPort->setEnabled(false);
		ui.label_proxyUser->setEnabled(false);
		ui.label_proxyPwd->setEnabled(false);
		ui.lineEdit_proxyHost->setEnabled(false);
		ui.spinBox->setEnabled(false);
		ui.checkBox_proxyAuth->setEnabled(false);
	}

#ifndef WIN32
//#ifndef __APPLE__
	ui.chbWinAutoStart->hide();

	//Resize the Startup groupBox to avoid border overlap: only visible in MacOS
	ui.groupBox_startup->resize(ui.groupBox_startup->width(), ui.groupBox_startup->height()-40);

	//TODO: test these translations in OSX and Windows
	QRect pos_1 = ui.groupBox_notifications->geometry();
	pos_1.translate(0, -30);
	ui.groupBox_notifications->setGeometry(pos_1);

	QRect pos = ui.groupBox_proxy->geometry();

	pos.translate(0, -90);
	ui.groupBox_proxy->setGeometry(pos);

	QRect pos_ok = ui.okButton->geometry();
	pos.translate(0, -90);
	ui.okButton->setGeometry(pos_ok);
//#endif
#endif

	ui.chbStartMinimized->setChecked( settings.getStartMinimized() );
	ui.chbShowNotification->setChecked( settings.getShowNotification() );

	ui.chbRegCert->setChecked( settings.getRegCertSetting() );
	ui.chbRemoveCert->setChecked( settings.getRemoveCertSetting() );

	ui.chbRegCert->setEnabled(settings.getAutoCardReading());
	ui.chbRemoveCert->setEnabled(settings.getAutoCardReading());

	ui.cmbCardReader->addItem(tr("Not specified"));

	const char* const* ReaderList = ReaderSet.readerList();
	for (unsigned long Idx=0; Idx<ReaderSet.readerCount(); Idx++)
	{
		const char* readerName = ReaderList[Idx];
		ui.cmbCardReader->addItem(readerName);
	}
	int SelectedReader = m_Settings.getSelectedReader()+1;
	ui.cmbCardReader->setCurrentIndex( SelectedReader );

#ifndef WIN32
	ui.chbRegCert->hide();
	ui.chbRemoveCert->hide();
	ui.groupBox->hide();
#endif

	this->setFixedSize(this->width(), this->height());
}

dlgOptions::~dlgOptions()
{
}

void dlgOptions::on_chbShowPicture_toggled( bool bChecked )
{
	m_Settings.setShowPicture( bChecked );
}
void dlgOptions::on_chbShowNotification_toggled( bool bChecked )
{
	m_Settings.setShowNotification( bChecked );
}

QString dlgOptions::getProxyHost()
{
	return ui.lineEdit_proxyHost->text();
}

int dlgOptions::getProxyPort()
{
	return ui.spinBox->value();
}

bool dlgOptions::getProxyAuth()
{
	return ui.checkBox_proxyAuth->isChecked();
}

//Enable or disable the proxy-related elements when the main checkbox is used
void dlgOptions::on_checkBox_proxy_toggled(bool checked)
{
	if (!checked)
	{
		ui.lineEdit_proxyHost->setText("");
		ui.lineEdit_proxyUser->setText("");
		ui.lineEdit_proxyPwd->setText("");

	}

	ui.lineEdit_proxyHost->setEnabled(checked);
	ui.lineEdit_proxyHost->setEnabled(checked);
	ui.spinBox->setEnabled(checked);
	ui.checkBox_proxyAuth->setEnabled(checked);
	ui.lineEdit_proxyUser->setEnabled(checked);
	ui.lineEdit_proxyPwd->setEnabled(checked);
	ui.label_proxyhost->setEnabled(checked);
	ui.label_proxyPort->setEnabled(checked);

	if (!checked)
	{
		ui.label_proxyUser->setEnabled(false);
		ui.label_proxyPwd->setEnabled(false);
	}
	else if (checked && ui.checkBox_proxyAuth->isChecked())
	{
		ui.label_proxyUser->setEnabled(true);
		ui.label_proxyPwd->setEnabled(true);
	}
}

void dlgOptions::on_checkBox_proxyAuth_toggled(bool checked)
{

	ui.lineEdit_proxyUser->setEnabled(checked);
	ui.lineEdit_proxyPwd->setEnabled(checked);
	ui.label_proxyUser->setEnabled(checked);
	ui.label_proxyPwd->setEnabled(checked);
}

void dlgOptions::on_okButton_clicked()
{
	//If proxy is disabled then clean existing values
	if (!ui.checkBox_proxy->isChecked())
	{
		m_Settings.setProxyHost(QString());
		m_Settings.setProxyPort(0);
		m_Settings.setProxyUsername(QString());
		m_Settings.setProxyPwd(QString());
	}

	if (ui.checkBox_proxy->isChecked() && getProxyHost().isEmpty())
	{
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Warning,
			QString::fromUtf8("Cart\xc3\xa3o de Cidad\xc3\xa3o"), tr("Proxy host is missing!"),
			QMessageBox::Ok, this);

			msgBox->setModal(true);
			msgBox->show();
			delete msgBox;//LL
			return;
	}

	if (!getProxyHost().isEmpty())
	{
		m_Settings.setProxyHost(getProxyHost());
		m_Settings.setProxyPort(getProxyPort());

		if (!ui.lineEdit_proxyUser->text().isEmpty() &&
			!ui.lineEdit_proxyPwd->text().isEmpty())
		{
			m_Settings.setProxyUsername(ui.lineEdit_proxyUser->text());
			m_Settings.setProxyPwd(ui.lineEdit_proxyPwd->text());
		}

		if (ui.checkBox_proxyAuth->isChecked() && (ui.lineEdit_proxyUser->text().isEmpty() ||
				ui.lineEdit_proxyPwd->text().isEmpty()))
		{
			QMessageBox *msgBox = new QMessageBox(QMessageBox::Warning,
			QString::fromUtf8("Cart\xc3\xa3o de Cidad\xc3\xa3o"), tr("Proxy Authentication details are incomplete!"),
			QMessageBox::Ok, this);

			msgBox->setModal(true);
			msgBox->show();
			delete msgBox;//LL
			return;
		}

	}

	this->close();
}

void dlgOptions::on_chbAutoCardReading_toggled( bool bChecked )
{
	m_Settings.setAutoCardReading(bChecked);
	ui.chbRegCert->setEnabled(bChecked);
	ui.chbRemoveCert->setEnabled(bChecked);
}
void dlgOptions::on_chbWinAutoStart_toggled( bool bChecked )
{
	m_Settings.setAutoStartup( bChecked );
}
void dlgOptions::on_chbStartMinimized_toggled( bool bChecked )
{
	m_Settings.setStartMinimized( bChecked );
}
void dlgOptions::on_cmbCardReader_activated ( int index )
{
	m_Settings.setSelectedReader(index-1);	// the first item is 'no cardreader' Put then the index to illegal (==-1)
}
void dlgOptions::on_chbRegCert_toggled( bool bChecked )
{
	m_Settings.setRegCert(bChecked);
}
void dlgOptions::on_chbRemoveCert_toggled( bool bChecked )
{
	m_Settings.setRemoveCert(bChecked);
}
