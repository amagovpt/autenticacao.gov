/* ****************************************************************************

 * PTEID Middleware Project.
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
#include <QDialog>
#include <QListView>
#include <QComboBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QMessageBox>
#include <QtConcurrent>
#include <QProgressDialog>
#include <QTextStream>
#include <QPainter>
#include <QStyleOption>
#include <QPen>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QFileInfo>
#include <QDateTime>
//For std::min_element()
#include <algorithm>

#include "eidlib.h"
#include "eidlibException.h"
#include "eidErrors.h"
#include "scapsignature.h"
#include "mylistview.h"
#include "attributesuppliers.h"
#include "readerandcardstate.h"
#include "treeiteminfo.h"
#include "ASService/soapH.h"
#include "ASService/soapAttributeSupplierBindingProxy.h"
#include "ASService/AttributeSupplierBinding.nsmap"
#include "acserviceclient.h"
#include "pdfsignatureclient.h"
#include "settings.h"
#include "dlgUtils.h"

#include <openssl/x509.h>

using namespace eIDMW;

ScapSignature::ScapSignature(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::ScapSignature), m_selected_reader(0), m_selected_sector(0), card_present(true), my_scene(NULL)
{

    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    flags ^= Qt::WindowMaximizeButtonHint;
    //Remove the Context Help Window button
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags( flags );

    // Initialize variables
    m_pdf_sig = NULL;
    table_lines = 6;
    table_columns = 3;
    sig_coord_x = -1, sig_coord_y = -1;
    success = SIG_ERROR;
    horizontal_page_flag = false;
    m_small_signature = false;

    ui->spinBox_page->setValue(1);
    list_model = new QStringListModel();
    ui->pdf_listview->setModel(list_model);
    ui->pdf_listview->enableNotify();

    connect(ui->pdf_listview, SIGNAL(itemRemoved(int)), this, SLOT(updateMaxPage(int)));

    /* ------ Layout parameters ---------------*/
    this->setFixedSize(this->width(), this->height());

    // Hide advance options
    ui->advancedOptionsLayout->setVisible(false);

    // Add a status bar with progress indicator
    statusLabel = new QLabel(this);
    statusProgressBar = new QProgressBar(this);
    // set text for the label
    statusLabel->setText("");
    statusLabel->setContentsMargins(5,0,30,0);

    // make progress bar text invisible
    statusProgressBar->setTextVisible(false);
    statusProgressBar->setMaximumHeight(statusProgressBar->height() - 15);
    statusProgressBar->setMaximumWidth(200);

    // add the two controls to the status bar
    ui->statusBar->addWidget(statusLabel);
    ui->statusBar->addWidget(statusProgressBar,1);

    ui->attributesList->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->attributesList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->attributesList->header()->setStretchLastSection(false);

    initReaderAndCard();

    connect(this, SIGNAL(loadedProfessionalAttributes()), this, SLOT(loadProfessionalAttributes()));
}

void ScapSignature::initReaderAndCard()
{
    eIDMW::PTEID_InitSDK();

    //Show initial status
    onCardStatusChanged(CardEvent::ET_NO_READER);

    // Creates a new class and moves it to a thread. This is required to detect card reader connection and if wasn't connected when program launched
    QThread *readerAndCardThread = new QThread();
    ReaderAndCardState *rcState = new ReaderAndCardState();
    rcState->moveToThread(readerAndCardThread);

    connect(readerAndCardThread, SIGNAL(started()), rcState, SLOT(doWork()));

    qRegisterMetaType<CardEvent::eCustomEventType>("CardEvent::eCustomEventType");
    connect(rcState, SIGNAL(cardStatusChanged(CardEvent::eCustomEventType)), this, SLOT(onCardStatusChanged(CardEvent::eCustomEventType)));
    connect(rcState, SIGNAL(error(QString)), this, SLOT(ShowErrorMsgBox(QString)));

    readerAndCardThread->start();
}

#define ROWCOUNT_LANDSCAPE 5
#define COLUMNCOUNT_LANDSCAPE 4
#define ROWCOUNT_PORTRAIT 6
#define COLUMNCOUNT_PORTRAIT 3

ScapSignature::~ScapSignature()
{
    eIDMW::PTEID_ReleaseSDK();

    delete list_model;

    if (m_pdf_sig)
        delete m_pdf_sig;
}


void ScapSignature::onCardStatusChanged(CardEvent::eCustomEventType cardEvent)
{
    std::cout << "Received card event on mainThread: " << cardEvent << std::endl << std::flush;
    bool cardInserted = false;
    switch(cardEvent){
        case CardEvent::ET_CARD_INSERTED:
            statusProgressBar->hide();
            statusLabel->setText(tr("Card inserted"));
            cardInserted = true;

            // Try to load card cache
            m_attributesType = ACServiceClient::loadAttributesFromCache();
            if( m_attributesType.size() > 0 )
            {
                emit loadedProfessionalAttributes();
            }

            break;
        case CardEvent::ET_CARD_REMOVED:
            statusProgressBar->hide();
            statusLabel->setText(tr("Card removed"));

            // Clean card related vars
            ui->attributesList->clear();
            m_selectedSuppliersList.clear();
            m_attributesType.clear();
            break;
        case CardEvent::ET_NO_CARD:
            statusProgressBar->hide();
            statusLabel->setText(tr("No card found"));
            break;
        case CardEvent::ET_NO_READER:
            statusProgressBar->setMaximum(statusProgressBar->minimum());
            statusProgressBar->show();
            statusLabel->setText(tr("Searching for card reader"));
            break;
        case CardEvent::ET_UNKNOWN:
            break;
    }
    ui->btn_reloadAatributes->setEnabled(cardInserted);
    ui->button_sign->setEnabled(cardInserted && list_model->rowCount() > 0);
}

