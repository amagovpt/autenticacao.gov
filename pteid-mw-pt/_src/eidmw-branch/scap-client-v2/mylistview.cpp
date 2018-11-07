#include <QListView>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QAbstractItemView>
#include "mylistview.h"
#include <iostream>
#include <cstdio>


MyListView::MyListView(QWidget *parent): 
	QListView(parent)
{
	notify = false;
	setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void MyListView::keyPressEvent(QKeyEvent* event)
{
	if (!model()) {
		QListView::keyPressEvent(event);
	} else if (event->matches(QKeySequence::Delete)) {
		removeSelected();
		event->accept();
	} else {
		QListView::keyPressEvent(event);
	}

}

void MyListView::enableNotify()
{
	notify = true;
}

bool CompareSelectionRanges(const QItemSelectionRange& a, const QItemSelectionRange& b) {
  return b.bottom() < a.bottom();
}

//Code gently provided by Clementine
void MyListView::removeSelected()
{
 
  int rows_removed = 0;
  QItemSelection selection(selectionModel()->selection());

  if (selection.isEmpty()) {
    return;
  }

  // Sort the selection so we remove the items at the *bottom* first, ensuring
  // we don't have to mess around with changing row numbers
  qSort(selection.begin(), selection.end(), CompareSelectionRanges);

  // Store the last selected row, which is the first in the list
  int last_row = selection.first().bottom();

  //PDFSignWindow needs to be notified of the item removed to update another widget
  emit itemRemoved(last_row);

  foreach (const QItemSelectionRange& range, selection) {
	  rows_removed += range.height();
	  model()->removeRows(range.top(), range.height(), range.parent());
  }

  int new_row = last_row-rows_removed+1;
  // Index of the first column for the row to select
  QModelIndex new_index = model()->index(new_row, 0);

  // Select the new current item, we want always the item after the last selected
  if (new_index.isValid()) {
	  // Update visual selection with the entire row
	  selectionModel()->select(QItemSelection(new_index, model()->index(new_row, model()->columnCount()-1)),
			  QItemSelectionModel::Select);
	  // Update keyboard selected row, if it's not the first row
	  if (new_row != 0)
		  keyPressEvent(new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
  }
  else
  {
	  // We're removing the last item, select the new last row
	  selectionModel()->select(QItemSelection(model()->index(model()->rowCount()-1, 0),
				  model()->index(model()->rowCount()-1, model()->columnCount()-1)),
			  QItemSelectionModel::Select);
  }

  /*	QModelIndexList indexes = this->selectionModel()->selectedIndexes();
	while(indexes.size()) {

		this->model()->removeRow(indexes.first().row());
		indexes = this->selectionModel()->selectedIndexes();
	}
	//ui.listView->repaint();
	*/

  if (notify && model()->rowCount() == 0)
  {
	  QEvent * delete_event =  new MyDeleteEvent();
	  //We have to deliver the event to the Window/dialog class
	  //which is the grandparent because we have the centralwidget in between
	  QCoreApplication::sendEvent(parentWidget()->parentWidget(), delete_event);

  }

}
