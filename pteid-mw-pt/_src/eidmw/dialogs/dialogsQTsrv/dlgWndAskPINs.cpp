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

#include "dlgWndAskPINs.h"
#include "../langUtil.h"

#include <iostream>
#include <stdio.h>

#define KP_BTN_SIZE 48

dlgWndAskPINs::dlgWndAskPINs( DlgPinInfo pinInfo1, DlgPinInfo pinInfo2, QString & Header, QString & PINName, bool UseKeypad, QWidget *parent ) : dlgWndBase(parent)
{
	ui.setupUi(this);

	setFixedSize(416, 258);

    if(PINName.contains("Assinatura", Qt::CaseInsensitive))
        this->setStyleSheet("background-image: url(:/Resources/bg_SignaturePin.png);");
    else
        this->setStyleSheet("background-image: url(:/Resources/bg_AuthenticationPin.png);");

	//this->resize( 350, 280 );
	m_UK_InputField = 0;
	m_ulPin1MinLen = pinInfo1.ulMinLen;
	m_ulPin2MinLen = pinInfo2.ulMinLen;
	m_ulPin1MaxLen = pinInfo1.ulMaxLen;
	m_ulPin2MaxLen = pinInfo2.ulMaxLen;
	m_ulPin2Flags  = pinInfo2.ulFlags;

	if( pinInfo1.ulFlags & PIN_FLAG_DIGITS )
	{
		char buffer[20];
		sprintf(buffer,"[0-9]{%ld,%ld}",m_ulPin1MinLen,m_ulPin1MaxLen);
		QRegExp rx(buffer);
		m_Pin1Validator=new QRegExpValidator(rx, 0);
	}
	else
	{
		m_Pin1Validator=NULL;
	}

	if( pinInfo2.ulFlags & PIN_FLAG_DIGITS )
	{
		char buffer[20];
		sprintf(buffer,"[0-9]{%ld,%ld}",m_ulPin2MinLen,m_ulPin2MaxLen);
		QRegExp rx(buffer);
		m_Pin2Validator=new QRegExpValidator(rx, 0);
	}
	else
	{
		m_Pin2Validator=NULL;
	}

	QString Title;

	
		this->setWindowIcon( QIcon( ":/Resources/ICO_CARD_EID_PLAIN_16x16.png" ) );
	
		Title+= QString::fromWCharArray(GETSTRING_DLG(RenewingPinCode));
		this->setWindowTitle( Title );

		ui.lblHeader->setText( QString::fromWCharArray(GETSTRING_DLG(EnterYourPin)) );
		ui.lblHeader->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(EnterYourPin)) );
		ui.lblPINName_2->setText( QString::fromWCharArray(GETSTRING_DLG(Pin)) );
		ui.lblPINName_2->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Pin)) );

		ui.lblOldPINName->setText( QString::fromWCharArray(GETSTRING_DLG(CurrentPin)) );
		ui.lblOldPINName->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(CurrentPin)) );
		ui.lblNewPIN1->setText( QString::fromWCharArray(GETSTRING_DLG(NewPin)) );
		ui.lblNewPIN1->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(NewPin)) );
		ui.lblNewPIN2->setText( QString::fromWCharArray(GETSTRING_DLG(ConfirmNewPin)) );
		ui.lblNewPIN2->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ConfirmNewPin)) );

		ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
		ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Ok)) );
		ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
		ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );

		ui.fraPIN_Keypad->setVisible( false );
		connect( ui.btnOk, SIGNAL( clicked() ), this, SLOT( FinalCheck() ) );

		if( !Header.isEmpty() )
		{
			ui.lblHeader->setText( Header );
			ui.lblHeader->setAccessibleName( Header );
		}

		OldPIN_OK = NewPIN1_OK = NewPIN2_OK = false;

	    //Max Length of PINs for PTEID cards as currently defined by INCM personalization
		ui.txtOldPIN->setMaxLength(8);

		ui.txtNewPIN1->setMaxLength(8);
		ui.txtNewPIN2->setMaxLength(8);

}

dlgWndAskPINs::~dlgWndAskPINs()
{
	if(m_Pin1Validator)
	{
		delete m_Pin1Validator;
		m_Pin1Validator=NULL;
	}

	if(m_Pin2Validator)
	{
		delete m_Pin2Validator;
		m_Pin2Validator=NULL;
	}
}

void dlgWndAskPINs::on_txtOldPIN_textChanged( const QString & text )
{
	OldPIN_OK = (unsigned) text.length() >= m_ulPin1MinLen;
	ui.btnOk->setEnabled( TestPINs() );
}

