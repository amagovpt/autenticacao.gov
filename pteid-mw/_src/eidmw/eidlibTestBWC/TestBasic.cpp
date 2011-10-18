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
#include <time.h>

#include "eidlib.h"

#include "FileUtil.h"
#include "PrintBasic.h"
#include "PrintStruct.h"

#include "TestBasic.h"

int test_Basic(const char *name, void (*test_fct)(FILE *f, int Ocsp, int Crl), void (*test_virtual)(FILE *f, int Ocsp, int Crl), const char *folder, const char *reader, int bVerify, int Ocsp, int Crl)
{

	BEID_Status tStatus = {0};
	long lHandle = 0;
	long lRet = 0;
	FILE *f=NULL;
	
	int i = 0;
	int j = 0;

	int iOcsp = 0;
	int iCrl = 0;
	char buffer[50];

	time_t timeStart;
	time_t timeStop;

	time(&timeStart);


	for(i=0;i<3;i++)
	{
		if(Ocsp>=0 && Ocsp<=2)
			i=Ocsp;

		switch(i)
		{
		case 0: iOcsp=BEID_OCSP_CRL_NOT_USED;	break;
		case 1: iOcsp=BEID_OCSP_CRL_OPTIONAL;	break;
		case 2: iOcsp=BEID_OCSP_CRL_MANDATORY;	break;
		}

		for(j=0;j<3;j++)
		{
			if(Crl>=0 && Crl<=2)
				j=Crl;

			switch(j)
			{
			case 0: iCrl=BEID_OCSP_CRL_NOT_USED;	break;
			case 1: iCrl=BEID_OCSP_CRL_OPTIONAL;	break;
			case 2: iCrl=BEID_OCSP_CRL_MANDATORY;	break;
			}

			if(NULL == (f=FileOpen(name, folder, bVerify, i, j)))
				return -1;

			PrintTestHeader(f,name);
			sprintf_s(buffer, sizeof(buffer), "%s (OCSP=%d - CRL=%d)",name,i,j);
			PrintHeader(f,buffer);

			PrintTestFunction(f,"BEID_Init");
			tStatus = BEID_Init((char*)reader, iOcsp, iCrl, &lHandle);
			PrintStatus(f,"BEID_Init",&tStatus);
			if(BEID_OK != tStatus.general)
			{
				PrintTestFunction(f,"BEID_Exit");
				tStatus = BEID_Exit();
				PrintStatus(f,"BEID_Exit",&tStatus);
				//Don't warn if both parameter are set to mandatory, that's not allow
				if(iOcsp!=BEID_OCSP_CRL_MANDATORY || iCrl!=BEID_OCSP_CRL_MANDATORY)
					PrintWARNING(f,"Initialisation failed");
				
				FileClose(f);
				lRet = FileVerify(name, folder, bVerify, i, j);

				if(Crl>=0 && Crl<=2)
					break;
				else
					continue;
			}

			test_fct(f,Ocsp,Crl);

			PrintTestFunction(f,"BEID_Exit");
			tStatus = BEID_Exit();
			PrintStatus(f,"BEID_Exit",&tStatus);

			//Virtual Reader
			if(test_virtual)
			{
 				PrintHeader(f,"INIT VIRTUAL READER");

				PrintTestFunction(f,"BEID_Init(VIRTUAL)");
				tStatus = BEID_Init("VIRTUAL", iOcsp, iCrl, &lHandle);
				PrintStatus(f,"BEID_Init",&tStatus);
				if(BEID_OK != tStatus.general)
				{
					PrintTestFunction(f,"BEID_Exit");
					tStatus = BEID_Exit();
					PrintWARNING(f,"Initialisation failed");
					PrintStatus(f,"BEID_Exit",&tStatus);

					FileClose(f);
					lRet = FileVerify(name, folder, bVerify, i, j);

					if(Crl>=0 && Crl<=2)
						break;
					else
						continue;
				}

				test_virtual(f,Ocsp,Crl);

				PrintTestFunction(f,"BEID_Exit");
				tStatus = BEID_Exit();
				PrintStatus(f,"BEID_Exit",&tStatus);
			}

			FileClose(f);
			lRet = FileVerify(name, folder, bVerify, i, j);

			if(Crl>=0 && Crl<=2)
				break;
		}

		if(Ocsp>=0 && Ocsp<=2)
			break;
	}

	time(&timeStop);

	printf("TOTAL TEST TIME = %.0lf secondes\n\n",difftime(timeStop,timeStart));

	return lRet;
}