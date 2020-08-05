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
#include <QMovie>

dlgWndPinpadInfo::dlgWndPinpadInfo( unsigned long ulHandle,
		DlgPinOperation operation, const QString & Reader,
		const QString &PINName, const QString & Message, QWidget *parent, Type_WndGeometry *pParentWndGeometry )

	: QWidget(parent)
{
	ui.setupUi(this);
    setFixedSize(this->width(), this->height());

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

    QString fontSize;
#ifdef __APPLE__
    fontSize = "12pt";
#else
    fontSize = "10pt";
#endif

    // HEADER
    QString sHeader = PINName;
    ui.lblHeader->setText(sHeader);
    ui.lblHeader->setAccessibleName(sHeader);
    ui.lblHeader->setStyleSheet("QLabel { background: rgba(0,0,0,0); color : #3C5DBC; font-size: 16pt; font-weight:800 }");

    // ICON
    QPixmap pix_icon(":/images/autenticacao.bmp");
    ui.lblIcon->setPixmap(pix_icon);
    ui.lblIcon->setScaledContents(true);

    // LOADING GIF
    QMovie *movie = new QMovie(":/images/loading.gif");
    ui.lblLoading->setMovie(movie);
    ui.lblLoading->setScaledContents(true);
    movie->start();

    // CENTER LABEL
    QString sCenter;
    if (operation == DLG_PIN_OP_UNBLOCK_CHANGE)
    {
        sCenter = QString::fromWCharArray(GETSTRING_DLG(UnlockDialogHeader));
    }
    else
    {
        sCenter = Message; //QString::fromWCharArray(GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader));
    }

    ui.lblCenter->setText(sCenter);
    ui.lblCenter->setAccessibleName(sCenter);
    ui.lblCenter->setStyleSheet("QLabel { background: rgba(0,0,0,0); color : #000000; font-size:" + fontSize + "; font-weight: 500}");
    ui.lblCenter->setAlignment(Qt::AlignCenter);

    // BOTTOM LABEL
    QString labelDisablePinpad = QString::fromWCharArray(GETSTRING_DLG(PinpadCanBeDisabled));
    ui.lblBottom->setText(labelDisablePinpad);
    ui.lblBottom->setAccessibleName(labelDisablePinpad);
    ui.lblBottom->setStyleSheet("QLabel { background: rgba(0,0,0,0); color : #000000; font-size: " + fontSize + "; }");
    ui.lblBottom->setAlignment(Qt::AlignCenter);

    m_ulHandle = ulHandle;

    Type_WndGeometry WndGeometry;
    if ( getWndCenterPos( pParentWndGeometry,
                          QApplication::desktop()->width(), QApplication::desktop()->height(),
                          this->width(), this->height(), &WndGeometry ) ) {
        parent->move(WndGeometry.x, WndGeometry.y);
    }
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
