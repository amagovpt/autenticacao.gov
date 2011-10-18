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
#include "qtsingleapplication.h"
#include <qt_windows.h>
#include <QtGui/QWidget>

class QtSingletonSysPrivate : public QWidget
{
public:
    QtSingletonSysPrivate()
        : QWidget(), listening(FALSE)
    {
    }
    uint listening :1;

protected:
    bool winEvent(MSG *msg, long *result)
    {
        if (msg->message != WM_COPYDATA)
            return FALSE;

        Q_ASSERT(msg->hwnd == winId());
        COPYDATASTRUCT *data = (COPYDATASTRUCT*)msg->lParam;
        QString message = QString::fromUtf16((unsigned short*)data->lpData);

        emit ((QtSingleApplication*)qApp)->messageReceived( message );

		if (result)
		{
			*result = 0;
			return TRUE;
		}
		return FALSE;
    }

};

static HANDLE createLockedMutex(const QString &id)
{
    HANDLE mutex;
    QT_WA({
        mutex = CreateMutex(0, FALSE, (TCHAR*)id.utf16());
    }, {
       mutex = CreateMutexA(0, FALSE, id.toLocal8Bit().data());
    });

    switch (WaitForSingleObject(mutex, INFINITE)) {
    case WAIT_ABANDONED:
    case WAIT_TIMEOUT:
        CloseHandle(mutex);
        mutex = 0;
        break;
    default: // WAIT_OBJECT_0
        break;
    }

    return mutex;
}

static inline void closeLockedMutex(HANDLE handle)
{
    if (!handle)
        return;

    ReleaseMutex(handle);
    CloseHandle(handle);
}

static HWND findWindow(const QString &id)
{
    HANDLE mutex = createLockedMutex(id);

    HWND hwnd;
    QString wid = id + "_QtSingleApplicationWindow";

    QT_WA( {
	hwnd = ::FindWindow(L"QWidget", (TCHAR*)wid.utf16());
    }, {
	hwnd = ::FindWindowA("QWidget", wid.toLocal8Bit().data());
    } )

    closeLockedMutex(mutex);

    return hwnd;
}

void QtSingleApplication::sysInit()
{
    sysd = new QtSingletonSysPrivate;
    (void)sysd->winId();                  // Force widget creation
}

void QtSingleApplication::sysCleanup()
{
    HANDLE mutex = createLockedMutex(id());

    delete sysd;

    closeLockedMutex(mutex);
}

void QtSingleApplication::initialize( bool activate )
{
    if (sysd->listening)
	return;

    HANDLE mutex = createLockedMutex(id());

    sysd->listening = true;
    sysd->setWindowTitle(id() + "_QtSingleApplicationWindow");

    closeLockedMutex(mutex);

    if ( activate )
	connect(this, SIGNAL(messageReceived(const QString&)),
		this, SLOT(activateWindow()));
}

bool QtSingleApplication::isRunning() const
{
    return findWindow(id()) != 0;
}

bool QtSingleApplication::sendMessage( const QString &message, int timeout )
{
    HWND hwnd = findWindow(id());
    if ( !hwnd )
	return FALSE;

    COPYDATASTRUCT data;
    data.dwData = 0;
    data.cbData = (message.length()+1) * sizeof(QChar);
    data.lpData = (void*)message.utf16();
    DWORD result;
    LRESULT res;
    QT_WA( {
	res = SendMessageTimeout(hwnd, WM_COPYDATA, 0/*hwnd sender*/, (LPARAM)&data,
				 SMTO_ABORTIFHUNG,timeout,&result);
    }, {
	res = SendMessageTimeoutA(hwnd, WM_COPYDATA, 0/*hwnd sender*/, (LPARAM)&data,
				  SMTO_ABORTIFHUNG,timeout,&result);
    } )
    //return res != 0;

	// always return TRUE.
	// The reason is that the main application can be processing for an undefined time.
	// e.g. downloading and processing something.
	// In that case, the timeout might trigger which is considered as a non responsive
	// main application, while the app is just intensively processing.
	// As a quick fix: if a window with the same ID exists, don't start a second instance
	// regardless of the timeout for a response.

	return TRUE;
}
