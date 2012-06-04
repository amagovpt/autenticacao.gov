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
#include <QImage>
#include <QString>
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>

#include "dlgprint.h"
#include "eidlib.h"
#include "mainwnd.h"

using namespace eIDMW;

enum OutputFormat
{
	PDF,
	PNG
};

dlgPrint::dlgPrint( QWidget* parent, CardInformation& CI_Data, GenPur::UI_LANGUAGE lng, QString const& cardTypeText) 
: QDialog(parent)
, m_CI_Data(CI_Data)
, m_CurrReaderName("")
{	
    if (CI_Data.isDataLoaded())
    {
    	PTEID_EIDCard*	Card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
    	ui.setupUi(this);
		setFixedSize(398, 245);
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

        CI_Data.LoadData(*Card,m_CurrReaderName);
    }

}

dlgPrint::~dlgPrint()
{
}

void dlgPrint::on_pbGeneratePdf_clicked( void )
{
    CardInformation cdata = m_CI_Data;
    QString pdffilepath;
    QString defaultfilepath;

    defaultfilepath = QDir::homePath();
    try
    {
        if (ui.chboxSignature->isChecked())
        {
            QString pdffiletmp;
            QString signfilepath;
            QString outputsign;
            QString nativepdftmp;
            PTEID_EIDCard*	Card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
            PTEID_ByteArray SignXades;

            signfilepath = QDir::homePath();
            signfilepath.append("/CartaoCidadao.zip");
            outputsign = QFileDialog::getSaveFileName(this, tr("Save Signature File"), signfilepath, tr("Zip files 'XAdES' (*.zip)"));

            if (outputsign.isNull() || outputsign.isEmpty())
            	return;

            pdffiletmp = QDir::tempPath();
            pdffiletmp.append("/CartaoCidadao.pdf");

            nativepdftmp = QDir::toNativeSeparators(pdffiletmp);

            drawpdf(cdata, PDF ,nativepdftmp.toStdString().c_str());

            char *cpychar;
            const char **files_to_sign = new const char*[1];

            cpychar = new char[500];
#ifdef WIN32
            strcpy(cpychar, nativepdftmp.toStdString().c_str());
#else
            strcpy(cpychar, nativepdftmp.toUtf8().constData());
#endif
            files_to_sign[0] = cpychar;

            PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Pdf File to Sign: %s", files_to_sign[0]);

            SignXades = Card->SignXades(files_to_sign, 1, outputsign.toStdString().c_str());
        } else {
            QString nativepdfpath;

            defaultfilepath.append("/CartaoCidadao.pdf");
            pdffilepath = QFileDialog::getSaveFileName(this, tr("Save Pdf File"), defaultfilepath, tr("Pdf Files (*.pdf)"));

            if (pdffilepath.isNull() || pdffilepath.isEmpty())
            	return;

            QCoreApplication::processEvents();

            nativepdfpath = QDir::toNativeSeparators(pdffilepath);

            drawpdf(cdata, PDF ,nativepdfpath.toStdString().c_str());
        }
    }	catch (PTEID_Exception &e) {
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "GeneratePdf failed");
        QString msg(tr("General exception"));
    }

	this->close();

}



void dlgPrint::on_pbPrint_clicked( void )
{
	CardInformation cdata = m_CI_Data;
	imageList.clear();

	drawpdf(cdata, PNG , "");
	QPrinter printer;
	QPrintDialog *dlg = new QPrintDialog(&printer,0);
	if(dlg->exec() == QDialog::Accepted) {
		QListIterator<QImage> i(imageList);
		while (i.hasNext()){
			QPainter painter(&printer);
			painter.drawImage(QPoint(0,0), i.next());
			painter.end();
		}
	}

	delete dlg;
}

void dlgPrint::on_pbCancel_clicked( void )
{
	done(0);
}

void dlgPrint::on_chboxID_toggled( bool bChecked )
{
    if(ui.chboxID->isChecked() || ui.chboxAddress->isChecked() || ui.chboxIDExtra->isChecked() || ui.chboxPersoData->isChecked())
    {
        ui.pbGeneratePdf->setEnabled(true);
        ui.pbPrint->setEnabled(true);
    }
    else
    {
        ui.pbGeneratePdf->setEnabled(false);
        ui.pbPrint->setEnabled(false);
    }
}

