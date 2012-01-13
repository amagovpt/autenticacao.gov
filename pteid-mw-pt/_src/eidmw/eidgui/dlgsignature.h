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

#ifndef _DLGSIGNATURE_H_
#define _DLGSIGNATURE_H_

#include "ui_dlgSignature.h"
#include <QDialog>
#include "genpur.h"
#include "CardInformation.h"

class dlgSignature : public QDialog
{
    Q_OBJECT

private slots:
	void on_pbCancel_clicked ( void );
	void on_pbAddFiles_clicked ( void );
	void on_pbSign_clicked ( void );

public:
    dlgSignature(QWidget* parent , CardInformation& CI_Data);
    ~dlgSignature();

private:
    Ui_dlgSignature ui;
    CardInformation const& m_CI_Data;
    QString m_CurrReaderName;//!< the current reader we're using

    void SignListView (QStringList list);
};

#endif
