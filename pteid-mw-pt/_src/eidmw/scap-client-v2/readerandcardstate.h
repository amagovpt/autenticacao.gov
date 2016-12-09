#ifndef READERANDCARDSTATE_H
#define READERANDCARDSTATE_H

#include <QObject>
#include <QEvent>

#include "cardevent.h"
#include <eidlib.h>

class ReaderAndCardState : public QObject {
    Q_OBJECT

public:
    explicit ReaderAndCardState(QObject *parent = 0);
    bool m_abort;
public slots:
    void doWork();
    void stopWork();
private slots:
    void customEvent( QEvent * event );
signals:
    void error(QString msg);
    void cardStatusChanged(CardEvent::eCustomEventType);
};

//***********************************************
// callback data class
// This class can be used to store whatever info we
// need from the callback function
//***********************************************
class CallBackData
{
public:
    CallBackData( void )
        : m_readerName()
        , m_mainWnd()
        , m_cardID(0)
    {
    }
    CallBackData( const char* pReaderName, ReaderAndCardState* pMainWnd )
        : m_readerName(pReaderName)
        , m_mainWnd(pMainWnd)
        , m_cardID(0)
    {
    }
    virtual ~CallBackData( void )
    {
    }
    QString const& getReaderName()
    {
        return m_readerName;
    }
    void setReaderName( QString const& readerName)
    {
        m_readerName = readerName;
    }
    void setReaderName( const char* readerName)
    {
        m_readerName = readerName;
    }
    ReaderAndCardState* getMainWnd( void )
    {
        return m_mainWnd;
    }
private:
    QString			m_readerName;
    ReaderAndCardState*		m_mainWnd;
public:
    unsigned long 		m_cardID;
};


#endif // READERANDCARDSTATE_H
