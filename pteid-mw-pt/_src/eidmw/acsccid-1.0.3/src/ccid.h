/*
    ccid.h: CCID structures
    Copyright (C) 2003-2009   Ludovic Rousseau
    Copyright (C) 2009-2012   Advanced Card Systems Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this library; if not, write to the Free Software Foundation,
	Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/*
 * $Id: ccid.h 4280 2009-06-26 14:58:23Z rousseau $
 */

#ifdef __APPLE__
#include <pthread.h>
#endif

typedef struct
{
	/*
	 * CCID Sequence number
	 */
	unsigned char *pbSeq;
	unsigned char real_bSeq;

	/*
	 * VendorID << 16 + ProductID
	 */
	int readerID;

	/*
	 * Maximum message length
	 */
	unsigned int dwMaxCCIDMessageLength;

	/*
	 * Maximum IFSD
	 */
	int dwMaxIFSD;

	/*
	 * Features supported by the reader (directly from Class Descriptor)
	 */
	int dwFeatures;

	/*
	 * PIN support of the reader (directly from Class Descriptor)
	 */
	char bPINSupport;

	/*
	 * Display dimensions of the reader (directly from Class Descriptor)
	 */
	unsigned int wLcdLayout;

	/*
	 * Default Clock
	 */
	int dwDefaultClock;

	/*
	 * Max Data Rate
	 */
	unsigned int dwMaxDataRate;

	/*
	 * Number of available slots
	 */
	char bMaxSlotIndex;

	/*
	 * Slot in use
	 */
	char bCurrentSlotIndex;

	/*
	 * The array of data rates supported by the reader
	 */
	unsigned int *arrayOfSupportedDataRates;

	/*
	 * Read communication port timeout
	 * value is seconds
	 * this value can evolve dynamically if card request it (time processing).
	 */
	unsigned int readTimeout;

	/*
	 * Card protocol
	 */
	int cardProtocol;

	/*
	 * bInterfaceProtocol (CCID, ICCD-A, ICCD-B)
	 */
	int bInterfaceProtocol;

	/*
	 * bNumEndpoints
	 */
	int bNumEndpoints;

	/*
	 * GemCore SIM PRO slot status management
	 * The reader always reports a card present even if no card is inserted.
	 * If the Power Up fails the driver will report IFD_ICC_NOT_PRESENT instead
	 * of IFD_ICC_PRESENT
	 */
	int dwSlotStatus;

	/*
	 * bVoltageSupport (bit field)
	 * 1 = 5.0V
	 * 2 = 3.0V
	 * 4 = 1.8V
	 */
	int bVoltageSupport;

	// bcdDevice
	unsigned int bcdDevice;

	// Pointer to array of bStatus
	unsigned char *bStatus;

#ifdef __APPLE__
	// Lock for array of bStatus
	pthread_mutex_t bStatusLock;
	pthread_mutex_t *pbStatusLock;
#endif

	// Firmware fix enabled
	int firmwareFixEnabled;

	// PICC enabled
	int piccEnabled;
	int *pPiccEnabled;

	// PICC reader index
	int piccReaderIndex;
	int *pPiccReaderIndex;

	// Card voltage (ACR38U, ACR38U-SAM and SCR21U)
	unsigned char cardVoltage;

	// Card type (ACR38U, ACR38U-SAM and SCR21U)
	unsigned char cardType;

	// True if it is a SAM slot
	int isSamSlot;
} _ccid_descriptor;

/* Features from dwFeatures */
#define CCID_CLASS_AUTO_CONF_ATR	0x00000002
#define CCID_CLASS_AUTO_VOLTAGE		0x00000008
#define CCID_CLASS_AUTO_BAUD		0x00000020
#define CCID_CLASS_AUTO_PPS_PROP	0x00000040
#define CCID_CLASS_AUTO_PPS_CUR		0x00000080
#define CCID_CLASS_AUTO_IFSD		0x00000400
#define CCID_CLASS_CHARACTER		0x00000000
#define CCID_CLASS_TPDU				0x00010000
#define CCID_CLASS_SHORT_APDU		0x00020000
#define CCID_CLASS_EXTENDED_APDU	0x00040000
#define CCID_CLASS_EXCHANGE_MASK	0x00070000

/* Features from bPINSupport */
#define CCID_CLASS_PIN_VERIFY		0x01
#define CCID_CLASS_PIN_MODIFY		0x02

/* See CCID specs ch. 4.2.1 */
#define CCID_ICC_PRESENT_ACTIVE		0x00	/* 00 0000 00 */
#define CCID_ICC_PRESENT_INACTIVE	0x01	/* 00 0000 01 */
#define CCID_ICC_ABSENT				0x02	/* 00 0000 10 */
#define CCID_ICC_STATUS_MASK		0x03	/* 00 0000 11 */

#define CCID_COMMAND_FAILED			0x40	/* 01 0000 00 */
#define CCID_TIME_EXTENSION			0x80	/* 10 0000 00 */

/* bInterfaceProtocol for ICCD */
#define ICCD_A	1	/* ICCD Version A */
#define ICCD_B	2	/* ICCD Version B */
#define PROTOCOL_ACR38	38	// ACR38 non-CCID

