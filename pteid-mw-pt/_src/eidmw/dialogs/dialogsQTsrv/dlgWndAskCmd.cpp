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

#include "dlgWndAskCmd.h"
#include "../countryCallingCodeList.h"
#include "../langUtil.h"
#include <QDesktopWidget>

#include <vector>
#include <stdio.h>


dlgWndAskCmd::dlgWndAskCmd(DlgCmdOperation operation, bool isValidateOtp,
        QString & Header,
        std::wstring *inOutId, std::wstring *userName,
        bool callbackWasCalled, bool askForId, QWidget *parent, Type_WndGeometry *pParentWndGeometry )  : dlgWndBase(parent)
{
    ui.setupUi(this);

    /* the otp dialog is taller by otpDialogStretch px.*/
    int otpDialogStretch = 70;
    int otpCautionStretch = 40;
    int height = (isValidateOtp ? this->height() + otpDialogStretch + otpCautionStretch: this->height());
    setFixedSize(this->width(), height);

    // TODO: this should come from a pin info struct. (same for Win32 dialogs)
    m_ulIdMinLen = 1;
    m_ulIdMaxLen = 50;
    m_ulCodeMinLen = (isValidateOtp ? 6 : 4);
    m_ulCodeMaxLen = (isValidateOtp ? 6 : 8);

    char buffer[20];
    sprintf(buffer,"[0-9]{%ld,%ld}",m_ulIdMinLen,m_ulIdMaxLen);
    QRegExp rx(buffer);
    m_IdValidator=new QRegExpValidator(rx, 0);

    sprintf(buffer,"[0-9]{%ld,%ld}",m_ulCodeMinLen,m_ulCodeMaxLen);
    rx.setPattern(buffer);
    m_CodeValidator=new QRegExpValidator(rx, 0);

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
        ui.lblTitle->move(ui.lblTitle->x(), ui.lblTitle->y() + 20); // move the title down since it uses two lines
        break;
    default:
        break;
    }

    this->setWindowTitle( Title );
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );
    ui.lblTitle->setText( Title );
    ui.lblTitle->setAccessibleName( Title );
    ui.lblTitle->setStyleSheet("QLabel { color : #3C5DBC; font-size:16pt; font-weight:800 }");

    // HEADER
    if( Header.isEmpty() && operation == DlgCmdOperation::DLG_CMD_SIGNATURE)
        Header = QString::fromWCharArray(GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronicWithCmd));

    ui.lblHeader->setText( Header );
    ui.lblHeader->setAccessibleName( Header );
    if (isValidateOtp)
    {
        ui.lblHeader->setStyleSheet("QLabel { color : #000000; font-size:12pt;}");
    } else
    {
        ui.lblHeader->setStyleSheet("QLabel { color : #000000; font-size:12pt; font-weight:500; background:rgba(0,0,0,0);}");
    }

    if (!isValidateOtp && operation == DlgCmdOperation::DLG_CMD_SIGNATURE) 
    {
        ui.lblHeader->clear();
        ui.lblMobile->move(ui.lblMobile->x(), ui.lblMobile->y() - 40);
        ui.txtMobile->move(ui.txtMobile->x(), ui.txtMobile->y() - 40);
        ui.listCountryCode->move(ui.listCountryCode->x(), ui.listCountryCode->y() - 40);
        ui.txtCode->move(ui.txtCode->x(), ui.txtCode->y() - 20);
        ui.lblCode->move(ui.lblCode->x(), ui.lblCode->y() - 20);
    }

    // DOCUMENT IDENTIFIER BOX
    if (isValidateOtp)
    {
        if (operation == DlgCmdOperation::DLG_CMD_SIGNATURE)
        {
            ui.lblDocumentId->setVisible(true);
            QString documentIdLabel = QString::fromWCharArray(GETSTRING_DLG(SigningDataWithIdentifier));
            documentIdLabel += "<br><b>\"";
            documentIdLabel += QString::fromWCharArray(inOutId->c_str());
            documentIdLabel += "\"</b>";
            ui.lblDocumentId->setText(documentIdLabel);
        }
        else if (operation == DlgCmdOperation::DLG_CMD_GET_CERTIFICATE)
        {
            ui.lblHeader->move(ui.lblHeader->x(), ui.lblHeader->y() + 70); // there is no id box => fill the space by moving header down
        }
    }

    // ICON
    QPixmap pix(":/images/autenticacao.bmp");
    ui.lblIcon->setPixmap(pix);
    ui.lblIcon->setScaledContents(true);

    // ENTER YOUR MOBILE
    if (!isValidateOtp)
    {
        QString cmdLink = "https://www.autenticacao.gov.pt/cmd-pedido-chave";
        QString clickToActivateText = QString("<a style='text-decoration:none' href='%1'>%2</a>")
                                      .arg(cmdLink, QString::fromWCharArray(GETSTRING_DLG(ActivateOrManageCMD)));
        ui.lblClickToActivate->setText(clickToActivateText);
        ui.lblClickToActivate->setOpenExternalLinks(true);

        if (operation == DlgCmdOperation::DLG_CMD_GET_CERTIFICATE) // header uses 2 lines, so link needs to be lower
            ui.lblClickToActivate->move(ui.lblClickToActivate->x(), ui.lblClickToActivate->y() + 15);

        QString enterYourMobileText = QString::fromWCharArray(GETSTRING_DLG(InsertMobileNumber));
        ui.lblMobile->setText( enterYourMobileText );
        ui.lblMobile->setAccessibleName( enterYourMobileText );
        ui.lblMobile->setStyleSheet("QLabel { color : #000000; font-size:12pt;}");

        ui.txtMobile->setMaxLength(m_ulIdMaxLen);
        if (m_IdValidator)
            ui.txtMobile->setValidator(m_IdValidator);
        ui.txtMobile->setFocus();
        ui.txtMobile->setStyleSheet("QLineEdit {border: 2px solid #D6D7D7; padding-left: 10px}\
    QLineEdit:focus {border: 3px solid #D6D7D7;}");

        std::vector<QString> countryCodeList = getCountryCallingCodeList();
        for (size_t i = 0; i < countryCodeList.size(); i++)
        {
            ui.listCountryCode->addItem(countryCodeList[i]);
        }

        //Pre-select country code if cached mobile number is available

        std::wstring cachedMobile;
        int cachedPrefix = -1;
        stripPrefixFromMobile(*inOutId, &cachedPrefix, &cachedMobile);

        if (cachedPrefix >= 0) {
            ui.listCountryCode->setCurrentIndex(cachedPrefix);
        }
        //Fill mobile number (excluding country code prefix) if already available
        if (cachedMobile.length() > 0) {

            ui.txtMobile->setText(QString::fromWCharArray(cachedMobile.c_str()));
            ui.txtCode->setFocus();
        }
        else {
            ui.txtMobile->setFocus();
        }


        ui.listCountryCode->setStyleSheet("QComboBox {border: 2px solid #3C5DBC; padding-left: 10px; combobox-popup: 0;\
         selection-background-color: #D6D7D7;selection-color: #3C5DBC;}\
        QComboBox:focus {border: 3px solid #3C5DBC;}\
        QComboBox::drop-down {border: 0px; width: 20px;}\
        QComboBox::down-arrow {\
            image: url(:/images/downarrow.png); width: 10px; }");
    }
    else
    {
        m_Id_OK = true;
        QString sendSmsLabel = QString::fromWCharArray(GETSTRING_DLG(ToSendSmsPress));
        sendSmsLabel += " \"";
        sendSmsLabel += QString::fromWCharArray(GETSTRING_DLG(SendSms));
        sendSmsLabel += "\".";
        ui.lblClickToActivate->setVisible(false);
        ui.lblSendSms->setText(sendSmsLabel);
        ui.lblMobile->setVisible(false);
        ui.txtMobile->setVisible(false);
        ui.listCountryCode->setVisible(false);
        ui.rectSendSms->setVisible(true);
        QString btnSmsText = QString::fromWCharArray(GETSTRING_DLG(SendSms));
        ui.btnSendSms->setText(btnSmsText);
        ui.btnSendSms->setStyleSheet("QPushButton {background-color: #D6D7D7; color: #3C5DBC; border-radius: 0; border: transparent;}\
QPushButton:hover{background-color: #C6C7C7}");
        if (callbackWasCalled)
        {
            // Disable button to send SMS (it was already pressed)
            ui.btnSendSms->setEnabled(false);
            ui.btnSendSms->setStyleSheet("QPushButton {background-color: #f1f2f2; color: #9daedd; border-radius: 0; border: transparent;}");
        }
        ui.txtCode->setFocus();
    }

    // ENTER CODE
    QString enterCodeText = (isValidateOtp ?
         QString::fromWCharArray(GETSTRING_DLG(InsertSecurityCode)):
         QString::fromWCharArray(GETSTRING_DLG(SignaturePinCmd)));
    ui.lblCode->setText( enterCodeText );
    ui.lblCode->setAccessibleName( enterCodeText );
    ui.lblCode->setStyleSheet("QLabel { color : #000000; font-size: 12pt; background: rgba(0,0,0,0);}");

    ui.txtCode->setMaxLength(m_ulCodeMaxLen);
    if (m_CodeValidator)
            ui.txtCode->setValidator(m_CodeValidator);
    ui.txtCode->setStyleSheet("QLineEdit {border: 2px solid #D6D7D7; padding-left: 10px}\
QLineEdit:focus {border: 3px solid #D6D7D7;}");
    if (!isValidateOtp)
    {
        ui.txtCode->setEchoMode(QLineEdit::Password);
    }

    // CAUTION MESSAGE
    if (isValidateOtp) {
        QString Caution;
        Caution += QString::fromWCharArray(GETSTRING_DLG(Caution));
        Caution += " ";
        Caution += QString::fromWCharArray(GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronicWithCmd));
        ui.lblCaution->setText( Caution );
        ui.lblCaution->setAccessibleName( Caution );
    }
    
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

    /* If validate otp dialog, move some widgets down since the dialog is taller (make room for "send SMS" button).*/
    if (isValidateOtp)
    {
        ui.widget->resize(ui.widget->width(), ui.widget->height() + otpDialogStretch + otpCautionStretch);
        ui.btnOk->move(ui.btnOk->x(), ui.btnOk->y() + otpDialogStretch + otpCautionStretch);
        ui.btnCancel->move(ui.btnCancel->x(), ui.btnCancel->y() + otpDialogStretch + otpCautionStretch);
        ui.txtCode->move(ui.txtCode->x(), ui.txtCode->y() + otpDialogStretch + 10);
        ui.lblCode->move(ui.lblCode->x(), ui.lblCode->y() + otpDialogStretch + 10);
    }


    Type_WndGeometry WndGeometry;
    if ( getWndCenterPos( pParentWndGeometry
                        , QApplication::desktop()->width(), QApplication::desktop()->height()
                        , this->width(), this->height()
                        , &WndGeometry ) ){
        this->move( WndGeometry.x, WndGeometry.y );
    }
}