//Event received from myListView
void ScapSignature::customEvent(QEvent *ev)
{
    //if (ev->type() == QEvent::User)
        //ui->button_sign->setEnabled(false);
}

void ScapSignature::enableSignButton()
{
    this->card_present = true;
}

void ScapSignature::disableSignButton()
{

    ui->button_sign->setEnabled(false);
    this->card_present = false;
}


void ImageCanvas::initDateString()
{
    QDateTime now = QDateTime::currentDateTime();

    this->date_str = now.toString("yyyy.MM.dd hh:mm:ss");

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

    //QPixmap pixmap2(":/images/Images/Icons/eIDMW::PTEID_signature_small.png");
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

    if (!this->previewVisible)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPixmap pixmap2(":/images/Images/Icons/eIDMW::PTEID_signature_watermark.jpg");
    QPixmap default_icon(":/images/Images/Icons/eIDMW::PTEID_signature_banner.jpg");

    QFont current_font = painter.font();
#ifdef __APPLE__
    current_font.setPointSize(10);
#else
    current_font.setPointSize(9);
#endif

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

    QRect citizen_name_rect = fontMetrics().boundingRect(citizen_name);

    int available_width = this->width()-40 - bounding_rect.width()-5;

    //Check if 2 lines are needed for longer names
    if (citizen_name_rect.width() > available_width)
    {
        //Add words one by one until we run out of space
        QStringList words = citizen_name.split(" ");
        QString tmp, first_line, second_line;
        for (int i = 0; i < words.size(); ++i)
        {
            if (i > 0)
                tmp += " ";
            QString new_string = tmp + words.at(i);
            if (fontMetrics().boundingRect(new_string).width() > available_width)
            {
                first_line = tmp;
                second_line = QStringList(words.mid(i)).join(" ");
                break;
            }
            else
            {
                tmp = new_string;
            }

        }
        painter.drawText(bounding_rect.width()+ 5, current_y_pos, first_line);

        current_y_pos += line_height + line_spacing;
        painter.drawText(5, current_y_pos, second_line);
    }
    else
    {
        painter.drawText(bounding_rect.width()+ 5, current_y_pos, citizen_name);
    }

    my_font.setBold(false);
    painter.setFont(my_font);

    current_y_pos += line_height + line_spacing;
    painter.drawText(5, current_y_pos, QString::fromUtf8("Num de Identificação Civil: ")+ citizen_nic);


    current_y_pos += line_height + line_spacing;
    painter.drawText(5, current_y_pos, "Data: " + this->date_str);

    if (!small_sig_format && !sig_location.isEmpty())
    {
        current_y_pos += line_height + line_spacing;
        painter.drawText(5, current_y_pos, "Local: " + sig_location);
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

void ScapSignature::on_button_cancel_clicked()
{
    this->close();

}

void ScapSignature::on_radioButton_choosepage_toggled(bool checked)
{
    ui->spinBox_page->setEnabled(checked);
}

void ScapSignature::ShowSuccessMsgBox()
{

    QString caption = tr("PDF Signature");
        QString msg = tr("Signature(s) succesfully generated");
    QMessageBox msgBoxp(QMessageBox::Information, caption, msg, 0, this);
    msgBoxp.exec();

}

void ScapSignature::ShowErrorMsgBox(QString msg)
{

    QString caption  = tr("Error");

    QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
    msgBoxp.exec();
}


const char * getSubjectSerialNumber(const PTEID_ByteArray &certificate)
{

    const unsigned char *data = certificate.GetBytes();

    X509 *x509 = d2i_X509(NULL, &data, certificate.Size());


    int bytes_req = X509_NAME_get_text_by_NID(X509_get_subject_name(x509), NID_serialNumber, NULL, 0) +1;

    if (bytes_req == -1)
        return NULL;

    const char * serial = (const char *)malloc(bytes_req*sizeof(unsigned char));

    int rc = X509_NAME_get_text_by_NID(X509_get_subject_name(x509), NID_serialNumber, (char *)serial, bytes_req);

    return serial;
}

void ScapSignature::run_sign(int selected_page, QString &savefilepath)
{
    // Sets user selected file save path
    const char* citizenId;

    // Creates a temporary file
    QTemporaryFile tempFile;

    if (!tempFile.open()) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "PDF Signature error: Error creating temporary file");
        return;
    }

    char *temp_save_path = strdup(tempFile.fileName().toStdString().c_str());
    int sign_rc = 0;

    try{
        eIDMW::PTEID_ReaderContext& readerContext = ReaderSet.getReader();
        if (!readerContext.isCardPresent())
        {
            std::cerr << "PDF Signature error: No card found in the reader!" << std::endl;
            this->success = SIG_ERROR;
        }

        try
        {
            eIDMW::PTEID_EIDCard &card = readerContext.getEIDCard();
            eIDMW::PTEID_Certificates &certs = card.getCertificates();

            // Get Citizen info
            QString citizenName = card.getID().getGivenName();
            citizenName.append(" ");
            citizenName.append(card.getID().getSurname());

            citizenId = getSubjectSerialNumber(certs.getAuthentication().getCertData()); //card.getID().getCivilianIdNumber();


            std::cout << "Sent PDF Signature landscape mode: " << m_landscape_mode << std::endl;

            if(m_selected_sector != 0)
            {
                if(m_landscape_mode)
                {
                    computeSigLocationFromSectorLandscape(m_selected_sector);
                }
                else
                {
                    computeSigLocationFromSector(m_selected_sector);
                }
            }

            std::cout << "Sent PDF Signature coordinates. X:" << sig_coord_x << " Y:" << sig_coord_y << std::endl;
            QString ltv(ui->ltvPicker->currentText());

            // Sign pdf
            sign_rc = card.SignPDF(*m_pdf_sig, selected_page, 0, m_landscape_mode, "" , "", temp_save_path);

            if (sign_rc == 0)
            {
                this->success = SIG_ERROR;
                bool successfull = PDFSignatureClient::signPDF(m_proxyInfo, savefilepath, QString(temp_save_path), QString(citizenName),
                    QString(citizenId), ltv.toInt(), PDFSignatureInfo(selected_page, sig_coord_x, sig_coord_y, m_landscape_mode), m_selected_attributesType);

                if( successfull ){
                    if ( this->FutureWatcher.future().isCanceled() ){
                        this->success = CANCELED_BY_USER;
                    } else{
                        this->success = SIG_SUCCESS;
                    }
                }
            }
            else
                this->success = TS_WARNING;
        }
        catch (eIDMW::PTEID_Exception &e)
        {
            std::cerr << "Caught exception getting EID Card. Error code: " << hex << e.GetError() << std::endl;
            this->success = SIG_ERROR;
        }
    }
    catch(eIDMW::PTEID_Exception &e){
        std::cerr << "Caught exception getting reader. Error code: " << hex << e.GetError() << std::endl;
        this->success = SIG_ERROR;
    }
    reloadPdfSig();
    free(temp_save_path);
    free((char *)citizenId);
}

