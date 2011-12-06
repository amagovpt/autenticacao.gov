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
#include "smartcard.h"
#include "externalpinui.h"
#include "util.h"
#include "cache.h"

#include <commctrl.h>
/****************************************************************************************************/

#define CHALLENGE_DATA_SIZE         16

#define BELPIC_MAX_FILE_SIZE        65535
#define BELPIC_PIN_BUF_SIZE         8
#define BELPIC_MIN_USER_PIN_LEN     4
#define BELPIC_MAX_USER_PIN_LEN     12

#define BELPIC_PAD_CHAR			    0xFF
#define BELPIC_KEY_REF_NONREP		0x83

/****************************************************************************************************/
//Forward declaration of helper functions
void IasSignatureHelper(PCARD_DATA pCardData);


#define WHERE "PteidDelayAndRecover"
void PteidDelayAndRecover(PCARD_DATA  pCardData,
						 BYTE   SW1, 
						 BYTE   SW2,
						 DWORD  dwReturn)
{
	if( (dwReturn == SCARD_E_COMM_DATA_LOST) || (dwReturn == SCARD_E_NOT_TRANSACTED) )
	{
		DWORD ap = 0;
		int i = 0;

		LogTrace(LOGTYPE_WARNING, WHERE, "Card is confused, trying to recover...");

		for (i = 0; (i < 10) && (dwReturn != SCARD_S_SUCCESS); i++)
		{
			if (i != 0)
				Sleep(1000);

			dwReturn = SCardReconnect(pCardData->hScard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, SCARD_RESET_CARD, &ap);
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_TRACE, WHERE, "  [%d] SCardReconnect errorcode: [0x%02X]", i, dwReturn);
				continue;
			}
			// transaction is lost after an SCardReconnect()
			dwReturn = SCardBeginTransaction(pCardData->hScard);
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_TRACE, WHERE, "  [%d] SCardBeginTransaction errorcode: [0x%02X]", i, dwReturn);
				continue;
			}
			dwReturn = PteidSelectApplet(pCardData);
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_TRACE, WHERE, "  [%d] SCardSelectApplet errorcode: [0x%02X]", i, dwReturn);
				continue;
			}

			LogTrace(LOGTYPE_INFO, WHERE, "  Card recovered in loop %d", i);
		}
		if(i >=10)
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X], Failed to recover", dwReturn);
		}
	}
	if ( (( SW1 == 0x90 ) && ( SW2 == 0x00 )) ||
		 ( SW1 == 0x61 ) ||
		 ( SW1 == 0x6c ) )
	{
		;//no error received, no sleep needed
	}
	else
	{
		Sleep(25);
	}
}
#undef WHERE



#define WHERE "PteidAuthenticate"
DWORD PteidAuthenticate(PCARD_DATA   pCardData, 
                       PBYTE        pbPin, 
                       DWORD        cbPin, 
                       PDWORD       pcAttemptsRemaining,
					   BYTE			pin_id) 
{
   DWORD             dwReturn  = 0;

   SCARD_IO_REQUEST  ioSendPci = {0, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {0, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg   = 0;
   unsigned char     recvbuf[256];
   unsigned long     recvlen   = sizeof(recvbuf);
   BYTE              SW1, SW2;
   unsigned int              i = 0;
   unsigned int				 j = 0;
   char				paddingChar;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pbPin == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbPin]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   /* Don't allow zero-length PIN */
   if ( ( cbPin < BELPIC_MIN_USER_PIN_LEN ) ||
        ( cbPin > BELPIC_MAX_USER_PIN_LEN ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cbPin]");
      CLEANUP(SCARD_W_WRONG_CHV);
   }

   PteidSelectApplet(pCardData);
  //00 20 00 81 08 32 31 34 38 FF FF FF FF
   /**********/
   /* Log On */
   /**********/
   Cmd[0] = 0x00;
   Cmd[1] = 0x20;  /* VERIFY COMMAND */
   Cmd[2] = 0x00;
   if (Is_Gemsafe)
	   Cmd[3] = 0x81 + pin_id; /* PIN ID  */
   else
   {
	   if (pin_id == 0)
		   Cmd[3] = 0x01;  /* Authentication PIN  */
	   else
		   Cmd[3] = 0x82;  /* Signature PIN  */
   }

   Cmd[4] = 0x08;  /* PIN Length (including padding) */

   if (Is_Gemsafe)
	   paddingChar = 0xFF;
   else
	   paddingChar = 0x2F;

   while(i < 0x08)
   {
	   if(i < cbPin)
		   Cmd[i+5] = pbPin[i];
	   else
		   Cmd[i+5] = paddingChar;
	   i++;
   }
	   
   uiCmdLg = 13;
   recvlen = sizeof(recvbuf);

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];

   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      dwReturn = SCARD_W_WRONG_CHV;
      LogTrace(LOGTYPE_ERROR, WHERE, "CardAuthenticatePin Failed: [0x%02X][0x%02X]", SW1, SW2);

      if ( ((SW1 == 0x63) && ((SW2 & 0xF0) == 0xC0)) )
      {
         if ( pcAttemptsRemaining != NULL )
         {
            /* -1: Don't support returning the count of remaining authentication attempts */
            *pcAttemptsRemaining = (SW2 & 0x0F);
         }
      }
      else if ( (SW1 == 0x69) && (SW2 == 0x83) )
      {
         dwReturn = SCARD_W_CHV_BLOCKED;
		 LogTrace(LOGTYPE_ERROR, WHERE, "Card Blocked, watch out!!");
      }
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "Logged on...");
   }

cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "PteidAuthenticateExternal"

//Helper macros to ease the transition to 3 arg version of swprintf
#define WRITE_MAIN_INSTRUCTION(MESSAGE) swprintf(wchMainInstruction, 100, MESSAGE)
#define WRITE_ERROR_MESSAGE(MESSAGE) swprintf(wchErrorMessage, 500, MESSAGE)