void dlgPrint::on_chboxAddress_toggled( bool bChecked )
{
    if(ui.chboxID->isChecked() || ui.chboxAddress->isChecked() || ui.chboxIDExtra->isChecked() || ui.chboxPersoData->isChecked())
    {
        ui.pbGeneratePdf->setEnabled(true);
        ui.pbPrint->setEnabled(true);
    }
    else
    {
        ui.pbGeneratePdf->setEnabled(false);
        ui.pbPrint->setEnabled(false);
    }
}

void dlgPrint::on_chboxIDExtra_toggled( bool bChecked )
{
    if(ui.chboxID->isChecked() || ui.chboxAddress->isChecked() || ui.chboxIDExtra->isChecked() || ui.chboxPersoData->isChecked())
    {
        ui.pbGeneratePdf->setEnabled(true);
        ui.pbPrint->setEnabled(true);
    }
    else
    {
        ui.pbGeneratePdf->setEnabled(false);
        ui.pbPrint->setEnabled(false);
    }
}

void dlgPrint::on_chboxPersoData_toggled( bool bChecked )
{
    if(ui.chboxID->isChecked() || ui.chboxAddress->isChecked() || ui.chboxIDExtra->isChecked() || ui.chboxPersoData->isChecked())
    {
        ui.pbGeneratePdf->setEnabled(true);
        ui.pbPrint->setEnabled(true);
    }
    else
    {
        ui.pbGeneratePdf->setEnabled(false);
        ui.pbPrint->setEnabled(false);
    }
}

void dlgPrint::on_chboxSignature_toggled( bool bChecked )
{
    if(ui.chboxID->isChecked() || ui.chboxAddress->isChecked() || ui.chboxIDExtra->isChecked() || ui.chboxPersoData->isChecked())
    {
        ui.pbGeneratePdf->setEnabled(true);
        ui.pbPrint->setEnabled(true);
    } else
    {
        ui.pbGeneratePdf->setEnabled(false);
        ui.pbPrint->setEnabled(false);
    }
}

const char * dlgPrint::persodata_triggered()
{
	try
	{
		PTEID_EIDCard*	Card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);

		return Card->readPersonalNotes();

	}	catch (PTEID_Exception &e) {
		QString msg(tr("General exception"));
		return NULL;
	}
}

bool dlgPrint::addressPINRequest_triggered(CardInformation& CI_Data)
{
	try
	{
		QString caption(tr("Identity Card: PIN verification"));

		PTEID_EIDCard*	Card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
		PTEID_Pin&		Pin	= Card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);

		unsigned long triesLeft = -1;
		bool		  bResult   = Pin.verifyPin("",triesLeft);

		QString msg = bResult ? tr("PIN verification passed"):tr("PIN verification failed");
		
		if (!bResult)
		{
			QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
			return false;
		}

		CI_Data.LoadDataAddress(*Card, m_CurrReaderName);
	}
	catch (PTEID_Exception &e)
	{
		QString msg(tr("General exception"));
		return false;
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		return false;
	}
	return true;
}


double lineSize(cairo_t *ct, const QString &str){
	cairo_text_extents_t extents;

	cairo_text_extents(ct,str.toUtf8(),&extents);
	return extents.x_advance;
}

void formatLines(cairo_t *ct, const QString &str, QStringList &qSList){
	QString strTemp;
	QString oldStrTemp;
	QStringList lstTemp = str.split(QRegExp("\\s"));
	int i=0;

	if (lstTemp.size()>1){
		while (i<lstTemp.size()){
			while (lineSize(ct,strTemp)<552 && i<lstTemp.size()){
				oldStrTemp = strTemp;
				strTemp+=lstTemp.at(i++)+" ";
			}
			if (!oldStrTemp.isEmpty()){
				qSList.append(oldStrTemp);
				oldStrTemp.clear();
				if (i<lstTemp.size())
					strTemp = lstTemp.at(--i);
			} else {
				QString temp = strTemp;
				while(!temp.isEmpty()){
					int j=0;
					while((temp.size()>=46+j) && lineSize(ct,temp.left(46+j))<552){
						j++;
					}
					qSList.append(temp.left(46+(j-1)));
					if (temp.size()>=46+j){
						temp = temp.right(temp.size()-(46+(j-1)));
					} else
						temp.clear();
				}
			}
		}
	} else {
		QString temp = str;
		while(!temp.isEmpty()){
			int j=0;
			while((temp.size()>=46+j) && lineSize(ct,temp.left(46+j))<552)
				j++;
			qSList.append(temp.left(46+(j-1)));
			if (temp.size()>=46+j){
				temp = temp.right(temp.size()-(46+(j-1)));
			} else
				temp.clear();
		}
	}
}


