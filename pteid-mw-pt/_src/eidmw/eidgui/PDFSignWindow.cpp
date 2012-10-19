/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2012 Caixa Mágica Software.
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
 * Authors: André Guerreiro <andre.guerreiro@caixamagica.pt>	
 *
 **************************************************************************** */
#include <QListView>
#include <QComboBox>
#include <QFileDialog>
#include <QProgressDialog>

#include <eidlib.h>
#include "PDFSignWindow.h"

using namespace eIDMW;

PDFSignWindow::PDFSignWindow( QWidget* parent, CardInformation& CI_Data)
	: m_CI_Data(CI_Data), m_selected_sector(0)
{

	ui.setupUi(this);
	//Set icon
	const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
	this->setWindowIcon( Ico );
	int i=0, j=0;
	
	ui.label_choose_sector->setText(tr(
	"<html>Choose the page sector where you <br> want your signature to appear."
	"<br>The positioning mechanism is optimized<br>for pages of A4 size in portrait<br> orientation."
	"</html>"));
	m_pdf_sig = NULL;
	ui.spinBox_page->setValue(1);
	
	char conteudo = 0x31;

	for( i = 0 ;i < 3; i++ ) 
	{
		for ( j = 0; j < 3; j++)
		{
			QTableWidgetItem * it = new QTableWidgetItem(QString(conteudo++));
			ui.tableWidget->setItem(i, j, it); 
		}
	}

	//save the default background to use in clearAllSectors()
	m_default_background = ui.tableWidget->item(0,0)->background();
	this->setFixedSize(this->width(), this->height());

}


PDFSignWindow::~PDFSignWindow()
{


}

void PDFSignWindow::on_tableWidget_currentCellChanged(int row, int column, 
		int prev_row, int prev_column)
{

	update_sector(row,column);

}

void PDFSignWindow::update_sector(int row, int column)
{
	if (row == 0)
		m_selected_sector = 1+column;
	else if (row == 1)
		m_selected_sector = 4+column;

	else if (row == 2)
		m_selected_sector = 7+column;

	ui.label_selectedsector->setText(tr("Selected sector: ")+
			QString::number(m_selected_sector));

}

void PDFSignWindow::on_button_cancel_clicked()
{
	this->close();

}


void PDFSignWindow::on_checkBox_reason_toggled(bool checked)
{
ui.reason_textbox->setEnabled(checked);

}


void PDFSignWindow::on_checkBox_location_toggled(bool checked)
{
ui.location_textbox->setEnabled(checked);

}

void PDFSignWindow::on_radioButton_choosepage_toggled(bool checked)
{
	ui.spinBox_page->setEnabled(checked);
}


void PDFSignWindow::on_visible_checkBox_toggled(bool checked)
{
	ui.radioButton_firstpage->setEnabled(checked);
	ui.radioButton_lastpage->setEnabled(checked);
	ui.radioButton_choosepage->setEnabled(checked);

	bool choose_b = ui.radioButton_choosepage->isChecked();

	if(checked)
		ui.spinBox_page->setEnabled(choose_b);
	else
		ui.spinBox_page->setEnabled(false);

	ui.label_page->setEnabled(checked);
	ui.label_choose_sector->setEnabled(checked);
	ui.label_selectedsector->setEnabled(checked);

	ui.tableWidget->setEnabled(checked);
	//Set sensible defaults for sector
	ui.tableWidget->setCurrentCell (0, 0);
	m_selected_sector = 1;

}

