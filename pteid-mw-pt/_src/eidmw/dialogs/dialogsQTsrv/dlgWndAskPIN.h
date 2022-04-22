/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
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

#ifndef DLGWNDASKPIN_H
#define DLGWNDASKPIN_H

#include <QDialog>
#include <QIntValidator>
#include "ui_dlgWndAskPIN.h"
#include "dlgWndBase.h"
#include "../dialogs.h"

using namespace eIDMW;

class dlgWndAskPIN : public dlgWndBase
{
	Q_OBJECT

public:
	dlgWndAskPIN( DlgPinInfo pinInfo, DlgPinUsage PinPusage, QString & Header, QString & PINName, bool UseKeypad, QWidget *parent = 0, Type_WndGeometry *wndGeometry = 0 );
	~dlgWndAskPIN();

	std::wstring getPIN();

private slots:
	void on_txtPIN_textChanged( const QString & text );

private:
	unsigned int m_ulPinMinLen;
	unsigned int m_ulPinMaxLen;
	Ui::dlgWndAskPINClass ui;
	QRegExpValidator *m_PinValidator;
};

#endif // DLGWNDASKPIN_H
