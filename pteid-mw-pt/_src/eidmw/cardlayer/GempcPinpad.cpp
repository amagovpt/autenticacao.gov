    
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <winscard.h>

#include "GempcPinpad.h"
#include "Context.h"
#include "../common/ByteArray.h"
#include "../common/Log.h"

namespace eIDMW
{

class CContext;


unsigned int GemPcPinpad::fillStructIAS(unsigned char* apdu, unsigned char ucPintype, int changePIN)
{

	unsigned int offset = 0;
        unsigned int apdu_size = 12;

	apdu[offset++] = 0x00;
	/* CLA */ /*********************************/
	if (changePIN)
		apdu[offset++] = 0x24;
	else
		apdu[offset++] = 0x20;
	/* INS: VERIFY or CHANGE PIN */
	if (changePIN)
		apdu[offset++] = 0x01;
	else
		apdu[offset++] = 0x00;
	/* P1 */
	switch (ucPintype)
	{
		case EIDMW_PP_TYPE_AUTH:
			apdu[offset++] = 0x01;
			break;

		case EIDMW_PP_TYPE_SIGN:
			apdu[offset++] = 0x82;
			break;

		case EIDMW_PP_TYPE_ADDR:
			apdu[offset++] = 0x83;
			break;

	}

	apdu[offset++] = apdu_size - 4;
	/* Lc: Length of data section */
	
	while (offset <= apdu_size)
		apdu[offset++] = 0x2F; //Padding byte 

	/* APDU size */
	if (changePIN)
        return sizeof(PP_CHANGE_CCID) - PP_APDU_MAX_LEN + offset;
    else
        return sizeof(PP_VERIFY_CCID) - PP_APDU_MAX_LEN + offset;

}


void GemPcPinpad::fillVerifyControlStruct(PP_VERIFY_CCID * pin_verify)
{

	/* PC/SC v2.0.2 Part 10 PIN verification data structure */
	pin_verify -> bTimerOut = 0x1E; 
	pin_verify -> bTimerOut2 = 0x1E;   //30 seconds timeout
	pin_verify -> bmFormatString = 0x82;
	pin_verify -> bmPINBlockString = 0x00; //On Linux it can be 0x04!!!
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

} //sizeof() == 19

void GemPcPinpad::fillModifyControlStruct(PP_CHANGE_CCID * pin_change, int include_verify)
{

	pin_change -> bTimerOut = 0x1E; 
	pin_change -> bTimerOut2 = 0x1E;   //30 seconds timeout
	pin_change -> bmFormatString = 0x02;
	pin_change -> bmPINBlockString = 0x00;
	pin_change -> bmPINLengthFormat = 0x00;
	pin_change -> bInsertionOffsetOld = 0x00;
	pin_change -> bInsertionOffsetNew = include_verify == 1 ? 0x08 : 0x00; 
	(pin_change -> wPINMaxExtraDigit)[0] = 0x08; /* Min Max */
	pin_change -> wPINMaxExtraDigit[1] = 0x04; 
	pin_change -> bConfirmPIN = include_verify == 1 ? 0x03 : 0x01;
	pin_change -> bEntryValidationCondition = 0x02;
	/* validation key pressed */
	pin_change -> bNumberMessage = include_verify == 1 ? 0x03 : 0x02;
	(pin_change -> wLangId)[0] = 0x16; //0x0816
	pin_change -> wLangId[1] = 0x08; 
	pin_change -> bMsgIndex1 = 0x00;
	pin_change -> bMsgIndex2 = 0x00;
	pin_change -> bMsgIndex3 = 0x00;
	(pin_change -> bTeoPrologue)[0] = 0x00;
	pin_change -> bTeoPrologue[1] = 0x00;
	pin_change -> bTeoPrologue[2] = 0x00;

} //sizeof() == 24

unsigned int GemPcPinpad::fillStructGemsafe(unsigned char * apdu, unsigned char ucPintype, int changePin)
{
	unsigned int offset = 0;
        unsigned int apdu_size = 0;

	apdu[offset++] = 0x00;
	/* CLA */ /*********************************/
	if (changePin)
		apdu[offset++] = 0x24;
	else
		apdu[offset++] = 0x20;
	/* INS: VERIFY */
	apdu[offset++] = 0x00;
	/* P1 */
	switch (ucPintype)
	{
		case EIDMW_PP_TYPE_AUTH:
			apdu[offset++] = 0x81;
			break;

		case EIDMW_PP_TYPE_SIGN:
			apdu[offset++] = 0x82;
			break;

		case EIDMW_PP_TYPE_ADDR:
			apdu[offset++] = 0x83;
			break;

	}
	/* P2 */
	if (!changePin)	
		apdu_size = 12;
	else
		apdu_size = 20;

	apdu[offset++] = apdu_size - 4;
	/* Lc: Length of data section */
	

	while (offset <= apdu_size)
		apdu[offset++] = 0xFF; //Padding byte 

	if (changePin)
        return sizeof(PP_CHANGE_CCID) - PP_APDU_MAX_LEN + offset;
    else
        return sizeof(PP_VERIFY_CCID) - PP_APDU_MAX_LEN + offset;
}

DWORD GemPcPinpad::loadStrings(SCARDHANDLE hCard, unsigned char ucPinType)
{

	/*The Following Blob contains the Portuguese strings to show on the Pinpad Display:
		PIN Autent.?   
		Novo PIN?    
		Conf. novo PIN
		OK  
		PIN falhou      
		Tempo expirou   
		* tentiv. restam
		Introduza cartao
		Erro cartao 
		PIN bloqueado   
	*/
	char stringTable[] =
		"\xB2\xA0\x00\x4D\x4C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x4E\x6F\x76"
		"\x6F\x20\x50\x49\x4E\x3F\x20\x20\x20\x20\x20\x20\x20\x43\x6F\x6E\x66\x2E\x20\x6E\x6F\x76\x6F"
		"\x20\x50\x49\x4E\x20\x20\x50\x49\x4E\x20\x4F\x4B\x2E\x20\x20\x20\x20\x20\x20\x20\x20\x20\x50"
		"\x49\x4E\x20\x66\x61\x6C\x68\x6F\x75\x20\x20\x20\x20\x20\x20\x54\x65\x6D\x70\x6F\x20\x65\x78"
		"\x70\x69\x72\x6F\x75\x20\x20\x20\x2A\x20\x74\x65\x6E\x74\x69\x76\x2E\x20\x72\x65\x73\x74\x61\x6D"
		"\x49\x6E\x74\x72\x6F\x64\x75\x7A\x61\x20\x63\x61\x72\x74\x61\x6F\x45\x72\x72\x6F\x20\x63\x61\x72"
		"\x74\x61\x6F\x20\x20\x20\x20\x20\x50\x49\x4E\x20\x62\x6C\x6F\x71\x75\x65\x61\x64\x6F\x20\x20\x20";

	DWORD ioctl = 0x00312000;
	char recvBuf[200];
	DWORD rv = 0;
	unsigned int STRING_LEN = 16;
	DWORD recvlen = sizeof(recvBuf);

	switch (ucPinType)
	{
		case EIDMW_PP_TYPE_AUTH:
			memcpy(&stringTable[5], "PIN Autent.?    ", STRING_LEN); 
			break;
		case EIDMW_PP_TYPE_SIGN:
			memcpy(&stringTable[5], "PIN Assinatura? ", STRING_LEN); 
			break;

		case EIDMW_PP_TYPE_ADDR:
			memcpy(&stringTable[5], "PIN Morada?     ", STRING_LEN); 
			break;
	}
	
	rv = SCardControl(hCard, ioctl, stringTable, sizeof(stringTable)-1,
		recvBuf, recvlen, &recvlen);

	
	if ( rv == SCARD_S_SUCCESS )
	{
		printf("Strings Loaded successfully\n");
	}
	else
		printf("Error in LoadStrings: SCardControl() returned: %08x\n", 
				(unsigned int)rv);
	
	return rv;

}


CByteArray GemPcPinpad::PinCmd(tPinOperation operation,
		const tPin & pin, unsigned char ucPinType,
        const CByteArray & oAPDU, unsigned long & ulRemaining,
        bool bShowDlg)
{

	PP_VERIFY_CCID pin_verify;
	PP_CHANGE_CCID pin_change;
	void * pin_struct = NULL;
	DWORD rv;
	int ioctl2;
	unsigned int apdu_length=0, length=0 ;

	fprintf(stderr, "Debug: GemPcPinpad - PinCmd() called with tPinOperation= %d\n", operation);
	CByteArray atr = m_poContext->m_oPCSC.GetATR(m_hCard);

#ifdef WIN32 //Can't get this to work on Linux :(
	rv = loadStrings(m_hCard, ucPinType);
#endif	

		//For IAS cards we need to VerifyPIN before Modify
		if (operation == PIN_OP_VERIFY )
		{
			printf("ppgemplus-plugin ==> Verify PIN\n");

			fillVerifyControlStruct(&pin_verify);
			pin_struct = &pin_verify;

			pin_verify.ulDataLength = 0x0D;

			ioctl2 = CM_IOCTL_VERIFY_PIN;
			if (!IsGemsafe(atr))
			{
				length = fillStructIAS(pin_verify.abData, ucPinType, 0);

			}
			else
			{
				length = fillStructGemsafe(pin_verify.abData, ucPinType, 0);

			}

			MWLOG(LEV_DEBUG, MOD_CAL, L"GemPC Pinpad sending %d bytes in VERIFY structure",
				length);

			CByteArray b1((const unsigned char *)pin_struct, (unsigned long)length);

			return PinpadControl((unsigned long)ioctl2, b1, operation,
				ucPinType, pin.csLabel, bShowDlg);
		}
		else if (operation == PIN_OP_CHANGE)
		{
			ioctl2 = CM_IOCTL_MODIFY_PIN;
			printf("ppgemplus-plugin ==> Modify PIN\n");

			pin_struct = &pin_change;

			if (!IsGemsafe(atr))
			{
				fillModifyControlStruct(&pin_change, 0);
				pin_change.ulDataLength = 0x0D; // The APDU only includes placeholders for the new PIN
				length = fillStructIAS(pin_change.abData, ucPinType, 1);
			}
			else
			{
				fillModifyControlStruct(&pin_change, 1);
				pin_change.ulDataLength = 0x15; // The APDU includes placeholders for both PINs
				length = fillStructGemsafe(pin_change.abData, ucPinType, 1);
			}

			MWLOG(LEV_DEBUG, MOD_CAL, L"GemPc Pinpad sending %d bytes in MODIFY PIN structure",
				length);
			CByteArray b2((const unsigned char *)pin_struct, (unsigned long)length);

			return PinpadControl((unsigned long)ioctl2, b2, operation,
				ucPinType, pin.csLabel, bShowDlg);
		}
		


}



}
