#include "pinpad2.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* PCSC missing types */
#include <reader.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


/*TODO: Remove Debug Printfs */

EIDMW_PP_API long EIDMW_PP2_Init(
	unsigned char ucMinorVersion,
	SCARDCONTEXT hCtx, SCARDHANDLE hCard, const char *csReader,
	unsigned long ulLanguage,
	tGuiInfo *pGuiInfo,
	unsigned long ulRfu, void *pRfu)
{

	if (hCard == 0 || hCtx == 0)
		return SCARD_E_INVALID_PARAMETER;

	if (strcasestr(csReader, "GemPC Pinpad") == 0)
		return SCARD_E_INVALID_PARAMETER;
	/*Should we leave the default strings ?? */		

	/* Do some initialization maybe... */
	printf("\n");

	return SCARD_S_SUCCESS;


}


DLL_LOCAL BOOL IsGemsafe(BYTE *atr, DWORD atrLen)
{

	char * GemsafeATRs[] = 
	{
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xc8\x83\x00\x90\x00",
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xa9\x83\x00\x90\x00",
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xc8\x83\x00",
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xa9\x83\x00" 

	};
	int i = 0;
	while (i < 4)
	{
	if (memcmp(GemsafeATRs[i], atr, atrLen) == 0)
		return TRUE;
	   i++;
	}

	return FALSE;
}


DLL_LOCAL unsigned int fillStructIAS(EIDMW_PP_VERIFY_CCID * pin_verify, unsigned char ucPintype)
{

	int offset = 0;

	pin_verify -> abData[offset++] = 0x00;
	/* CLA */ /*********************************/
	pin_verify -> abData[offset++] = 0x20;
	/* INS: VERIFY */
	pin_verify -> abData[offset++] = 0x00;
	/* P1 */
	switch (ucPintype)
	{
		case EIDMW_PP_TYPE_AUTH:
		printf("ppgemplus-plugin: Get Authentication PIN\n");
			pin_verify -> abData[offset++] = 0x01;
			break;

		case EIDMW_PP_TYPE_SIGN:
			printf("ppgemplus-plugin: Get Signature PIN\n");
			pin_verify -> abData[offset++] = 0x82;
			break;

		case EIDMW_PP_TYPE_ADDR:
			printf("ppgemplus-plugin: Get Address PIN\n");
			pin_verify -> abData[offset++] = 0x83;
			break;

	}
	/* P2 */
	pin_verify -> abData[offset++] = 0x08;
	/* Lc: 8 data bytes */
	while (offset <= 12) 
		pin_verify -> abData[offset++] = 0x2F; //Padding byte 

	pin_verify -> ulDataLength = offset;
	/* APDU size */

	return sizeof(EIDMW_PP_VERIFY_CCID) + offset -1;

}


DLL_LOCAL void fillControlStruct(EIDMW_PP_VERIFY_CCID * pin_verify)
{

	/* PC/SC v2.0.2 Part 10 PIN verification data structure */
	pin_verify -> bTimerOut = 0; 
	pin_verify -> bTimerOut2 = 0x1E;   //30 seconds timeout
	pin_verify -> bmFormatString = 0x82;
	pin_verify -> bmPINBlockString = 0x04;
	pin_verify -> bmPINLengthFormat = 0x00;
	(pin_verify -> wPINMaxExtraDigit)[0] = 0x08; /* Min Max */
	pin_verify -> wPINMaxExtraDigit[1] = 0x04; 
	pin_verify -> bEntryValidationCondition = 0x02;
	/* validation key pressed */
	pin_verify -> bNumberMessage = 0x01;
	(pin_verify -> wLangId)[0] = 0x16; //0x0816
	pin_verify -> wLangId[1] = 0x08; 
	pin_verify -> bMsgIndex = 0x00;
	(pin_verify -> bTeoPrologue)[0] = 0x00;
	pin_verify -> bTeoPrologue[1] = 0x00;
	pin_verify -> bTeoPrologue[2] = 0x00;

}

