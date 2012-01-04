/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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

**************************************************************************** */

#include <QPixmap>
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>

#include "dlgprint.h"
#include "eidlib.h"

using namespace eIDMW;

dlgPrint::dlgPrint( QWidget* parent, CardInformation& CI_Data, GenPur::UI_LANGUAGE lng, QString const& cardTypeText) 
: QDialog(parent)
, m_CI_Data(CI_Data)
{	
	ui.setupUi(this);
	const QIcon Ico = QIcon( ":/images/Images/Icons/Print.png" );
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

	QTextBlockFormat textBlockFormat;
	textBlockFormat.setLeftMargin ( 100 );

	/*PQTextCursor cursor(ui.paperview->textCursor());
	QTextFrame*  topFrame = cursor.currentFrame();
	cursor.movePosition(QTextCursor::Start);*/

	/*if (on_pbPrint_clicked())
	{
		drawpdf(CI_Data);
	}*/
}

dlgPrint::~dlgPrint()
{
}

void dlgPrint::on_pbPrint_clicked( void )
{
	CardInformation cdata = m_CI_Data;
	QString pdffilepath;
	QString defaultfilepath;

	defaultfilepath = QDir::homePath();
	defaultfilepath.append("/CartaoCidadao.pdf");
	pdffilepath = QFileDialog::getSaveFileName(this, tr("Save File"), defaultfilepath, tr("Pdf Files (*.pdf)"));

	drawpdf(cdata, pdffilepath.toStdString().c_str());

	/*QPrinter	  printer;
 	QPrintDialog* dialog = new QPrintDialog(&printer, this);
 	dialog->setWindowTitle(tr("Print Document"));
	if (ui.paperview->textCursor().hasSelection())
	{
		dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
	}
	if (dialog->exec() != QDialog::Accepted)
	{
		return;
	}

	ui.paperview->print(&printer);
	*/
	done(0);
}

void dlgPrint::on_pbCancel_clicked( void )
{
	done(0);
}

void dlgPrint::on_chboxID_toggled( bool bChecked )
{

}

void dlgPrint::on_chboxAddress_toggled( bool bChecked )
{

}

void dlgPrint::on_chboxIDExtra_toggled( bool bChecked )
{

}

void dlgPrint::on_chboxPersoData_toggled( bool bChecked )
{

}