DWORD PteidAuthenticateExternal(
	PCARD_DATA   pCardData, 
	PDWORD       pcAttemptsRemaining,
	BOOL		 bSilent
	) 
{
	DWORD						dwReturn  = 0;
	SCARD_IO_REQUEST				ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
	SCARD_IO_REQUEST				ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

	PIN_VERIFY_STRUCTURE			verifyCommand;

	unsigned int					uiCmdLg   = 0;
	unsigned char				recvbuf[256];
	unsigned char				ucLastKey;
	unsigned long				recvlen     = sizeof(recvbuf);
	BYTE							SW1, SW2;
	int							i           = 0;
	int							offset		= 0;
	DWORD						dwRetriesLeft, dwDataLen;
	BOOL							bRetry      = TRUE;
	int							nButton;
	HRESULT						hResult;

	EXTERNAL_PIN_INFORMATION		externalPinInfo;
	HANDLE						DialogThreadHandle;


	wchar_t						wchErrorMessage[500];
	wchar_t						wchMainInstruction[100];


	LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

	/********************/
	/* Check Parameters */
	/********************/
	if ( pCardData == NULL )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
		CLEANUP(SCARD_E_INVALID_PARAMETER);
	}


	/*********************/
	/* External PIN Info */
	/*********************/
	externalPinInfo.hCardHandle = pCardData->hScard;
	CCIDgetFeatures(&(externalPinInfo.features), externalPinInfo.hCardHandle);

	/*********************/
	/* Get Parent Window */
	/*********************/
	dwReturn = CardGetProperty(pCardData, 
		CP_PARENT_WINDOW, 
		(PBYTE) &(externalPinInfo.hwndParentWindow), 
		sizeof(externalPinInfo.hwndParentWindow), 
		&dwDataLen, 
		0);
	if (dwReturn != 0) {
		LogTrace(LOGTYPE_ERROR, WHERE, "CardGetProperty Failed: %02X", dwReturn);
		externalPinInfo.hwndParentWindow = NULL;
	}


	/*********************/
	/* Get Pin Context String */
	/*********************/		
	dwReturn = CardGetProperty(pCardData, 
		CP_PIN_CONTEXT_STRING, 
		(PBYTE) externalPinInfo.lpstrPinContextString, 
		sizeof(externalPinInfo.lpstrPinContextString), 
		&dwDataLen, 
		0);
	if (dwReturn != 0) {
		LogTrace(LOGTYPE_ERROR, WHERE, "CardGetProperty Failed: %02X", dwReturn);
		wcscpy(externalPinInfo.lpstrPinContextString, L"");
	}

	/**********/
	/* Log On */
	/**********/

	createVerifyCommand(&verifyCommand);

	uiCmdLg = sizeof(verifyCommand);
	recvlen = sizeof(recvbuf);

	while (bRetry) {
		bRetry = FALSE;
		nButton = -1;

		// We introduce a short sleep before starting the PIN VERIFY procedure
		// Reason: we do this for users with a combined keyboard/secure PIN pad smartcard reader
		//   "enter" key far right on the keyboard ==  "OK" button of the PIN pad
		//   Problem: key becomes PIN-pad button before key is released. Result: the keyup event is not sent.
		//   This sleep gives the user some time to release the Enter key.

		Sleep(100);

		if(externalPinInfo.features.VERIFY_PIN_DIRECT != 0)
		{
			externalPinInfo.iPinCharacters = 0;
			externalPinInfo.cardState = CS_PINENTRY;
			// show dialog
			if (!bSilent)
				DialogThreadHandle = CreateThread(NULL, 0, DialogThreadPinEntry, &externalPinInfo, 0, NULL);

			dwReturn = SCardControl(pCardData->hScard, 
				externalPinInfo.features.VERIFY_PIN_DIRECT, 
				&verifyCommand, 
				uiCmdLg,                              
				recvbuf, 
				recvlen,
				&recvlen);

			externalPinInfo.cardState = CS_PINENTERED;
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "SCardControl errorcode: [0x%02X]", dwReturn);
				CLEANUP(dwReturn);
			}
		}
		else
		{
			dwReturn = SCardControl(pCardData->hScard, 
				externalPinInfo.features.VERIFY_PIN_START, 
				&verifyCommand, 
				uiCmdLg,                              
				recvbuf, 
				recvlen,
				&recvlen);
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "SCardControl errorcode: [0x%02X]", dwReturn);
				CLEANUP(dwReturn);
			}
			externalPinInfo.iPinCharacters = 0;
			externalPinInfo.cardState = CS_PINENTRY;

			// show dialog
			if (!bSilent)
				DialogThreadHandle = CreateThread(NULL, 0, DialogThreadPinEntry, &externalPinInfo, 0, NULL);
			while (1) {
				dwReturn = SCardControl(pCardData->hScard,
					externalPinInfo.features.GET_KEY_PRESSED,
					NULL,
					0,
					recvbuf,
					recvlen,
					&recvlen);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "SCardControl errorcode: [0x%02X]", dwReturn);
					CLEANUP(dwReturn);
				}
				ucLastKey = recvbuf[0];
				switch (recvbuf[0]) {
				case 0x00:
					// No key 
					Sleep(200);
					break;
				case 0x0d:
					// OK button
					goto endkeypress;
				case 0x1b:
					// Cancel button
					goto endkeypress;
				case 0x40:
					// Aborted/timeout
					goto endkeypress;
				case 0x2b:
					// 0-9
					externalPinInfo.iPinCharacters++;
					break;
				case 0x08:	
					// Backspace
					externalPinInfo.iPinCharacters--;
					break;
				case 0x0a:
					// Clear
					externalPinInfo.iPinCharacters = 0;
					break;
				default:
					//printf("Key pressed: 0x%x\n", bRecvBuffer[0]);
					;
				}

			}
