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
/**
 * eID middleware Dialogs.
 */

#ifndef DLGWNDASKACCESS_H
#define DLGWNDASKACCESS_H

#include <QDialog>
#include "ui_dlgWndAskAccess.h"

#include "dlgWndBase.h"
#include "../dialogs.h"

using namespace eIDMW;

class dlgWndAskAccess : public dlgWndBase
{
	Q_OBJECT

public:
	dlgWndAskAccess( QString AppPath, QString ReaderName,
			DlgPFOperation ulOperation, QWidget *parent = 0 );
	~dlgWndAskAccess();
	DlgRet dlgResult;
	bool ForAllIsChecked() { return (ui.chkForAll->checkState()==Qt::Checked?true:false); }

private slots:
	void on_btnYes_clicked();
	void on_btnNo_clicked();
	void on_btnAlways_clicked();
	void on_btnNever_clicked();

private:
	Ui::dlgWndAskAccess ui;
};

#endif // DLGWNDASKACCESS_H
