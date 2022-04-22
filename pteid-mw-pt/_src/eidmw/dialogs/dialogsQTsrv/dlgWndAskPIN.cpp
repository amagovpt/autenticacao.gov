/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011-2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012, 2016-2019 André Guerreiro - <aguerreiro1985@gmail.com>
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

#include <iostream>

#include "dlgWndAskPIN.h"
#include "../langUtil.h"
#include <QDesktopWidget>

#include <stdio.h>

dlgWndAskPIN::dlgWndAskPIN( DlgPinInfo pinInfo, DlgPinUsage PinPusage, QString & Header, QString & PINName, bool UseKeypad, QWidget *parent, Type_WndGeometry *pParentWndGeometry ) : dlgWndBase(parent)
{
	ui.setupUi(this);


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
	Title = QString::fromWCharArray(GETSTRING_DLG(VerifyingPinCode));
	this->setWindowTitle(Title);
	this->setAccessibleName(Title);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );

	// HEADER
	if( Header.isEmpty() )
		Header = QString::fromWCharArray(GETSTRING_DLG(EnterYourPin));

	ui.lblHeader->setText( Header );
	ui.lblHeader->setAccessibleName( Header );
	ui.lblHeader->setStyleSheet("QLabel { color : #3C5DBC; font-size:16pt; font-weight:800; background:rgba(0,0,0,0)}");

	// ICON
	QPixmap pix(":/images/autenticacao.bmp");
	ui.lblImage->setPixmap(pix);
	ui.lblImage->setScaledContents(true);

	// SIGNATURE WARNING LABEL
	if( PinPusage == DLG_PIN_SIGN )
	{
		QString attentionText = QString::fromWCharArray(GETSTRING_DLG(Caution)) + " "
			+ QString::fromWCharArray(GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronic));
		ui.lblAttention->setText( attentionText );
		ui.lblAttention->setAccessibleName( attentionText );
		ui.lblAttention->setWordWrap(true);
	}

	// PIN NAME LABEL
	if( PINName.isEmpty() )
		PINName = QString::fromWCharArray(GETSTRING_DLG(AuthenticationPin));

	ui.lblPINName->setText( PINName );
	ui.lblPINName->setAccessibleName( PINName );
	ui.lblPINName->setStyleSheet("QLabel { color : #000000; font-size:12pt; }");

	// OK BUTTON
	ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Confirm)) );
	ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Confirm)) );
	ui.btnOk->setStyleSheet("QPushButton {background-color: rgba(60, 93, 188, 0.7); color: #ffffff; border-radius: 0}\
QPushButton:hover{background-color: #2C3DAC}");

	// CANCEL BUTTON
	ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
	ui.btnCancel->setStyleSheet("QPushButton {background-color: #D6D7D7; color: #3C5DBC; border-radius: 0}\
QPushButton:hover{background-color: #C6C7C7}");

	// PIN TEXT EDIT
	m_ulPinMinLen = pinInfo.ulMinLen;
	m_ulPinMaxLen = pinInfo.ulMaxLen;
	//Max Length of PINs for PTEID cards as currently defined by INCM personalization
	ui.txtPIN->setMaxLength( 8 );
	if (m_PinValidator)
		ui.txtPIN->setValidator(m_PinValidator);
	ui.txtPIN->setStyleSheet("QLineEdit {color: #000000; font-size: 12pt; border: 2px solid #D6D7D7; padding-left: 10px}\
QLineEdit:focus {border: 3px solid #D6D7D7;}");

	setFixedSize(this->width(), this->height());

    Type_WndGeometry WndGeometry;
	if ( getWndCenterPos( pParentWndGeometry
                        , QApplication::desktop()->width(), QApplication::desktop()->height()
                        , this->width(), this->height()
                        , &WndGeometry ) ){
        this->move( WndGeometry.x, WndGeometry.y );
	}
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

	return QString(ui.txtPIN->text()).toStdWString();
}

void dlgWndAskPIN::on_txtPIN_textChanged( const QString & text )
{
	bool bEnable = (unsigned) text.length() >= m_ulPinMinLen;
	QString colorButton = "rgba(60, 93, 188, 0.7)"; //blue with transparency
	QString colorInputTextBox = "#D6D7D7"; //grey
	if (bEnable)
		colorButton = colorInputTextBox = "#3C5DBC"; //blue

	ui.btnOk->setEnabled(bEnable);
	ui.txtPIN->setStyleSheet("QLineEdit {color: #000000; font-size: 12pt; border: 2px solid " + colorInputTextBox + "; padding-left: 10px;}\
QLineEdit:focus {border: 3px  solid " + colorInputTextBox + ";}");
	ui.btnOk->setStyleSheet("QPushButton {background-color: " + colorButton + "; color: #ffffff; border-radius: 0}\
QPushButton:hover{background-color: #2C3DAC}");
}