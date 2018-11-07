#include <QThread>
#include <QCoreApplication>

#include "readerandcardstate.h"
#include "portablesleep.h"
#include "eidlib.h"
#include "eidlibException.h"
#include <iostream>


using namespace eIDMW;

/*
 * ReaderAndCardState
 *
 * Check if the card reader is connected to the computer. It will continue running until the
 * program closes or a card reader is found. After the card reader is found, it will check
 * for card events (inserted, removed) and will notify the main thread of this ocurrences
 *
 * This class contains locking methods and should be run in a separate thread.
 *
 */

ReaderAndCardState::ReaderAndCardState(QObject *parent) : QObject(parent), m_abort(false){
}

//****************************************************
// Callback function used by the Readercontext to notify insertion/removal
// of a card
// The callback comes at:
// - insertion of a card
// - removal of a card
// When a card is inserted we post a custom event to the ReaderAndCardState thread
// tealing a new card is inserted. A postEvent is called because this function is
// called from another thread outside ReaderAndCardState context.
//****************************************************

void cardEventCallback(long lRet, unsigned long ulState, CallBackData* pCallBackData){
    CardEvent::eCustomEventType cardEvent;
    try
    {
        PTEID_ReaderContext& readerContext = ReaderSet.getReader();
        std::cout << "CallEventCallback - Is card present: " << readerContext.isCardPresent() << std::endl << std::flush;

        if(readerContext.isCardPresent()){
            cardEvent = CardEvent::ET_CARD_INSERTED;
        }else{
            cardEvent = CardEvent::ET_CARD_REMOVED;
        }
    }
    catch(PTEID_ExBadTransaction& e){
        std::cout << "Error: " << e.GetError() << std::endl;
        cardEvent = CardEvent::ET_UNKNOWN;
    }
    CardEvent* event = new CardEvent(pCallBackData->getReaderName(), cardEvent);
    QCoreApplication::postEvent(pCallBackData->getMainWnd(), event);
}

/*
 * When a custom event is received, it will emit a signal to notify GUI
 * that it needs to be updated according the new card status
 */

void ReaderAndCardState::customEvent( QEvent* pEvent ){
    CardEvent*	cardEvent	= (CardEvent*)pEvent;
    std::cout << "Custom event. Card Event: " << cardEvent->getType()  << std::endl << std::flush;

    // Emit so GUI updates to new card status
    emit cardStatusChanged(cardEvent->getType());
}

/*
 * Responsible for checking reader status and, if reader is connected, set
 * the callback for card activity
 */

#define NR_RETRIES 7
void ReaderAndCardState::doWork(){
    ulong millisecs = 2000;
    do{
        try{
            ReaderSet.releaseReaders();
            PTEID_ReaderContext& readerContext = ReaderSet.getReader();

            std::cout << "Reading card" << std::endl << std::flush;

            if (!readerContext.isCardPresent())
            {
                std::cout << "No card found in the reader!" << std::endl << std::flush;
                emit cardStatusChanged(CardEvent::ET_NO_CARD);
            }else{
                std::cout << "Card found!" << std::endl << std::flush;

                const char* readerName = ReaderSet.getReaderName(0);
                CallBackData*		 pCBData		= new CallBackData(readerName, this);

                // Sets callback for card events
                void (*fCallback)(long lRet, unsigned long ulState, void* pCBData);
                fCallback = (void (*)(long,unsigned long,void *))&cardEventCallback;
                readerContext.SetEventCallback(fCallback, pCBData);

                // Stops thread
                m_abort = true;
            }
        }
        catch(PTEID_Exception &e){
            std::cout << "Caught exception getting reader. Error code: " << e.GetError() << std::endl;
            emit cardStatusChanged(CardEvent::ET_NO_READER);
        }

        // Uses portablesleep to be compatible with multiple OS
        PortableSleep::msleep(millisecs);
    }while(!m_abort);
}

void ReaderAndCardState::stopWork(){
    m_abort = true;
}
