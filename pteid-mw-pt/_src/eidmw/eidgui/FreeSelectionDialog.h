#include <QtGui>
#include <QDialog>
#include "ui_FreeSelectionDialog.h"

static double scene_height = 421;
static double scene_width = 297;

class FreeSelectionDialog : public QDialog
{
	Q_OBJECT

	private slots:

//	void on_text_edit_x_editingFinished();
//	void on_text_edit_y_editingFinished();
	void on_pushButton_reset_clicked();
	void on_pushButton_ok_clicked();

	public:
		FreeSelectionDialog (QWidget *parent, bool landscape_mode);
		void setPosition(QPointF new_pos);
		//This is intended to be called after the dialog is closed
		void getValues(double *x, double *y);

	private:
		void resetRectanglePos();
		void drawBackgroundGrid(QGraphicsScene *);
		Ui_FreeSelectionDialog ui;
		QGraphicsScene * my_scene;
		bool m_landscape_mode;
		double rx, ry;

};
