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

#include "dlgprint.h"
#include "eidlib.h"

using namespace eIDMW;
/*
dlgPrint::dlgPrint( QWidget* parent, CardInformation& CI_Data, GenPur::UI_LANGUAGE lng ) 
: QDialog(parent)
, m_CI_Data(CI_Data)
{	
	ui.setupUi(this);

	QTextBlockFormat textBlockFormat;
	textBlockFormat.setLeftMargin ( 100 );

	QTextCursor cursor(ui.paperview->textCursor());
	QTextFrame*  topFrame = cursor.currentFrame();
	cursor.movePosition(QTextCursor::Start);

	//-----------------------------------------------------------
	// Insert the seal-picture
	//-----------------------------------------------------------
	QTextImageFormat seal;
	QString sealName(":/images/Images/schild_");
	sealName += GenPur::getLanguage(lng);
	sealName += ".jpg";
	seal.setName(sealName);
	seal.setHeight( 200.0 );
	seal.setWidth( 200.0 );
	cursor.insertImage(seal); 

	//-----------------------------------------------------------
	// insert the picture from the eID card
	//-----------------------------------------------------------
	QImage img;
	img.loadFromData(CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData,"JPG");
	QTextDocument* textDocument = ui.paperview->document();
	textDocument->addResource(QTextDocument::ImageResource, QUrl("myimage"), img);
	cursor.insertImage("myimage"); 

	cursor.insertBlock(textBlockFormat);

	tFieldMap& cardInfo = CI_Data.m_CardInfo.getFields();

	//-----------------------------------------------------------
	// create some kind of header
	//-----------------------------------------------------------
	{
		QString htmlText;
		cursor.insertBlock();

		htmlText += "<html>";
		htmlText += "<head>";
		htmlText += "</head>";
		htmlText += "<body>";
		htmlText += "<table>";
		htmlText += "<tr><th colspan=\"2\">Portuguese</th></tr>";
		htmlText += "<tr><th colspan=\"2\">Identity card</th></tr>";
		htmlText += "<tr><td>card number</td><td>1234567</td></tr>";
		htmlText += "";
		htmlText += "";
		htmlText += "";
		htmlText += "</table>";
		htmlText += "";
		htmlText += "</body>";
		htmlText += "</html>";
		ui.paperview->setHtml(htmlText);
	}
}
*/

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

	QTextCursor cursor(ui.paperview->textCursor());
	QTextFrame*  topFrame = cursor.currentFrame();
	cursor.movePosition(QTextCursor::Start);

	if (PTEID_CARDTYPE_SIS == CI_Data.m_CardInfo.getType())
	{
		//-----------------------------------------------------------
		// Insert the SIS-logo
		//-----------------------------------------------------------
		QTextImageFormat seal;
		QString sealName(":/images/Images/Background_SisBack");
		sealName += ".jpg";
		seal.setName(sealName);
		seal.setHeight( 100.0 );
		seal.setWidth( (688.0/434.0)*100.0 );
		cursor.insertImage(seal); 

		cursor.insertBlock(textBlockFormat);

		tFieldMap& cardInfo = CI_Data.m_CardInfo.getFields();

		//-----------------------------------------------------------
		// create some kind of header
		// The header is in fact 2 tables of 1 column. These tables are
		// horizontally centered.
		//-----------------------------------------------------------
		QTextTableFormat headerTableFormat;
		headerTableFormat.setAlignment(Qt::AlignHCenter);
		headerTableFormat.setBorder(0);
		headerTableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
		cursor.insertBlock();
		{

			int nrRows = 1;
			int nrCols = 1;

			QTextTable* headerTable = cursor.insertTable(nrRows,nrCols,headerTableFormat);

			int row = 0;
			int col = 0;
			cursor = headerTable->cellAt(row, col).firstCursorPosition();
			QTextCharFormat txtFormat;
			QFont			font = ui.paperview->currentFont(); 
			font.setPointSize(14);
			txtFormat.setFont(font);

			cursor.insertText(tr("PORTUGUESE"),txtFormat);

			cursor.setPosition(topFrame->lastPosition());
			cursor.insertBlock();
			headerTable = cursor.insertTable(nrRows,nrCols,headerTableFormat);

			cursor = headerTable->cellAt(row, col).firstCursorPosition();
			cursor.insertText(tr("SIS CARD"),txtFormat);

		}
		cursor.setPosition(topFrame->lastPosition());
		cursor.insertBlock();
		cursor.insertBlock();
		{

			//-----------------------------------------------------------
			// Insert all necessary fields
			// It is created as a table with 2 columns
			//-----------------------------------------------------------

			QTextTableFormat tableFormat;
			tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);

			QVector<QTextLength> colWidths;
			colWidths.push_back(QTextLength(QTextLength::FixedLength,250));
			colWidths.push_back(QTextLength(QTextLength::FixedLength,250));
			tableFormat.setColumnWidthConstraints ( colWidths ) ;

			int nrRows = 14;
			int nrCols = 2;
			QTextTable* dataTable = cursor.insertTable(nrRows,nrCols);
			dataTable->setFormat(tableFormat);

			int row = 0;
			int col = 0;
			cursor = dataTable->cellAt(row, col++).firstCursorPosition();
			cursor.insertText(tr("Card number"));
			cursor = dataTable->cellAt(row++, col).firstCursorPosition();
			cursor.insertText(cardInfo[CARD_NUMBER]);

			col = 0;
			cursor = dataTable->cellAt(row, col++).firstCursorPosition();
			cursor.insertText(tr("Valid from"));
			cursor = dataTable->cellAt(row++, col).firstCursorPosition();
			cursor.insertText(cardInfo[CARD_VALIDFROM]);

			col = 0;
			cursor = dataTable->cellAt(row, col++).firstCursorPosition();
			cursor.insertText(tr("Valid until"));
			cursor = dataTable->cellAt(row++, col).firstCursorPosition();
			cursor.insertText(cardInfo[CARD_VALIDUNTIL]);

			tFieldMap& extraInfo = CI_Data.m_PersonInfo.m_PersonExtraInfo.getFields();

			col = 0;
			cursor = dataTable->cellAt(row, col++).firstCursorPosition();
			cursor.insertText(tr("National Number"));
			cursor = dataTable->cellAt(row++, col).firstCursorPosition();
			cursor.insertText(extraInfo[SOCIALSECURITYNUMBER]);

			tFieldMap& personInfo = CI_Data.m_PersonInfo.getFields();

			col = 0;
			cursor = dataTable->cellAt(row, col++).firstCursorPosition();
			cursor.insertText(tr("Name"));
			cursor = dataTable->cellAt(row++, col).firstCursorPosition();
			cursor.insertText(personInfo[NAME]);

			col = 0;
			cursor = dataTable->cellAt(row, col++).firstCursorPosition();
			cursor.insertText(tr("First Names"));
			cursor = dataTable->cellAt(row++, col).firstCursorPosition();
			cursor.insertText(personInfo[FIRSTNAME]);

			col = 0;
			cursor = dataTable->cellAt(row, col++).firstCursorPosition();
			cursor.insertText(tr("Sex"));
			cursor = dataTable->cellAt(row++, col).firstCursorPosition();
			if ("1" == personInfo[SEX])
			{
				cursor.insertText("M");
			}
			else
			{
				cursor.insertText("F");
			}

			col = 0;
			cursor = dataTable->cellAt(row, col++).firstCursorPosition();
			cursor.insertText(tr("Birth date"));
			cursor = dataTable->cellAt(row++, col).firstCursorPosition();
			cursor.insertText(personInfo[BIRTHDATE]);
		}
		return;
	}
	//-----------------------------------------------------------
	// Insert the seal-picture
	//-----------------------------------------------------------
	QTextImageFormat seal;
	QString sealName(":/images/Images/schild_");
	sealName += GenPur::getLanguage(lng);
	sealName += ".jpg";
	seal.setName(sealName);
	seal.setHeight( 200.0 );
	seal.setWidth( 200.0 );
	cursor.insertImage(seal); 

	//-----------------------------------------------------------
	// insert the picture from the eID card
	//-----------------------------------------------------------
	QImage img;
	img.loadFromData(CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData,"JPG");
	QTextDocument* textDocument = ui.paperview->document();
	textDocument->addResource(QTextDocument::ImageResource, QUrl("myimage"), img);
	cursor.insertImage("myimage"); 

	cursor.insertBlock(textBlockFormat);

	tFieldMap& cardInfo = CI_Data.m_CardInfo.getFields();

	//-----------------------------------------------------------
	// create some kind of header
	// The header is in fact 2 tables of 1 column. These tables are
	// horizontally centered.
	//-----------------------------------------------------------
	QTextTableFormat headerTableFormat;
	headerTableFormat.setAlignment(Qt::AlignHCenter);
	headerTableFormat.setBorder(0);
	headerTableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
	{
		cursor.insertBlock();

		int nrRows = 1;
		int nrCols = 1;

		QTextTable* headerTable = cursor.insertTable(nrRows,nrCols,headerTableFormat);

		int row = 0;
		int col = 0;
		cursor = headerTable->cellAt(row, col).firstCursorPosition();
		QTextCharFormat txtFormat;
		QFont			font = ui.paperview->currentFont(); 
		font.setPointSize(14);
		txtFormat.setFont(font);

		cursor.insertText(tr("PORTUGUESE"),txtFormat);

		cursor.setPosition(topFrame->lastPosition());
		cursor.insertBlock();
		headerTable = cursor.insertTable(nrRows,nrCols,headerTableFormat);

		if (PTEID_CARDTYPE_FOREIGNER == CI_Data.m_CardInfo.getType())
		{
			cursor = headerTable->cellAt(row, col).firstCursorPosition();
			cursor.insertText(cardTypeText,txtFormat);
			//cursor.insertText(tr("RESIDENCE PERMIT"),txtFormat);
		}
		else
		{
			cursor = headerTable->cellAt(row, col).firstCursorPosition();
			cursor.insertText(tr("CARTAO DE CIDADAO"),txtFormat);
		}
	}

	cursor.setPosition(topFrame->lastPosition());
	cursor.insertBlock();
	cursor.insertBlock();

	//-----------------------------------------------------------
	// Insert all necessary fields
	// It is created as a table with 2 columns
	//-----------------------------------------------------------

	QTextTableFormat tableFormat;
	tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);

	QVector<QTextLength> colWidths;
	colWidths.push_back(QTextLength(QTextLength::FixedLength,250));
	colWidths.push_back(QTextLength(QTextLength::FixedLength,250));
	tableFormat.setColumnWidthConstraints ( colWidths ) ;

	int nrRows = 14;
	int nrCols = 2;
	QTextTable* dataTable = cursor.insertTable(nrRows,nrCols);
	dataTable->setFormat(tableFormat);

	int row = 0;
	int col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Card number"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	QString formattedCardNr = CI_Data.m_CardInfo.formatCardNumber(cardInfo[CARD_NUMBER], CI_Data.m_CardInfo.getType());
	cursor.insertText(formattedCardNr);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Chip number"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(cardInfo[CHIP_NUMBER]);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Valid from"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(cardInfo[CARD_VALIDFROM]);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Valid until"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(cardInfo[CARD_VALIDUNTIL]);

	tFieldMap& personInfo = CI_Data.m_PersonInfo.getFields();

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("National Number"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	QString formattedNationalNr = CI_Data.m_PersonInfo.formatNationalNumber( personInfo[NATIONALNUMBER], CI_Data.m_CardInfo.getType());
	cursor.insertText(formattedNationalNr);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Title"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(personInfo[TITLE]);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Name"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(personInfo[NAME]);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("First Names"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(personInfo[FIRSTNAME]);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Nationality"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(personInfo[NATIONALITY]);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Sex"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(personInfo[SEX]);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Birth date"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(personInfo[BIRTHDATE]);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Birth place"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(personInfo[BIRTHPLACE]);

	tFieldMap& addressInfo = CI_Data.m_AddressInfo.getFields();

	QString address;
	address += addressInfo[ADDRESS_STREET];
	address += "\n";
	address += addressInfo[ADDRESS_ZIPCODE];
	address += " ";
	address += addressInfo[ADDRESS_CITY];

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Address"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(address);

	col = 0;
	cursor = dataTable->cellAt(row, col++).firstCursorPosition();
	cursor.insertText(tr("Issuing municipality"));
	cursor = dataTable->cellAt(row++, col).firstCursorPosition();
	cursor.insertText(cardInfo[CARD_PLACEOFISSUE]);
}

dlgPrint::~dlgPrint()
{	
}

void dlgPrint::on_pbPrint_clicked( void )
{
	QPrinter	  printer;
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

	done(0);
}

void dlgPrint::on_pbCancel_clicked( void )
{
	done(0);
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
