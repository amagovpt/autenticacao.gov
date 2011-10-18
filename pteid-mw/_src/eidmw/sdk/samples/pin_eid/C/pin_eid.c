#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eidlibC.h"

//*****************************************************************************
// verify the pin for the card in the reader
//*****************************************************************************
void verifyPin( void )
{
	long h;
	BEID_Status status;
	BEID_Pins pins;
	BEID_Pin pin;
	long lTriesLeft=0;

	status = BEID_Init("",0,0,&h);
	if(status.general!=BEID_OK)
	{
		printf("verify pin failed (Error code = %ld)\n",status.general);
		return;
	}

	status = BEID_GetPINs(&pins);
	if(status.general==BEID_E_PCSC)
	{
		printf("No card present\n");
		return;
	}
	if(status.general==BEID_E_UNKNOWN_CARD)
	{
		printf("This is not an eid card\n");
		return;
	}
	if(status.general!=BEID_OK)
	{
		printf("verify pin failed (Error code = %ld)\n",status.general);
		return;
	}

	pin.pinType   = pins.pins[0].pinType;
	pin.id		  = pins.pins[0].id;
	pin.usageCode = pins.pins[0].usageCode;
	pin.shortUsage = NULL;
	pin.longUsage  = NULL;
	status = BEID_VerifyPIN(&pin, NULL, &lTriesLeft);
	if(status.general==BEID_E_KEYPAD_CANCELLED)
	{
		printf("verify pin canceled\n");
		return;
	}
	if(status.general==BEID_E_KEYPAD_PIN_MISMATCH)
	{
		printf("verify pin failed (Tries left = %ld)\n",lTriesLeft);
		return;
	}
	if(status.general!=BEID_OK)
	{
		printf("verify pin failed (Error code = %ld)\n",status.general);
		return;
	}

	printf("verify pin succeeded\n");
}

//*****************************************************************************
// change the pin for the card in the reader
//*****************************************************************************
void changePin( void )
{
	long h;
	BEID_Status status;
	BEID_Pins pins;
	BEID_Pin pin;
	long lTriesLeft=0;

	status = BEID_Init("",0,0,&h);
	if(status.general!=BEID_OK)
	{
		printf("change pin failed (Error code = %ld)\n",status.general);
		return;
	}

	status = BEID_GetPINs(&pins);
	if(status.general==BEID_E_PCSC)
	{
		printf("No card present\n");
		return;
	}
	if(status.general==BEID_E_UNKNOWN_CARD)
	{
		printf("This is not an eid card\n");
		return;
	}
	if(status.general!=BEID_OK)
	{
		printf("change pin failed (Error code = %ld)\n",status.general);
		return;
	}

	pin.pinType   = pins.pins[0].pinType;
	pin.id		  = pins.pins[0].id;
	pin.usageCode = pins.pins[0].usageCode;
	pin.shortUsage = NULL;
	pin.longUsage  = NULL;
	status = BEID_ChangePIN(&pin, NULL, NULL, &lTriesLeft);
	if(status.general==BEID_E_KEYPAD_CANCELLED)
	{
		printf("change pin canceled\n");
		return;
	}
	if(status.general==BEID_E_KEYPAD_PIN_MISMATCH)
	{
		printf("change pin failed (Tries left = %ld)\n",lTriesLeft);
		return;
	}
	if(status.general!=BEID_OK)
	{
		printf("change pin failed (Error code = %ld)\n",status.general);
		return;
	}

	printf("change pin succeeded\n");

}

int main(int argc, char* argv[])
{
	char c;
	int iStop;

	printf("eID SDK sample program: pin_eid\n");
	printf("   Hit v to verify the pin\n");
	printf("       c to change the pin\n");
	printf("       q to quit\n");
	printf("\n");

	iStop = 0;

	while(!iStop)
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
			iStop=1;
			break;
		}
	}

	BEID_Exit();

}

