    
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


 unsigned int ACR83Pinpad::fillStructIAS(unsigned char* apdu, unsigned char ucPintype, int changePIN)
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
        printf("ppacr83-plugin: Get Authentication PIN\n");
            apdu[offset++] = 0x01;
            break;

        case EIDMW_PP_TYPE_SIGN:
            printf("ppacr83-plugin: Get Signature PIN\n");
            apdu[offset++] = 0x82;
            break;

        case EIDMW_PP_TYPE_ADDR:
            printf("ppacr83-plugin: Get Address PIN\n");
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

unsigned int ACR83Pinpad::fillStructGemsafe(unsigned char * apdu, unsigned char ucPintype, int changePin)
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
        printf("ppacr83-plugin: Get Authentication PIN\n");
        apdu[offset++] = 0x81;
        break;

    case EIDMW_PP_TYPE_SIGN:
        printf("ppacr83-plugin: Get Signature PIN\n");
        apdu[offset++] = 0x82;
        break;

    case EIDMW_PP_TYPE_ADDR:
        printf("ppacr83-plugin: Get Address PIN\n");
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

CByteArray ACR83Pinpad::PinCmd(tPinOperation operation,
		const tPin & pin, unsigned char ucPinType,
        const CByteArray & oAPDU, unsigned long & ulRemaining,
        bool bShowDlg)
{

    PP_VERIFY_CCID pin_verify;
    PP_CHANGE_CCID pin_change;
    void * pin_struct = NULL;
    int ioctl2;
    unsigned int apdu_length=0, length=0 ;

	printf("Debug: ACR83 - PinCmd() called with tPinOperation= %d\n", operation);
	CByteArray atr = m_poContext->m_oPCSC.GetATR(m_hCard);

        //For IAS cards we need to VerifyPIN before Modify
	if (operation == PIN_OP_VERIFY)
	{
            printf("ppacr83-plugin ==> Verify PIN\n");
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

		MWLOG(LEV_DEBUG, MOD_CAL, L"ACR83 Pinpad sending %d bytes in VERIFY structure",
				length);
	    CByteArray b1((const unsigned char *)pin_struct, (unsigned long)length);

	    return PinpadControl((unsigned long)ioctl2, b1, operation,
				ucPinType, pin.csLabel, bShowDlg);

        }

        if (operation == PIN_OP_CHANGE)
        {
            printf("ppacr83-plugin ==> Modify PIN\n");
	        ioctl2 = CM_IOCTL_MODIFY_PIN;
            fillModifyControlStruct(&pin_change);
            pin_struct = &pin_change;

	    if (!IsGemsafe(atr))
            {
                pin_change.ulDataLength = 0x0D; // The APDU only includes placeholders for the new PIN
                //Overriding the prompt options in order to not ask for the old pin again
                pin_change.bConfirmPIN = 0x01;
                pin_change.bNumberMessage = 0x02;
                length = fillStructIAS(pin_change.abData, ucPinType, 1);
            }
            else
            {
                pin_change.ulDataLength = 0x15; // The APDU includes placeholders for both PINs

                length = fillStructGemsafe(pin_change.abData, ucPinType, 1);
            }
	    
		MWLOG(LEV_DEBUG, MOD_CAL, L"ACR83 Pinpad sending %d bytes in MODIFY PIN structure",
				length);
		
	    CByteArray b2((const unsigned char *)pin_struct, (unsigned long)length);

		MWLOG(LEV_DEBUG, MOD_CAL, L"Struct: %S", b2.ToString(true, false, 0, -1).c_str());

	    return PinpadControl((unsigned long)ioctl2, b2, operation,
				ucPinType, pin.csLabel, bShowDlg);
        }
}

}

