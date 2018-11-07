/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2011-2014 Caixa Magica Software
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
#include <QString>
#include <QPrinter>
#include <QPainter>
#include <QStyleOption>
#include <QPen>
#include <QFontDatabase>
#include <QPrintDialog>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QtConcurrent>

#include "dlgprint.h"
#include "eidlib.h"
#include "mainwnd.h"
#include "dlgUtils.h"

using namespace eIDMW;


void CenterParent(QWidget* parent, QWidget* child)
{
    QPoint centerparent(
            parent->x() + ((parent->frameGeometry().width() - child->frameGeometry().width()) /2),
            parent->y() + ((parent->frameGeometry().height() - child->frameGeometry().height()) /2));

    QDesktopWidget * pDesktop = QApplication::desktop();
    QRect sgRect = pDesktop->screenGeometry(pDesktop->screenNumber(parent));
    QRect childFrame = child->frameGeometry();

    if(centerparent.x() < sgRect.left())
        centerparent.setX(sgRect.left());
    else if((centerparent.x() + childFrame.width()) > sgRect.right())
        centerparent.setX(sgRect.right() - childFrame.width());

    if(centerparent.y() < sgRect.top())
        centerparent.setY(sgRect.top());
    else if((centerparent.y() + childFrame.height()) > sgRect.bottom())
        centerparent.setY(sgRect.bottom() - childFrame.height());

    child->move(centerparent);
}


dlgPrint::dlgPrint( QWidget* parent, CardInformation& CI_Data, GenPur::UI_LANGUAGE lng)
: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
, m_CI_Data(CI_Data)
, m_CurrReaderName(CI_Data.m_cardReader)
{
    if (CI_Data.isDataLoaded())
    {
    	PTEID_EIDCard*	Card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
    	ui.setupUi(this);
		setFixedSize(420, 245);

		const QIcon Ico = QIcon( ":/images/Images/Icons/Print.png" );
		this->setWindowIcon( Ico );

		QDesktopWidget* desktop = QApplication::desktop();
		int screenNr = desktop->screenNumber();
		QRect rect = desktop->availableGeometry(screenNr);
		int height = rect.height();

		int thiswidth = this->width();
		int thisheight = this->height();
		this->sections_to_print = 0;

		if (thisheight > height)
		{
			this->resize(thiswidth,height-20); //make sure the window fits
		}

        pdialog = new QProgressDialog();
#ifdef _WIN32
        pdialog->setWindowTitle(tr("Export PDF / Print"));
#else
        pdialog->setWindowFlags(Qt::Popup);
        pdialog->setWindowModality(Qt::WindowModal);
        // We need to reset geometry because in Linux/Xorg
        // the progress popup would be positioned at (0, 0)
        CenterParent(this, pdialog);
#endif

        pdialog->setCancelButton(NULL);
        pdialog->setMinimum(0);
        pdialog->setMaximum(0);
        connect(&this->FutureWatcher, SIGNAL(finished()), pdialog, SLOT(cancel()));
        pdialog->reset();

        CI_Data.LoadData(*Card,m_CurrReaderName);
    }

}

dlgPrint::~dlgPrint()
{
}

char *QStringToCString(QString &string)
{

    char * cpychar = new char[string.length()*2];
    strcpy(cpychar, getPlatformNativeString(string));
	return cpychar;
}

bool SignPDF_wrapper(PTEID_EIDCard * card, const char * file_to_sign, QString &outputsign)
{
	PTEID_PDFSignature pdf_sig_handler(file_to_sign);
	char * output_path = strdup(getPlatformNativeString(outputsign));
	try
	{

		card->SignPDF(pdf_sig_handler, 0, 0, false, "", "", output_path);
	}
	catch(...)
	{
		free(output_path);
        free((char *)file_to_sign);
		return false;
	}
	free(output_path);
    free((char *)file_to_sign);
	return true;
}

