#include "pinpad2-private.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    printf ("ACR83U init\n");

    if (hCard == 0 || hCtx == 0)
        return SCARD_E_INVALID_PARAMETER;

    if (strstr(csReader, "ACR83U") == 0)
        return SCARD_E_INVALID_PARAMETER;


    return SCARD_S_SUCCESS;

}


void printHex(char *array, unsigned int len)
{
    unsigned int i = 0;
    char *p = array;

    if (array == NULL )
    {
        printf("printfHex: NULL Array\n");
        return;
    }

    while(i != len)
    {
        if (( i % 16 ) == 0)
        {
            printf("\n");
        }

        printf ("%02X ", *p++);
        i++;
    }
    printf("\n\n");

}

DLL_LOCAL BOOL IsGemsafe(BYTE *atr, DWORD atrLen)
{

    char * GemsafeATRs[] =
    {
        "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xc8\x83\x00\x90\x00",
        "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xa9\x83\x00\x90\x00",
        "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xc8\x83\x00",
        "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xB0\x83\x11\xC0\xA9\x83\x00\x90\x00",
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

DLL_LOCAL void fillVerifyControlStruct(EIDMW_PP_VERIFY_CCID * pin_verify)
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

DLL_LOCAL void fillModifyControlStruct(EIDMW_PP_CHANGE_CCID * pin_change)
{

    pin_change -> bTimerOut = 0;
    pin_change -> bTimerOut2 = 0x00;   //30 seconds timeout
    pin_change -> bmFormatString = 0x00;
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
    pin_change -> bMsgIndex1 = 0x01;
    pin_change -> bMsgIndex2 = 0x00;
    pin_change -> bMsgIndex3 = 0x00;
    (pin_change -> bTeoPrologue)[0] = 0x00;
    pin_change -> bTeoPrologue[1] = 0x00;
    pin_change -> bTeoPrologue[2] = 0x00;

} //sizeof() == 24


DLL_LOCAL unsigned int fillStructIAS(unsigned char* apdu, unsigned char ucPintype, int changePIN)
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
        return sizeof(EIDMW_PP_CHANGE_CCID) + offset -1 ;
    else
        return sizeof(EIDMW_PP_VERIFY_CCID) + offset -1;

}

DLL_LOCAL unsigned int fillStructGemsafe(unsigned char * apdu, unsigned char ucPintype, int changePin)
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

    /* APDU size */
    if (changePin)
        return sizeof(EIDMW_PP_CHANGE_CCID) + offset -1 ;
    else
        return sizeof(EIDMW_PP_VERIFY_CCID) + offset -1;
}

DLL_LOCAL DWORD sendControl(SCARDHANDLE hCard, int ioctl,
        void * pin_struct, unsigned int length, unsigned char * pucRecvbuf,
        DWORD dwRecvlen, DWORD *pdwRecvlen)
{
    DWORD rv = 0;
    int i = 0;
    printf ("Send buffer: \n");
    printHex(pin_struct, length);

    rv = SCardControl(hCard, ioctl, pin_struct,
            length, pucRecvbuf, dwRecvlen, pdwRecvlen);

    printf ("\tCard response:");
    for (i=0; i < *pdwRecvlen; i++)
        printf(" %02X", pucRecvbuf[i]);
    printf("\n");

    if ( rv == SCARD_S_SUCCESS )
    {
        printf("Control Message sent successful\n");

    }
    else
          printf("\tppacr83-plugin =>  %04x\n",
                (unsigned int)rv);
    return rv;
}

/*
 * PinPad Control Commands Implementation for ACS ACR83U Pinpad
 * */

EIDMW_PP_API long EIDMW_PP2_Command(
    SCARDHANDLE hCard, int ioctl,
    const unsigned char *pucSendbuf, DWORD dwSendlen,
    unsigned char *pucRecvbuf, DWORD dwRrecvlen, DWORD *pdwRrecvlen,
    unsigned char ucPintype, unsigned char ucOperation,
    unsigned long ulRfu, void *pRfu)
{
    printf ("ACR83U Command\n");
    EIDMW_PP_VERIFY_CCID pin_verify;
    EIDMW_PP_CHANGE_CCID pin_change;
    void * pin_struct = NULL;
    BYTE pbAtr[64];
    DWORD dwReaderLen = 0, dwState, dwProt, dwAtrLen;
    DWORD rv;
    int ioctl2;
    unsigned int i, apdu_length=0, length=0 ;

    dwAtrLen = sizeof(pbAtr);

    if (ioctl == CM_IOCTL_GET_FEATURE_REQUEST)
    {
        printf("ppacr83-plugin ==> Get Feature Request\n");
        rv = SCardControl(hCard, CM_IOCTL_GET_FEATURE_REQUEST, NULL, 0,
                          pucRecvbuf, dwRrecvlen, pdwRrecvlen);

        return rv;
    }
    else
    {
        //Check the ATR to get Card Type
        rv = SCardStatus(hCard, NULL, &dwReaderLen, &dwState, &dwProt,
                         pbAtr, &dwAtrLen);

        printf ("\tATR is: ");
        for (i=0; i < dwAtrLen; i++)
            printf (" %02X", pbAtr[i]);
        printf("\n");

        //For IAS cards we need to VerifyPIN before Modify
        if (ioctl == CM_IOCTL_VERIFY_PIN || !IsGemsafe(pbAtr, dwAtrLen))
        {
            printf("ppacr83-plugin ==> Verify PIN\n");
            fillVerifyControlStruct(&pin_verify);
            pin_struct = &pin_verify;

            pin_verify.ulDataLength = 0x0D;

            ioctl2 = CM_IOCTL_VERIFY_PIN;
            if (!IsGemsafe(pbAtr, dwAtrLen))
            {
                printf("ias\n");
                length = fillStructIAS(pin_verify.abData, ucPintype, 0);
            }
            else
            {
                printf("gemsafe\n");
                length = fillStructGemsafe(pin_verify.abData, ucPintype, 0);

            }
            rv = sendControl(hCard, ioctl2, pin_struct,length, pucRecvbuf, dwRrecvlen, pdwRrecvlen);

            if (ioctl == CM_IOCTL_VERIFY_PIN)
                return rv;
        }

        if (ioctl == CM_IOCTL_MODIFY_PIN)
        {
            printf("ppacr83-plugin ==> Modify PIN\n");
            fillModifyControlStruct(&pin_change);
            pin_struct = &pin_change;

            if (!IsGemsafe(pbAtr, dwAtrLen))
            {
                pin_change.ulDataLength = 0x0D; // The APDU only includes placeholders for the new PIN
                //Overriding the prompt options in order to not ask for the old pin again
                pin_change.bConfirmPIN = 0x01;
                pin_change.bNumberMessage = 0x02;
                length = fillStructIAS(pin_change.abData, ucPintype, 1);
            }
            else
            {
                pin_change.ulDataLength = 0x15; // The APDU only includes placeholders for both PINs

                length = fillStructGemsafe(pin_change.abData, ucPintype, 1);
            }
            return sendControl(hCard, ioctl, pin_struct,
                               length, pucRecvbuf, dwRrecvlen, pdwRrecvlen);
        }
    }
    return rv;
}