void formatNotes(QString &personalNotes, cairo_t *ct, QStringList &qSList){
	personalNotes.replace("\t", "    ");
	QStringList lines = personalNotes.split("\n");

	for (int i = 0; i < lines.size(); ++i){
		if (lineSize(ct, lines.at(i)) < 552)
			qSList.append(lines.at(i));
		else
			formatLines(ct, lines.at(i), qSList);
	}
}

cairo_t *dlgPrint::createPage(int format, bool firstPage, const char *filepath, cairo_t *crt){
	cairo_surface_t *surface;
	cairo_surface_t *imagefront;
	int w, h;
	cairo_t *cr;

	if (firstPage)
		background = new QImage (":/images/Images/application_print_PDF.png");
	else
		background = new QImage (":/images/Images/application_print_PDF_pg_2.png");

	if (format == PDF && firstPage)
			surface = cairo_pdf_surface_create(filepath, 504, 648);
		else if (format == PNG)
			surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 504, 648);

	if ((format == PDF && firstPage) || format == PNG)
		cr = cairo_create(surface);
	else
		cr = crt;

	imagefront = cairo_image_surface_create_for_data(background->bits(), CAIRO_FORMAT_RGB24, background->width(), background->height(), background->bytesPerLine());
	w = cairo_image_surface_get_width (imagefront);
	h = cairo_image_surface_get_height (imagefront);

	if ((format == PDF && firstPage) || format == PNG)
		cairo_scale (cr, 510.0/w, 650.0/h);

	cairo_set_source_surface(cr, imagefront, 0, 0);
	cairo_paint(cr);

	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 12.0);
	cairo_set_source_rgb(cr, 0, 0, 0);

	return cr;
}


static cairo_status_t write_png_stream_to_Qimage (void *in_closure, const unsigned char *data,
                                                unsigned int length)
{
	QByteArray *img = (QByteArray*) in_closure;

	img->append((const char*)data, length);

    return CAIRO_STATUS_SUCCESS; //CAIRO_STATUS_WRITE_ERROR
}


