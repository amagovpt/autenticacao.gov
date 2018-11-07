/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
/*
 * This API should be implemented by libraries (also called
 * plugins) that want to support a pinpad reader for use
 * with the eID middleware.
 *
 * The middleware has support for CCID readers; see the PCSC V2
 * part 10 standard and the USB CCID specifications.
 *
 * However these standards don't specify how to display messages
 * on the reader's display. Because the middleware will show a
 * some explanation on the PC whenever a pinpad operation needs
 * to be done, this restriction is not important for pinpad
 * readers without display.
 *
 * But for readers with display, it is important that the correct
 * message be displayed, as to avoid confusion between the different
 * PINs on the eID card and the fact that the "PIN change command"
 * is used for both PIN change and PIN unblock.
 *
 * In this case, as well as for non-CCID pinpad readers or readers
 * that offer extra functionality, a library (a DLL on Windows, an
 * .so file on Linux and a .dylib file on Mac) can be made that
 * exports the 2 functions below.
 * The middleware will first try to load this library and check if
 * it can be used by mean of the EIDMW_PP_Init() function. If so, then
 * all pinpad functionality will be requested  through this library.
 *
 * To allow the middleware to find the library, it must be placed in
 * the following directory:
 *   - On Linux and Mac OS X: /usr/local/lib/pteidpp
 *   - On Windows: <systemdir>\pteidpp
 *     (<systemdir> is the Windows system folder, usually this is
 *      C:\WINDOWS\system32 on WinXP and Vista)
 * And the name of the library should start with "pteidpp2" on
 * Windows, and with "libpteidpp2" on Linux and Mac.
 *
 * Future, incompatible versions of this library will start with
 * "pteidpp3" etc.
 *
 * Remark: for the middleware for other country, "pteid" should be
 * replace by e.g. "pteid", "ileid", ...
 */

#ifndef __PINPAD2_H__
#define __PINPAD2_H__

#ifdef  __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(WIN32)
#ifdef EIDMW_PP_IMPORT
#define EIDMW_PP_API __declspec(dllimport)
#else
#define EIDMW_PP_API __declspec(dllexport)
#endif
#else
#define EIDMW_PP_API
#endif

#include <winscard.h>

#ifndef WIN32
#include "wintypes.h"
#endif

#define PTEID_MINOR_VERSION       0

#define EIDMW_PP_TYPE_AUTH         0x01   /* The Authentication PIN/PUK */
#define EIDMW_PP_TYPE_SIGN         0x02    /* The Signature PIN/PUK */
#define EIDMW_PP_TYPE_ADDR         0x03    /* The Address PIN/PUK */
#define EIDMW_PP_TYPE_ACTIV        0x04    /* The Activation PIN */
#define EIDMW_PP_TYPE_UNKNOWN      0xFF    /* Unkonwn PIN/PUK */

#define EIDMW_PP_OP_VERIFY                    0x01    /* PIN verification */
#define EIDMW_PP_OP_CHANGE                    0x02    /* PIN change */
#define EIDMW_PP_OP_UNBLOCK_NO_CHANGE         0x03    /* PIN unblock without PIN change */
#define EIDMW_PP_OP_UNBLOCK_CHANGE            0x04    /* PIn unblock plus PIN change */
#define EIDMW_PP_OP_UNBLOCK_MERGE_NO_CHANGE   0x05    /* PIN unblock using PUK merge, without PIN change*/
#define EIDMW_PP_OP_UNBLOCK_MERGE_CHANGE      0x06    /* PIN unblock using PUK merge, plus PIN change */

#define DLG_INFO_CHARS       2000

#define PP_LANG_EN   0x0409
#define PP_LANG_NL   0x0813
#define PP_LANG_FR   0x080c
#define PP_LANG_DE   0x0407


/**
 * This function is called for a "Get Feature Request" with control = CM_IOCTL_GET_FEATURE_REQUEST
 * (right after a successfull call to EIDMW_PP_Init())
 * and when a pinpad operation (verify, change, unblock) is needed.
 *
 * The following ioctl codes are recognized and used by the middleware
 *  - For PIN verification:
 *       FEATURE_VERIFY_PIN_DIRECT
 *       FEATURE_VERIFY_PIN_START and FEATURE_VERIFY_PIN_FINISH
 *  - For PIN change and unblock:
 *       FEATURE_MODIFY_PIN_DIRECT
 *       FEATURE_MODIFY_PIN_START and FEATURE_MODIFY_PIN_FINISH
 *
 * - The first 7 parameters are identical to the ones given in an SCardControl()
 *     command, as specified in part 10 of the PCSC standard.
 *   In case of a standard CCID pinpad reader without display, this function could
 *     directly 'forward' these parameters to an SCardControl() function.
 * - ucPintype: one of EIDMW_PP_TYPE_AUTH, ..., EIDMW_PP_TYPE_ACTIV; is ignored for
 *     a "Get Feature Request"
 * - ucOperation: one of EIDMW_PP_OP_VERIFY, ..., EIDMW_PP_OP_UNBLOCK_MERGE; is
 *     ignored for a "Get Feature Request"
 * - ulRfu: reserved for future use, set to 0 for this ucMinorVersion
 * - pRfu: reserved for future use, set to NULL for this ucMinorVersion
 *
 * Returns SC_ERROR_SUCCESS upon success, or another (preferably) PCSC error code otherwise.
 */
