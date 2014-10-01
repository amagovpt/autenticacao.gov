/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2014 Caixa Mágica Software.
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
 * Author: Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 **************************************************************************** */

#ifndef DLGCHANGEADDRESS_H_
#define DLGCHANGEADDRESS_H_

#include <QDialog>
#include <QString>
#include "ui_ChangeAddressDialog.h"

class ChangeAddressDialog : public QDialog
{
    Q_OBJECT

public:
    ChangeAddressDialog(QWidget* parent);
    ~ChangeAddressDialog();
    QString getSecretCode();
    QString getProcess();

public slots:
	void process_textEdited(QString text);
	void secretCode_textEdited(QString text);

private:
	Ui_ChangeAddressDialog ui;

};

#endif
