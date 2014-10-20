/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2012-2014 Caixa Mágica Software.
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
 * Author: André Guerreiro <andre.guerreiro@caixamagica.pt>	
 *
 **************************************************************************** */
#include <QListView>
#include <QComboBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QTextStream>
#include <QPainter>
#include <QStyleOption>
#include <QPen>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QFileInfo>

#include <eidlib.h>
#include "PDFSignWindow.h"
//#include "FreeSelectionDialog.h"
#include "mylistview.h"

using namespace eIDMW;

PDFSignWindow::PDFSignWindow( QWidget* parent, CardInformation& CI_Data)
	: m_CI_Data(CI_Data), m_selected_sector(0), card_present(true), my_scene(NULL)
{

	ui.setupUi(this);
	//Set icon
	const QIcon Ico = QIcon(":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png");
	this->setWindowIcon(Ico);
	int i=0, j=0;

/*
	ui.label_choose_sector->setText(tr(
	"Choose the page sector where you want your signature to appear.\n"
	"The grey sectors are already filled with other signatures."));
	ui.label_choose_sector->setWordWrap(true);
	*/

	//DEBUG
	//ui.label_selectedsector->setWordWrap(true);	

	m_pdf_sig = NULL;
	//m_selection_dialog = NULL;
	table_lines = 6;
	table_columns = 3;
	sig_coord_x = -1, sig_coord_y = -1;
	success = SIG_ERROR;
	horizontal_page_flag = false;
	m_small_signature = false;
	ui.spinBox_page->setValue(1);
	list_model = new QStringListModel();
	ui.pdf_listview->setModel(list_model);
	ui.pdf_listview->enableNotify();

	int items = ui.horizontalLayout_3->count();

	for (int i = 0; i!= items; i++)
	{
		ui.horizontalLayout_3->itemAt(i)->setAlignment(Qt::AlignLeft);
	}

	ui.verticalLayout->setContentsMargins(15,15,15,15);

	//save the default background to use in clearAllSectors()
	// m_default_background = ui.tableWidget->item(0,0)->background();
	// ui.label_selectedimg->setPixmap(QPixmap( ":/images/Images/backgrounds/signature_image_default.png"));
	image_canvas = new ImageCanvas(ui.tab3);
	image_canvas->setCitizenName(composeCitizenFullName());
	image_canvas->setCitizenNIC(getCitizenNIC());

	image_canvas->setFixedSize(420, 200);
	ui.verticalLayout_4->addWidget(image_canvas);
	// image_canvas->setLayout(ui.verticalLayout);
	//image_canvas->show();

	items = ui.verticalLayout->count();

	for (int i = 0; i!= items; i++)
	{
		ui.verticalLayout->itemAt(i)->setAlignment(Qt::AlignTop);
	}

	ui.verticalLayout1->setSpacing(5);	

	ui.verticalLayout_2->setSpacing(10);

	items = ui.verticalLayout_4->count();

	for (int i = 0; i!= items; i++)
	{
		ui.verticalLayout_4->itemAt(i)->setAlignment(Qt::AlignTop);
	}

	items = ui.verticalLayout1->count();

	for (int i = 0; i!= items; i++)
	{
		ui.verticalLayout1->itemAt(i)->setAlignment(Qt::AlignTop);

	}

	this->setFixedSize(this->width(), this->height());	
}


QString PDFSignWindow::composeCitizenFullName()
{
	const tFieldMap PersonFields = m_CI_Data.m_PersonInfo.getFields();

	 return QString::fromUtf8(PersonFields[GIVENNAME].toStdString().c_str()) + " " +
	     QString::fromUtf8(PersonFields[NAME].toStdString().c_str());
		 
}

QString PDFSignWindow::getCitizenNIC()
{	
	const tFieldMap PersonFields = m_CI_Data.m_PersonInfo.getFields();
	return QString::fromUtf8(PersonFields[DOCUMENTNUMBER].toStdString().c_str());
}

#define ROWCOUNT_LANDSCAPE 5
#define COLUMNCOUNT_LANDSCAPE 4
#define ROWCOUNT_PORTRAIT 6
#define COLUMNCOUNT_PORTRAIT 3

PDFSignWindow::~PDFSignWindow()
{

	delete list_model;
	// delete m_selection_dialog;
	// if (m_pdf_sig)
	// 	delete m_pdf_sig;
}