endkeypress:

			externalPinInfo.cardState = CS_PINENTERED;
			dwReturn = SCardControl(pCardData->hScard,
				externalPinInfo.features.VERIFY_PIN_FINISH,
				NULL,
				0,
				recvbuf,
				sizeof(recvbuf),
				&recvlen);
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "SCardControl errorcode: [0x%02X]", dwReturn);
				CLEANUP(dwReturn);
			}
			SW1 = recvbuf[recvlen-2];
			SW2 = recvbuf[recvlen-1];
			if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
			{
				dwReturn = SCARD_W_WRONG_CHV;
				LogTrace(LOGTYPE_ERROR, WHERE, "CardAuthenticateEx Failed: [0x%02X][0x%02X]", SW1, SW2);
				if (SW1 == 0x64) {
					//error during pin entry
					switch(SW2){
					case 0x00:
						// Timeout
						if (ucLastKey == 0x0d) {
							// OK button preceded by no other keys also results in 0x64 0x00

							WRITE_MAIN_INSTRUCTION( t[PIN_TOO_SHORT_MAININSTRUCTIONS][getLanguage()]);
							WRITE_ERROR_MESSAGE( t[PIN_TOO_SHORT_CONTENT][getLanguage()] );
						} else {
							WRITE_MAIN_INSTRUCTION( t[PIN_TIMED_OUT_MAININSTRUCTIONS][getLanguage()]);
							// the user entered something but probably forgot to push OK.
							WRITE_ERROR_MESSAGE(t[PIN_TIMED_OUT_CONTENT][getLanguage()]);
						}
						break;
					case 0x01:
						// Cancelled
						WRITE_MAIN_INSTRUCTION( t[PIN_CANCELLED_MAININSTRUCTIONS][getLanguage()]);
						WRITE_ERROR_MESSAGE( t[PIN_CANCELLED_CONTENT][getLanguage()]);
						break;
					case 0x02:
						// PINs do not match
						WRITE_MAIN_INSTRUCTION( t[PIN_DO_NOT_MATCH_MAININSTRUCTIONS][getLanguage()]);
						WRITE_ERROR_MESSAGE( t[PIN_DO_NOT_MATCH_CONTENT][getLanguage()]);
						break;
					case 0x03:
						// PIN size error
						if (externalPinInfo.iPinCharacters > 0 && externalPinInfo.iPinCharacters < BELPIC_MIN_USER_PIN_LEN) {
							// PIN too short
							WRITE_MAIN_INSTRUCTION( t[PIN_TOO_SHORT_MAININSTRUCTIONS][getLanguage()]);
							WRITE_ERROR_MESSAGE( t[PIN_TOO_SHORT_CONTENT][getLanguage()]);
						} else {
							if (externalPinInfo.iPinCharacters >= BELPIC_MAX_USER_PIN_LEN) {
								// PIN too long
								WRITE_MAIN_INSTRUCTION( t[PIN_TOO_LONG_MAININSTRUCTIONS][getLanguage()]);
								WRITE_ERROR_MESSAGE( t[PIN_TOO_LONG_CONTENT][getLanguage()]);
							} else {
								// no info about PIN chars
								WRITE_MAIN_INSTRUCTION(  t[PIN_SIZE_MAININSTRUCTIONS][getLanguage()]);
								WRITE_ERROR_MESSAGE( t[PIN_SIZE_CONTENT][getLanguage()]);
							}
						}
						break;
					default:
						// Should not happen
						WRITE_MAIN_INSTRUCTION( t[PIN_UNKNOWN_MAININSTRUCTIONS][getLanguage()]);
						swprintf (wchErrorMessage, t[PIN_UNKNOWN_CONTENT][getLanguage()], SW1,SW2);
						break;
					}
					if (externalPinInfo.uiState == US_PINENTRY && !bSilent)
						hResult = TaskDialog(externalPinInfo.hwndParentWindow, 
						NULL, 
						t[WINDOW_TITLE][getLanguage()], 
						wchMainInstruction, 
						wchErrorMessage, 
						TDCBF_RETRY_BUTTON  | TDCBF_CANCEL_BUTTON ,
						TD_ERROR_ICON,
						&nButton);

				}
				if (SW1 == 0x63) {
					// Invalid PIN
					dwRetriesLeft = SW2 & 0x0F;
					if ( pcAttemptsRemaining != NULL )
					{
						/* -1: Don't support returning the count of remaining authentication attempts */
						*pcAttemptsRemaining = dwRetriesLeft;
					}
					WRITE_MAIN_INSTRUCTION( t[PIN_INVALID_MAININSTRUCTIONS][getLanguage()]);
					swprintf(wchErrorMessage, t[PIN_INVALID_CONTENT][getLanguage()], dwRetriesLeft);

					if (externalPinInfo.uiState == US_PINENTRY && !bSilent)
						hResult = TaskDialog(externalPinInfo.hwndParentWindow, 
						NULL, 
						t[WINDOW_TITLE][getLanguage()], 
						wchMainInstruction, 
						wchErrorMessage, 
						TDCBF_RETRY_BUTTON  | TDCBF_CANCEL_BUTTON,
						TD_ERROR_ICON,
						&nButton);
				}

				if (SW1 == 0x69 && SW2 == 0x83) {
					// PIN blocked
					WRITE_MAIN_INSTRUCTION( t[PIN_BLOCKED_MAININSTRUCTIONS][getLanguage()]);
					WRITE_ERROR_MESSAGE( t[PIN_BLOCKED_CONTENT][getLanguage()]);
					if (externalPinInfo.uiState == US_PINENTRY && !bSilent)
						hResult = TaskDialog(externalPinInfo.hwndParentWindow, 
						NULL, 
						t[WINDOW_TITLE][getLanguage()], 
						wchMainInstruction, 
						wchErrorMessage, 
						TDCBF_OK_BUTTON,
						TD_ERROR_ICON,
						&nButton);
					dwReturn = SCARD_W_CHV_BLOCKED;
				}
				bRetry = (nButton == IDRETRY);

			}
			else
			{
				LogTrace(LOGTYPE_INFO, WHERE, "Logged on...");
			}
		}
	}

cleanup:

	LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
	return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "PteidDeAuthenticate"
