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

#include "dlgWndPinpadInfo.h"
#include "../langUtil.h"
#include <QDesktopWidget>
#include <QIcon>

dlgWndPinpadInfo::dlgWndPinpadInfo( unsigned long ulHandle,
		DlgPinOperation operation, const QString & Reader,
		const QString &PINName, const QString & Message, QWidget *parent, Type_WndGeometry *pParentWndGeometry )

	: QWidget(parent)
{
	ui.setupUi(this);
    setFixedSize(417, 259);

    /*if(PINName.contains("Assinatura", Qt::CaseInsensitive))
        this->setStyleSheet("background-image: url(:/Resources/bg_SignaturePin.png);");
    else
        this->setStyleSheet("background-image: url(:/Resources/bg_AuthenticationPin.png);");
        */

	QString Title="";

	this->setWindowIcon( QIcon(":/Resources/ICO_CARD_EID_PLAIN_16x16.png") );

    if (operation == DLG_PIN_OP_CHANGE)
        Title+= QString::fromWCharArray(GETSTRING_DLG(ChangeYourPin));
    else if (operation == DLG_PIN_OP_VERIFY)
		Title+=QString::fromWCharArray(GETSTRING_DLG(VerifyingPinCode));
	else if (operation == DLG_PIN_OP_UNBLOCK_CHANGE)
		Title += QString::fromWCharArray(GETSTRING_DLG(UnblockPinHeader));
    else
        Title+=QString::fromWCharArray(GETSTRING_DLG(PinpadInfo));

    parent->setWindowTitle( Title );
    parent->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );

    Type_WndGeometry WndGeometry;
    if ( getWndCenterPos( pParentWndGeometry
                        , QApplication::desktop()->width(), QApplication::desktop()->height()
                        , this->width(), this->height()
                        , &WndGeometry ) ){
        parent->move( WndGeometry.x, WndGeometry.y );
    }/* if ( getWndCenterPos( pParentWndGeometry, ... ) ) */

	QString tmpHeader;
	
	tmpHeader += PINName;

    ui.label_2->setText( tmpHeader );
    ui.label_2->setAccessibleName( tmpHeader );
    ui.label_2->setStyleSheet("QLabel { color : #3C5DBC; font-size:12pt; }");
    ui.label->wordWrap();

    QString label;
    if (operation == DLG_PIN_OP_UNBLOCK_CHANGE)
    {
    	label = QString::fromWCharArray(GETSTRING_DLG(UnlockDialogHeader));
    }
    else
    {
    	label = QString::fromWCharArray(GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader));
    }

    ui.label->setText(label);
    ui.label->setAccessibleName(label);
    ui.label->setStyleSheet("QLabel { color : #3C5DBC; font-size:12pt; }");
	m_ulHandle = ulHandle;
}

dlgWndPinpadInfo::~dlgWndPinpadInfo()
{
}

void dlgWndPinpadInfo::closeEvent( QCloseEvent *event)
{
	if( m_ulHandle )
	{
		unsigned long tmp = m_ulHandle;
		m_ulHandle = 0;
		DlgClosePinpadInfo( tmp );
	}
}
