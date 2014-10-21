#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QEvent>
#include <QListView>

class MyDeleteEvent : public QEvent
{
	public:
		MyDeleteEvent(): QEvent(QEvent::User) {}
		~MyDeleteEvent() {}

};

class MyListView : public QListView
{
    Q_OBJECT

    public:

    void keyPressEvent(QKeyEvent* event);
    MyListView(QWidget *parent = 0);
    void removeSelected(); 
    void enableNotify();

    signals:
    
    void itemRemoved(int index);

    private:
    bool notify;




};

#endif
