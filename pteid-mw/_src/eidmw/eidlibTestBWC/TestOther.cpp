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
#include "TestOther.h"

void PrintTestOther(FILE *f, int Ocsp, int Crl)
{
    BEID_Status tStatus = {0};
    BEID_Bytes Application = {0};
    BEID_Bytes APDU = {0};
    BEID_Bytes Response = {0};
    BEID_Bytes FileID = {0};
    BEID_Bytes Data = {0};
	BEID_Pins Pins = {0};
	BEID_Pin Pin = {0};

	PrintTestFunction(f,"BEID_GetPINs");
	tStatus = BEID_GetPINs(&Pins);
	PrintStatus(f,"BEID_GetPINs",&tStatus);

	Pin.pinType   = Pins.pins[0].pinType;
	Pin.id		  = Pins.pins[0].id;
	Pin.usageCode = Pins.pins[0].usageCode;
	Pin.shortUsage = NULL;
	Pin.longUsage  = NULL;

	//BEID_ReadFile

	Response.length = BEID_MAX_PICTURE_LEN;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	Application.length = 12;
 	Application.data = (unsigned char *)"\xA0\x00\x00\x01\x77\x50\x4B\x43\x53\x2D\x31\x35";
	FileID.length = 2;
    FileID.data = (unsigned char *)"\x50\x38";

 	PrintTestFunction(f,"BEID_SelectApplication");
    tStatus = BEID_SelectApplication(&Application);
	PrintStatus(f,"BEID_SelectApplication",&tStatus);

 	PrintTestFunction(f,"BEID_ReadFile");
    tStatus = BEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"BEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);

/* DOES NOT WORK WITH MW 2.6
	Response.length = BEID_MAX_PICTURE_LEN;
	memset(Response.data,0,Response.length);

	Application.length = 12;
	Application.data = (unsigned char *)"\xA0\x00\x00\x01\x77\x49\x64\x46\x69\x6C\x65\x73";
	FileID.length = 2;
    FileID.data = (unsigned char *)"\x40\x35";

 	PrintTestFunction(f,"BEID_SelectApplication");
    tStatus = BEID_SelectApplication(&Application);
	PrintStatus(f,"BEID_SelectApplication",&tStatus);

 	PrintTestFunction(f,"BEID_ReadFile");
    tStatus = BEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"BEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);
*/

/* DOES NOT WORK WITH MW 2.6
	Response.length = BEID_MAX_PICTURE_LEN;
	memset(Response.data,0,Response.length);

	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x35";

 	PrintTestFunction(f,"BEID_ReadFile");
    tStatus = BEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"BEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);

	Response.length = BEID_MAX_PICTURE_LEN;
	memset(Response.data,0,Response.length);
*/

/* DOES NOT WORK WITH MW 2.6
	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x00\x50\x38";

 	PrintTestFunction(f,"BEID_ReadFile");
    tStatus = BEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"BEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);
*/

	free(Response.data);

	//BEID_WriteFile
	PrintTestFunction(f,"BEID_WriteFile");
	PrintComment(f,"THIS FUNCTION DOES NOT WORK PROPERLY WITH MW 2.6");

/* DOES NOT WORK WITH MW 2.6

	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x31";
	Data.length =6;
	Data.data = "\x5b\x54\x65\x73\x74\x5d";

	PrintTestFunction(f,"BEID_WriteFile");
	PrintComment(f,"Trying to write into ID file");
	tStatus = BEID_WriteFile(&FileID,&Data,&Pin);
	PrintStatus(f,"BEID_WriteFile",&tStatus);

	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x39";
	Data.length =6;
	Data.data = "\x5b\x54\x65\x73\x74\x5d";

	Response.length = 150;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"BEID_ReadFile");
	PrintComment(f,"Read the Preference file");
	tStatus = BEID_ReadFile(&FileID,&Response,&Pin);
	PrintStatus(f,"BEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);

	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x39";
	Data.length =6;
	Data.data = "\x5b\x54\x65\x73\x74\x5d";

	PrintTestFunction(f,"BEID_WriteFile");
	PrintComment(f,"Write into the Preference file");
	tStatus = BEID_WriteFile(&FileID,&Data,&Pin);
	PrintStatus(f,"BEID_WriteFile",&tStatus);

	Response.length = 150;
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"BEID_ReadFile");
	PrintComment(f,"Read the preference file to check the data written");
    tStatus = BEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"BEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);

	PrintTestFunction(f,"BEID_WriteFile");
	PrintComment(f,"Rewrite into the Preference file with initial content");
	tStatus = BEID_WriteFile(&FileID,&Response,&Pin);
	PrintStatus(f,"BEID_WriteFile",&tStatus);

	free(Response.data);
*/

	//BEID_ReadBinary
	PrintTestFunction(f,"BEID_ReadBinary");
	PrintComment(f,"THIS FUNCTION DOES NOT WORK PROPERLY WITH MW 2.6");

/* DOES NOT WORK WITH MW 2.6
	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x31";

	Response.length = 128;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"BEID_ReadBinary");
	tStatus = BEID_ReadBinary(&FileID,64,512,&Response);
	PrintStatus(f,"BEID_ReadBinary",&tStatus);

	PrintBytes(f,"FILE",&Response);

	free(Response.data);


	Response.length = 512;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"BEID_ReadBinary");
	tStatus = BEID_ReadBinary(&FileID,64,512,&Response);
	PrintStatus(f,"BEID_ReadBinary",&tStatus);

	PrintBytes(f,"FILE",&Response);

	free(Response.data);

	Response.length = 64;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"BEID_ReadBinary");
	tStatus = BEID_ReadBinary(&FileID,64,64,&Response);
	PrintStatus(f,"BEID_ReadBinary",&tStatus);

	PrintBytes(f,"FILE",&Response);

	free(Response.data);
*/

	//BEID_BeginTransaction
	PrintTestFunction(f,"BEID_BeginTransaction");
	tStatus = BEID_BeginTransaction();
	PrintStatus(f,"BEID_BeginTransaction",&tStatus);

	PrintTestFunction(f,"BEID_BeginTransaction");
	tStatus = BEID_BeginTransaction();
	PrintStatus(f,"BEID_BeginTransaction",&tStatus);


	//BEID_SendAPDU
	//Get serial number with to short buffer
	APDU.length = 5;
	APDU.data = (unsigned char *)"\x80\xE4\x00\x00\x0A";

	Response.length = APDU.data[4]+2;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"BEID_SendAPDU");
	tStatus = BEID_SendAPDU(&APDU,NULL,&Response);
	PrintStatus(f,"BEID_SendAPDU",&tStatus);

	PrintBytes(f,"RESPONSE",&Response);

	PrintTestFunction(f,"BEID_SendAPDU");
	tStatus = BEID_SendAPDU(&APDU,&Pin,&Response);
	PrintStatus(f,"BEID_SendAPDU",&tStatus);

	PrintBytes(f,"RESPONSE",&Response);

	free(Response.data);

	//Get serial number
	APDU.length = 5;
	APDU.data = (unsigned char *)"\x80\xE4\x00\x00\x1C";

	Response.length = APDU.data[4]+2;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"BEID_SendAPDU");
	tStatus = BEID_SendAPDU(&APDU,&Pin,&Response);
	PrintStatus(f,"BEID_SendAPDU",&tStatus);

	PrintBytes(f,"RESPONSE",&Response);

	free(Response.data);

	//BEID_EndTransaction
	PrintTestFunction(f,"BEID_EndTransaction");
	tStatus = BEID_EndTransaction();
	PrintStatus(f,"BEID_EndTransaction",&tStatus);

	PrintTestFunction(f,"BEID_EndTransaction");
	tStatus = BEID_EndTransaction();
	PrintStatus(f,"BEID_EndTransaction",&tStatus);

	//BEID_FlushCache
	PrintTestFunction(f,"BEID_FlushCache");
	tStatus = BEID_FlushCache();
	PrintStatus(f,"BEID_FlushCache",&tStatus);
}

int test_Other(const char *folder, const char *reader, int bVerify, int Ocsp, int Crl)
{
	return test_Basic("other", &PrintTestOther, NULL, folder, reader, bVerify, Ocsp, Crl);	
}