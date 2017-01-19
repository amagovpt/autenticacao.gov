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

#ifndef DLGWNDPINPADINFO_H
#define DLGWNDPINPADINFO_H

#include <QWidget>
#include "ui_dlgWndPinpadInfo.h"

#include "dlgWndBase.h"
#include "../dialogs.h"

using namespace eIDMW;

class dlgWndPinpadInfo : public QWidget
{
	Q_OBJECT

public:
	dlgWndPinpadInfo( unsigned long ulHandle,
		DlgPinOperation operation, const QString & Reader,
		const QString &PINName, const QString & Message, QWidget *parent = 0, Type_WndGeometry *pParentWndGeometry = 0 );
	~dlgWndPinpadInfo();

protected:
	void closeEvent( QCloseEvent *event );

private:
	Ui::dlgWndPinpadInfo ui;
	unsigned long m_ulHandle;
};

#endif // DLGWNDPINPADINFO_H