void dlgPrint::drawpdf(CardInformation& CI_Data, int format, const char *filepath)
{
	cairo_t *cr;
	cairo_surface_t *idphoto;
	int w, h;

	cr = createPage(format, true, filepath, NULL);

	tFieldMap& PersonFields = CI_Data.m_PersonInfo.getFields();
	tFieldMap& CardFields = CI_Data.m_CardInfo.getFields();

	//////////////////////////////ID FIELDS///////////////////////////
	if (ui.chboxID->isChecked())
	{
		////Surname
		cairo_move_to(cr, 18.0, 90.0);
		cairo_show_text(cr, PersonFields[NAME].toStdString().c_str());


		////Given Name
		cairo_move_to(cr, 18.0, 128.0);
		cairo_show_text(cr, PersonFields[GIVENNAME].toStdString().c_str());

		///Sex
		cairo_move_to(cr, 18.0, 170.0);
		cairo_show_text(cr, PersonFields[SEX].toStdString().c_str());

		///Height
		cairo_move_to(cr, 145.0, 170.0);
		cairo_show_text(cr, PersonFields[HEIGHT].toStdString().c_str());

		///Nacionality
		cairo_move_to(cr, 18.0, 204.0);
		cairo_show_text(cr, PersonFields[NATIONALITY].toStdString().c_str());

		////Date of Birth
		cairo_move_to(cr, 145.0, 204.0);
		cairo_show_text(cr, PersonFields[BIRTHDATE].toStdString().c_str());

		///Document nr
		cairo_move_to(cr, 18.0, 238.0);
		cairo_show_text(cr, PersonFields[DOCUMENTNUMBER].toStdString().c_str());

		///Expirity Date
		cairo_move_to(cr, 145.0, 238.0);
		cairo_show_text(cr, CardFields[CARD_VALIDUNTIL].toStdString().c_str());

		////Country
		cairo_move_to(cr, 290.0, 238.0);
		cairo_show_text(cr, PersonFields[COUNTRY].toStdString().c_str());

		////Father
		cairo_move_to(cr, 18.0, 273.0);
		cairo_show_text(cr, PersonFields[FATHER].toStdString().c_str());

		////Mother
		cairo_move_to(cr, 18.0, 295.0);
		cairo_show_text(cr, PersonFields[MOTHER].toStdString().c_str());

		////Accidental Indications
		cairo_move_to(cr, 20.0, 335.0);
		cairo_show_text(cr, PersonFields[ACCIDENTALINDICATIONS].toStdString().c_str());

	}

	//////////////////////////////IDExtra FIELDS///////////////////////////
	if (ui.chboxIDExtra->isChecked())
	{
		////NIF
		cairo_move_to(cr, 18.0, 380.0);
		cairo_show_text(cr, PersonFields[TAXNO].toStdString().c_str());

		////NSS
		cairo_move_to(cr, 220.0, 380.0);
		cairo_show_text(cr, PersonFields[SOCIALSECURITYNO].toStdString().c_str());

		///NUS
		cairo_move_to(cr, 440.0, 380.0);
		cairo_show_text(cr, PersonFields[HEALTHNO].toStdString().c_str());

		///Card Version
		cairo_move_to(cr, 18.0, 418.0);
		cairo_show_text(cr, PersonFields[CARDVERSION].toStdString().c_str());

		///Issuing Date
		cairo_move_to(cr, 170.0, 418.0);
		cairo_show_text(cr, CardFields[CARD_VALIDFROM].toStdString().c_str());

		////Issuing Entity
		cairo_move_to(cr, 296.0, 418.0);
		cairo_show_text(cr, PersonFields[ISSUINGENTITY].toStdString().c_str());

		///Document type
		cairo_move_to(cr, 440.0, 418.0);
		cairo_show_text(cr, PersonFields[DOCUMENTTYPE].toStdString().c_str());

		///Card Status
		cairo_move_to(cr, 18.0, 463.0);
		cairo_show_text(cr, PersonFields[VALIDATION].toStdString().c_str());

		////Local of Request
		cairo_move_to(cr, 296.0, 463.0);
		cairo_show_text(cr, PersonFields[LOCALOFREQUEST].toStdString().c_str());
	}

	//////////////////////////////Address FIELDS///////////////////////////
	if (ui.chboxAddress->isChecked())
	{
		addressPINRequest_triggered(CI_Data);
		tFieldMap& AddressFields = CI_Data.m_AddressInfo.getFields();

		////ADDRESS District
		cairo_move_to(cr, 18.0, 511.0);
		cairo_show_text(cr, AddressFields[ADDRESS_DISTRICT].toStdString().c_str());

		////ADDRESS Municipality
		cairo_move_to(cr, 18.0, 550.0);
		cairo_show_text(cr, AddressFields[ADDRESS_MUNICIPALITY].toStdString().c_str());

		////ADDRESS Civil Parish
		cairo_move_to(cr, 296.0, 550.0);
		cairo_show_text(cr, AddressFields[ADDRESS_CIVILPARISH].toStdString().c_str());



		////ADDRESS ABBrStreetType
		cairo_move_to(cr, 18.0, 590.0);
		cairo_show_text(cr, AddressFields[ADDRESS_ABBRSTREETTYPE].toStdString().c_str());

		////ADDRESS Street Type
		cairo_move_to(cr, 185.0, 590.0);
		cairo_show_text(cr, AddressFields[ADDRESS_STREETTYPE].toStdString().c_str());

		////ADDRESS Street Name
		cairo_move_to(cr, 340.0, 590.0);
		cairo_show_text(cr, AddressFields[ADDRESS_STREETNAME].toStdString().c_str());

		////ADDRESS Abbr Building Type
		cairo_move_to(cr, 18.0, 630.0);
		cairo_show_text(cr, AddressFields[ADDRESS_ABBRBUILDINGTYPE].toStdString().c_str());

		////ADDRESS Building Type
		cairo_move_to(cr, 230.0, 630.0);
		cairo_show_text(cr, AddressFields[ADDRESS_BUILDINGTYPE].toStdString().c_str());

		////ADDRESS Door No
		cairo_move_to(cr, 18.0, 672.0);
		cairo_show_text(cr, AddressFields[ADDRESS_DOORNO].toStdString().c_str());

		////ADDRESS Floor
		cairo_move_to(cr, 170.0, 672.0);
		cairo_show_text(cr, AddressFields[ADDRESS_FLOOR].toStdString().c_str());

		////ADDRESS Side
		cairo_move_to(cr, 298.0, 672.0);
		cairo_show_text(cr, AddressFields[ADDRESS_SIDE].toStdString().c_str());

		////ADDRESS Place
		cairo_move_to(cr, 430.0, 672.0);
		cairo_show_text(cr, AddressFields[ADDRESS_PLACE].toStdString().c_str());

		////ADDRESS Zip4
		cairo_move_to(cr, 18.0, 713.0);
		cairo_show_text(cr, AddressFields[ADDRESS_ZIP4].toStdString().c_str());

		////ADDRESS Zip3
		cairo_move_to(cr, 93.0, 713.0);
		cairo_show_text(cr, AddressFields[ADDRESS_ZIP3].toStdString().c_str());


		////ADDRESS Postal Locality
		cairo_move_to(cr, 170.0, 713.0);
		cairo_show_text(cr, AddressFields[ADDRESS_POSTALLOCALITY].toStdString().c_str());

		////ADDRESS Locality
		cairo_move_to(cr, 430.0, 713.0);
		cairo_show_text(cr, AddressFields[ADDRESS_LOCALITY].toStdString().c_str());
	}

	if (ui.chboxID->isChecked())
	{
		cairo_save(cr);

		//Image
		img = QImage();
		img.loadFromData(m_CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData);

		idphoto = cairo_image_surface_create_for_data(img.bits(), CAIRO_FORMAT_RGB24, img.width(),
				img.height(), img.bytesPerLine());

		int w2 = cairo_image_surface_get_width (idphoto);
		int h2 = cairo_image_surface_get_height (idphoto);

		cairo_scale (cr, 160.0/w2, 220.0/h2);

		cairo_set_source_surface(cr, idphoto, 1050, 180);
		cairo_paint(cr);

		cairo_restore(cr);
	}

	if (ui.chboxPersoData->isChecked())
	{
		const char *notes = persodata_triggered();
		QString perso_data = QString::fromUtf8(notes);

		// cairo_show_text() doesn't render linebreaks, at this time pango wasnt an alternative (strategies)
		QStringList notesFormt;
		formatNotes(perso_data,cr,notesFormt);
		if (!notesFormt.isEmpty()){
			int i=0;
			for (; i < notesFormt.size() && i < 5; ++i)
			{
				cairo_move_to(cr, 20.0, 760 + 15*i);
				cairo_show_text(cr, notesFormt.at(i).toUtf8());
			}

			if (i<notesFormt.size()){
				int firstLineYpos;
				for (; i<notesFormt.size(); i++){
					if ((i-5)%50 == 0){
						firstLineYpos = 90;
						if (format == PDF){
							cairo_show_page(cr);
						} else {
							cairo_surface_write_to_png_stream (cairo_get_target(cr), write_png_stream_to_Qimage,&image);
							imageList.append(QImage::fromData(image));
							image.clear();
							cairo_surface_destroy(cairo_get_target(cr));
							cairo_destroy(cr);
							delete background;
						}
						cr = createPage(format, false, NULL, cr);
					}
					cairo_move_to(cr, 20.0, firstLineYpos + 15*((i-5)%50));
					cairo_show_text(cr, notesFormt.at(i).toUtf8());
				}
			}
		}
	}

	if (format == PDF)
	{
		cairo_show_page(cr);
	} else {
		cairo_surface_write_to_png_stream (cairo_get_target(cr), write_png_stream_to_Qimage,&image);
		imageList.append(QImage::fromData(image));
		image.clear();
	}

	cairo_surface_destroy(cairo_get_target(cr));
	cairo_destroy(cr);
	delete background;
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
