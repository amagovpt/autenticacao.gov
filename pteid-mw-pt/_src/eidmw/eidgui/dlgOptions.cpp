/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2012-2017 Caixa Mágica Software
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

dlgOptions::dlgOptions( GUISettings& settings, QString &currentReaderName, QWidget *parent )
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
	, m_Settings(settings)
{
	ui.setupUi(this);
	ui.chbAutoCardReading->setChecked( settings.getAutoCardReading() );
	ui.chbWinAutoStart->setChecked( settings.getAutoStartup() );

	if (!settings.getProxyHost().isEmpty()){
		ui.lineEdit_proxyHost->setText(settings.getProxyHost());
		if (settings.getProxyPort() > 0){
			ui.spinBox->setValue(settings.getProxyPort());
		}
		ui.checkBox_proxy->setChecked(true);

        bool proxyAuth_Checked = false;
    	if (!settings.getProxyUsername().isEmpty()){
			ui.lineEdit_proxyUser->setText(settings.getProxyUsername());
			ui.lineEdit_proxyPwd->setText(settings.getProxyPwd());

            proxyAuth_Checked = true;
		}
        checkBox_proxyAuth_EnableGroup(proxyAuth_Checked);
		ui.checkBox_proxyAuth->setChecked( proxyAuth_Checked );
	} else{
        on_checkBox_proxy_toggled(false);
        on_checkBox_proxyAuth_toggled(false);
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

	if ( ReaderSet.readerCount() < 1 ) {
		ui.cmbCardReader->addItem(tr("Not specified"));
	}
	else 
	{
		int selected_reader = m_Settings.getSelectedReader();
		const char* const* ReaderList = ReaderSet.readerList();
		for (unsigned long Idx=0; Idx < ReaderSet.readerCount(); Idx++)
		{
			const char* readerName = ReaderList[Idx];
			ui.cmbCardReader->addItem(readerName);
			if (currentReaderName == readerName)
				selected_reader = Idx;
		}
		
		if ( selected_reader < 0 ) {
			selected_reader = 0;
		} else {
			if (selected_reader >= ReaderSet.readerCount()) {
				selected_reader = ReaderSet.readerCount() - 1;
			}
        }

		ui.cmbCardReader->setCurrentIndex(selected_reader);
	}

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
	if ( bChecked ) ui.chbAutoCardReading->setChecked( bChecked );
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

void dlgOptions::checkBox_proxy_EnableGroup(bool checked){
    ui.label_proxyhost->setEnabled(checked);
	ui.lineEdit_proxyHost->setEnabled(checked);

	ui.label_proxyPort->setEnabled(checked);
	ui.spinBox->setEnabled(checked);
}

//Enable or disable the proxy-related elements when the main checkbox is used
void dlgOptions::on_checkBox_proxy_toggled(bool checked)
{
	if ( !checked ){
		ui.lineEdit_proxyHost->setText("");
	}

	checkBox_proxy_EnableGroup( checked );
    ui.checkBox_proxyAuth->setEnabled(checked);

    if ( ( checked && getProxyAuth() ) || (!checked) ){
        checkBox_proxyAuth_EnableGroup( checked );
    }
}

void dlgOptions::checkBox_proxyAuth_EnableGroup(bool checked){
	ui.lineEdit_proxyUser->setEnabled(checked);
    ui.label_proxyUser->setEnabled(checked);

	ui.lineEdit_proxyPwd->setEnabled(checked);
    ui.label_proxyPwd->setEnabled(checked);
}

void dlgOptions::on_checkBox_proxyAuth_toggled(bool checked)
{
	if (!checked){
		ui.lineEdit_proxyUser->setText("");
		ui.lineEdit_proxyPwd->setText("");
	}

    checkBox_proxyAuth_EnableGroup( checked );
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

	if (!ui.checkBox_proxyAuth->isChecked())
	{
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
			return;
		}

	}

	this->done(QDialog::Accepted);
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
	m_Settings.setSelectedReader(index);
}
void dlgOptions::on_chbRegCert_toggled( bool bChecked )
{
	m_Settings.setRegCert(bChecked);
}
void dlgOptions::on_chbRemoveCert_toggled( bool bChecked )
{
	m_Settings.setRemoveCert(bChecked);
}