//Event received from myListView
void PDFSignWindow::customEvent(QEvent *ev)
{
	if (ev->type() == QEvent::User)
		ui.button_sign->setEnabled(false);
}

void PDFSignWindow::enableSignButton()
{
	this->card_present = true;
}

void PDFSignWindow::disableSignButton()
{

	ui.button_sign->setEnabled(false);
	this->card_present = false;
}


QImage ImageCanvas::drawToImage()
{
	// const int spacing = 10;
	const int img_width = 185;
	const int img_height = 42;
	
	QImage output_img(img_width, img_height, QImage::Format_RGB32);

	//We need to fill the white background because the user-selected image may be too narrow for the predefined width
	output_img.fill(Qt::white);

	QPainter painter(&output_img);

	//QPixmap pixmap2(":/images/Images/Icons/pteid_signature_small.png");
	//QPixmap pixmap2_scaled = pixmap2.scaledToHeight(img_height, Qt::SmoothTransformation);

	//Scale height if needed
	QPixmap scaled = user_pixmap.height() > img_height ? user_pixmap.scaledToHeight(img_height,
	 	Qt::SmoothTransformation) :
			user_pixmap;

	//Scale width if needed
	// if (scaled.width() > img_width)
	// {
	// 	scaled = scaled.scaledToWidth(img_width, Qt::SmoothTransformation);
	// } 
    
   	painter.drawPixmap(0, 0, scaled);

   	return output_img;
}

void ImageCanvas::paintEvent(QPaintEvent *)
{
	const int spacing = 10;
	const int line_spacing = 5;
	int line_count = 0;
	int current_y_pos = 5;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QPixmap pixmap2(":/images/Images/Icons/pteid_signature_watermark.jpg");
	QPixmap default_icon(":/images/Images/Icons/pteid_signature_banner.jpg");

	QFont current_font = painter.font();
	current_font.setPointSize(9);
	painter.setFont(current_font);
	
	int line_height = fontMetrics().height();
	current_y_pos = line_height;

	QRectF border_rectangle(0.0, 0.0, this->width()-40, this->height()-5);
 	QPen my_pen = painter.pen();
 	my_pen.setWidth(2);
 	painter.setPen(my_pen);
 	painter.setBrush(Qt::white);
 	painter.drawRoundedRect(border_rectangle, 20.0, 15.0);

 	if (!small_sig_format && !sig_reason.isEmpty())
	{
		QPen new_pen = painter.pen();
		QPen orig_pen = painter.pen();
		// new_pen.setItalic(true);
		new_pen.setColor(QColor(78, 138, 190));
		painter.setPen(new_pen);
		current_y_pos += line_spacing;
		painter.drawText(5, current_y_pos,  sig_reason);
		painter.setPen(orig_pen);
	}
 	
 	current_y_pos += line_height + line_spacing;
 	const QString SIGNED_BY = "Assinado por: ";

 	QRect bounding_rect = painter.boundingRect(5, current_y_pos, 420, 100, Qt::AlignLeft | Qt::AlignTop, SIGNED_BY);
 	//Draw background watermark aligned with SIGNED_BY line
 	painter.drawPixmap(5, current_y_pos-fontMetrics().xHeight(), pixmap2);
	painter.drawText(5, current_y_pos, SIGNED_BY);

	QFont my_font = painter.font();
	my_font.setBold(true);
	painter.setFont(my_font);
	painter.drawText(bounding_rect.width()+ 5, current_y_pos, citizen_name);
	my_font.setBold(false);
	painter.setFont(my_font);

	current_y_pos += line_height + line_spacing;
	painter.drawText(5, current_y_pos, QString::fromUtf8("Num de Identificação Civil: ")+ citizen_nic);

	
	current_y_pos += line_height + line_spacing;
	painter.drawText(5, current_y_pos, "Date: 06 de Outubro de 2014 11:20:22");

	if (!small_sig_format && !sig_location.isEmpty())
	{
		current_y_pos += line_height + line_spacing;
		painter.drawText(5, current_y_pos, "Location: " + sig_location);
	}

	
	current_y_pos += line_spacing * 2;

	if (pixmap2.isNull())
	{
        qDebug() << "pixmap2 not loaded! ";
        return;
    }

    //Dont draw the second image if the signature is small format
    if (small_sig_format)
    {
    	return;
    }

	if (!user_pixmap.isNull())
	{
		QPixmap scaled = user_pixmap.height() > default_icon.height() ? user_pixmap.scaledToHeight(default_icon.height(), Qt::SmoothTransformation):
		   user_pixmap;
    	painter.drawPixmap(5, current_y_pos, scaled);

    }
    else
    {
    	
    	painter.drawPixmap(5, current_y_pos, default_icon);
    }
    
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

	// Reflect changes in the signature preview canvas
	if (!checked)
		image_canvas->setReason("");
	else
		image_canvas->setReason(ui.reason_textbox->text());
	image_canvas->update();
}

