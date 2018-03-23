    
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <winscard.h>
#include <string>

#include "GempcPinpad.h"
#include "Context.h"
#include "../common/ByteArray.h"
#include "../common/Log.h"

namespace eIDMW
{

class CContext;

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

}

void GemPcPinpad::fillModifyControlStruct(PP_CHANGE_CCID * pin_change, bool include_verify)
{

	pin_change -> bTimerOut = 0x1E;                                         
	pin_change -> bTimerOut2 = 0x1E;   //30 seconds timeout
	pin_change -> bmFormatString = 0x02;
	pin_change -> bmPINBlockString = 0x00;
	pin_change -> bmPINLengthFormat = 0x00;
	pin_change -> bInsertionOffsetOld = 0x00;
	pin_change -> bInsertionOffsetNew = include_verify ? 0x08 : 0x00; 
	(pin_change -> wPINMaxExtraDigit)[0] = 0x08; /* Min Max */
	pin_change -> wPINMaxExtraDigit[1] = 0x04; 
	pin_change -> bConfirmPIN = include_verify ? 0x03 : 0x01;
	pin_change -> bEntryValidationCondition = 0x02;
	/* validation key pressed */
	pin_change -> bNumberMessage = include_verify ? 0x03 : 0x02;
	(pin_change -> wLangId)[0] = 0x16; //0x0816
	pin_change -> wLangId[1] = 0x08; 
	pin_change -> bMsgIndex1 = 0x00;
	pin_change -> bMsgIndex2 = 0x00;
	pin_change -> bMsgIndex3 = 0x00;
	(pin_change -> bTeoPrologue)[0] = 0x00;
	pin_change -> bTeoPrologue[1] = 0x00;
	pin_change -> bTeoPrologue[2] = 0x00;

}

//This function is used for the special case of unlockPIN without introducing a PUK (included in the supplied APDU)
void GemPcPinpad::fillUnlockControlStruct(PP_CHANGE_CCID * pin_change)
{
	pin_change->bTimerOut = 0x1E;
	pin_change->bTimerOut2 = 0x1E;   //30 seconds timeout
	pin_change->bmFormatString = 0x02;
	pin_change->bmPINBlockString = 0x00;
	pin_change->bmPINLengthFormat = 0x00;
	pin_change->bInsertionOffsetOld = 0x00;
	pin_change->bInsertionOffsetNew = 0x08;
	(pin_change->wPINMaxExtraDigit)[0] = 0x08; /* Min Max */
	pin_change->wPINMaxExtraDigit[1] = 0x04;
	pin_change->bConfirmPIN = 0x01;
	pin_change->bEntryValidationCondition = 0x02;
	/* validation key pressed */
	pin_change->bNumberMessage = 0x02; //The messages should be (New PIN, Confirm PIN)
	(pin_change->wLangId)[0] = 0x16; //0x0816
	pin_change->wLangId[1] = 0x08;
	pin_change->bMsgIndex1 = 0x00;
	pin_change->bMsgIndex2 = 0x00;
	pin_change->bMsgIndex3 = 0x00;
	(pin_change->bTeoPrologue)[0] = 0x00;
	pin_change->bTeoPrologue[1] = 0x00;
	pin_change->bTeoPrologue[2] = 0x00;
}


DWORD GemPcPinpad::loadStrings(SCARDHANDLE hCard, unsigned char ucPinType, tPinOperation operation)
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
	std::string pin_string;
	unsigned int STRING_LEN = 16;
	DWORD recvlen = sizeof(recvBuf);

	if (operation == PIN_OP_RESET || operation == PIN_OP_RESET_NO_PUK)
	{
		pin_string = "PUK ";
	}
	else
	{
		pin_string = "PIN ";
	}

	switch (ucPinType)
	{
		case EIDMW_PP_TYPE_AUTH:
			pin_string += "Autent.?    ";
			break;
		case EIDMW_PP_TYPE_SIGN:
			pin_string += "Assinatura? ";
			break;
		case EIDMW_PP_TYPE_ADDR:
			pin_string += "Morada?     "; 
			break;
	}

	memcpy(&stringTable[5], pin_string.c_str(), STRING_LEN); 
	
	rv = SCardControl(hCard, ioctl, stringTable, sizeof(stringTable)-1,
		recvBuf, recvlen, &recvlen);

	
	if ( rv == SCARD_S_SUCCESS )
	{
		MWLOG(LEV_DEBUG, MOD_CAL, "GemPcPinpad: Strings Loaded successfully");
	}
	else
	{
		MWLOG(LEV_ERROR, MOD_CAL, "Error in GemPcPinpad::LoadStrings: SCardControl() returned: %08x\n",
			(unsigned int)rv);
	}
	
	return rv;
}


