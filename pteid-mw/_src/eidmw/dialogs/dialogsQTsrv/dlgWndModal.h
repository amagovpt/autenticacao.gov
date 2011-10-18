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

#ifndef DLGWNDMODAL_H
#define DLGWNDMODAL_H

#include <QDialog>
#include "ui_dlgWndModal.h"

#include "dlgWndBase.h"
#include "../dialogs.h"

using namespace eIDMW;

class dlgWndModal : public dlgWndBase
{
	Q_OBJECT

public:
	dlgWndModal( DlgIcon icon, QString Msg, unsigned char ulButtons, 
		unsigned char ulEnterButton, unsigned char ulCancelButton, QWidget *parent = 0 );
	~dlgWndModal();
	DlgRet dlgResult;

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_btnRetry_clicked();
	void on_btnYes_clicked();
	void on_btnNo_clicked();
	void reject();

private:
	Ui::dlgWndModal ui;

	unsigned char m_ulCancelButton;

};

#endif // DLGWNDMODAL_H
