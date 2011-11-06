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
#include <QtCore/QString>
#include <Carbon/Carbon.h>

#include "qtsingleapplication.h"

CFStringRef qstring2cfstring(const QString &str)
{
    return CFStringCreateWithCharacters(0, (UniChar *)str.unicode(), str.length());
}

QString cfstring2qstring(CFStringRef str)
{
    if(!str)
	return QString();

    CFIndex length = CFStringGetLength(str);
    if(const UniChar *chars = CFStringGetCharactersPtr(str))
	return QString((QChar *)chars, length);
    UniChar *buffer = (UniChar*)malloc(length * sizeof(UniChar));
    CFStringGetCharacters(str, CFRangeMake(0, length), buffer);
    QString ret((QChar *)buffer, length);
    free(buffer);
    return ret;
}

class QtSingletonSysPrivate
{
public:
    inline QtSingletonSysPrivate() { port = 0; }
    CFMessagePortRef port;
};

class QtSingletonPrivate
{
};

bool QtSingleApplication::isRunning() const
{
    CFStringRef cfstr = qstring2cfstring(id());
    CFMessagePortRef myport = CFMessagePortCreateRemote(kCFAllocatorDefault, cfstr);
    CFRelease(cfstr);
    return (myport != 0);
}

void QtSingleApplication::sysInit()
{
    sysd = new QtSingletonSysPrivate;
}

void QtSingleApplication::sysCleanup()
{
    delete sysd;
}

CFDataRef MyCallBack(CFMessagePortRef, SInt32, CFDataRef data, void *info)
{
    CFIndex index = CFDataGetLength(data);
    const UInt8 *p = CFDataGetBytePtr(data);
    QByteArray ba(index, 0);
    for (int i = 0; i < index; ++i)
        ba[i] = p[i];
    QString message(ba);
    emit static_cast<QtSingleApplication *>(info)->messageReceived(message);
    return 0;
}

void QtSingleApplication::initialize(bool activate)
{
    CFStringRef cfstr = qstring2cfstring(id());
    CFMessagePortContext context;
    context.version = 0;
    context.info = this;
    context.retain = 0;
    context.release = 0;
    context.copyDescription = 0;
    sysd->port = CFMessagePortCreateLocal(kCFAllocatorDefault, cfstr, MyCallBack, &context, 0);
    CFRunLoopRef runloop = CFRunLoopGetCurrent();
    if (sysd->port && runloop) {
        CFRunLoopSourceRef source = CFMessagePortCreateRunLoopSource(0, sysd->port, 0);
        if (source)
            CFRunLoopAddSource(runloop, source, kCFRunLoopCommonModes);
        CFRelease(source);
    }
    CFRelease(cfstr);
    if (activate) {
	connect(this, SIGNAL(messageReceived(const QString&)),
		this, SLOT(activateWindow()));
    }
}

bool QtSingleApplication::sendMessage(const QString &message, int timeout)
{
    CFStringRef cfstr = qstring2cfstring(id());
    CFMessagePortRef myport = CFMessagePortCreateRemote(kCFAllocatorDefault, cfstr);
    CFRelease(cfstr);
    if (!myport)
        return false;
    static SInt32 msgid = 0;
    const QByteArray latin1Message = message.toLatin1();
    CFDataRef data = CFDataCreate(0, (UInt8*)latin1Message.constData(), latin1Message.length());
    CFDataRef reply = 0;
    SInt32 result = CFMessagePortSendRequest(myport, ++msgid, data, timeout / 1000,
                                             timeout / 1000, 0, &reply);
    CFRelease(data);
    if (reply)
        CFRelease(reply);
    return result == kCFMessagePortSuccess;

}
