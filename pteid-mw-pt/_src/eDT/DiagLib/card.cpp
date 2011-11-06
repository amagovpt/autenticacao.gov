/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#ifdef WIN32
#include <windows.h>
#elif __APPLE__
#include "Mac/mac_helper.h"
#endif

#include "diaglib.h"

#include "card.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#include "reader.h"
#include "pcsc.h"
#include "middleware.h"

#include "Repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t *getSourceName(Reader_SOURCE source);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int cardGetListFromPcsc(Card_LIST *cardList)
{
	return pcscGetCardList(cardList);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int cardGetListFromMW(Card_LIST *cardList)
{
	return mwGetCardList(cardList);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int cardGetInfo(Card_ID id, Card_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	switch(id.Reader.Source)
	{
	case PCSC_READER_SOURCE:
		iReturnCode = pcscGetCardInfo(id,info);
		break;
	case MW_READER_SOURCE:
		iReturnCode = mwGetCardInfo(id,info);
		break;
	default:
		iReturnCode = DIAGLIB_ERR_NOT_AVAILABLE;
		break;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int cardReportInfo(Report_TYPE type, const Card_INFO &info)
{
	int iReturnCode = DIAGLIB_OK;
	reportPrint(type,L"          Serial = %ls\n",info.id.Serial.c_str());
	reportPrint(type,L"          Reader = %ls (%ls view)\n",info.id.Reader.Name.c_str(),getSourceName(info.id.Reader.Source));
	reportPrint(type,L"       FirstName = %ls\n",info.FirstName.c_str());
	reportPrint(type,L"        LastName = %ls\n",info.LastName.c_str());
	reportPrint(type,L"          Street = %ls\n",info.Street.c_str());
	reportPrint(type,L"        Read Id is %ls\n",info.ReadIdOk?L"OK":L"NOT OK");
	reportPrint(type,L"   Read Address is %ls\n",info.ReadAddressOk?L"OK":L"NOT OK");
	reportPrint(type,L"   Read Picture is %ls\n",info.ReadPictureOk?L"OK":L"NOT OK");
	reportPrint(type,L"   Read Version is %ls\n",info.ReadTokenInfoOk?L"OK":L"NOT OK");
	reportPrint(type,L"  Read Cert Rrn is %ls\n",info.ReadCertRrnOk?L"OK":L"NOT OK");
	reportPrint(type,L" Read Cert Root is %ls\n",info.ReadCertRootOk?L"OK":L"NOT OK");
	reportPrint(type,L"   Read Cert CA is %ls\n",info.ReadCertCaOk?L"OK":L"NOT OK");
	reportPrint(type,L" Read Cert Sign is %ls\n",info.ReadCertSignOk?L"OK":L"NOT OK");
	reportPrint(type,L" Read Cert Auth is %ls\n",info.ReadCertAuthOk?L"OK":L"NOT OK");
	reportPrintSeparator(type, REPORT_CARD_SEPARATOR);
	return iReturnCode;
}

void cardContributeInfo(const Card_INFO &info)
{
	REP_PREFIX(						 info.id.Serial.		c_str());
	REP_CONTRIBUTE(L"Serial"		,info.id.Serial.		c_str());
	REP_CONTRIBUTE(L"Reader"		,info.id.Reader.Name.	c_str());
	REP_CONTRIBUTE(L"FirstName"		,info.FirstName.		c_str());
	REP_CONTRIBUTE(L"LastName"		,info.LastName.			c_str());
	REP_CONTRIBUTE(L"Street"		,info.Street.			c_str());
	REP_CONTRIBUTE(L"ReadIdOK"		,info.ReadIdOk?			L"true":L"false");
	REP_CONTRIBUTE(L"ReadAddressOK"	,info.ReadAddressOk?	L"true":L"false");
	REP_CONTRIBUTE(L"ReadPictureOK"	,info.ReadPictureOk?	L"true":L"false");
	REP_CONTRIBUTE(L"ReadVersionOK"	,info.ReadTokenInfoOk?	L"true":L"false");
	REP_CONTRIBUTE(L"ReadCertRrnOK"	,info.ReadCertRrnOk?	L"true":L"false");
	REP_CONTRIBUTE(L"ReadCertRootOK",info.ReadCertRootOk?	L"true":L"false");
	REP_CONTRIBUTE(L"ReadCertCAOK"	,info.ReadCertCaOk?		L"true":L"false");
	REP_CONTRIBUTE(L"ReadCertSignOK",info.ReadCertSignOk?	L"true":L"false");
	REP_CONTRIBUTE(L"ReadCertAuthOK",info.ReadCertAuthOk?	L"true":L"false");
	REP_UNPREFIX();
}

////////////////////////////////////////////////////////////////////////////////////////////////
int cardReportList(Report_TYPE type, const Card_LIST &cardList, const wchar_t *TitleIn)
{
	int iReturnCode = DIAGLIB_OK;

	std::wstring Title;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"Card list";
	
	Title.append(L" (#");
	wchar_t buf[10];
	if(-1==swprintf_s(buf,10,L"%ld",cardList.size()))
	{
		Title.append(L"???");
		LOG_ERROR(L"swprintf_s failed");
	}
	else
	{
		Title.append(buf);
	}
	Title.append(L")");

	reportPrintHeader2(type, Title.c_str(), REPORT_CARD_SEPARATOR);

	Card_INFO info;

	progressInit(cardList.size());

	Card_LIST::const_iterator itr;
	for(itr=cardList.begin();itr!=cardList.end();itr++)
	{
		if(DIAGLIB_OK == cardGetInfo(*itr,&info))
		{
			cardReportInfo(type,info);
			cardContributeInfo(info);
		}
		progressIncrement();
	}

	progressRelease();

	return iReturnCode;
}
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
