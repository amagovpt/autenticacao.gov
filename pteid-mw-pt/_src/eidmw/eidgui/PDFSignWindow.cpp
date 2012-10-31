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
	
	int cell_number = 1;	

	for (i = 0; i < table_lines; i++) 
	{
		for (j = 0; j < table_columns; j++)
		{
			QTableWidgetItem * it = new QTableWidgetItem(
					QString::number(cell_number++));
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
	m_selected_sector = 1 + table_columns*row + column;

	ui.label_selectedsector->setText(tr("Selected sector: ")+
			QString::number(m_selected_sector));

}

void PDFSignWindow::on_button_cancel_clicked()
{
	this->close();

}


/* For filenames we need to maintain latin-1 or UTF-8 native encoding */
//This macro's argument is a QString
#ifdef _WIN32
#define getPlatformNativeString(s) s.toStdString().c_str()
#else
#define getPlatformNativeString(s) s.toUtf8().constData()
#endif


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

void PDFSignWindow::ShowErrorMsgBox(QString &msg)
{

	QString caption  = tr("Error");
  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
  	msgBoxp.exec();
}

void PDFSignWindow::ShowSectorErrorMessage()
{

	QString caption  = tr("Error");
        QString msg = tr("The selected sector is already filled!\nPlease choose another one.");
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
				m_selected_sector, location, reason, strdup(getPlatformNativeString(savefilepath)));
		this->success = true;

	}

	catch (PTEID_Exception &e)
	{
		this->success = false;
		fprintf(stderr, "Caught exception in some SDK method. Error code: 0x%08x\n", 
			(unsigned int)e.GetError());
	}


}

void PDFSignWindow::on_button_sign_clicked()
{
	//For invisible sigs the implementation we'll
	//need to add a reference to the sigfield in some page so...
	int selected_page = 1;
	QString savefilepath;


	//Read Page
	if (ui.visible_checkBox->isChecked())
	{
		if (ui.radioButton_firstpage->isChecked())
			selected_page = 1;
		else if (ui.radioButton_lastpage->isChecked())
			selected_page = ui.spinBox_page->maximum();
		else
			selected_page = ui.spinBox_page->value();

		if (!validateSelectedSector())
		{
		   ShowSectorErrorMessage();
		   return;

		}

	}

	QStringListModel *model = dynamic_cast<QStringListModel *>(list_model);
	if (model->rowCount() == 0)
	{
	   return;

	}
	QFileInfo my_file_info(current_input_path);
	QString defaultsavefilepath = my_file_info.dir().absolutePath();

	if (model->rowCount() > 1)
	{
		m_pdf_sig = new PTEID_PDFSignature();
		for (int i = 0; i < model->rowCount(); i++)
		{
			QString tmp = model->data(model->index(i, 0), 0).toString();
			char *final = strdup(getPlatformNativeString(tmp));
			m_pdf_sig->addToBatchSigning(final);

		}

		savefilepath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				QDir::toNativeSeparators(defaultsavefilepath),
				QFileDialog::ShowDirsOnly);

	}
	else
	{
		QString basename = my_file_info.completeBaseName();
		savefilepath = QFileDialog::getSaveFileName(this, tr("Save File"), 
			QDir::toNativeSeparators(defaultsavefilepath+"/"+basename+"_signed.pdf"), tr("PDF files (*.pdf)"));

	}

	if (savefilepath.isNull() || savefilepath.isEmpty())
		return;

	char *reason = NULL, *location = NULL;
	if (ui.location_textbox->isEnabled() && ui.location_textbox->text().size() > 0) {
		location = strdup(ui.location_textbox->text().toUtf8().data());

	}
	if (ui.reason_textbox->isEnabled() && ui.reason_textbox->text().size() > 0) {
		reason = strdup(ui.reason_textbox->text().toUtf8().data());
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
		ShowErrorMsgBox(tr("Error Generating Signature!"));

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

	int rem = sector % 3;

	*row = (sector-1) / 3;

	if (rem == 0)
		*column = 2;
	else if (rem == 1)
		*column = 0;
	else if (rem == 2)
		*column = 1;

}

void PDFSignWindow::clearAllSectors()
{

	for(int i = 0 ;i < table_lines; i++ ) 
	{
		for (int j = 0; j < table_columns; j++)
		{
		ui.tableWidget->item(i, j)->setBackground(m_default_background);
		}
	}

}


bool PDFSignWindow::validateSelectedSector()
{
	int row, col;
	mapSectorToRC(m_selected_sector, &row, &col);

	return ui.tableWidget->item(row, col)->background() !=
			     QBrush(Qt::darkGray);
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

	current_input_path = str.at(0);

	m_pdf_sig = new PTEID_PDFSignature(strdup(getPlatformNativeString(current_input_path)));

	try
	{
		m_current_page_number = m_pdf_sig->getPageCount();
	}
	catch (PTEID_Exception &e)
	{
		ShowErrorMsgBox(tr("Unsupported or corrupted file!"));
		m_pdf_sig = NULL;
		return;
	}

	for(int i=0; i != str.size(); i++)
	{

		list_model->insertRows(list_model->rowCount(), 1);
		list_model->setData(list_model->index(list_model->rowCount()-1, 0), str.at(i));

	}

	//Set the spinbox with the appropriate max value
	ui.spinBox_page->setMaximum(m_current_page_number);

	QString sectors = QString::fromAscii(m_pdf_sig->getOccupiedSectors(1));
	highlightSectors(sectors);

	if (list_model->rowCount() > 1)
	{
		clearAllSectors();
	}

	//Enable sign button now that we have data
	if (!str.isEmpty())
		ui.button_sign->setEnabled(true);

}