CByteArray GemPcPinpad::PinCmd(tPinOperation operation,
		const tPin & pin, unsigned char ucPinType,
        const CByteArray & oAPDU, unsigned long & ulRemaining, void *wndGeometry )
{

	PP_VERIFY_CCID pin_verify;
	PP_CHANGE_CCID pin_change;
	void * pin_struct = NULL;
	DWORD rv;
	int ioctl2;
	unsigned int apdu_length=0, length=0 ;

	CByteArray atr = m_poContext->m_oPCSC.GetATR(m_hCard);

#ifdef WIN32 //Can't get this to work on Linux :(
	rv = loadStrings(m_hCard, ucPinType, operation);
#endif	

	//For IAS cards we need to VerifyPIN before Modify
	if (operation == PIN_OP_VERIFY)
	{
		ioctl2 = CM_IOCTL_VERIFY_PIN;
		pin_struct = &pin_verify;

		fillVerifyControlStruct(&pin_verify);
		pin_verify.ulDataLength = oAPDU.Size();
		memcpy(pin_verify.abData,oAPDU.GetBytes(),oAPDU.Size());
		length = sizeof(PP_VERIFY_CCID) - PP_APDU_MAX_LEN + oAPDU.Size();

		MWLOG(LEV_DEBUG, MOD_CAL, L"[Verify Pin] GemPC Pinpad sending %d bytes in VERIFY structure",
			length);

		CByteArray b1((const unsigned char *)pin_struct, (unsigned long)length);

		return PinpadControl((unsigned long)ioctl2, b1, operation,
                            ucPinType, pin.csLabel, wndGeometry );
	}
	else if (operation == PIN_OP_CHANGE)
	{
		ioctl2 = CM_IOCTL_MODIFY_PIN;
		pin_struct = &pin_change;

		fillModifyControlStruct(&pin_change, IsGemsafe(atr));
		pin_change.ulDataLength = oAPDU.Size();
		memcpy(pin_change.abData, oAPDU.GetBytes(), oAPDU.Size());
		length = sizeof(PP_CHANGE_CCID) - PP_APDU_MAX_LEN + oAPDU.Size();

		MWLOG(LEV_DEBUG, MOD_CAL, L"[Modify Pin] GemPc Pinpad sending %d bytes in MODIFY PIN structure", length);
		CByteArray b2((const unsigned char *)pin_struct, (unsigned long)length);

		return PinpadControl((unsigned long)ioctl2, b2, operation,
                            ucPinType, pin.csLabel, wndGeometry );
	}
	else if (operation == PIN_OP_RESET || operation == PIN_OP_RESET_NO_PUK)
	{
		ioctl2 = CM_IOCTL_MODIFY_PIN;
		pin_struct = &pin_change;
		bool include_verify = IsGemsafe(atr);
		if (operation == PIN_OP_RESET)
			fillModifyControlStruct(&pin_change, include_verify);
		else
			fillUnlockControlStruct(&pin_change);

		pin_change.ulDataLength = oAPDU.Size();
		memcpy(pin_change.abData,oAPDU.GetBytes(),oAPDU.Size());
		length = sizeof(PP_CHANGE_CCID) - PP_APDU_MAX_LEN + oAPDU.Size();

		MWLOG(LEV_DEBUG, MOD_CAL, L"[Reset Pin] GemPc Pinpad sending %d bytes in MODIFY PIN structure",	length);
		CByteArray b2((const unsigned char *)pin_struct, (unsigned long)length);

		return PinpadControl((unsigned long)ioctl2, b2, operation,
                            ucPinType, pin.csLabel, wndGeometry );
	}
}



}