dlgWndAskCmd::~dlgWndAskCmd()
{
    if(m_IdValidator)
    {
        delete m_IdValidator;
        m_IdValidator=NULL;
    }

    if(m_CodeValidator)
    {
        delete m_CodeValidator;
        m_CodeValidator=NULL;
    }
}

void dlgWndAskCmd::on_txtMobile_textChanged( const QString & text )
{
    m_Id_OK = (unsigned) text.length() >= m_ulIdMinLen && text.length() <= m_ulIdMaxLen;
    updateColors(ui.txtMobile, m_Id_OK);
}

void dlgWndAskCmd::on_txtCode_textChanged( const QString & text )
{
    m_Code_OK = (unsigned) text.length() >= m_ulCodeMinLen && text.length() <= m_ulCodeMaxLen;
    ui.lblError->setVisible(false);
    updateColors(ui.txtCode, m_Code_OK);
}

void dlgWndAskCmd::sendSms()
{
    m_callCallback = true;
    accept(); // close the dialog;
}

void dlgWndAskCmd::updateColors( QLineEdit* lineEdit, bool bOk )
{
    // Update text field border color
    QString borderColor = bOk ? "#3C5DBC" : "#D6D7D7"; // blue : grey
    lineEdit->setStyleSheet("QLineEdit {border: 2px solid " + borderColor + "; padding-left: 10px}\
QLineEdit:focus {border: 3px solid " + borderColor + ";}");

    // Update OK button enabled status and color
    QString buttonColor = m_Id_OK && m_Code_OK ? "#3C5DBC" : "rgba(60, 93, 188, 0.7)"; // blue : blue with transparency
    ui.btnOk->setEnabled(m_Id_OK && m_Code_OK);
    ui.btnOk->setStyleSheet("QPushButton {background-color: " + buttonColor + "; color: #ffffff; border-radius: 0}\
QPushButton:hover{background-color: #2C3DAC}");
}

std::wstring dlgWndAskCmd::getId()
{
    QString mobile;
    QString currentCountryCode = ui.listCountryCode->currentText();
    int codeDelimiterIndex = currentCountryCode.indexOf("-");
    if (codeDelimiterIndex > 0)
    {
        mobile += currentCountryCode.left(codeDelimiterIndex - 1);
    }
    mobile += ui.txtMobile->text();
    return mobile.toStdWString();
}