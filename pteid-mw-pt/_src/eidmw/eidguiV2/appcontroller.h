#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QVariant>

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = 0);

public slots:
    Q_INVOKABLE QVariant getCursorPos();
};

#endif // APPCONTROLLER_H
