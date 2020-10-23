/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011-2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2016, 2018-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
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
#include <QDesktopWidget>

#include <iostream>
#include <stdio.h>


dlgWndAskPINs::dlgWndAskPINs( DlgPinInfo pinInfo1, DlgPinInfo pinInfo2, QString & Header, QString & PINName, bool UseKeypad, QWidget *parent, Type_WndGeometry *pParentWndGeometry ) : dlgWndBase(parent)
{
	ui.setupUi(this);

	setFixedSize(this->width(), this->height());

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

    if (PINName.contains("PUK"))
    {
        Title+= QString::fromWCharArray(GETSTRING_DLG(Unblock));
    }
    else
    {
        Title+= QString::fromWCharArray(GETSTRING_DLG(RenewingPinCode));
    }

    this->setWindowTitle( Title );
    this->setWindowTitle( Title );
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );

    // HEADER
    if( Header.isEmpty() )
        Header = QString::fromWCharArray(GETSTRING_DLG(EnterYourPin));

    ui.lblHeader->setText( Header );
    ui.lblHeader->setAccessibleName( Header );
    ui.lblHeader->setStyleSheet("QLabel { color : #3C5DBC; font-size:16pt; font-weight:800 }");

    // ICON
    QPixmap pix(":/images/autenticacao.bmp");
    ui.lblIcon->setPixmap(pix);
    ui.lblIcon->setScaledContents(true);

    // ENTER YOUR PIN LABEL
    QString enterYourPINText = QString::fromWCharArray(GETSTRING_DLG(EnterYour))
                               + " " + PINName + " "
                               + QString::fromWCharArray(GETSTRING_DLG(OfCitizenCard)) + ".";
    ui.lblEnterYourPIN->setText( enterYourPINText );
    ui.lblEnterYourPIN->setAccessibleName( enterYourPINText );
    ui.lblEnterYourPIN->setStyleSheet("QLabel { color : #000000; font-size:12pt; font-weight:500 }");

    ui.lblPINName_2->setText( QString::fromWCharArray(GETSTRING_DLG(Pin)) );
    ui.lblPINName_2->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Pin)) );
    ui.lblPINName_2->setStyleSheet("QLabel { color : #3C5DBC; font-size:12pt; }");

    // CURRENT PIN
    if (PINName.contains("PUK"))
        ui.lblOldPINName->setText( QString::fromWCharArray(GETSTRING_DLG(Puk)) );
    else
        ui.lblOldPINName->setText( QString::fromWCharArray(GETSTRING_DLG(CurrentPin)) );
    ui.lblOldPINName->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(CurrentPin)) );
    ui.lblOldPINName->setStyleSheet("QLabel { color : #000000; font-size: 12pt; background: rgba(0,0,0,0);}");

    ui.txtOldPIN->setMaxLength(8); //Max Length of PINs for PTEID cards as currently defined by INCM personalization
    ui.txtOldPIN->setStyleSheet("QLineEdit {border: 2px solid #D6D7D7; padding-left: 10px}\
QLineEdit:focus {border: 3px solid #D6D7D7;}");

    // NEW PIN
    ui.lblNewPIN1->setText( QString::fromWCharArray(GETSTRING_DLG(NewPin)) );
    ui.lblNewPIN1->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(NewPin)) );
    ui.lblNewPIN1->setStyleSheet("QLabel { color : #000000; font-size: 12pt; background: rgba(0,0,0,0);}");

    ui.txtNewPIN1->setMaxLength(8); //Max Length of PINs for PTEID cards as currently defined by INCM personalization
    ui.txtNewPIN1->setStyleSheet("QLineEdit {border: 2px solid #D6D7D7; padding-left: 10px}\
QLineEdit:focus {border: 3px solid #D6D7D7;}");

    // CONFIRM NEW PIN
    ui.lblNewPIN2->setText( QString::fromWCharArray(GETSTRING_DLG(ConfirmNewPin)) );
    ui.lblNewPIN2->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ConfirmNewPin)) );
    ui.lblNewPIN2->setStyleSheet("QLabel { color : #000000; font-size: 12pt; background: rgba(0,0,0,0);}");

    ui.txtNewPIN2->setMaxLength(8); //Max Length of PINs for PTEID cards as currently defined by INCM personalization
    ui.txtNewPIN2->setStyleSheet("QLineEdit {border: 2px solid #D6D7D7; padding-left: 10px}\
QLineEdit:focus {border: 3px solid #D6D7D7;}");

    OldPIN_OK = NewPIN1_OK = NewPIN2_OK = false;

    // OK BUTTON
    ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Confirm)) );
    ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Confirm)) );
    ui.btnOk->setStyleSheet("QPushButton {background-color: rgba(60, 93, 188, 0.7); color: #ffffff; border-radius: 0}\
QPushButton:hover{background-color: #2C3DAC}");
    connect( ui.btnOk, SIGNAL( clicked() ), this, SLOT( FinalCheck() ) );

    // CANCEL BUTTON
    ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
    ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
    ui.btnCancel->setStyleSheet("QPushButton {background-color: #D6D7D7; color: #3C5DBC; border-radius: 0}\
QPushButton:hover{background-color: #C6C7C7}");

    ui.fraPIN_Keypad->setVisible( false );

    Type_WndGeometry WndGeometry;
    if ( getWndCenterPos( pParentWndGeometry
                        , QApplication::desktop()->width(), QApplication::desktop()->height()
                        , this->width(), this->height()
                        , &WndGeometry ) ){
        this->move( WndGeometry.x, WndGeometry.y );
    }
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
	updateColors(ui.txtOldPIN, OldPIN_OK);
}

