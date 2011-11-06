/****************************************************************************
**
** Copyright (C) 2003-2007 Trolltech ASA. All rights reserved.
**
** This file is part of a Qt Solutions component.
**
** Licensees holding a valid Qt Solutions License Agreement may use this
** file in accordance with the rights, responsibilities, and obligations
** contained therein. Please consult your licensing agreement or contact
** sales@trolltech.com if any conditions of this licensing are not clear
** to you.
**
** Further information about Qt Solutions licensing is available at:
** http://www.trolltech.com/products/qt/addon/solutions/ 
** or by contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef QTSINGLEAPPLICATION_H
#define QTSINGLEAPPLICATION_H

#include <QtGui/QApplication>
#ifdef Q_WS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

class QtSingletonPrivate;
class QtSingletonSysPrivate;
class QWidget;

#if defined(Q_WS_WIN)
#  if !defined(QT_QTSINGLEAPPLICATION_EXPORT) && !defined(QT_QTSINGLEAPPLICATION_IMPORT)
#    define QT_QTSINGLEAPPLICATION_EXPORT
#  elif defined(QT_QTSINGLEAPPLICATION_IMPORT)
#    if defined(QT_QTSINGLEAPPLICATION_EXPORT)
#      undef QT_QTSINGLEAPPLICATION_EXPORT
#    endif
#    define QT_QTSINGLEAPPLICATION_EXPORT __declspec(dllimport)
#  elif defined(QT_QTSINGLEAPPLICATION_EXPORT)
#    undef QT_QTSINGLEAPPLICATION_EXPORT
#    define QT_QTSINGLEAPPLICATION_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTSINGLEAPPLICATION_EXPORT
#endif

class QT_QTSINGLEAPPLICATION_EXPORT QtSingleApplication : public QApplication
{
    Q_OBJECT
public:
    QtSingleApplication(const QString &id, int &argc, char **argv, Type = GuiClient);
#ifdef Q_WS_X11
    QtSingleApplication(Display* dpy, const QString &id, int argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
#endif
    ~QtSingleApplication();

    bool isRunning() const;
    QString id() const;

    void initialize(bool activate = true);
    void setActivationWindow(QWidget* aw);
    QWidget* activationWindow() const;

protected:
#if defined(Q_WS_X11)
    bool x11EventFilter(XEvent *msg);
#endif

public Q_SLOTS:
    bool sendMessage(const QString &message, int timeout = 5000);
    virtual void activateWindow();

Q_SIGNALS:
    void messageReceived(const QString& message);

private:
    friend class QtSingletonSysPrivate;
#ifdef Q_WS_MAC
    friend CFDataRef MyCallBack(CFMessagePortRef, SInt32, CFDataRef, void *);
#endif
    QtSingletonPrivate *d;
    QtSingletonSysPrivate *sysd;
    QWidget* actWin;

    void sysInit();
    void sysCleanup();
};

#endif //QTSINGLEAPPLICATION_H