DWORD PteidDeAuthenticate(PCARD_DATA    pCardData) 
{
   DWORD             dwReturn  = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg   = 0;
   unsigned char     recvbuf[256];
   unsigned long     recvlen   = sizeof(recvbuf);
   BYTE              SW1, SW2;
   int               i         = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   /***********/
   /* Log Off */
   /***********/
   Cmd [0] = 0x80;
   Cmd [1] = 0xE6; /* LOG OFF */
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   uiCmdLg = 4;
   recvlen = sizeof(recvbuf);

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   if ( (SW1 != 0x90) || (SW2 != 0x00) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit status bytes: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "PteidMSE"
DWORD PteidMSE(PCARD_DATA   pCardData, 
			     BYTE      key_id) 
{
  
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {0, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {0, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;
   unsigned char     recvbuf[256+2];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   int               i = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");
   /*
    * The MSE: SET Command will fail with error 0x000006f7
    * if the command is executed too fast after an command which resulted in an error condition
    */
   // Sleep(20);
   Cmd [0] = 0x00;
   Cmd [1] = 0x22; //MSE
   Cmd [2] = 0x41;
   if (Is_Gemsafe)
   {
	   Cmd [3] = 0xB6;
	   Cmd [4] = 0x06; //Length of data
	   Cmd [5] = 0x80; //Tag (Algorithm ID)
	   Cmd [6] = 0x01;
	   Cmd [7] = 0x02; 
	   Cmd [8] = 0x84; //Tag (Key Reference) 
	   Cmd [9] = 0x01;
	   if(key_id == 0) 
		Cmd [10] = 0x02; //Auth keyRef
	   else
		Cmd [10] = 0x01; //Sign keyRef
	   uiCmdLg = 11;

   }
   else
   {
	   Cmd [3] = 0xA4;
	   Cmd [4] = 0x09; //Length of data
	   Cmd [5] = 0x95; //Tag (Usage Qualifier)
	   Cmd [6] = 0x01;
	   Cmd [7] = 0x40;
	   Cmd [8] = 0x84; //Tag (Key Reference)
	   Cmd [9] = 0x01;
	   if(key_id == 0)
		Cmd [10] = 0x01;
	   else
		Cmd [10] = 0x82;
	   Cmd [11] = 0x80; //Tag (Algorithm Reference)
	   Cmd [12] = 0x01;
	   Cmd [13] = 0x02; //(RSA-PKCS#1)
	   uiCmdLg = 14;
   }
  
   //Sleep(1000);
   LogTrace(LOGTYPE_INFO, WHERE, "APDU MSE");
   LogDump (uiCmdLg, (char *)Cmd);
  
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SET) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SET Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "PteidChangePIN"
DWORD    PteidChangePIN
         (
            PCARD_DATA  pCardData, 
            PBYTE       pbCurrentAuthenticator,
            DWORD       cbCurrentAuthenticator,
            PBYTE       pbNewAuthenticator,
            DWORD       cbNewAuthenticator,
            PDWORD      pcAttemptsRemaining
         ) 
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;
   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   int               i        = 0;
   int               offset   = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
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
   if ( ( cbCurrentAuthenticator < BELPIC_MIN_USER_PIN_LEN ) ||
        ( cbCurrentAuthenticator > BELPIC_MAX_USER_PIN_LEN ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cbCurrentAuthenticator]");
      CLEANUP(SCARD_W_WRONG_CHV);
   }
   if ( ( cbNewAuthenticator < BELPIC_MIN_USER_PIN_LEN ) ||
        ( cbNewAuthenticator > BELPIC_MAX_USER_PIN_LEN ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cbCurrentAuthenticator]");
      CLEANUP(SCARD_W_WRONG_CHV);
   }

   /* TODO: This must be corrected see PkiCard.cpp in cardlayer sub-project
   /* Change PIN code: Old PIN + New PIN + Padding */
   Cmd [0] = 0x00;
   Cmd [1] = 0x24;   /* CHANGE REFERENCE DATA COMMAND    */
   Cmd [2] = 0x00;   /* Support 'USER' password change   */
   Cmd [3] = 0x01;
   Cmd [4] = 0x10;

   /* Fill verification data with padding character */
   for ( i = 0 ; i < 0x10 ; i++ )
   {
      Cmd [5 + i] = BELPIC_PAD_CHAR;
   }

   Cmd [5] = 0x20 + (unsigned char)cbCurrentAuthenticator;  /* 0x20 + length of pin */
   for ( i = 0 ; i < (unsigned char) cbCurrentAuthenticator ; i++ )
   {
    offset = 6 + (i/2);

      if ( (i % 2) == 0 )
      {
         Cmd [offset] = (((pbCurrentAuthenticator[i] - 48) << 4) & 0xF0);
      }
      else
      {
         Cmd [offset] = (Cmd[offset] & 0xF0) + ((pbCurrentAuthenticator[i] - 48) & 0x0F);
      }
   }
   Cmd [13] = 0x20 + (unsigned char)cbNewAuthenticator;  /* 0x20 + length of pin */
   for ( i = 0 ; i < (unsigned char) cbNewAuthenticator ; i++ )
   {
    offset = 14 + (i/2);

      if ( (i % 2) == 0 )
      {
         Cmd [offset] = (((pbNewAuthenticator[i] - 48) << 4) & 0xF0);
      }
      else
      {
         Cmd [offset] = (Cmd[offset] & 0xF0) + ((pbNewAuthenticator[i] - 48) & 0x0F);
      }
   }

   uiCmdLg = 21;
   recvlen = sizeof(recvbuf);

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }

   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      dwReturn = SCARD_W_WRONG_CHV;
      LogTrace(LOGTYPE_ERROR, WHERE, "CardChangeAuthenticator Failed: [0x%02X][0x%02X]", SW1, SW2);

      if ( ((SW1 == 0x63) && ((SW2 & 0xF0) == 0xC0)) )
      {
         if ( pcAttemptsRemaining != NULL )
         {
            /* -1: Don't support returning the count of remaining authentication attempts */
            *pcAttemptsRemaining = (SW2 & 0x0F);
         }
      }
      else if ( (SW1 == 0x69) && (SW2 == 0x83) )
      {
         dwReturn = SCARD_W_CHV_BLOCKED;
      }
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "Changed PIN...");
   }

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "PteidGetCardSN"
DWORD PteidGetCardSN(PCARD_DATA  pCardData, 
	PBYTE pbSerialNumber, 
	DWORD cbSerialNumber, 
	PDWORD pdwSerialNumber) 
{
   DWORD                   dwReturn = 0;

   SCARD_IO_REQUEST        ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST        ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char           Cmd[128];
   unsigned int            uiCmdLg = 0;

   unsigned char           recvbuf[256];
   unsigned long           recvlen = sizeof(recvbuf);
   BYTE                    SW1, SW2;

   int                     i = 0;
   int                     iWaitApdu = 100;
   int   				      bRetry = 0;

   if (cbSerialNumber < 16) {
		CLEANUP(ERROR_INSUFFICIENT_BUFFER);
   }

   PteidSelectApplet(pCardData);

   *pdwSerialNumber = 0;
   Cmd [0] = 0x00;
   Cmd [1] = 0xCA;
   if (Is_Gemsafe)
   {
   Cmd [2] = 0xDF;
   Cmd [3] = 0x30;
   Cmd [4] = 0x08;
   }
   else
   {
   Cmd [2] = 0x02;
   Cmd [3] = 0x5A;
   Cmd [4] = 0x0D;
   }
   uiCmdLg = 5;

	do {

		Sleep(iWaitApdu);
		recvlen = sizeof(recvbuf);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
		SW1 = recvbuf[recvlen-2];
		SW2 = recvbuf[recvlen-1];
		PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
		i = i + 1;
		bRetry = 0;
		if (dwReturn == SCARD_E_COMM_DATA_LOST)
		{
			bRetry++;
			LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit failed with SCARD_E_COMM_DATA_LOST. Sleep %d ms and try again", iWaitApdu);
		}
		if (dwReturn == SCARD_S_SUCCESS)
		{
			// 6d = "command not available in current life cycle"
			if ( SW1 == 0x6d )
			{
				LogTrace(LOGTYPE_TRACE, WHERE, "SCardTransmit returned SW1 = 6d. Sleep %d ms and try again", iWaitApdu);
				bRetry++;
			}
		}
	} while (bRetry != 0 && i < 10);

   if ( dwReturn != SCARD_S_SUCCESS )
   {
		LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (GET_CARD_DATA) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];

  
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Bad status bytes: [0x%02X][0x%02X]", SW1, SW2);
		CLEANUP(SCARD_E_UNEXPECTED);
   }
   if (Is_Gemsafe)
   {
	*pdwSerialNumber = 8;
   memcpy(pbSerialNumber, recvbuf, 8);
   }
   else
   {
   *pdwSerialNumber = 13; //For IAS: length(SerialNumber) = 13
   memcpy(pbSerialNumber, recvbuf, 13);
   }

cleanup:
   return (dwReturn);
}