// A4 values for portrait
const int pdf_height = 842;
const int pdf_width = 595;

const int sig_height = 90;

// Scene sizes
const double default_scene_height = 372;
const double default_scene_width = 262;
const double rectY = 30;
const double rectX = 75;
const int margin = 6;

// Max value for rectangle to go down inside scene
const int maxSceneY = default_scene_height - rectY - 2*margin;
const int maxSceneYLandscape = default_scene_width - rectY - 2*margin;

double g_scene_height;
double g_scene_width;
const int pdf_margin = 20;

void ScapSignature::computeSigLocationFromSectorLandscape(int sector)
{
    int page_width = pdf_height;
    int page_height = pdf_width;

    double vert_align = 6; //Add this to vertically center inside each cell
    //Number of columns for portrait layout
    int columns = 4;
    double signature_height = sig_height;
    int MAX_SECTOR = 20;
    const double n_lines = MAX_SECTOR / 4.0;
    int x1,x2,y1,y2;
    x1 = x2 = y1 = y2 = 0;

    double sig_width = (page_width - pdf_margin*2) / columns;

    //Add left margin
    x1 = pdf_margin;
    x2 = pdf_margin;

    if (sector < 1 || sector > MAX_SECTOR)
        fprintf (stderr, "Illegal value for signature page sector: %u Valid values [1-%d]\n",
                sector, MAX_SECTOR);

    if (sector < MAX_SECTOR - 3)
    {
        int line = sector / 4 + 1;
        if (sector % 4 == 0)
           line = sector / 4;

        y1 += (page_height - 2*pdf_margin) * (n_lines-line) / n_lines;
        y2 += (page_height - 2*pdf_margin) * (n_lines-line) / n_lines;
    }

    int column = (sector-1) % 4;

    x1 += (column * sig_width);
    x2 += (column * sig_width);

    y1 += margin + vert_align;

    //Define height and width of the rectangle
    x2 += sig_width;
    y2 += signature_height + pdf_margin + vert_align;


    //fprintf(stderr, "DEBUG: Sector: %02d Location = (%f, %f) (%f, %f) \n", sector, x1, y1, x2, y2);

    sig_coord_x = x1;
    sig_coord_y = y1;

    std::cout << "Sector to coord. X: " << sig_coord_x << " y:" << sig_coord_y << std::endl;
}

void ScapSignature::computeSigLocationFromSector(int sector)
{
    int page_width = pdf_width;
    int page_height = pdf_height;

    int MAX_SECTOR = 18;
    double signature_height = sig_height;
    const double n_lines = MAX_SECTOR / 3.0;
    // Add padding, adjust to subtly tweak the location
    // The units for x_pad, y_pad, sig_height and sig_width are postscript
    // points (1 px == 0.75 points)
    double vert_align = 16; //Add this to vertically center inside each cell
    //Number of columns for portrait layout
    int columns = 3.0;
    int x1,x2,y1,y2;
    x1 = x2 = y1 = y2 = 0;

    double sig_width = (page_width - pdf_margin*2) / columns;

    //Add left margin
    x1 = pdf_margin;
    x2 = pdf_margin;

    if (sector < 1 || sector > MAX_SECTOR)
        fprintf (stderr, "Illegal value for signature page sector: %u Valid values [1-%d]\n",
                sector, MAX_SECTOR);

    if (sector < MAX_SECTOR -2)
    {
        int line = sector / 3 + 1;
        if (sector % 3 == 0)
           line = sector / 3;

        y1 += (page_height - 2*pdf_margin) * (n_lines-line) / n_lines;
        y2 += (page_height - 2*pdf_margin) * (n_lines-line) / n_lines;
    }

    if (sector % 3 == 2 )
    {
        x1 += sig_width;
        x2 += sig_width;
    }

    if (sector % 3 == 0 )
    {
        x1 += sig_width * 2.0;
        x2 += sig_width * 2.0;
    }

    y1 += pdf_margin + vert_align;

    //Define height and width of the rectangle
    x2 += sig_width;
    y2 += signature_height + pdf_margin + vert_align;

    sig_coord_x = x1;
    sig_coord_y = y1;

    std::cout << "Sector to coord. X: " << sig_coord_x << " y:" << sig_coord_y << std::endl;

}


