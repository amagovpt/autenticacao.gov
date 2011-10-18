/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include <stdlib.h>
#include <string.h>

#include "FileUtil.h"
#include "PrintBasic.h"
#include "PrintStruct.h"

#include "TestBasic.h"
#include "TestPin.h"

void PrintTestPin(FILE *f, int Ocsp, int Crl)
{
    BEID_Status tStatus = {0};
	BEID_Pins pins = {0};
	BEID_Pin pin = {0};
	BEID_Bytes signedStatus = {0};
	long lTriesLeft = 0;
	char buffer[50];

	//BEID_GetPINs
	PrintTestFunction(f,"BEID_GetPINs");
	tStatus = BEID_GetPINs(&pins);
	PrintStatus(f,"BEID_GetPINs",&tStatus);
	if(tStatus.general==BEID_E_PCSC)
	{
		PrintWARNING(f,"No card present");
		PrintPins(f,&pins);
		return;
	}
	if(tStatus.general==BEID_E_UNKNOWN_CARD)
	{
		PrintWARNING(f,"This is not an eid card");
		PrintPins(f,&pins);
		return;
	}
	if(tStatus.general!=BEID_OK)
	{
		if(tStatus.cardSW[0]!=0x62 && tStatus.cardSW[0]!=0x82)	//Avoid warning in virtual context
		{
			sprintf_s(buffer,sizeof(buffer),"Get pin failed (Error code = %ld)",tStatus.general);
			PrintWARNING(f,buffer);
		}
		PrintPins(f,&pins);
		return;
	}

	PrintPins(f,&pins);

	pin.pinType   = pins.pins[0].pinType;
	pin.id		  = pins.pins[0].id;
	pin.usageCode = pins.pins[0].usageCode;
	pin.shortUsage = NULL;
	pin.longUsage  = NULL;

	//BEID_GetPINStatus
	PrintTestFunction(f,"BEID_GetPINStatus");
	tStatus = BEID_GetPINStatus(&pin, &lTriesLeft, FALSE, NULL);
	PrintStatus(f,"BEID_GetPINStatus",&tStatus);
	if(BEID_OK != tStatus.general)
	{
		PrintWARNING(f,"GetPINStatus failed");
	}

	PrintPin(f,&pin,lTriesLeft);

	signedStatus.length		 = 10;
	signedStatus.data		 = (BYTE *)malloc(sizeof(BYTE)*signedStatus.length);
    memset(signedStatus.data, 0, signedStatus.length*sizeof(BYTE));

	PrintTestFunction(f,"BEID_GetPINStatus(Signature=TRUE)");
	tStatus = BEID_GetPINStatus(&pin, &lTriesLeft, TRUE, &signedStatus);
	PrintStatus(f,"BEID_GetPINStatus",&tStatus);
	if(BEID_E_INSUFFICIENT_BUFFER == tStatus.general)
	{
		PrintPin(f,&pin,lTriesLeft);
		PrintBytes(f,"signedStatus",&signedStatus);

		free(signedStatus.data);

		signedStatus.length		 = 1000;
		signedStatus.data		 = (BYTE *)malloc(sizeof(BYTE)*signedStatus.length);

		tStatus = BEID_GetPINStatus(&pin, &lTriesLeft, 1, &signedStatus);
		PrintStatus(f,"BEID_GetPINStatus",&tStatus);
	}

	PrintPin(f,&pin,lTriesLeft);
	PrintBytes(f,"signedStatus",&signedStatus);

	free(signedStatus.data);

	//BEID_VerifyPIN
	PrintTestFunction(f,"BEID_VerifyPIN");
	tStatus = BEID_VerifyPIN(&pin, NULL, &lTriesLeft);
	PrintStatus(f,"BEID_VerifyPIN",&tStatus);
	if(tStatus.general==BEID_E_KEYPAD_CANCELLED)
	{
		PrintWARNING(f,"verify pin canceled");
	}
	if(tStatus.general==BEID_E_KEYPAD_PIN_MISMATCH)
	{
		sprintf_s(buffer,sizeof(buffer),"verify pin failed (Tries left = %ld)",lTriesLeft);
		PrintWARNING(f,buffer);
	}
	if(tStatus.general!=BEID_OK)
	{
		sprintf_s(buffer,sizeof(buffer),"verify pin failed (Error code = %ld)",tStatus.general);
		PrintWARNING(f,buffer);
	}

	//BEID_GetPINStatus
	PrintTestFunction(f,"BEID_GetPINStatus");
	tStatus = BEID_GetPINStatus(&pin, &lTriesLeft, FALSE, NULL);
	PrintStatus(f,"BEID_GetPINStatus",&tStatus);
	if(BEID_OK != tStatus.general)
	{
		PrintWARNING(f,"GetPINStatus failed");
	}

	PrintPin(f,&pin,lTriesLeft);

	//BEID_ChangePIN
	PrintTestFunction(f,"BEID_ChangePIN");
	tStatus = BEID_ChangePIN(&pin, NULL, NULL, &lTriesLeft);
	PrintStatus(f,"BEID_ChangePIN",&tStatus);
	if(tStatus.general==BEID_E_KEYPAD_CANCELLED)
	{
		PrintWARNING(f,"change pin canceled");
	}
	if(tStatus.general==BEID_E_KEYPAD_PIN_MISMATCH)
	{
		sprintf_s(buffer,sizeof(buffer),"change pin failed (Tries left = %ld)",lTriesLeft);
		PrintWARNING(f,buffer);
	}
	if(tStatus.general!=BEID_OK)
	{
		sprintf_s(buffer,sizeof(buffer),"change pin failed (Error code = %ld)",tStatus.general);
		PrintWARNING(f,buffer);
	}
}

int test_Pin(const char *folder, const char *reader, int bVerify, int Ocsp, int Crl)
{
	return test_Basic("pin", &PrintTestPin, NULL, folder, reader, bVerify, Ocsp, Crl);	
}