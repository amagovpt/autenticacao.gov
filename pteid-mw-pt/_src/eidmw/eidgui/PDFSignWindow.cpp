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
#include "mylistview.h"

using namespace eIDMW;

PDFSignWindow::PDFSignWindow( QWidget* parent, CardInformation& CI_Data)
	: m_CI_Data(CI_Data), m_selected_sector(0)
{

	ui.setupUi(this);
	//Set icon
	const QIcon Ico = QIcon(":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png");
	this->setWindowIcon(Ico);
	int i=0, j=0;

	ui.label_choose_sector->setText(tr(
	"<html>Choose the page sector where you <br> want your signature to appear."
	"<br>The grey sectors are already filled<br>with other signatures."
	"</html>"));
	m_pdf_sig = NULL;
	success = false;
	ui.spinBox_page->setValue(1);
	list_model = new QStringListModel();
	ui.pdf_listview->setModel(list_model);
	ui.pdf_listview->enableNotify();
	
	char conteudo = 0x31;

	for ( i = 0; i < 3; i++ ) 
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
//Event received from myListView
void PDFSignWindow::customEvent(QEvent *ev)
{
	if (ev->type() == QEvent::User)
		ui.button_sign->setEnabled(false);
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

void PDFSignWindow::ShowSuccessMsgBox()
{

	QString caption = tr("PDF Signature");
        QString msg = tr("Signature(s) succesfully generated");
	QMessageBox msgBoxp(QMessageBox::Information, caption, msg, 0, this);
	msgBoxp.exec();

}

void PDFSignWindow::ShowErrorMsgBox()
{

	QString caption  = tr("Error");
        QString msg = tr("Error Generating Signature!");
  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
  	msgBoxp.exec();
}


void PDFSignWindow::run_sign(int selected_page, QString &savefilepath,
	       	char *location, char *reason)
{

	PTEID_EIDCard* card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
	try
	{
		card->SignPDF(*m_pdf_sig, selected_page,
				m_selected_sector, location, reason, strdup(savefilepath.toUtf8().data()));
		this->success = true;

	}

	catch (PTEID_Exception &e)
	{
		this->success = false;
		fprintf(stderr, "Caught exception in some SDK method. Error code: 0x%08x\n", (unsigned int)e.GetError());
		//TODO: Show error message box
	}


}

void PDFSignWindow::on_button_sign_clicked()
{
	//For invisible sigs the implementation we'll
	//need to add a reference to the sigfield in some page so...
	int selected_page = 1;
	QString savefilepath;


	QStringListModel *model = dynamic_cast<QStringListModel *>(list_model);
	if (model->rowCount() == 0)
	{
	   return;

	}

	if (model->rowCount() > 1)
	{
		m_pdf_sig = new PTEID_PDFSignature();
		for (int i=0; i < model->rowCount(); i++)
		{
			QString tmp = model->data(model->index(i, 0), 0).toString();
			char *final = strdup(tmp.toUtf8().data());
			m_pdf_sig->addToBatchSigning(final);

		}

		savefilepath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				QDir::homePath(),
				QFileDialog::ShowDirsOnly);

	}
	else
		savefilepath = QFileDialog::getSaveFileName(this, tr("Save File"), 
			QDir::homePath()+"/signed.pdf", tr("PDF files (*.pdf)"));

	if (savefilepath.isNull() || savefilepath.isEmpty())
		return;

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


	//Single File Signature case
	pdialog = new QProgressDialog();
	pdialog->setWindowModality(Qt::WindowModal);
	pdialog->setWindowTitle(tr("PDF Signature"));
	pdialog->setLabelText(tr("Signing PDF file(s)..."));
	pdialog->setMinimum(0);
	pdialog->setMaximum(0);
	connect(&this->FutureWatcher, SIGNAL(finished()), pdialog, SLOT(cancel()));

	QFuture<void> future = QtConcurrent::run(this,
			&PDFSignWindow::run_sign, selected_page, savefilepath, location, reason );

	this->FutureWatcher.setFuture(future);
	pdialog->exec();

	if (this->success)
		ShowSuccessMsgBox();
	else
		ShowErrorMsgBox();

	//TODO: Fix this for Windows URLs or else just remove it
	//if (model->rowCount() == 1)
	//	QDesktopServices::openUrl(QString("file://")+ savefilepath);
	this->close();

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

void PDFSignWindow::on_radioButton_firstpage_toggled(bool value)
{
	if (value && m_pdf_sig)
	{
		clearAllSectors();
		QString sectors = QString::fromAscii(m_pdf_sig->getOccupiedSectors(1));
		highlightSectors(sectors);

	}

}

void PDFSignWindow::on_radioButton_lastpage_toggled(bool value)
{
	if (value && m_pdf_sig)
	{
		clearAllSectors();
		QString sectors =
			QString::fromAscii(m_pdf_sig->getOccupiedSectors(m_current_page_number));
		highlightSectors(sectors);
	}

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
				*row = 0; *column = 0;
				break;
			case 2:
				*row = 0; *column = 1;
				break;
			case 3:
				*row = 0; *column = 2;
				break;
			case 4:
				*row = 1; *column = 0;
				break;
			case 5:
				*row = 1; *column = 1;
				break;
			case 6:
				*row = 1; *column = 2;
				break;
			case 7:
				*row = 2; *column = 0;
				break;
			case 8:
				*row = 2; *column = 1;
				break;
			case 9:
				*row = 2; *column = 2;
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
	if (str.isEmpty())
		return;

	for(int i=0; i != str.size(); i++)
	{

		list_model->insertRows(list_model->rowCount(), 1);
		list_model->setData(list_model->index(list_model->rowCount()-1, 0), str.at(i));

	}

	current_input_path = str.at(0);

	m_pdf_sig = new PTEID_PDFSignature(current_input_path.toUtf8().data());
	m_current_page_number = m_pdf_sig->getPageCount();

	//Set the spinbox with the appropriate max value
	ui.spinBox_page->setMaximum(m_current_page_number);

	QString sectors = QString::fromAscii(m_pdf_sig->getOccupiedSectors(1));
	highlightSectors(sectors);

	if (list_model->rowCount() > 1)
	{
		clearAllSectors();
		//TODO
	}

	//Enable sign button now that we have data
	if (!str.isEmpty())
		ui.button_sign->setEnabled(true);

}
