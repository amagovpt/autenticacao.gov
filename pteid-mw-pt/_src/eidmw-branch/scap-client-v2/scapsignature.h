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

#ifndef ScapSignature_H_
#define ScapSignature_H_

#include <QDialog>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QFutureWatcher>
#include "eidlib.h"
#include "ui_scapsignature.h"
#include "cardevent.h"
#include <QTreeWidgetItem>
#include "ErrorConn.h"

enum ErrorCode
{
    SIG_SUCCESS
    ,SIG_ERROR
    ,TS_WARNING
    , CANCELED_BY_USER
};

/* Forward declarations */
class FreeSelectionDialog;
class ScapSignature;
class ns3__AttributeSupplierType;
class ns2__AttributeRequestType;
class ns2__AttributeResponseType;
class SelectedTreeInfo;
class TreeItemInfo;

namespace ACService {
    class ns2__AttributesType;
    class ns3__AttributeType;
}


class ProxyInfo
{
public:
	ProxyInfo();
	QString getProxyHost() {
		return m_proxy_host;
	}
	QString getProxyPort() {
		return m_proxy_port;
	}

	QString getProxyUser() { return m_proxy_user; }

	QString getProxyPwd() { return m_proxy_pwd; }

	bool isSystemProxy() { return system_proxy;  }

	void getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port);

private:
	bool system_proxy;
	QString m_pac_url;
	QString m_proxy_host;
	QString m_proxy_port;
	QString m_proxy_user;
	QString m_proxy_pwd;

};

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
    QColor getColor() { return m_brush.color(); }
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

namespace Ui {
class ScapSignature;
}

class ScapSignature : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScapSignature(QWidget *parent = 0);
    ~ScapSignature();

    void customEvent(QEvent *ev);
    void addCardEventCallback(unsigned long callbackEvent, char *readerName);
    void enableSignButton();
    void disableSignButton();
    void setPosition(QPointF new_pos);
    void setSelectedSector(int sector);
private:
    Ui::ScapSignature *ui;

    /* Connection error functions */
    QString getConnErrStr();
    void setConnErr( int soapConnectionErr, void *in_suppliers_resp );

    /* --- Start Functions -- */
    void clear_sig_position();
    void invertSelectionMode();
    void highlightSectors(QString &csv_sectors);
    void clearAllSectors();
    void addFileToListView(QStringList &str);
    void run_sign(int page, QString& savefile);
    void ShowSuccessMsgBox();
    void buildLocationTab();
    void loadProfessionalAttributes(std::vector<ns3__AttributeSupplierType *> selectedSuppliersList);
    void resetRectanglePos();
    void drawBackgroundGrid(QGraphicsScene *);
    void addSquares();
    void reloadPdfSig();
    void computeSigLocationFromSectorLandscape(int sector);
    void computeSigLocationFromSector(int sector);
    void clearScapSign();

    // Card related Functions
    void initReaderAndCard();
    //AttributesList
    QTreeWidgetItem * AddRoot(TreeItemInfo);
    void addChildToAttributesList(QTreeWidgetItem *, TreeItemInfo);
    void updateSelectedAttributes();
    void getAttributeSuppliers();

    void ShowWarningMsgBoxList(QString warningMsg, QStringList errorList);

    /* --- End Functions -- */

    /* --- Start Variables -- */
    std::vector<ns3__AttributeSupplierType *> m_suppliersList;
    std::vector<ns3__AttributeSupplierType *> m_selectedSuppliersList;

    int table_lines;
    int table_columns;
    double sig_coord_x;
    double sig_coord_y;

    bool card_present;
    bool horizontal_page_flag;
    bool m_small_signature;
    ErrorConn connectionErr;
    int m_selected_reader;

    QAbstractItemModel *list_model;
    QProgressDialog *pdialog;
    //FreeSelectionDialog *m_selection_dialog;

    QFutureWatcher<void> FutureWatcher;
    eIDMW::PTEID_PDFSignature *m_pdf_sig;
    QList<int> page_numbers;
    //Number of pages of currently loaded document
    int m_pageCount;
    int m_current_page_number;

    QString current_input_path;
    QBrush m_default_background;
    QImage m_custom_image;
    QByteArray m_jpeg_scaled_data;
    int m_selected_sector;
    MyGraphicsScene * my_scene;
    DraggableRectangle *my_rectangle;
    ImageCanvas *image_canvas;
    bool m_landscape_mode;
    double rx, ry;

	ProxyInfo m_proxyInfo;

    std::vector<ACService::ns2__AttributesType *> m_attributesType;
    std::vector<ACService::ns3__AttributeType *> m_selected_attributesType;

    //Success flag for the error messages
    ErrorCode success;

    // add references to Label and ProgressBar
    QLabel *statusLabel;
    QProgressBar *statusProgressBar;

    /* --- End Variables -- */
private slots:
    // GUI related slots
    void on_button_addfile_clicked();
    void on_button_sign_clicked();
    void on_button_cancel_clicked();
    void on_radioButton_choosepage_toggled(bool);
    void on_radioButton_firstpage_toggled(bool);
    void on_radioButton_lastpage_toggled(bool);
    void on_spinBox_page_valueChanged(int);
    void on_pushButton_freeselect_clicked();
    void on_btn_advancedOptions_clicked();
    void on_btn_reloadAatributes_clicked();

    // Events slots
    void updateMaxPage(int removed_index);
    void ShowErrorMsgBox(QString);
    void onCardStatusChanged(CardEvent::eCustomEventType);
    void loadProfessionalAttributes();
signals:
    void loadedProfessionalAttributes();
};
#endif
