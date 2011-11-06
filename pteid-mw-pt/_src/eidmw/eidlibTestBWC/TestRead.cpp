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
    PTEID_Status tStatus = {0};
    PTEID_ID_Data idData = {0};
    PTEID_Address adData = {0};
    PTEID_VersionInfo vdData = {0};
    PTEID_Certif_Check tCheck = {0};
	PTEID_Bytes Picture = {0};
	PTEID_Bytes Signature = {0};

    // Read ID Data
	PrintTestFunction(f,"PTEID_GetID");
    tStatus = PTEID_GetID(&idData, &tCheck);
	PrintStatus(f,"PTEID_GetID",&tStatus);
	if(PTEID_OK != tStatus.general)
	{
		PrintWARNING(f,"Retrieveing id data failed");
	}

	PrintId(f,&idData);
	PrintCertificates(f,&tCheck);

    memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

    // Read Address Data
	PrintTestFunction(f,"PTEID_GetAddress");
    tStatus = PTEID_GetAddress(&adData, &tCheck);
	PrintStatus(f,"PTEID_GetAddress",&tStatus);
	if(PTEID_OK != tStatus.general)
	{
		PrintWARNING(f,"Retrieveing address data failed");
	}

	PrintAddress(f,&adData);
	PrintCertificates(f,&tCheck);

    memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

	// Read Picture
	PrintTestFunction(f,"PTEID_GetPicture");
	if(Ocsp==0 && Crl==0) 
	{
		//We pass other crl/ocsp case becaus MW 2.6 return insufficent buffer 
		//before saying the the operation is not allowed
		//But with MW 3.0 it is not possible to know if the buffer is too short if you'r not allowed to get the data
		Picture.length = 10;
		Picture.data = (BYTE*)malloc(Picture.length*sizeof(BYTE));
		memset(Picture.data, 0, Picture.length*sizeof(BYTE));
		tStatus = PTEID_GetPicture(&Picture, &tCheck);
		PrintStatus(f,"PTEID_GetPicture",&tStatus);
		PrintBytes(f,"PICTURE",&Picture);
		PrintCertificates(f,&tCheck);

		if (PTEID_E_INSUFFICIENT_BUFFER == tStatus.general)
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

	tStatus = PTEID_GetPicture(&Picture, &tCheck);
	PrintStatus(f,"PTEID_GetPicture",&tStatus);

	if(PTEID_OK != tStatus.general)
	{
		PrintWARNING(f,"Retrieveing picture failed");
	}

	PrintBytes(f,"PICTURE",&Picture);
	PrintCertificates(f,&tCheck);

	free(Picture.data);

    // Read Version Data
	PrintTestFunction(f,"PTEID_GetVersionInfo");
	Signature.length = 0;
	Signature.data = NULL;

    tStatus = PTEID_GetVersionInfo(&vdData, FALSE, NULL);
	PrintStatus(f,"PTEID_GetVersionInfo",&tStatus);
	if(PTEID_OK != tStatus.general)
 	{
		PrintWARNING(f,"Retrieveing version info failed");
	}
	
	PrintVersionInfo(f,&vdData);

	PrintTestFunction(f,"PTEID_GetVersionInfo(Signature=TRUE)");
	Signature.length	= 10;
	Signature.data		= (BYTE *)malloc(sizeof(BYTE)*Signature.length);
    memset(Signature.data, 0, Signature.length*sizeof(BYTE));

    tStatus = PTEID_GetVersionInfo(&vdData, TRUE, &Signature);
	PrintStatus(f,"PTEID_GetVersionInfo",&tStatus);
	if (PTEID_E_INSUFFICIENT_BUFFER == tStatus.general)
	{
		PrintVersionInfo(f,&vdData);
        PrintBytes(f,"SIGNATURE", &Signature);

		free(Signature.data);
		Signature.length	= 1000;
		Signature.data		= (BYTE *)malloc(sizeof(BYTE)*Signature.length);
		memset(Signature.data, 0, Signature.length*sizeof(BYTE));
	}

    tStatus = PTEID_GetVersionInfo(&vdData, TRUE, &Signature);
	PrintStatus(f,"PTEID_GetVersionInfo",&tStatus);
	//if(PTEID_OK != tStatus.general)
	//{
	//	PrintWARNING(f,"Retrieveing version info failed");
	//}

	PrintVersionInfo(f,&vdData);
    PrintBytes(f,"SIGNATURE", &Signature);

	memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

	PrintTestFunction(f,"PTEID_GetCertificates");
	tStatus = PTEID_GetCertificates(&tCheck);
	PrintStatus(f,"PTEID_GetCertificates",&tStatus);
	if(PTEID_OK != tStatus.general)
 	{
		PrintWARNING(f,"Retrieveing certificate failed");
	}

	PrintCertificates(f,&tCheck);

    memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

	PrintTestFunction(f,"PTEID_GetID");
    tStatus = PTEID_GetID(&idData, &tCheck);
	PrintStatus(f,"PTEID_GetID",&tStatus);
	if(PTEID_OK != tStatus.general)
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