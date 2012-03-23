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

**************************************************************************** *
*
*/

#ifndef __PINPAD3_H__
#define __PINPAD3_H__

#include <winscard.h>

#ifndef _WIN32
#include "wintypes.h"
#endif

#ifndef CM_IOCTL_GET_FEATURE_REQUEST
#define CM_IOCTL_GET_FEATURE_REQUEST SCARD_CTL_CODE(3400) //Definition from reader.h in pcsclite
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
    unsigned int ulDataLength; // length of the following field
	unsigned char abData[PP_APDU_MAX_LEN]; // APDU to send to the card (to be completed by the reader)
} PP_VERIFY_CCID;

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
        unsigned int ulDataLength; // length of the following field
	unsigned char abData[PP_APDU_MAX_LEN]; // APDU to send to the card (to be completed by the reader)
} PP_CHANGE_CCID;

#pragma pack(pop, pinpad2)


#endif
