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

#ifndef DLGWNDPICKDEVICE_H
#define DLGWNDPICKDEVICE_H

#include <QDialog>
#include "ui_dlgWndPickDevice.h"
#include "dlgWndBase.h"
#include "../dialogs.h"

using namespace eIDMW;

class dlgWndPickDevice : public dlgWndBase
{
    Q_OBJECT

public:
    dlgWndPickDevice(QWidget *parent = NULL, Type_WndGeometry *wndGeometry = NULL );
    ~dlgWndPickDevice();

    DlgDevice getOutDevice() { return m_outDevice; };

private:
    Ui::dlgWndPickDeviceClass ui;
    bool m_Id_OK = false;
    bool m_Code_OK = false;
    DlgDevice m_outDevice = (DlgDevice)-1;

private slots:
    void on_radioDeviceCC_clicked() { m_outDevice = DlgDevice::DLG_CC; };
    void on_radioDeviceCMD_clicked() { m_outDevice = DlgDevice::DLG_CMD; };
};

#endif // DLGWNDPICKDEVICE_H