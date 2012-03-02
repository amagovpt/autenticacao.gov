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

#include "dlgWndBadPIN.h"
#include "../langUtil.h"

#include <iostream>
using namespace std;

dlgWndBadPIN::dlgWndBadPIN( QString & PINName, unsigned long RemainingTries, QWidget *parent ) : dlgWndBase(parent)
{
	ui.setupUi(this);

	setFixedSize(411, 183);

	QString Title="";
	//if( DApplic == DLG_APP_BELPIC )
	//{
		this->setWindowIcon( QIcon( ":/Resources/ICO_CARD_EID_PLAIN_16x16.png" ) );
	//	Title+=QString::fromWCharArray(GETSTRING_DLG(Belpic));
	//	Title+=": ";
	//}
	//else
	//{
		Title+=QString::fromWCharArray(GETSTRING_DLG(Notification));
		Title+=": ";
	//}
	Title+=QString::fromWCharArray(GETSTRING_DLG(Bad));
	Title+=" ";
	Title+=PINName;

	wcout << PINName.toStdWString() << endl;

	this->setWindowTitle(Title);

	ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnRetry->setText( QString::fromWCharArray(GETSTRING_DLG(Retry)) );
	ui.btnRetry->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Retry)) );
	ui.lblIcon->setPixmap( QPixmap( ":/Resources/ICO_CARD_NOK_64x64.png" ) );

	QString sHeader;
	sHeader=QString::fromWCharArray(GETSTRING_DLG(Bad));
	sHeader+=" "; 
	sHeader+=PINName;
	sHeader+=": "; 
	sHeader+=QString().setNum( RemainingTries );
	sHeader+=" ";
	sHeader+=QString::fromWCharArray(GETSTRING_DLG(RemainingAttempts));
	ui.lblHeader->setText( sHeader );
	ui.lblHeader->setAccessibleName( sHeader );

	QString sCenter="";
	if( RemainingTries == 0 )
	{
		sCenter+=PINName;
		sCenter+=" ";
		sCenter+=QString::fromWCharArray(GETSTRING_DLG(PinBlocked));
		ui.btnRetry->setVisible(false);
		ui.btnCancel->setVisible(false);
	}
	else
	{
		sCenter+=QString::fromWCharArray(GETSTRING_DLG(TryAgainOrCancel));
		ui.btnOk->setVisible(false);
	}

	ui.lblCenter->setText(sCenter);
	ui.lblCenter->setAccessibleName(sCenter);
}

dlgWndBadPIN::~dlgWndBadPIN()
{

}