void PDFSignWindow::on_smallsig_checkBox_toggled(bool checked)
{
	ui.pushButton_imgChooser->setEnabled(!checked);
	ui.pushButton_resetImage->setEnabled(!checked);

	image_canvas->setSmallSignature(checked);
	image_canvas->update();

	m_small_signature = checked;

	if (my_scene)
	{
		//Delete and re-add a different number of squares
		my_scene->clear();
		addSquares();

		if(checked)
			my_rectangle->makeItSmaller();

	}

	//Keep the free selection mode if its the current mode
	if (my_scene->isFreeSelectMode())
	{
		invertSelectionMode();
		my_rectangle->show();
	}
}

void PDFSignWindow::on_checkBox_location_toggled(bool checked)
{

	ui.location_textbox->setEnabled(checked);
	// Reflect changes in the signature preview canvas
	if (!checked)
		image_canvas->setLocation("");
	else
		image_canvas->setLocation(ui.location_textbox->text());
	image_canvas->update();
}

void PDFSignWindow::on_reason_textbox_textEdited(QString text)
{
	image_canvas->setReason(text);
	image_canvas->update();
}

void PDFSignWindow::on_location_textbox_textEdited(QString text)
{
	image_canvas->setLocation(text);
	image_canvas->update();
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

    //Pre-select first page option
    if (!ui.radioButton_choosepage->isChecked() && !ui.radioButton_firstpage->isChecked()
            && !ui.radioButton_lastpage->isChecked())
        ui.radioButton_firstpage->click();

	bool choose_b = ui.radioButton_choosepage->isChecked();

	if (checked) {
		ui.spinBox_page->setEnabled(choose_b);
	} else {
		ui.spinBox_page->setEnabled(false);
	}

	// ui.label_page->setEnabled(checked);
	// ui.label_choose_sector->setEnabled(checked);
	// ui.label_selectedsector->setEnabled(checked);
	// ui.pushButton_freeselection->setEnabled(checked);
	ui.pushButton_imgChooser->setEnabled(checked);
	ui.pushButton_resetImage->setEnabled(checked);
	ui.pushButton_freeselect->setEnabled(checked);
	ui.smallsig_checkBox->setEnabled(checked);

}

void PDFSignWindow::on_pushButton_imgChooser_clicked()
{
	QString defaultopenfilepath = QDir::homePath();
	QString image_file = QFileDialog::getOpenFileName(this, tr("Select File(s)"),
			defaultopenfilepath, "Images (*.png *.jpg)");

	m_custom_image = QImage(image_file);

	if (!m_custom_image.isNull())
		this->image_canvas->setCustomPixmap(QPixmap::fromImage(m_custom_image));
	// ui.image_canvas->setCustomPixmap(QPixmap::fromImage(original_image));
}

void PDFSignWindow::on_pushButton_resetImage_clicked()
{

	this->image_canvas->setCustomPixmap(QPixmap());
	this->image_canvas->update();
}

void PDFSignWindow::ShowSuccessMsgBox()
{

	QString caption = tr("PDF Signature");
        QString msg = tr("Signature(s) succesfully generated");
	QMessageBox msgBoxp(QMessageBox::Information, caption, msg, 0, this);
	msgBoxp.exec();

}

void PDFSignWindow::ShowErrorMsgBox(QString msg)
{

	QString caption  = tr("Error");

  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
  	msgBoxp.exec();
}

