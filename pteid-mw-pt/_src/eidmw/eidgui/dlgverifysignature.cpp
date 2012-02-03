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
    std::cout << "opensign clicked" << std::endl;
    QString getSignFile;
    QString defaultopenfilepath;
    QByteArray line;
    bool vsignsucess;
    PTEID_ByteArray xmlverify;

    defaultopenfilepath = QDir::homePath();
    getSignFile = QFileDialog::getOpenFileName(this, tr("Open Signature files"), defaultopenfilepath, tr("XAdES XML files (*.xml)"), NULL);
    QCoreApplication::processEvents();

    QFile file(getSignFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd()) {
        line = file.readLine();
    }

    line.data();
    xmlverify.Append((unsigned char*)line.constData(), line.size());

    /*std::cout << "xmlverify " << xmlverify.GetBytes() << std::endl;

    char *error;
    unsigned long errorlen;
    vsignsucess = VerifySignature(xmlverify, error, errorlen);
    if (vsignsuccess)
    {
        std::cout << "sucess\n";
    } else {
        std::cout << "non sucess\n";
    }*/
}