void ScapSignature::on_button_sign_clicked()
{
    updateSelectedAttributes();
    if( m_selected_attributesType.size() < 1 )
    {
        ShowErrorMsgBox(tr("You need to select at least 1 attribute to sign."));
        std::cout << "User tried to sign without selecting any attribute";
        return;
    }

    //For invisible sigs the implementation we'll
    //need to add a reference to the sigfield in some page so...
    int selected_page = 1;
    QString savefilepath;

    //Read Page
    //Validate if any location was chosen
//    if (sig_coord_x == -1 && m_selected_sector == 0)
//    {
//        ShowErrorMsgBox(tr("You must choose a location for visible signature!"));
//        return;
//    }
    selected_page = m_current_page_number;

    QStringListModel *model = dynamic_cast<QStringListModel *>(list_model);
    if (model->rowCount() == 0)
    {
       return;

    }
    QFileInfo my_file_info(current_input_path);
    QString defaultsavefilepath = my_file_info.dir().absolutePath();

    if (model->rowCount() > 1)
    {
        //First we need to free the first instance that was created unless we want to leak the file handle...
        delete m_pdf_sig;
        m_pdf_sig = new eIDMW::PTEID_PDFSignature();

        for (int i = 0; i < model->rowCount(); i++)
        {
            QString tmp = model->data(model->index(i, 0), 0).toString();
            char *final = strdup(getPlatformNativeString(tmp));
            m_pdf_sig->addToBatchSigning(final, ui->radioButton_lastpage->isChecked());

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

    //Avoid overwriting the original PDF, it doesn't work currently in Windows
    if (savefilepath == current_input_path)
    {
        ShowErrorMsgBox(tr("The signed PDF filename must be different from the original!"));
        return;
    }

    //Single File Signature case
    pdialog = new QProgressDialog();
    pdialog->setWindowIcon(QIcon(":/appicon/Images/pteid.ico"));
    pdialog->setWindowModality(Qt::WindowModal);
    pdialog->setWindowTitle(tr("PDF Signature"));
    pdialog->setLabelText(tr("Signing PDF file..."));
    pdialog->setMinimum(0);
    pdialog->setMaximum(0);

    connect(&this->FutureWatcher, SIGNAL(finished()), pdialog, SLOT(reset()));
    connect(pdialog, SIGNAL(canceled()), &this->FutureWatcher, SLOT(cancel()));

    // Start the computation.
    this->FutureWatcher.setFuture( QtConcurrent::run(this
                                            , &ScapSignature::run_sign
                                            , selected_page, savefilepath) );

    // Display the dialog
    pdialog->exec();

    this->FutureWatcher.waitForFinished();

    if ( this->success == SIG_SUCCESS ){
        std::cout << "PDF Signature completed successfully." << std::endl;
        ShowSuccessMsgBox();

    } else if ( this->success == TS_WARNING ){
        std::cout << "PDF Signature finished with timestamp errors." << std::endl;
        QString sig_detail = model->rowCount() > 1 ?  tr("some of the timestamps could not be applied") :
                tr("the timestamp could not be applied");
        ShowErrorMsgBox(tr("Signature(s) successfully generated but ")+ sig_detail);

    } else if ( this->success == CANCELED_BY_USER ){
        QFile::remove(savefilepath);
        std::cout << "Operation canceled by user - No PDF Signature generated" << std::endl;
        ShowErrorMsgBox(tr("Operation canceled by user"));

        eIDMW::PTEID_LOG( eIDMW::PTEID_LOG_LEVEL_ERROR
                        , "ScapSignature"
                        , "Operation canceled by user - No PDF Signature generated");

    } else {
        std::cout << "PDF Signature finished with errors." << std::endl;
        ShowErrorMsgBox(tr("Error Generating Signature"));
    }

}

// IN MB
const int MAX_FILE_SIZE = 15;

void ScapSignature::clearScapSign(){
    ui->button_addfile->setText(tr("&Add File"));
    page_numbers.clear();
    m_pageCount = 0;
    m_landscape_mode = false;

    ui->button_sign->setEnabled(false);
    ui->pushButton_freeselect->setEnabled(false);

    ui->radioButton_firstpage->setEnabled(false);
    ui->radioButton_lastpage->setEnabled(false);
    ui->radioButton_choosepage->setEnabled(false);
    ui->spinBox_page->setEnabled(false);

    ui->pushButton_freeselect->setText(tr("Free Selection"));
    ui->label_x->setText("");
    ui->label_y->setText("");

    my_scene->clear();
    ui->scene_view->setFixedSize(default_scene_width, default_scene_height);
}

void ScapSignature::on_button_addfile_clicked()
{
    if(list_model->rowCount() > 0){
        ui->button_addfile->setText(tr("&Add File"));
        list_model->removeRows( 0, list_model->rowCount() );
        clearScapSign();
    }else{
        QStringList fileselect;
        QString selectedFile;
        QString defaultopenfilepath;
        defaultopenfilepath = QDir::homePath();
        float fileSize = 0;

        selectedFile = QFileDialog::getOpenFileName(this, tr("Select File"), defaultopenfilepath, "PDF Documents (*.pdf)");
        QCoreApplication::processEvents();

        if(selectedFile.length() < 1)
        {
            std::cout << "User didn't select any file" << std::endl;
            return;
        }

        QFile *file = new QFile(selectedFile);
        fileSize = (float)file->size()/1048576;
        file->close();
        if(fileSize >= MAX_FILE_SIZE){
            ShowErrorMsgBox(tr("File size is too big. Max supported file size is %1 MB").arg(MAX_FILE_SIZE));
            return;
        }

        std::cout << "File Size: " <<  fileSize << std::endl << std::flush;

        // Keep the QStringList in order to ease future change to multiple pdf
        fileselect.append(selectedFile);


        addFileToListView(fileselect);
    }
}

void ScapSignature::buildLocationTab()
{
    g_scene_height = default_scene_height;
    g_scene_width = default_scene_width;
//  double rect_h = g_scene_height *0.106888361045; //Proportional to current signature height
//	double rect_w = g_scene_width / 3.0;

    if (my_scene)
        delete my_scene;

    MyGraphicsScene *scene = new MyGraphicsScene(tr("A4 Page"));

    my_rectangle = NULL;
    // m_landscape_mode = landscape_mode;

    if (m_landscape_mode)
    {
        double tmp = g_scene_width;
        g_scene_width = g_scene_height;
        g_scene_height = tmp;

        //ui->widget->setMinimumWidth(tmp);
    }

    this->rx = 0;
    this->ry = 0;

    // This line fixes scene following mouse
    scene->setSceneRect(0, 0, g_scene_width, g_scene_height);

    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    my_scene = scene;
    QGraphicsView *view = ui->scene_view;
    view->setFixedSize(g_scene_width, g_scene_height);
    view->setScene(scene);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    view->setToolTip(tr("Choose the page sector where you want your signature to appear.\n"
                "The grey sectors are already filled with other signatures."));
    // view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    addSquares();

    my_scene->setParent(this);
    // fprintf(stderr, "FreeSelectionDialog Width: %d, Height: %d\n", ui->widget->width(), ui->widget->height());

    //Add room for margins, the textboxes and buttons below the widget
    // this->setFixedSize(ui->widget->width() + 50, ui->widget->height() + 70);

}

void ScapSignature::on_radioButton_firstpage_toggled(bool value)
{
    if (value && m_pdf_sig)
    {
         m_current_page_number = 1;
         QString sectors = QString::fromLatin1(m_pdf_sig->getOccupiedSectors(1));
         highlightSectors(sectors);

    }

}

void ScapSignature::on_radioButton_lastpage_toggled(bool value)
{
    if (value && m_pdf_sig)
    {
        m_current_page_number = m_pageCount;
         QString sectors =
			 QString::fromLatin1(m_pdf_sig->getOccupiedSectors(m_pageCount));
         highlightSectors(sectors);
    }

}

void ScapSignature::on_spinBox_page_valueChanged(int new_value)
{
    //Check if there is at least 1 PDF chosen
    if (!m_pdf_sig)
        return;
     clearAllSectors();
     m_current_page_number = new_value;
	 QString sectors = QString::fromLatin1(m_pdf_sig->getOccupiedSectors(new_value));
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
        ((ScapSignature*)parent)->setPosition(newPos);

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
	QGraphicsItem *it = this->itemAt(selected_pos, QTransform());
    ((ScapSignature*)parent)->setSelectedSector(((SelectableRectangle *)it)->getSectorNumber());
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

void DraggableRectangle::paint(QPainter *painter,
        const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::black);
    painter->setBrush(Qt::lightGray);
    painter->drawRect(0, 0, m_rect_w, m_rect_h);
}

void DraggableRectangle::makeItSmaller()
{
    m_rect_h /= 2;
}

void DraggableRectangle::makeItLarger()
{
    m_rect_h *= 2;
}

QRectF DraggableRectangle::boundingRect() const
{
    return QRectF(0, 0, m_rect_w+1, m_rect_h+1);
}


/* Change mouse cursor to indicate draggable rectangle
 */
void DraggableRectangle::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    //setCursor(Qt::OpenHandCursor);
    QApplication::setOverrideCursor(QCursor(Qt::OpenHandCursor));
    update();
}

void DraggableRectangle::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    update();
}