void dlgPrint::on_pbGeneratePdf_clicked( void )
{
    CardInformation cdata = m_CI_Data;
    QString pdffilepath;
    QString caption  = tr("Export / Print");
    QString defaultfilepath;

    QPrinter pdf_printer;
    bool res = false;

    defaultfilepath = QDir::homePath();
    try
    {
        PTEID_EIDCard*  card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
        PTEID_Pin&      addressPIN = card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);
        PTEID_Pin&      signaturePIN = card->getPins().getPinByPinRef(PTEID_Pin::SIGN_PIN);

        if (ui.chboxAddress->isChecked() && addressPIN.getTriesLeft() == 0)
        {
            QMessageBox msgBoxp(QMessageBox::Critical, caption, tr("Error in PDF export: Address PIN is blocked"), 0, this);
            msgBoxp.exec();
            return;
        }
        
        if (ui.chboxSignature->isChecked())
		{
            QString pdffiletmp;
            QString signfilepath;
            QString output_path;
            QString nativepdftmp;

            if (signaturePIN.getTriesLeft() == 0)
            {
                QMessageBox msgBoxp(QMessageBox::Critical, caption, tr("Error in PDF export: Signature PIN is blocked"), 0, this);
                msgBoxp.exec();
                return;
            }

            signfilepath = QDir::homePath();
            signfilepath.append("/CartaoCidadao_signed.pdf");
            output_path = QFileDialog::getSaveFileName(this, tr("Save signed PDF file"), signfilepath, tr("PDF Files (*.pdf)"));

            if (output_path.isNull() || output_path.isEmpty())
            	return;

            pdffiletmp = QDir::tempPath();
            pdffiletmp.append("/CartaoCidadao.pdf");

            nativepdftmp = QDir::toNativeSeparators(pdffiletmp);

			res = drawpdf(cdata, pdf_printer, nativepdftmp);

            if (!res)
            {
            	return;
            }

			QFuture<bool> new_thread = QtConcurrent::run(SignPDF_wrapper, card, QStringToCString(nativepdftmp), output_path);
		    this->FutureWatcher.setFuture(new_thread);

		    pdialog->exec();
		    res = new_thread.result();
			
        }
        else
		{
            QString nativepdfpath;

            defaultfilepath.append("/CartaoCidadao.pdf");
            pdffilepath = QFileDialog::getSaveFileName(this, tr("Save PDF File"), defaultfilepath, tr("PDF Files (*.pdf)"));

            if (pdffilepath.isNull() || pdffilepath.isEmpty())
            	return;

            QCoreApplication::processEvents();

            nativepdfpath = QDir::toNativeSeparators(pdffilepath);

			res = drawpdf(cdata, pdf_printer, nativepdfpath);

        }
    }
	catch (PTEID_Exception &e) {
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "GeneratePDF failed");
    }

    if (res)
	    ShowSuccessMsgBox();
    else
	    ShowErrorMsgBox(Operation::PDF);
    this->close();

}



void dlgPrint::on_pbPrint_clicked()
{
    CardInformation cardData = m_CI_Data;
	QPrinter printer;

    if (ui.chboxAddress->isChecked())
    {
        bool res = addressPINRequest_triggered(cardData);
        if (!res)
        {
            ShowErrorMsgBox(Operation::PRINT);
            return;
        }
    }

	QPrintDialog dlg(&printer, this);
	
    if(dlg.exec() == QDialog::Accepted) {

        bool res = drawpdf(cardData, printer, "");
        if (!res)
        {
            ShowErrorMsgBox(Operation::PRINT);
            return;
        }
        
        this->close();
	}

}

void dlgPrint::on_pbCancel_clicked()
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

//The argument needs to be a PTEID_Pin * because
// this class has no copy construtor which is needed for the QtConcurrent operation
bool verifyPin_wrapper(PTEID_Pin *pin)
{

	unsigned long triesLeft = -1;
	const char * csPin = "";
	bool res = false;
	try
	{
		res = pin->verifyPin( csPin, triesLeft, true);
	}
	catch(PTEID_Exception &e)
	{
		res = false;
	}

	return res;
}

void dlgPrint::ShowSuccessMsgBox()
{

	QString caption  = tr("Export / Print");
	QString msg = tr("PDF file successfully generated");
	QMessageBox *msgBoxp = new QMessageBox(QMessageBox::Information, caption, msg, 0, this);
	msgBoxp->exec();
	delete msgBoxp;
}

void dlgPrint::ShowErrorMsgBox(Operation op)
{

	QString caption  = tr("Export / Print");
    QString msg = op == Operation::PRINT ? tr("Printing was canceled!"): tr("Error Generating PDF File!");
  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
  	msgBoxp.exec();
}

bool dlgPrint::addressPINRequest_triggered(CardInformation& CI_Data)
{
	try
	{

		PTEID_EIDCard*	Card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
		PTEID_Pin&		Pin	= Card->getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);

		QFuture<bool> new_thread = QtConcurrent::run(verifyPin_wrapper, &Pin);
		this->FutureWatcher.setFuture(new_thread);

		pdialog->exec();
		bool bResult = new_thread.result();

		if (!bResult)
		{
			return false;
		}

		CI_Data.LoadDataAddress(*Card, m_CurrReaderName);
	}
	catch (PTEID_Exception &e)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
}


