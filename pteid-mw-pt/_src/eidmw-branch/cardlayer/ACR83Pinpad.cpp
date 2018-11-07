    
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <winscard.h>

#include "ACR83Pinpad.h"
#include "Context.h"
#include "../common/ByteArray.h"
#include "../common/Log.h"

namespace eIDMW
{


class CContext;

void ACR83Pinpad::fillVerifyControlStruct(PP_VERIFY_CCID * pin_verify)
{

    /* PC/SC v2.0.2 Part 10 PIN verification data structure */
    pin_verify -> bTimerOut = 0x00;
    pin_verify -> bTimerOut2 = 0x00;   //30 seconds timeout
    pin_verify -> bmFormatString = 0x82;
    pin_verify -> bmPINBlockString = 0x08;
    pin_verify -> bmPINLengthFormat = 0x00;
    (pin_verify -> wPINMaxExtraDigit)[0] = 0x08; /* Min Max */
    pin_verify -> wPINMaxExtraDigit[1] = 0x04;
    pin_verify -> bEntryValidationCondition = 0x02;
    /* validation key pressed */
    pin_verify -> bNumberMessage = 0x01;
    (pin_verify -> wLangId)[0] = 0x09; //0x0816
    pin_verify -> wLangId[1] = 0x04;
    pin_verify -> bMsgIndex = 0x00;
    (pin_verify -> bTeoPrologue)[0] = 0x00;
    pin_verify -> bTeoPrologue[1] = 0x00;
    pin_verify -> bTeoPrologue[2] = 0x00;

}

void ACR83Pinpad::fillModifyControlStruct(PP_CHANGE_CCID * pin_change)
{

    pin_change -> bTimerOut = 0;
    pin_change -> bTimerOut2 = 0x00;   //30 seconds timeout
    pin_change -> bmFormatString = 0x02;
    pin_change -> bmPINBlockString = 0x00;
    pin_change -> bmPINLengthFormat = 0x00;
    pin_change -> bInsertionOffsetOld = 0x00;
    pin_change -> bInsertionOffsetNew = 0x08;
    (pin_change -> wPINMaxExtraDigit)[0] = 0x08; /* Min Max */
    pin_change -> wPINMaxExtraDigit[1] = 0x04;
    pin_change -> bConfirmPIN = 0x01;
    pin_change -> bEntryValidationCondition = 0x02;
    /* validation key pressed */
    pin_change -> bNumberMessage = 0x01;
    (pin_change -> wLangId)[0] = 0x09; //0x0816
    pin_change -> wLangId[1] = 0x04;
    pin_change -> bMsgIndex1 = 0x00;
    pin_change -> bMsgIndex2 = 0x01;
    pin_change -> bMsgIndex3 = 0x02;
    (pin_change -> bTeoPrologue)[0] = 0x00;
    pin_change -> bTeoPrologue[1] = 0x00;
    pin_change -> bTeoPrologue[2] = 0x00;

} //sizeof() == 24


CByteArray ACR83Pinpad::PinCmd(tPinOperation operation,
		const tPin & pin, unsigned char ucPinType,
        const CByteArray & oAPDU, unsigned long & ulRemaining, void *wndGeometry)
{

    PP_VERIFY_CCID pin_verify;
    PP_CHANGE_CCID pin_change;
    void * pin_struct = NULL;
    int ioctl2;
    unsigned int apdu_length=0, length=0 ;

	CByteArray atr = m_poContext->m_oPCSC.GetATR(m_hCard);

        //For IAS cards we need to VerifyPIN before Modify
	if (operation == PIN_OP_VERIFY)
	{

		ioctl2 = CM_IOCTL_VERIFY_PIN;
		pin_struct = &pin_verify;

		fillVerifyControlStruct(&pin_verify);
		pin_verify.ulDataLength = oAPDU.Size();
		memcpy(pin_verify.abData,oAPDU.GetBytes(),oAPDU.Size());
		length = sizeof(PP_VERIFY_CCID) - PP_APDU_MAX_LEN + oAPDU.Size();

		MWLOG(LEV_DEBUG, MOD_CAL, L"[Verify Pin] ACR83 Pinpad sending %d bytes in VERIFY structure",
				length);
		CByteArray b1((const unsigned char *)pin_struct, (unsigned long)length);

		return PinpadControl((unsigned long)ioctl2, b1, operation,
                                ucPinType, pin.csLabel, wndGeometry);

	}
	else if (operation == PIN_OP_CHANGE)
	{

		ioctl2 = CM_IOCTL_MODIFY_PIN;
		pin_struct = &pin_change;

		fillModifyControlStruct(&pin_change);
		if (!IsGemsafe(atr))
		{
			//Overriding the prompt options in order to not ask for the old pin again
			pin_change.bConfirmPIN = 0x01;
			pin_change.bNumberMessage = 0x02;
		}
		pin_change.ulDataLength = oAPDU.Size();
		memcpy(pin_change.abData,oAPDU.GetBytes(),oAPDU.Size());
		length = sizeof(PP_CHANGE_CCID) - PP_APDU_MAX_LEN + oAPDU.Size();

		MWLOG(LEV_DEBUG, MOD_CAL, L"[Modify Pin] ACR83 Pinpad sending %d bytes in MODIFY PIN structure",
				length);

		CByteArray b2((const unsigned char *)pin_struct, (unsigned long)length);

		MWLOG(LEV_DEBUG, MOD_CAL, L"Struct: %S", b2.ToString(true, false, 0, -1).c_str());

		return PinpadControl((unsigned long)ioctl2, b2, operation,
                            ucPinType, pin.csLabel, wndGeometry);
	}
	else if (operation == PIN_OP_RESET)
	{
		ioctl2 = CM_IOCTL_MODIFY_PIN;
		pin_struct = &pin_change;

		fillModifyControlStruct(&pin_change);
		if (!IsGemsafe(atr))
		{
			//Overriding the prompt options in order to not ask for the old pin again
			pin_change.bConfirmPIN = 0x01;
			pin_change.bNumberMessage = 0x02;
		}

		pin_change.ulDataLength = oAPDU.Size();
		memcpy(pin_change.abData,oAPDU.GetBytes(),oAPDU.Size());
		length = sizeof(PP_CHANGE_CCID) - PP_APDU_MAX_LEN + oAPDU.Size();

		MWLOG(LEV_DEBUG, MOD_CAL, L"[Reset Pin] ACR83 Pinpad sending %d bytes in MODIFY PIN structure",
			length);

		CByteArray b2((const unsigned char *)pin_struct, (unsigned long)length);

		return PinpadControl((unsigned long)ioctl2, b2, operation,
                            ucPinType, pin.csLabel, wndGeometry);
	}
}

}