void DraggableRectangle::mousePressEvent (QGraphicsSceneMouseEvent * event)
{

    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));

}

void DraggableRectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
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

/* Coordinate conversion */
void ScapSignature::setPosition(QPointF new_pos)
{
    this->rx = new_pos.rx()-margin;
    this->ry = new_pos.ry()-margin;

    //Actual coordinates passed to SignPDF() expressed final PDF size
    if(m_landscape_mode)
    {
        sig_coord_x = (this->rx * pdf_height) / default_scene_height;
        sig_coord_y = ((maxSceneYLandscape - this->ry) * pdf_width) / default_scene_width;
    }
    else
    {
        sig_coord_x = (this->rx * pdf_width) / default_scene_width;
        sig_coord_y = ((maxSceneY - this->ry) * pdf_height) / default_scene_height;
    }

    ui->label_x->setText(tr("Horizontal position: %1")
        .arg(QString::number(sig_coord_x, 'f', 1)));

    ui->label_y->setText(tr("Vertical position: %1")
        .arg(QString::number(sig_coord_y, 'f', 1)));

}

void ScapSignature::setSelectedSector(int sector)
{
    m_selected_sector = sector;
    ui->label_x->setText(tr("Selected sector: %1").arg(QString::number(sector)));
    ui->label_y->setText("");
}

void ScapSignature::on_pushButton_freeselect_clicked()
{

    if (my_scene->isFreeSelectMode())
    {
        ui->pushButton_freeselect->setText(tr("Free Selection"));
        if (my_rectangle)
            my_rectangle->hide();

    }
    else
    {
        ui->pushButton_freeselect->setText(tr("Show sectors"));
        my_rectangle->show();
    }

    m_selected_sector = 0;
    my_scene->switchFreeSelectMode();

    invertSelectionMode();

    //Re-apply the grey highlight
    if (!my_scene->isFreeSelectMode())
    {
         QString sectors =
            QString::fromLatin1(m_pdf_sig->getOccupiedSectors(m_current_page_number));
        highlightSectors(sectors);
    }
}