static QPen black_pen;
static QPen blue_pen;


void drawSingleField(QPainter &painter, double pos_x, double pos_y, QString name, QString value, bool single_column=false)
{

    int line_length = single_column ? 300 : 180;

    painter.setPen(blue_pen);
    painter.drawText(QPointF(pos_x, pos_y), name);
    pos_y += 7;

    painter.drawLine(QPointF(pos_x, pos_y), QPointF(pos_x+line_length, pos_y));
    pos_y += 15;
    painter.setPen(black_pen);
    painter.drawText(QPointF(pos_x, pos_y), value);
}

void drawSectionHeader(QPainter &painter, double pos_x, double pos_y, QString section_name)
{
    QFont header_font("DIN Light");
    header_font.setPointSize(11);
    QFont regular_font("DIN Medium");
    regular_font.setPointSize(12);

    painter.setFont(header_font);

    QColor light_grey(233, 233, 233);
    painter.setBrush(light_grey);
    painter.setPen(light_grey);

    painter.drawRoundedRect(QRectF(pos_x, pos_y, 250, 30), 10.0, 10.0);
    painter.setPen(black_pen);

    painter.drawText(pos_x+20, pos_y+20, section_name);

    painter.setFont(regular_font);
}

QPixmap loadHeader()
{
    return QPixmap(":/images/Images/pdf_document_header.png");
}

QString getUtf8String(const QString &in)
{
	return QString::fromUtf8(in.toStdString().c_str());
}

QString dlgPrint::getTranslated(const QString &str)
{
	return tr(str.toUtf8().constData());
}

void addFonts()
{
#ifdef __APPLE__
    QString resource_dir = QCoreApplication::applicationDirPath() + "/../Resources/";
    QFontDatabase::addApplicationFont(resource_dir + "din-light.ttf");
    QFontDatabase::addApplicationFont(resource_dir + "din-medium.ttf");
#else
    QFontDatabase::addApplicationFont(":/images/Images/din-light.ttf");
    QFontDatabase::addApplicationFont(":/images/Images/din-medium.ttf");
#endif
}


