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
    PTEID_Status tStatus = {0};
	PTEID_Pins pins = {0};
	PTEID_Pin pin = {0};
	PTEID_Bytes signedStatus = {0};
	long lTriesLeft = 0;
	char buffer[50];

	//PTEID_GetPINs
	PrintTestFunction(f,"PTEID_GetPINs");
	tStatus = PTEID_GetPINs(&pins);
	PrintStatus(f,"PTEID_GetPINs",&tStatus);
	if(tStatus.general==PTEID_E_PCSC)
	{
		PrintWARNING(f,"No card present");
		PrintPins(f,&pins);
		return;
	}
	if(tStatus.general==PTEID_E_UNKNOWN_CARD)
	{
		PrintWARNING(f,"This is not an eid card");
		PrintPins(f,&pins);
		return;
	}
	if(tStatus.general!=PTEID_OK)
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

	//PTEID_GetPINStatus
	PrintTestFunction(f,"PTEID_GetPINStatus");
	tStatus = PTEID_GetPINStatus(&pin, &lTriesLeft, FALSE, NULL);
	PrintStatus(f,"PTEID_GetPINStatus",&tStatus);
	if(PTEID_OK != tStatus.general)
	{
		PrintWARNING(f,"GetPINStatus failed");
	}

	PrintPin(f,&pin,lTriesLeft);

	signedStatus.length		 = 10;
	signedStatus.data		 = (BYTE *)malloc(sizeof(BYTE)*signedStatus.length);
    memset(signedStatus.data, 0, signedStatus.length*sizeof(BYTE));

	PrintTestFunction(f,"PTEID_GetPINStatus(Signature=TRUE)");
	tStatus = PTEID_GetPINStatus(&pin, &lTriesLeft, TRUE, &signedStatus);
	PrintStatus(f,"PTEID_GetPINStatus",&tStatus);
	if(PTEID_E_INSUFFICIENT_BUFFER == tStatus.general)
	{
		PrintPin(f,&pin,lTriesLeft);
		PrintBytes(f,"signedStatus",&signedStatus);

		free(signedStatus.data);

		signedStatus.length		 = 1000;
		signedStatus.data		 = (BYTE *)malloc(sizeof(BYTE)*signedStatus.length);

		tStatus = PTEID_GetPINStatus(&pin, &lTriesLeft, 1, &signedStatus);
		PrintStatus(f,"PTEID_GetPINStatus",&tStatus);
	}

	PrintPin(f,&pin,lTriesLeft);
	PrintBytes(f,"signedStatus",&signedStatus);

	free(signedStatus.data);

	//PTEID_VerifyPIN
	PrintTestFunction(f,"PTEID_VerifyPIN");
	tStatus = PTEID_VerifyPIN(&pin, NULL, &lTriesLeft);
	PrintStatus(f,"PTEID_VerifyPIN",&tStatus);
	if(tStatus.general==PTEID_E_KEYPAD_CANCELLED)
	{
		PrintWARNING(f,"verify pin canceled");
	}
	if(tStatus.general==PTEID_E_KEYPAD_PIN_MISMATCH)
	{
		sprintf_s(buffer,sizeof(buffer),"verify pin failed (Tries left = %ld)",lTriesLeft);
		PrintWARNING(f,buffer);
	}
	if(tStatus.general!=PTEID_OK)
	{
		sprintf_s(buffer,sizeof(buffer),"verify pin failed (Error code = %ld)",tStatus.general);
		PrintWARNING(f,buffer);
	}

	//PTEID_GetPINStatus
	PrintTestFunction(f,"PTEID_GetPINStatus");
	tStatus = PTEID_GetPINStatus(&pin, &lTriesLeft, FALSE, NULL);
	PrintStatus(f,"PTEID_GetPINStatus",&tStatus);
	if(PTEID_OK != tStatus.general)
	{
		PrintWARNING(f,"GetPINStatus failed");
	}

	PrintPin(f,&pin,lTriesLeft);

	//PTEID_ChangePIN
	PrintTestFunction(f,"PTEID_ChangePIN");
	tStatus = PTEID_ChangePIN(&pin, NULL, NULL, &lTriesLeft);
	PrintStatus(f,"PTEID_ChangePIN",&tStatus);
	if(tStatus.general==PTEID_E_KEYPAD_CANCELLED)
	{
		PrintWARNING(f,"change pin canceled");
	}
	if(tStatus.general==PTEID_E_KEYPAD_PIN_MISMATCH)
	{
		sprintf_s(buffer,sizeof(buffer),"change pin failed (Tries left = %ld)",lTriesLeft);
		PrintWARNING(f,buffer);
	}
	if(tStatus.general!=PTEID_OK)
	{
		sprintf_s(buffer,sizeof(buffer),"change pin failed (Error code = %ld)",tStatus.general);
		PrintWARNING(f,buffer);
	}
}

int test_Pin(const char *folder, const char *reader, int bVerify, int Ocsp, int Crl)
{
	return test_Basic("pin", &PrintTestPin, NULL, folder, reader, bVerify, Ocsp, Crl);	
}