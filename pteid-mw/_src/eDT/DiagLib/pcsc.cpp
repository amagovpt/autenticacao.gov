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

#include "pcsc.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include "progress.h"

typedef LONG(__stdcall *fSCardEstablishContext)(
        DWORD dwScope,
        LPCVOID pvReserved1,
        LPCVOID pvReserved2,
        LPSCARDCONTEXT phContext
        );
static fSCardEstablishContext g_fSCardEstablishContext=NULL;
#define FCTNAME_SCARD_ESTABLISHCONTEXT "SCardEstablishContext"

typedef LONG(__stdcall *fSCardReleaseContext)(
        SCARDCONTEXT hContext
        );
static fSCardReleaseContext g_fSCardReleaseContext=NULL;
#define FCTNAME_SCARD_RELEASECONTEXT "SCardReleaseContext"

typedef LONG(__stdcall *fSCardConnect)(
        SCARDCONTEXT hContext,
        LPCTSTR szReader,
        DWORD dwShareMode,
        DWORD dwPreferredProtocols,
        LPSCARDHANDLE phCard,
        LPDWORD pdwActiveProtocol
        );
static fSCardConnect g_fSCardConnect=NULL;
#ifdef WIN32
#define FCTNAME_SCARD_CONNECT "SCardConnectW"
#elif __APPLE__
#define FCTNAME_SCARD_CONNECT "SCardConnect"
#endif	

typedef LONG(__stdcall *fSCardDisconnect)(
        SCARDHANDLE hCard,
        DWORD dwDisposition
        );
static fSCardDisconnect g_fSCardDisconnect=NULL;
#define FCTNAME_SCARD_DISCONNECT "SCardDisconnect"

#ifdef WIN32
typedef LONG(__stdcall *fSCardFreeMemory)(
        SCARDCONTEXT hContext,
        LPCVOID pvMem
        );
static fSCardFreeMemory g_fSCardFreeMemory=NULL;
#define FCTNAME_SCARD_FREEMEMORY "SCardFreeMemory"
#endif

typedef LONG(__stdcall *fSCardListReaders)(
        SCARDCONTEXT hContext,
        LPCTSTR mszGroups,
        LPTSTR mszReaders,
        LPDWORD pcchReaders
        );	
static fSCardListReaders g_fSCardListReaders=NULL;
#ifdef WIN32
#define FCTNAME_SCARD_LISTREADERS "SCardListReadersW"
#elif __APPLE__
#define FCTNAME_SCARD_LISTREADERS "SCardListReaders"
#endif	

#ifdef WIN32
typedef LONG(__stdcall *fSCardGetAttrib)(
        SCARDHANDLE hCard,
        DWORD dwAttrId,
        LPBYTE pbAttr,
        LPDWORD pcbAttrLen
        );
static fSCardGetAttrib g_fSCardGetAttrib;
#define FCTNAME_SCARD_GETATTRIB "SCardGetAttrib"
#endif

typedef LONG(__stdcall *fSCardStatus)(
        SCARDHANDLE hCard,
        LPTSTR szReaderName,
        LPDWORD pcchReaderLen,
        LPDWORD pdwState,
        LPDWORD pdwProtocol,
        LPBYTE pbAtr,
        LPDWORD pcbAtrLen
        );
static fSCardStatus g_fSCardStatus=NULL;
#ifdef WIN32
#define FCTNAME_SCARD_STATUS "SCardStatusW"
#elif __APPLE__
#define FCTNAME_SCARD_STATUS "SCardStatus"
#endif	

typedef LONG(__stdcall *fSCardTransmit)(
        SCARDHANDLE hCard,
        LPCSCARD_IO_REQUEST pioSendPci,
        LPCBYTE pbSendBuffer,
        DWORD cbSendLength,
        LPSCARD_IO_REQUEST pioRecvPci,
        LPBYTE pbRecvBuffer,
        LPDWORD pcbRecvLength
        );
static fSCardTransmit g_fSCardTransmit=NULL;
#define FCTNAME_SCARD_TRANSMIT "SCardTransmit"

typedef LONG(__stdcall *fSCardBeginTransaction)(
        SCARDHANDLE hCard
        );
static fSCardBeginTransaction g_fSCardBeginTransaction=NULL;
#define FCTNAME_SCARD_BEGINTRANSACTION "SCardBeginTransaction"

typedef LONG(__stdcall *fSCardEndTransaction)(
        SCARDHANDLE hCard,
        DWORD dwDisposition
        );
static fSCardEndTransaction g_fSCardEndTransaction=NULL;
#define FCTNAME_SCARD_ENDTRANSACTION "SCardEndTransaction"

static HMODULE g_hWinscardLib=NULL;

static SCARD_IO_REQUEST g_ioSendPci;
static SCARD_IO_REQUEST g_ioRecvPci;

#define DEFAULT_CONNECT_DELAY 200
#define DEFAULT_TRANSMIT_DELAY 3

static int g_ConnectDelay = DEFAULT_CONNECT_DELAY;
static int g_TransmitDelay = DEFAULT_TRANSMIT_DELAY;
static bool g_Transaction = true;

const BYTE PATH_ID[]		= {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x31};
const BYTE PATH_ADDRESS[]	= {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x33};
const BYTE PATH_PICTURE[]	= {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x35};
const BYTE PATH_VERSION[]	= {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x32};
const BYTE PATH_CERTRRN[]	= {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x3C};
const BYTE PATH_CERTROOT[]	= {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x3B};
const BYTE PATH_CERTCA[]	= {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x3A};
const BYTE PATH_CERTSIGN[]	= {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x39};
const BYTE PATH_CERTAUTH[]	= {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x38};

