/* ****************************************************************************

 * PTeID Middleware Project.
 * Copyright (C) 2012-2017 Caixa Mágica Software.
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
#include <QMessageBox>
#include <QtConcurrent>
#include <QProgressDialog>
#include <QTextStream>
#include <QPainter>
#include <QStyleOption>
#include <QPen>
#include <QCursor>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QFileInfo>
#include <QDateTime>
#include <QInputDialog>
//For std::min_element()
#include <algorithm>

#include <eidlib.h>
#include "eidErrors.h"
#include "PDFSignWindow.h"
#include "mylistview.h"
#include "dlgUtils.h"
#include "dlgCmdUserInfo.h"
#include "dlgCmdCode.h"

#include "PDFSignature.h"
#include "PDFSignatureCli.h"

using namespace eIDMW;

PDFSignWindow::PDFSignWindow( QWidget* parent
                              , int selected_reader
                              , CardInformation& CI_Data )
    : m_CI_Data(CI_Data), m_selected_reader(selected_reader), card_present(true), m_doc(NULL), my_rectangle(NULL), my_scene(NULL)
{
    ui.setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    flags ^= Qt::WindowMaximizeButtonHint;
    //Remove the Context Help Window button
    flags = flags & ( ~Qt::WindowContextHelpButtonHint );
    setWindowFlags( flags );

    //Set icon
    const QIcon Ico = QIcon(":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png");

    this->setWindowIcon(Ico);
    int i=0, j=0;

    m_pdf_sig = NULL;
    success = NOT_YET_SIGNED;

    sig_coord_x = -1, sig_coord_y = -1;
    success = SIG_ERROR;
    horizontal_page_flag = false;
    m_small_signature = false;
    ui.spinBox_page->setValue(1);
    list_model = new QStringListModel();
    ui.pdf_listview->setModel(list_model);
    ui.pdf_listview->enableNotify();

    //Sensible limits for the signature metadata because it will be displayed in the visible signature limited space
    ui.reason_textbox->setMaxLength( 96 );
    ui.location_textbox->setMaxLength( 35 );

    image_canvas = new ImageCanvas( ui.tab3 );
    image_canvas->setCitizenName(composeCitizenFullName());
    image_canvas->setCitizenNIC( getCitizenNIC() );

    image_canvas->setFixedSize( 420, 200 );
    ui.verticalLayout_4->addWidget( image_canvas );

    ui.button_clearFiles->setEnabled( false );

    ui.verticalLayout1->setSpacing( 5 );

    int items = ui.verticalLayout_4->count();

    for(int i = 0; i!= items; i++){
        ui.verticalLayout_4->itemAt(i)->setAlignment( Qt::AlignTop );
    }/* for(int i) */

    items = ui.verticalLayout1->count();

    for (int i = 0; i!= items; i++){
        ui.verticalLayout1->itemAt(i)->setAlignment( Qt::AlignTop );
    }/* for(int i) */

    for (int i = 0; i!= items; i++){
        ui.verticalLayout_4->itemAt(i)->setAlignment( Qt::AlignTop );
    }/* for(int i) */

    items = ui.horizontalLayout_tab1->count();
    for (int i = 0; i!= items; i++){
        ui.horizontalLayout_tab1->itemAt(i)->setAlignment( Qt::AlignHCenter );
    }/* for(int i) */

    this->setFixedSize( this->width(), this->height() );

    m_client = NULL;

    connect( ui.pdf_listview
             , SIGNAL(itemRemoved(int))
             , this
             , SLOT(updateMaxPage(int) ) );

    PTEID_InitSDK();
}/* PDFSignWindow::PDFSignWindow() */

PDFSignWindow::~PDFSignWindow(){
    PTEID_ReleaseSDK();

    delete list_model;
    closeDocument();

    /* TODO: the condition sucess != SIG_ERROR is a workaround
     * for the issue of cancel button not actually canceling the thread
     * If we free'd the file handle here, the background thread would crash
     */
    if (m_pdf_sig && (success != SIG_ERROR)) delete m_pdf_sig;
}/* PDFSignWindow::~PDFSignWindow() */

QString PDFSignWindow::composeCitizenFullName(){
    const tFieldMap PersonFields = m_CI_Data.m_PersonInfo.getFields();

    return QString::fromUtf8(PersonFields[GIVENNAME].toStdString().c_str()) + " " +
        QString::fromUtf8(PersonFields[NAME].toStdString().c_str());
}/* PDFSignWindow::composeCitizenFullName() */

QString documentNumberToNIC(const QString &doc_number){
    int last_space = doc_number.lastIndexOf(' ');
    return doc_number.left(last_space);
}/* documentNumberToNIC() */

QString PDFSignWindow::getCitizenNIC(){
    const tFieldMap PersonFields = m_CI_Data.m_PersonInfo.getFields();
    const QString doc_number = PersonFields[DOCUMENTNUMBER];

    return documentNumberToNIC(doc_number);
}/* PDFSignWindow::getCitizenNIC() */

void PDFSignWindow::customEvent( QEvent *ev ){
    if ( ev->type() == QEvent::User ){
        ui.button_sign->setEnabled( false );
        if ( my_scene ){
            //Clear the background pixmap
            QPixmap null_pixmap;
            my_scene->updateBackground( null_pixmap );

            if ( my_rectangle ) my_rectangle->hide();

            my_scene->update();
        }/* if ( my_scene ) */
    }/* if ( ev->type() == QEvent::User ) */
}/* PDFSignWindow::customEvent() */

void PDFSignWindow::enableSignButton(){
    this->card_present = true;
}/* PDFSignWindow::enableSignButton() */

void PDFSignWindow::disableSignButton(){
    ui.button_sign->setEnabled( false );
    this->card_present = false;
}/* PDFSignWindow::disableSignButton() */

void ImageCanvas::initDateString(){
    QDateTime now = QDateTime::currentDateTime();

    this->date_str = now.toString( "yyyy.MM.dd hh:mm:ss" );
}/* ImageCanvas::initDateString() */

