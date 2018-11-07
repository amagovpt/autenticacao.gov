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
#include <QDesktopWidget>

#include <iostream>
using namespace std;

dlgWndBadPIN::dlgWndBadPIN( QString & PINName, unsigned long RemainingTries, QWidget *parent, Type_WndGeometry *pParentWndGeometry ) : dlgWndBase(parent)
{
	ui.setupUi(this);

	setFixedSize(450, 183);

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
    //Title+=QString::fromWCharArray(GETSTRING_DLG(Bad));
    Title+=" ";
    Title+=PINName;

    this->setWindowTitle(Title);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );

	ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
    ui.btnOk->setStyleSheet("QPushButton {background-color: #D6D7D7; color: #333333;}");
	ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
    ui.btnCancel->setStyleSheet("QPushButton {background-color: #D6D7D7; color: #333333;}");
	ui.btnRetry->setText( QString::fromWCharArray(GETSTRING_DLG(Retry)) );
	ui.btnRetry->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Retry)) );
    ui.btnRetry->setStyleSheet("QLabel { color : #3C5DBC; font-size:12pt;}");
    //ui.lblIcon->setPixmap( QPixmap( ":/Resources/ICO_CARD_NOK_64x64.png" ) );

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
    ui.lblHeader->setStyleSheet("QLabel { color : #3C5DBC; font-size:12pt; }");


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
    ui.lblCenter->setStyleSheet("QLabel { color : #3C5DBC; font-size:12pt; }");

        Type_WndGeometry WndGeometry;
        if ( getWndCenterPos( pParentWndGeometry
                        , QApplication::desktop()->width(), QApplication::desktop()->height()
                        , this->width(), this->height()
                        , &WndGeometry ) ){
            this->move( WndGeometry.x, WndGeometry.y );
        }/* if ( getWndCenterPos( pParentWndGeometry, ... ) ) */
}

dlgWndBadPIN::~dlgWndBadPIN()
{

}
