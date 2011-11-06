//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to use the eID SDK to add events on a card reader
//*****************************************************************************
#include <iostream>
#include <map>
#include <string.h>

#ifdef WIN32
#include <conio.h>
#include <windows.h>
#endif

#include "eidlib.h"
#include "eidlibException.h"

using namespace eIDMW;

struct ReaderRef
{
	PTEID_ReaderContext *reader;
	unsigned long eventHandle;
    unsigned long cardId;
};

typedef std::map<char *, ReaderRef*> ReadersMap;
typedef std::map<char *, ReaderRef*>::iterator ReadersMapIt;
ReadersMap myReadersMap;

//*****************************************************************************
// Callback function
//*****************************************************************************
void callback(long lRet, unsigned long ulState, void *pvRef)
{
    try
	{
		const char *action = "";

		char * readerName = (char *)pvRef;
		ReaderRef *ref = myReadersMap[readerName];

		bool bChange = false;

		if(ref->reader->isCardPresent())
		{
			if(ref->reader->isCardChanged(ref->cardId))
			{
				action = "inserted in";
				bChange = true;
			}
		}
		else
		{
			if(ref->cardId != 0)
			{
				action = "removed from";
				bChange = true;
			}
		}

		if(bChange)
		{
			std::cout << std::endl << "A card has been " << action << " the reader : " << readerName << std::endl;
		}
	}
    catch(PTEID_Exception &ex)
	{
        std::cout << "PTEID_Exception exception" << std::endl;
	}
    catch(...)
	{
        std::cout << "Other exception" << std::endl;
	}
}

//*****************************************************************************
// Attach events on all readers
//*****************************************************************************
void attachEvents( void )
{
    try
	{
        PTEID_ReaderContext *reader;
        ReaderRef *ref;

		std::string readerName;
		char *pReaderName=NULL;

        for(unsigned long i = 0;i<ReaderSet.readerCount();i++)
		{
            reader = &ReaderSet.getReaderByNum(i);

            readerName = ReaderSet.getReaderName(i);
			pReaderName = new char[readerName.size()+1];
			strcpy(pReaderName,readerName.c_str());

            ref = new ReaderRef();

            ref->reader = reader;
            ref->cardId = 0;
            myReadersMap[pReaderName]=ref;
            ref->eventHandle = reader->SetEventCallback(callback, (void*)pReaderName);
		}
	}
    catch(PTEID_Exception &ex)
	{
        std::cout << "PTEID_Exception exception" << std::endl;
	}
    catch(...)
	{
        std::cout << "Other exception" << std::endl;
	}
}

//*****************************************************************************
// Detach events on all readers
//*****************************************************************************
void detachEvents( void )
{
    try
	{
        PTEID_ReaderContext *reader;
        ReaderRef *ref;

		char *pName=NULL;

 		ReadersMapIt itr;
		for(itr=myReadersMap.begin();itr!=myReadersMap.end();itr++)
		{
			pName=itr->first;
			ref = itr->second;
            reader = ref->reader;
            reader->StopEventCallback(ref->eventHandle);

			delete ref;
			delete[] pName;
		}
		myReadersMap.clear();
	}
    catch(PTEID_Exception &ex)
	{
        std::cout << "PTEID_Exception exception" << std::endl;
	}
    catch(...)
	{
        std::cout << "Other exception" << std::endl;
	}
}

//*****************************************************************************
// scan all the card readers and if a card is present, show the content of the
// card.
//*****************************************************************************
void scanReaders( void )
{
	if(ReaderSet.isReadersChanged())
	{
		detachEvents();
		unsigned long count = ReaderSet.readerCount(true); //Force the read of reader list
        attachEvents();

       std::cout << std::endl << "Readers has been plugged/unplugged" << std::endl << "    Number of readers : " << count;
	}
}

//*****************************************************************************
// Main entry point
//*****************************************************************************
int main( int argc, char* argv[] )
{
	std::cout << "eID SDK sample program: events_eid" << std::endl;
	std::cout << "   Insert/Remove card to call event callback" << std::endl;
	std::cout << "   Plug/Unplug reader to check changes" << std::endl;
	std::cout << "   Press 'q' to stop" << std::endl;
	std::cout << std::endl;

	unsigned long delay = 2000;

	attachEvents();

	while(true)
	{

#ifdef WIN32
		Sleep(delay);
#else
		usleep(delay * 1000);
#endif

		scanReaders();	

		std::string myStr;

		std::cin >> myStr;

		if(myStr=="q")
		{
			break;
		}
		else
		{
			std::cout << "Press 'q' to stop" << std::endl;
		}
	}

	detachEvents();

	PTEID_ReleaseSDK();

}


