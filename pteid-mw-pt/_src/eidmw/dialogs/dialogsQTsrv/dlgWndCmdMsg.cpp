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

#include "dlgWndCmdMsg.h"
#include "../langUtil.h"
#include <QDesktopWidget>
#include <QMovie>

dlgWndCmdMsg::dlgWndCmdMsg( DlgCmdOperation operation, DlgCmdMsgType type, QString &message, unsigned long ulHandle, QWidget *parent, Type_WndGeometry *pParentWndGeometry )  : dlgWndBase(parent)
{
    ui.setupUi(this);

    setFixedSize(this->width(), this->height());

    // TITLE
    QString Title;
    switch (operation)
    {
    case DlgCmdOperation::DLG_CMD_SIGNATURE:
        Title+= QString::fromWCharArray(GETSTRING_DLG(SigningWith));
        Title+= " ";
        Title+= QString::fromWCharArray(GETSTRING_DLG(CMD));
        break;
    case DlgCmdOperation::DLG_CMD_GET_CERTIFICATE:
        Title+= QString::fromWCharArray(GETSTRING_DLG(ObtainingCMDCert));
        break;
    default:
        break;
    }

    this->setWindowTitle( Title );
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );
    ui.lblTitle->setText( Title );
    ui.lblTitle->setAccessibleName( Title );
    ui.lblTitle->setStyleSheet("QLabel { color : #3C5DBC; font-size:16pt; font-weight:800 }");

    // ICON
    QPixmap pix(":/images/autenticacao.bmp");
    ui.lblIcon->setPixmap(pix);
    ui.lblIcon->setScaledContents(true);

    // LOADING GIF
    m_movie = new QMovie(":/images/loading.gif");
    ui.lblLoading->setMovie(m_movie);
    ui.lblLoading->setScaledContents(true);
    m_movie->start();

        QString fontSize;
#ifdef __APPLE__
    fontSize = "12pt";
#else
    fontSize = "10pt";
#endif

    // TOP TEXT
    QString topText = QString::fromWCharArray(type == DlgCmdMsgType::DLG_CMD_PROGRESS ? GETSTRING_DLG(PleaseWait) : GETSTRING_DLG(Error));
    ui.lblTopMsg->setText(topText);
    ui.lblTopMsg->setAccessibleName( topText );
    ui.lblTopMsg->setStyleSheet("QLabel { background: rgba(0,0,0,0); color : #000000; font-size:" + fontSize + "; font-weight: 500}");
    ui.lblTopMsg->setAlignment(Qt::AlignCenter);

    // BOTTOM TEXT
    ui.lblBottomMsg->setText(message);
    ui.lblBottomMsg->setAccessibleName( message );
    ui.lblBottomMsg->setStyleSheet("QLabel { background: rgba(0,0,0,0); color : #000000; font-size: " + fontSize + "; }");
    ui.lblBottomMsg->setAlignment(Qt::AlignCenter);

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

dlgWndCmdMsg::~dlgWndCmdMsg() {
    if (m_movie)
    {
        m_movie->stop();
        delete m_movie;
        m_movie = NULL;
    }
    
}
