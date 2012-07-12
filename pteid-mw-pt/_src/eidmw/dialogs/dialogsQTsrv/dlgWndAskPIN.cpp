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

#include <iostream>

#include "dlgWndAskPIN.h"
#include "../langUtil.h"

#include <stdio.h>

#define KP_BTN_SIZE 48

dlgWndAskPIN::dlgWndAskPIN( DlgPinInfo pinInfo, DlgPinUsage PinPusage, QString & Header, QString & PINName, bool UseKeypad, QWidget *parent ) : dlgWndBase(parent)
{
	ui.setupUi(this);
	//setFixedSize(416,270);

	if( pinInfo.ulFlags & PIN_FLAG_DIGITS )
	{
		char buffer[20];
		sprintf(buffer,"[0-9]{%ld,%ld}",pinInfo.ulMinLen,pinInfo.ulMaxLen);
		QRegExp rx(buffer);
		m_PinValidator=new QRegExpValidator(rx, 0);
	}
	else
	{
		m_PinValidator=NULL;
	}

	QString Title;

	this->setWindowIcon( QIcon( ":/Resources/ICO_CARD_EID_PLAIN_16x16.png" ) );

	if( PinPusage == DLG_PIN_SIGN )
	{
		Title+=QString::fromWCharArray(GETSTRING_DLG(SigningWith));
		Title+= ": ";
	}
	else
	{
		Title+=QString::fromWCharArray(GETSTRING_DLG(Asking));
		Title+= " ";
	}
	Title+= PINName;
	this->setWindowTitle(Title);
	this->setAccessibleName(Title);

	ui.lblHeader->setText( QString::fromWCharArray(GETSTRING_DLG(EnterYourPin)) );
	ui.lblHeader->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(EnterYourPin)) );
	ui.lblPINName->setText( QString::fromWCharArray(GETSTRING_DLG(AuthenticationPin)) );
	ui.lblPINName->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(AuthenticationPin)) );
	ui.lblPINName_2->setText( QString::fromWCharArray(GETSTRING_DLG(AuthenticationPin)) );
	ui.lblPINName_2->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(AuthenticationPin)) );

	ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
	ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );

	if( PinPusage == DLG_PIN_SIGN )
		this->setStyleSheet("background-image: url(:/Resources/bg_SignaturePin.png);");
	else
		this->setStyleSheet("background-image: url(:/Resources/bg_AuthenticationPin.png);");

	m_ulPinMinLen = pinInfo.ulMinLen;
	m_ulPinMaxLen = pinInfo.ulMaxLen;

	m_UseKeypad = UseKeypad;

	ui.fraPIN_Keypad->setVisible( false );

	if( !PINName.isEmpty() )
	{
		ui.lblPINName->setText( PINName );
		ui.lblPINName->setAccessibleName( PINName );
	}
	if( !Header.isEmpty() )
	{
		ui.lblHeader->setText( Header );
		ui.lblHeader->setAccessibleName( Header );
	}
	if( pinInfo.ulFlags & PIN_FLAG_DIGITS )

	ui.txtPIN->setMaxLength( 8 );
}

dlgWndAskPIN::~dlgWndAskPIN()
{
	if(m_PinValidator)
	{
		delete m_PinValidator;
		m_PinValidator=NULL;
	}
}

std::wstring dlgWndAskPIN::getPIN()
{
	if( m_UseKeypad ) 
		return QString(ui.txtPIN_2->text()).toStdWString(); 
	else 
		return QString(ui.txtPIN->text()).toStdWString();
}

void dlgWndAskPIN::on_txtPIN_textChanged( const QString & text )
{
	ui.btnOk->setEnabled( (unsigned) text.length() >= m_ulPinMinLen );
}

void dlgWndAskPIN::on_txtPIN_2_textChanged( const QString & text )
{
	ui.btnOk->setEnabled( (unsigned) text.length() >= m_ulPinMinLen );
}

void dlgWndAskPIN::on_tbtNUM_0_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"0" ); 
}

void dlgWndAskPIN::on_tbtNUM_1_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"1" ); 
}

void dlgWndAskPIN::on_tbtNUM_2_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"2" ); 
}

void dlgWndAskPIN::on_tbtNUM_3_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"3" ); 
}

void dlgWndAskPIN::on_tbtNUM_4_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"4" ); 
}

void dlgWndAskPIN::on_tbtNUM_5_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"5" ); 
}

void dlgWndAskPIN::on_tbtNUM_6_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"6" ); 
}

void dlgWndAskPIN::on_tbtNUM_7_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"7" ); 
}

void dlgWndAskPIN::on_tbtNUM_8_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"8" ); 
}

void dlgWndAskPIN::on_tbtNUM_9_clicked()
{
	if( (unsigned) ui.txtPIN_2->text().size() < m_ulPinMaxLen ) ui.txtPIN_2->setText( ui.txtPIN_2->text() +"9" ); 
}

void dlgWndAskPIN::on_tbtClear_clicked()
{
	ui.txtPIN_2->clear(); 
}

