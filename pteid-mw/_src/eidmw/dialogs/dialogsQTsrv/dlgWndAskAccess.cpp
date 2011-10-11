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

#include "dlgWndAskAccess.h"
#include "../langUtil.h"

dlgWndAskAccess::dlgWndAskAccess( QString AppPath, QString ReaderName,
								 DlgPFOperation ulOperation, QWidget *parent) : dlgWndBase(parent)

{
	ui.setupUi(this);

	QString(tmpTitle);

	//if( DApplic == DLG_APP_BELPIC )
	//{
		this->setWindowIcon( QIcon( ":/Resources/ICO_CARD_EID_PLAIN_16x16.png" ) );
		tmpTitle=QString::fromWCharArray(GETSTRING_DLG(ReadIdentityData)) + ": " + QString::fromWCharArray(GETSTRING_DLG(PrivacyFilter));
	//}
	//else
	//{
	//	tmpTitle=QString::fromWCharArray(GETSTRING_DLG(PrivacyFilter));
	//}
	if(!ReaderName.isEmpty())
	{
		tmpTitle.append(" - ");
		tmpTitle.append(ReaderName);
	}

	this->setWindowTitle( tmpTitle );

	setMaximumSize( 380,200 );

	ui.lblIcon->setPixmap( QPixmap( ":/Resources/ICO_CARD_QUESTION_64x64.png" ) );

	ui.lblMessage_1->setText( QString::fromWCharArray(GETSTRING_DLG(AnApplicationWantsToAccessTheCard)) );
	ui.lblMessage_1->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(AnApplicationWantsToAccessTheCard)) );
	ui.lblMessage_2->setText( QString::fromWCharArray(GETSTRING_DLG(Application)) );
	ui.lblMessage_2->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Application)) );
	ui.lblMessage_3->setText( AppPath );
	ui.lblMessage_3->setAccessibleName( AppPath );
	ui.lblMessage_4->setText( QString::fromWCharArray(GETSTRING_DLG(Function)) );
	ui.lblMessage_4->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Function)) );
	switch(ulOperation)
	{
	case 	DLG_PF_OP_READ_ID:
		ui.lblMessage_5->setText( QString::fromWCharArray(GETSTRING_DLG(ReadIdentityData)) );
		ui.lblMessage_5->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ReadIdentityData)) );
		break;
	case 	DLG_PF_OP_READ_PHOTO:
		ui.lblMessage_5->setText( QString::fromWCharArray(GETSTRING_DLG(ReadPhoto)) );
		ui.lblMessage_5->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ReadPhoto)) );
		break;
	case 	DLG_PF_OP_READ_ADDRESS:
		ui.lblMessage_5->setText( QString::fromWCharArray(GETSTRING_DLG(ReadAddressData)) );
		ui.lblMessage_5->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ReadAddressData)) );	
		break;
	case 	DLG_PF_OP_READ_CERTS:
		ui.lblMessage_5->setText( QString::fromWCharArray(GETSTRING_DLG(ReadCertificates)) );
		ui.lblMessage_5->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ReadCertificates)) );	
		break;
	default:
		ui.lblMessage_5->setText( QString::fromWCharArray(GETSTRING_DLG(ReadUnknownData)) );
		ui.lblMessage_5->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ReadUnknownData)) );	
		break;
	}
	ui.lblMessage_6->setText( QString::fromWCharArray(GETSTRING_DLG(DoYouWantToAcceptIt)) );
	ui.lblMessage_6->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(DoYouWantToAcceptIt)) );

	ui.chkForAll->setText( QString::fromWCharArray(GETSTRING_DLG(ForAllOperations)) );
	ui.chkForAll->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ForAllOperations)) );

	ui.btnYes->setText( QString::fromWCharArray(GETSTRING_DLG(Yes)) );
	ui.btnYes->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Yes)) );
	ui.btnNo->setText( QString::fromWCharArray(GETSTRING_DLG(CancelNo)) );
	ui.btnNo->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(CancelNo)) );
	ui.btnAlways->setText( QString::fromWCharArray(GETSTRING_DLG(Always)) );
	ui.btnAlways->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Always)) );
	ui.btnNever->setText( QString::fromWCharArray(GETSTRING_DLG(Never)) );
	ui.btnNever->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Never)) );

	dlgResult = DLG_ERR;
}

dlgWndAskAccess::~dlgWndAskAccess()
{

}

void dlgWndAskAccess::on_btnAlways_clicked()
{
	dlgResult = DLG_ALWAYS;
	hide();
}

void dlgWndAskAccess::on_btnNever_clicked()
{
	dlgResult = DLG_NEVER;
	hide();
}

void dlgWndAskAccess::on_btnYes_clicked()
{
	dlgResult = DLG_YES;
	hide();
}

void dlgWndAskAccess::on_btnNo_clicked()
{
	dlgResult = DLG_CANCEL;
	hide();
}