void ScapSignature::invertSelectionMode()
{
       //Invert mode on the SelectableRectangles
        QList<QGraphicsItem *> scene_items = my_scene->items();

        for (int i=0; i!= scene_items.size(); i++)
        {
            QGraphicsItem *item = scene_items.at(i);
            //Find the rectangle object
            if (item->type() == SelectableRectType)
            {
                SelectableRectangle *rect = qgraphicsitem_cast<SelectableRectangle*>(scene_items.at(i));

                rect->switchSelectionMode();
                rect->resetColor();
            }

        }

}

void ScapSignature::resetRectanglePos()
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

void ScapSignature::addSquares()
{
    double scene_height = ui->scene_view->height()-2*margin;
    double scene_width = ui->scene_view->width()-2*margin;

    // qDebug() << "scene_height: " << scene_height;
    // qDebug() << "scene_width: " << scene_width;

    int h_lines = 0, v_lines = 0;

    if (m_landscape_mode)
    {
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

    my_rectangle = new DraggableRectangle(my_scene, scene_height, scene_width, rectY, rectX);
    my_scene->addItem(my_rectangle);
    my_rectangle->setPos(margin, margin);
    my_rectangle->hide();
}

void ScapSignature::clearAllSectors()
{

     my_scene->clearAllRectangles();

}

void ScapSignature::highlightSectors(QString &csv_sectors)
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

void ScapSignature::updateMaxPage(int removed_index)
{
    page_numbers.removeAt(removed_index);
    if (page_numbers.size() > 0)
        ui->spinBox_page->setMaximum(*std::min_element(page_numbers.begin(), page_numbers.end()));

    clearScapSign();
}

void ScapSignature::reloadPdfSig(){
    if(current_input_path.length() > 0)
    {
        m_pdf_sig = new eIDMW::PTEID_PDFSignature(strdup(getPlatformNativeString(current_input_path)));

        int tmp_count = m_pdf_sig->getPageCount();

        if (tmp_count < 1)
        {
            QFileInfo fi(current_input_path);
            ShowErrorMsgBox(tr("Unsupported or damaged PDF file: ") + fi.fileName());
            m_pdf_sig = NULL;
            return;
        }
    }
}

void ScapSignature::addFileToListView(QStringList &str)
{
    if (str.isEmpty())
        return;

    current_input_path = str.at(0);

    m_pdf_sig = new eIDMW::PTEID_PDFSignature(strdup(getPlatformNativeString(current_input_path)));


    int tmp_count = m_pdf_sig->getPageCount();


    if (tmp_count < 1)
    {
        QFileInfo fi(current_input_path);
        ShowErrorMsgBox(tr("Unsupported or damaged PDF file: ") + fi.fileName());
        m_pdf_sig = NULL;
        return;
    }

    page_numbers.append(tmp_count);
    m_pageCount = tmp_count;
    m_landscape_mode = m_pdf_sig->isLandscapeFormat();

    // qDebug() << "Landscape format? " << m_landscape_mode;

    for(int i=0; i != str.size(); i++)
    {
        QString tmp = str.at(i);
        list_model->insertRows(list_model->rowCount(), 1);
        list_model->setData(list_model->index(list_model->rowCount()-1, 0), tmp);
        if (i > 0)
        {
            //int page_n = m_pdf_sig->getOtherPageCount(strdup(getPlatformNativeString(tmp)));
            page_numbers.append(tmp_count);
        }

    }

    //Set the spinbox with the appropriate max value
    ui->spinBox_page->setMaximum(*std::min_element(page_numbers.begin(), page_numbers.end()));

    //if (!my_scene)
        buildLocationTab();

    QString sectors = QString::fromLatin1(m_pdf_sig->getOccupiedSectors(1));
    highlightSectors(sectors);

    //Enable sign button now that we have data and a card inserted
    if (!str.isEmpty() && this->card_present)
    {
        ui->button_sign->setEnabled(true);
        ui->pushButton_freeselect->setEnabled(true);

        ui->radioButton_firstpage->setEnabled(true);
        ui->radioButton_lastpage->setEnabled(true);
        ui->radioButton_choosepage->setEnabled(true);

        //Pre-select first page option
        if (!ui->radioButton_choosepage->isChecked() && !ui->radioButton_firstpage->isChecked()
                && !ui->radioButton_lastpage->isChecked())
            ui->radioButton_firstpage->click();

        bool choose_b = ui->radioButton_choosepage->isChecked();
        ui->spinBox_page->setEnabled(choose_b);
    }
    ui->button_addfile->setText(tr("&Remove File"));



    if (list_model->rowCount() > 1)
    {
        clearAllSectors();
    }

}

void ScapSignature::on_btn_advancedOptions_clicked()
{
    ui->advancedOptionsLayout->setVisible(!ui->advancedOptionsLayout->isVisible());
}


ProxyInfo::ProxyInfo()
{
	eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
	eIDMW::PTEID_Config config2(eIDMW::PTEID_PARAM_PROXY_PORT);
	eIDMW::PTEID_Config config_username(eIDMW::PTEID_PARAM_PROXY_USERNAME);
	eIDMW::PTEID_Config config_pwd(eIDMW::PTEID_PARAM_PROXY_PWD);
	eIDMW::PTEID_Config pacfile(eIDMW::PTEID_PARAM_PROXY_PACFILE);

	std::string proxy_host = config.getString();
	std::string proxy_username = config_username.getString();
	std::string proxy_pwd = config_pwd.getString();
	long proxy_port = config2.getLong();
	const char * pacfile_url = pacfile.getString();

	if (pacfile_url != NULL && strlen(pacfile_url) > 0)
	{
		system_proxy = true;
		m_pac_url = QString(pacfile_url);
	}

	if (!proxy_host.empty() && proxy_port != 0)
	{

		m_proxy_host = QString::fromStdString(proxy_host);
		m_proxy_port = QString::number(proxy_port);

		if (!proxy_username.empty())
		{
			m_proxy_user = QString::fromStdString(proxy_username);
			m_proxy_pwd = QString::fromStdString(proxy_pwd);
		}

	}
}

void ProxyInfo::getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port)
{
	if (!system_proxy)
		return;
	std::string proxy_port_str;

	PTEID_GetProxyFromPac(m_pac_url.toUtf8().constData(), urlToFetch.c_str(), proxy_host, &proxy_port_str);

	if (proxy_host->size() > 0 && proxy_port_str.size() > 0)
		*proxy_port = atol(proxy_port_str.c_str());

}

