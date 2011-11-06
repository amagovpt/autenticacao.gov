//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to use the eID SDK to add events on a card reader
//*****************************************************************************
#include <iostream>
#include <map>

#ifdef WIN32
#include <conio.h>
#include <windows.h>
#endif

#include "eidlib.h"
#include "eidlibException.h"

using namespace eIDMW;


//*****************************************************************************
// verify the pin for the card in the reader
//*****************************************************************************
void verifyPin( void )
{
	try
	{
		unsigned long ulRemaining=0xFFFF;

		PTEID_ReaderContext &reader = ReaderSet.getReader();
		PTEID_EIDCard &card = reader.getEIDCard();

		if(card.getPins().getPinByNumber(0).verifyPin("",ulRemaining))
		{
			std::cout << "verify pin succeeded" << std::endl;
		}
		else
		{
			if(ulRemaining==0xFFFF)
				std::cout << "verify pin canceled" << std::endl;
			else
				std::cout << "verify pin failed (" << ulRemaining << " tries left)" << std::endl;
		}
	}
    catch(PTEID_ExCardBadType &ex)
	{
        std::cout << "This is not an eid card" << std::endl;
	}
    catch(PTEID_ExNoCardPresent &ex)
	{
        std::cout << "No card present" << std::endl;
	}
    catch(PTEID_ExNoReader &ex)
	{
        std::cout << "No reader found" << std::endl;
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
// change the pin for the card in the reader
//*****************************************************************************
void changePin( void )
{
	try
	{
		unsigned long ulRemaining=0xFFFF;

		PTEID_ReaderContext &reader = ReaderSet.getReader();
		PTEID_EIDCard &card = reader.getEIDCard();

		if(card.getPins().getPinByNumber(0).changePin("","",ulRemaining))
		{
			std::cout << "Change pin succeeded" << std::endl;
		}
		else
		{
			if(ulRemaining==0xFFFF)
				std::cout << "Change pin canceled" << std::endl;
			else
				std::cout << "Change pin failed (" << ulRemaining << " tries left)" << std::endl;
		}
	}
    catch(PTEID_ExCardBadType &ex)
	{
        std::cout << "This is not an eid card" << std::endl;
	}
    catch(PTEID_ExNoCardPresent &ex)
	{
        std::cout << "No card present" << std::endl;
	}
    catch(PTEID_ExNoReader &ex)
	{
        std::cout << "No reader found" << std::endl;
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
// Main entry point
//*****************************************************************************
int main( int argc, char* argv[] )
{
	std::cout << "eID SDK sample program: pin_eid" << std::endl;
	std::cout << "   Hit v to verify the pin" << std::endl;
	std::cout << "       c to change the pin" << std::endl;
	std::cout << "       q to quit" << std::endl;
	std::cout << std::endl;

	char c;

	bool bStop = false;

	while(!bStop)
	{

		c=getchar();
		switch(c)
		{
		case 'v':
		case 'V':
			verifyPin();
			break;
		case 'c':
		case 'C':
			changePin();
			break;
		case 'q':
		case 'Q':
			bStop=true;
			break;
		}
	}

	PTEID_ReleaseSDK();

}


