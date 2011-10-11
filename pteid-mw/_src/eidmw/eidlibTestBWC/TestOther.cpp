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
    PTEID_Status tStatus = {0};
    PTEID_Bytes Application = {0};
    PTEID_Bytes APDU = {0};
    PTEID_Bytes Response = {0};
    PTEID_Bytes FileID = {0};
    PTEID_Bytes Data = {0};
	PTEID_Pins Pins = {0};
	PTEID_Pin Pin = {0};

	PrintTestFunction(f,"PTEID_GetPINs");
	tStatus = PTEID_GetPINs(&Pins);
	PrintStatus(f,"PTEID_GetPINs",&tStatus);

	Pin.pinType   = Pins.pins[0].pinType;
	Pin.id		  = Pins.pins[0].id;
	Pin.usageCode = Pins.pins[0].usageCode;
	Pin.shortUsage = NULL;
	Pin.longUsage  = NULL;

	//PTEID_ReadFile

	Response.length = PTEID_MAX_PICTURE_LEN;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	Application.length = 12;
 	Application.data = (unsigned char *)"\xA0\x00\x00\x01\x77\x50\x4B\x43\x53\x2D\x31\x35";
	FileID.length = 2;
    FileID.data = (unsigned char *)"\x50\x38";

 	PrintTestFunction(f,"PTEID_SelectApplication");
    tStatus = PTEID_SelectApplication(&Application);
	PrintStatus(f,"PTEID_SelectApplication",&tStatus);

 	PrintTestFunction(f,"PTEID_ReadFile");
    tStatus = PTEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"PTEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);

/* DOES NOT WORK WITH MW 2.6
	Response.length = PTEID_MAX_PICTURE_LEN;
	memset(Response.data,0,Response.length);

	Application.length = 12;
	Application.data = (unsigned char *)"\xA0\x00\x00\x01\x77\x49\x64\x46\x69\x6C\x65\x73";
	FileID.length = 2;
    FileID.data = (unsigned char *)"\x40\x35";

 	PrintTestFunction(f,"PTEID_SelectApplication");
    tStatus = PTEID_SelectApplication(&Application);
	PrintStatus(f,"PTEID_SelectApplication",&tStatus);

 	PrintTestFunction(f,"PTEID_ReadFile");
    tStatus = PTEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"PTEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);
*/

/* DOES NOT WORK WITH MW 2.6
	Response.length = PTEID_MAX_PICTURE_LEN;
	memset(Response.data,0,Response.length);

	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x35";

 	PrintTestFunction(f,"PTEID_ReadFile");
    tStatus = PTEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"PTEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);

	Response.length = PTEID_MAX_PICTURE_LEN;
	memset(Response.data,0,Response.length);
*/

/* DOES NOT WORK WITH MW 2.6
	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x00\x50\x38";

 	PrintTestFunction(f,"PTEID_ReadFile");
    tStatus = PTEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"PTEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);
*/

	free(Response.data);

	//PTEID_WriteFile
	PrintTestFunction(f,"PTEID_WriteFile");
	PrintComment(f,"THIS FUNCTION DOES NOT WORK PROPERLY WITH MW 2.6");

/* DOES NOT WORK WITH MW 2.6

	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x31";
	Data.length =6;
	Data.data = "\x5b\x54\x65\x73\x74\x5d";

	PrintTestFunction(f,"PTEID_WriteFile");
	PrintComment(f,"Trying to write into ID file");
	tStatus = PTEID_WriteFile(&FileID,&Data,&Pin);
	PrintStatus(f,"PTEID_WriteFile",&tStatus);

	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x39";
	Data.length =6;
	Data.data = "\x5b\x54\x65\x73\x74\x5d";

	Response.length = 150;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"PTEID_ReadFile");
	PrintComment(f,"Read the Preference file");
	tStatus = PTEID_ReadFile(&FileID,&Response,&Pin);
	PrintStatus(f,"PTEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);

	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x39";
	Data.length =6;
	Data.data = "\x5b\x54\x65\x73\x74\x5d";

	PrintTestFunction(f,"PTEID_WriteFile");
	PrintComment(f,"Write into the Preference file");
	tStatus = PTEID_WriteFile(&FileID,&Data,&Pin);
	PrintStatus(f,"PTEID_WriteFile",&tStatus);

	Response.length = 150;
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"PTEID_ReadFile");
	PrintComment(f,"Read the preference file to check the data written");
    tStatus = PTEID_ReadFile(&FileID, &Response, &Pin);
	PrintStatus(f,"PTEID_ReadFile",&tStatus);

	PrintBytes(f,"FILE",&Response);

	PrintTestFunction(f,"PTEID_WriteFile");
	PrintComment(f,"Rewrite into the Preference file with initial content");
	tStatus = PTEID_WriteFile(&FileID,&Response,&Pin);
	PrintStatus(f,"PTEID_WriteFile",&tStatus);

	free(Response.data);
*/

	//PTEID_ReadBinary
	PrintTestFunction(f,"PTEID_ReadBinary");
	PrintComment(f,"THIS FUNCTION DOES NOT WORK PROPERLY WITH MW 2.6");