DLL_LOCAL unsigned int fillStructGemsafe(EIDMW_PP_VERIFY_CCID * pin_verify, unsigned char ucPintype)
{
	unsigned int offset = 0;
	pin_verify -> abData[offset++] = 0x00;
	/* CLA */ /*********************************/
	pin_verify -> abData[offset++] = 0x20;
	/* INS: VERIFY */
	pin_verify -> abData[offset++] = 0x00;
	/* P1 */
	switch (ucPintype)
	{
		case EIDMW_PP_TYPE_AUTH:
		printf("ppgemplus-plugin: Get Authentication PIN\n");
			pin_verify -> abData[offset++] = 0x81;
			break;

		case EIDMW_PP_TYPE_SIGN:
			printf("ppgemplus-plugin: Get Signature PIN\n");
			pin_verify -> abData[offset++] = 0x82;
			break;

		case EIDMW_PP_TYPE_ADDR:
			printf("ppgemplus-plugin: Get Address PIN\n");
			pin_verify -> abData[offset++] = 0x83;
			break;

	}
	/* P2 */
	pin_verify -> abData[offset++] = 0x08;
	/* Lc: 8 data bytes */

	while (offset <= 12) 
		pin_verify -> abData[offset++] = 0xFF; //Padding byte 

	pin_verify -> ulDataLength = offset;
	/* APDU size */

	return sizeof(EIDMW_PP_VERIFY_CCID) + offset -1;
}

/*
 * PinPad Control Commands Implementation for GemPlus GempPC Pinpad
 * */

EIDMW_PP_API long EIDMW_PP2_Command(
	SCARDHANDLE hCard, int ioctl,
	const unsigned char *pucSendbuf, DWORD dwSendlen,
	unsigned char *pucRecvbuf, DWORD dwRrecvlen, DWORD *pdwRrecvlen,
	unsigned char ucPintype, unsigned char ucOperation,
	unsigned long ulRfu, void *pRfu) 
{
	DWORD rv = 0;
	EIDMW_PP_VERIFY_CCID pin_verify;
	BYTE pbAtr[64];
	DWORD dwReaderLen, dwState, dwProt, dwAtrLen;
	dwAtrLen = sizeof(pbAtr);
	dwReaderLen = 0;
	unsigned int i, length;


	if (ioctl == CM_IOCTL_GET_FEATURE_REQUEST)
	{
		printf("ppgemplus-plugin ==> Get Feature Request\n");
		rv = SCardControl(hCard, CM_IOCTL_GET_FEATURE_REQUEST, NULL, 0,
				pucRecvbuf, dwRrecvlen, pdwRrecvlen);

		return rv;
	}
	else
	{
		//Assume verify PIN for now, we would need to implement change PIN also
		printf("ppgemplus-plugin ==> Verify PIN\n");

		//Check the ATR to get Card Type
		rv = SCardStatus(hCard, NULL, &dwReaderLen, &dwState, &dwProt,
				pbAtr, &dwAtrLen);

		printf ("\tATR is: ");
		for (i=0; i < dwAtrLen; i++)
			printf(" %02X", pbAtr[i]);
		printf("\n");

		fillControlStruct(&pin_verify);		

		if (!IsGemsafe(pbAtr, dwAtrLen))
			length = fillStructIAS(&pin_verify, ucPintype);
		else
			length = fillStructGemsafe(&pin_verify, ucPintype);


		rv = SCardControl(hCard, ioctl, &pin_verify,
				length, pucRecvbuf, dwRrecvlen, pdwRrecvlen);

		if ( rv == SCARD_S_SUCCESS )
		{
			printf("\tCard response:");
			for (i=0; i< *pdwRrecvlen; i++)
				printf(" %02X", pucRecvbuf[i]);
			printf("\n");

		}
		else
			printf("\tppgemplus-plugin => Error on VerifyPIN. Returning error code: %u\n", 
					(unsigned int)rv);

	}

	return rv;


}



