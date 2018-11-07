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
 **************************************************************************** */

#ifndef PDFSIGNWINDOW_H_

#include <QDialog>
//#include <QDropEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QProgressDialog>

//For signature preview
#include <poppler-qt5.h>

#include "ui_PDFSignWindow.h"
#include "CardInformation.h"

enum ErrorCode
{
	NOT_YET_SIGNED,
	SIG_SUCCESS
	,SIG_ERROR
	,TS_WARNING
    , CANCELED_BY_USER
};


class MyGraphicsScene : public QGraphicsScene
{

public:
	MyGraphicsScene(QPixmap &background_pixmap): QGraphicsScene()
	{
		background = background_pixmap;
		parent = NULL;
		m_greyedOut = false;
	}

	void updateBackground(QPixmap & background_pixmap)
	{
		background = background_pixmap;
	}
	
	void itemMoved(QPointF newPos);
	
    void setParent(QWidget *my_parent)
    {
        parent = my_parent;
    }

    void enableOverlayRectangle(bool enable) { m_greyedOut = enable; }
	void drawBackground (QPainter * painter, const QRectF & rect );


private:

	QWidget *parent;
	QPixmap background;
    bool m_greyedOut;

};


enum ItemTypes {
		SelectableRectType = QGraphicsItem::UserType+5,

};

class ImageCanvas : public QWidget
{
	Q_OBJECT
public:
	ImageCanvas(QWidget *parent): QWidget(parent), small_sig_format(false), previewVisible(false)
	{
		initDateString();
	}
	QImage drawToImage();

	void setSmallSignature(bool isEnabled) { small_sig_format = isEnabled; }
	void setPreviewEnabled(bool value) { previewVisible = value; this->update(); }
	void setCitizenName(QString name) { citizen_name = name; }
	void setCitizenNIC(QString nic) { citizen_nic = nic; }
	void setLocation(QString loc) { sig_location = loc; }
	void setReason(QString reason) { sig_reason = reason; }
	void setCustomPixmap(QPixmap pixmap) { user_pixmap = pixmap; }


protected:
	void paintEvent(QPaintEvent *);
	void initDateString();

	QPixmap user_pixmap;
	QString date_str, citizen_name, citizen_nic, sig_location, sig_reason;
	bool previewVisible;
	bool small_sig_format;

};


class DraggableRectangle : public QGraphicsItem
{
	public:
		DraggableRectangle(MyGraphicsScene *parent, double max_y, double max_x, double rect_h, double rect_w): my_scene(parent),
		m_max_x(max_x), m_max_y(max_y), m_rect_w(rect_w), m_rect_h(rect_h)
		{
	    	setFlag(QGraphicsItem::ItemIsMovable);
	    	logo_pixmap = QPixmap(":/images/Images/Icons/pteid_signature_small.png");
	    	logo_pixmap = logo_pixmap.scaledToHeight(m_rect_h *(3.0/4.0), Qt::SmoothTransformation);
	    	logo_pixmap_small = logo_pixmap.scaledToHeight(logo_pixmap.height() / 2, Qt::SmoothTransformation);
	    	current_logo = &logo_pixmap;

			setFlag(QGraphicsItem::ItemIsSelectable);
			setAcceptHoverEvents(true);

		};

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		void mouseReleaseEvent (QGraphicsSceneMouseEvent * event);
		void mousePressEvent (QGraphicsSceneMouseEvent * event);
		void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
		void makeItSmaller();
		void makeItLarger();
		void setMaxLocation(double max_x, double max_y) 
		{
			m_max_x = max_x;
			m_max_y = max_y;
		}
		void setToPos( QPointF *newPos );

		QRectF boundingRect() const;

	private:
		QPixmap *current_logo;
		QPixmap logo_pixmap, logo_pixmap_small;
		MyGraphicsScene * my_scene;
		double m_rect_h, m_rect_w, m_max_x, m_max_y;

};



class PDFSignWindow : public QDialog
{
	Q_OBJECT
	private slots:
	    void on_button_addfile_clicked();
	    void on_button_sign_clicked();
	    void on_button_cancel_clicked();
	    void on_button_clearFiles_clicked();

	    void on_pushButton_imgChooser_clicked();
	    void on_pushButton_resetImage_clicked();
	    void on_checkBox_location_toggled(bool);
	    void on_checkBox_reason_toggled(bool);
	    void on_smallsig_checkBox_toggled(bool);
	    void on_visible_checkBox_toggled(bool);
	    void on_radioButton_choosepage_toggled(bool);
	    void on_radioButton_firstpage_toggled(bool);
	    void on_radioButton_lastpage_toggled(bool);
	    void on_spinBox_page_valueChanged(int);
	    void on_reason_textbox_textEdited(QString text);
	    void on_location_textbox_textEdited(QString text);

	    void updateMaxPage(int removed_index);

	public:
	    void customEvent(QEvent *ev);
	    /*Event Handlers for card inserted/removed events
	    that come from Main Window
	    */
	    void enableSignButton();
	    void disableSignButton();
	    void setPosition(QPointF new_pos);
	    PDFSignWindow(QWidget * parent, int selected_reader, CardInformation &ci);
	    ~PDFSignWindow();


	private:

	    QString composeCitizenFullName();
	    QString getCitizenNIC();
		PTEID_EIDCard &getNewCard();
        
	    void addFileToListView(QStringList &str);

	    //PDF Document preview
	    void loadDocumentForPreview(const QString &file);
	    QPixmap renderPageToImage();
	    void closeDocument();

	    void run_sign(int page, QString& savefile, char *location, char *reason);
	    void ShowSuccessMsgBox();
	    void ShowErrorMsgBox(QString message);
	    void buildLocationTab();


	    Ui_PDFSignWindow ui;

	    double sig_coord_x;
	    double sig_coord_y;

	    bool card_present;
	    bool horizontal_page_flag;
	    bool m_small_signature;
		int m_selected_reader;

    	CardInformation const& m_CI_Data;
	    QAbstractItemModel *list_model;
	    QProgressDialog *pdialog;

	    QFutureWatcher<void> FutureWatcher;
	    PTEID_PDFSignature *m_pdf_sig;
	    QList<int> page_numbers;

	    //Number of pages of currently loaded document
	    int m_pageCount;
	    int m_current_page_number;

	    QString current_input_path;
	    QBrush m_default_background;
	    QImage m_custom_image;
	    
	    QByteArray m_jpeg_scaled_data;

	    //Location tab stuff

		double getMaxX();
		double getMaxY();

		MyGraphicsScene * my_scene;
		QPixmap preview_pixmap;
		DraggableRectangle *my_rectangle;
		Poppler::Document * m_doc;

		ImageCanvas *image_canvas;
		bool m_landscape_mode;

		double rx, ry;

	    //Success flag for the error messages
	    ErrorCode success;

};


#endif