void dlgPrint::drawpdf(CardInformation& CI_Data, const char *filepath)
{
	cairo_surface_t *surface;
	cairo_t *cr;
	cairo_surface_t *imagefront;
	cairo_surface_t *imageback;
	int w, h;

	tFieldMap& PersonFields = CI_Data.m_PersonInfo.getFields();
	tFieldMap& CardFields = CI_Data.m_CardInfo.getFields();

	//// Create pdf with cairo
	surface = cairo_pdf_surface_create(filepath, 504, 648);

	cr = cairo_create(surface);

	//// Set Image - Front
	imagefront = cairo_image_surface_create_from_png("/home/metalgod/CaixaMagica/pteid-mw-pt/_src/eidmw/eidgui/Images/application_print_PDF.png");
	w = cairo_image_surface_get_width (imagefront);
	h = cairo_image_surface_get_height (imagefront);

	cairo_scale (cr, 510.0/w, 650.0/h);

	cairo_set_source_surface(cr, imagefront, 0, 0);
	cairo_paint(cr);


	//// Set Text Entries ////
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 12.0);
	cairo_set_source_rgb(cr, 0, 0, 0);

	//////////////////////////////ID FIELDS///////////////////////////
	if (ui.chboxID->isChecked())
	{
		////Surname
		cairo_move_to(cr, 20.0, 90.0);
		cairo_show_text(cr, PersonFields[NAME].toStdString().c_str());


		////Given Name
		cairo_move_to(cr, 20.0, 125.0);
		cairo_show_text(cr, PersonFields[GIVENNAME].toStdString().c_str());

		///Sex
		cairo_move_to(cr, 20.0, 165.0);
		cairo_show_text(cr, PersonFields[SEX].toStdString().c_str());

		///Height
		cairo_move_to(cr, 145.0, 170.0);
		cairo_show_text(cr, PersonFields[HEIGHT].toStdString().c_str());

		///Nacionality
		cairo_move_to(cr, 20.0, 200.0);
		cairo_show_text(cr, PersonFields[NATIONALITY].toStdString().c_str());

		////Date of Birth
		cairo_move_to(cr, 145.0, 200.0);
		cairo_show_text(cr, PersonFields[BIRTHDATE].toStdString().c_str());

		///Document nr
		cairo_move_to(cr, 20.0, 240.0);
		cairo_show_text(cr, PersonFields[DOCUMENTNUMBER].toStdString().c_str());

		///Expirity Date
		cairo_move_to(cr, 145.0, 240.0);
		cairo_show_text(cr, CardFields[CARD_VALIDUNTIL].toStdString().c_str());

		////Country
		cairo_move_to(cr, 290.0, 240.0);
		cairo_show_text(cr, PersonFields[COUNTRY].toStdString().c_str());

		////Father
		cairo_move_to(cr, 20.0, 270.0);
		cairo_show_text(cr, PersonFields[FATHER].toStdString().c_str());

		////Mother
		cairo_move_to(cr, 20.0, 295.0);
		cairo_show_text(cr, PersonFields[MOTHER].toStdString().c_str());
	}

	//////////////////////////////IDExtra FIELDS///////////////////////////
	if (ui.chboxIDExtra->isChecked())
	{
		////NIF
		cairo_move_to(cr, 20.0, 380.0);
		cairo_show_text(cr, PersonFields[TAXNO].toStdString().c_str());

		////NSS
		cairo_move_to(cr, 220.0, 380.0);
		cairo_show_text(cr, PersonFields[SOCIALSECURITYNO].toStdString().c_str());

		///NUS
		cairo_move_to(cr, 450.0, 380.0);
		cairo_show_text(cr, PersonFields[HEALTHNO].toStdString().c_str());

		///Card Version
		cairo_move_to(cr, 20.0, 420.0);
		cairo_show_text(cr, PersonFields[CARDVERSION].toStdString().c_str());

		///Issuing Date
		cairo_move_to(cr, 180.0, 420.0);
		cairo_show_text(cr, CardFields[CARD_VALIDFROM].toStdString().c_str());

		////Issuing Entity
		cairo_move_to(cr, 300.0, 420.0);
		cairo_show_text(cr, PersonFields[ISSUINGENTITY].toStdString().c_str());

		///Document type
		cairo_move_to(cr, 470.0, 420.0);
		cairo_show_text(cr, PersonFields[DOCUMENTTYPE].toStdString().c_str());

		///Card Status
		cairo_move_to(cr, 20.0, 460.0);
		cairo_show_text(cr, "???????");

		////Local of Request
		cairo_move_to(cr, 220.0, 460.0);
		cairo_show_text(cr, PersonFields[LOCALOFREQUEST].toStdString().c_str());
	}

	if (ui.chboxPersoData->isChecked())
	{

	}

	//Page 1
	cairo_show_page(cr);

	cairo_surface_destroy(surface);
	cairo_destroy(cr);
	done(0);
	return;
}


//-----------------------------------------------------------
// a button btnPDF can be inserted on this window.
// The PDF export is not very good though...
//-----------------------------------------------------------
/*
void dlgPrint::on_btnPDF_clicked( void )
{
	QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
	if (!fileName.isEmpty()) {
		if (QFileInfo(fileName).suffix().isEmpty())
		{
			fileName.append(".pdf");
		}
		QPrinter printer(QPrinter::HighResolution);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(fileName);
		ui.paperview->document()->print(&printer);
	} 
	done(0);
}
*/
