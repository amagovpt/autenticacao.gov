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
#include "UnitTest++/src/UnitTest++.h"
#include "ByteArray.h"
#include "Thread.h"
#include "../applayer/CertStatusCache.h"
#include "../applayer/APLCertif.h"
#include "../applayer/APLReader.h"
#include "../applayer/APLCardBeid.h"
#include <iostream>

using namespace eIDMW;

//Test the flags convertion
TEST(CscLineFlags)
{
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<2;j++)
		{
			for(int k=0;k<2;k++)
			{
				APL_CscLine line(1234,(CSC_Validation)i,j!=0,k!=0);
				//std::cout << line.getFlags() << "\n";
				switch(i)
				{
				case 0:
					CHECK_EQUAL(CSC_VALIDATION_NONE,line.getValidationType());
					break;
				case 1:
					CHECK_EQUAL(CSC_VALIDATION_CRL,line.getValidationType());
					break;
				case 2:
					CHECK_EQUAL(CSC_VALIDATION_OCSP,line.getValidationType());
					break;
				}

				if(j)
					CHECK_EQUAL(true,line.allowTestRoot());
				else
					CHECK_EQUAL(false,line.allowTestRoot());

				if(k)
					CHECK_EQUAL(true,line.allowBadDate());
				else
					CHECK_EQUAL(false,line.allowBadDate());

				CHECK_EQUAL(true,line.isEqual(1234,line.getFlags()));
				CHECK_EQUAL(false,line.isEqual(1234,line.getFlags()+1));
				CHECK_EQUAL(false,line.isEqual(1235,line.getFlags()));
			}
		}
	}


}

//Test the line validity
TEST(CscLineValidity)
{
	APL_CscLine line(1234,CSC_VALIDATION_NONE,true,true);
	line.setValidity(1);
	CHECK_EQUAL(true,line.checkValidity());		//Must be valid just after creation

	CThread::SleepMillisecs(1000);
	CHECK_EQUAL(false,line.checkValidity());	//Must be invalid one second later
}


TEST(ValidationNone)
{
	APL_CertStatusCache *CSCache=AppLayer.getCertStatusCache();

	APL_EIDCard *card;
	APL_Certif *cert;
	CSC_Status status;

	const char * const *list=AppLayer.readerList();

	APL_ReaderContext &reader2=AppLayer.getReader(list[2]);	
	card=reader2.getEIDCard();				
	cert=card->getCertificates()->getCertFromCard(0);

	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_NONE,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_VALID_SIGN,status);			//Must be valid
	if (status == CSC_STATUS_CONNECT)
		printf("  (looks like a connect problem -- perhaps we're offline?)\n");

	APL_ReaderContext &reader0=AppLayer.getReader(list[0]);	
	card=reader0.getEIDCard();				
	card->setAllowTestCard(false);
	cert=card->getCertificates()->getCertFromCard(0);

	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_NONE,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_TEST,status);	//Must be Root test
	if (status == CSC_STATUS_CONNECT)
		printf("  (looks like a connect problem -- perhaps we're offline?)\n");

	card->setAllowTestCard(true);
	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_NONE,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_VALID_SIGN,status);		//Must be valid as root test is accepted
	if (status == CSC_STATUS_CONNECT)
		printf("  (looks like a connect problem -- perhaps we're offline?)\n");
}

TEST(ValidationCRL)
{
	APL_CertStatusCache *CSCache=AppLayer.getCertStatusCache();

	APL_EIDCard *card;
	APL_Certif *cert;
	CSC_Status status;

	const char * const *list=AppLayer.readerList();

	APL_ReaderContext &reader2=AppLayer.getReader(list[2]);	
	card=reader2.getEIDCard();				
	cert=card->getCertificates()->getCertFromCard(0);

	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_CRL,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_VALID_FULL,status);			//Must be valid

	APL_ReaderContext &reader0=AppLayer.getReader(list[0]);	
	card=reader0.getEIDCard();				
	card->setAllowTestCard(false);
	cert=card->getCertificates()->getCertFromCard(0);

	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_CRL,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_TEST,status);			//Must be Root test

	card->setAllowTestCard(true);
	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_CRL,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_VALID_SIGN,status);			//Must be valid as root test is accepted (but this is a Zetes card, cdp is not valid)

	APL_ReaderContext &reader5=AppLayer.getReader(list[5]);	
	card=reader5.getEIDCard();				
	cert=card->getCertificates()->getCertFromCard(0);

	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_CRL,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_REVOKED,status);			//Must be not valid
}

TEST(ValidationOCSP)
{
	APL_CertStatusCache *CSCache=AppLayer.getCertStatusCache();

	APL_EIDCard *card;
	APL_Certif *cert;
	CSC_Status status;

	const char * const *list=AppLayer.readerList();

	APL_ReaderContext &reader2=AppLayer.getReader(list[2]);	
	card=reader2.getEIDCard();				
	cert=card->getCertificates()->getCertFromCard(0);

	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_OCSP,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_VALID_FULL,status);	//Must be valid

	APL_ReaderContext &reader0=AppLayer.getReader(list[0]);	
	card=reader0.getEIDCard();				
	card->setAllowTestCard(false);
	cert=card->getCertificates()->getCertFromCard(0);

	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_OCSP,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_TEST,status);	//Must be Root test

	card->setAllowTestCard(true);
	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_OCSP,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_VALID_SIGN,status);	//Must be valid as root test is accepted (but this is a Zetes card, ocsp responder is not valid)

	APL_ReaderContext &reader5=AppLayer.getReader(list[5]);	
	card=reader5.getEIDCard();				
	cert=card->getCertificates()->getCertFromCard(0);

	status=CSCache->getCertStatus(cert->getUniqueId(),CSC_VALIDATION_OCSP,card->getCertificates());
	CHECK_EQUAL(CSC_STATUS_REVOKED,status);			//Must be not valid
}