void PDFSignWindow::run_sign(int selected_page, QString &savefilepath,
	       	char *location, char *reason)
{

	PTEID_EIDCard* card = dynamic_cast<PTEID_EIDCard*>(m_CI_Data.m_pCard);
	int sign_rc = 0;
	char * save_path = strdup(getPlatformNativeString(savefilepath));
	try
	{
		//printf("@PDFSignWindow::run_sign:\n");
		//printf("x=%f, y=%f\n", sig_coord_x, sig_coord_y);
		//printf("selected_page=%d, selected_sector=%d\n", selected_page, m_selected_sector);

		if (sig_coord_x != -1 && sig_coord_y != -1)
			sign_rc = card->SignPDF(*m_pdf_sig, selected_page,
			sig_coord_x, sig_coord_y, location, reason, save_path);
		else
			sign_rc = card->SignPDF(*m_pdf_sig, selected_page,
                m_selected_sector, m_landscape_mode, location, reason, save_path);

		if (sign_rc == 0)
			this->success = SIG_SUCCESS;
		else
			this->success = TS_WARNING;

	}

	catch (PTEID_Exception &e)
	{
		this->success = SIG_ERROR;
		fprintf(stderr, "Caught exception in some SDK method. Error code: 0x%08x\n", 
			(unsigned int)e.GetError());
	}

	free(save_path);

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
		//TODO: The batch signing is all very hackish with validations missing
		// and this awkward API...
		//First we need to free the first instance that was created unless we want to leak the file handle...
		delete m_pdf_sig;
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

	if (ui.timestamp_checkBox->isChecked())
		m_pdf_sig->enableTimestamp();

	if (ui.smallsig_checkBox->isChecked())
		m_pdf_sig->enableSmallSignatureFormat();

	//Generate a scaled JPEG and pass the byte array to the Signature class
	if (!m_custom_image.isNull())
	{

		QBuffer buffer(&m_jpeg_scaled_data);
		buffer.open(QIODevice::WriteOnly);

		//Save the generated image as high-quality JPEG data
		QImage generated_img = image_canvas->drawToImage();
		generated_img.save(&buffer, "JPG", 100);
		//generated_img.save("/tmp/generated.jpg", "JPG", 100);
		// fprintf(stderr, "setting CustomImage with data= 0x%p: %d bytes\n", m_jpeg_scaled_data.data(), m_jpeg_scaled_data.size());

		m_pdf_sig->setCustomImage((unsigned char *)m_jpeg_scaled_data.data(), m_jpeg_scaled_data.size());

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
			&PDFSignWindow::run_sign, selected_page, savefilepath, location, reason);

	this->FutureWatcher.setFuture(future);
	pdialog->exec();

	if (this->success == SIG_SUCCESS)
		ShowSuccessMsgBox();
	else if (this->success == TS_WARNING)
	{
		QString sig_detail = model->rowCount() > 1 ?  tr("some of the timestamps could not be applied") :
				tr("the timestamp could not be applied");
		ShowErrorMsgBox(tr("Signature(s) successfully generated but ")+ sig_detail);
	}
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

double g_scene_height;
double g_scene_width;
const int margin = 5;

void PDFSignWindow::buildLocationTab()
{
    g_scene_height = 370;
    g_scene_width = 282;
//  double rect_h = g_scene_height *0.106888361045; //Proportional to current signature height
//	double rect_w = g_scene_width / 3.0;

    MyGraphicsScene *scene = new MyGraphicsScene(tr("A4 Page"));

    my_rectangle = NULL;
    // m_landscape_mode = landscape_mode;

    if (m_landscape_mode)
    {
        double tmp = g_scene_width;
        g_scene_width = g_scene_height;
        g_scene_height = tmp;

    	//ui.widget->setMinimumWidth(tmp);
    }

    this->rx = 0;
    this->ry = 0;

    // scene->setSceneRect(0, 0, real_scene_width, real_scene_height);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    my_scene = scene;
    QGraphicsView *view = ui.scene_view;
    view->setFixedSize(g_scene_width, g_scene_height);
    view->setScene(scene);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    view->setToolTip(tr("Choose the page sector where you want your signature to appear.\n"
                "The grey sectors are already filled with other signatures."));
    // view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    addSquares();
    
    my_scene->setParent(this);
    // fprintf(stderr, "FreeSelectionDialog Width: %d, Height: %d\n", ui.widget->width(), ui.widget->height());

    //Add room for margins, the textboxes and buttons below the widget
    // this->setFixedSize(ui.widget->width() + 50, ui.widget->height() + 70);

}

void PDFSignWindow::on_radioButton_firstpage_toggled(bool value)
{
	if (value && m_pdf_sig)
	{
		// clearAllSectors();
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

	if (sector < 1)
		return;

	int rem = sector % 3;

	*row = (sector-1) / 3;

	if (rem == 0)
		*column = 2;
	else if (rem == 1)
		*column = 0;
	else if (rem == 2)
		*column = 1;

}

void MyGraphicsScene::itemMoved(QPointF newPos)
{
	if (parent)
		((PDFSignWindow*)parent)->setPosition(newPos);

}

void MyGraphicsScene::setOccupiedSector(int s)
{
    QList<QGraphicsItem *> scene_items = this->items();

    int i = 0;

    while (i!= scene_items.size())
    {
        QGraphicsItem *item = scene_items.at(i);
        //Find the rectangle object
        if (item->type() == SelectableRectType && ((SelectableRectangle *)item)->getSectorNumber() == s)
        {
            SelectableRectangle *rect = qgraphicsitem_cast<SelectableRectangle*>(scene_items.at(i));
            rect->setSectorFilled();
        }

        i++;
    }
}

void MyGraphicsScene::clearAllRectangles()
{
    QList<QGraphicsItem *> scene_items = this->items();

    int i = 0;

    while (i!= scene_items.size())
    {
        QGraphicsItem *item = scene_items.at(i);
        //Find the rectangle object
        if (item->type() == SelectableRectType)
        {
            SelectableRectangle *rect = qgraphicsitem_cast<SelectableRectangle*>(scene_items.at(i));
            rect->resetColor();
        }

        i++;
    }
}

void MyGraphicsScene::selectNewRectangle(QPointF selected_pos)
{
 	QList<QGraphicsItem *> scene_items = this->items();

	int i = 0;
    QGraphicsItem *it = this->itemAt(selected_pos);
    ((PDFSignWindow*)parent)->setSelectedSector(((SelectableRectangle *)it)->getSectorNumber());
	while (i!= scene_items.size())
	{
		QGraphicsItem *item = scene_items.at(i);
		//Find the rectangle object
        if (item->type() == SelectableRectType && item->pos() != selected_pos &&
              !((SelectableRectangle *)item)->isSectorFilled())
		{
			SelectableRectangle *rect = qgraphicsitem_cast<SelectableRectangle*>(scene_items.at(i));
			rect->resetColor();
		}

		i++;
	}
}

/* Commented out for now, it could only show up if the selectableRectangles were semi-transparent
	which would then cause rendering glitches while dragging the grey rectangle
void MyGraphicsScene::drawBackground(QPainter * painter, const QRectF & rect )
{

	QFont sansSerifFont("Arial", 26, QFont::Bold);
	painter->setPen(Qt::lightGray);
	painter->setFont(sansSerifFont);

	painter->drawText(rect, Qt::AlignCenter, this->background);

}
*/

void Rectangle::paint(QPainter *painter,
		const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::black);
	painter->setBrush(Qt::lightGray);
	painter->drawRect(0, 0, m_rect_w, m_rect_h);
}

void Rectangle::makeItSmaller()
{
	m_rect_h /= 2;
}

void Rectangle::makeItLarger()
{
	m_rect_h *= 2;
}

QRectF Rectangle::boundingRect() const
{
	return QRectF(0, 0, m_rect_w+1, m_rect_h+1);
}


/* Change mouse cursor to indicate draggable rectangle
 */
void Rectangle::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	//setCursor(Qt::OpenHandCursor);
	QApplication::setOverrideCursor(QCursor(Qt::OpenHandCursor));
	update();
}

