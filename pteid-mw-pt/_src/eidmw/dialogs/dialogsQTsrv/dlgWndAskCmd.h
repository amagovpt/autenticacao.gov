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

#ifndef DLGWNDASKCMD_H
#define DLGWNDASKCMD_H

#include <QDialog>
#include <QIntValidator>
#include "ui_dlgWndAskCmd.h"
#include "dlgWndBase.h"
#include "../dialogs.h"

using namespace eIDMW;

#define INPUTFIELD_ID 0
#define INPUTFIELD_CODE 1

class dlgWndAskCmd : public dlgWndBase
{
    Q_OBJECT

public:
    dlgWndAskCmd(DlgCmdOperation operation, bool isValidateOtp,
        QString & Header,
        std::wstring *inOutId = NULL, std::wstring *userName = NULL,
        bool callbackWasCalled = false, bool askForId = false,
        QWidget *parent = NULL, Type_WndGeometry *wndGeometry = NULL );
    ~dlgWndAskCmd();

    std::wstring getId();
    std::wstring getCode(){ return QString(ui.txtCode->text()).toStdWString(); };
    bool callCallback() { return m_callCallback; };

private:
    Ui::dlgWndAskCmdClass ui;
    bool m_Id_OK = false;
    bool m_Code_OK = false;
    bool m_callCallback = false;
    void updateColors( QLineEdit* lineEdit, bool bOk );
    unsigned int m_ulIdMinLen;
    unsigned int m_ulIdMaxLen;
    unsigned int m_ulCodeMinLen;
    unsigned int m_ulCodeMaxLen;
    QRegExpValidator *m_IdValidator;
    QRegExpValidator *m_CodeValidator;

private slots:
    void on_txtMobile_textChanged( const QString & );
    void on_txtCode_textChanged( const QString & );
    void sendSms();
};

#endif // DLGWNDASKCMD_H