/* DOES NOT WORK WITH MW 2.6
	FileID.length = 6;
	FileID.data = (unsigned char *)"\x3F\x00\xDF\x01\x40\x31";

	Response.length = 128;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"PTEID_ReadBinary");
	tStatus = PTEID_ReadBinary(&FileID,64,512,&Response);
	PrintStatus(f,"PTEID_ReadBinary",&tStatus);

	PrintBytes(f,"FILE",&Response);

	free(Response.data);


	Response.length = 512;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"PTEID_ReadBinary");
	tStatus = PTEID_ReadBinary(&FileID,64,512,&Response);
	PrintStatus(f,"PTEID_ReadBinary",&tStatus);

	PrintBytes(f,"FILE",&Response);

	free(Response.data);

	Response.length = 64;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"PTEID_ReadBinary");
	tStatus = PTEID_ReadBinary(&FileID,64,64,&Response);
	PrintStatus(f,"PTEID_ReadBinary",&tStatus);

	PrintBytes(f,"FILE",&Response);

	free(Response.data);
*/

	//PTEID_BeginTransaction
	PrintTestFunction(f,"PTEID_BeginTransaction");
	tStatus = PTEID_BeginTransaction();
	PrintStatus(f,"PTEID_BeginTransaction",&tStatus);

	PrintTestFunction(f,"PTEID_BeginTransaction");
	tStatus = PTEID_BeginTransaction();
	PrintStatus(f,"PTEID_BeginTransaction",&tStatus);


	//PTEID_SendAPDU
	//Get serial number with to short buffer
	APDU.length = 5;
	APDU.data = (unsigned char *)"\x80\xE4\x00\x00\x0A";

	Response.length = APDU.data[4]+2;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"PTEID_SendAPDU");
	tStatus = PTEID_SendAPDU(&APDU,NULL,&Response);
	PrintStatus(f,"PTEID_SendAPDU",&tStatus);

	PrintBytes(f,"RESPONSE",&Response);

	PrintTestFunction(f,"PTEID_SendAPDU");
	tStatus = PTEID_SendAPDU(&APDU,&Pin,&Response);
	PrintStatus(f,"PTEID_SendAPDU",&tStatus);

	PrintBytes(f,"RESPONSE",&Response);

	free(Response.data);

	//Get serial number
	APDU.length = 5;
	APDU.data = (unsigned char *)"\x80\xE4\x00\x00\x1C";

	Response.length = APDU.data[4]+2;
	Response.data = (BYTE *)malloc(Response.length*sizeof(BYTE));
	memset(Response.data,0,Response.length);

	PrintTestFunction(f,"PTEID_SendAPDU");
	tStatus = PTEID_SendAPDU(&APDU,&Pin,&Response);
	PrintStatus(f,"PTEID_SendAPDU",&tStatus);

	PrintBytes(f,"RESPONSE",&Response);

	free(Response.data);

	//PTEID_EndTransaction
	PrintTestFunction(f,"PTEID_EndTransaction");
	tStatus = PTEID_EndTransaction();
	PrintStatus(f,"PTEID_EndTransaction",&tStatus);

	PrintTestFunction(f,"PTEID_EndTransaction");
	tStatus = PTEID_EndTransaction();
	PrintStatus(f,"PTEID_EndTransaction",&tStatus);

	//PTEID_FlushCache
	PrintTestFunction(f,"PTEID_FlushCache");
	tStatus = PTEID_FlushCache();
	PrintStatus(f,"PTEID_FlushCache",&tStatus);
}

int test_Other(const char *folder, const char *reader, int bVerify, int Ocsp, int Crl)
{
	return test_Basic("other", &PrintTestOther, NULL, folder, reader, bVerify, Ocsp, Crl);	
}