#define BEID_FIELD_TAG_ID_Surname				0x07
#define BEID_FIELD_TAG_ID_FirstName_1			0x08
#define BEID_FIELD_TAG_ID_FirstName_2			0x09
#define BEID_FIELD_TAG_ADDR_Street				0x01

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void *loadPCSCLib();
int Connect(const SCARDCONTEXT hSC, const wchar_t *csReader, SCARDHANDLE *phCard,int *pConnectDelay=NULL);
int Disconnect(SCARDHANDLE hCard, bool reset);

int SendApdu(SCARDHANDLE hCard, const std::vector<BYTE>&Command, std::vector<BYTE> *Response, BYTE *SW1, BYTE *SW2,int *pTransmitDelay);
int ReadCardFile(SCARDHANDLE hCard, const BYTE *Path, const int PathLen, std::vector<BYTE> *Content, int *TransmitDelay) ;
int GetTlvValue(const std::vector<BYTE>&FileContent, int Tag, std::wstring *Value);
int GetATR(SCARDHANDLE hCard, std::wstring *atr);
int GetSerial(SCARDHANDLE hCard, Card_SERIAL *serial, int * const TransmitDelay);
int ReadCard(Card_ID id, Card_INFO *info, bool *success, int * const TransmitDelay);
int SelectMF(SCARDHANDLE hCard, int * const TransmitDelay);

int ReadCard(Card_ID id, Card_INFO *info, bool *success, int * const TransmitDelay);
int TestConnect(Card_ID id, bool *success, int ConnectDelay);