/* Product identification for special treatments */
#define GEMPC433	0x08E64433
#define GEMPCKEY	0x08E63438
#define GEMPCTWIN	0x08E63437
#define GEMPCPINPAD 0x08E63478
#define GEMCORESIMPRO 0x08E63480
#define GEMCOREPOSPRO 0x08E63479
#define GEMALTOPROXDU 0x08E65503
#define GEMALTOPROXSU 0x08E65504
#define CARDMAN3121	0x076B3021
#define LTC31		0x07830003
#define SCR331DI	0x04E65111
#define SCR331DINTTCOM	0x04E65120
#define SDI010		0x04E65121
#define CHERRYXX33	0x046A0005
#define CHERRYST2000	0x046A003E
#define OZ776		0x0B977762
#define OZ776_7772	0x0B977772
#define SPR532		0x04E6E003
#define MYSMARTPAD	0x09BE0002
#define CHERRYXX44	0x046a0010
#define CL1356D		0x0B810200
#define REINER_SCT	0x0C4B0300
#define SEG			0x08E68000
#define BLUDRIVEII_CCID	0x1B0E1078
#define DELLSCRK    0x413C2101
#define KOBIL_TRIBANK	0x0D463010

// CCID readers
#define ACS_ACR33U_A1_3SAM_ICC_READER	0x072f8300
#define ACS_ACR33U_A2_3SAM_ICC_READER	0x072f8302
#define ACS_ACR33U_A3_3SAM_ICC_READER	0x072f8307
#define ACS_ACR33U_4SAM_ICC_READER		0x072f8301
#define ACS_ACR83U						0x072f90d2
#define ACS_ACR85_PINPAD_READER_ICC		0x072f8306
#define ACS_ACR85_PINPAD_READER_PICC	0x072f2210
#define ACS_ACR88U						0x072f2011
#define ACS_ACR89_ICC_READER			0x072f8900
#define ACS_ACR89_DUAL_READER			0x072f8901
#define ACS_ACR89_FP_READER				0x072f8902
#define ACS_ACR122U						0x072f2200
#define ACS_ACR1222_1SAM_PICC_READER	0x072f2214	// ACR1222U-C1
#define ACS_ACR1222_1SAM_DUAL_READER	0x072f1280	// ACR1222U-C3
#define ACS_ACR1222_DUAL_READER			0x072f2207	// ACR1222U-C6
#define ACS_ACR1222_3S_PICC_READER		0x072f2206	// ACR1222L-D1
#define ACS_ACR128U						0x072f2100
#define ACS_ACR1281_DUAL_READER_QPBOC	0x072f2208
#define ACS_ACR1281_PICC_READER_BSI		0x072f0901
#define ACS_ACR1281_DUAL_READER_BSI		0x072f220a
#define ACS_ACR1281_1S_DUAL_READER		0x072f2224
#define ACS_ACR1281_2S_CL_READER		0x072f2215
#define ACS_ACR1281_1S_PICC_READER		0x072f2220
#define ACS_AET62_PICC_READER			0x072f0102
#define ACS_AET62_1SAM_PICC_READER		0x072f0103
#define ACS_APG8201						0x072f8201
#define ACS_APG8201Z					0x072f8202

// non-CCID readers
#define ACS_ACR38U						0x072f9000
#define ACS_ACR38U_SAM					0x072f90cf
#define IRIS_SCR21U						0x072f90ce
#define ACS_AET65_1SAM_ICC_READER		0x072f0101
#define ACS_CRYPTOMATE					0x072f9006

/*
 * The O2Micro OZ776S reader has a wrong USB descriptor
 * The extra[] field is associated with the last endpoint instead of the
 * main USB descriptor
 */
#define O2MICRO_OZ776_PATCH

/* Escape sequence codes */
#define ESC_GEMPC_SET_ISO_MODE		1
#define ESC_GEMPC_SET_APDU_MODE		2

/*
 * Possible values :
 * 3 -> 1.8V, 3V, 5V
 * 2 -> 3V, 5V
 * 1 -> 5V only
 * 0 -> automatic (selection made by the reader)
 */
/*
 * To be safe we default to 5V
 * otherwise we would have to parse the ATR and get the value of TAi (i>2) when
 * in T=15
 */
#define VOLTAGE_AUTO 0
#define VOLTAGE_5V 1
#define VOLTAGE_3V 2
#define VOLTAGE_1_8V 3

int ccid_open_hack_pre(unsigned int reader_index);
int ccid_open_hack_post(unsigned int reader_index);
void ccid_error(int error, const char *file, int line, const char *function);
_ccid_descriptor *get_ccid_descriptor(unsigned int reader_index);
void acr38_error(int error, const char *file, int line, const char *function);
void EnablePicc(unsigned int reader_index, int enabled);

/* convert a 4 byte integer in USB format into an int */
#define dw2i(a, x) (unsigned int)((((((a[x+3] << 8) + a[x+2]) << 8) + a[x+1]) << 8) + a[x])

/* all the data rates specified by ISO 7816-3 Fi/Di tables */
#define ISO_DATA_RATES 10753, 14337, 15625, 17204, \
		20833, 21505, 23438, 25806, 28674, \
		31250, 32258, 34409, 39063, 41667, \
		43011, 46875, 52083, 53763, 57348, \
		62500, 64516, 68817, 71685, 78125, \
		83333, 86022, 93750, 104167, 107527, \
		114695, 125000, 129032, 143369, 156250, \
		166667, 172043, 215054, 229391, 250000, \
		344086

/* data rates supported by the secondary slots on the GemCore Pos Pro & SIM Pro */
#define GEMPLUS_CUSTOM_DATA_RATES 10753, 21505, 43011, 125000