QImage ImageCanvas::drawToImage(){
    const int img_width = 185;
    const int img_height = 42;

    QImage output_img( img_width
                     , img_height
                     , QImage::Format_RGB32 );

    /* We need to fill the white background
     *  because the user-selected image may be
     * too narrow for the predefined width
     */
    output_img.fill( Qt::white );

    QPainter painter( &output_img );

    //Scale height if needed
    QPixmap scaled = user_pixmap.height() > img_height
              ?  user_pixmap.scaledToHeight( img_height, Qt::SmoothTransformation )
              : user_pixmap;

    painter.drawPixmap( 0, 0, scaled );

    return output_img;
}/* ImageCanvas::drawToImage() */

void ImageCanvas::paintEvent( QPaintEvent * ){
	const int spacing = 10;
	const int line_spacing = 5;
	int line_count = 0;
	int current_y_pos = 5;

    if ( !this->previewVisible ) return;

	QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing );

    QPixmap pixmap2( ":/images/Images/Icons/pteid_signature_watermark.jpg" );
    QPixmap default_icon( ":/images/Images/Icons/pteid_signature_banner.jpg" );

	QFont current_font = painter.font();
#ifdef __APPLE__
    current_font.setPointSize( 10 );
#else
    current_font.setPointSize( 9 );
#endif

    painter.setFont( current_font );

	int line_height = fontMetrics().height();
	current_y_pos = line_height;

    QRectF border_rectangle( 0.0, 0.0, this->width()-40, this->height()-5 );
 	QPen my_pen = painter.pen();
    my_pen.setWidth( 2 );
    painter.setPen( my_pen );
    painter.setBrush( Qt::white );
    painter.drawRoundedRect( border_rectangle, 20.0, 15.0 );

    if ( !small_sig_format && !sig_reason.isEmpty() ){
		QPen new_pen = painter.pen();
		QPen orig_pen = painter.pen();
		// new_pen.setItalic(true);
        new_pen.setColor( QColor( 78, 138, 190 ) );
        painter.setPen( new_pen );
		current_y_pos += line_spacing;
        painter.drawText( 5, current_y_pos,  sig_reason );
        painter.setPen( orig_pen );
    }/* if ( !small_sig_format && !sig_reason.isEmpty() ) */

 	current_y_pos += line_height + line_spacing;
 	const QString SIGNED_BY = "Assinado por: ";

    QRect bounding_rect = painter.boundingRect( 5
                                        , current_y_pos
                                        , 420, 100
                                        , Qt::AlignLeft | Qt::AlignTop, SIGNED_BY );
 	//Draw background watermark aligned with SIGNED_BY line
    painter.drawPixmap( 5, current_y_pos-fontMetrics().xHeight(), pixmap2 );
    painter.drawText( 5, current_y_pos, SIGNED_BY );

	QFont my_font = painter.font();
    my_font.setBold( true );
    painter.setFont( my_font );

    QRect citizen_name_rect = fontMetrics().boundingRect( citizen_name );

    int available_width = this->width() - 40 - bounding_rect.width() - 5;

	//Check if 2 lines are needed for longer names
    if ( citizen_name_rect.width() > available_width ){
		//Add words one by one until we run out of space
		QStringList words = citizen_name.split(" ");
		QString tmp, first_line, second_line;
        for( int i = 0; i < words.size(); ++i ){
            if (i > 0) tmp += " ";

			QString new_string = tmp + words.at(i);

            if ( fontMetrics().boundingRect(new_string).width() > available_width ){
				first_line = tmp;
				second_line = QStringList(words.mid(i)).join(" ");
				break;
            } else{
				tmp = new_string;
            }/* !if (fontMetrics().boundingRect(new_string).width() > available_width) */
        }/* for (int i = 0) */
        painter.drawText( bounding_rect.width() + 5, current_y_pos, first_line );

		current_y_pos += line_height + line_spacing;
        painter.drawText( 5, current_y_pos, second_line );
    } else{
        painter.drawText( bounding_rect.width() + 5, current_y_pos, citizen_name );
    }/* if ( citizen_name_rect.width() > available_width ) */

    my_font.setBold( false );
    painter.setFont( my_font );

	current_y_pos += line_height + line_spacing;
    painter.drawText( 5
                    , current_y_pos
                    , QString::fromUtf8("Num de Identificação Civil: ") + citizen_nic );


	current_y_pos += line_height + line_spacing;
    painter.drawText( 5, current_y_pos, "Data: " + this->date_str );

    if ( !small_sig_format && !sig_location.isEmpty() ){
		current_y_pos += line_height + line_spacing;
        painter.drawText( 5, current_y_pos, "Local: " + sig_location );
    }/* if ( !small_sig_format && !sig_location.isEmpty() ) */

	current_y_pos += line_spacing * 2;

    if ( pixmap2.isNull() ){
        qDebug() << "pixmap2 not loaded! ";
        return;
    }/* if ( pixmap2.isNull() ) */

    //Dont draw the second image if the signature is small format
    if ( small_sig_format ){
    	return;
    }/* if ( small_sig_format ) */

    if ( !user_pixmap.isNull() ){
        QPixmap scaled = user_pixmap.height() > default_icon.height()
                ? user_pixmap.scaledToHeight( default_icon.height(), Qt::SmoothTransformation )
                : user_pixmap;
        painter.drawPixmap( 5, current_y_pos, scaled );
    } else{
        painter.drawPixmap( 5, current_y_pos, default_icon );
    }/* if ( !user_pixmap.isNull() ) */
}/* ImageCanvas::paintEvent() */

void PDFSignWindow::on_button_cancel_clicked(){
    this->close();
}/* PDFSignWindow::on_button_cancel_clicked() */

void PDFSignWindow::on_checkBox_reason_toggled( bool checked ){
    ui.reason_textbox->setEnabled( checked );

    // Reflect changes in the signature preview canvas
    if ( !checked ){
        image_canvas->setReason("");
    } else{
        image_canvas->setReason( ui.reason_textbox->text() );
    }/* if ( !checked ) */

    image_canvas->update();
}/* PDFSignWindow::on_checkBox_reason_toggled() */

