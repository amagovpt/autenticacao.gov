#include "appcontroller.h"
#include <QObject>
#include <QCursor>
#include <QDebug>

AppController::AppController(QObject *parent) :
    QObject(parent)
{
}

QVariant AppController::getCursorPos()
{
    return QVariant(QCursor::pos());
}