void PDFSignWindow::on_button_sign_clicked()
{
	//For invisible sigs the implementation we'll
	//need to add a reference to the sigfield in some page so...
	int selected_page = 1;
	PTEID_EIDCard*  card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);

	QString savefilepath = QFileDialog::getSaveFileName(this, tr("Save File"), 
			QDir::homePath()+"/signed.pdf", tr("PDF files (*.pdf)"));

	char *reason = NULL, *location = NULL;
	if (ui.location_textbox->isEnabled() && ui.location_textbox->text().size() > 0) {
		location = strdup(ui.location_textbox->text().toUtf8().data());

	}
	if (ui.reason_textbox->isEnabled() && ui.reason_textbox->text().size() > 0) {
		reason = strdup(ui.reason_textbox->text().toUtf8().data());
	}

	//Read Page
	if (ui.visible_checkBox->isChecked())
	{
		if (ui.radioButton_firstpage->isChecked())
			selected_page = 1;
		else if (ui.radioButton_lastpage->isChecked())
			selected_page = ui.spinBox_page->maximum();
		else
			selected_page = ui.spinBox_page->value();

	}

	try
	{
		/*
		//Single File Signature case
		//TODO: Batch signing
		pdialog = new QProgressDialog();
		pdialog->setWindowModality(Qt::WindowModal);
		pdialog->setWindowTitle(tr(" PDF Sign"));
		pdialog->setLabelText(tr("Signing PDF file..."));
		pdialog->setMinimum(0);
		pdialog->setMaximum(0);
		connect(&this->FutureWatcher, SIGNAL(finished()), pdialog, SLOT(cancel()));
		*/
		card->SignPDF(*m_pdf_sig, selected_page,
				m_selected_sector, location, reason, savefilepath.toUtf8().data());
	}
	catch (PTEID_Exception &e)
	{
		fprintf(stderr, "Caught exception in some SDK method. Error code: 0x%08x\n", (unsigned int)e.GetError());
		return;
		//TODO: Show error message box
	}
	
	//TODO: Fix this for Windows URLs
	QDesktopServices::openUrl(QString("file://")+ current_input_path);


}

void PDFSignWindow::on_button_addfile_clicked()
{

	QStringList fileselect;
	QString defaultopenfilepath;

	defaultopenfilepath = QDir::homePath();
	fileselect = QFileDialog::getOpenFileNames(this, tr("Select File(s)"),
			defaultopenfilepath, "PDF Documents (*.pdf)");
	QCoreApplication::processEvents();
	
	addFileToListView(fileselect);


}

void PDFSignWindow::on_spinBox_page_valueChanged(int new_value)
{
	//Check if there is at least 1 PDF chosen
	if (!m_pdf_sig)
		return;
	clearAllSectors();

	QString sectors = QString::fromAscii(m_pdf_sig->getOccupiedSectors(new_value));
	highlightSectors(sectors);

}

void mapSectorToRC(int sector, int *row, int *column)
{
		switch(sector)
		{
			case 1:
				*row = 0 ; *column = 0 ;
				break;
			case 2:
				*row = 0 ; *column = 1 ;
				break;
			case 3:
				*row = 0 ; *column =2 ;
				break;
			case 4:
				*row = 1; *column =0 ;
				break;
			case 5:
				*row = 1; *column =1 ;
				break;
			case 6:
				*row = 1; *column = 2;
				break;
			case 7:
				*row = 2 ; *column = 0 ;
				break;
			case 8:
				*row = 2; *column = 1;
				break;
			case 9:
				*row = 2; *column = 2 ;
				break;
			default:
				fprintf(stderr, "Invalid Sector: %d\n", sector);
		}

}

void PDFSignWindow::clearAllSectors()
{

	for(int i = 0 ;i < 3; i++ ) 
	{
		for (int j = 0; j < 3; j++)
		{
		ui.tableWidget->item(i, j)->setBackground(m_default_background);
		}
	}

}


//TODO ListView: delete

void PDFSignWindow::highlightSectors(QString &csv_sectors)
{
	if (csv_sectors.length()==0)
		return;

	QStringList sl = csv_sectors.split(",");
	QTableWidgetItem * it = NULL;
	QStringListIterator sectors_it(sl);

	int row = -1, column = -1;
	while (sectors_it.hasNext())
	{
		uint s = sectors_it.next().toUInt();

		mapSectorToRC(s, &row, &column);

		it = ui.tableWidget->item(row, column);
		if (!it)
		{
			fprintf(stderr, "Couldnt find table item!!\n");
			continue;
		}
		it->setBackground(QBrush(Qt::darkGray));

	}

}

void PDFSignWindow::addFileToListView(QStringList &str)
{

	QStringListModel* localModel = new QStringListModel();
	aList.append(str);

	aList.removeDuplicates();

	localModel->setStringList(aList);

	current_input_path = aList.at(0);

	m_pdf_sig = new PTEID_PDFSignature(current_input_path.toUtf8().data());
	ui.pdf_listview->setModel(localModel);

	//Set the spinbox with the appropriate value
	ui.spinBox_page->setMaximum(m_pdf_sig->getPageCount());

	QString sectors = QString::fromAscii(m_pdf_sig->getOccupiedSectors(1));
	highlightSectors(sectors);

	if (aList.size() > 1)
	{
		clearAllSectors();
		//TODO
	}

	//Enable sign button now that we have data
	if (!aList.isEmpty())
		ui.button_sign->setEnabled(true);

}
