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

#include "dlgsignature.h"
#include "eidlib.h"
#include "mainwnd.h"

using namespace eIDMW;


dlgSignature::dlgSignature( QWidget* parent, CardInformation& CI_Data) 
    : QDialog(parent)
    , m_CI_Data(CI_Data)
    , m_CurrReaderName("")
{
    if (CI_Data.isDataLoaded())
    {

	ui.setupUi(this);
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

}

dlgSignature::~dlgSignature()
{

}

void dlgSignature::on_pbCancel_clicked( void )
{
	done(0);
}