QString ScapSignature::getConnErrStr(){
    QString error_msg( tr( "Error loading entities" ) );

    std::string strConnErr = connectionErr.getErrStr();
    if ( strConnErr.empty() ){
        qDebug() << "ScapSignature::getConnErrStr() - strConnErr empty";
        return error_msg;
    }/* if ( strConnErr.empty() ) */

    error_msg += "\n\n";
    error_msg += tr( strConnErr.c_str() );

    return error_msg;
}/* ScapSignature::getConnErrStr() */

void ScapSignature::setConnErr( int soapConnErr, void *in_suppliers_resp ){
    qDebug() << "ScapSignature::setConnErr() - soapConnErr: " << soapConnErr;
    connectionErr.setErr( soapConnErr, in_suppliers_resp );
}/* ScapSignature::setConnErr() */

void ScapSignature::on_btn_reloadAatributes_clicked()
{
    //Single File Signature case
    pdialog = new QProgressDialog();
    pdialog->setWindowIcon(QIcon(":/appicon/Images/pteid.ico"));
    pdialog->setWindowModality(Qt::WindowModal);
    pdialog->setWindowTitle(tr("Attribute Suppliers"));
    pdialog->setLabelText(tr("Loading attribute suppliers..."));
    pdialog->setCancelButtonText(tr("Cancel"));
    pdialog->setMinimum(0);
    pdialog->setMaximum(0);
    connect(&this->FutureWatcher, SIGNAL(finished()), pdialog, SLOT(cancel()));
    QFuture<void> future = QtConcurrent::run(this,&ScapSignature::getAttributeSuppliers);

    this->FutureWatcher.setFuture(future);
    pdialog->exec();

    std::cout << "Number of loaded attribute suppliers: " << m_suppliersList.size() << std::endl << std::flush;

    if (m_suppliersList.size() == 0)
    {
        QString error_msg = getConnErrStr();

        QMessageBox msgBoxp( QMessageBox::Warning
                            , tr( "Warning" )
                            , error_msg
                            , 0
                            , this );

        msgBoxp.exec();
        return;
    }

    // Show AttributeSuppliers passing the list of received suppliers
    AttributeSuppliers attrSpl(m_suppliersList);
    attrSpl.exec();

    m_selectedSuppliersList = attrSpl.m_selectedSuppliersList;

    // Runs after attributeSuppliers is closed in order to load professionalattributes
    if(attrSpl.m_selectedSuppliersList.size() > 0)
    {
        // Loading attributes case
        pdialog->setWindowTitle(tr("Attribute Suppliers"));
        pdialog->setLabelText(tr("Loading attributes from selected suppliers..."));

        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "Selected suppliers: %d", attrSpl.m_selectedSuppliersList.size());

        connect(&this->FutureWatcher, SIGNAL(finished()), pdialog, SLOT(cancel()));
        future = QtConcurrent::run(this,&ScapSignature::loadProfessionalAttributes, attrSpl.m_selectedSuppliersList);
        this->FutureWatcher.setFuture(future);
        pdialog->exec();
    }
    delete pdialog;
}

const char * as_endpoint = "/DSS/ASService";

void ScapSignature::getAttributeSuppliers()
{

	soap * sp = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
	//TODO: this disables server certificate verification !!
	soap_ssl_client_context(sp, SOAP_SSL_NO_AUTHENTICATION, NULL, NULL, NULL, NULL, NULL);

	// Get Endpoint from settings
	ScapSettings settings;
	std::string port = settings.getScapServerPort().toStdString();
	std::string sup_endpoint = std::string("https://") + settings.getScapServerHost().toStdString() + ":" + port + as_endpoint;

    //Define appropriate network timeouts
    sp->recv_timeout = 20;
    sp->send_timeout = 20;
    sp->connect_timeout = 20;

	std::string proxy_host;
	long proxy_port;
	//Proxy support using the gsoap BindingProxy
	if (m_proxyInfo.isSystemProxy())
	{
		m_proxyInfo.getProxyForHost(sup_endpoint, &proxy_host, &proxy_port);
		if (proxy_host.size() > 0)
		{
			sp->proxy_host = proxy_host.c_str();
			sp->proxy_port = proxy_port;

			eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "Using System Proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);
		}
	}
	else if (m_proxyInfo.getProxyHost().size() > 0)
	{
		sp->proxy_host = strdup(m_proxyInfo.getProxyHost().toUtf8().constData());
		sp->proxy_port = m_proxyInfo.getProxyPort().toLong();
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "Using Manual Proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);

		if (m_proxyInfo.getProxyUser().size() > 0)
		{
			sp->proxy_userid = strdup(m_proxyInfo.getProxyUser().toUtf8().constData());
			sp->proxy_passwd = strdup(m_proxyInfo.getProxyPwd().toUtf8().constData());
		}
	}

    ns2__AttributeSupplierResponseType suppliers_resp;

    const char * c_sup_endpoint = sup_endpoint.c_str();

    AttributeSupplierBindingProxy suppliers_proxy(sp);
    suppliers_proxy.soap_endpoint = c_sup_endpoint;

    int ret = suppliers_proxy.AttributeSuppliers(suppliers_resp);
    setConnErr( ret, &suppliers_resp );

    if (ret != SOAP_OK) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR
                        , "ScapSignature"
                        , "Error in getAttributeSuppliers: Gsoap returned %d, Attribute suppliers end point: %s, host: %s, port: %s"
                        , ret
                        , c_sup_endpoint
                        , settings.getScapServerHost().toStdString().c_str()
                        , settings.getScapServerPort().toStdString().c_str() );

        if ( ( suppliers_proxy.soap->fault != NULL )
			&& (suppliers_proxy.soap->fault->faultstring != NULL)) {

            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR
                            , "ScapSignature", "SOAP Fault: %s"
                            , suppliers_proxy.soap->fault->faultstring );
        }
    }
    m_suppliersList = suppliers_resp.AttributeSupplier;
}

