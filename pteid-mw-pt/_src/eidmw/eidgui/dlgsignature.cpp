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

#include <fstream>
#include <iostream>

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
	QString defaultopenfilepath;

	defaultopenfilepath = QDir::homePath();
	fileselect = QFileDialog::getOpenFileNames(this, tr("Select File(s)"), defaultopenfilepath, NULL);
	QCoreApplication::processEvents();

	SignListView(fileselect);
}

void dlgSignature::SignListView (QStringList list)
{
	view = ui.listView;
	QStringListModel* localModel = new QStringListModel();

	alist.append(list);

	localModel->setStringList(alist);
	view->setModel(localModel);

	//Enable sign button now that we have data
	if (!alist.isEmpty())
		ui.pbSign->setEnabled(true);

	//signal right click
	view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect (ui.listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
}

void dlgSignature::RemoveFromView()
{
	std::cout << "remove from view" << std::endl;
	/*QModelIndex index = ui.listView->currentIndex();
	int row = index.row();
	int count = 1;

	ui.listView->model()->removeRows( 1, count, index );
	ui.listView->repaint();*/

	QModelIndexList indexes = ui.listView->selectionModel()->selectedIndexes();
	while(indexes.size()) {
		ui.listView->model()->removeRow(indexes.first().row());
		indexes = ui.listView->selectionModel()->selectedIndexes();
	}
	//ui.listView->repaint();
}

void dlgSignature::ShowContextMenu(const QPoint& pos)
{
	QPoint globalPos = ui.listView->mapToGlobal(pos);

	QMenu *myMenu = new QMenu(ui.listView);
	//myMenu->addAction("Remove Item");
	QAction *_open = new QAction("Remove", this);
	myMenu->addAction(_open);
	connect( _open, SIGNAL( triggered() ), this, SLOT( RemoveFromView() ) );
	QAction* selectedItem = myMenu->exec(globalPos);


	/*if (selectedItem)
	{
		std::cout << "remove item" << std::endl;
		//remove item;
		QModelIndex index = ui.listView->currentIndex();
		int row = index.row();
		int count = 1;

		ui.listView->model()->removeRows( row, count, index );
		ui.listView->update();
	}*/
}

void dlgSignature::on_pbSign_clicked ( void )
{
	QAbstractItemModel* model = view->model() ;
	QStringList strlist;

	for ( int i = 0 ; i < model->rowCount() ; ++i )
	{
		// Get item at row i, col 0.
		strlist << model->index( i, 0 ).data( Qt::DisplayRole ).toString() ;
	}

	try
	{
            int i;
            int listsize = strlist.count();
            char *cpychar;
            const char **files_to_sign = new const char*[listsize];

            for (i=0; i < listsize; i++)
            {
                int listtotalLength = strlist.at(i).size();
                cpychar = new char[listtotalLength+1];
                strcpy(cpychar, strlist.at(i).toStdString().c_str());
                files_to_sign[i] = cpychar;
            }

            QString defaultsavefilepath;
            QString savefilepath;
            QString nativedafaultpath;

            defaultsavefilepath = QDir::homePath();
            defaultsavefilepath.append("/xadessign.zip");
            nativedafaultpath = QDir::toNativeSeparators(defaultsavefilepath);
            savefilepath = QFileDialog::getSaveFileName(this, tr("Save File"), nativedafaultpath, tr("Zip files 'XAdES' (*.zip)"));

            pdialog = new QProgressDialog();
            pdialog->setWindowModality(Qt::WindowModal);
            pdialog->setWindowTitle(tr("Sign"));
            pdialog->setLabelText(tr("Signing data..."));
            pdialog->setMinimum(0);
            pdialog->setMaximum(0);

            QFuture<void> future = QtConcurrent::run(this, &dlgSignature::runsign, files_to_sign, i, savefilepath.toStdString().c_str());
            this->FutureWatcher.setFuture(future);

            pdialog->exec();

            //pdialog->close();

            delete []files_to_sign;
            delete cpychar;
	}
	catch (PTEID_Exception &e)
	{
            QString msg(tr("General exception"));
            return;
	}

        this->close();
}

void dlgSignature::runsign(const char ** paths, unsigned int n_paths, const char *output_path)
{
    unsigned long	ReaderStartIdx = 1;
    bool		bRefresh = false;
    unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
    unsigned long	ReaderIdx	   = 0;

    try
    {
        if (ReaderStartIdx!=(unsigned long)-1)
        {
            ReaderEndIdx = ReaderStartIdx+1;
        }
        else
        {
            ReaderStartIdx=0;
        }

        const char* readerName = ReaderSet.getReaderName(ReaderIdx);
        m_CurrReaderName = readerName;
        PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

        //------------------------------------
        // make always sure a card is present
        //------------------------------------
        if (ReaderContext.isCardPresent())
        {
            PTEID_EIDCard&	Card	= ReaderContext.getEIDCard();
            PTEID_ByteArray SignXades;
            SignXades = Card.SignXades(paths, n_paths, output_path);
        }
    }
    catch (PTEID_Exception &e)
    {
        QString msg(tr("General exception"));
        return;
    }

    pdialog->close();

    return;
}