void IasSignatureHelper(PCARD_DATA pCardData)
{
  /* This is another magic incantation (probably RE'd from APDU captures)
	needed for IAS signing with "unusual" hash lengths  */
   DWORD                   dwReturn = 0;

   SCARD_IO_REQUEST        ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST        ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned int            uiCmdLg = 0;
   unsigned int		       i = 0;

   unsigned char           recvbuf[1024];
   unsigned long           recvlen = sizeof(recvbuf);

   BYTE apdu1[] = {0x00, 0xA4, 0x04, 0x00, 0x60, 0x46, 0x32,0xFF, 0x00, 0x01, 0x02};
   BYTE apdu2[] = {0x00, 0xA4, 0x03, 0x0C};
   BYTE apdu3[] = {0x00, 0xA4, 0x09, 0x00, 0x2F, 0x00};	
   BYTE apdu4[] = {0x00, 0xB0, 0x00, 0x00, 0x3E};
   BYTE apdu5[] = {0x00, 0xA4, 0x09, 0x0C, 0x4F, 0x00};
   BYTE apdu6[] = {0x00, 0xA4, 0x09, 0x00, 0x50, 0x32};
   BYTE apdu7[] = {0x00, 0xB0, 0x00, 0x00, 0x2F};
   BYTE apdu8[] = {0x84, 0x00, 0x00, 0x08}; //To be sent 3 times (?!!)
   BYTE apdu9[] = {0x00, 0xA4, 0x03, 0x0C};
   BYTE apdu10[] = {0x00, 0xA4, 0x09, 0x0C, 0x5F, 0x00};

   uiCmdLg = sizeof(apdu1);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu1, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   uiCmdLg = sizeof(apdu2);
   
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu2, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
	uiCmdLg = sizeof(apdu3);
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu3, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
	uiCmdLg = sizeof(apdu4);
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu4, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
	uiCmdLg = sizeof(apdu5);
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu5, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
	uiCmdLg = sizeof(apdu6);
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu6, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
	uiCmdLg = sizeof(apdu7);
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu7, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
							&recvlen);
	uiCmdLg = sizeof(apdu8);
	while (i != 3)
	{
		dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu8, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
							&recvlen);

		i++;
	}
	uiCmdLg = sizeof(apdu9);
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu9, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
							&recvlen);
	
	uiCmdLg = sizeof(apdu10);
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            apdu10, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
							&recvlen);
	
}

#undef WHERE

/****************************************************************************************************/

#define WHERE "PteidSignData"
DWORD PteidSignData(PCARD_DATA pCardData, BYTE pin_id, DWORD cbToBeSigned, PBYTE pbToBeSigned, DWORD *pcbSignature, PBYTE *ppbSignature)
{

   DWORD                   dwReturn = 0;

   SCARD_IO_REQUEST        ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST        ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char           Cmd[128];
   unsigned int            uiCmdLg = 0;

   unsigned char           recvbuf[1024];
   unsigned long           recvlen = sizeof(recvbuf);
   BYTE                    SW1, SW2;

   unsigned int            i          = 0;
   unsigned int            cbHdrHash  = 0;
   const unsigned char     *pbHdrHash = NULL;
	
   PteidSelectApplet(pCardData);
   dwReturn = PteidMSE(pCardData, pin_id);

   if (dwReturn != SCARD_S_SUCCESS)
   {
	CLEANUP(dwReturn);
   }

   /* Sign Command for IAS*/
   /* 00 88 02 00 24 EC 61 B0 5B 70 33 78 39 F0 C8 C5 EB 79 64 */
   Cmd [0] = 0x00; //0x88, 0x02, 0x00
   Cmd [1] = 0x88;   /* PSO: Compute Digital Signature COMMAND */
   Cmd [2] = 0x02;
   Cmd [3] = 0x00; 
   Cmd [4] = (BYTE)(cbToBeSigned);
   memcpy(Cmd + 5, pbToBeSigned, cbToBeSigned);
   uiCmdLg = 5 + cbToBeSigned;
   
#ifdef _DEBUG
   LogDumpBin("C:\\SmartCardMinidriverTest\\signdata.bin", cbHdrHash + cbToBeSigned, (char *)&Cmd[5]);
#endif
   
   //printByteArray(Cmd, uiCmdLg);
   recvlen = sizeof(recvbuf);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (SIGN) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }

   if ( ( SW1 != 0x61 ) || ( SW2 != 0x80 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Sign Failed: [0x%02X][0x%02X]", SW1, SW2);

      if ( SW1 == 0x69 )
      {
         CLEANUP(SCARD_W_SECURITY_VIOLATION);
      }
      else
      {
         CLEANUP(SCARD_E_UNEXPECTED);
      }
   }

   /* Retrieve signature Command */
   Cmd [0] = 0x00;
   Cmd [1] = 0xC0;   /* PSO: GET RESPONSE COMMAND */
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   Cmd [4] = 0x80;   /* Length of response */
   uiCmdLg = 5;

   recvlen = sizeof(recvbuf);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (Get Response) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Get Response Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   if ( (recvlen - 2) != 0x80 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid length received: [0x%02X][0x%02X]", recvlen - 2, 0x80);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   *pcbSignature = 0x80;

   /* Allocate memory for the target buffer */
   *ppbSignature = pCardData->pfnCspAlloc(*pcbSignature);

   if ( *ppbSignature == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbSignature]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }
   /* Copy the signature */
   for ( i = 0 ; i < *pcbSignature ; i++ )
   {
      (*ppbSignature)[i] = recvbuf[*pcbSignature - i - 1];
   }

cleanup:
   return (dwReturn);
}
#undef WHERE