void PDFSignWindow::on_smallsig_checkBox_toggled( bool checked ){

    ui.pushButton_imgChooser->setEnabled( !checked );
    ui.pushButton_resetImage->setEnabled( !checked );

    image_canvas->setSmallSignature( checked );
    image_canvas->update();

    m_small_signature = checked;

    if ( checked ){
        my_rectangle->makeItSmaller();
        my_scene->update();
    } else{
        my_rectangle->makeItLarger();
        my_scene->update();
    }/* if ( checked ) */
}/* PDFSignWindow::on_smallsig_checkBox_toggled() */

void PDFSignWindow::on_checkBox_location_toggled( bool checked ){
    ui.location_textbox->setEnabled( checked );

    // Reflect changes in the signature preview canvas
    if ( !checked ){
        image_canvas->setLocation("");
    } else{
        image_canvas->setLocation(ui.location_textbox->text());
    }/* if ( !checked ) */

    image_canvas->update();
}/* PDFSignWindow::on_checkBox_location_toggled() */

void PDFSignWindow::on_reason_textbox_textEdited( QString text ){
    image_canvas->setReason(text);
    image_canvas->update();
}/* PDFSignWindow::on_reason_textbox_textEdited() */

void PDFSignWindow::on_location_textbox_textEdited( QString text ){
    image_canvas->setLocation( text );
    image_canvas->update();
}/* PDFSignWindow::on_location_textbox_textEdited() */

void PDFSignWindow::on_radioButton_choosepage_toggled( bool checked ){
    ui.spinBox_page->setEnabled( checked );
}/* PDFSignWindow::on_radioButton_choosepage_toggled() */

void PDFSignWindow::on_visible_checkBox_toggled( bool checked ){
    ui.radioButton_firstpage->setEnabled( checked );
    ui.radioButton_lastpage->setEnabled( checked );
    ui.radioButton_choosepage->setEnabled( checked );

    ui.scene_view->setEnabled( checked );

    //Pre-select first page option
    if ( !ui.radioButton_choosepage->isChecked()
         && !ui.radioButton_firstpage->isChecked()
         && !ui.radioButton_lastpage->isChecked() ){
        ui.radioButton_firstpage->click();
    }/* if ( !ui.radioButton_choosepage->isChecked() ... */

    bool choose_b = ui.radioButton_choosepage->isChecked();

    if ( checked ){
        ui.spinBox_page->setEnabled( choose_b );
        if (my_rectangle != NULL) my_rectangle->show();
    } else{
        ui.spinBox_page->setEnabled( false );
        my_rectangle->hide();
    }/* if ( checked ) */

    ui.pushButton_imgChooser->setEnabled( checked );
    ui.smallsig_checkBox->setEnabled( checked );

    this->image_canvas->setPreviewEnabled( checked );

    my_scene->enableOverlayRectangle( !checked );
}/* PDFSignWindow::on_visible_checkBox_toggled() */

void PDFSignWindow::on_pushButton_imgChooser_clicked(){
    QString defaultopenfilepath = QDir::homePath();
    QString image_file = QFileDialog::getOpenFileName( this
                                        , tr("Select File(s)")
                                        , defaultopenfilepath
                                        , "Images (*.png *.jpg)");

    m_custom_image = QImage( image_file );

    if ( !m_custom_image.isNull() ){
        this->image_canvas->setCustomPixmap( QPixmap::fromImage( m_custom_image ) );
        this->image_canvas->update();

        //This button is only useful when we load a custom image
        ui.pushButton_resetImage->setEnabled( true );
    }/* if ( !m_custom_image.isNull() ) */
}/* PDFSignWindow::on_pushButton_imgChooser_clicked() */

void PDFSignWindow::on_pushButton_resetImage_clicked(){
    this->image_canvas->setCustomPixmap( QPixmap() );
    this->image_canvas->update();
}/* PDFSignWindow::on_pushButton_resetImage_clicked() */

void PDFSignWindow::ShowSuccessMsgBox(){
    QString caption = tr( "PDF Signature" );
    QString msg = tr( "Signature(s) succesfully generated" );
    QMessageBox msgBoxp( QMessageBox::Information, caption, msg, 0, this );
    msgBoxp.exec();
}/* PDFSignWindow::on_pushButton_resetImage_clicked() */

void PDFSignWindow::ShowErrorMsgBox( QString msg ){
    QString caption  = tr( "Error" );

    QMessageBox msgBoxp( QMessageBox::Warning, caption, msg, 0, this );
    msgBoxp.exec();
}/* PDFSignWindow::ShowErrorMsgBox() */

PTEID_EIDCard& PDFSignWindow::getNewCard(){
    unsigned long	ReaderStartIdx = m_selected_reader;
    bool			bRefresh	   = false;
    unsigned long	ReaderEndIdx   = ReaderSet.readerCount( bRefresh );
    unsigned long	ReaderIdx	   = 0;

    if (ReaderStartIdx!=(unsigned long)-1){
        ReaderEndIdx = ReaderStartIdx + 1;
    } else{
        ReaderStartIdx = 0;
    }/* if (ReaderStartIdx!=(unsigned long)-1) */

    for( ReaderIdx = ReaderStartIdx; ReaderIdx < ReaderEndIdx; ReaderIdx++ ){
        PTEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum( ReaderIdx );
        if ( ReaderContext.isCardPresent() ){
            try{
                PTEID_EIDCard& Card = ReaderContext.getEIDCard();
                return Card;
            } catch ( PTEID_ExCardBadType const& e ) {
            }/* catch () */
        }/* if ( ReaderContext.isCardPresent() ) */
    }/* for( ReaderIdx )*/
} /* PDFSignWindow::getNewCard() */

