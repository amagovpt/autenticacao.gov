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

#ifndef DLGWNDBADPIN_H
#define DLGWNDBADPIN_H

#include <QDialog>
#include "ui_dlgWndBadPIN.h"
#include "dlgWndBase.h"
#include "../dialogs.h"

using namespace eIDMW;

class dlgWndBadPIN : public dlgWndBase
{
	Q_OBJECT

public:
	dlgWndBadPIN( QString & PINName, unsigned long RemainingTries, QWidget *parent = 0, Type_WndGeometry *pParentWndGeometry = 0 );
	~dlgWndBadPIN();

private:
	Ui::dlgWndBadPIN ui;

};

#endif // DLGWNDBADPIN_H