void dlgWndAskPINs::on_txtNewPIN1_textChanged( const QString & text )
{
	NewPIN1_OK = (unsigned) text.length() >= m_ulPin2MinLen;
	ui.btnOk->setEnabled( TestPINs() );
}

void dlgWndAskPINs::on_txtNewPIN2_textChanged( const QString & text )
{
	NewPIN2_OK = (unsigned) text.length() >= m_ulPin2MinLen;
	ui.btnOk->setEnabled( TestPINs() );
}

void dlgWndAskPINs::on_txtPIN_Keypad_textChanged( const QString & text )
{
	if( m_UK_InputField == INPUTFIELD_OLD )
	{
		ui.btnOk->setEnabled( (unsigned) text.length() >= m_ulPin1MinLen );
	}
	else // INPUTFIELD_NEW || INPUTFIELD_CONFIRM
	{
		ui.btnOk->setEnabled( (unsigned) text.length() >= m_ulPin2MinLen );
	}
}

void dlgWndAskPINs::NextField()
{
    switch( m_UK_InputField )
	{
		case INPUTFIELD_OLD: // 0 
		{
			ui.txtOldPIN->setText( ui.txtPIN_Keypad->text() );
			ui.txtOldPIN->setAccessibleName( ui.txtPIN_Keypad->text() );
			ui.lblHeader->setText( QString::fromWCharArray(GETSTRING_DLG(EnterYourNewPinCode)) );
			ui.lblHeader->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(EnterYourNewPinCode)) );
			m_ulPinMaxLen=m_ulPin2MaxLen;
			if( m_ulPin2Flags & PIN_FLAG_DIGITS )
				ui.txtPIN_Keypad->setValidator(m_Pin2Validator);
			else
				ui.txtPIN_Keypad->setMaxLength( m_ulPin2MaxLen );

			break;
		}

		case INPUTFIELD_NEW: // 1
		{
			ui.txtNewPIN1->setText( ui.txtPIN_Keypad->text() );
			ui.txtNewPIN1->setAccessibleName( ui.txtPIN_Keypad->text() );
			ui.lblHeader->setText( QString::fromWCharArray(GETSTRING_DLG(EnterYourNewPinCodeAgainToConfirm)));
			ui.lblHeader->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(EnterYourNewPinCodeAgainToConfirm)));
            //ui.lblError->setVisible( false );
			break;
		}

		case INPUTFIELD_CONFIRM: // 2
		{
			ui.txtNewPIN2->setText( ui.txtPIN_Keypad->text() );
			ui.txtNewPIN2->setAccessibleName( ui.txtPIN_Keypad->text() );
			FinalCheck();
			return;
		}		
	}
	m_UK_InputField++;
	ui.txtPIN_Keypad->clear();
}

void dlgWndAskPINs::FinalCheck()
{
	if( ui.txtNewPIN2->text() == ui.txtNewPIN1->text() && !ui.txtNewPIN1->text().isEmpty() )
	{
		this->accept();
	}
	else
	{
        //ui.lblError->setText( QString::fromWCharArray(GETSTRING_DLG(ErrorTheNewPinCodesAreNotIdentical)) );
        //ui.lblError->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ErrorTheNewPinCodesAreNotIdentical)) );
        //ui.lblError->setVisible( true );
		if( m_UseKeypad )
		{
			m_UK_InputField = INPUTFIELD_NEW;
			ui.txtPIN_Keypad->clear();
			ui.lblHeader->setText( QString::fromWCharArray(GETSTRING_DLG(RetryEnterYourNewPinCode)) );
			ui.lblHeader->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(RetryEnterYourNewPinCode)) );
		}
	}
}

void dlgWndAskPINs::on_tbtNUM_0_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"0" ); 
}

void dlgWndAskPINs::on_tbtNUM_1_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"1" ); 
}

void dlgWndAskPINs::on_tbtNUM_2_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"2" ); 
}

void dlgWndAskPINs::on_tbtNUM_3_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"3" ); 
}

void dlgWndAskPINs::on_tbtNUM_4_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"4" ); 
}

void dlgWndAskPINs::on_tbtNUM_5_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"5" ); 
}

void dlgWndAskPINs::on_tbtNUM_6_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"6" ); 
}

void dlgWndAskPINs::on_tbtNUM_7_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"7" ); 
}

void dlgWndAskPINs::on_tbtNUM_8_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"8" ); 
}

void dlgWndAskPINs::on_tbtNUM_9_clicked()
{
	if( (unsigned) ui.txtPIN_Keypad->text().size() < m_ulPinMaxLen ) ui.txtPIN_Keypad->setText( ui.txtPIN_Keypad->text() +"9" ); 
}

void dlgWndAskPINs::on_tbtClear_clicked()
{
	ui.txtPIN_Keypad->clear(); 
}