void PDFSignWindow::run_signOpen( int selected_page
                                , QString &savefilepath
                                , char *location, char *reason ){
    PTEID_EIDCard &card = getNewCard();

    printf( "getNewCard - OK\n" );

    int sign_rc = -1;
    bool keepTrying = true;
    char * save_path = strdup( getPlatformNativeString( savefilepath ) );

    printf( "strdup save_path - OK\n" );

    if ( NULL != m_client ){
        delete m_client;
    }

    m_client = new PDFSignatureCli( &card, m_pdf_sig );

    do{
        try{
            printf( "(1) run_signOpen() - sig_coord_x: %lf, sig_coord_y: %lf\n", sig_coord_x, sig_coord_y );

            sign_rc = m_client->signOpen( m_userId.toStdString(), m_userPin.toStdString()
                                    , selected_page
                                    , sig_coord_x, sig_coord_y
                                    , location, reason, save_path );

            keepTrying = false;
            this->success = (sign_rc == 0) ? SIG_SUCCESS : TS_WARNING;
        } catch ( PTEID_Exception &e ){
            this->success = SIG_ERROR;
            PTEID_LOG( PTEID_LOG_LEVEL_ERROR
                       , "eidgui"
                       , "Caught exception in signPDF() method. Error code: 0x%08x"
                       , (unsigned int)e.GetError() );

            if ( e.GetError() == EIDMW_ERR_CARD_RESET ){
                    //PTEID_EIDCard &new_card = getNewCard();
                    //card = getNewCard();
                    //card = new_card;

            } else{
                keepTrying = false;
            }/* !if ( e.GetError() == EIDMW_ERR_CARD_RESET ) */
        }/* !try */
    } while( keepTrying );

    if ( save_path != NULL ) free( save_path );
}/* PDFSignWindow::run_signOpen() */

void PDFSignWindow::run_signClose(){
    int sign_rc = -1;
    bool keepTrying = true;

    printf( "strdup save_path - OK\n" );

    if ( NULL == m_client ){
        PTEID_LOG( PTEID_LOG_LEVEL_ERROR, "eidgui", "Sign was not started!" );
        this->success = SIG_ERROR;
        return;
    }

    do{
        try{
            printf( "(1) run_signClose()\n" );

            sign_rc = m_client->signClose( m_receivedCode.toStdString() );

            keepTrying = false;
            this->success = (sign_rc == 0) ? SIG_SUCCESS : TS_WARNING;
        } catch ( PTEID_Exception &e ){
            this->success = SIG_ERROR;
            PTEID_LOG( PTEID_LOG_LEVEL_ERROR
                       , "eidgui"
                       , "Caught exception in signPDF() method. Error code: 0x%08x"
                       , (unsigned int)e.GetError() );

            if ( e.GetError() == EIDMW_ERR_CARD_RESET ){
                    //PTEID_EIDCard &new_card = getNewCard();
                    //card = getNewCard();
                    //card = new_card;

            } else{
                keepTrying = false;
            }/* !if ( e.GetError() == EIDMW_ERR_CARD_RESET ) */
        }/* !try */
    } while( keepTrying );
}/* PDFSignWindow::run_signClose() */

