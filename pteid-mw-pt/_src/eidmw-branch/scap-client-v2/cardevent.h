#ifndef CARDEVENT
#define CARDEVENT

#include <QEvent>

class CardEvent : public QEvent
{
public:
    enum eCustomEventType
    {
        ET_UNKNOWN
        , ET_NO_READER
        , ET_NO_CARD
        , ET_CARD_REMOVED
        , ET_CARD_INSERTED
    };
    CardEvent()
        : QEvent(QEvent::User)
        , m_cardReader("")
        , m_cardCitizenNIC("")
    {

    }

    CardEvent(QString const& reader, eCustomEventType eType )
        : QEvent(QEvent::User)
        , m_cardReader(reader)
        , m_customEventType(eType)
        , m_cardCitizenNIC("")
    {
    }
    ~CardEvent()
    {
    }
    QString const& getReaderName( void )
    {
        return m_cardReader;
    }
    eCustomEventType getType( void )
    {
        return m_customEventType;
    }

    void setEvent (eCustomEventType event)
    {
        m_customEventType = event;
    }

    void setCitizenNIC(QString citizenNIC)
    {
        m_cardCitizenNIC = citizenNIC;
    }

    QString getCardCitizenNIC ( void )
    {
        return m_cardCitizenNIC;
    }

private:
    QString m_cardReader;
    QString m_cardCitizenNIC;
    eCustomEventType m_customEventType;
};

#endif // CARDEVENT

