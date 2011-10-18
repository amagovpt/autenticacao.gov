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

#include "beidlib.h"

#include "error.h"
#include "log.h"
#include "util.h"
#include "folder.h"
#include "progress.h"


static BeidlibWrapper g_beidlibWrapper = {NULL, NULL, NULL, NULL, NULL, NULL};

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
HMODULE loadBeidlib();
int beidlibIsAvailable(bool *available);
int beidlibGetReaderList(Reader_LIST *readerList);
int beidlibGetCardList(Card_LIST *cardList);
int beidlibGetCardInfo (Card_ID id, Card_INFO *info);
int beidlibIsOcspAvailable(Card_ID id, bool *available);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
BeidlibWrapper *loadBeidLibWrapper()
{
	if(g_beidlibWrapper.hBeidLib)
		return &g_beidlibWrapper;

	g_beidlibWrapper.hBeidLib = loadBeidlib();

	g_beidlibWrapper.beidlibIsAvailable = beidlibIsAvailable;
	g_beidlibWrapper.beidlibGetReaderList = beidlibGetReaderList;
	g_beidlibWrapper.beidlibGetCardList = beidlibGetCardList;
	g_beidlibWrapper.beidlibGetCardInfo = beidlibGetCardInfo;
	g_beidlibWrapper.beidlibIsOcspAvailable = beidlibIsOcspAvailable;

	return &g_beidlibWrapper;
/*
	const wchar_t *beidlibName = L"libDiagLib_beidlib.dylib";
	
	Folder_ID path;
	
	if(g_beidlibWrapper.hBeidLib == NULL)
	{
		path=L"";
		path.append(beidlibName);
		g_beidlibWrapper.hBeidLib = LoadLibrary(path.c_str());
	} 
	
	if(g_beidlibWrapper.hBeidLib == NULL)
	{
		path.assign(wstring_From_string(GetBundlePath()));
		path.append(L"Contents/Resources/");
		path.append(beidlibName);
		g_beidlibWrapper.hBeidLib = LoadLibrary(path.c_str());
	}
	
	if(g_beidlibWrapper.hBeidLib == NULL)
	{
		LOG(L"Could not find '%ls'",beidlibName);
		return NULL;
	}
	else
	{
		LOG(L"LOAD '%ls'\n",path.c_str());
	}
	
	if(NULL == (g_beidlibWrapper.beidlibIsAvailable = reinterpret_cast<fct_beidlibIsAvailable>(GetProcAddress(g_beidlibWrapper.hBeidLib, "beidlibIsAvailable"))))
	{
		LOG_LASTERROR(L"GetProcAddress on beidlibIsAvailable failed");
		unloadBeidLibWrapper();		
		return NULL;
	}

    if(NULL == (g_beidlibWrapper.beidlibGetReaderList = reinterpret_cast<fct_beidlibGetReaderList>(GetProcAddress(g_beidlibWrapper.hBeidLib, "beidlibGetReaderList"))))
 	{
		LOG_LASTERROR(L"GetProcAddress on beidlibGetReaderList failed");
		unloadBeidLibWrapper();
		return NULL;
	}

    if(NULL == (g_beidlibWrapper.beidlibGetCardList = reinterpret_cast<fct_beidlibGetCardList>(GetProcAddress(g_beidlibWrapper.hBeidLib, "beidlibGetCardList"))))
	{
		LOG_LASTERROR(L"GetProcAddress on beidlibGetCardList failed");
		unloadBeidLibWrapper();
		return NULL;
	}

    if(NULL == (g_beidlibWrapper.beidlibGetCardInfo = reinterpret_cast<fct_beidlibGetCardInfo>(GetProcAddress(g_beidlibWrapper.hBeidLib, "beidlibGetCardInfo"))))
	{
		LOG_LASTERROR(L"GetProcAddress on beidlibGetCardInfo failed");
		unloadBeidLibWrapper();
		return NULL;
	}

    if(NULL == (g_beidlibWrapper.beidlibIsOcspAvailable = reinterpret_cast<fct_beidlibIsOcspAvailable>(GetProcAddress(g_beidlibWrapper.hBeidLib, "beidlibIsOcspAvailable"))))
	{
		LOG_LASTERROR(L"GetProcAddress on beidlibOcspIsAvailable failed");
		unloadBeidLibWrapper();
		return NULL;
	}
	
	LOG(L"LOAD '%ls'\n",beidlibName);

	return &g_beidlibWrapper;
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////
void unloadBeidLibWrapper()
{
	if(g_beidlibWrapper.hBeidLib) 
	{
		FreeLibrary(g_beidlibWrapper.hBeidLib);
	}
	
	g_beidlibWrapper.hBeidLib = NULL;
	g_beidlibWrapper.beidlibIsAvailable = NULL;
	g_beidlibWrapper.beidlibGetReaderList = NULL;
	g_beidlibWrapper.beidlibGetCardList = NULL;
	g_beidlibWrapper.beidlibGetCardInfo = NULL;
	g_beidlibWrapper.beidlibIsOcspAvailable = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// BEIDLIB FUNCTION /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "eidlib.h"
#include "eidlibException.h"

using namespace eIDMW;

HMODULE loadBeidlib()
{
	HMODULE hBeidlib = NULL;

#ifdef WIN32
	const wchar_t *libName = L"beid35libCpp.dll";
#elif __APPLE__
	const wchar_t *libName = L"libbeidlib.dylib";
#endif
	Folder_ID path;
	
	//First try without path
	if(hBeidlib == NULL)
	{
		path=L"";
		path.append(libName);
		hBeidlib = LoadLibrary(path.c_str());
	} 

	//Then in system folder (Normal place for Mac)
	if(hBeidlib == NULL)
	{
		if(DIAGLIB_OK == folderGetPath(FOLDER_SYSTEM,&path))
		{
			path.append(libName);
			hBeidlib = LoadLibrary(path.c_str());
		}
	}

	//Then in application folder (Normal place for Win)
	if(hBeidlib == NULL)
	{
		if(DIAGLIB_OK == folderGetPath(FOLDER_APP,&path))
		{
			path.append(libName);
			hBeidlib = LoadLibrary(path.c_str());
		}
	}

	if(hBeidlib == NULL)
	{
		LOG(L"Could not find '%ls'",libName);
	}
	else
	{
		LOG(L"LOAD '%ls'\n",path.c_str());
	}

	return hBeidlib;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int beidlibFillReaderList(Reader_LIST *readerList,Card_LIST *cardList)
{
	int iReturnCode = DIAGLIB_OK;

	if(readerList == NULL && cardList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(readerList) readerList->clear();
	if(cardList) cardList->clear();

	if(NULL == loadBeidlib())
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	Reader_ID readerId;
	Card_ID cardId;
	readerId.Source=MW_READER_SOURCE;

	try
	{
		BEID_InitSDK();

		std::string szReader;

		unsigned long nbrReader=ReaderSet.readerCount();

		progressInit(nbrReader);

		for(unsigned long i=0;i<nbrReader;i++)
		{
			progressIncrement();

			readerId.Name=wstring_From_string(ReaderSet.getReaderName(i));
			if(cardList)
			{
				BEID_ReaderContext &reader=ReaderSet.getReaderByNum(i);
				if(reader.isCardPresent())
				{
					BEID_CardType type = reader.getCardType();
					if(type == BEID_CARDTYPE_EID || type == BEID_CARDTYPE_KIDS || type == BEID_CARDTYPE_FOREIGNER)
					{
						BEID_EIDCard &card=reader.getEIDCard();
						cardId.Reader=readerId;
						cardId.Serial=wstring_From_string(card.getVersionInfo().getSerialNumber());
						cardList->push_back(cardId);
					}
				}
			}
			if(readerList) readerList->push_back(readerId);
		}
		BEID_ReleaseSDK();
	}
	catch(...)
	{
		BEID_ReleaseSDK();
		progressRelease();
		return RETURN_LOG_INTERNAL_ERROR;
	}

	progressRelease();
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int beidlibIsAvailable(bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available = false;
	
	if(NULL == loadBeidlib())
	{
		return DIAGLIB_OK;
	}
	
	*available = true;
	
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int beidlibGetReaderList(Reader_LIST *readerList)
{
	return beidlibFillReaderList(readerList, NULL);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int beidlibGetCardList(Card_LIST *cardList)
{
	return beidlibFillReaderList(NULL, cardList);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int beidlibGetCardInfo (Card_ID id, Card_INFO *info)
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

	if(NULL == loadBeidlib())
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	try
	{
		BEID_InitSDK();

		BEID_ReaderContext &reader = ReaderSet.getReaderByName(string_From_wstring(id.Reader.Name).c_str());
		if(!reader.isCardPresent())
		{
			BEID_ReleaseSDK();
			return RETURN_LOG_ERROR(DIAGLIB_ERR_CARD_NOT_FOUND);
		}

		BEID_CardType type = reader.getCardType();
		if(type != BEID_CARDTYPE_EID && type != BEID_CARDTYPE_KIDS && type != BEID_CARDTYPE_FOREIGNER)
		{
			BEID_ReleaseSDK();
			return RETURN_LOG_ERROR(DIAGLIB_ERR_CARD_BAD_TYPE);
		}
			
		BEID_EIDCard &card = reader.getEIDCard();

		if(card.isTestCard())
			card.setAllowTestCard(true);

		BEID_EId &DataId = card.getID();
		
		info->FirstName=wstring_From_string(DataId.getFirstName());
		info->LastName=wstring_From_string(DataId.getSurname());
		vector_From_bytes(&info->FileId,card.getRawData_Id().GetBytes(),card.getRawData_Id().Size());
		if(info->FirstName.length()>0 && info->FileId.size()>0) info->ReadIdOk=true;

		info->Street=wstring_From_string(DataId.getStreet());
		vector_From_bytes(&info->FileAddress,card.getRawData_Addr().GetBytes(),card.getRawData_Addr().Size());
		if(info->Street.length()>0 && info->FileAddress.size()>0) info->ReadAddressOk=true;

		BEID_Picture &DataPicture = card.getPicture();
		vector_From_bytes(&info->FilePicture,DataPicture.getData().GetBytes(),DataPicture.getData().Size());
		if(DataPicture.getData().Size()>0) info->ReadPictureOk=true;

		BEID_CardVersionInfo &DataVersion = card.getVersionInfo();
		vector_From_bytes(&info->FileTokenInfo,card.getRawData_TokenInfo().GetBytes(),card.getRawData_TokenInfo().Size());
		if(strlen(DataVersion.getSerialNumber())>0 && info->FileTokenInfo.size()>0) info->ReadTokenInfoOk=true;

		BEID_Certificate &DataCertRrn = card.getRrn();
		vector_From_bytes(&info->FileCertRrn,DataCertRrn.getCertData().GetBytes(),DataCertRrn.getCertData().Size());
		if(DataCertRrn.getCertData().Size()>0) info->ReadCertRrnOk=true;

		BEID_Certificate &DataCertRoot = card.getRoot();
		vector_From_bytes(&info->FileCertRoot,DataCertRoot.getCertData().GetBytes(),DataCertRoot.getCertData().Size());
		if(DataCertRoot.getCertData().Size()>0) info->ReadCertRootOk=true;

		BEID_Certificate &DataCertCA = card.getCA();
		vector_From_bytes(&info->FileCertCa,DataCertCA.getCertData().GetBytes(),DataCertCA.getCertData().Size());
		if(DataCertCA.getCertData().Size()>0) info->ReadCertCaOk=true;

		BEID_Certificate &DataCertSig = card.getSignature();
		vector_From_bytes(&info->FileCertSign,DataCertSig.getCertData().GetBytes(),DataCertSig.getCertData().Size());
		if(DataCertSig.getCertData().Size()>0) info->ReadCertSignOk=true;

		BEID_Certificate &DataCertAuth = card.getAuthentication();
		vector_From_bytes(&info->FileCertAuth,DataCertAuth.getCertData().GetBytes(),DataCertAuth.getCertData().Size());
		if(DataCertAuth.getCertData().Size()>00) info->ReadCertAuthOk=true;

		BEID_ReleaseSDK();
	}
	catch(BEID_ExNotAllowByUser &ex)
	{
		BEID_ReleaseSDK();
		return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_ALLOWED_BY_USER);
	}
	catch(...)
	{
		BEID_ReleaseSDK();
		return RETURN_LOG_INTERNAL_ERROR;
	}
	
	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int beidlibIsOcspAvailable(Card_ID id, bool *available)
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

	if(NULL == loadBeidlib())
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	try
	{
		BEID_InitSDK();

		BEID_ReaderContext &reader = ReaderSet.getReaderByName(string_From_wstring(id.Reader.Name).c_str());
		if(!reader.isCardPresent())
		{
			BEID_ReleaseSDK();
			return RETURN_LOG_ERROR(DIAGLIB_ERR_CARD_NOT_FOUND);
		}

		BEID_CardType type = reader.getCardType();
		if(type != BEID_CARDTYPE_EID && type != BEID_CARDTYPE_KIDS && type != BEID_CARDTYPE_FOREIGNER)
		{
			BEID_ReleaseSDK();
			return RETURN_LOG_ERROR(DIAGLIB_ERR_CARD_BAD_TYPE);
		}
			
		BEID_EIDCard &card = reader.getEIDCard();

		if(card.isTestCard())
			card.setAllowTestCard(true);

		BEID_Certificate &certAuth = card.getCert(BEID_CERTIF_TYPE_AUTHENTICATION);
		BEID_CertifStatus certStatusAuth = certAuth.verifyOCSP();

		BEID_Certificate &certSign = card.getCert(BEID_CERTIF_TYPE_SIGNATURE);
		BEID_CertifStatus certStatuSign = certSign.verifyOCSP();
		
		if((BEID_CERTIF_STATUS_VALID_OCSP == certStatusAuth 
			|| BEID_CERTIF_STATUS_REVOKED ==certStatusAuth
			|| BEID_CERTIF_STATUS_UNKNOWN ==certStatusAuth)
			&&
			(BEID_CERTIF_STATUS_VALID_OCSP == certStatuSign 
			|| BEID_CERTIF_STATUS_REVOKED ==certStatuSign
			|| BEID_CERTIF_STATUS_UNKNOWN ==certStatuSign))
		{
			*available=true;
		}

		BEID_ReleaseSDK();
	}
	catch(BEID_ExNoReader &ex)
	{
		BEID_ReleaseSDK();
		return RETURN_LOG_ERROR(DIAGLIB_ERR_READER_NOT_FOUND);
	}
	catch(...)
	{
		BEID_ReleaseSDK();
		return RETURN_LOG_INTERNAL_ERROR;
	}
	
	return iReturnCode;
} 
