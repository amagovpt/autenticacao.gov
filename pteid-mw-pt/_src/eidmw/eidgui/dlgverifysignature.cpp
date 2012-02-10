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
	const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
	this->setWindowIcon( Ico );

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
    QString nativedafaultpath;
    bool vsignsucess;
    PTEID_SigVerifier vsign;
    char *error, *sig_path_native;
    unsigned long errorlen = 500;
	

    getSignFile = QFileDialog::getOpenFileName(this, tr("Open Signature files"), QDir::homePath(), tr("Zip files 'XAdES' (*.zip)"), NULL);
    QCoreApplication::processEvents();

    if (!getSignFile.isEmpty())
    {
        nativedafaultpath = QDir::toNativeSeparators(getSignFile);
	
	sig_path_native = new char[nativedafaultpath.size()*2];
        error = new char[errorlen];
	strcpy(sig_path_native, nativedafaultpath.toStdString().c_str());

        vsignsucess = vsign.VerifySignature(sig_path_native, error, &errorlen);

        if (vsignsucess)
        {
            QMessageBox::information(this, tr("Verify Signature"), tr("Signature was successfully verified."));
            this->close();

        }
	else
	{
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", 
				"Error message received from VerifySignature() size=%d: %s", errorlen, error);
		QMessageBox::critical(this, tr("Verify Signature"), QString::fromAscii(error, errorlen));
		this->close();
	}
    }
}
