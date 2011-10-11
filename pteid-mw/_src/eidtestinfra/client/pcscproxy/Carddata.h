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

#ifndef CARDDATA_H
#define CARDDATA_H

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <stddef.h>
#define IN
#define OUT
#endif

namespace eidmw { namespace pcscproxy { namespace carddata {
										static const size_t CHIP_NR_LEN    = 16;
										static const size_t VERSION_NR_LEN = 11;

										const BYTE          APDU_GET_CARD_DATA[5] = { 0x80, 0xE4, 0x00, 0x00, 0x1C };
										const size_t        SIZEOF_APDU           = 5;

										const BYTE          APDU_SELECT_APPLET[20] = {0x00,0xA4,0x04,0x00,0x0F,0xA0,0x00,0x00,0x00,0x30,0x29,0x05,0x70,0x00,0xAD,0x13,0x10,0x01,0x01,0xFF};
										const size_t        SIZEOF_SEL_APPL       = 20;

										//STATUS_BYTES
										static const unsigned char
										NORMAL_ENDING_OF_CMD[2] = { 0x90, 0x00 },        //Normal ending of the command
																						 //Table 80 � GET CARD DATA Status bytes
										NO_PRECISE_DIAGNOSTIC[2] = { 0x64, 0x00 },       //No precise diagnostic
										EEPROM_CORRUPTED[2]      = { 0x65, 0x81 },       //EEPROM corrupted (followed by a mute state)
										WRONG_LENGTH[2]          = { 0x67, 0x00 },       //Wrong length
										WRONG_PARAM_P1P2[2]      = { 0x6B, 0x00 },       //Wrong parameter P1-P2
										CMD_NOT_AVAILABLE[2]     = { 0x6D, 0x00 },       //Command not available within the current life cycle
										WRONG_CLASS_BYTE[2]      = { 0x6E, 0x00 },       //Wrong Class byte

																						 //Table 17 � SELECT FILE Status bytes
										SELECTED_FILE_NOT_ACTIVATED[2] = { 0x62, 0x83 }, //Selected file not activated
										FILE_NOT_FOUND[2]              = { 0x6A, 0x82 }, //File not found
										FILE_WRONG_PARAM_P1P2[2]       = { 0x6A, 0x86 }, //Wrong parameter P1-P2
										LC_INCONSISTENT_WITH_P1P2[2]   = { 0x6A, 0x87 }, //Lc inconsistent with P1-P2
										CLA_NOT_SUPPORTED[2]           = { 0x6E, 0x00 }, //CLA not supported

																						 //Table 20 � ACTIVATE Status bytes
										CMD_NOT_ALLOWED_FILE_NO_EF[2] = { 0x69, 0x86 };  //Command not allowed (The file to activate is not a transparent EF)

																						 //APP_LIFE_CYCLE_STATE
										const unsigned char
										PRE_PERSONALIZED = 0x07,
										DEACTIVATED      = 0x0F,
										ACTIVATED        = 0x8A,
										LOCKED           = 0xFF;

																  //PKCS_1_SUPPORT
										const char
										RSASSA_PKCS1_V1_5 = 0x01, //	-	-	-	-	-	-	-	1	RSASSA-PKCS1 v1.5 supported (MD5 and SHA-1)
										RSASSA_PSS        = 0x02, //	-	-	-	-	-	-	1	-	RSASSA-PSS supported (SHA-1)
										RSAES_PKCS1_V1_5  = 0x04, //	-	-	-	-	-	1	-	-	RSAES-PKCS1 v1.5 supported
										RSAES_OAEP        = 0x08, //	-	-	-	-	1	-	-	-	RSAES-OAEP supported
										RSA_KEM           = 0x10; //	-	-	-	1	-	-	-	-	RSA-KEM supported
																  //	0	0	0	-	-	-	-	-	Other values are RFU

																  //INS
										const unsigned char
										INS_GET_CARD_DATA = 0xE4, // instruction for the proprietary GET CARD DATA command
										INS_SELECT_FILE   = 0xA4, // instruction for the ISO/IEC 7816-4 SELECT FILE command
										INS_READ_BINARY   = 0xB0, // instruction for the ISO/IEC 7816-4 READ BINARY command
										INS_GET_RESPONSE  = 0xC0; // instruction for the ISO/IEC 7816-4 GET RESPONSE command

																  //CLA
										const unsigned char
										DEF_CLASS  = 0x00,        // Class of the default APDU commands
										PROP_CLASS = 0x80;        // Class of the proprietary APDU commands

																  //SELECT_FILE
										const unsigned char
										FID           = 0x02,     // P1 (the data field shall contain a File ID)
										ABSOLUTE_PATH = 0x08,     // P1 the data field shall contain an absolute path
										AID           = 0x04,     // P1 the data field shall contain an AID
										NO_RETURN_FCI = 0x0C;     // P2 (No FCI to be returned)

										class Carddata {
public:
										~Carddata()
										{
										};

										static LONG cmdNotAvailable(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);
										static LONG wrongParamP1P2(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);
										static LONG leTooLong(IN size_t expected, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);
										static LONG bytesRemaining(IN size_t remaining, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength, IN size_t offset = 0);
										static LONG normalEndingOfCmd(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength, IN size_t offset = 0);
										static LONG fileLcInconsistentWithP1P2(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);
										static LONG fileNotFound(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);
										static LONG fileWrongParamP1P2(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);
										static LONG cmdNotAllowedFileNoEF(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);

private:
										Carddata();                           //do not implement
										Carddata(const Carddata&);            //do not implement
										Carddata& operator=(const Carddata&); //do not implement
										};
										} //carddata
				  } //pcscproxy
}                   //eidmw

#endif

