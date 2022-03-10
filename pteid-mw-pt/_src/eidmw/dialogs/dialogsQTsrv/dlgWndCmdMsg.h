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

#ifndef DLGWNDCMDMSG_H
#define DLGWNDCMDMSG_H

#include <QDialog>
#include "ui_dlgWndCmdMsg.h"
#include "dlgWndBase.h"
#include "../dialogs.h"

using namespace eIDMW;

class dlgWndCmdMsg : public dlgWndBase
{
    Q_OBJECT

public:
    dlgWndCmdMsg(DlgCmdOperation operation, DlgCmdMsgType type, QString &message, unsigned long ulHandle, QWidget *parent = NULL, Type_WndGeometry *wndGeometry = NULL );
    ~dlgWndCmdMsg();

private:
    Ui::dlgWndCmdMsgClass ui;
    unsigned long m_ulHandle = 0;
    QMovie *m_movie = NULL;
};

#endif // DLGWNDCMDMSG_H