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
#include "confirmdialogbox.h"

confirmDialogBox::confirmDialogBox(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent,flags)
{
	ui.setupUi(this);
	transparentstyle = "color: rgba(0, 0, 0, 0);background-color: rgb(0, 0, 0,0);alternate-background-color: rgb(0, 0, 0,0);border-color: rgba(0, 0, 0, 0);border-top-color: rgba(0, 0, 0, 0);border-right-color: rgba(0, 0, 0, 0);border-left-color: rgba(0, 0, 0, 0);border-bottom-color: rgba(0, 0, 0, 0);gridline-color: rgba(0, 0, 0, 0);selection-color: rgba(0, 0, 0, 0);selection-background-color: rgba(0, 0, 0, 0);";

	QPalette palette = this->palette();
	palette.setBrush(QPalette::Window, QPixmap(":/images/fond_testen"));
	this->setPalette(palette);

	QBrush brushWhite(QColor(255, 255, 255, 255));
	QPalette palLabelsWhite;
	palLabelsWhite.setBrush(QPalette::Active, QPalette::WindowText, brushWhite);


	ui.lblExclamationMark->setPixmap(QPixmap(":/images/waarschuwing"));
    ui.clbCancel->setIcon(QIcon(":/images/xknop")); ui.clbCancel->setStyleSheet(transparentstyle);

	ui.clbTestSignature->setIcon(QIcon(":/images/knop_breed"));ui.clbTestSignature->setStyleSheet(transparentstyle);
	ui.clbDoNotTestSignature->setIcon(QIcon(":/images/knop_breed"));ui.clbDoNotTestSignature->setStyleSheet(transparentstyle);

	mQLDoTest.setunderlyingButton(ui.clbTestSignature);mQLDoTest.setPalette(palLabelsWhite);
	mQLDontTest.setunderlyingButton(ui.clbDoNotTestSignature);mQLDontTest.setPalette(palLabelsWhite);
	mQLDoTest.setIcons(QString(":/images/knop_breed"),QString(":/images/knop_breed_down"));
	mQLDontTest.setIcons(QString(":/images/knop_breed"),QString(":/images/knop_breed_down"));

}


confirmDialogBox::~confirmDialogBox()
{

}




void confirmDialogBox::on_clbTestSignature_clicked()
{
   this->accept();
}

void confirmDialogBox::on_clbDoNotTestSignature_clicked()
{
   this->reject();
}

void confirmDialogBox::on_clbCancel_clicked()
{
   this->reject();
}