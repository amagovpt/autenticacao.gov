/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011-2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2018 André Guerreiro - <aguerreiro1985@gmail.com>
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

	setFixedSize(this->width(), this->height());

	QString Title = QString::fromWCharArray(GETSTRING_DLG(Notification)) + ": " + PINName;
	this->setWindowTitle(Title);
	this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );

	// HEADER
	QString sHeader = QString::fromWCharArray(GETSTRING_DLG(Bad)) + " \"" + PINName + "\"";
	ui.lblHeader->setText( sHeader );
	ui.lblHeader->setAccessibleName( sHeader );
	ui.lblHeader->setStyleSheet("QLabel { color : #3C5DBC; font-size:16pt; font-weight:800; background:rgba(0,0,0,0)}");

	// ICON
	QPixmap pix_icon(":/images/autenticacao.bmp");
	ui.lblIcon->setPixmap(pix_icon);
	ui.lblIcon->setScaledContents(true);

	// ERROR IMAGE
	QPixmap pix_error(":/images/error.ico");
	ui.lblImage->setPixmap(pix_error);
	ui.lblImage->setScaledContents(true);

	// RETRY BUTTON
	ui.btnRetry->setText( QString::fromWCharArray(GETSTRING_DLG(Retry)) );
	ui.btnRetry->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Retry)) );
	ui.btnRetry->setStyleSheet("QPushButton {background-color: #3C5DBC; color: #ffffff; border-radius: 0}\
QPushButton:hover{background-color: #2C3DAC}");
	ui.btnRetry->setVisible(RemainingTries);

	// CANCEL BUTTON
	ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setStyleSheet("QPushButton {background-color: #D6D7D7; color: #3C5DBC; border-radius: 0}\
QPushButton:hover{background-color: #C6C7C7}");
	ui.btnCancel->setVisible(RemainingTries);

	// OK BUTTON
	ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnOk->setStyleSheet("QPushButton {background-color: #3C5DBC; color: #ffffff; border-radius: 0}\
QPushButton:hover{background-color: #2C3DAC}");
	ui.btnOk->setVisible(!RemainingTries);

	// STRING REMAINING TRIES / BLOCKED
	QString sCenter = QString::fromWCharArray(GETSTRING_DLG(IncorrectPin)) + "\n";
	sCenter += QString().setNum(RemainingTries) + " ";
	sCenter += QString::fromWCharArray(GETSTRING_DLG(RemainingAttempts)) + ".";
	if( !RemainingTries )
		sCenter = QString::fromWCharArray(GETSTRING_DLG(PinBlocked));

	QString fontSize;
#ifdef __APPLE__
	fontSize = "12pt";
#else
	fontSize = "10pt";
#endif

	ui.lblCenter->setText(sCenter);
	ui.lblCenter->setAccessibleName(sCenter);
	ui.lblCenter->setStyleSheet("QLabel { background: rgba(0,0,0,0); color : #000000; font-size: " + fontSize + "; }");
	ui.lblCenter->setAlignment(Qt::AlignCenter);
	ui.lblCenter->setWordWrap(true);

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