void PDFSignWindow::on_button_sign_clicked(){

    /*dlgCmdUserInfo diagCmdUserInfo( this );
    diagCmdUserInfo.exec();
    QString userId = diagCmdUserInfo.getUserId();
    QString userPin = diagCmdUserInfo.getUserPin();

    printf( "userId: %s\n", userId.toStdString().c_str() );
    printf( "userPin: %s\n", userPin.toStdString().c_str() );*/

    /*dlgCmdCode diagCmdCode( this );
    diagCmdCode.exec();
    QString receivedCode = diagCmdCode.getReceivedCode();
    printf( "receivedCode: %s\n", receivedCode.toStdString().c_str() );*/

    /*
        *************************************************************************
        *       PDFSignature                                                    *
        *************************************************************************
    */

    //For invisible sigs the implementation we'll
    //need to add a reference to the sigfield in some page so...
    int selected_page = 1;
    QString savefilepath;

    //Read Page
#if 1
    if ( ui.visible_checkBox->isChecked() ){
        //Validate if any location was chosen
        if ( sig_coord_x == -1 ){
            ShowErrorMsgBox(tr("You must choose a location for visible signature!"));
            return;
        }/* if ( sig_coord_x == -1 ) */

        selected_page = m_current_page_number;
    }/* if ( ui.visible_checkBox->isChecked() ) */
#else
    QPointF new_pos;
    if ( my_scene == NULL ){
        new_pos.setX( -1 );
        new_pos.setY( -1 );
    }else{
        new_pos.setX( my_scene->rx() );
        new_pos.setY( my_scene->rx() );
    }

    /*qreal pos_x = ( sig_coord_x < 0 ) ? -1 : sig_coord_x;
    qreal pos_y = ( sig_coord_y < 0 ) ? -1 : sig_coord_y;

    printf( "(1) on_button_sign_clicked() - pos_x      : %lf, pos_y      : %lf\n", pos_x, pos_y );
    printf( "(2) on_button_sign_clicked() - sig_coord_x: %lf, sig_coord_y: %lf\n", sig_coord_x, sig_coord_y );
    QPointF new_pos( pos_x, pos_y );*/
    setPosition( new_pos );
#endif //0

    QStringListModel *model = dynamic_cast<QStringListModel *>(list_model);
    if ( model->rowCount() == 0 ){
       return;

    }/* if ( model->rowCount() == 0 ) */

    QFileInfo my_file_info( current_input_path );
    QString defaultsavefilepath = my_file_info.dir().absolutePath();

    if ( model->rowCount() > 1 ){

        savefilepath = QFileDialog::getExistingDirectory( this
                                        , tr("Open Directory")
                                        , QDir::toNativeSeparators( defaultsavefilepath )
                                        , QFileDialog::ShowDirsOnly );

        if ( savefilepath.isNull() || savefilepath.isEmpty() ) return;

        //First we need to free the first instance that was created unless we want to leak the file handle...
        delete m_pdf_sig;
        printf( "(3) on_button_sign_clicked() - IF ( model->rowCount() > 1 )\n" );

        m_pdf_sig = new PTEID_PDFSignature();

        for(int i = 0; i < model->rowCount(); i++ ){
            QString tmp = model->data(model->index(i, 0), 0).toString();
            char *final = strdup( getPlatformNativeString(tmp) );
            if ( final != NULL ){
                m_pdf_sig->addToBatchSigning( final, ui.radioButton_lastpage->isChecked() );
                free(final);
            }/* if ( final != NULL ) */
        }/* for(int i) */
    } else{
        QString basename = my_file_info.completeBaseName();
        savefilepath = QFileDialog::getSaveFileName( this
                            , tr( "Save File" )
                            , QDir::toNativeSeparators( defaultsavefilepath + "/" + basename + "_signed.pdf" )
                            , tr( "PDF files (*.pdf)" ) );
        printf( "(4) on_button_sign_clicked() - ELSE ( model->rowCount() > 1 )\n" );
    }/* if ( model->rowCount() > 1 ) */

    if ( ui.timestamp_checkBox->isChecked() ) m_pdf_sig->enableTimestamp();
    if ( ui.smallsig_checkBox->isChecked() ) m_pdf_sig->enableSmallSignatureFormat();

    //Generate a scaled JPEG and pass the byte array to the Signature class
    if ( !m_custom_image.isNull() ){
        QBuffer buffer( &m_jpeg_scaled_data );
        buffer.open( QIODevice::WriteOnly );

        //Save the generated image as high-quality JPEG data
        QImage generated_img = image_canvas->drawToImage();
        generated_img.save(&buffer, "JPG", 100);

        //generated_img.save("/tmp/generated.jpg", "JPG", 100);
        m_pdf_sig->setCustomImage( (unsigned char *)m_jpeg_scaled_data.data()
                                   , m_jpeg_scaled_data.size() );
    }/* if ( !m_custom_image.isNull() ) */

    //Avoid overwriting the original PDF, it doesn't work currently in Windows
    if ( savefilepath == current_input_path ){
        ShowErrorMsgBox(tr("The signed PDF filename must be different from the original!"));
		return;
    }/* if ( savefilepath == current_input_path ) */

    if ( savefilepath.isNull() || savefilepath.isEmpty() ) return;

	char *reason = NULL, *location = NULL;
    if ( ui.location_textbox->isEnabled()
            && ui.location_textbox->text().size() > 0 ){
        location = strdup( ui.location_textbox->text().toUtf8().data() );
    }/* if ( ui.location_textbox->isEnabled() .. ) */

    if ( ui.reason_textbox->isEnabled()
            && ui.reason_textbox->text().size() > 0 ){
        reason = strdup( ui.reason_textbox->text().toUtf8().data() );
    }/* if ( ui.reason_textbox->isEnabled() ... ) */

	//Single File Signature case
	pdialog = new QProgressDialog();
    pdialog->setWindowModality( Qt::WindowModal );
    pdialog->setWindowTitle(tr( "PDF Signature" ) );
    pdialog->setLabelText( tr( "Signing PDF file(s)..." ) );

	//Set windows flags
	Qt::WindowFlags flags = pdialog->windowFlags();
	flags &= (~Qt::WindowMinMaxButtonsHint);// remove Min & Max Buttons
	flags &= (~Qt::WindowCloseButtonHint);  // remove close Button
	pdialog->setWindowFlags( flags );

	//Set fixed size window
    pdialog->setFixedSize( pdialog->size() );

    //Set icon
    pdialog->setWindowIcon( QIcon(":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png") );

    pdialog->setMinimum( 0 );
    pdialog->setMaximum( 0 );

    dlgCmdUserInfo diagCmdUserInfo( this );
    diagCmdUserInfo.exec();
    m_userId = diagCmdUserInfo.getUserId();
    if ( m_userId.isEmpty() ){
        PTEID_LOG( PTEID_LOG_LEVEL_ERROR, "eidgui", "Empty user ID" );
        ShowErrorMsgBox( tr("User ID should NOT be empty!" ) );
        return;
    }/* if ( m_userId.isEmpty() ) */

    printf( "diagCmdUserInfo.getUserId() - OK\n" );

    m_userPin = diagCmdUserInfo.getUserPin();
    if ( m_userPin.isEmpty() ){
        PTEID_LOG( PTEID_LOG_LEVEL_ERROR, "eidgui", "Empty user PIN" );
        ShowErrorMsgBox( tr("User PIN should NOT be empty!" ) );
        return;
    }/* if ( m_userPin.isEmpty() ) */

    printf( "diagCmdUserInfo.getUserPin() - OK\n" );

    connect( &this->FutureWatcher, SIGNAL(finished()), pdialog, SLOT(cancel() ) );

    QFuture<void> future_signOpen = QtConcurrent::run( this
                                            , &PDFSignWindow::run_signOpen
                                            , selected_page
                                            , savefilepath
                                            , location, reason );

    this->FutureWatcher.setFuture( future_signOpen );
	pdialog->reset();
	pdialog->exec();

    if ( this->success == SIG_SUCCESS ){

        dlgCmdCode diagCmdCode( this );
        diagCmdCode.exec();
        m_receivedCode = diagCmdCode.getReceivedCode();
        if ( m_receivedCode.isEmpty() ){
            PTEID_LOG( PTEID_LOG_LEVEL_ERROR, "eidgui", "Empty received Code" );
            ShowErrorMsgBox( tr("Received Code should NOT be empty!" ) );
            return;
        }/* if ( m_receivedCode.isEmpty() ) */

        QFuture<void> future_signClose = QtConcurrent::run( this
                                                        , &PDFSignWindow::run_signClose );

        this->FutureWatcher.setFuture( future_signClose );
        pdialog->reset();
        pdialog->exec();

        if ( SIG_SUCCESS == this->success ){
            ShowSuccessMsgBox();
        } else if ( TS_WARNING == this->success ){
            QString sig_detail = model->rowCount() > 1
                    ?  tr( "some of the timestamps could not be applied" )
                    : tr( "the timestamp could not be applied" );
            ShowErrorMsgBox( tr("Signature(s) successfully generated but " ) + sig_detail );
        } else{
                ShowErrorMsgBox(tr("Error Generating Signature!"));
        }

    } else if ( this->success == TS_WARNING ){
        QString sig_detail = model->rowCount() > 1
                ?  tr( "some of the timestamps could not be applied" )
                : tr( "the timestamp could not be applied" );
        ShowErrorMsgBox( tr("Signature(s) successfully generated but " ) + sig_detail );
    } else{
		ShowErrorMsgBox(tr("Error Generating Signature!"));
    }/* if ( this->success == SIG_SUCCESS ) */

    if ( SIG_SUCCESS == this->success ) this->close();
    if ( location != NULL ) free( location );
    if ( reason != NULL ) free( reason );

}/* PDFSignWindow::on_button_sign_clicked() */