#define WHERE "PteidSignDataGemsafe"
DWORD PteidSignDataGemsafe(PCARD_DATA pCardData, BYTE pin_id, DWORD cbToBeSigned, PBYTE pbToBeSigned, DWORD *pcbSignature, PBYTE *ppbSignature)
{

   DWORD                   dwReturn = 0;

   SCARD_IO_REQUEST        ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST        ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char           Cmd[128];
   unsigned int            uiCmdLg = 0;

   unsigned char           recvbuf[1024];
   unsigned long           recvlen = sizeof(recvbuf);
   BYTE                    SW1, SW2;

   unsigned int            i          = 0;
   unsigned int            cbHdrHash  = 0;
   const unsigned char     *pbHdrHash = NULL;
	
   dwReturn = PteidMSE(pCardData, pin_id);

   if (dwReturn != SCARD_S_SUCCESS)
   {
	CLEANUP(dwReturn);
   }

   /* Sign Command for GEMSAFE*/
   Cmd [0] = 0x00;
   Cmd [1] = 0x2A;   /* PSO: Hash COMMAND */
   Cmd [2] = 0x90;
   Cmd [3] = 0xA0; 
   Cmd [4] = cbToBeSigned+2; // The value of cbToBeSigned should always fit a single byte so this cast is safe 
   Cmd [5] = 0x90;
   Cmd [6] = (BYTE)(cbToBeSigned);
   memcpy(Cmd + 7, pbToBeSigned, cbToBeSigned);
   uiCmdLg = 7 + cbToBeSigned;
   
#ifdef _DEBUG
   LogDumpBin("C:\\SmartCardMinidriverTest\\signdata.bin", cbHdrHash + cbToBeSigned, (char *)&Cmd[5]);
   
   LogTrace(LOGTYPE_INFO, WHERE, "APDU PSO Hash");
   LogDump (uiCmdLg, (char *)Cmd);

#endif
   
   //printByteArray(Cmd, uiCmdLg);
   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   //PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
	   LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (PSO: Hash) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   LogTrace(LOGTYPE_INFO, WHERE, "Return: APDU PSO Hash");
   LogDump (recvlen, (char *)recvbuf);
  
   Cmd [0] = 0x00;
   Cmd [1] = 0x2A;   /* PSO: Compute Digital Signature COMMAND */
   Cmd [2] = 0x9E;
   Cmd [3] = 0x9A; 
   Cmd [4] = 0x80;  /* Length of expected signature */
   
   uiCmdLg = 5;

   LogTrace(LOGTYPE_INFO, WHERE, "APDU PSO CDS");
   LogDump (uiCmdLg, (char *)Cmd);
   
   recvlen = sizeof(recvbuf);
   dwReturn = SCardTransmit(pCardData->hScard,
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   

   if ( dwReturn != SCARD_S_SUCCESS )
   {
	   LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit (PSO: CDS) errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   LogTrace(LOGTYPE_INFO, WHERE, "Return: APDU PSO CDS");
   LogDump (recvlen, (char *)recvbuf);

   if ( (recvlen - 2) != 0x80 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid length received: [0x%02X][0x%02X]", recvlen - 2, 0x80);
      CLEANUP(SCARD_E_UNEXPECTED);
   }

   *pcbSignature = 0x80;

   /* Allocate memory for the target buffer */
   *ppbSignature = pCardData->pfnCspAlloc(*pcbSignature); //pfnCspAlloc é o Memory allocator fornecido pelo CSP

   if ( *ppbSignature == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbSignature]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }
   /* Copy the signature to output buffer */
   for ( i = 0 ; i < *pcbSignature ; i++ )
   {
      (*ppbSignature)[i] = recvbuf[*pcbSignature - i - 1];
   }

cleanup:
   return (dwReturn);
}

#undef WHERE

/****************************************************************************************************/

#define WHERE "PteidReadFile"
DWORD PteidReadFile(PCARD_DATA  pCardData, DWORD dwOffset, DWORD *cbStream, PBYTE pbStream)
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;

   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

   DWORD             cbRead      = 0;
   DWORD             cbPartRead  = 0;

   /***************/
   /* Read File */
   /***************/
   Cmd [0] = 0x00;
   Cmd [1] = 0xB0; /* READ BINARY COMMAND */
   Cmd [2] = 0x00;
   Cmd [3] = 0x00;
   Cmd [4] = 0x00;
   uiCmdLg = 5;

   while ( ( *cbStream - cbRead ) > 0 )
   {
        Cmd[2] = (BYTE)((dwOffset + cbRead) >> 8);   /* set reading startpoint     */
        Cmd[3] = (BYTE)(dwOffset + cbRead);

      cbPartRead = *cbStream - cbRead;
        if(cbPartRead > PTEID_READ_BINARY_MAX_LEN)    /*if more than maximum length */
      {
         Cmd[4] = PTEID_READ_BINARY_MAX_LEN;        /* is requested, than read    */
      }
        else                                         /* maximum length             */
      {
            Cmd[4] = (BYTE)(cbPartRead);
      }
		recvlen = sizeof(recvbuf);
      dwReturn = SCardTransmit(pCardData->hScard, 
                               &ioSendPci, 
                               Cmd, 
                               uiCmdLg, 
                               &ioRecvPci, 
                               recvbuf, 
                               &recvlen);
		SW1 = recvbuf[recvlen-2];
		SW2 = recvbuf[recvlen-1];
		PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
      if ( dwReturn != SCARD_S_SUCCESS )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
         CLEANUP(dwReturn);
      }

		/* Special case: when SW1 == 0x6C (=incorrect value of Le), we will
		retransmit with SW2 as Le, if SW2 is smaller then the 
		PTEID_READ_BINARY_MAX_LEN */
		if ( ( SW1 == 0x6c ) && ( SW2 <= PTEID_READ_BINARY_MAX_LEN ) ) 
		{
			Cmd[4] = SW2;
			recvlen = sizeof(recvbuf);
			dwReturn = SCardTransmit(pCardData->hScard, 
				&ioSendPci, 
				Cmd, 
				uiCmdLg, 
				&ioRecvPci, 
				recvbuf, 
				&recvlen);
			if ( dwReturn != SCARD_S_SUCCESS )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
				CLEANUP(dwReturn);
			}
			SW1 = recvbuf[recvlen - 2];
			SW2 = recvbuf[recvlen - 1];

		}

      if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
      {

         LogTrace(LOGTYPE_ERROR, WHERE, "Read Binary Failed: [0x%02X][0x%02X]", SW1, SW2);
         CLEANUP(dwReturn);
      }

      memcpy (pbStream + cbRead, recvbuf, recvlen - 2);
      cbRead += recvlen - 2;
   }
	*cbStream = cbRead;
cleanup:
   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "PteidSelectAndReadFile"
DWORD PteidSelectAndReadFile(PCARD_DATA  pCardData, DWORD dwOffset, BYTE cbFileID, PBYTE pbFileID, DWORD *cbStream, PBYTE * ppbStream)
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;

   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   BYTE              SW1, SW2;

	DWORD             cbReadBuf;


//   BYTE              bRead [255];
//   DWORD             cbRead;

//   DWORD             cbCertif;

   /***************/
   /* Select File */
   /***************/
   Cmd [0] = 0x00;
   Cmd [1] = 0xA4; /* SELECT COMMAND */
   Cmd [2] = 0x08;
   Cmd [3] = 0x0C;
   Cmd [4] = cbFileID;
   uiCmdLg = 5;

   memcpy(&Cmd[5], pbFileID, cbFileID);
   uiCmdLg += cbFileID;

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
      CLEANUP(dwReturn);
   }

	*cbStream = 0;
	*ppbStream = NULL;
	cbReadBuf = 1024;
	while (cbReadBuf == 1024) {
		if (*ppbStream == NULL)
			*ppbStream = (PBYTE) pCardData->pfnCspAlloc(*cbStream + cbReadBuf);
		else
			*ppbStream = (PBYTE) pCardData->pfnCspReAlloc(*ppbStream, *cbStream + cbReadBuf);
		
		if (*ppbStream == NULL) {
			LogTrace(LOGTYPE_ERROR, WHERE, "pfnCsp(Re)Alloc failed");
			CLEANUP(dwReturn);
		}

		dwReturn = PteidReadFile(pCardData, dwOffset, &cbReadBuf, *ppbStream + *cbStream * sizeof(BYTE));
		if ( dwReturn != SCARD_S_SUCCESS )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "PteidReadFile errorcode: [0x%02X]", dwReturn);
			pCardData->pfnCspFree(*ppbStream);
			CLEANUP(dwReturn);
		}
		*cbStream = *cbStream + cbReadBuf;
	}
cleanup:
   return (dwReturn);
}

