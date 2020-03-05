/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011-2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012, 2016-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
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

	QString Title="";

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
    if ( getWndCenterPos( pParentWndGeometry,
                          QApplication::desktop()->width(), QApplication::desktop()->height(),
                          this->width(), this->height(), &WndGeometry ) ) {
        parent->move(WndGeometry.x, WndGeometry.y);
    }

	QString tmpHeader;
	
	tmpHeader += PINName;

    ui.label_2->setText( tmpHeader );
    ui.label_2->setAccessibleName( tmpHeader );
    ui.label_2->setStyleSheet("QLabel { color : #3C5DBC; font-size:14pt; font-weight:600 }");
    ui.label->wordWrap();

    QString label;
    if (operation == DLG_PIN_OP_UNBLOCK_CHANGE)
    {
    	label = QString::fromWCharArray(GETSTRING_DLG(UnlockDialogHeader));
    }
    else
    {
    	label = Message;
    }

    ui.label->setText(label);
    ui.label->setAccessibleName(label);
    ui.label->setStyleSheet("QLabel { color : #3C5DBC; font-size:10pt; }");
	m_ulHandle = ulHandle;

    QString labelDisablePinpad = QString::fromWCharArray(GETSTRING_DLG(PinpadCanBeDisabled));
    ui.label_bottom->setText(labelDisablePinpad);
    ui.label_bottom->setAccessibleName(labelDisablePinpad);
    ui.label_bottom->setStyleSheet("QLabel { color : #3C5DBC; font-size:10pt; }");
    ui.label_bottom->wordWrap();
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
