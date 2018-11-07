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

#ifndef OPTIONSDLG_H
#define OPTIONSDLG_H

#include "ui_dlgOptions.h"
#include <QDialog>
#include "Settings.h"

class dlgOptions : public QDialog
{
    Q_OBJECT

private slots:

	void on_chbShowPicture_toggled( bool bChecked );
	void on_chbAutoCardReading_toggled( bool bChecked );
	void on_chbWinAutoStart_toggled( bool bChecked );
	void on_chbStartMinimized_toggled( bool bChecked );
	void on_cmbCardReader_activated ( int index );
	void on_chbRegCert_toggled( bool bChecked );
	void on_chbRemoveCert_toggled( bool bChecked );
	void on_chbShowNotification_toggled( bool bChecked );
	void on_checkBox_proxy_toggled(bool bChecked);
	void on_checkBox_proxyAuth_toggled(bool bChecked);
	void on_okButton_clicked();

public:
	dlgOptions( GUISettings& settings, QString &currentReaderName, QWidget *parent = 0);
    ~dlgOptions();

	void setShowPicture( bool Checked )	{ ui.chbShowPicture->setChecked( Checked ); };
	bool getShowPicture( void )			{ return ui.chbShowPicture->isChecked(); };
	void setShowNotification( bool Checked )	{ ui.chbShowNotification->setChecked( Checked ); };
	bool getShowNotification( void )			{ return ui.chbShowNotification->isChecked(); };

	QString getProxyHost();
	int getProxyPort();
	bool getProxyAuth();
    void checkBox_proxy_EnableGroup(bool checked);
    void checkBox_proxyAuth_EnableGroup(bool checked);

private:
    Ui_dlgOptions ui;
// 	static bool m_bAutoCardReading;
// 	static bool m_bAutoStartup;
// 	static bool m_bStartupMini;
// 	static bool m_bRegCert;
// 	static bool m_bRemoveCert;

	GUISettings& m_Settings;

};

#endif
