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

#include <iostream>

dlgWndPinpadInfo::dlgWndPinpadInfo( unsigned long ulHandle,
		DlgPinOperation operation, const QString & Reader,  
		const QString &PINName, const QString & Message, QWidget *parent )

	: QWidget(parent)
{
	ui.setupUi(this);
    setFixedSize(417, 259);

	QString Title="";

	//if( DApplic == DLG_APP_BELPIC )
	//{
		this->setWindowIcon( QIcon( ":/Resources/ICO_CARD_EID_PLAIN_16x16.png" ) );
	//	Title+=QString::fromWCharArray(GETSTRING_DLG(Belpic));
	//	Title+=": ";
	//}
    std::cout << "PINName " << PINName.toStdString() << std::endl;
    if (operation == DLG_PIN_OP_CHANGE)
        Title+=QString::fromUtf8("Alterar PIN ");
    else if (operation == DLG_PIN_OP_VERIFY)
        Title+=QString::fromUtf8("Verificar PIN ");
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
	m_ulHandle = ulHandle;
//	ui.lblIcon->setPixmap( QPixmap( ":/Resources/ICO_CARD_PIN_128x128.png" ) );

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