void Rectangle::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	update();
}

void Rectangle::mousePressEvent (QGraphicsSceneMouseEvent * event)
{

	QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));

}

void Rectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{

	QPointF tmp = scenePos();
	// qDebug() << "Boundary value used: " <<  m_max_y+margin - m_rect_h;
	// qDebug() << "Boundary value used: " << m_max_x+margin - m_rect_w ;
	
	/* Detect out of boundaries mouseEvent 
	and correct them to the nearest valid position **/
	if (tmp.rx() < margin)
		tmp.setX(margin);
	if (tmp.ry() < margin)
		tmp.setY(margin);
	if (tmp.rx() >= m_max_x + margin - m_rect_w) 
		tmp.setX(m_max_x + margin-m_rect_w);
	if (tmp.ry() >= m_max_y + margin - m_rect_h)
		tmp.setY(m_max_y + margin - m_rect_h);

	QApplication::setOverrideCursor(QCursor(Qt::OpenHandCursor));
	setPos(tmp);
	//Send message to parent
	my_scene->itemMoved(tmp);
	QGraphicsItem::mouseReleaseEvent(event);
}

QRectF SelectableRectangle::boundingRect() const
{
	return QRectF(0, 0, m_rect_w, m_rect_h);
}

void SelectableRectangle::resetColor()
{
	m_brush = QBrush(QColor(193, 221, 221));
	update();
}

