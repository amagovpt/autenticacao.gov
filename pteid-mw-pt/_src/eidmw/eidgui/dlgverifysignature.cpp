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

#include "dlgverifysignature.h"

#include "eidlib.h"
#include "mainwnd.h"

using namespace eIDMW;

dlgVerifySignature::dlgVerifySignature( QWidget* parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	//Set icon
	//const QIcon Ico = QIcon( ":/images/Images/Icons/Print.png" );
	//this->setWindowIcon( Ico );

	QDesktopWidget* desktop = QApplication::desktop();
	int screenNr = desktop->screenNumber();
	QRect rect = desktop->availableGeometry(screenNr);
	int height = rect.height();

	int thiswidth = this->width();
	int thisheight = this->height();

	if (thisheight > height)
	{
		this->resize(thiswidth,height-20); //make sure the window fits
	}

}

dlgVerifySignature::~dlgVerifySignature()
{

}

void dlgVerifySignature::on_pbCancel_clicked()
{
        done(0);
}

void dlgVerifySignature::on_pbOpenSign_clicked()
{
    QString getSignFile;
    QString defaultopenfilepath;
    bool vsignsucess;
    PTEID_SigVerifier vsign;

    defaultopenfilepath = QDir::homePath();
    getSignFile = QFileDialog::getOpenFileName(this, tr("Open Signature files"), defaultopenfilepath, tr("Zip files 'XAdES' (*.zip)"), NULL);
    QCoreApplication::processEvents();

    char *error;
    unsigned long errorlen;

    error = new char[500];

    errorlen = sizeof(error);

    vsignsucess = vsign.VerifySignature(getSignFile.toStdString().c_str(), error, &errorlen);

    if (vsignsucess)
    {
        QMessageBox::information(this, tr("Error"), tr("Verified Signature with success"));
        this->close();
    } else {
        std::cout << "error " << error << " error_len " << std::string(error, errorlen) << std::endl;
        QMessageBox::critical(this, tr("Error"), QString::fromUtf8(error, errorlen));
        this->close();
    }
}
