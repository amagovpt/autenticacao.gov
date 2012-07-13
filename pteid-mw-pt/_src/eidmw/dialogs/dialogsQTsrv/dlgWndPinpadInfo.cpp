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

dlgWndPinpadInfo::dlgWndPinpadInfo( unsigned long ulHandle,
		DlgPinOperation operation, const QString & Reader,  
		const QString &PINName, const QString & Message, QWidget *parent )

	: QWidget(parent)
{
	ui.setupUi(this);
    setFixedSize(417, 259);

    if(PINName.contains("Assinatura", Qt::CaseInsensitive))
        this->setStyleSheet("background-image: url(:/Resources/bg_SignaturePin.png);");
    else
        this->setStyleSheet("background-image: url(:/Resources/bg_AuthenticationPin.png);");

	QString Title="";

	//if( DApplic == DLG_APP_BELPIC )
	//{
		this->setWindowIcon( QIcon( ":/Resources/ICO_CARD_EID_PLAIN_16x16.png" ) );
	//	Title+=QString::fromWCharArray(GETSTRING_DLG(Belpic));
	//	Title+=": ";
	//}

    if (operation == DLG_PIN_OP_CHANGE)
        Title+= QString::fromWCharArray(GETSTRING_DLG(ChangeYourPin));
    else if (operation == DLG_PIN_OP_VERIFY)
	Title+=QString::fromWCharArray(GETSTRING_DLG(PleaseEnterYourPin));
    else
        Title+=QString::fromWCharArray(GETSTRING_DLG(PinpadInfo));

	if(!Reader.isEmpty())
	{
		Title+=" - ";
		Title+=Reader;
	}

	parent->setWindowTitle( Title );

	QString tmpHeader;
	tmpHeader = PINName;

    ui.label_2->setText( tmpHeader );
    ui.label_2->setAccessibleName( tmpHeader );
    ui.label->wordWrap();
    ui.label->setText(QString::fromWCharArray(GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader)));
    ui.label->setAccessibleName(QString::fromWCharArray(GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader)));
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
