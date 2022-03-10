/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2021 Miguel Figueira - <miguelblcfigueira@gmail.com>
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

#include "dlgWndPickDevice.h"
#include "../langUtil.h"
#include <QDesktopWidget>

dlgWndPickDevice::dlgWndPickDevice( QWidget *parent, Type_WndGeometry *pParentWndGeometry )  : dlgWndBase(parent)
{
    ui.setupUi(this);

    setFixedSize(this->width(), this->height());

    // TITLE
    QString Title;

    Title+= QString::fromWCharArray(GETSTRING_DLG(SelectDevice));

    this->setWindowTitle( Title );
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );
    ui.lblTitle->setText( Title );
    ui.lblTitle->setAccessibleName( Title );
    ui.lblTitle->setStyleSheet("QLabel { color : #3C5DBC; font-size:16pt; font-weight:800 }");

    // ICON
    QPixmap pix(":/images/autenticacao.bmp");
    ui.lblIcon->setPixmap(pix);
    ui.lblIcon->setScaledContents(true);

    // RADIO BUTTONS
    QString radioButtonSS = "QRadioButton { padding: 15px;} \
                            QRadioButton:focus {border: 2px solid #f1f2f2; padding: 13px;} \
                            QRadioButton::indicator:checked {background-color: #3C5DBC; border-radius: 7px;}";
    ui.radioDeviceCC->setText( QString::fromWCharArray(GETSTRING_DLG(CitizenCard)) );
    ui.radioDeviceCMD->setText( QString::fromWCharArray(GETSTRING_DLG(CMD)) );
    ui.radioDeviceCC->setStyleSheet(radioButtonSS);
    ui.radioDeviceCMD->setStyleSheet(radioButtonSS);
    ui.radioDeviceCC->click(); // Select CC as default

    // OK BUTTON
    ui.btnOk->setText( QString::fromWCharArray(GETSTRING_DLG(Confirm)) );
    ui.btnOk->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Confirm)) );
    ui.btnOk->setStyleSheet("QPushButton {background-color: #3C5DBC; color: #ffffff; border-radius: 0}\
QPushButton:hover{background-color: #2C3DAC}");

    // CANCEL BUTTON
    ui.btnCancel->setText( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
    ui.btnCancel->setAccessibleName( QString::fromWCharArray(GETSTRING_DLG(Cancel)) );
    ui.btnCancel->setStyleSheet("QPushButton {background-color: #D6D7D7; color: #3C5DBC; border-radius: 0}\
QPushButton:hover{background-color: #C6C7C7}");

    Type_WndGeometry WndGeometry;
    if ( getWndCenterPos( pParentWndGeometry
                        , QApplication::desktop()->width(), QApplication::desktop()->height()
                        , this->width(), this->height()
                        , &WndGeometry ) ){
        this->move( WndGeometry.x, WndGeometry.y );
    }

}

dlgWndPickDevice::~dlgWndPickDevice()
{

}

