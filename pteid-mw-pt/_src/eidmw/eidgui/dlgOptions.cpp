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

using namespace eIDMW;

dlgOptions::dlgOptions( GUISettings& settings, QWidget *parent ) 
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
	, m_Settings(settings)
{	
	ui.setupUi(this); 
	ui.chbAutoCardReading->setChecked( settings.getAutoCardReading() );
	ui.chbWinAutoStart->setChecked( settings.getAutoStartup() );


	if (!settings.getProxyHost().isEmpty())
	{
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
		ui.lineEdit_proxyHost->setEnabled(false);
		ui.spinBox->setEnabled(false);
		ui.checkBox_proxyAuth->setEnabled(false);	
	}

#ifndef WIN32 
//#ifndef __APPLE__
	ui.chbWinAutoStart->hide();

	//TODO: test these translations in OSX and Windows
	QRect pos_1 = ui.groupBox_notifications->geometry();
	pos_1.translate(0, -30);
	ui.groupBox_notifications->setGeometry(pos_1);

	QRect pos = ui.groupBox_proxy->geometry();
	
	pos.translate(0, -90);
	ui.groupBox_proxy->setGeometry(pos);
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
	ui.lineEdit_proxyHost->setEnabled(checked);
	ui.lineEdit_proxyHost->setEnabled(checked);
	ui.spinBox->setEnabled(checked);
	ui.checkBox_proxyAuth->setEnabled(checked);
	ui.lineEdit_proxyUser->setEnabled(checked);
	ui.lineEdit_proxyPwd->setEnabled(checked);
}

void dlgOptions::on_checkBox_proxyAuth_toggled(bool checked)
{
	
	ui.lineEdit_proxyUser->setEnabled(checked);
	ui.lineEdit_proxyPwd->setEnabled(checked);
}

void dlgOptions::on_okButton_clicked()
{
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