void ScapSignature::loadProfessionalAttributes(std::vector<ns3__AttributeSupplierType *> selectedSuppliersList){
    m_attributesType = ACServiceClient::reqAttributeSupplierListType(selectedSuppliersList);
    emit loadedProfessionalAttributes();
}

void ScapSignature::loadProfessionalAttributes(){
    std::cout << "Number of loaded Professional attributes: " << m_attributesType.size() << std::endl;
    ui->attributesList->clear();
    QStringList errorList;

    std::vector<ns3__AttributeSupplierType *> attrWithoutChilds = m_selectedSuppliersList;

    // Iterates over received attributes type list
    for(uint y = 0; y < m_attributesType.size() ; y++)
    {
        bool foundSelectedSupplier = false;
        bool hasChildren = false;

        TreeItemInfo treeInfo(ACServiceClient::getAttrSupplierInfo(m_attributesType.at(y)));
        QTreeWidgetItem *privateParent;

        std::vector<TreeItemInfo> childs = ACServiceClient::getChildrenTreeInfo(m_attributesType.at(y));

        // Adds all supplier received attributes
        for(uint childPos = 0; childPos < childs.size(); childPos++)
        {
            if(!foundSelectedSupplier)
            {
                foundSelectedSupplier = true;
                for(int i = 0; i < attrWithoutChilds.size(); i++)
                {
                    ns3__AttributeSupplierType * attrSelected =  attrWithoutChilds.at(i);
                    std::string selectedId = attrSelected->Id;
                    if( treeInfo.getTreeItemId().toStdString().compare(selectedId) == 0)
                    {
                        attrWithoutChilds.erase( attrWithoutChilds.begin() + i );
                    }
                }
            }

            // Only needs to add tree parent 1 time
            if(!hasChildren)
            {
                privateParent = AddRoot(treeInfo);
                hasChildren = true;
            }

            addChildToAttributesList(privateParent, childs.at(childPos));
        }
    }

    if(attrWithoutChilds.size() > 0 )
    {
        for(uint i = 0; i < attrWithoutChilds.size(); i++ )
        {
            ns3__AttributeSupplierType * attrNotFound = attrWithoutChilds.at(i);

            errorList.append(QString::fromUtf8(attrNotFound->Name.c_str()));
        }
    }

    // Display error msg
    if(errorList.size() > 0){
        ShowWarningMsgBoxList(tr("No attributes from entities:"), errorList);
    }
}

void ScapSignature::ShowWarningMsgBoxList(QString warningMsg, QStringList errorList)
{
    QString caption  = tr("Warning");

    for(int i = 0; i < errorList.size(); i++)
    {
        warningMsg.append("\n - " + errorList.at(i));
    }

    QMessageBox msgBoxp(QMessageBox::Warning, caption, warningMsg, 0, this);
    msgBoxp.exec();
}

QTreeWidgetItem * ScapSignature::AddRoot(TreeItemInfo treeInfo){
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->attributesList);
    item->setExpanded(true);
    item->setText(0,treeInfo.getTreeItemName());
    item->setData(0, Qt::UserRole, treeInfo.getTreeItemId());
    ui->attributesList->addTopLevelItem(item);
    return item;
}

void ScapSignature::addChildToAttributesList(QTreeWidgetItem *parent, TreeItemInfo treeInfo){
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, treeInfo.getTreeItemName());
    item->setData(0, Qt::UserRole, treeInfo.getTreeItemId());
    item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
    item->setCheckState(0,Qt::Unchecked);
    parent->addChild(item);
}

void ScapSignature::updateSelectedAttributes(){
    m_selected_attributesType.clear();

    //Foreach top level
    for( int parent = 0; parent < ui->attributesList->topLevelItemCount(); parent++ )
    {
       QTreeWidgetItem *topLevel = ui->attributesList->topLevelItem( parent );

       //Foreach childreen
       for( int child = 0; child < topLevel->childCount(); child++ )
       {

           QTreeWidgetItem *childItem = topLevel->child(child);

           // See if user checked item
           if (childItem->checkState(0))
           {
               std::cout << "Child name: " << childItem->text(0).toStdString() << std::endl;
               QString parentID = topLevel->data(0, Qt::UserRole).toString();
               QString childID = childItem->data(0, Qt::UserRole).toString();


               // Add to list when checked
               ACService::ns3__AttributeType * attributeType = ACServiceClient::getAttributeType(parentID, childID, m_attributesType);
               if(attributeType != NULL)
                   m_selected_attributesType.push_back(attributeType);
           }
       }
    }
}