EIDMW_PP_API long EIDMW_PP2_Command(
	SCARDHANDLE hCard, int ioctl,
	const unsigned char *pucSendbuf, DWORD dwSendlen,
	unsigned char *pucRecvbuf, DWORD dwRrecvlen, DWORD *pdwRrecvlen,
	unsigned char ucPintype, unsigned char ucOperation,
	unsigned long ulRfu, void *pRfu);


/////////////////////////////////// CCID things ///////////////////////////////
// More info:
// http://www.pcscworkgroup.com/specifications/files/pcsc10_v2.01.6.pdf
// http://www.usb.org/developers/devclass_docs/DWG_Smart-Card_CCID_Rev110.pdf
///////////////////////////////////////////////////////////////////////////////

#ifndef SCARD_CTL_CODE
#define SCARD_CTL_CODE(code)     (0x42000000 + (code))
#endif
#define CCID_IOCTL_GET_FEATURE_REQUEST  SCARD_CTL_CODE(3400)

#define CCID_VERIFY_START    0x01
#define CCID_VERIFY_FINISH   0x02
#define CCID_VERIFY_DIRECT   0x06
#define CCID_CHANGE_START    0x03
#define CCID_CHANGE_FINISH   0x04
#define CCID_CHANGE_DIRECT   0x07

/* Big endian encoding to 2 bytes */
inline void ToUchar2(unsigned long ulIn, unsigned char *pucOut2)
{
	pucOut2[0] = (unsigned char) (ulIn % 256);
	pucOut2[1] = (unsigned char) (ulIn / 256);
}

/* Big endian encoding to 4 bytes */
inline void ToUchar4(unsigned long ulIn, unsigned char *pucOut4)
{
	pucOut4[0] = (unsigned char) (ulIn % 256);
	ulIn /= 256;
	pucOut4[1] = (unsigned char) (ulIn % 256);
	ulIn /= 256;
	pucOut4[2] = (unsigned char) (ulIn % 256);
	pucOut4[3] = (unsigned char) (ulIn / 256);
}

// The structs below need packing with 1-byte alignment
#pragma pack(push, pinpad2, 1)

#define PP_APDU_MAX_LEN  40

/**
 * Data to be sent during a PIN verification
 * (or unblock without PIN change).
 */
typedef struct
{
	unsigned char bTimerOut;	// timeout in seconds (0: default timeout)
	unsigned char bTimerOut2; // timeout in seconds after first key stroke
	unsigned char bmFormatString; // formatting options
	unsigned char bmPINBlockString; // length (in bytes) of the PIN block
	unsigned char bmPINLengthFormat; // where (if needed) to put the PIN length
	unsigned char wPINMaxExtraDigit[2]; // min and max number of PIN digits
	unsigned char bEntryValidationCondition; // e.g. 0x02: "OK" button pressed
	unsigned char bNumberMessage; // number of messages to display
	unsigned char wLangId[2]; // LANG_ID code
	unsigned char bMsgIndex; // Message index (should be 00)
	unsigned char bTeoPrologue[3]; // T=1 block prologue field to use (fill with 00)
	unsigned char ulDataLength[4]; // length of the following field
	unsigned char abData[PP_APDU_MAX_LEN]; // APDU to send to the card (to be completed by the reader)
} EIDMW_PP_VERIFY_CCID;

/**
 * Data to be sent during a PIN change
 * (or unblock with PIN change).
 */
typedef struct
{
	unsigned char bTimerOut;	// timeout is seconds (00 means use default timeout)
	unsigned char bTimerOut2; // timeout in seconds after first key stroke
	unsigned char bmFormatString; // formatting options
	unsigned char bmPINBlockString; // length (in bytes) of the PIN block
	unsigned char bmPINLengthFormat; // where (if needed) to put the PIN length
	unsigned char bInsertionOffsetOld; //insertion position offset in bytes for the current PIN
	unsigned char bInsertionOffsetNew; // insertion position offset in bytes for the new PIN
	unsigned char wPINMaxExtraDigit[2]; // min and max number of PIN digits
	unsigned char bConfirmPIN; // flags governing need for confirmation of new PIN
	unsigned char bEntryValidationCondition; // e.g. 0x02: "OK" button pressed
	unsigned char bNumberMessage; // number of messages to display
	unsigned char wLangId[2]; // LANG_ID code to specify the language
	unsigned char bMsgIndex1; // index of 1st prompting message
	unsigned char bMsgIndex2; // index of 2d prompting message
	unsigned char bMsgIndex3; // index of 3d prompting message
	unsigned char bTeoPrologue[3]; // T=1 block prologue field to use (fill with 00)
	unsigned char ulDataLength[4]; // length of the following field
	unsigned char abData[PP_APDU_MAX_LEN]; // APDU to send to the card (to be completed by the reader)
} EIDMW_PP_CHANGE_CCID;

#pragma pack(pop, pinpad2)

#ifdef  __cplusplus
}
#endif

#endif
