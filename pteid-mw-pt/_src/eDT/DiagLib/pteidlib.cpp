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

#include "pteidlib.h"

#include "error.h"
#include "log.h"
#include "util.h"
#include "folder.h"
#include "progress.h"


static PteidlibWrapper g_pteidlibWrapper = {NULL, NULL, NULL, NULL, NULL, NULL};

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
HMODULE loadPteidlib();
int pteidlibIsAvailable(bool *available);
int pteidlibGetReaderList(Reader_LIST *readerList);
int pteidlibGetCardList(Card_LIST *cardList);
int pteidlibGetCardInfo (Card_ID id, Card_INFO *info);
int pteidlibIsOcspAvailable(Card_ID id, bool *available);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
PteidlibWrapper *loadPteidLibWrapper()
{
	if(g_pteidlibWrapper.hPteidLib)
		return &g_pteidlibWrapper;

	g_pteidlibWrapper.hPteidLib = loadPteidlib();

	g_pteidlibWrapper.pteidlibIsAvailable = pteidlibIsAvailable;
	g_pteidlibWrapper.pteidlibGetReaderList = pteidlibGetReaderList;
	g_pteidlibWrapper.pteidlibGetCardList = pteidlibGetCardList;
	g_pteidlibWrapper.pteidlibGetCardInfo = pteidlibGetCardInfo;
	g_pteidlibWrapper.pteidlibIsOcspAvailable = pteidlibIsOcspAvailable;

	return &g_pteidlibWrapper;
/*
	const wchar_t *pteidlibName = L"libDiagLib_pteidlib.dylib";
	
	Folder_ID path;
	
	if(g_pteidlibWrapper.hPteidLib == NULL)
	{
		path=L"";
		path.append(pteidlibName);
		g_pteidlibWrapper.hPteidLib = LoadLibrary(path.c_str());
	} 
	
	if(g_pteidlibWrapper.hPteidLib == NULL)
	{
		path.assign(wstring_From_string(GetBundlePath()));
		path.append(L"Contents/Resources/");
		path.append(pteidlibName);
		g_pteidlibWrapper.hPteidLib = LoadLibrary(path.c_str());
	}
	
	if(g_pteidlibWrapper.hPteidLib == NULL)
	{
		LOG(L"Could not find '%ls'",pteidlibName);
		return NULL;
	}
	else
	{
		LOG(L"LOAD '%ls'\n",path.c_str());
	}
	
	if(NULL == (g_pteidlibWrapper.pteidlibIsAvailable = reinterpret_cast<fct_pteidlibIsAvailable>(GetProcAddress(g_pteidlibWrapper.hPteidLib, "pteidlibIsAvailable"))))
	{
		LOG_LASTERROR(L"GetProcAddress on pteidlibIsAvailable failed");
		unloadPteidLibWrapper();		
		return NULL;
	}

    if(NULL == (g_pteidlibWrapper.pteidlibGetReaderList = reinterpret_cast<fct_pteidlibGetReaderList>(GetProcAddress(g_pteidlibWrapper.hPteidLib, "pteidlibGetReaderList"))))
 	{
		LOG_LASTERROR(L"GetProcAddress on pteidlibGetReaderList failed");
		unloadPteidLibWrapper();
		return NULL;
	}

    if(NULL == (g_pteidlibWrapper.pteidlibGetCardList = reinterpret_cast<fct_pteidlibGetCardList>(GetProcAddress(g_pteidlibWrapper.hPteidLib, "pteidlibGetCardList"))))
	{
		LOG_LASTERROR(L"GetProcAddress on pteidlibGetCardList failed");
		unloadPteidLibWrapper();
		return NULL;
	}

    if(NULL == (g_pteidlibWrapper.pteidlibGetCardInfo = reinterpret_cast<fct_pteidlibGetCardInfo>(GetProcAddress(g_pteidlibWrapper.hPteidLib, "pteidlibGetCardInfo"))))
	{
		LOG_LASTERROR(L"GetProcAddress on pteidlibGetCardInfo failed");
		unloadPteidLibWrapper();
		return NULL;
	}

    if(NULL == (g_pteidlibWrapper.pteidlibIsOcspAvailable = reinterpret_cast<fct_pteidlibIsOcspAvailable>(GetProcAddress(g_pteidlibWrapper.hPteidLib, "pteidlibIsOcspAvailable"))))
	{
		LOG_LASTERROR(L"GetProcAddress on pteidlibOcspIsAvailable failed");
		unloadPteidLibWrapper();
		return NULL;
	}
	
	LOG(L"LOAD '%ls'\n",pteidlibName);

	return &g_pteidlibWrapper;
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////
void unloadPteidLibWrapper()
{
	if(g_pteidlibWrapper.hPteidLib) 
	{
		FreeLibrary(g_pteidlibWrapper.hPteidLib);
	}
	
	g_pteidlibWrapper.hPteidLib = NULL;
	g_pteidlibWrapper.pteidlibIsAvailable = NULL;
	g_pteidlibWrapper.pteidlibGetReaderList = NULL;
	g_pteidlibWrapper.pteidlibGetCardList = NULL;
	g_pteidlibWrapper.pteidlibGetCardInfo = NULL;
	g_pteidlibWrapper.pteidlibIsOcspAvailable = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PTEIDLIB FUNCTION /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "eidlib.h"
#include "eidlibException.h"

using namespace eIDMW;

HMODULE loadPteidlib()
{
	HMODULE hPteidlib = NULL;

#ifdef WIN32
	const wchar_t *libName = L"pteid35libCpp.dll";
#elif __APPLE__
	const wchar_t *libName = L"libpteidlib.dylib";
#endif
	Folder_ID path;
	
	//First try without path
	if(hPteidlib == NULL)
	{
		path=L"";
		path.append(libName);
		hPteidlib = LoadLibrary(path.c_str());
	} 

	//Then in system folder (Normal place for Mac)
	if(hPteidlib == NULL)
	{
		if(DIAGLIB_OK == folderGetPath(FOLDER_SYSTEM,&path))
		{
			path.append(libName);
			hPteidlib = LoadLibrary(path.c_str());
		}
	}

	//Then in application folder (Normal place for Win)
	if(hPteidlib == NULL)
	{
		if(DIAGLIB_OK == folderGetPath(FOLDER_APP,&path))
		{
			path.append(libName);
			hPteidlib = LoadLibrary(path.c_str());
		}
	}

	if(hPteidlib == NULL)
	{
		LOG(L"Could not find '%ls'",libName);
	}
	else
	{
		LOG(L"LOAD '%ls'\n",path.c_str());
	}

	return hPteidlib;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pteidlibFillReaderList(Reader_LIST *readerList,Card_LIST *cardList)
{
	int iReturnCode = DIAGLIB_OK;

	if(readerList == NULL && cardList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(readerList) readerList->clear();
	if(cardList) cardList->clear();

	if(NULL == loadPteidlib())
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	Reader_ID readerId;
	Card_ID cardId;
	readerId.Source=MW_READER_SOURCE;

	try
	{
		PTEID_InitSDK();

		std::string szReader;

		unsigned long nbrReader=ReaderSet.readerCount();

		progressInit(nbrReader);

		for(unsigned long i=0;i<nbrReader;i++)
		{
			progressIncrement();

			readerId.Name=wstring_From_string(ReaderSet.getReaderName(i));
			if(cardList)
			{
				PTEID_ReaderContext &reader=ReaderSet.getReaderByNum(i);
				if(reader.isCardPresent())
				{
					PTEID_CardType type = reader.getCardType();
					if(type == PTEID_CARDTYPE_EID || type == PTEID_CARDTYPE_KIDS || type == PTEID_CARDTYPE_FOREIGNER)
					{
						PTEID_EIDCard &card=reader.getEIDCard();
						cardId.Reader=readerId;
						cardId.Serial=wstring_From_string(card.getVersionInfo().getSerialNumber());
						cardList->push_back(cardId);
					}
				}
			}
			if(readerList) readerList->push_back(readerId);
		}
		PTEID_ReleaseSDK();
	}
	catch(...)
	{
		PTEID_ReleaseSDK();
		progressRelease();
		return RETURN_LOG_INTERNAL_ERROR;
	}

	progressRelease();
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pteidlibIsAvailable(bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available = false;
	
	if(NULL == loadPteidlib())
	{
		return DIAGLIB_OK;
	}
	
	*available = true;
	
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pteidlibGetReaderList(Reader_LIST *readerList)
{
	return pteidlibFillReaderList(readerList, NULL);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int pteidlibGetCardList(Card_LIST *cardList)
{
	return pteidlibFillReaderList(NULL, cardList);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int pteidlibGetCardInfo (Card_ID id, Card_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(id.Reader.Source != MW_READER_SOURCE)
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);
	}

	info->id=id;
	info->FirstName.clear();
    info->LastName.clear();
    info->Street.clear();
	info->FileId.clear();
	info->FileAddress.clear();
	info->FilePicture.clear();
	info->FileTokenInfo.clear();
	info->FileCertRrn.clear();
	info->FileCertRoot.clear();
	info->FileCertCa.clear();
	info->FileCertSign.clear();
	info->FileCertAuth.clear();
	info->ReadIdOk=false;
	info->ReadAddressOk=false;
	info->ReadPictureOk=false;
	info->ReadTokenInfoOk=false;
	info->ReadCertRrnOk=false;
	info->ReadCertRootOk=false;
	info->ReadCertCaOk=false;
	info->ReadCertSignOk=false;
	info->ReadCertAuthOk=false;

	if(NULL == loadPteidlib())
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	try
	{
		PTEID_InitSDK();

		PTEID_ReaderContext &reader = ReaderSet.getReaderByName(string_From_wstring(id.Reader.Name).c_str());
		if(!reader.isCardPresent())
		{
			PTEID_ReleaseSDK();
			return RETURN_LOG_ERROR(DIAGLIB_ERR_CARD_NOT_FOUND);
		}

		PTEID_CardType type = reader.getCardType();
		if(type != PTEID_CARDTYPE_EID && type != PTEID_CARDTYPE_KIDS && type != PTEID_CARDTYPE_FOREIGNER)
		{
			PTEID_ReleaseSDK();
			return RETURN_LOG_ERROR(DIAGLIB_ERR_CARD_BAD_TYPE);
		}
			
		PTEID_EIDCard &card = reader.getEIDCard();

		if(card.isTestCard())
			card.setAllowTestCard(true);

		PTEID_EId &DataId = card.getID();
		
		info->FirstName=wstring_From_string(DataId.getFirstName());
		info->LastName=wstring_From_string(DataId.getSurname());
		vector_From_bytes(&info->FileId,card.getRawData_Id().GetBytes(),card.getRawData_Id().Size());
		if(info->FirstName.length()>0 && info->FileId.size()>0) info->ReadIdOk=true;

		info->Street=wstring_From_string(DataId.getStreet());
		vector_From_bytes(&info->FileAddress,card.getRawData_Addr().GetBytes(),card.getRawData_Addr().Size());
		if(info->Street.length()>0 && info->FileAddress.size()>0) info->ReadAddressOk=true;

		PTEID_Picture &DataPicture = card.getPicture();
		vector_From_bytes(&info->FilePicture,DataPicture.getData().GetBytes(),DataPicture.getData().Size());
		if(DataPicture.getData().Size()>0) info->ReadPictureOk=true;

		PTEID_CardVersionInfo &DataVersion = card.getVersionInfo();
		vector_From_bytes(&info->FileTokenInfo,card.getRawData_TokenInfo().GetBytes(),card.getRawData_TokenInfo().Size());
		if(strlen(DataVersion.getSerialNumber())>0 && info->FileTokenInfo.size()>0) info->ReadTokenInfoOk=true;

		PTEID_Certificate &DataCertRrn = card.getRrn();
		vector_From_bytes(&info->FileCertRrn,DataCertRrn.getCertData().GetBytes(),DataCertRrn.getCertData().Size());
		if(DataCertRrn.getCertData().Size()>0) info->ReadCertRrnOk=true;

		PTEID_Certificate &DataCertRoot = card.getRoot();
		vector_From_bytes(&info->FileCertRoot,DataCertRoot.getCertData().GetBytes(),DataCertRoot.getCertData().Size());
		if(DataCertRoot.getCertData().Size()>0) info->ReadCertRootOk=true;

		PTEID_Certificate &DataCertCA = card.getCA();
		vector_From_bytes(&info->FileCertCa,DataCertCA.getCertData().GetBytes(),DataCertCA.getCertData().Size());
		if(DataCertCA.getCertData().Size()>0) info->ReadCertCaOk=true;

		PTEID_Certificate &DataCertSig = card.getSignature();
		vector_From_bytes(&info->FileCertSign,DataCertSig.getCertData().GetBytes(),DataCertSig.getCertData().Size());
		if(DataCertSig.getCertData().Size()>0) info->ReadCertSignOk=true;

		PTEID_Certificate &DataCertAuth = card.getAuthentication();
		vector_From_bytes(&info->FileCertAuth,DataCertAuth.getCertData().GetBytes(),DataCertAuth.getCertData().Size());
		if(DataCertAuth.getCertData().Size()>00) info->ReadCertAuthOk=true;

		PTEID_ReleaseSDK();
	}
	catch(PTEID_ExNotAllowByUser &ex)
	{
		PTEID_ReleaseSDK();
		return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_ALLOWED_BY_USER);
	}
	catch(...)
	{
		PTEID_ReleaseSDK();
		return RETURN_LOG_INTERNAL_ERROR;
	}
	
	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int pteidlibIsOcspAvailable(Card_ID id, bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(id.Reader.Source != MW_READER_SOURCE)
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);
	}

	*available=false;

	if(NULL == loadPteidlib())
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	try
	{
		PTEID_InitSDK();

		PTEID_ReaderContext &reader = ReaderSet.getReaderByName(string_From_wstring(id.Reader.Name).c_str());
		if(!reader.isCardPresent())
		{
			PTEID_ReleaseSDK();
			return RETURN_LOG_ERROR(DIAGLIB_ERR_CARD_NOT_FOUND);
		}

		PTEID_CardType type = reader.getCardType();
		if(type != PTEID_CARDTYPE_EID && type != PTEID_CARDTYPE_KIDS && type != PTEID_CARDTYPE_FOREIGNER)
		{
			PTEID_ReleaseSDK();
			return RETURN_LOG_ERROR(DIAGLIB_ERR_CARD_BAD_TYPE);
		}
			
		PTEID_EIDCard &card = reader.getEIDCard();

		if(card.isTestCard())
			card.setAllowTestCard(true);

		PTEID_Certificate &certAuth = card.getCert(PTEID_CERTIF_TYPE_AUTHENTICATION);
		PTEID_CertifStatus certStatusAuth = certAuth.verifyOCSP();

		PTEID_Certificate &certSign = card.getCert(PTEID_CERTIF_TYPE_SIGNATURE);
		PTEID_CertifStatus certStatuSign = certSign.verifyOCSP();
		
		if((PTEID_CERTIF_STATUS_VALID_OCSP == certStatusAuth 
			|| PTEID_CERTIF_STATUS_REVOKED ==certStatusAuth
			|| PTEID_CERTIF_STATUS_UNKNOWN ==certStatusAuth)
			&&
			(PTEID_CERTIF_STATUS_VALID_OCSP == certStatuSign 
			|| PTEID_CERTIF_STATUS_REVOKED ==certStatuSign
			|| PTEID_CERTIF_STATUS_UNKNOWN ==certStatuSign))
		{
			*available=true;
		}

		PTEID_ReleaseSDK();
	}
	catch(PTEID_ExNoReader &ex)
	{
		PTEID_ReleaseSDK();
		return RETURN_LOG_ERROR(DIAGLIB_ERR_READER_NOT_FOUND);
	}
	catch(...)
	{
		PTEID_ReleaseSDK();
		return RETURN_LOG_INTERNAL_ERROR;
	}
	
	return iReturnCode;
} 
