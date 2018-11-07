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
	void on_tbtNUM_0_clicked();
	void on_tbtNUM_1_clicked();
	void on_tbtNUM_2_clicked();
	void on_tbtNUM_3_clicked();
	void on_tbtNUM_4_clicked();
	void on_tbtNUM_5_clicked();
	void on_tbtNUM_6_clicked();
	void on_tbtNUM_7_clicked();
	void on_tbtNUM_8_clicked();
	void on_tbtNUM_9_clicked();
	void on_tbtClear_clicked();

	void on_txtPIN_textChanged( const QString & text );
	void on_txtPIN_2_textChanged( const QString & text );

private:
	unsigned int m_ulPinMinLen;
	unsigned int m_ulPinMaxLen;
	bool m_UseKeypad;
	Ui::dlgWndAskPINClass ui;
	QRegExpValidator *m_PinValidator;
};

#endif // DLGWNDASKPIN_H