void PDFSignWindow::on_button_addfile_clicked(){
	QStringList fileselect;
	QString defaultopenfilepath;

	defaultopenfilepath = QDir::homePath();
    fileselect = QFileDialog::getOpenFileNames( this
                                                , tr( "Select File(s)" )
                                                , defaultopenfilepath
                                                , "PDF Documents (*.pdf)" );
	QCoreApplication::processEvents();

	addFileToListView(fileselect);
}/* PDFSignWindow::on_button_addfile_clicked() */

double g_scene_height;
double g_scene_width;

QPixmap PDFSignWindow::renderPageToImage(){
	// Document starts at page 0 in the poppler-qt5 API
	Poppler::Page *popplerPage = m_doc->page(m_current_page_number-1);

	//TODO: Test the resolution on Windows
    const double resX = 45.0;
    const double resY = 45.0;
    if ( popplerPage == NULL ){
    	qDebug() << "Failed to get page object: " << m_current_page_number-1;
    	return QPixmap();
    }/* if ( popplerPage == NULL ) */

    QImage image = popplerPage->renderToImage( resX
                                               , resY
                                               , -1, -1, -1, -1
                                               , Poppler::Page::Rotate0 );
    delete popplerPage;

    if ( !image.isNull() ){
    	return QPixmap::fromImage(image);
    } else{
    	qDebug() << "Error rendering PDF page to image!";
       return QPixmap();
    }/* if ( !image.isNull() ) */
}/* PDFSignWindow::renderPageToImage() */

void PDFSignWindow::buildLocationTab(){
	//These dimensions respect A4 page ratio
    g_scene_height = 520;
    g_scene_width = 367.75;

    preview_pixmap = renderPageToImage();
    if ( my_scene == NULL ){
        my_scene = new MyGraphicsScene( preview_pixmap );
    } else{
        my_scene->updateBackground( preview_pixmap );
    }/* if ( my_scene == NULL ) */

    double rect_h = 0.1 * g_scene_height;
	// This ratio for signature width is based on the following formula: (Page_W - 60) / 3
	double rect_w = 0.29715 * g_scene_width;

    if ( m_landscape_mode ){
        double tmp = g_scene_width;
        g_scene_width = g_scene_height;
        g_scene_height = tmp;
    }/* if ( m_landscape_mode ) */

    this->rx = 0;
    this->ry = 0;

    my_scene->setSceneRect( 0, 0, g_scene_width, g_scene_height );
    my_scene->setItemIndexMethod( QGraphicsScene::NoIndex );

    QGraphicsView *view = ui.scene_view;
    view->setFixedSize( g_scene_width, g_scene_height );
    view->setScene( my_scene );
    view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    my_scene->setParent( this );
    if ( my_rectangle == NULL ){
        my_rectangle = new DraggableRectangle( my_scene
                                            , g_scene_height, g_scene_width
                                            , rect_h, rect_w );
        my_scene->addItem( my_rectangle );
    } else{
        my_rectangle->setMaxLocation( g_scene_width, g_scene_height );
		my_rectangle->show();
    }/* if ( my_rectangle == NULL ) */

	my_scene->update();

    //Pre-select visible signature if we added a valid PDF file
    ui.visible_checkBox->setCheckState( Qt::Checked );
    view->setEnabled( true );
}/* PDFSignWindow::buildLocationTab() */

//TODO: Rendering should not be done in event handlers, because it blocks the GUI thread...

void PDFSignWindow::on_radioButton_firstpage_toggled( bool value ){
    if ( value && m_pdf_sig ){
		// clearAllSectors();
		m_current_page_number = 1;

        preview_pixmap = renderPageToImage();

        my_scene->updateBackground( preview_pixmap );
        my_scene->update();
    }/* if ( value && m_pdf_sig ) */
}/* PDFSignWindow::on_radioButton_firstpage_toggled() */

void PDFSignWindow::on_radioButton_lastpage_toggled( bool value ){
    if ( value && m_pdf_sig ){
         //clearAllSectors();
         m_current_page_number = m_pageCount;
         preview_pixmap = renderPageToImage();

         my_scene->updateBackground( preview_pixmap );
         my_scene->update();

         QString sectors =
            QString::fromLatin1(m_pdf_sig->getOccupiedSectors( m_current_page_number ) );
    }/* if ( value && m_pdf_sig ) */
}/* PDFSignWindow::on_radioButton_lastpage_toggled() */

void PDFSignWindow::on_spinBox_page_valueChanged( int new_value ){
	//Check if there is at least 1 PDF chosen
    if ( !m_pdf_sig ) return;

	m_current_page_number = new_value;
	preview_pixmap = renderPageToImage();
    my_scene->updateBackground( preview_pixmap );
	my_scene->update();
    // clearAllSectors();

    QString sectors = QString::fromLatin1( m_pdf_sig->getOccupiedSectors( new_value ) );
}/* PDFSignWindow::on_spinBox_page_valueChanged() */

void mapSectorToRC( int sector, int *row, int *column ){
    if ( sector < 1 ) return;

	int rem = sector % 3;

	*row = (sector-1) / 3;

    switch( rem ){
        case 0:
            *column = 2;
            break;

        case 1:
            *column = 0;
            break;

        case 2:
            *column = 1;
            break;
    }/* switch( rem ) */
}/* mapSectorToRC( int sector, int *row, int *column ) */

void MyGraphicsScene::itemMoved( QPointF newPos ){
    if ( parent ){
        ((PDFSignWindow*)parent)->setPosition( newPos );
    }
}/* MyGraphicsScene::itemMoved() */

void MyGraphicsScene::drawBackground(QPainter * painter, const QRectF & rect ){

    painter->drawPixmap( 0, 0, background );
    if ( m_greyedOut ){
		//Draw grey rectangle with alpha = 100
        painter->fillRect(QRect( 0, 0, width(), height() )
                          , QColor( 190, 190, 190, 100 ) );
    }/* if ( m_greyedOut ) */
}/* MyGraphicsScene::drawBackground() */