void dlgWndAskPINs::on_txtNewPIN1_textChanged( const QString & text )
{
	NewPIN1_OK = (unsigned) text.length() >= m_ulPin2MinLen;
	ui.lblError->setVisible(false);
	updateColors(ui.txtNewPIN1, NewPIN1_OK);
}

void dlgWndAskPINs::on_txtNewPIN2_textChanged( const QString & text )
{
	NewPIN2_OK = (unsigned) text.length() >= m_ulPin2MinLen;
	ui.lblError->setVisible(false);
	updateColors(ui.txtNewPIN2, NewPIN2_OK);
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

void dlgWndAskPINs::FinalCheck()
{
	if( ui.txtNewPIN2->text() == ui.txtNewPIN1->text() && !ui.txtNewPIN1->text().isEmpty() )
	{
		this->accept();
	}
	else
	{
        ui.lblError->setText( QString::fromWCharArray(GETSTRING_DLG(ErrorTheNewPinCodesAreNotIdentical)) );
        ui.lblError->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(ErrorTheNewPinCodesAreNotIdentical)) );
        ui.lblError->setVisible(true);
		if( m_UseKeypad )
		{
			m_UK_InputField = INPUTFIELD_NEW;
			ui.txtPIN_Keypad->clear();
			ui.lblHeader->setText( QString::fromWCharArray(GETSTRING_DLG(RetryEnterYourNewPinCode)) );
			ui.lblHeader->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(RetryEnterYourNewPinCode)) );
		}
	}
}

void dlgWndAskPINs::updateColors( QLineEdit* lineEdit, bool bOk )
{
	// Update text field border color
	QString borderColor = bOk ? "#3C5DBC" : "#D6D7D7"; // blue : grey
	lineEdit->setStyleSheet("QLineEdit {border: 2px solid " + borderColor + "; padding-left: 10px}\
QLineEdit:focus {border: 3px solid " + borderColor + ";}");

	// Update OK button enabled status and color
	QString buttonColor = TestPINs() ? "#3C5DBC" : "rgba(60, 93, 188, 0.7)"; // blue : blue with transparency
	ui.btnOk->setEnabled(TestPINs());
	ui.btnOk->setStyleSheet("QPushButton {background-color: " + buttonColor + "; color: #ffffff; border-radius: 0}\
QPushButton:hover{background-color: #2C3DAC}");
}

void dlgWndAskPINs::on_tbtClear_clicked()
{
	ui.txtPIN_Keypad->clear(); 
}
