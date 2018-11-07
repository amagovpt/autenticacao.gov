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
 * Authors: Luis Medinas <luis.medinas@caixamagica.pt>
 * 	    André Guerreiro <andre.guerreiro@caixamagica.pt>	
 *
 **************************************************************************** */

#include <QListView>
#include <QFileInfo>
#include <QFileDialog>
#include <QtConcurrent>
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QProgressDialog>
#include <QDir>

#include <fstream>
#include <iostream>
#include <string.h>

#include "dlgsignature.h"

#include "eidlib.h"
#include "mainwnd.h"

#include "eidErrors.h"
#include "dlgUtils.h"

using namespace eIDMW;


dlgSignature::dlgSignature( QWidget* parent, int selected_reader, CardInformation& CI_Data)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
    , m_CI_Data(CI_Data)
	, m_selected_reader(selected_reader)
    , m_CurrReaderName("")
{
	if (CI_Data.isDataLoaded())
	{
		ui.setupUi(this);

		//Set icon
		const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
		this->setWindowIcon( Ico );

		QDesktopWidget* desktop = QApplication::desktop();
		int screenNr = desktop->screenNumber();
		QRect rect = desktop->availableGeometry(screenNr);
		int height = rect.height();
		list_model = new QStringListModel();
		ui.listView->setModel(list_model);

		//Set XADES-BES as preselected level

		ui.radioButton_xades_b->setChecked(true);

		error_code = 0;

		int thiswidth = this->width();
		int thisheight = this->height();

		if (thisheight > height)
		{
			this->resize(thiswidth,height-20); //make sure the window fits
		}
		
		this->setFixedSize(this->width(), this->height());
	}

}

dlgSignature::~dlgSignature()
{

}

void dlgSignature::ShowErrorMsgBox()
{
	QString msg;
	QString caption  = tr("Error");
	switch(this->error_code)
	{
		case EIDMW_TIMESTAMP_ERROR:
        msg = tr("Error obtaining timestamp");
        break;

        case EIDMW_ERR_PIN_CANCEL:
        case EIDMW_ERR_TIMEOUT:
        default:
        msg = tr("Error generating Signature!");
    }

  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
  	msgBoxp.exec();
}

void dlgSignature::ShowSuccessMsgBox()
{

		QString caption  = tr("File Signature (XAdES)");
        QString msg = tr("Signature(s) succesfully generated");
		QMessageBox msgBoxp(QMessageBox::Information, caption, msg, 0, this);
  		msgBoxp.exec();

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

	for(int i=0; i != list.size(); i++)
	{

		list_model->insertRows(list_model->rowCount(), 1);
		list_model->setData(list_model->index(list_model->rowCount()-1, 0), list.at(i));

	}

	//Enable sign button now that we have data
	if (!list.isEmpty())
		ui.pbSign->setEnabled(true);

	//signal right click
	//view->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect (ui.listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
}

XadesLevel dlgSignature::getSelectedXadesLevel()
{
	if (ui.radioButton_xades_b->isChecked())
		return XADES_B;
	else if (ui.radioButton_xades_t->isChecked())
		return XADES_T;
	//else if (ui.radioButton_xades_a->isChecked())
	//	return XADES_A;
}


