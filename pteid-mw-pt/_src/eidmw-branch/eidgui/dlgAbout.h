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

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include "ui_dlgAbout.h"
#include <QDialog>

class dlgAbout : public QDialog
{
    Q_OBJECT

private slots:

public:
    dlgAbout(QString const& version, QWidget *parent = 0);
    ~dlgAbout();

private:
    Ui_dlgAbout ui;
};

#endif