void DraggableRectangle::paint( QPainter *painter
                                , const QStyleOptionGraphicsItem *
                                , QWidget * ){
    painter->setPen( Qt::black );
    painter->setPen( Qt::DotLine );

    painter->drawRect( 0, 0, m_rect_w, m_rect_h );

    painter->drawPixmap( 0, 0, *current_logo );
}/* DraggableRectangle::paint() */

void DraggableRectangle::makeItSmaller(){
	current_logo = &logo_pixmap_small;
	m_rect_h /= 2;
}/* DraggableRectangle::makeItSmaller() */

void DraggableRectangle::makeItLarger(){
	m_rect_h *= 2;
	current_logo = &logo_pixmap;
}/* DraggableRectangle::makeItLarger() */

QRectF DraggableRectangle::boundingRect() const{
    return QRectF( 0, 0, m_rect_w + 1, m_rect_h + 1 );
}/* DraggableRectangle::boundingRect() */

/* Change mouse cursor to indicate draggable rectangle */
void DraggableRectangle::hoverEnterEvent( QGraphicsSceneHoverEvent * event ){
    QApplication::setOverrideCursor( QCursor(Qt::OpenHandCursor) );
	update();
}/* DraggableRectangle::hoverEnterEvent() */

void DraggableRectangle::hoverLeaveEvent( QGraphicsSceneHoverEvent * event ){
    QApplication::setOverrideCursor( QCursor( Qt::ArrowCursor ) );
	update();
}/* DraggableRectangle::hoverLeaveEvent() */

void DraggableRectangle::mousePressEvent(QGraphicsSceneMouseEvent * event){

    QApplication::setOverrideCursor( QCursor( Qt::ClosedHandCursor ) );
}/* DraggableRectangle::mousePressEvent() */

void DraggableRectangle::setToPos( QPointF *newPos ){
	/* Detect out of boundaries mouseEvent
	and correct the point to the nearest valid position */
    if ( newPos->rx() < 0 ) newPos->setX( 0 );
    if ( newPos->ry() < 0 ) newPos->setY( 0 );
    if ( newPos->rx() >= m_max_x - m_rect_w ) newPos->setX( m_max_x - m_rect_w );
    if ( newPos->ry() >= m_max_y - m_rect_h ) newPos->setY( m_max_y - m_rect_h );

    setPos( *newPos );
}/* DraggableRectangle::setToPos() */

void DraggableRectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
	QPointF sig_location = scenePos();
    setToPos( &sig_location );

	//Send message to parent with a corrected position
    my_scene->itemMoved( sig_location );
    QApplication::setOverrideCursor( QCursor( Qt::OpenHandCursor ) );
    QGraphicsItem::mouseReleaseEvent( event );
}/* DraggableRectangle::mouseReleaseEvent() */

#ifdef OLD_SIGNATURE_LOCATION
QRectF SelectableRectangle::boundingRect() const
{
    return QRectF( 0, 0, m_rect_w, m_rect_h );
}/* SelectableRectangle::boundingRect() */

void SelectableRectangle::resetColor(){
    m_brush = QBrush( QColor( 193, 221, 221 ) );
	update();
}/* SelectableRectangle::resetColor() */

void SelectableRectangle::setSectorFilled( Qt::GlobalColor color ){
    m_brush = QBrush( QColor( color ) );
    is_sector_filled = true;
    update();
}/* SelectableRectangle::setSectorFilled() */

void SelectableRectangle::mousePressEvent( QGraphicsSceneMouseEvent * event ){
}/* SelectableRectangle::mousePressEvent() */

void SelectableRectangle::mouseReleaseEvent( QGraphicsSceneMouseEvent * event ){
    //Ignore the event if we are in Free Selection mode
    if ( dont_select_mode || is_sector_filled ) return;

    my_scene->clearAllRectangles( true );

    m_brush = QBrush( Qt::black );
    my_scene->selectNewRectangle( pos() );
	update();
}/* SelectableRectangle::mouseReleaseEvent() */

void SelectableRectangle::paint( QPainter *painter
                                 , const QStyleOptionGraphicsItem *
                                 , QWidget *){
    painter->fillRect( 0, 0, m_rect_w, m_rect_h, m_brush );
    painter->setPen( Qt::black );
    painter->drawRect( 0, 0, m_rect_w, m_rect_h );


    painter->setBrush( Qt::lightGray );

    if ( !dont_select_mode ){
		//Draw text centered in the rectangle
        QFont sansSerifFont( "Arial", 16, QFont::Bold );
        painter->setPen( Qt::lightGray );
        painter->setFont( sansSerifFont );

        painter->drawText( boundingRect(), Qt::AlignCenter, m_string );
    }/* if ( !dont_select_mode ) */
}/* SelectableRectangle::paint() */
#endif /* OLD_SIGNATURE_LOCATION */

double PDFSignWindow::getMaxX(){
	return  g_scene_width;
}/* PDFSignWindow::getMaxX() */

double PDFSignWindow::getMaxY(){
    return g_scene_height;
}/* PDFSignWindow::getMaxY() */

void PDFSignWindow::closeDocument(){
    if ( !m_doc ) return;

    delete m_doc;
    m_doc = 0;
}/* PDFSignWindow::closeDocument() */

/* Load document with poppler-qt5 for preview */
void PDFSignWindow::loadDocumentForPreview( const QString &file ){
    Poppler::Document *newdoc = Poppler::Document::load( file );
    if ( !newdoc ){
        QMessageBox msgbox( QMessageBox::Critical
                            , tr( "Open Error" )
                            , tr( "Cannot open:\n") + file
                            , QMessageBox::Ok
                            , this );
        msgbox.exec();
        return;
    }/* if ( !newdoc ) */

    //Close a previous document
    closeDocument();

    m_doc = newdoc;

    m_doc->setRenderHint( Poppler::Document::TextAntialiasing, true );
    m_doc->setRenderHint( Poppler::Document::Antialiasing, true );
    m_doc->setRenderBackend( Poppler::Document::SplashBackend );
}/* PDFSignWindow::loadDocumentForPreview() */