BOOL checkStatusCode(const char * context, DWORD dwReturn, BYTE SW1, BYTE SW2)
{

   if ( dwReturn != SCARD_S_SUCCESS )
   {
      LogTrace(LOGTYPE_ERROR, context, "SCardTransmit errorcode: [0x%02X]", dwReturn);
      return FALSE;
   }
   if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
   {
      LogTrace(LOGTYPE_ERROR, context, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
      return FALSE;
   }

   return TRUE;
}


#undef WHERE
/****************************************************************************************************/

/*
/**/
#define WHERE "PteidReadCert"
DWORD PteidReadCert(PCARD_DATA  pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif)
{
   DWORD             dwReturn = 0;

   SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
   SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

   unsigned char     Cmd[128];
   unsigned int      uiCmdLg = 0;

   unsigned char     recvbuf[256];
   unsigned long     recvlen = sizeof(recvbuf);
   char certPath[25];
   const int		 max_path = 512;
   BYTE              SW1, SW2;

   DWORD             cbCertif;
   
   VENDOR_SPECIFIC * vs;
   int serial_len = 16;
   char * filename;

   cbCertif = 2500; //More than enough for any certificate lmedinas dixit...
   *ppbCertif = pCardData->pfnCspAlloc(cbCertif);
   
   
   vs = (VENDOR_SPECIFIC*)pCardData->pvVendorSpecific;

   if (!runningUnderService())
   {
	   memcpy(certPath, vs->szSerialNumber, serial_len);
	   certPath[serial_len] = translateCertType(dwCertSpec);

	   //More than enough for long file paths...
	   filename = pCardData->pfnCspAlloc(max_path);

	   getCacheFilePath(certPath, filename, max_path);

	   if (readFromCache(filename, *ppbCertif))
	   {
		   *pcbCertif = cbCertif;
		   pCardData->pfnCspFree(filename);
		   return SCARD_S_SUCCESS;
	   }
   }

   // Certificate Not Cached

   PteidSelectApplet(pCardData);
   memset(recvbuf, 0, sizeof(recvbuf));
   /***************/
   /* Select File */
   /***************/
   Cmd [0] = 0x00;
   Cmd [1] = 0xA4; /* SELECT COMMAND */
   Cmd [2] = 0x00;
   Cmd [3] = 0x0C;
   Cmd [4] = 0x02; 
   Cmd [5] = 0x3F; //5F, (EF, 0C), ReadBinary(), (02, CertID)  
   Cmd [6] = 0x00;
   uiCmdLg = 7;

   // 00 A4 09 00 02 EF 09

   dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
                            &recvlen);
   SW1 = recvbuf[recvlen-2];
   SW2 = recvbuf[recvlen-1];
   //PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
	if (!checkStatusCode(WHERE" -> select Dir Root", dwReturn, SW1, SW2))
		CLEANUP(dwReturn);

	Cmd[5] = 0x5F;
	memset(recvbuf, 0, sizeof(recvbuf));
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
		                    &recvlen);
	if (!checkStatusCode(WHERE" -> select Specific Dir", dwReturn, SW1, SW2))
		CLEANUP(dwReturn);
	
	if (!Is_Gemsafe)
	{
	Cmd[2] = 0x09; 
    Cmd[3] = 0x00;
	}
	Cmd[4] = 0x02;
	Cmd[5] = 0xEF;
    Cmd[6] = translateCertType(dwCertSpec);

    memset(recvbuf, 0, sizeof(recvbuf));
	dwReturn = SCardTransmit(pCardData->hScard, 
                            &ioSendPci, 
                            Cmd, 
                            uiCmdLg, 
                            &ioRecvPci, 
                            recvbuf, 
		                    &recvlen);
	if (!checkStatusCode(WHERE" -> select CertFile", dwReturn, SW1, SW2))
		CLEANUP(dwReturn);

   
   if ( *ppbCertif == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbCertif]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }

   dwReturn = PteidReadFile(pCardData, 0, &cbCertif, *ppbCertif);
   if (dwReturn != SCARD_S_SUCCESS)
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "PteidReadFile errorcode: [0x%02X]", dwReturn);
      CLEANUP(dwReturn);
   }
  
	/* Skip caching if running under the Certificate Propagation Service 
	   It would be painful to grab the cachedir because the relevant
	   environment variables APPLOCALDATA will point to somewhere under
	   %WINDIR% (i.e. not to user-writable directories)
	*/
  if (!runningUnderService())
  {
	   LogTrace(LOGTYPE_INFO, WHERE, "certPath: %s", filename);
	   CacheCertificate(filename, *ppbCertif, cbCertif);
	   pCardData->pfnCspFree(filename);
  }

   /* Certificate Length */
   *pcbCertif = cbCertif;

cleanup:
   return (dwReturn);
}
#undef WHERE


#define WHERE "PteidSelectApplet"
/**/
DWORD PteidSelectApplet(PCARD_DATA  pCardData)
{
	DWORD             dwReturn = 0;

	SCARD_IO_REQUEST  ioSendPci = {1, sizeof(SCARD_IO_REQUEST)};
	SCARD_IO_REQUEST  ioRecvPci = {1, sizeof(SCARD_IO_REQUEST)};

	unsigned char     Cmd[128];
	unsigned int      uiCmdLg = 0;

	unsigned char     recvbuf[256];
	unsigned long     recvlen = sizeof(recvbuf);
	BYTE              SW1, SW2;
	BYTE IAS_PTEID_APPLET_AID[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x01, 0x02};
	BYTE GEMSAFE_APPLET_AID[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x00, 0x02};
	BYTE              cAppletID = sizeof(IAS_PTEID_APPLET_AID);
	
	int               i = 0;

	/***************/
	/* Select File */
	/***************/
	Cmd [0] = 0x00;
	Cmd [1] = 0xA4; /* SELECT COMMAND 00 A4 04 0C 07 */
	Cmd [2] = 0x04;
	if (Is_Gemsafe)
		Cmd [3] = 0x00;
	else 
		Cmd [3] = 0x0C;
	Cmd [4] = 0x07;
	if (Is_Gemsafe)
		memcpy(&Cmd[5], GEMSAFE_APPLET_AID, sizeof(GEMSAFE_APPLET_AID));
	else
		memcpy(&Cmd[5], IAS_PTEID_APPLET_AID, cAppletID);

	uiCmdLg = 5 + cAppletID;

	dwReturn = SCardTransmit(pCardData->hScard, 
		&ioSendPci, 
		Cmd, 
		uiCmdLg, 
		&ioRecvPci, 
		recvbuf, 
		&recvlen);
	SW1 = recvbuf[recvlen-2];
	SW2 = recvbuf[recvlen-1];
	PteidDelayAndRecover(pCardData, SW1, SW2, dwReturn);
	if ( dwReturn != SCARD_S_SUCCESS )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "SCardTransmit errorcode: [0x%02X]", dwReturn);
		CLEANUP(dwReturn);
    }

	if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
    
		if ( ( SW1 != 0x90 ) || ( SW2 != 0x00 ) )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Select Failed: [0x%02X][0x%02X]", SW1, SW2);
			CLEANUP(dwReturn);
		}
	}


