//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to implement WaitForCardPresent and WaitForCardAbsent functions
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

PTEID_ReaderSet *m_ReaderSet=NULL;

//*****************************************************************************
// Exception that will be generated in case of timeout
//*****************************************************************************
class TimeoutException : public std::exception
{
public:
	TimeoutException()
	{
	}
	virtual const char* what() const throw()
  	{
	    return "Timeout";
	}
};

//*****************************************************************************
// WaitForCardPresent function
// This function hangs until a card has been inserted into the reader
// Or for a maximum of Timeout seconds
// It returns the reader context or throw an exception if a timeout occured
//*****************************************************************************
PTEID_ReaderContext& WaitForCardPresent(int Timeout)
{
    PTEID_ReaderContext *reader=NULL;
    try
    {
        bool bContinue  = true;
        int  Count		= 0;

        while(bContinue)
        {
			//---------------------------------------------------
			//Get a reader. getReader (without parameter) returns the first reader with a card.
			//or the first reader (if no card is present)
			//if no reader is connected, an PTEID_ExNoReader exception is thrown
			//---------------------------------------------------
            reader = &m_ReaderSet->getReader();

            if(reader->isCardPresent())
            {
                bContinue=false;
            }
			//---------------------------------------------------
			//If timeout occured, we leave the loop and throw the exception
			//---------------------------------------------------
            else if (Count > Timeout)
            {
				bContinue=false;
				TimeoutException *e = new TimeoutException();
				throw(e);
            }
			//---------------------------------------------------
			//Else we sleep 1 second and check again
			//---------------------------------------------------
            else
            {
#ifdef WIN32
				Sleep(1000);
#else
				usleep(1000 * 1000);
#endif
                Count++;
            }
        }
		if(!bContinue)
			return *reader;
    }
    catch (PTEID_ExNoReader &ex)
    {
		std::cout << "No reader connected" << std::endl;
    }
    catch (PTEID_Exception &ex)
    {
 		std::cout << "PTEID_Exception : " << ex.GetError() << std::endl;
    }
	catch ( TimeoutException* ex)
	{
		throw;
	}
    catch (...)
    {
		std::cout << "System.Exception!" << std::endl;
		throw;
	}

    throw(new std::exception());
}

//*****************************************************************************
// WaitForCardAbsent Function
// This function hangs until no more card is pluged in the reader
// Or for a maximum of Timeout seconds
// It returns true if the no card is inserted and false if timeout occured
//*****************************************************************************
bool WaitForCardAbsent(int Timeout)
{
    try
    {
        PTEID_ReaderContext* reader = NULL;
        int					Count  = 0;

        while (true)
        {
			//---------------------------------------------------
			//Get a reader. getReader (without parameter) return the first reader with a card.
			//or the first reader (if no card is present)
			//if no reader is connected, an PTEID_ExNoReader exception is thrown
			//---------------------------------------------------
            reader = &m_ReaderSet->getReader();

			//---------------------------------------------------
			//If no card is present, we return true
			//---------------------------------------------------
            if (!reader->isCardPresent())
            {
                return true;
            }
			//---------------------------------------------------
			//If timeout occured, we return false
			//---------------------------------------------------
            else if (Count > Timeout)
            {
                return false;
            }
			//---------------------------------------------------
			//Else we sleep 1 second and check again
			//---------------------------------------------------
            else
            {
#ifdef WIN32
				Sleep(1000);
#else
				usleep(1000 * 1000);
#endif
                Count++;
            }
        }
    }
    catch (PTEID_ExNoReader &ex)
    {
 		std::cout << "No reader connected" << std::endl;
    }
    catch (PTEID_Exception &ex)
    {
 		std::cout << "PTEID_Exception : " << ex.GetError() << std::endl;
    }
    catch (...)
    {
		std::cout << "System.Exception!" << std::endl;
    }

    return false;
}


//*****************************************************************************
// Main entry point
//*****************************************************************************
int main( int argc, char* argv[] )
{
#define _TIMEOUT 5

	m_ReaderSet = &PTEID_ReaderSet::instance();

	std::cout << "eID SDK sample program: wait_card" << std::endl;
	std::cout << "   Follow the instruction" << std::endl;
	std::cout << std::endl;

    try
    {
		std::string sName="";

        std::cout << "Please insert your card" << std::endl;

		//---------------------------------------------------
		//Get the reader in which a card is inserted (timeout = 5 sec)
		//---------------------------------------------------
        PTEID_ReaderContext &reader = WaitForCardPresent(_TIMEOUT);

		//---------------------------------------------------
		//Get the card (if it's not an eid card, an exception is thrown)
		//---------------------------------------------------
        PTEID_EIDCard &card = reader.getEIDCard();

		card.setAllowTestCard(true);	//Not needed for real Belgian ID card

		//---------------------------------------------------
		//Get data from the card
		//---------------------------------------------------
        sName = card.getID().getFirstName();
		sName.append(" ");
		sName.append(card.getID().getSurname());

        std::cout << sName.c_str() << ", please remove your card." << std::endl;

		//---------------------------------------------------
		//Wait until the card has been remove (timeout = 5 sec)
		//---------------------------------------------------
        if (!WaitForCardAbsent(_TIMEOUT))
        {
			//---------------------------------------------------
			//Remind the user to remove his card with a beep
			//---------------------------------------------------
            while (!WaitForCardAbsent(1))
            {
                std::cout << sName.c_str() << ", don't forget to remove your card!" << std::endl;
				std::cout << '\a' << std::flush;
            }
        }
        std::cout << "Thanks!" << std::endl;
    }
    catch (PTEID_Exception &ex)
    {
 		std::cout << "PTEID_Exception : " << ex.GetError() << std::endl;
    }
    catch (TimeoutException *ex)
    {
		std::cout << "You haven't inserted a card." << std::endl;
    }
    catch (std::exception& ex)
    {
		std::cout << "System.Exception!" << std::endl;
    }
    catch (...)
    {
		std::cout << "System.Exception!" << std::endl;
    }

	//---------------------------------------------------
	//Release the sdk
	//---------------------------------------------------
	PTEID_ReleaseSDK();
}