void dlgSignature::on_pbSign_clicked ( void )
{
	QAbstractItemModel* model = view->model() ;
	QStringList strlist;
	QFuture<void> future;
	int n_files;

	for ( int i = 0 ; i < model->rowCount() ; ++i )
	{
		// Get item at row i, col 0.
		strlist << model->index( i, 0 ).data( Qt::DisplayRole ).toString() ;
	}

	int listsize = strlist.count();
	char *cpychar;
	const char **files_to_sign = new const char*[listsize];
	char *output_file;

	bool individual_sigs = listsize > 1;

	for (n_files = 0; n_files < listsize; n_files++)
	{
		int listtotalLength = strlist.at(n_files).size();
		QString s = QDir::toNativeSeparators(strlist.at(n_files));

		cpychar = new char[listtotalLength+1];
		strcpy(cpychar, getPlatformNativeString(s) );
		files_to_sign[n_files] = cpychar;
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "File to Sign: %s", files_to_sign[n_files]);
	}
	
	//Default save filepath should be helpful and try to save in the same directory as the file(s)
	QString defaultsavefilepath = QFileInfo(strlist.last()).dir().absolutePath();
	QString savefilepath;
	QString nativedafaultpath;
	if (!individual_sigs)
		defaultsavefilepath.append("/xadessign.ccsigned");
	nativedafaultpath = QDir::toNativeSeparators(defaultsavefilepath);
	if (individual_sigs)
	{
		savefilepath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				nativedafaultpath,
				QFileDialog::ShowDirsOnly);

	}
	else
		savefilepath = QFileDialog::getSaveFileName(this, tr("Save File"), 
				nativedafaultpath, tr("XAdES Signatures (*.ccsigned)"));

	if (savefilepath.isNull() || savefilepath.isEmpty()) {
		delete[] files_to_sign;
		return;
	}

	QString native_path = QDir::toNativeSeparators(savefilepath);

	pdialog = new QProgressDialog();
	pdialog->setWindowModality(Qt::WindowModal);
	pdialog->setWindowTitle(tr("Sign"));
	pdialog->setLabelText(tr("Signing data..."));
	pdialog->setMinimum(0);
	pdialog->setMaximum(0);
	connect(&this->FutureWatcher, SIGNAL(finished()), pdialog, SLOT(cancel()));
	pdialog->reset();

	//Get the Xades-T checkbox value
	// QCheckBox *xades_t = ui.checkbox_timestamp;
	// bool is_xades_t = xades_t->checkState() == Qt::Checked;

	XadesLevel level = getSelectedXadesLevel();

	output_file = new char[native_path.size() + 1];
	strcpy(output_file, getPlatformNativeString(native_path));

	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Save to file %s", output_file);
	if (individual_sigs)
		future = 
			QtConcurrent::run(this, &dlgSignature::run_multiple_sign, files_to_sign, n_files, output_file, level);
	else
		future = 
			QtConcurrent::run(this, &dlgSignature::runsign, files_to_sign, n_files, output_file, level);
	this->FutureWatcher.setFuture(future);

	pdialog->exec();

	if (this->error_code == 0)
		ShowSuccessMsgBox();
	else
		ShowErrorMsgBox();

	delete []cpychar;

	this->close();
}


void dlgSignature::runsign(const char ** paths, unsigned int n_paths, const char *output_path, XadesLevel level)
{
	bool keepTrying = true;
	PTEID_EIDCard* card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
	PTEID_ByteArray SignXades;
	do
	{
		try
		{

			if (level == XADES_T)
				SignXades = card->SignXadesT(output_path, paths, n_paths);
			else if (level == XADES_B)
				SignXades = card->SignXades(output_path, paths, n_paths);
			
			this->error_code = 0;
			keepTrying = false;

		}
		catch (PTEID_Exception &e)
		{
			this->error_code = e.GetError();
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Caught exception in SignXades*() method. Error code: 0x%08x\n", 
				(unsigned int)e.GetError());

			if (e.GetError() == EIDMW_ERR_CARD_RESET)
			{
				PTEID_EIDCard &new_card = getNewCard();
				card = &new_card;

			}
			else
				keepTrying = false;
		}
	}
	while(keepTrying);

	delete[] paths;
	delete output_path;
	return;
}

PTEID_EIDCard& dlgSignature::getNewCard()
{
		unsigned long	ReaderStartIdx = m_selected_reader;
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;

		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx; ReaderIdx++)
		{
			PTEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{

				return ReaderContext.getEIDCard();
			}
		}
}


void dlgSignature::run_multiple_sign(const char ** paths, unsigned int n_paths, const char *output_path, XadesLevel level)
{

	try
	{
		PTEID_EIDCard*	Card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);

		if (level == XADES_T)
			Card->SignXadesTIndividual(output_path, paths, n_paths);
		else if (level == XADES_B)
			Card->SignXadesIndividual(output_path, paths, n_paths);
		else if (level == XADES_A)
			Card->SignXadesAIndividual(output_path, paths, n_paths);
		this->error_code = 0;
	}
	catch (PTEID_Exception &e)
	{
		this->error_code = e.GetError();
	}
	delete [] paths;
	delete output_path;
	return;
}
