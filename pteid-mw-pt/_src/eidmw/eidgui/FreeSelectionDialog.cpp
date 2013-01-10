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
		Rectangle(MyGraphicsScene *parent ): my_scene(parent)
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
		static const double rect_h, rect_w;

};

const double Rectangle::rect_h = scene_height *0.106888361045; //Proportional to current signature height
const double Rectangle::rect_w = scene_width / 3.0;
const int margin = 10;

void Rectangle::paint(QPainter *painter,
		const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::black);
	painter->setBrush(Qt::lightGray);
	painter->drawRect(0, 0, rect_w, rect_h);
}

QRectF Rectangle::boundingRect() const
{
	return QRectF(0, 0, rect_w+1, rect_h+1);
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
	if (tmp.rx() >= scene_width+margin-rect_w) 
		tmp.setX(scene_width+margin-rect_w);
	if (tmp.ry() >= scene_height+margin-rect_h)
		tmp.setY(scene_height+margin-rect_h);

	QApplication::setOverrideCursor(QCursor(Qt::OpenHandCursor));
	setPos(tmp);
	//Send message to parent
	my_scene->itemMoved(tmp);
	QGraphicsItem::mouseReleaseEvent(event);
}

void FreeSelectionDialog::setPosition(QPointF new_pos)
{
	this->rx = new_pos.rx()-margin;
	this->ry = new_pos.ry()-margin;
	//printf("Pos: rx=%f, ry=%f\n", this->rx, this->ry);
	ui.text_edit_x->setText(
			QString::number(this->rx/scene_width * 209.916, 'f', 1));
	ui.text_edit_y->setText(
			QString::number(this->ry/scene_height * 297.0576, 'f', 1));

}

void FreeSelectionDialog::on_pushButton_reset_clicked()
{

	ui.text_edit_x->setText(QString("0"));
	ui.text_edit_y->setText(QString("0"));
	changeRectanglePos();

}

void FreeSelectionDialog::on_pushButton_ok_clicked()
{
	this->accept();

}

/*
void FreeSelectionDialog::on_text_edit_x_editingFinished()
{

	changeRectanglePos();
}

void FreeSelectionDialog::on_text_edit_y_editingFinished()
{


	changeRectanglePos();

}
*/

void FreeSelectionDialog::getValues(double *x, double *y)
{
       *x = this->rx / scene_width;
       *y = this->ry / scene_height;

}


void FreeSelectionDialog::changeRectanglePos()
{

	QList<QGraphicsItem *> scene_items = my_scene->items();

	QPointF pos;
	pos.setY(ui.text_edit_y->text().toInt());
	pos.setX(ui.text_edit_x->text().toInt());

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


FreeSelectionDialog::FreeSelectionDialog(QWidget *parent): QDialog(parent)
{
    
    ui.setupUi(this);

    MyGraphicsScene *scene = new MyGraphicsScene(tr("A4 Page"));

    double real_scene_width = scene_width + 18;
    double real_scene_height = scene_height + 24;
    this->rx = 0;
    this->ry = 0;

    scene->setSceneRect(0, 0, real_scene_width, real_scene_height);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->addItem(new Rectangle(scene));

    QPen dashed_pen(Qt::DashLine);
	
    //Draw the background dashed grid representing the 18 page sectors 
    for (int i=0; i<=6; i++)
	scene->addLine(margin, i*scene_height/6.0 +margin, scene_width+margin, i*scene_height/6.0+margin, 
			    dashed_pen);
    for (int i=0; i<=3; i++)
	scene->addLine(i*scene_width/3.0 +margin, 0+10, i*scene_width/3.0+margin, scene_height+ margin,
			    dashed_pen); 

    my_scene = scene;
    QGraphicsView *view = new QGraphicsView(ui.widget);
    view->setScene(scene);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    QValidator *validator_x = new QIntValidator(0, (int)scene_width, this);
    QValidator *validator_y = new QIntValidator(0, (int)scene_height, this);
    ui.text_edit_x->setValidator(validator_x);
    ui.text_edit_x->setValidator(validator_y);
    ui.text_edit_x->setReadOnly(true);
    ui.text_edit_y->setReadOnly(true);

    scene->setParent(this);
    this->setFixedSize(this->width(), this->height());

}

