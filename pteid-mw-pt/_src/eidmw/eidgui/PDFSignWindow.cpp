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


#include "PDFSignWindow.h"

using namespace eIDMW;

PDFSignWindow::PDFSignWindow( QWidget* parent, CardInformation& CI_Data)
	: m_CI_Data(CI_Data), m_selected_sector(0)
{

	ui.setupUi(this);
	//Set icon
	const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
	this->setWindowIcon( Ico );
	int i=1,j=1;
	
	ui.label_choose_sector->setText(tr(
	"<html>Choose the page sector<br> where you want your<br> signature to appear:</html>"));
	m_pdf_sig = NULL;
	ui.spinBox_page->setValue(1);

	for( ;i<=3 && j <= 3; ) 
	{
		QTableWidgetItem * it = new QTableWidgetItem(QString("   "));
		ui.tableWidget->setItem(i, j, it); 
		i++; j++;
	}
	//ui.tableWidget->resizeColumnsToContents();
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
void PDFSignWindow::on_visible_checkBox_toggled(bool checked)
{
	ui.label_page->setEnabled(checked);
	ui.label_choose_sector->setEnabled(checked);
	ui.label_selectedsector->setEnabled(checked);
	ui.spinBox_page->setEnabled(checked);
	ui.tableWidget->setEnabled(checked);
	//Set sensible defaults for sector
	ui.tableWidget->setCurrentCell (0, 0);
	m_selected_sector = 1;

}

void PDFSignWindow::on_button_sign_clicked()
{
	try
	{
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
		
		//Single File Signature case
		//TODO: Batch signing
		card->SignPDF(*m_pdf_sig, ui.spinBox_page->isEnabled() ? ui.spinBox_page->value(): 0,
			m_selected_sector, location, reason, savefilepath.toUtf8().data());


	}
	catch (PTEID_Exception &e)
	{
		fprintf(stderr, "Caught exception in some SDK method. Error code: 0x%08x\n", (unsigned int)e.GetError());
	}


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

void PDFSignWindow::addFileToListView(QStringList &str)
{

	QStringListModel* localModel = new QStringListModel();
	aList.append(str);

	aList.removeDuplicates();

	localModel->setStringList(aList);

	m_pdf_sig = new PTEID_PDFSignature(aList.at(0).toUtf8().data());
	ui.pdf_listview->setModel(localModel);

	//Set the spinbox with the appropriate value
	ui.spinBox_page->setMaximum(m_pdf_sig->getPageCount());

	if (aList.size() > 1)
	{
		//Replace the spinbox with a combo box with first/last page options
		ui.horizontalLayout->removeWidget(ui.spinBox_page);
		//This is needed AFAICT to make the widget disappear
		ui.spinBox_page->setGeometry(0, 0, 0, 0);
		QComboBox *combo_pages = new QComboBox();
		combo_pages->addItem("First Page");
		combo_pages->addItem("Last Page");
		ui.horizontalLayout->addWidget(combo_pages);
	}

	//Enable sign button now that we have data
	if (!aList.isEmpty())
		ui.button_sign->setEnabled(true);

}