bool dlgPrint::drawpdf(CardInformation& CI_Data, QPrinter &printer, QString filepath)
{

	const tFieldMap PersonFields = CI_Data.m_PersonInfo.getFields();
	tFieldMap& CardFields = CI_Data.m_CardInfo.getFields();

	//QPrinter printer;
	printer.setResolution(96);
	printer.setColorMode(QPrinter::Color);
    printer.setPaperSize(QPrinter::A4);

    //if (strlen(filepath) > 0)
	if ( filepath.size() > 0)
    	printer.setOutputFileName(filepath);

    //Add custom fonts
	addFonts();

    //Start drawing
    pos_x = 0, pos_y = 0;
    QPainter painter(&printer);

    //Font setup
    QFont din_font("DIN Medium");
    din_font.setPixelSize(18);

//  Include header as png pixmap
    QPixmap header = loadHeader();
    painter.drawPixmap(QPointF(pos_x, pos_y), header);

//  //Alternative using the QtSVG module, not enabled for now because the rendering is far from perfect
	//QSvgRenderer renderer(QString("C:\\Users\\agrr\\Desktop\\GMC_logo.svg"));
    //std::cout << renderer.defaultSize().width() << "x" << renderer.defaultSize().height() << std::endl;

	//renderer.render(&painter, QRect(pos_x, pos_y, 504, 132));
	//renderer.render(&painter, QRect(pos_x, pos_y, 250, 120));

	pos_y += header.height() + 15;
	black_pen = painter.pen();

	blue_pen.setColor(QColor(78, 138, 190));
    painter.setPen(blue_pen);

	int line_length = 487;
	//Horizontal separator below the CC logo
	painter.drawLine(QPointF(pos_x, pos_y), QPointF(pos_x+line_length, pos_y));
	pos_y += 25;

    //Change text color
    blue_pen = painter.pen();

    const int COLUMN_WIDTH = 220;
    const int LINE_HEIGHT = 45;

//    din_font.setBold(true);
    painter.setFont(din_font);

    painter.drawText(QPointF(pos_x, pos_y), tr("PERSONAL DATA"));

    pos_y += 15;
    int circle_radius = 6;

    //Draw 4 blue circles
    painter.setBrush(QColor(78, 138, 190));
    painter.drawEllipse(QPointF(pos_x+10, pos_y), circle_radius, circle_radius);
    painter.drawEllipse(QPointF(pos_x+40, pos_y), circle_radius, circle_radius);
    painter.drawEllipse(QPointF(pos_x+70, pos_y), circle_radius, circle_radius);
    painter.drawEllipse(QPointF(pos_x+100, pos_y), circle_radius, circle_radius);

    painter.setPen(black_pen);
    //Reset font
    din_font.setPixelSize(10);
    din_font.setBold(false);
    painter.setFont(din_font);

    pos_y += 30;

    if (ui.chboxID->isChecked())
	{

   		drawSectionHeader(painter, pos_x, pos_y, tr("BASIC INFORMATION"));
   		sections_to_print++;

    	//Image
		QPixmap pixmap_photo;
		pixmap_photo.loadFromData(m_CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData);

		const int img_height = 200;

        //Scale height if needed
    	QPixmap scaled = pixmap_photo.scaledToHeight(img_height, Qt::SmoothTransformation);

    	painter.drawPixmap(QPointF(pos_x + 500, pos_y-80), scaled);

    	pos_y += 50;

	    drawSingleField(painter, pos_x, pos_y, tr("Given Name(s)"), getUtf8String(PersonFields[GIVENNAME]), true);

	    pos_y += LINE_HEIGHT;

	    drawSingleField(painter, pos_x, pos_y, tr("Surname(s)"), getUtf8String(PersonFields[NAME]), true);

	    pos_y += LINE_HEIGHT;

	    drawSingleField(painter, pos_x, pos_y, tr("Gender"), PersonFields[SEX]);
	    drawSingleField(painter, pos_x + COLUMN_WIDTH, pos_y, tr("Height"), PersonFields[HEIGHT]);
		drawSingleField(painter, pos_x + COLUMN_WIDTH*2, pos_y, tr("Date of birth"), PersonFields[BIRTHDATE]);

	    pos_y += LINE_HEIGHT;

	    drawSingleField(painter, pos_x, pos_y, tr("Document Number"), PersonFields[DOCUMENTNUMBER]);
	    drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Validity Date"), CardFields[CARD_VALIDUNTIL]);
	    drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Country"), PersonFields[COUNTRY]);

	    pos_y += LINE_HEIGHT;
	    drawSingleField(painter, pos_x, pos_y, tr("Father"), getUtf8String(PersonFields[FATHER]), true);
	    pos_y += LINE_HEIGHT;

	    drawSingleField(painter, pos_x, pos_y, tr("Mother"), getUtf8String(PersonFields[MOTHER]), true);
	    pos_y += LINE_HEIGHT;

	    drawSingleField(painter, pos_x, pos_y, tr("Notes"), getUtf8String(PersonFields[ACCIDENTALINDICATIONS]), true);

	    pos_y += 50;
	}

    if (ui.chboxIDExtra->isChecked())
	{
		drawSectionHeader(painter, pos_x, pos_y, tr("ADDITIONAL INFORMATION"));
		sections_to_print++;
	    pos_y += 50;

	    drawSingleField(painter, pos_x, pos_y, tr("VAT identification no."), PersonFields[TAXNO]);
	    drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Social Security no."), PersonFields[SOCIALSECURITYNO]);
		drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("National Health System no."), PersonFields[HEALTHNO]);
	    pos_y += LINE_HEIGHT;

	    drawSingleField(painter, pos_x, pos_y, tr("Card Version"), PersonFields[CARDVERSION]);
		drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Delivery Date"), CardFields[CARD_VALIDFROM]);
	    drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Delivery Entity"), getUtf8String(PersonFields[ISSUINGENTITY]));
	    pos_y += LINE_HEIGHT;

        /* LL - Change columns order - No overlap text */
	    drawSingleField(painter, pos_x, pos_y, tr("Card State"), getTranslated(PersonFields[VALIDATION]));
	    drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Document type"), getUtf8String(PersonFields[DOCUMENTTYPE]));
	    pos_y += LINE_HEIGHT;

        drawSingleField(painter, pos_x, pos_y, tr("Delivery Location"), getUtf8String(PersonFields[LOCALOFREQUEST]));

	    pos_y += 50;
	}

	if (ui.chboxAddress->isChecked())
	{
        if (filepath.size() > 0)
        {
    		bool res = addressPINRequest_triggered(CI_Data);
    		if (!res)
    		{

                //Delete partial PDF file
                painter.end();
                bool ret = QFile::remove(QString(filepath));
                if (!ret)
                {
                    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Failed to delete partial PDF file: %s", filepath.toStdString().c_str());
                }

                return false;
            }
        }

		tFieldMap& AddressFields = CI_Data.m_AddressInfo.getFields();

		drawSectionHeader(painter, pos_x, pos_y, tr("ADDRESS"));
		sections_to_print++;

    	pos_y += 50;

        if (CI_Data.m_AddressInfo.isForeign())
        {
            /* Foreign Address*/
            drawSingleField(painter, pos_x, pos_y, tr("Country"), getUtf8String(AddressFields[FOREIGN_COUNTRY]));
            drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Region"),  getUtf8String(AddressFields[FOREIGN_REGION]));
            drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("City"),  getUtf8String(AddressFields[FOREIGN_CITY]));

            pos_y += LINE_HEIGHT;

            drawSingleField(painter, pos_x, pos_y, tr("Locality"), getUtf8String(AddressFields[FOREIGN_LOCALITY]));
            drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Zip Code"),  getUtf8String(AddressFields[FOREIGN_POSTALCODE]));

            pos_y += LINE_HEIGHT;

            drawSingleField(painter, pos_x, pos_y, tr("Address"), getUtf8String(AddressFields[FOREIGN_ADDRESS]));
        }
        else
        {

        	drawSingleField(painter, pos_x, pos_y, tr("District"), getUtf8String(AddressFields[ADDRESS_DISTRICT]));
        	drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Municipality"), getUtf8String(AddressFields[ADDRESS_MUNICIPALITY]));
        	drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Civil Parish"), getUtf8String(AddressFields[ADDRESS_CIVILPARISH]));

     	    pos_y += LINE_HEIGHT;

        	drawSingleField(painter, pos_x, pos_y, tr("Ab. street type"), AddressFields[ADDRESS_ABBRSTREETTYPE]);
        	drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Street type"), AddressFields[ADDRESS_STREETTYPE]);
        	drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Street Name"), getUtf8String(AddressFields[ADDRESS_STREETNAME]));

    	    pos_y += LINE_HEIGHT;

    	    drawSingleField(painter, pos_x, pos_y, tr("Ab. Building Type"), AddressFields[ADDRESS_ABBRBUILDINGTYPE]);
    	    drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Building Type"), getUtf8String(AddressFields[ADDRESS_BUILDINGTYPE]));
    	    drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("House/building no."), getUtf8String(AddressFields[ADDRESS_DOORNO]));
    	    pos_y += LINE_HEIGHT;

    	    drawSingleField(painter, pos_x, pos_y, tr("Floor"), getUtf8String(AddressFields[ADDRESS_FLOOR]));
    	    drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Side"), AddressFields[ADDRESS_SIDE]);
    	    drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Place"), getUtf8String(AddressFields[ADDRESS_PLACE]));
    	    pos_y += LINE_HEIGHT;

    	    drawSingleField(painter, pos_x, pos_y, tr("Zip Code 4"), AddressFields[ADDRESS_ZIP4]);
    	    drawSingleField(painter, pos_x+COLUMN_WIDTH, pos_y, tr("Zip Code 3"), AddressFields[ADDRESS_ZIP3]);
    	    drawSingleField(painter, pos_x+COLUMN_WIDTH*2, pos_y, tr("Postal Locality"), getUtf8String(AddressFields[ADDRESS_POSTALLOCALITY]));

    	    pos_y += LINE_HEIGHT;

    	    drawSingleField(painter, pos_x, pos_y, tr("Locality"), getUtf8String(AddressFields[ADDRESS_LOCALITY]));
        }

	    pos_y += 80;
	}

	if (ui.chboxPersoData->isChecked())
	{
        QString perso_data;

        const char *notes = persodata_triggered();
        perso_data = QString::fromUtf8(notes);

        if (perso_data.size() > 0)
        {
            //Force a page-break before PersoData only if we're drawing all the sections
            if (sections_to_print == 3)
            {
            	printer.newPage();
                pos_y = 0;
            }
            pos_x = 0;


            drawSectionHeader(painter, pos_x, pos_y, tr("PERSONAL NOTES"));

            pos_y += 75;
            painter.drawText(QRectF(pos_x, pos_y, 700, 700), Qt::TextWordWrap, perso_data);
        }
	}

	//Finish drawing/printing
	painter.end();
	return true;
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