cleanup:
	return (dwReturn);
}
#undef WHERE



/****************************************************************************************************/

/* CCID Features */
#define WHERE "CCIDfindFeature"
DWORD CCIDfindFeature(BYTE featureTag, BYTE* features, DWORD featuresLength) {
    DWORD idx = 0;
    int count;
    while (idx < featuresLength) {
        BYTE tag = features[idx];
        idx++;
        idx++;
        if (featureTag == tag) {
            DWORD feature = 0;
            for (count = 0; count < 3; count++) {
                feature |= features[idx] & 0xff;
                idx++;
                feature <<= 8;
            }
            feature |= features[idx] & 0xff;
            return feature;
        }
        idx += 4;
    }
    return 0;
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CCIDgetFeatures"
DWORD CCIDgetFeatures(PFEATURES pFeatures, SCARDHANDLE hCard) {
	BYTE pbRecvBuffer[200];
	DWORD dwRecvLength, dwReturn;
	pFeatures->VERIFY_PIN_START = 0;
	pFeatures->VERIFY_PIN_FINISH = 0;
	pFeatures->VERIFY_PIN_DIRECT = 0;
	pFeatures->MODIFY_PIN_START = 0;
	pFeatures->MODIFY_PIN_FINISH = 0;
	pFeatures->MODIFY_PIN_DIRECT = 0;
	pFeatures->GET_KEY_PRESSED = 0;
	pFeatures->ABORT = 0;

	dwReturn = SCardControl(hCard, 
		SCARD_CTL_CODE(3400),
		NULL,
		0,
		pbRecvBuffer,
		sizeof(pbRecvBuffer),
		&dwRecvLength);
	if ( SCARD_S_SUCCESS != dwReturn ) {
		LogTrace(LOGTYPE_ERROR, WHERE, "CCIDgetFeatures errorcode: [0x%02X]", dwReturn);
        CLEANUP(dwReturn);
	}
	pFeatures->VERIFY_PIN_START = CCIDfindFeature(FEATURE_VERIFY_PIN_START, pbRecvBuffer, dwRecvLength);
	pFeatures->VERIFY_PIN_FINISH = CCIDfindFeature(FEATURE_VERIFY_PIN_FINISH, pbRecvBuffer, dwRecvLength);
	pFeatures->VERIFY_PIN_DIRECT = CCIDfindFeature(FEATURE_VERIFY_PIN_DIRECT, pbRecvBuffer, dwRecvLength);
	pFeatures->MODIFY_PIN_START = CCIDfindFeature(FEATURE_MODIFY_PIN_START, pbRecvBuffer, dwRecvLength);
	pFeatures->MODIFY_PIN_FINISH = CCIDfindFeature(FEATURE_MODIFY_PIN_FINISH, pbRecvBuffer, dwRecvLength);
	pFeatures->MODIFY_PIN_DIRECT = CCIDfindFeature(FEATURE_MODIFY_PIN_DIRECT, pbRecvBuffer, dwRecvLength);
	pFeatures->GET_KEY_PRESSED = CCIDfindFeature(FEATURE_GET_KEY_PRESSED, pbRecvBuffer, dwRecvLength);
	pFeatures->ABORT = CCIDfindFeature(FEATURE_ABORT, pbRecvBuffer, dwRecvLength);
cleanup:
   return (dwReturn);
}

#undef WHERE

DWORD createVerifyCommand(PPIN_VERIFY_STRUCTURE pVerifyCommand) {
    pVerifyCommand->bTimeOut = 30;
    pVerifyCommand->bTimeOut2 = 30;
    pVerifyCommand->bmFormatString = 0x80 | 0x08 | 0x00 | 0x01;
    /*
     * bmFormatString. 
     *  01234567
     *  10001001
     *
     * bit 7: 1 = system units are bytes
     *
     * bit 6-3: 1 = PIN position in APDU command after Lc, so just after the
     * 0x20 | pinSize.
     * 
     * bit 2: 0 = left justify data
     * 
     * bit 1-0: 01 = BCD
     */

    pVerifyCommand->bmPINBlockString = 0x47;
    /*
     * bmPINBlockString
     * 
     * bit 7-4: 4 = PIN length
     * 
     * bit 3-0: 7 = PIN block size (7 times 0xff)
     */

    pVerifyCommand->bmPINLengthFormat = 0x04;
    /*
     * bmPINLengthFormat. weird... the values do not make any sense to me.
     * 
     * bit 7-5: 0 = RFU
     * 
     * bit 4: 0 = system units are bits
     * 
     * bit 3-0: 4 = PIN length position in APDU
     */


    pVerifyCommand->wPINMaxExtraDigit = BELPIC_MIN_USER_PIN_LEN << 8 | BELPIC_MAX_USER_PIN_LEN ;
    /*
     * First byte:  maximum PIN size in digit
     * 
     * Second byte: minimum PIN size in digit
     */

    pVerifyCommand->bEntryValidationCondition = 0x02;
    /*
     * 0x02 = validation key pressed. So the user must press the green
     * button on his pinpad.
     */

    pVerifyCommand->bNumberMessage = 0x01;
    /*
     * 0x01 = message with index in bMsgIndex
     */

    pVerifyCommand->wLangId = 0x1308;
    /*
     * We should support multiple languages for CCID devices with LCD screen
     */

    pVerifyCommand->bMsgIndex = 0x00;
    /*
     * 0x00 = PIN insertion prompt
     */

    pVerifyCommand->bTeoPrologue[0] = 0x00;
    pVerifyCommand->bTeoPrologue[1] = 0x00;
    pVerifyCommand->bTeoPrologue[2] = 0x00;
    /*
     * bTeoPrologue : only significant for T=1 protocol.
     */

    pVerifyCommand->abData[0] = 0x00; // CLA
    pVerifyCommand->abData[1] = 0x20; // INS Verify
    pVerifyCommand->abData[2] = 0x00; // P1
    pVerifyCommand->abData[3] = 0x01; // P2
    pVerifyCommand->abData[4] = 0x08; // Lc = 8 bytes in command data
    pVerifyCommand->abData[5] = 0x20 ; // 
    pVerifyCommand->abData[6] = BELPIC_PAD_CHAR; // Pin[1]
    pVerifyCommand->abData[7] = BELPIC_PAD_CHAR; // Pin[2]
    pVerifyCommand->abData[8] = BELPIC_PAD_CHAR; // Pin[3]
    pVerifyCommand->abData[9] = BELPIC_PAD_CHAR; // Pin[4]
    pVerifyCommand->abData[10] = BELPIC_PAD_CHAR; // Pin[5]
    pVerifyCommand->abData[11] = BELPIC_PAD_CHAR; // Pin[6]
    pVerifyCommand->abData[12] = BELPIC_PAD_CHAR; // Pin[7]

    pVerifyCommand->ulDataLength = 13;

    return 0;
}
/****************************************************************************************************/
