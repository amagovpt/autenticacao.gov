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


QString dlgVerifySignature::translateVerifyReturnCode(int return_code)
{

	QString msg(tr("Signature Validation error: "));
switch (return_code)
{
	case 1:
		msg += tr("Couldn't extract signature from zip container");
		break;
	case 2:
		msg += tr("RSA Signature of referenced content is invalid");
		break;
	case 3:
		msg += tr("At least one of the signed file(s) was changed or is missing");
		break;
	case 4:
		msg += tr("RSA Signature of referenced content is invalid");
		break;
	case 5:
		msg += tr("The certificate used to sign this data is not trusted");
		break;
	case 6:
		msg += tr("The timestamp appended to the signature is invalid");
		break;

}

return msg;
}

void dlgVerifySignature::on_pbOpenSign_clicked()
{
    QString getSignFile;
    QString nativedafaultpath;
    char *sig_path_native;
    int return_code = 0;
	

    getSignFile = QFileDialog::getOpenFileName(this, tr("Open Signature files"), QDir::homePath(), tr("Zip files 'XAdES' (*.zip)"), NULL);
    QCoreApplication::processEvents();

    if (!getSignFile.isEmpty())
    {
        nativedafaultpath = QDir::toNativeSeparators(getSignFile);
	
		sig_path_native = new char[nativedafaultpath.size()*2];
		strcpy(sig_path_native, nativedafaultpath.toStdString().c_str());

		PTEID_SigVerifier vsign(sig_path_native);
	        return_code = vsign.Verify();

		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", 
				"Return code from VerifySignature(): %d", return_code);

		QString signer = QString("\n")+tr("Signed by: ");

		//If the signature was extracted get the signer name
		if (return_code != 1)
			signer += QString::fromUtf8(vsign.GetSigner());

		if (return_code == 0)
		{
			QString msg = tr("Signature was successfully verified.");
			char *ts = vsign.GetTimestampString();
			if (ts != NULL)
			{
				QString timestamp = QString("\n") + tr("Timestamp: ");
				msg += timestamp;
				msg += ts;
			}
			msg += signer;

			QMessageBox::information(this, tr("Signature Validation"), msg);

			this->close();

		}
		else
		{
			QString error = translateVerifyReturnCode(return_code);
			if (return_code != 1)
				error += signer;

			QMessageBox::critical(this, tr("Signature Validation"), error);
			this->close();
		}
	}
}