int pcscFillReaderList(Reader_LIST *readersList, Card_LIST *cardList);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int pcscIsAvailable(bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available = false;

	HMODULE hWinscardLib = (HMODULE)loadPCSCLib();

	if (hWinscardLib == NULL) 
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_LIBRARY_NOT_FOUND);
	}

	SCARDCONTEXT hSC=NULL;

	int err=SCARD_S_SUCCESS;

    if (SCARD_S_SUCCESS != (err = g_fSCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hSC)))
	{
        LOG_ERRORCODE(L"SCardEstablishContext failed",err);
        return DIAGLIB_OK;
    }
	
	*available=true;

	if (SCARD_S_SUCCESS != (err = g_fSCardReleaseContext(hSC)))
	{
        LOG_ERRORCODE(L"SCardReleaseContext failed",err);
    }

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscGetReaderList(Reader_LIST *readerList)
{
	return pcscFillReaderList(readerList, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscGetCardList(Card_LIST *cardList)
{
	return pcscFillReaderList(NULL, cardList);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscGetCardInfo(Card_ID id, Card_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	iReturnCode = ReadCard(id, info, NULL, NULL);

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscCheckConnect(Card_ID id, int MaxNbrOfSuccess, int Delay, int *countSuccess)
{
	int iReturnCode = DIAGLIB_OK;

	if(countSuccess == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}


	bool success=false;

	progressInit(MaxNbrOfSuccess);

	for(*countSuccess=0;*countSuccess<MaxNbrOfSuccess;(*countSuccess)++)
	{
		success=false;
		if(DIAGLIB_OK != (iReturnCode = pcscTestConnectOnce(id, &success, Delay)))
		{
			break;
		}
		else if(!success)
		{
			iReturnCode = DIAGLIB_ERR_PCSC_CHECK_TIMING_FAILED;
			break;
		}
		progressIncrement();
	}

	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscOptimizeConnect(Card_ID id, int NbrOfSuccess, int DelayMin,	int DelayMax, int DelayStep, int *DelayOptimum)
{
	int iReturnCode = DIAGLIB_OK;

	if(DelayOptimum == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*DelayOptimum=0;

	bool success=false;
	int countSuccess=0;

	progressInit((DelayMax-DelayMin+DelayStep)/DelayStep);
	
	for(int delay=DelayMin;delay<=DelayMax;delay+=DelayStep)
	{
		success=false;
		countSuccess=0;

		switch(pcscCheckConnect(id,NbrOfSuccess, delay, &countSuccess))
		{
		case DIAGLIB_ERR_PCSC_CHECK_TIMING_FAILED:
			break;
		case DIAGLIB_OK:
			success = true;
			break;
		default:
			break;
		}

		if(success)
		{
			*DelayOptimum=delay;
			break;
		}

		progressIncrement();
	}

	if(!success)
	{
		iReturnCode=DIAGLIB_ERR_PCSC_CHECK_TIMING_FAILED;
	}

	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscTestConnectOnce(Card_ID id, bool *success, int ConnectDelay)
{
	int iReturnCode = DIAGLIB_OK;

	if(success == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(id.Reader.Source != PCSC_READER_SOURCE)
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);
	}

	*success=false;

	HMODULE hWinscardLib = (HMODULE)loadPCSCLib();

	if (hWinscardLib == NULL) 
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	SCARDCONTEXT hSC=NULL;

	int err=SCARD_S_SUCCESS;

    if (SCARD_S_SUCCESS != (err = g_fSCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hSC)))
	{
        LOG_ERRORCODE(L"SCardEstablishContext failed",err);
        return DIAGLIB_ERR_PCSC_CONTEXT_FAILED;
    }
	
	g_Transaction = false; //The connect problem is hidden when transaction is enabled

	SCARDHANDLE hCard = NULL;
	if(DIAGLIB_OK != (iReturnCode = Connect(hSC, (wchar_t *)id.Reader.Name.c_str(), &hCard, &ConnectDelay)))
	{
		LOG_ERROR(L"Connect failed");
	}

	if(DIAGLIB_OK == iReturnCode)
	{
		if(DIAGLIB_OK != SelectMF(hCard, NULL))
		{
			LOG_ERROR(L"SelectMF failed");
		}
		else
		{
			*success=true;
		}

		if(DIAGLIB_OK != Disconnect(hCard, true))
		{
			LOG_ERROR(L"Disconnect failed");
		}
	}

	g_Transaction = true;

	if (SCARD_S_SUCCESS != (err = g_fSCardReleaseContext(hSC)))
	{
        LOG_ERRORCODE(L"SCardReleaseContext failed",err);
    }

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscSetConnectTiming(int Delay)
{
	int iReturnCode = DIAGLIB_OK;

	g_ConnectDelay = Delay;

	LOG(L"Connect timing set to %ld", g_ConnectDelay);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscCheckTransmit(Card_ID id, int MaxNbrOfSuccess, int Delay, int *countSuccess)
{
	int iReturnCode = DIAGLIB_OK;

	if(countSuccess == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	bool success=false;

	progressInit(MaxNbrOfSuccess);

	for(*countSuccess=0;*countSuccess<MaxNbrOfSuccess;*countSuccess++)
	{
		success=false;
		if(DIAGLIB_OK != (iReturnCode = pcscTestTransmitOnce(id, &success, Delay)))
		{
			break;
		}
		else if(!success)
		{
			iReturnCode = DIAGLIB_ERR_PCSC_CHECK_TIMING_FAILED;
			break;
		}
		progressIncrement();
	}
	
	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscOptimizeTransmit(Card_ID id, int NbrOfSuccess, int DelayMin, int DelayMax, int DelayStep, int *DelayOptimum)
{
	int iReturnCode = DIAGLIB_OK;

	if(DelayOptimum == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*DelayOptimum=0;

	bool success=false;
	int countSuccess=0;

	progressInit((DelayMax-DelayMin+DelayStep)/DelayStep);
	
	for(int delay=DelayMin;delay<=DelayMax;delay+=DelayStep)
	{
		success=false;
		countSuccess=0;

		switch(pcscCheckTransmit(id,NbrOfSuccess, delay, &countSuccess))
		{
		case DIAGLIB_ERR_PCSC_CHECK_TIMING_FAILED:
			break;
		case DIAGLIB_OK:
			success = true;
			break;
		default:
			break;
		}

		if(success)
		{
			*DelayOptimum=delay;
			break;
		}

		progressIncrement();
	}

	if(!success)
	{
		iReturnCode=DIAGLIB_ERR_PCSC_CHECK_TIMING_FAILED;
	}

	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscTestTransmitOnce(Card_ID id, bool *success, int delai)
{
	int iReturnCode = DIAGLIB_OK;

	//g_Transaction = false; //The delay is worst when transaction is enabled

	iReturnCode = ReadCard(id, NULL, success, &delai);

	//g_Transaction = true;

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int pcscSetTransmitTiming(int Delay)
{
	int iReturnCode = DIAGLIB_OK;

	g_TransmitDelay = Delay;

	LOG(L"Transmit timing set to %ld", g_TransmitDelay);

	return iReturnCode;
}


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int BeginTransaction( SCARDHANDLE hCard )
{
    if (g_Transaction)
	{
		int err = SCARD_S_SUCCESS;
		if (SCARD_S_SUCCESS != (err = g_fSCardBeginTransaction(hCard))) 
		{
			LOG_ERRORCODE(L"SCardBeginTransaction failed",err);
			return DIAGLIB_ERR_INTERNAL;
		} 
	}
	return DIAGLIB_OK;
}
int EndTransaction( SCARDHANDLE hCard )
{
    if (g_Transaction)
	{
		int err = SCARD_S_SUCCESS;
		if (SCARD_S_SUCCESS != (err = g_fSCardEndTransaction(hCard, SCARD_LEAVE_CARD))) 
		{
			LOG_ERRORCODE(L"SCardEndTransaction failed",err);
			return DIAGLIB_ERR_INTERNAL;
		}
    }
	return DIAGLIB_OK;
}
int SelectApplet( SCARDHANDLE hCard, int *pTransmitDelay )
{
	const BYTE SELECT_APPLET_APDU[] = {0x00,0xA4,0x04,0x00,0x0F,0xA0,0x00,0x00,0x00,0x30,0x29,0x05,0x70,0x00,0xAD,0x13,0x10,0x01,0x01,0xFF};

	// build select applet apdu
	std::vector<BYTE> Command;
	std::vector<BYTE> Response;
	BYTE SW1=0xFF;
	BYTE SW2=0xFF;

	Command.reserve(sizeof (SELECT_APPLET_APDU));
	for (int i = 0; i < sizeof (SELECT_APPLET_APDU); ++i)
		Command.push_back(SELECT_APPLET_APDU[i]);
	// select applet

	int iReturnCode = DIAGLIB_OK;

	if(DIAGLIB_OK != (iReturnCode = SendApdu(hCard, Command, &Response, &SW1, &SW2, pTransmitDelay)))
	{
		LOG_ERROR(L"SelectApplet failed");
		return iReturnCode;
	}
	if (SW1 != 0x90 || SW2 != 0x00) 
	{
		LOG_ERROR(L"SendApdu 'SelectApplet' failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	return iReturnCode;
}

void *loadPCSCLib()
{
	if(g_hWinscardLib)
		return g_hWinscardLib;

#ifdef WIN32
	const wchar_t *pcscLib = L"winscard.dll";
#elif __APPLE__
	const wchar_t *pcscLib = L"/System/Library/Frameworks/PCSC.framework/PCSC";
#endif

	if(NULL == (g_hWinscardLib = LoadLibrary(pcscLib)))
	{
		LOG_LASTERROR(L"LoadLibrary failed");
		return g_hWinscardLib;
	}

	if(NULL == (g_fSCardEstablishContext = reinterpret_cast<fSCardEstablishContext>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_ESTABLISHCONTEXT))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardEstablishContext failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

    if(NULL == (g_fSCardReleaseContext = reinterpret_cast<fSCardReleaseContext>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_RELEASECONTEXT))))
 	{
		LOG_LASTERROR(L"GetProcAddress on fSCardReleaseContext failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

    if(NULL == (g_fSCardListReaders = reinterpret_cast<fSCardListReaders>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_LISTREADERS))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardListReaders failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

    if(NULL == (g_fSCardConnect = reinterpret_cast<fSCardConnect>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_CONNECT))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardConnect failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

    if(NULL == (g_fSCardDisconnect = reinterpret_cast<fSCardDisconnect>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_DISCONNECT))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardDisconnect failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

#ifdef WIN32
    if(NULL == (g_fSCardFreeMemory = reinterpret_cast<fSCardFreeMemory>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_FREEMEMORY))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardFreeMemory failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

	//Not available on Mac OS Tiger
    if(NULL == (g_fSCardGetAttrib = reinterpret_cast<fSCardGetAttrib>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_GETATTRIB))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardGetAttrib failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}
#endif
	
    if(NULL == (g_fSCardStatus = reinterpret_cast<fSCardStatus>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_STATUS))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardStatus failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

    if(NULL == (g_fSCardTransmit = reinterpret_cast<fSCardTransmit>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_TRANSMIT))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardTransmit failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

	if(NULL == (g_fSCardBeginTransaction = reinterpret_cast<fSCardBeginTransaction>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_BEGINTRANSACTION))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardBeginTransaction failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}

    if(NULL == (g_fSCardEndTransaction = reinterpret_cast<fSCardEndTransaction>(GetProcAddress(g_hWinscardLib, FCTNAME_SCARD_ENDTRANSACTION))))
	{
		LOG_LASTERROR(L"GetProcAddress on fSCardEndTransaction failed");
		FreeLibrary(g_hWinscardLib);
		g_hWinscardLib=NULL;
		return NULL;
	}
	
	LOG(L"LOAD '%ls'\n",pcscLib);

	return g_hWinscardLib;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int Connect(const SCARDCONTEXT hSC, const wchar_t *csReader, SCARDHANDLE *phCard, int *pConnectDelay)
{
	int iReturnCode = DIAGLIB_OK;

	if((void*)hSC == NULL || csReader == NULL || wcslen(csReader) == 0 || phCard == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*phCard = NULL;
    DWORD dwProtocol = 1;
	unsigned int err=SCARD_S_SUCCESS;

#ifdef WIN32
    if (SCARD_S_SUCCESS != (err = g_fSCardConnect(hSC, csReader,							  SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, phCard, &dwProtocol)))
#elif __APPLE__
    if (SCARD_S_SUCCESS != (err = g_fSCardConnect(hSC, string_From_wstring(csReader).c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, phCard, &dwProtocol)))
#endif
	{
		if (SCARD_E_NO_SMARTCARD == err
			|| SCARD_W_REMOVED_CARD == err)
		{
			return DIAGLIB_ERR_CARD_NOT_FOUND;
		}
		else
		{
			LOG_ERRORCODE(L"SCardConnect failed",err);
			return DIAGLIB_ERR_PCSC_CONNECT_FAILED;
		}
	}

	g_ioSendPci.dwProtocol = dwProtocol;
	g_ioSendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);
	g_ioRecvPci.dwProtocol = dwProtocol;
	g_ioRecvPci.cbPciLength = sizeof(SCARD_IO_REQUEST);

	// If you do an SCardTransmit() too fast after an SCardConnect(),
	// some cards/readers will return an error (e.g. 0x801002f)
	Sleep(pConnectDelay==NULL?g_ConnectDelay:*pConnectDelay);

    return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int Disconnect(SCARDHANDLE hCard, bool reset)
{
	int iReturnCode = DIAGLIB_OK;

	if((void*)hCard == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

    DWORD dwDisposition = (reset ? /*SCARD_UNPOWER_CARD*/ SCARD_RESET_CARD : SCARD_LEAVE_CARD);
	int err=SCARD_S_SUCCESS;

    if (SCARD_S_SUCCESS != (err = g_fSCardDisconnect((SCARDHANDLE) hCard, dwDisposition)))
	{
		LOG_ERRORCODE(L"SCardDisconnect failed",err);
		return DIAGLIB_ERR_INTERNAL;
	}

	return iReturnCode;
}

int pcscFillReaderList(Reader_LIST *readerList, Card_LIST *cardList)
{
	int iReturnCode = DIAGLIB_OK;

	if(readerList == NULL && cardList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(readerList) readerList->clear();
	if(cardList) cardList->clear();

	Reader_ID readerId;
	readerId.Source=PCSC_READER_SOURCE;
	Card_ID cardId;

	HMODULE hWinscardLib = (HMODULE)loadPCSCLib();

	if (hWinscardLib == NULL) 
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	progressInit(4);

	SCARDCONTEXT hSC=NULL;

	unsigned int err=SCARD_S_SUCCESS;

    if (SCARD_S_SUCCESS != (err = g_fSCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hSC)))
	{
        LOG_ERRORCODE(L"SCardEstablishContext failed",err);
		progressRelease();
        return DIAGLIB_ERR_PCSC_CONTEXT_FAILED;
    }

	progressIncrement();

	DWORD bcReaders = 1024;
#ifdef WIN32
	wchar_t szReaders[1024];
	wchar_t *pReaderName = NULL;
#elif __APPLE__
	char szReaders[1024];
	char *pReaderName = NULL;
#endif
 
    if (SCARD_S_SUCCESS != (err = g_fSCardListReaders(hSC,NULL,(LPTSTR)&szReaders,&bcReaders))) 
	{
		if(SCARD_E_NO_READERS_AVAILABLE != err)
		{
			LOG_ERRORCODE(L"SCardListReaders failed",err);
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else
		{
			LOG_ERROR(L"SCardListReaders failed");
			iReturnCode = DIAGLIB_ERR_READER_NOT_FOUND;
		}
    }
	else
	{
		progressIncrement();
		if (bcReaders > 0) 
		{
			pReaderName = szReaders;

			while(true) 
			{
				if(*pReaderName == 0)
					break;
#ifdef WIN32
				readerId.Name=pReaderName;
#elif __APPLE__
				readerId.Name=wstring_From_string(pReaderName);
#endif
				if(cardList)
				{
					SCARDHANDLE hCard = NULL;
					if(DIAGLIB_OK == Connect(hSC, readerId.Name.c_str(), &hCard))
					{
						cardId.Reader=readerId;
						if(DIAGLIB_OK == GetSerial(hCard, &cardId.Serial, NULL))
						{
							cardList->push_back(cardId);
						}
						Disconnect(hCard, false);
					}
				}
				if(readerList) readerList->push_back(readerId);

				while(*pReaderName++ != 0) {}
			}
		}
	}
	progressIncrement();

	if (SCARD_S_SUCCESS != (err = g_fSCardReleaseContext(hSC)))
	{
        LOG_ERRORCODE(L"SCardReleaseContext failed",err);
    }

	progressIncrement();

	progressRelease();
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int ReadCard(Card_ID id, Card_INFO *pInfo, bool *success, int * const TransmitDelay)
{
	int iReturnCode = DIAGLIB_OK;

	if(pInfo == NULL && success == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(id.Reader.Source != PCSC_READER_SOURCE)
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);
	}

	Card_INFO info;

	info.id=id;
	info.FirstName.clear();
	info.LastName.clear();
	info.Street.clear();
	info.FileId.clear();
	info.FileAddress.clear();
	info.FilePicture.clear();
	info.FileTokenInfo.clear();
	info.FileCertRrn.clear();
	info.FileCertRoot.clear();
	info.FileCertCa.clear();
	info.FileCertSign.clear();
	info.FileCertAuth.clear();
	info.ReadIdOk=false;
	info.ReadAddressOk=false;
	info.ReadPictureOk=false;
	info.ReadTokenInfoOk=false;
	info.ReadCertRrnOk=false;
	info.ReadCertRootOk=false;
	info.ReadCertCaOk=false;
	info.ReadCertSignOk=false;
	info.ReadCertAuthOk=false;

	if(pInfo) *pInfo = info;

	if(success) *success=false;


	HMODULE hWinscardLib = (HMODULE)loadPCSCLib();

	if (hWinscardLib == NULL) 
	{
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	SCARDCONTEXT hSC=NULL;

	int err=SCARD_S_SUCCESS;

    if (SCARD_S_SUCCESS != (err = g_fSCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hSC)))
	{
        LOG_ERRORCODE(L"SCardEstablishContext failed",err);
        return DIAGLIB_ERR_PCSC_CONTEXT_FAILED;
    }
	
	SCARDHANDLE hCard = NULL;
	if(DIAGLIB_OK != (iReturnCode = Connect(hSC, (wchar_t *)id.Reader.Name.c_str(), &hCard, NULL)))
	{
		LOG_ERROR(L"Connect failed");
	}

	if(DIAGLIB_OK == iReturnCode)
	{
		std::vector<BYTE> Content;
		if(DIAGLIB_OK != GetSerial(hCard, &info.id.Serial, TransmitDelay))
		{
			LOG_ERROR(L"GetSerial failed");
		}

		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_ID, sizeof(PATH_ID), &Content, TransmitDelay))
		{
			GetTlvValue(Content,BEID_FIELD_TAG_ID_Surname,&info.LastName);
			std::wstring FirstName;
			GetTlvValue(Content,BEID_FIELD_TAG_ID_FirstName_1,&FirstName);
			info.FirstName.assign(FirstName);
			GetTlvValue(Content,BEID_FIELD_TAG_ID_FirstName_2,&FirstName);
			if(FirstName.length()>0)
			{
				info.FirstName.append(L" ");
				info.FirstName.append(FirstName);
			}
			info.FileId=Content;
			if(info.FileId.size()>0) info.ReadIdOk=true;
		}
		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_ADDRESS, sizeof(PATH_ADDRESS), &Content, TransmitDelay))
		{
			GetTlvValue(Content,BEID_FIELD_TAG_ADDR_Street,&info.Street);
			info.FileAddress=Content;
			if(info.FileAddress.size()>0) info.ReadAddressOk=true;
		}
		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_PICTURE, sizeof(PATH_PICTURE), &Content, TransmitDelay))
		{
			info.FilePicture=Content;
			if(info.FilePicture.size()>0) info.ReadPictureOk=true;
		}
		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_VERSION, sizeof(PATH_VERSION), &Content, TransmitDelay))
		{
			info.FileTokenInfo=Content;
			if(info.FileTokenInfo.size()>0) info.ReadTokenInfoOk=true;
		}
		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_CERTRRN, sizeof(PATH_CERTRRN), &Content, TransmitDelay))
		{
			info.FileCertRrn=Content;
			if(info.FileCertRrn.size()>0) info.ReadCertRrnOk=true;
		}
		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_CERTROOT, sizeof(PATH_CERTROOT), &Content, TransmitDelay))
		{
			info.FileCertRoot=Content;
			if(info.FileCertRoot.size()>0) info.ReadCertRootOk=true;
		}
		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_CERTCA, sizeof(PATH_CERTCA), &Content, TransmitDelay))
		{
			info.FileCertCa=Content;
			if(info.FileCertCa.size()>0) info.ReadCertCaOk=true;
		}
		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_CERTSIGN, sizeof(PATH_CERTSIGN), &Content, TransmitDelay))
		{
			info.FileCertSign=Content;
			if(info.FileCertSign.size()>0) info.ReadCertSignOk=true;
		}
		if(DIAGLIB_OK == ReadCardFile(hCard, PATH_CERTAUTH, sizeof(PATH_CERTAUTH), &Content, TransmitDelay))
		{
			info.FileCertAuth=Content;
			if(info.FileCertAuth.size()>0) info.ReadCertAuthOk=true;
		}

		if(DIAGLIB_OK != Disconnect(hCard, false))
		{
			LOG_ERROR(L"Disconnect failed");
		}
	}

	if(success)
	{
		*success = info.ReadIdOk 
					&& info.ReadAddressOk 
					&& info.ReadPictureOk 
					&& info.ReadTokenInfoOk 
					&& info.ReadCertRrnOk 
					&& info.ReadCertRootOk 
					&& info.ReadCertCaOk 
					&& info.ReadCertSignOk 
					&& info.ReadCertAuthOk;
	}

	if(pInfo) *pInfo = info;

	if (SCARD_S_SUCCESS != (err = g_fSCardReleaseContext(hSC)))
	{
        LOG_ERRORCODE(L"SCardReleaseContext failed",err);
    }

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SendApdu(SCARDHANDLE hCard, const std::vector<BYTE>&Command, std::vector<BYTE> *Response, BYTE *pSW1, BYTE *pSW2, int *pTransmitDelay) 
{
 	int iReturnCode = DIAGLIB_OK;

	if((void*)hCard == NULL || Response == NULL || pSW1 == NULL || pSW2 == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	Response->clear();

	// send buffer 
    BYTE SendBuffer[280];
    memset(&SendBuffer, 0, sizeof (SendBuffer));
    int apdulen = static_cast<int> (Command.size());
    for (int i = 0; i < apdulen; ++i)
        SendBuffer[i] = Command.at(i);

    // receive buffer 
    BYTE RecvBuffer[258];
    memset(&RecvBuffer, 0, sizeof (RecvBuffer));
    DWORD cbRecvLength = sizeof (RecvBuffer);

	int err=SCARD_S_SUCCESS;
	
	*pSW1 = 0xFF;
	*pSW2 = 0xFF;

    // Start Transaction : Also nessesary for read-only actions.
//     if (g_Transaction)
// 	{
// 		if (SCARD_S_SUCCESS != (err = g_fSCardBeginTransaction(hCard))) 
// 		{
// 			LOG_ERRORCODE(L"SCardBeginTransaction failed",err);
// 			return DIAGLIB_ERR_INTERNAL;
// 		} 
// 	}

    // transmit
	Sleep(pTransmitDelay==NULL?g_TransmitDelay:*pTransmitDelay);

    if (SCARD_S_SUCCESS != (err = g_fSCardTransmit(hCard,&g_ioSendPci,SendBuffer,apdulen,&g_ioRecvPci,RecvBuffer,&cbRecvLength))) 
	{
		if(cbRecvLength<2)
		{
			LOG_ERRORCODE(L"SCardTransmit failed without SW1 - SW2",err);
		}
		else
		{
			*pSW1=RecvBuffer[cbRecvLength-2];
			*pSW2=RecvBuffer[cbRecvLength-1];
			LOG_PCSCERROR(L"SCardTransmit failed",err,*pSW1,*pSW2);
		}
		iReturnCode = DIAGLIB_ERR_PCSC_TRANSMIT_FAILED;
    }
    // End Transaction
//     if (g_Transaction)
// 	{
// 		if (SCARD_S_SUCCESS != (err = g_fSCardEndTransaction(hCard, SCARD_LEAVE_CARD))) 
// 		{
// 			LOG_ERRORCODE(L"SCardEndTransaction failed",err);
// 			return DIAGLIB_ERR_INTERNAL;
// 		}
//     }

	if(DIAGLIB_OK == iReturnCode)
	{
		if (cbRecvLength < 2) 
		{
			LOG_ERROR(L"SCardTransmit failed without SW1 - SW2");
			iReturnCode = DIAGLIB_ERR_PCSC_TRANSMIT_FAILED;
		}
		else
		{
			*pSW1=RecvBuffer[cbRecvLength-2];
			*pSW2=RecvBuffer[cbRecvLength-1];


			// Store the buffer into the response 
			for (DWORD i = 0; i < cbRecvLength-2; ++i)
				Response->push_back(RecvBuffer[i]);

			// Heeft de kaart nog meer data ? 
			if (*pSW1 == 0x61) {
				const BYTE GET_MORE_APDU[] = {0x00, 0xC0, 0x00, 0x00};
				std::vector<BYTE> GetMoreCommand;
				for (unsigned int i = 0; i < sizeof (GET_MORE_APDU); ++i)
					GetMoreCommand.push_back(GET_MORE_APDU[i]);
				GetMoreCommand.push_back(*pSW2);

				std::vector<BYTE> MoreResponse;
				if(DIAGLIB_OK != (iReturnCode = SendApdu(hCard, GetMoreCommand, &MoreResponse,pSW1,pSW2,pTransmitDelay)))
				{
					LOG_ERROR(L"SendApdu failed to get more data");
				}
				else
				{
					// plak de results aaneen
					for (std::vector<BYTE>::const_iterator itr = MoreResponse.begin(); itr != MoreResponse.end(); itr++)
						Response->push_back(*itr);
				}
			}
		}
	}

    return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int ReadCardFile(SCARDHANDLE hCard, const BYTE *Path, const int PathLen, std::vector<BYTE> *Content, int *TransmitDelay) 
{
	int iReturnCode = DIAGLIB_OK;

	if((void*)hCard == NULL || Path == NULL || PathLen <= 0)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	Content->clear();

	if (DIAGLIB_OK != (iReturnCode = BeginTransaction(hCard)))
	{
		return iReturnCode;
	}

	if(DIAGLIB_OK != (iReturnCode = SelectApplet(hCard, TransmitDelay)))
	{
		LOG_ERROR(L"ReadCardFile 'SelectApplet' failed");
		if(DIAGLIB_OK != EndTransaction(hCard))
		{
			LOG_ERROR(L"ReadCardFile 'EndTransaction' failed");
		}
		return iReturnCode;
	}
	
	BYTE SW1=0xFF;
	BYTE SW2=0xFF;

    //Build the 'select file' apdu
    const BYTE SELECT_FILE_APDU[] = {0x00, 0xA4, 0x08, 0x0C};

    std::vector<BYTE> SelectFileCommand;

    for (unsigned int i = 0; i < sizeof (SELECT_FILE_APDU); ++i)
        SelectFileCommand.push_back(SELECT_FILE_APDU[i]);
	SelectFileCommand.push_back(PathLen);
    for (int i = 0; i < PathLen; ++i)
        SelectFileCommand.push_back(Path[i]);

	std::vector<BYTE> tmpResponse;
    //Run the 'select file'
    if(DIAGLIB_OK != (iReturnCode = SendApdu(hCard,SelectFileCommand,&tmpResponse,&SW1,&SW2,TransmitDelay)))
	{
		LOG_ERROR(L"ReadCardFile 'SendApdu' failed");
		if(DIAGLIB_OK != EndTransaction(hCard))
		{
			LOG_ERROR(L"EndTransaction failed");
		}
		return iReturnCode;
	}

    if (SW1 != 0x90 || SW2 != 0x00) 
	{
		LOG_ERROR(L"SendApdu 'Select file' failed");
		if(DIAGLIB_OK != EndTransaction(hCard))
		{
			LOG_ERROR(L"EndTransaction failed");
		}
		return RETURN_LOG_INTERNAL_ERROR;
    }

    //Build the 'read binary' apdu
    const int MAX_READ_SIZE = 0xF8;
    const BYTE READ_BINARY_APDU[] = {0x00, 0xB0, 0x00, 0x00};
    std::vector<BYTE> SelectReadBinary;
    for (unsigned int i = 0; i < sizeof (READ_BINARY_APDU); ++i)
		SelectReadBinary.push_back(READ_BINARY_APDU[i]);
	SelectReadBinary.push_back(MAX_READ_SIZE);

    int offset = 0;

    while(true)
	{
        SelectReadBinary.at(2) = ((offset & 0xFF00) >> 8); // P1 = HiByte(offset) 
        SelectReadBinary.at(3) = (offset & 0xFF); // P2 = LoByte(Offset) }
		if(DIAGLIB_OK != (iReturnCode = SendApdu(hCard,SelectReadBinary,&tmpResponse,&SW1,&SW2,TransmitDelay)))
		{
			LOG_ERROR(L"SendApdu 'Read binary' failed");
			if(DIAGLIB_OK != EndTransaction(hCard))
			{
				LOG_ERROR(L"ReadCardFile 'EndTransaction' failed");
			}
			return iReturnCode;
		}

        // als de offset voorbij EOF zit, hebben we gedaan 
        if (SW1 == 0x6B && SW2 == 0x00)
		{
            break;
        }

        // als er niet meer zoveel is: lees laatste of enige stukje en stop 
        if (SW1 == 0x6C) 
		{
            SelectReadBinary.at(4) = SW2; // gebruik juiste lengte voor Le 
            continue;
        }
        // als MAX_READ_SIZE bytes gelezen: accumuleer en lees verder 
        if (SW1 != 0x90 || SW2 != 0x00)
		{
			LOG_PCSCERROR(L"SendApdu 'Read binary' failed",0,SW1,SW2);
			iReturnCode = DIAGLIB_ERR_PCSC_TRANSMIT_FAILED;
			break;
		}
		else
		{
			for (std::vector<BYTE>::const_iterator itr = tmpResponse.begin(); itr != tmpResponse.end(); itr++)
				Content->push_back(*itr);
            offset += MAX_READ_SIZE;
        }
    }

	if (DIAGLIB_OK != (iReturnCode = EndTransaction(hCard)))
	{
		LOG_ERROR(L"EndTransaction failed");
		return iReturnCode;
	}

    return iReturnCode;

}

////////////////////////////////////////////////////////////////////////////////////////////////
int GetTlvValue(const std::vector<BYTE>&FileContent, int Tag, std::wstring *Value) 
{
	int iReturnCode = DIAGLIB_OK;

	if(Value == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

    Value->clear();

	std::string strValue = "";

    int ip;
    int currentTag;
    int size;

    ip = 0;
    while (ip < static_cast<int> (FileContent.size())) 
	{
        currentTag = FileContent.at(ip++);
        size = FileContent.at(ip++);
        if (size == 255) 
		{
            size = (size << 8) | FileContent.at(ip++);
        }
        if (currentTag == Tag) 
		{
            int j = ip;
            for (int i = 0; i < size; ++i) 
			{
                strValue.push_back(FileContent.at(j++));
            }
            break;
        }
        ip += size;
    }

	Value->append(wstring_From_string(strValue));

    return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int GetATR(SCARDHANDLE hCard, std::wstring *atr)
{
	int iReturnCode = DIAGLIB_OK;

	if((void*)hCard == NULL || atr == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	atr->clear();

	DWORD dwReaderLen = 0;
	DWORD dwState, dwProtocol;
	unsigned char ucAtr[64];
	DWORD dwATRLen = sizeof(ucAtr);

	int err=SCARD_S_SUCCESS;

    if (SCARD_S_SUCCESS != (err = g_fSCardStatus(hCard, NULL, &dwReaderLen, &dwState, &dwProtocol, ucAtr, &dwATRLen))) 
	{
		LOG_ERRORCODE(L"SCardStatus failed",err);
		iReturnCode = DIAGLIB_ERR_INTERNAL;
    }
	else
	{
		wchar_t buff[3];
		for(DWORD i=0;i<dwATRLen;i++)
		{
			if(-1==swprintf_s(buff,3,L"%02X",ucAtr[i]))
			{
				LOG_ERROR(L"swprintf_s failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
				break;
			}
			else
			{
				atr->append(buff);
			}
		}
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int GetSerial(SCARDHANDLE hCard, Card_SERIAL *serial, int * const TransmitDelay)
{
	int iReturnCode = DIAGLIB_OK;

	if((void*)hCard == NULL || serial == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	serial->clear();

	BYTE SW1=0xFF;
	BYTE SW2=0xFF;
	
    const BYTE GET_INFO_APDU[] = {0x80, 0xE4, 0x00, 0x00, 0x1C};

    std::vector<BYTE> GetInfoCommand;

    for (unsigned int i = 0; i < sizeof (GET_INFO_APDU); ++i)
        GetInfoCommand.push_back(GET_INFO_APDU[i]);

	if(DIAGLIB_OK != (iReturnCode = BeginTransaction(hCard)))
	{
		return iReturnCode;
	}

	if(DIAGLIB_OK != (iReturnCode = SelectApplet(hCard, TransmitDelay)))
	{
		return iReturnCode;
	}

	std::vector<BYTE> Response;
    //Run the 'select file'
	
    if(DIAGLIB_OK != (iReturnCode = SendApdu(hCard,GetInfoCommand,&Response,&SW1,&SW2,TransmitDelay)))
	{
		LOG_ERROR(L"SendApdu 'Get info' failed");
		if(DIAGLIB_OK != EndTransaction(hCard))
		{
			LOG_ERROR(L"EndTransaction failed");
		}
		return iReturnCode;
	}

	if(DIAGLIB_OK != (iReturnCode = EndTransaction(hCard)))
	{
		LOG_ERROR(L"EndTransaction failed");
		return iReturnCode;
	}

    if (SW1 != 0x90 || SW2 != 0x00) 
	{
		LOG_PCSCERROR(L"SendApdu 'Get info' failed",0,SW1,SW2);
		if(DIAGLIB_OK != (iReturnCode = EndTransaction(hCard)))
		{
			LOG_ERROR(L"EndTransaction failed");
		}
		return RETURN_LOG_INTERNAL_ERROR;
    }

	wchar_t buff[3];
	for(int i=0;i<16;i++)
	{
		if(-1==swprintf_s(buff,3,L"%02X",Response[i]))
		{
			LOG_ERROR(L"swprintf_s failed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
			break;
		}
		else
		{
			serial->append(buff);
		}
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SelectMF(SCARDHANDLE hCard, int * const TransmitDelay)
{
	int iReturnCode = DIAGLIB_OK;

	if((void*)hCard == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if (DIAGLIB_OK != (iReturnCode = BeginTransaction(hCard)))
	{
		return iReturnCode;
	}

	if(DIAGLIB_OK != (iReturnCode = SelectApplet(hCard, TransmitDelay)))
	{
		LOG_ERROR(L"SelectMF 'SelectApplet' failed");
		if(DIAGLIB_OK != (iReturnCode = EndTransaction(hCard)))
		{
			LOG_ERROR(L"SelectMF 'EndTransaction' failed");
		}
		return iReturnCode;
	}

	BYTE SW1=0xFF;
	BYTE SW2=0xFF;
	
    const BYTE SELECT_MF_APDU[] = {0x00, 0xA4, 0x02, 0x0C, 0x02, 0x3F, 0x00};

    std::vector<BYTE> SelectMfCommand;

    for (unsigned int i = 0; i < sizeof (SELECT_MF_APDU); ++i)
        SelectMfCommand.push_back(SELECT_MF_APDU[i]);

	std::vector<BYTE> Response;
    //Run the 'select file'
    if(DIAGLIB_OK != (iReturnCode = SendApdu(hCard,SelectMfCommand,&Response,&SW1,&SW2,TransmitDelay)))
	{
		LOG_ERROR(L"SendApdu 'Select MF' failed");
		if(DIAGLIB_OK != (iReturnCode = EndTransaction(hCard)))
		{
			LOG_ERROR(L"EndTransaction failed");
		}
		return iReturnCode;
	}

    if (SW1 != 0x90 || SW2 != 0x00) 
	{
		LOG_PCSCERROR(L"SendApdu 'Select MF' failed",0,SW1,SW2);
		if (DIAGLIB_OK != (iReturnCode = EndTransaction(hCard)))
		{
			LOG_ERROR(L"EndTransaction 'Select MF' failed");
		}
		return RETURN_LOG_INTERNAL_ERROR;
    }

	if (DIAGLIB_OK != (iReturnCode = EndTransaction(hCard)))
	{
		LOG_ERROR(L"EndTransaction 'Select MF' failed");
	}

	return iReturnCode;
}
