/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2011-2012 Caixa Mágica Software.
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
 *
 * Author: Luis Medinas <luis.medinas@caixamagica.pt>
 **************************************************************************** */

#ifndef DLGVERIFYSIGNATURE_H_
#define DLGVERIFYSIGNATURE_H_

#include <QDialog>

#include "ui_dlgVerifySignature.h"
#include "genpur.h"

class dlgVerifySignature : public QDialog
{
    Q_OBJECT

private slots:
    void on_pbCancel_clicked ( void );
    void on_pbOpenSign_clicked ( void );

public:
    dlgVerifySignature(QWidget* parent);
    ~dlgVerifySignature();

private:
    QString translateVerifyReturnCode(int return_code);
    Ui_dlgVerifySignature ui;
};

#endif /* DLGVERIFYSIGNATURE_H_ */
