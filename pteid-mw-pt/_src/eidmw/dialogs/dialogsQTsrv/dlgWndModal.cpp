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

#include "dlgWndModal.h"
#include "../langUtil.h"

dlgWndModal::dlgWndModal( DlgIcon icon, QString Msg, unsigned char ulButtons,
						 unsigned char ulEnterButton, unsigned char ulCancelButton, QWidget *parent ) : dlgWndBase(parent)
{
	m_ulCancelButton=ulCancelButton;

	ui.setupUi(this);

	QString Title="";
	//if( DApplic == DLG_APP_BELPIC )
	//{
		this->setWindowIcon( QIcon( ":/Resources/ICO_CARD_EID_PLAIN_16x16.png" ) );
	//	Title+=QString::fromWCharArray(GETSTRING_DLG(Belpic));
	//	Title+=": ";
	//}
	Title+=QString::fromWCharArray(GETSTRING_DLG(Message));
	this->setWindowTitle( Title );
	setMaximumSize( 380,200 );

	ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnOk->setDefault( ulEnterButton==DLG_BUTTON_OK);

	ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setDefault( ulEnterButton==DLG_BUTTON_CANCEL);

	ui.btnRetry->setText( QString::fromWCharArray(GETSTRING_DLG(Retry)) );
	ui.btnRetry->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Retry)) );
	ui.btnRetry->setDefault( ulEnterButton==DLG_BUTTON_RETRY);

	ui.btnYes->setText( QString::fromWCharArray(GETSTRING_DLG(Yes)) );
	ui.btnYes->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Yes)) );
	ui.btnYes->setDefault( ulEnterButton==DLG_BUTTON_YES);

	ui.btnNo->setText( QString::fromWCharArray(GETSTRING_DLG(No)) );
	ui.btnNo->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(No)) );
	ui.btnNo->setDefault( ulEnterButton==DLG_BUTTON_NO);

	switch( icon )
	{
	case DLG_ICON_INFO:
		ui.lblIcon->setPixmap( QPixmap( ":/Resources/ICO_CARD_INFO_64x64.png" ) );
		break;

	case DLG_ICON_WARN:
		ui.lblIcon->setPixmap( QPixmap( ":/Resources/ICO_CARD_WARNING_64x64.png" ) );
		break;

	case DLG_ICON_QUESTION:
		ui.lblIcon->setPixmap( QPixmap( ":/Resources/ICO_CARD_QUESTION_64x64.png" ) );
		break;

	default:
		ui.lblIcon->setPixmap( QPixmap( ":/Resources/ICO_CARD_NOK_64x64.png" ) );
		break;
	}

	ui.lblMessage->setText( Msg );
	ui.lblMessage->setAccessibleName( Msg );

	ui.btnOk->setVisible( ulButtons & DLG_BUTTON_OK );
	ui.btnCancel->setVisible( ulButtons & DLG_BUTTON_CANCEL );
	ui.btnRetry->setVisible( ulButtons & DLG_BUTTON_RETRY );
	ui.btnYes->setVisible( ulButtons & DLG_BUTTON_YES );
	ui.btnNo->setVisible( ulButtons & DLG_BUTTON_NO );

	dlgResult = DLG_ERR;
}

dlgWndModal::~dlgWndModal()
{

}

void dlgWndModal::on_btnOk_clicked()
{
	dlgResult = DLG_OK;
	hide();
}

void dlgWndModal::on_btnCancel_clicked()
{
	dlgResult = DLG_CANCEL;
	hide();
}

void dlgWndModal::on_btnRetry_clicked()
{
	dlgResult = DLG_RETRY;
	hide();
}

void dlgWndModal::on_btnYes_clicked()
{
	dlgResult = DLG_YES;
	hide();
}

void dlgWndModal::on_btnNo_clicked()
{
	dlgResult = DLG_NO;
	hide();
}

void dlgWndModal::reject()
{
	switch(m_ulCancelButton)
	{
	case DLG_BUTTON_NO:
		dlgResult = eIDMW::DLG_NO;	break;
	case DLG_BUTTON_RETRY:
		dlgResult = eIDMW::DLG_RETRY;	break;
	case DLG_BUTTON_YES:
		dlgResult = eIDMW::DLG_YES;	break;
	case DLG_BUTTON_OK:
		dlgResult = eIDMW::DLG_OK;	break;
	default:
		dlgResult = eIDMW::DLG_CANCEL;	break;
	}

	QDialog::reject();
}
