#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QListView>

class MyListView : public QListView
{
    Q_OBJECT

    public:

    void keyPressEvent(QKeyEvent* event);
    MyListView(QWidget *parent = 0);
    void removeSelected(); 

    private slots:

};

#endif
