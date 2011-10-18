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
#ifndef CONFIRMDIALOGBOX_H
#define CONFIRMDIALOGBOX_H

#include <QDialog>
#include "ui_confirmdialogbox.h"
#include "myQLabel.h"

class confirmDialogBox : public QDialog
{
	Q_OBJECT

public:
	confirmDialogBox(QWidget *parent = 0, Qt::WFlags flags =  Qt::CustomizeWindowHint);
	~confirmDialogBox();
	void setLabels(QString _title, QString _sure, QString onlyTested ,QString warning, QString btnTest, QString btnNoTest) {
		this->ui.lblTitle->setText(_title);
		this->ui.lblAreYouSure->setText(_sure);
		this->ui.lblOnlyTested->setText(onlyTested);
		this->ui.lblWarning->setText(warning);
		this->ui.clbTestSignature->setText(btnTest);
		this->ui.clbDoNotTestSignature->setText(btnNoTest);
		mQLDoTest.resetLabelText();
		mQLDontTest.resetLabelText();
	};

private:
	myQLabel mQLDoTest;
	myQLabel mQLDontTest;
	Ui::confirmDialogBoxClass ui;
	QString transparentstyle;



private slots:
	void on_clbCancel_clicked();
	void on_clbDoNotTestSignature_clicked();
	void on_clbTestSignature_clicked();
};

#endif // CONFIRMDIALOGBOX_H
