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
#include "TestRead.h"

void PrintTestRead(FILE *f, int Ocsp, int Crl)
{
    BEID_Status tStatus = {0};
    BEID_ID_Data idData = {0};
    BEID_Address adData = {0};
    BEID_VersionInfo vdData = {0};
    BEID_Certif_Check tCheck = {0};
	BEID_Bytes Picture = {0};
	BEID_Bytes Signature = {0};

    // Read ID Data
	PrintTestFunction(f,"BEID_GetID");
    tStatus = BEID_GetID(&idData, &tCheck);
	PrintStatus(f,"BEID_GetID",&tStatus);
	if(BEID_OK != tStatus.general)
	{
		PrintWARNING(f,"Retrieveing id data failed");
	}

	PrintId(f,&idData);
	PrintCertificates(f,&tCheck);

    memset(&tCheck, 0, sizeof(BEID_Certif_Check));

    // Read Address Data
	PrintTestFunction(f,"BEID_GetAddress");
    tStatus = BEID_GetAddress(&adData, &tCheck);
	PrintStatus(f,"BEID_GetAddress",&tStatus);
	if(BEID_OK != tStatus.general)
	{
		PrintWARNING(f,"Retrieveing address data failed");
	}

	PrintAddress(f,&adData);
	PrintCertificates(f,&tCheck);

    memset(&tCheck, 0, sizeof(BEID_Certif_Check));

	// Read Picture
	PrintTestFunction(f,"BEID_GetPicture");
	if(Ocsp==0 && Crl==0) 
	{
		//We pass other crl/ocsp case becaus MW 2.6 return insufficent buffer 
		//before saying the the operation is not allowed
		//But with MW 3.0 it is not possible to know if the buffer is too short if you'r not allowed to get the data
		Picture.length = 10;
		Picture.data = (BYTE*)malloc(Picture.length*sizeof(BYTE));
		memset(Picture.data, 0, Picture.length*sizeof(BYTE));
		tStatus = BEID_GetPicture(&Picture, &tCheck);
		PrintStatus(f,"BEID_GetPicture",&tStatus);
		PrintBytes(f,"PICTURE",&Picture);
		PrintCertificates(f,&tCheck);

		if (BEID_E_INSUFFICIENT_BUFFER == tStatus.general)
		{
			free(Picture.data);
			Picture.length = 5000;
			Picture.data = (BYTE*)malloc(Picture.length*sizeof(BYTE));
			memset(Picture.data, 0, Picture.length*sizeof(BYTE));
		}
	}
	else
	{
		Picture.length = 5000;
		Picture.data = (BYTE*)malloc(Picture.length*sizeof(BYTE));
		memset(Picture.data, 0, Picture.length*sizeof(BYTE));
	}

	tStatus = BEID_GetPicture(&Picture, &tCheck);
	PrintStatus(f,"BEID_GetPicture",&tStatus);

	if(BEID_OK != tStatus.general)
	{
		PrintWARNING(f,"Retrieveing picture failed");
	}

	PrintBytes(f,"PICTURE",&Picture);
	PrintCertificates(f,&tCheck);

	free(Picture.data);

    // Read Version Data
	PrintTestFunction(f,"BEID_GetVersionInfo");
	Signature.length = 0;
	Signature.data = NULL;

    tStatus = BEID_GetVersionInfo(&vdData, FALSE, NULL);
	PrintStatus(f,"BEID_GetVersionInfo",&tStatus);
	if(BEID_OK != tStatus.general)
 	{
		PrintWARNING(f,"Retrieveing version info failed");
	}
	
	PrintVersionInfo(f,&vdData);

	PrintTestFunction(f,"BEID_GetVersionInfo(Signature=TRUE)");
	Signature.length	= 10;
	Signature.data		= (BYTE *)malloc(sizeof(BYTE)*Signature.length);
    memset(Signature.data, 0, Signature.length*sizeof(BYTE));

    tStatus = BEID_GetVersionInfo(&vdData, TRUE, &Signature);
	PrintStatus(f,"BEID_GetVersionInfo",&tStatus);
	if (BEID_E_INSUFFICIENT_BUFFER == tStatus.general)
	{
		PrintVersionInfo(f,&vdData);
        PrintBytes(f,"SIGNATURE", &Signature);

		free(Signature.data);
		Signature.length	= 1000;
		Signature.data		= (BYTE *)malloc(sizeof(BYTE)*Signature.length);
		memset(Signature.data, 0, Signature.length*sizeof(BYTE));
	}

    tStatus = BEID_GetVersionInfo(&vdData, TRUE, &Signature);
	PrintStatus(f,"BEID_GetVersionInfo",&tStatus);
	//if(BEID_OK != tStatus.general)
	//{
	//	PrintWARNING(f,"Retrieveing version info failed");
	//}

	PrintVersionInfo(f,&vdData);
    PrintBytes(f,"SIGNATURE", &Signature);

	memset(&tCheck, 0, sizeof(BEID_Certif_Check));

	PrintTestFunction(f,"BEID_GetCertificates");
	tStatus = BEID_GetCertificates(&tCheck);
	PrintStatus(f,"BEID_GetCertificates",&tStatus);
	if(BEID_OK != tStatus.general)
 	{
		PrintWARNING(f,"Retrieveing certificate failed");
	}

	PrintCertificates(f,&tCheck);

    memset(&tCheck, 0, sizeof(BEID_Certif_Check));

	PrintTestFunction(f,"BEID_GetID");
    tStatus = BEID_GetID(&idData, &tCheck);
	PrintStatus(f,"BEID_GetID",&tStatus);
	if(BEID_OK != tStatus.general)
	{
		PrintWARNING(f,"Retrieveing id data failed");
	}

	PrintId(f,&idData);
	PrintCertificates(f,&tCheck);

	free(Signature.data);
}

int test_Read(const char *folder, const char *reader, int bVerify, int Ocsp, int Crl)
{
	return test_Basic("read", &PrintTestRead, NULL, folder, reader, bVerify, Ocsp, Crl);	
}