/*********************************************************************
**
** Graphical Widget with draggable rectangle
** for selection of an area within an A4-sized page
**
**********************************************************************/

#include <QtGui>
#include <QPainter>
#include <QStyleOption>
#include <QPen>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QDialog>

#include "FreeSelectionDialog.h"


double g_scene_height;
double g_scene_width;

class MyGraphicsScene : public QGraphicsScene
{
	
public: 
	MyGraphicsScene(QString background_string): QGraphicsScene()
	{
		background = background_string;
	}
	void itemMoved(QPointF newPos);
	void setParent(FreeSelectionDialog *my_parent)
	{
		parent = my_parent;
	}
	//override to draw the text in background
	void drawBackground (QPainter * painter, const QRectF & rect );
	

private:

	FreeSelectionDialog *parent;
	QString background;


};

void MyGraphicsScene::itemMoved(QPointF newPos)
{
	if (parent)
		parent->setPosition(newPos);

}

void MyGraphicsScene::drawBackground(QPainter * painter, const QRectF & rect )
{

	QFont sansSerifFont("Arial", 26, QFont::Bold);
	painter->setPen(Qt::lightGray);
	painter->setFont(sansSerifFont);

	painter->drawText(rect, Qt::AlignCenter, this->background);

}


class Rectangle : public QGraphicsItem
{
	public:
		Rectangle(MyGraphicsScene *parent, double rect_h, double rect_w): my_scene(parent), m_rect_w(rect_w), m_rect_h(rect_h)
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

		QRectF boundingRect() const;

	private:
		MyGraphicsScene * my_scene;
		double m_rect_h, m_rect_w;

};
const int margin = 10;

void Rectangle::paint(QPainter *painter,
		const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::black);
	painter->setBrush(Qt::lightGray);
	painter->drawRect(0, 0, m_rect_w, m_rect_h);
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
	
	/* Detect out of boundaries mouseEvent 
	and correct them to the nearest valid position **/
	if (tmp.rx() < margin)
		tmp.setX(margin);
	if (tmp.ry() < margin)
		tmp.setY(margin);
	if (tmp.rx() >= g_scene_width+margin - m_rect_w) 
		tmp.setX(g_scene_width+margin-m_rect_w);
	if (tmp.ry() >= g_scene_height+margin - m_rect_h)
		tmp.setY(g_scene_height+margin - m_rect_h);

	QApplication::setOverrideCursor(QCursor(Qt::OpenHandCursor));
	setPos(tmp);
	//Send message to parent
	my_scene->itemMoved(tmp);
	QGraphicsItem::mouseReleaseEvent(event);
}

double FreeSelectionDialog::convertX()
{
	double full_width = m_landscape_mode ? 297.0576 : 209.916;
	return this->rx/g_scene_width * full_width;
}

double FreeSelectionDialog::convertY()
{
	double full_height = m_landscape_mode ? 209.916 : 297.0576;
	return this->ry/g_scene_height * full_height;
}

/* Coordinate conversion */
void FreeSelectionDialog::setPosition(QPointF new_pos)
{
	this->rx = new_pos.rx()-margin;
	this->ry = new_pos.ry()-margin;

	ui.label_x->setText(tr("Horizontal position: %1")
		.arg(QString::number(convertX(), 'f', 1)));
	
	ui.label_y->setText(tr("Vertical Position: %2")
		.arg(QString::number(convertY(), 'f', 1)));

}

void FreeSelectionDialog::on_pushButton_reset_clicked()
{

	ui.label_x->setText(tr("Horizontal position: 0.0"));
	ui.label_y->setText(tr("Vertical position: 0.0"));
	resetRectanglePos();

}

void FreeSelectionDialog::on_pushButton_ok_clicked()
{
	this->done(QDialog::Accepted);

}

void FreeSelectionDialog::getValues(double *x, double *y)
{
       *x = convertX();
       *y = convertY();
}

void FreeSelectionDialog::resetRectanglePos()
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

void FreeSelectionDialog::drawBackgroundGrid(QGraphicsScene *scene)
{

    QPen dashed_pen(Qt::DashLine);

    int v_lines = m_landscape_mode ? 6 : 3;
    int h_lines = m_landscape_mode ? 3 : 6;
	
    //Draw the background dashed grid representing the 18 page sectors 
    for (int i=0; i<= h_lines; i++)
		scene->addLine(margin, i*g_scene_height/h_lines +margin, g_scene_width+margin, i*g_scene_height/h_lines +margin, 
			    dashed_pen);

    for (int i=0; i<= v_lines; i++)
		scene->addLine(i*g_scene_width/v_lines +margin, margin, i*g_scene_width/v_lines +margin, g_scene_height+ margin,
			    dashed_pen);
}

FreeSelectionDialog::FreeSelectionDialog(QWidget *parent, bool landscape_mode): QDialog(parent)
{
	g_scene_height = 421;
	g_scene_width = 297;
    double rect_h = g_scene_height *0.106888361045; //Proportional to current signature height
	double rect_w = g_scene_width / 3.0;
    ui.setupUi(this);

    MyGraphicsScene *scene = new MyGraphicsScene(tr("A4 Page"));
    m_landscape_mode = landscape_mode;

    if (landscape_mode)
    {
    	double tmp = ui.widget->height();
    	g_scene_width = ui.widget->height()-20;
    	g_scene_height = ui.widget->width()-70;

    	// fprintf(stderr, "H: %d W: %d\n", ui.widget->height(), ui.widget->width());
    	ui.widget->setFixedSize(ui.widget->height(), ui.widget->width()-20);

    	rect_w = g_scene_width / 6.0;
    	rect_h = g_scene_height * 0.15;
    	//ui.widget->setMinimumWidth(tmp);
    }

    double real_scene_width = g_scene_width + 18;
    double real_scene_height = g_scene_height + 24;
    this->rx = 0;
    this->ry = 0;

    scene->setSceneRect(0, 0, real_scene_width, real_scene_height);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // if (landscape_mode)
    // 	scene->addItem(new Rectangle(scene, rect_w, rect_h));
    // else
    scene->addItem(new Rectangle(scene, rect_h, rect_w));

    drawBackgroundGrid(scene);

    my_scene = scene;
    QGraphicsView *view = new QGraphicsView(ui.widget);
    view->setScene(scene);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    scene->setParent(this);
    // fprintf(stderr, "FreeSelectionDialog Width: %d, Height: %d\n", ui.widget->width(), ui.widget->height());

    //Add room for margins, the textboxes and buttons below the widget
    // this->setFixedSize(ui.widget->width() + 50, ui.widget->height() + 70);
    int items = ui.horizontalLayout->count();
	for (int i = 0; i!= items; i++)
	{
		ui.horizontalLayout->itemAt(i)->setAlignment(Qt::AlignRight);
	}

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

