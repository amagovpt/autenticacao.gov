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

#include <QListView>

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

}

dlgSignature::~dlgSignature()
{

}

void dlgSignature::on_pbCancel_clicked( void )
{
	done(0);
}

void dlgSignature::on_pbAddFiles_clicked( void )
{
	QStringList fileselect;
	QString defaultfilepath;

	defaultfilepath = QDir::homePath();
	fileselect = QFileDialog::getOpenFileNames(this, tr("Select File(s)"), defaultfilepath, NULL);
	QCoreApplication::processEvents();

	SignListView(fileselect);
}

void dlgSignature::SignListView (QStringList list)
{
	QListView *view = ui.listView;
	QStringListModel* localModel = new QStringListModel();
	localModel->setStringList(list);
	view->setModel(localModel);
}

void dlgSignature::on_pbSign_clicked ( void )
{
	QListView *view = ui.listView;
	QStringList selectfiles ;

	QAbstractItemModel* model = view->model() ;

	for ( int i = 0 ; i < model->rowCount() ; ++i )
	{
		// Get item at row i, col 0.
		selectfiles << model->index( i, 0 ).data( Qt::DisplayRole ).toString() ;
	}

	//See result 1 element of Qlistview
	QString myString = selectfiles.at(1);
	std::cout << "strings " << myString.toStdString() << std::endl;
}
