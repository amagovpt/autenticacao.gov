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
/****************************************************************************************************/

#include "globmdrv.h"

#include "log.h"
#include "util.h"
#include "smartcard.h"

/****************************************************************************************************/

//
// Function: CardAuthenticatePin
//

#define WHERE "CardAuthenticatePin()"
DWORD WINAPI   CardAuthenticatePin
	(
	__in                 PCARD_DATA  pCardData,
	__in                 LPWSTR      pwszUserId,
	__in_bcount(cbPin)   PBYTE       pbPin,
	__in                 DWORD       cbPin,
	__out_opt            PDWORD      pcAttemptsRemaining
	)
{
	DWORD dwReturn = SCARD_S_SUCCESS;
	PIN_INFO pbPinInfo;
	DWORD dwDataLen;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");
	

	/********************/
	/* Check Parameters */
	/********************/
	if ( pCardData == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if(wcscmp(pwszUserId, L"user") != 0 && wcscmp(pwszUserId, L"anonymous") != 0
		&& wcscmp(pwszUserId, L"admin") != 0)
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pwszUserId]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if(pbPin == NULL || cbPin == 0)
	{	
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbPin][cbPin]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	dwReturn = CardGetProperty(pCardData, 
		CP_CARD_PIN_INFO, 
		(PBYTE) &pbPinInfo, 
		sizeof(pbPinInfo), 
		&dwDataLen, 
		ROLE_DIGSIG); /* Apparently CardAuthenticatePin() assumes authentication PIN */

	if (dwReturn != 0) {
		LogTrace(LOGTYPE_ERROR, WHERE, "CardGetProperty Failed: %02X", dwReturn);
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if ( pbPinInfo.PinType == ExternalPinType ) {	
		dwReturn = PteidAuthenticateExternal(pCardData, pcAttemptsRemaining, FALSE, 0);
	} 
	else {
		dwReturn = PteidAuthenticate(pCardData, pbPin, cbPin, pcAttemptsRemaining, 0);
	}

	if ( dwReturn != SCARD_S_SUCCESS )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Logon: [0x%02X]", dwReturn);
		CLEANUP(dwReturn);
	}

cleanup:

	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardGetChallenge
//

#define WHERE "CardGetChallenge()"
DWORD WINAPI   CardGetChallenge
	(
	__in                                   PCARD_DATA  pCardData,
	__deref_out_bcount(*pcbChallengeData)  PBYTE       *ppbChallengeData,
	__out                                  PDWORD      pcbChallengeData
	)
{
	DWORD    dwReturn = SCARD_S_SUCCESS;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardAuthenticateChallenge
//

#define WHERE "CardAuthenticateChallenge()"
DWORD WINAPI   CardAuthenticateChallenge
	(
	__in                             PCARD_DATA  pCardData,
	__in_bcount(cbResponseData)      PBYTE       pbResponseData,
	__in                             DWORD       cbResponseData,
	__out_opt                        PDWORD      pcAttemptsRemaining
	)
{
	DWORD    dwReturn = SCARD_S_SUCCESS;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardDeauthenticate
//
// Purpose: De-authenticate the specified logical user name on the card.
//
// This is an optional API.  If implemented, this API is used instead
// of SCARD_RESET_CARD by the Base CSP.  An example scenario is leaving
// a transaction in which the card has been authenticated (a Pin has been
// successfully presented).
//
// The pwszUserId parameter will point to a valid well-known User Name (see
// above).
//
// The dwFlags parameter is currently unused and will always be zero.
//
// Card modules that choose to not implement this API must set the CARD_DATA
// pfnCardDeauthenticate pointer to NULL.
//

#define WHERE "CardDeauthenticate()"
DWORD WINAPI   CardDeauthenticate
	(
	__in     PCARD_DATA  pCardData,
	__in     LPWSTR      pwszUserId,
	__in     DWORD       dwFlags
	)
{
	return SCARD_E_UNSUPPORTED_FEATURE;
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardAuthenticateEx
//

#define WHERE "CardAuthenticateEx()"
DWORD WINAPI   CardAuthenticateEx  
	(
	__in                                   PCARD_DATA     pCardData,
	__in                                   PIN_ID         PinId,
	__in                                   DWORD          dwFlags,
	__in                                   PBYTE          pbPinData,
	__in                                   DWORD          cbPinData,
	__deref_out_bcount_opt(*pcbSessionPin) PBYTE          *ppbSessionPin,
	__out_opt                              PDWORD         pcbSessionPin,
	__out_opt                              PDWORD         pcAttemptsRemaining
	)
{

	/*
	__in                 PCARD_DATA  pCardData,
	__in                 LPWSTR      pwszUserId,
	__in_bcount(cbPin)   PBYTE       pbPin,
	__in                 DWORD       cbPin,
	__out_opt            PDWORD      pcAttemptsRemaining

	*/
	DWORD dwReturn = SCARD_S_SUCCESS;
	PIN_INFO pbPinInfo;
	DWORD dwDataLen;
	BYTE specific_pin_id;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	/********************/
	/* Check Parameters */
	/********************/
	if ( pCardData == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( ( PinId != ROLE_DIGSIG   ) &&
		( PinId != ROLE_NONREP   ) )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [PinId]:[0x%02X]", PinId);
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if ( ( (dwFlags & CARD_AUTHENTICATE_SESSION_PIN         ) == CARD_AUTHENTICATE_SESSION_PIN         ) ||
		( (dwFlags & CARD_AUTHENTICATE_GENERATE_SESSION_PIN) == CARD_AUTHENTICATE_GENERATE_SESSION_PIN) )
	{
		if ( ( ppbSessionPin == NULL ) ||
			( pcbSessionPin == NULL ) )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [ppbSessionPin][pcbSessionPin]");
			CLEANUP(SCARD_E_INVALID_PARAMETER);
		}
	}

	//dwReturn = PteidMSE(pCardData, PinId);
	//if ( dwReturn != SCARD_S_SUCCESS )
	//{
	//	LogTrace(LOGTYPE_ERROR, WHERE, "MSE: [0x%02X]", dwReturn);
	//	CLEANUP(dwReturn);
	//}
	/* External Pin?  */
	dwReturn = CardGetProperty(pCardData, 
		CP_CARD_PIN_INFO, 
		(PBYTE) &pbPinInfo, 
		sizeof(pbPinInfo), 
		&dwDataLen, 
		PinId);
	if (dwReturn != 0) {
		LogTrace(LOGTYPE_ERROR, WHERE, "CardGetProperty Failed: %02X", dwReturn);
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	specific_pin_id = PinId == ROLE_DIGSIG ? 0 : 1;

	if ( pbPinInfo.PinType == ExternalPinType ) {	
		dwReturn = PteidAuthenticateExternal(pCardData, pcAttemptsRemaining, (dwFlags & CARD_PIN_SILENT_CONTEXT ) == CARD_PIN_SILENT_CONTEXT,
				specific_pin_id);
	} 
	else {
		dwReturn = PteidAuthenticate(pCardData, pbPinData, cbPinData, pcAttemptsRemaining, specific_pin_id);
	}

	if ( dwReturn != SCARD_S_SUCCESS )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Logon: [0x%02X]", dwReturn);
		CLEANUP(dwReturn);
	}

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardGetChallengeEx
//

#define WHERE "CardGetChallengeEx()"
DWORD WINAPI   CardGetChallengeEx
	(
	__in                                   PCARD_DATA  pCardData,
	__in                                   PIN_ID      PinId,
	__deref_out_bcount(*pcbChallengeData)  PBYTE       *ppbChallengeData,
	__out                                  PDWORD      pcbChallengeData,
	__in                                   DWORD       dwFlags
	)
{
	DWORD    dwReturn = SCARD_S_SUCCESS;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardDeauthenticateEx
//

#define WHERE "CardDeauthenticateEx()"
DWORD WINAPI   CardDeauthenticateEx
	(
	__in  PCARD_DATA  pCardData,
	__in  PIN_SET     PinId,
	__in  DWORD       dwFlags
	)
{
	return SCARD_E_UNSUPPORTED_FEATURE;
	
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardChangeAuthenticatorEx
//

#define WHERE "CardChangeAuthenticatorEx()"
DWORD WINAPI   CardChangeAuthenticatorEx
	(
	__in                                PCARD_DATA  pCardData,
	__in                                DWORD       dwFlags,
	__in                                PIN_ID      dwAuthenticatingPinId,
	__in_bcount(cbCurrentAuthenticator) PBYTE       pbAuthenticatingPinData,
	__in                                DWORD       cbAuthenticatingPinData,
	__in                                PIN_ID      dwTargetPinId,
	__in_bcount(cbNewAuthenticator)     PBYTE       pbTargetData,
	__in                                DWORD       cbTargetData,
	__in                                DWORD       cRetryCount,
	__out_opt                           PDWORD      pcAttemptsRemaining
	)
{
	DWORD    dwReturn = SCARD_S_SUCCESS;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardUnblockPin
//

#define WHERE "CardUnblockPin()"
DWORD WINAPI   CardUnblockPin
	(
	__in                                PCARD_DATA  pCardData,
	__in                                LPWSTR      pwszUserId,
	__in_bcount(cbAuthenticationData)   PBYTE       pbAuthenticationData,
	__in                                DWORD       cbAuthenticationData,
	__in_bcount(cbNewPinData)           PBYTE       pbNewPinData,
	__in                                DWORD       cbNewPinData,
	__in                                DWORD       cRetryCount,
	__in                                DWORD       dwFlags
	)
{
	DWORD    dwReturn = SCARD_S_SUCCESS;
	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardChangeAuthenticator
//

#define WHERE "CardChangeAuthenticator()"
DWORD WINAPI   CardChangeAuthenticator
	(
	__in                                   PCARD_DATA    pCardData,
	__in                                   LPWSTR        pwszUserId,
	__in_bcount(cbCurrentAuthenticator)    PBYTE         pbCurrentAuthenticator,
	__in                                   DWORD         cbCurrentAuthenticator,
	__in_bcount(cbNewAuthenticator)        PBYTE         pbNewAuthenticator,
	__in                                   DWORD         cbNewAuthenticator,
	__in                                   DWORD         cRetryCount,
	__in                                   DWORD         dwFlags,
	__out_opt                              PDWORD        pcAttemptsRemaining
	)
{
	DWORD dwReturn = SCARD_S_SUCCESS;

	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

	/********************/
	/* Check Parameters */
	/********************/
	if ( pCardData == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( pwszUserId == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pwszUserId]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( pbCurrentAuthenticator == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbCurrentAuthenticator]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( pbNewAuthenticator == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbNewAuthenticator]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if ( wcscmp(pwszUserId, wszCARD_USER_USER) != 0 )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter value [pwszUserId]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	if ( dwFlags != CARD_AUTHENTICATE_PIN_PIN )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	/* Don't support setting the retry count */
	if ( cRetryCount != 0 )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cRetryCount]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}

	dwReturn = PteidChangePIN(pCardData, 
		pbCurrentAuthenticator,
		cbCurrentAuthenticator,
		pbNewAuthenticator,
		cbNewAuthenticator,
		pcAttemptsRemaining,
		0 //Auth PIN
		);
	if ( dwReturn != SCARD_S_SUCCESS )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "ChangePIN: [0x%02X]", dwReturn);
		CLEANUP(dwReturn);
	}

cleanup:
	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/