void SelectableRectangle::setSectorFilled()
{
    m_brush = QBrush(QColor(Qt::darkGray));
    is_sector_filled = true;
    update();
}


void SelectableRectangle::mousePressEvent (QGraphicsSceneMouseEvent * event)
{

}

void SelectableRectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    //Ignore the event if we are in Free Selection mode
    if (dont_select_mode || is_sector_filled)
        return;

	m_brush = QBrush(Qt::black);
    my_scene->selectNewRectangle(pos());
	update();
}

void SelectableRectangle::paint(QPainter *painter,
		const QStyleOptionGraphicsItem *, QWidget *)
{

	painter->fillRect(0, 0, m_rect_w, m_rect_h, m_brush);
	painter->setPen(Qt::black);
	painter->drawRect(0, 0, m_rect_w, m_rect_h);
	

	painter->setBrush(Qt::lightGray);

	if (!dont_select_mode)
	{
		//Draw text centered in the rectangle
		QFont sansSerifFont("Arial", 16, QFont::Bold);
		painter->setPen(Qt::lightGray);
		painter->setFont(sansSerifFont);

		painter->drawText(boundingRect(), Qt::AlignCenter, m_string);
	}
}


double PDFSignWindow::convertX()
{
	double full_width = m_landscape_mode ? 297.0576 : 209.916;
	return this->rx/g_scene_width * full_width;
}

double PDFSignWindow::convertY()
{
	double full_height = m_landscape_mode ? 209.916 : 297.0576;
	return this->ry/g_scene_height * full_height;
}

/* Coordinate conversion */
void PDFSignWindow::setPosition(QPointF new_pos)
{
    this->rx = new_pos.rx()-margin;
	this->ry = new_pos.ry()-margin;

    //Actual coordinates passed to SignPDF() expressed as a fraction
    sig_coord_x = this->rx/g_scene_width;
    sig_coord_y = this->ry/g_scene_height;

     ui.label_x->setText(tr("Horizontal position: %1")
        .arg(QString::number(convertX(), 'f', 1)));
	
     ui.label_y->setText(tr("Vertical position: %1")
        .arg(QString::number(convertY(), 'f', 1)));

}


void PDFSignWindow::setSelectedSector(int sector)
{
    m_selected_sector = sector;
    ui.label_x->setText(tr("Selected sector: %1").arg(QString::number(sector)));
    ui.label_y->setText("");
}

void PDFSignWindow::on_pushButton_freeselect_clicked()
{

    if (my_scene->isFreeSelectMode())
    {
        ui.pushButton_freeselect->setText(tr("Free Selection"));
        if (my_rectangle)
        	my_rectangle->hide();

    }
    else
    {

        ui.pushButton_freeselect->setText(tr("Show sectors"));
     
        my_rectangle->show();
      
    }

    my_scene->switchFreeSelectMode();

    invertSelectionMode();

}


void PDFSignWindow::invertSelectionMode()
{
	   //Invert mode on the SelectableRectangles
        QList<QGraphicsItem *> scene_items = my_scene->items();
        int i = 0;
        while (i!= scene_items.size())
        {
            QGraphicsItem *item = scene_items.at(i);
            //Find the rectangle object
            if (item->type() == SelectableRectType)
            {
                SelectableRectangle *rect = qgraphicsitem_cast<SelectableRectangle*>(scene_items.at(i));
                rect->switchSelectionMode();
                rect->resetColor();
            }

            i++;
        }

}