/* Coordinate conversion */
void PDFSignWindow::setPosition( QPointF new_pos ){

    double scaled_rectangle_height = m_landscape_mode
                    ? 0.16 * g_scene_height
                    : 0.1069 * g_scene_height;

    /* Check border limits */
    if ( new_pos.rx() < 0 ){
        this->rx = 0;
	} else if ( new_pos.rx() > getMaxX() ){
        this->rx = getMaxX();
	} else {
        this->rx = new_pos.rx();
    }/* !if ( new_pos.rx() < 0 ) */

    if ( new_pos.ry() < 0 ){
        this->ry = 0;
	} else if ( new_pos.ry() > getMaxY()) {
        this->ry = getMaxY();
	} else {
        this->ry = new_pos.ry();
    }/* !if ( new_pos.ry() < 0 ) */

    //Actual coordinates passed to SignPDF() expressed as a fraction
    sig_coord_x = this->rx/g_scene_width;

    //printf("eidgui: DEBUG: this->ry: %f\n", this->ry);
    //Vertical coordinate needs the rectangle height offset because new_pos contains the top-left corner
    sig_coord_y = ( this->ry + scaled_rectangle_height) / g_scene_height;
    qDebug() << "New coordinates=  x: " <<sig_coord_x << "y: " << sig_coord_y;
}/* PDFSignWindow::setPosition() */

void PDFSignWindow::on_button_clearFiles_clicked(){
    list_model->removeRows( 0, list_model->rowCount() );

    if ( m_pdf_sig != NULL ){
		delete m_pdf_sig;
		m_pdf_sig = NULL;
    }/* if ( m_pdf_sig != NULL ) */

	page_numbers.clear();

    ui.button_sign->setEnabled( false );
    ui.visible_checkBox->setEnabled( false );

    ui.timestamp_checkBox->setEnabled( false );
    ui.checkBox_location->setEnabled( false );
    ui.checkBox_reason->setEnabled( false );

    ui.scene_view->setEnabled( false );
    if ( my_scene ){
		//Clear the background pixmap
		QPixmap null_pixmap;
        my_scene->updateBackground( null_pixmap );

        if ( my_rectangle ) my_rectangle->hide();

		my_scene->update();
    }/* if ( my_scene ) */
}/* PDFSignWindow::on_button_clearFiles_clicked() */

void PDFSignWindow::updateMaxPage( int removed_index ){
    page_numbers.removeAt( removed_index );

    if ( page_numbers.size() > 0 )
        ui.spinBox_page->setMaximum( *std::min_element( page_numbers.begin()
                                                      , page_numbers.end() ) );
}/* PDFSignWindow::updateMaxPage() */

void PDFSignWindow::addFileToListView( QStringList &str ){
    if ( str.isEmpty() ) return;

	//Remove duplicates in all the selected files
    for( int i = 0; i < list_model->rowCount(); i++ ){
        QString tmp = list_model->data( list_model->index(i, 0), 0).toString();
        str.removeAll( tmp );
    }/* for( int i) */

	//Check again because we may have removed all elements of the input list
    if ( str.isEmpty() ) return;

    if ( m_pdf_sig != NULL ) delete m_pdf_sig;

	m_pdf_sig = NULL;

	int j = 0;
	int tmp_count = 0;

    while ( !m_pdf_sig && j < str.size() ){
        current_input_path = str.at( j );
        m_pdf_sig = new PTEID_PDFSignature( strdup( getPlatformNativeString( current_input_path ) ) );

        printf("(1) addFileToListView() - while ( !m_pdf_sig && j < str.size() )\n");

		tmp_count = m_pdf_sig->getPageCount();

        if ( tmp_count < 1 ){
			delete m_pdf_sig;
			m_pdf_sig = NULL;

            QFileInfo fi( current_input_path );
            ShowErrorMsgBox( tr("Unsupported or damaged PDF file: ") + fi.fileName() );
            if ( str.size() == 1 ) return;
        }/* if ( tmp_count < 1 ) */
		j++;
    }/* while ( !m_pdf_sig && j < str.size() ) */

	//All the files were broken
    if ( !m_pdf_sig ) return;

    list_model->insertRows( list_model->rowCount(), 1 );
    list_model->setData( list_model->index( list_model->rowCount()-1, 0 )
                         , current_input_path );

    page_numbers.append( tmp_count );
	m_pageCount = tmp_count;
    m_landscape_mode = m_pdf_sig->isLandscapeFormat();

    int page_n = 0;
    for ( int i = j; i < str.size(); i++ ){
        QString tmp = str.at( i );

		//The first file was already checked using getPageCount()
        if ( i > 0 ){
            char *dupStr = strdup( getPlatformNativeString( tmp ));
            if ( dupStr != NULL ){
                page_n = m_pdf_sig->getOtherPageCount( dupStr );

				//Check for invalid file or unsupported PDF
                if ( page_n < 1 ){
                    ShowErrorMsgBox( tr("Unsupported or damaged PDF file: ") + tmp );

                    free( dupStr );
					continue;
                } else{
                    list_model->insertRows( list_model->rowCount(), 1 );
                    list_model->setData( list_model->index(list_model->rowCount()-1, 0 ), tmp );
                    page_numbers.append( page_n );
                }/* !if ( page_n < 1 ) */

                free( dupStr );
            }/* if ( dupStr != NULL ) */
        }/* if ( i > 0 ) */
    }/* for ( int i ) */

	//Set the spinbox with the appropriate max value
    ui.spinBox_page->setMaximum( *std::min_element( page_numbers.begin()
                                                   , page_numbers.end() ) );

    loadDocumentForPreview( current_input_path );
	m_current_page_number = 1;

	//Enable sign button now that we have data and a card inserted
    if ( !str.isEmpty() && this->card_present ){
        ui.button_sign->setEnabled( true );
        ui.visible_checkBox->setEnabled( true );

        ui.timestamp_checkBox->setEnabled( true );
        ui.checkBox_location->setEnabled( true );
        ui.checkBox_reason->setEnabled( true );

        ui.button_clearFiles->setEnabled( true );
    }/* if ( !str.isEmpty() && this->card_present ) */

    buildLocationTab();
}/* PDFSignWindow::addFileToListView() */
