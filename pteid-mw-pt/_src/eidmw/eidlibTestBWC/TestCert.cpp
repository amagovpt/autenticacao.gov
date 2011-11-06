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
#include "TestCert.h"

void PrintTestCert(FILE *f, int Ocsp, int Crl)
{
    PTEID_Status tStatus = {0};
    PTEID_Certif_Check tCheck = {0};
	char buffer[50];

    memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

	PrintTestFunction(f,"PTEID_VerifyOCSP");
	tStatus = PTEID_VerifyOCSP(&tCheck);
	PrintStatus(f,"PTEID_VerifyOCSP",&tStatus);

	PrintCertificates(f,&tCheck);

    memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

	PrintTestFunction(f,"PTEID_VerifyCRL");
	tStatus = PTEID_VerifyCRL(&tCheck,TRUE);
	PrintStatus(f,"PTEID_VerifyCRL",&tStatus);

	PrintCertificates(f,&tCheck);

	memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

	PrintTestFunction(f,"PTEID_GetCertificates");
	tStatus = PTEID_GetCertificates(&tCheck);
	PrintStatus(f,"PTEID_GetCertificates",&tStatus);
	if(tStatus.general==PTEID_E_PCSC)
	{
		PrintWARNING(f,"No card present");
		return;
	}
	if(tStatus.general==PTEID_E_UNKNOWN_CARD)
	{
		PrintWARNING(f,"This is not an eid card");
		return;
	}
	if(tStatus.general!=PTEID_OK)
	{
		sprintf_s(buffer,sizeof(buffer),"get certificates failed (Error code = %ld)",tStatus.general);
		PrintWARNING(f,buffer);
		return;
	}

	PrintCertificates(f,&tCheck);

	PrintTestFunction(f,"PTEID_VerifyOCSP");
	tStatus = PTEID_VerifyOCSP(&tCheck);
	PrintStatus(f,"PTEID_VerifyOCSP",&tStatus);

	PrintCertificates(f,&tCheck);

	PrintTestFunction(f,"PTEID_VerifyCRL");
	tStatus = PTEID_VerifyCRL(&tCheck,FALSE);
	PrintStatus(f,"PTEID_VerifyCRL",&tStatus);

	PrintCertificates(f,&tCheck);

	PrintTestFunction(f,"PTEID_VerifyCRL");
	tStatus = PTEID_VerifyCRL(&tCheck,TRUE);
	PrintStatus(f,"PTEID_VerifyCRL",&tStatus);

	PrintCertificates(f,&tCheck);

	memset(&tCheck, 0, sizeof(PTEID_Certif_Check));

	PrintTestFunction(f,"PTEID_GetCertificates");
	tStatus = PTEID_GetCertificates(&tCheck);
	PrintStatus(f,"PTEID_GetCertificates",&tStatus);
	if(tStatus.general==PTEID_E_PCSC)
	{
		PrintWARNING(f,"No card present");
		return;
	}
	if(tStatus.general==PTEID_E_UNKNOWN_CARD)
	{
		PrintWARNING(f,"This is not an eid card");
		return;
	}
	if(tStatus.general!=PTEID_OK)
	{
		sprintf_s(buffer,sizeof(buffer),"get certificates failed (Error code = %ld)",tStatus.general);
		PrintWARNING(f,buffer);
		return;
	}

	PrintCertificates(f,&tCheck);
}

int test_Cert(const char *folder, const char *reader, int bVerify, int Ocsp, int Crl)
{
	return test_Basic("cert", &PrintTestCert, NULL, folder, reader, bVerify, Ocsp, Crl);	
}
