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
 **************************************************************************** */

#ifndef PDFSIGNWINDOW_H_

#include <QDialog>
#include <QDropEvent>
#include "ui_PDFSignWindow.h"
#include "CardInformation.h"

enum ErrorCode
{
	SIG_SUCCESS
	,SIG_ERROR
	,TS_WARNING

};

/* Forward declaration */
class FreeSelectionDialog;


class MyGraphicsScene : public QGraphicsScene
{
	
public:
	MyGraphicsScene(QString background_string): QGraphicsScene()
	{
		background = background_string;
        free_select = false;
	}
	void itemMoved(QPointF newPos);
    void setParent(QWidget *my_parent)
    {
        parent = my_parent;
    }

    void selectNewRectangle(QPointF pos);
    void setOccupiedSector(int s);
    void clearAllRectangles();
    void switchFreeSelectMode() { free_select = !free_select; }

    bool isFreeSelectMode() { return free_select; }
	//override to draw the text in background
	//void drawBackground (QPainter * painter, const QRectF & rect );
	

private:

	QWidget *parent;
	QString background;
    bool free_select;

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

class SelectableRectangle: public QGraphicsItem
{
	public:
        SelectableRectangle(MyGraphicsScene *parent, double rect_h, double rect_w, int sector_number): my_scene(parent), m_rect_w(rect_w), m_rect_h(rect_h)
        {
    		//setFlag(QGraphicsItem::ItemIsMovable);
		//setFlag(QGraphicsItem::ItemSendsGeometryChanges);
		setFlag(QGraphicsItem::ItemIsSelectable);
		// setAcceptHoverEvents(true);
        m_brush = QBrush(QColor (193, 221, 221));
		dont_select_mode = false;
        m_sector_number = sector_number;
        m_string = QString::number(sector_number);
        is_sector_filled = false;

        };
        bool isSectorFilled() { return is_sector_filled; }
        int getSectorNumber() { return m_sector_number; }
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		void mouseReleaseEvent (QGraphicsSceneMouseEvent * event);
		void mousePressEvent (QGraphicsSceneMouseEvent * event);
		void resetColor();
        void setSectorFilled();

        void switchSelectionMode() {
            dont_select_mode = !dont_select_mode;

        };

		QRectF boundingRect() const;
		int type() const { return SelectableRectType; }

	private:
		MyGraphicsScene * my_scene;
        int m_sector_number;
		double m_rect_h, m_rect_w;
		QBrush m_brush;
		QString m_string;
        bool is_sector_filled;
		bool dont_select_mode;

};

class DraggableRectangle : public QGraphicsItem
{
	public:
		DraggableRectangle(MyGraphicsScene *parent, double max_y, double max_x, double rect_h, double rect_w): my_scene(parent), 
		m_max_x(max_x), m_max_y(max_y), m_rect_w(rect_w), m_rect_h(rect_h)
	{
    		setFlag(QGraphicsItem::ItemIsMovable);
		//setFlag(QGraphicsItem::ItemSendsGeometryChanges);
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

		QRectF boundingRect() const;

	private:
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
	    // void on_pushButton_switchOrientation_clicked();
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

	    // Location Tab Stuff
		void on_pushButton_freeselect_clicked();
//	    void on_tableWidget_currentCellChanged(int row, int column, int prev_row, int prev_column);
	    // void on_FreeSelectionDialog_closed();


	public:
	    void customEvent(QEvent *ev);
	    /*Event Handlers for card inserted/removed events
	    that come from Main Window
	    */
	    void enableSignButton();
	    void disableSignButton();
	    void setPosition(QPointF new_pos);
        void setSelectedSector(int sector);
	    PDFSignWindow(QWidget * parent, int selected_reader, CardInformation &ci);
	    ~PDFSignWindow();


	private:
	    // void update_sector(int row, int column);
	    // void update_sector(double x_pos, double y_pos);
	    void clear_sig_position();
	    QString composeCitizenFullName();
	    QString getCitizenNIC();
		PTEID_EIDCard &getNewCard();

	    
	    // bool validateSelectedSector();
	    void invertSelectionMode();
        void highlightSectors(QString &csv_sectors);
        void clearAllSectors();
	    void addFileToListView(QStringList &str);
	    void run_sign(int page, QString& savefile, char *location, char *reason);
	    void ShowSuccessMsgBox();
	    //void switchOrientation();
	    void ShowErrorMsgBox(QString message);
	    void buildLocationTab();

		//This is intended to be called after the dialog is closed
		void getValues(double *x, double *y);


	    Ui_PDFSignWindow ui;
	    int table_lines;
	    int table_columns;
	    double sig_coord_x;
	    double sig_coord_y;

	    bool card_present;
	    bool horizontal_page_flag;
	    bool m_small_signature;
		int m_selected_reader;

    	CardInformation const& m_CI_Data;
	    QAbstractItemModel *list_model;
	    QProgressDialog *pdialog;
	    //FreeSelectionDialog *m_selection_dialog;

	    QFutureWatcher<void> FutureWatcher;
	    PTEID_PDFSignature *m_pdf_sig;
	    QList<int> page_numbers;
	
	    //Number of pages of currently loaded document
	    int m_current_page_number;

	    QString current_input_path;
	    QBrush m_default_background;
	    QImage m_custom_image;
	    QByteArray m_jpeg_scaled_data;

	    int m_selected_sector;

	    //Location tab stuff
	    void resetRectanglePos();
		void drawBackgroundGrid(QGraphicsScene *);
		void addSquares();

		double convertX();

		double convertY();

		MyGraphicsScene * my_scene;
		DraggableRectangle *my_rectangle;

		ImageCanvas *image_canvas;
		bool m_landscape_mode;

		double rx, ry;

	    //Success flag for the error messages
	    ErrorCode success;

};


#endif