void PDFSignWindow::getValues(double *x, double *y)
{
    *x = convertX();
       *y = convertY();
}

void PDFSignWindow::resetRectanglePos()
{

	QList<QGraphicsItem *> scene_items = my_scene->items();

	QPointF pos;
	pos.setY(0.0);
	pos.setX(0.0);

	int i = 0;
	while (i!= scene_items.size())
	{
		//Find the rectangle object
		if (scene_items.at(i)->type() == QGraphicsItem::UserType)
		{
			scene_items.at(i)->setPos(pos);
			break;
		}
		i++;
	}

}

void PDFSignWindow::addSquares()
{
    double scene_height = ui.scene_view->height()-2*margin;
    double scene_width = ui.scene_view->width()-2*margin;

    qDebug() << "scene_height: " << scene_height;
    qDebug() << "scene_width: " << scene_width;

    int h_lines = 0, v_lines = 0;

    if (m_landscape_mode)
    {
        scene_height = ui.scene_view->height()-2*margin;
        scene_width = ui.scene_view->width()-2*margin;

        if (m_small_signature)
        {
        	h_lines = 10; v_lines = 4;
        }
        else
        {
        	h_lines = 5; v_lines = 4;
        }
    }
    else
    {
    	if (m_small_signature)
    	{
    		h_lines = 12; v_lines = 3;
    	}
    	else
    	{
    		h_lines = 6;  v_lines = 3;
    	}
    }

    // fprintf(stderr, "DEBUG: g_scene_height: %f, g_scene_width: %f\n", g_scene_height, g_scene_width);
    // fprintf(stderr, "DEBUG: scene_height: %f, scene_width: %f\n", scene_height, scene_width);

    //Sector number to be displayed at the center of the rectangle
    int count = 1;

    for (int j= 0; j!= h_lines; j++)
        for (int i= 0; i!= v_lines; i++)
        {
            SelectableRectangle * s1 = new SelectableRectangle(my_scene, scene_height/h_lines, scene_width/v_lines, count);

            my_scene->addItem(s1);
            s1->setPos(QPointF(i*scene_width/v_lines +margin, j*scene_height/h_lines +margin ));

            count++;

        }

        my_rectangle = new Rectangle(my_scene, scene_height, scene_width, 30, 50);
        my_scene->addItem(my_rectangle);
        my_rectangle->hide();

}


void PDFSignWindow::clearAllSectors()
{

     my_scene->clearAllRectangles();

}


void PDFSignWindow::highlightSectors(QString &csv_sectors)
{
	if (csv_sectors.length() == 0)
		return;

	QStringList sl = csv_sectors.split(",");
	QStringListIterator sectors_it(sl);

	while (sectors_it.hasNext())
	{
		uint s = sectors_it.next().toUInt();
        my_scene->setOccupiedSector(s);

	}

}


void PDFSignWindow::addFileToListView(QStringList &str)
{
	if (str.isEmpty())
		return;

	current_input_path = str.at(0);

	m_pdf_sig = new PTEID_PDFSignature(strdup(getPlatformNativeString(current_input_path)));
	
	
	int tmp_count = m_pdf_sig->getPageCount();

	if (tmp_count < 1)
	{
		QFileInfo fi(current_input_path);
		ShowErrorMsgBox(tr("Unsupported or damaged PDF file: ") + fi.fileName());
		m_pdf_sig = NULL;
		return;
	}
	m_current_page_number = tmp_count;
    m_landscape_mode = m_pdf_sig->isLandscapeFormat();

    qDebug() << "Landscape format? " << m_landscape_mode;

	for(int i=0; i != str.size(); i++)
	{

		list_model->insertRows(list_model->rowCount(), 1);
		list_model->setData(list_model->index(list_model->rowCount()-1, 0), str.at(i));

	}

	//Set the spinbox with the appropriate max value
	ui.spinBox_page->setMaximum(m_current_page_number);

	// QString sectors = QString::fromAscii(m_pdf_sig->getOccupiedSectors(1));
	// highlightSectors(sectors);

	if (list_model->rowCount() > 1)
	{
		// clearAllSectors();
	}

	//Enable sign button now that we have data and a card inserted
	if (!str.isEmpty() && this->card_present)
		ui.button_sign->setEnabled(true);

	if (!my_scene)
    	buildLocationTab();

}
