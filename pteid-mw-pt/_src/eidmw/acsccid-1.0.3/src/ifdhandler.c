/*
    ifdhandler.c: IFDH API
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

/* $Id: ifdhandler.c 4346 2009-07-28 13:39:37Z rousseau $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "config.h"
#include "misc.h"
#include <pcsclite.h>
#include <ifdhandler.h>
#include <reader.h>

#include "ccid.h"
#include "defs.h"
#include "ccid_ifdhandler.h"
#include "debug.h"
#include "utils.h"
#include "commands.h"
#include "acr38cmd.h"
#include "towitoko/atr.h"
#include "towitoko/pps.h"
#include "parser.h"

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

#ifndef IFD_ERROR_INSUFFICIENT_BUFFER
#define IFD_ERROR_INSUFFICIENT_BUFFER 618
#endif

/* Array of structures to hold the ATR and other state value of each slot */
static CcidDesc CcidSlots[CCID_DRIVER_MAX_READERS];

/* global mutex */
#ifdef HAVE_PTHREAD
static pthread_mutex_t ifdh_context_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

int LogLevel = DEBUG_LEVEL_CRITICAL | DEBUG_LEVEL_INFO;
int DriverOptions = 0;
int PowerOnVoltage = VOLTAGE_5V;
static int DebugInitialized = FALSE;

// Card voltage and card type selection for ACR38U, ACR38U-SAM and SCR21U
int ACR38CardVoltage = 0;
int ACR38CardType = 0;

/* local functions */
#if HAVE_DECL_TAG_IFD_POLLING_THREAD && !defined(TWIN_SERIAL) && defined(USE_USB_INTERRUPT)
static RESPONSECODE IFDHPolling(DWORD Lun);
static RESPONSECODE IFDHSleep(DWORD Lun);
#endif
static void init_driver(void);
static void extra_egt(ATR_t *atr, _ccid_descriptor *ccid_desc, DWORD Protocol);
static char find_baud_rate(unsigned int baudrate, unsigned int *list);
static unsigned int T0_card_timeout(double f, double d, int TC1, int TC2,
	int clock_frequency);
static unsigned int T1_card_timeout(double f, double d, int TC1, int BWI,
	int CWI, int clock_frequency);
static int get_IFSC(ATR_t *atr, int *i);


EXTERNAL RESPONSECODE IFDHCreateChannelByName(DWORD Lun, LPSTR lpcDevice)
{
	RESPONSECODE return_value = IFD_SUCCESS;
	int reader_index;
	_ccid_descriptor *ccid_descriptor;
	status_t ret;

	if (! DebugInitialized)
		init_driver();

	DEBUG_INFO3("lun: %X, device: %s", Lun, lpcDevice);

	if (-1 == (reader_index = GetNewReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	ccid_descriptor = get_ccid_descriptor(reader_index);

	/* Reset ATR buffer */
	CcidSlots[reader_index].nATRLength = 0;
	*CcidSlots[reader_index].pcATRBuffer = '\0';

	/* Reset PowerFlags */
	CcidSlots[reader_index].bPowerFlags = POWERFLAGS_RAZ;

	/* reader name */
	CcidSlots[reader_index].readerName = strdup(lpcDevice);

#ifdef HAVE_PTHREAD
	(void)pthread_mutex_lock(&ifdh_context_mutex);
#endif

	ret = OpenPortByName(reader_index, lpcDevice);
	if (ret != STATUS_SUCCESS)
	{
		DEBUG_CRITICAL("failed");
		if (STATUS_NO_SUCH_DEVICE == ret)
			return_value = IFD_NO_SUCH_DEVICE;
		else
			return_value = IFD_COMMUNICATION_ERROR;

		/* release the allocated reader_index */
		ReleaseReaderIndex(reader_index);
	}
	else
	{
		unsigned char pcbuffer[SIZE_GET_SLOT_STATUS];
		unsigned int oldReadTimeout;
		int readerOk;
		int numRetries;

		// Assign reader operations
		if (ccid_descriptor->bInterfaceProtocol == PROTOCOL_ACR38)
		{
			// ACR38 reader
			CcidSlots[reader_index].pPowerOn = ACR38_CmdPowerOn;
			CcidSlots[reader_index].pPowerOff = ACR38_CmdPowerOff;
			CcidSlots[reader_index].pGetSlotStatus = ACR38_CmdGetSlotStatus;
			CcidSlots[reader_index].pXfrBlock = ACR38_CmdXfrBlock;
			CcidSlots[reader_index].pTransmitT1 = ACR38_TransmitT1;
			CcidSlots[reader_index].pTransmitPPS = ACR38_TransmitPPS;
			CcidSlots[reader_index].pReceive = ACR38_Receive;
			CcidSlots[reader_index].pSetParameters = ACR38_SetParameters;

			// Set card voltage
			(void)ACR38_SetCardVoltage(reader_index, (unsigned char *) &ACR38CardVoltage,
				sizeof(ACR38CardVoltage), NULL, NULL);

			// Set card type
			(void)ACR38_SetCardType(reader_index, (unsigned char *) &ACR38CardType,
				sizeof(ACR38CardType), NULL, NULL);
		}
		else
		{
			// CCID reader
			CcidSlots[reader_index].pPowerOn = CmdPowerOn;
			CcidSlots[reader_index].pPowerOff = CmdPowerOff;
			CcidSlots[reader_index].pGetSlotStatus = CmdGetSlotStatus;
			CcidSlots[reader_index].pXfrBlock = CmdXfrBlock;
			CcidSlots[reader_index].pTransmitT1 = CCID_Transmit;
			CcidSlots[reader_index].pTransmitPPS = CCID_Transmit;
			CcidSlots[reader_index].pReceive = CCID_Receive;
			CcidSlots[reader_index].pSetParameters = SetParameters;
		}

		/* Maybe we have a special treatment for this reader */
		(void)ccid_open_hack_pre(reader_index);

		/* save the current read timeout computed from card capabilities */
		oldReadTimeout = ccid_descriptor->readTimeout;

		/* 1 second just to resync the USB toggle bits */
		ccid_descriptor->readTimeout = 1;

		/* Try to access the reader */
		/* This "warm up" sequence is sometimes needed when pcscd is
		 * restarted with the reader already connected. We get some
		 * "usb_bulk_read: Resource temporarily unavailable" on the first
		 * few tries. It is an empirical hack */
		readerOk = FALSE;
		numRetries = 10;
		while (numRetries > 0)
		{
			if (CcidSlots[reader_index].pGetSlotStatus(reader_index, pcbuffer) == IFD_SUCCESS)
			{
				readerOk = TRUE;
				break;
			}

			numRetries--;
		}

		if (!readerOk)
		{
			DEBUG_CRITICAL("failed");
			return_value = IFD_COMMUNICATION_ERROR;

			/* release the allocated resources */
			(void)ClosePort(reader_index);
			ReleaseReaderIndex(reader_index);
		}
		else
		{
			/* set back the old timeout */
			ccid_descriptor->readTimeout = oldReadTimeout;

			/* Maybe we have a special treatment for this reader */
			(void)ccid_open_hack_post(reader_index);
		}
	}

#ifdef HAVE_PTHREAD
	(void)pthread_mutex_unlock(&ifdh_context_mutex);
#endif

	if (return_value == IFD_SUCCESS)
	{
		DEBUG_INFO2("dwFeatures: 0x%08X", ccid_descriptor->dwFeatures);
		DEBUG_INFO2("wLcdLayout: 0x%04X", ccid_descriptor->wLcdLayout);
		DEBUG_INFO2("bPINSupport: 0x%02X", ccid_descriptor->bPINSupport);
		DEBUG_INFO2("dwMaxCCIDMessageLength: %d", ccid_descriptor->dwMaxCCIDMessageLength);
		DEBUG_INFO2("dwMaxIFSD: %d", ccid_descriptor->dwMaxIFSD);
		DEBUG_INFO2("dwDefaultClock: %d", ccid_descriptor->dwDefaultClock);
		DEBUG_INFO2("dwMaxDataRate: %d", ccid_descriptor->dwMaxDataRate);
		DEBUG_INFO2("bMaxSlotIndex: %d", ccid_descriptor->bMaxSlotIndex);
		DEBUG_INFO2("bCurrentSlotIndex: %d", ccid_descriptor->bCurrentSlotIndex);
		DEBUG_INFO2("bInterfaceProtocol: 0x%02X", ccid_descriptor->bInterfaceProtocol);
		DEBUG_INFO2("bNumEndpoints: %d", ccid_descriptor->bNumEndpoints);
		DEBUG_INFO2("bVoltageSupport: 0x%02X", ccid_descriptor->bVoltageSupport);
	}

	return return_value;
} /* IFDHCreateChannelByName */


EXTERNAL RESPONSECODE IFDHCreateChannel(DWORD Lun, DWORD Channel)
{
	/*
	 * Lun - Logical Unit Number, use this for multiple card slots or
	 * multiple readers. 0xXXXXYYYY - XXXX multiple readers, YYYY multiple
	 * slots. The resource manager will set these automatically.  By
	 * default the resource manager loads a new instance of the driver so
	 * if your reader does not have more than one smartcard slot then
	 * ignore the Lun in all the functions. Future versions of PC/SC might
	 * support loading multiple readers through one instance of the driver
	 * in which XXXX would be important to implement if you want this.
	 */

	/*
	 * Channel - Channel ID.  This is denoted by the following: 0x000001 -
	 * /dev/pcsc/1 0x000002 - /dev/pcsc/2 0x000003 - /dev/pcsc/3
	 *
	 * USB readers may choose to ignore this parameter and query the bus
	 * for the particular reader.
	 */

	/*
	 * This function is required to open a communications channel to the
	 * port listed by Channel.  For example, the first serial reader on
	 * COM1 would link to /dev/pcsc/1 which would be a sym link to
	 * /dev/ttyS0 on some machines This is used to help with intermachine
	 * independance.
	 *
	 * Once the channel is opened the reader must be in a state in which
	 * it is possible to query IFDHICCPresence() for card status.
	 *
	 * returns:
	 *
	 * IFD_SUCCESS IFD_COMMUNICATION_ERROR
	 */
	RESPONSECODE return_value = IFD_SUCCESS;
	_ccid_descriptor *ccid_descriptor;
	int reader_index;

	if (! DebugInitialized)
		init_driver();

	DEBUG_INFO2("lun: %X", Lun);

	if (-1 == (reader_index = GetNewReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	ccid_descriptor = get_ccid_descriptor(reader_index);

	/* Reset ATR buffer */
	CcidSlots[reader_index].nATRLength = 0;
	*CcidSlots[reader_index].pcATRBuffer = '\0';

	/* Reset PowerFlags */
	CcidSlots[reader_index].bPowerFlags = POWERFLAGS_RAZ;

	/* reader name */
	CcidSlots[reader_index].readerName = strdup("no name");

#ifdef HAVE_PTHREAD
	(void)pthread_mutex_lock(&ifdh_context_mutex);
#endif

	if (OpenPort(reader_index, Channel) != STATUS_SUCCESS)
	{
		DEBUG_CRITICAL("failed");
		return_value = IFD_COMMUNICATION_ERROR;

		/* release the allocated reader_index */
		ReleaseReaderIndex(reader_index);
	}
	else
	{
		unsigned char pcbuffer[SIZE_GET_SLOT_STATUS];
		unsigned int oldReadTimeout;
		int readerOk;
		int numRetries;

		// Assign reader operations
		if (ccid_descriptor->bInterfaceProtocol == PROTOCOL_ACR38)
		{
			// ACR38 reader
			CcidSlots[reader_index].pPowerOn = ACR38_CmdPowerOn;
			CcidSlots[reader_index].pPowerOff = ACR38_CmdPowerOff;
			CcidSlots[reader_index].pGetSlotStatus = ACR38_CmdGetSlotStatus;
			CcidSlots[reader_index].pXfrBlock = ACR38_CmdXfrBlock;
			CcidSlots[reader_index].pTransmitT1 = ACR38_TransmitT1;
			CcidSlots[reader_index].pTransmitPPS = ACR38_TransmitPPS;
			CcidSlots[reader_index].pReceive = ACR38_Receive;
			CcidSlots[reader_index].pSetParameters = ACR38_SetParameters;

			// Set card voltage
			(void)ACR38_SetCardVoltage(reader_index, (unsigned char *) &ACR38CardVoltage,
				sizeof(ACR38CardVoltage), NULL, NULL);

			// Set card type
			(void)ACR38_SetCardType(reader_index, (unsigned char *) &ACR38CardType,
				sizeof(ACR38CardType), NULL, NULL);
		}
		else
		{
			// CCID reader
			CcidSlots[reader_index].pPowerOn = CmdPowerOn;
			CcidSlots[reader_index].pPowerOff = CmdPowerOff;
			CcidSlots[reader_index].pGetSlotStatus = CmdGetSlotStatus;
			CcidSlots[reader_index].pXfrBlock = CmdXfrBlock;
			CcidSlots[reader_index].pTransmitT1 = CCID_Transmit;
			CcidSlots[reader_index].pTransmitPPS = CCID_Transmit;
			CcidSlots[reader_index].pReceive = CCID_Receive;
			CcidSlots[reader_index].pSetParameters = SetParameters;
		}

		/* Maybe we have a special treatment for this reader */
		(void)ccid_open_hack_pre(reader_index);

		/* save the current read timeout computed from card capabilities */
		oldReadTimeout = ccid_descriptor->readTimeout;

		/* 1 second just to resync the USB toggle bits */
		ccid_descriptor->readTimeout = 1;

		/* Try to access the reader */
		/* This "warm up" sequence is sometimes needed when pcscd is
		 * restarted with the reader already connected. We get some
		 * "usb_bulk_read: Resource temporarily unavailable" on the first
		 * few tries. It is an empirical hack */
		readerOk = FALSE;
		numRetries = 10;
		while (numRetries > 0)
		{
			if (CcidSlots[reader_index].pGetSlotStatus(reader_index, pcbuffer) == IFD_SUCCESS)
			{
				readerOk = TRUE;
				break;
			}

			numRetries--;
		}

		if (!readerOk)
		{
			DEBUG_CRITICAL("failed");
			return_value = IFD_COMMUNICATION_ERROR;

			/* release the allocated resources */
			(void)ClosePort(reader_index);
			ReleaseReaderIndex(reader_index);
		}
		else
		{
			/* set back the old timeout */
			ccid_descriptor->readTimeout = oldReadTimeout;

			/* Maybe we have a special treatment for this reader */
			(void)ccid_open_hack_post(reader_index);
		}
	}

#ifdef HAVE_PTHREAD
	(void)pthread_mutex_unlock(&ifdh_context_mutex);
#endif

	if (return_value == IFD_SUCCESS)
	{
		DEBUG_INFO2("dwFeatures: 0x%08X", ccid_descriptor->dwFeatures);
		DEBUG_INFO2("wLcdLayout: 0x%04X", ccid_descriptor->wLcdLayout);
		DEBUG_INFO2("bPINSupport: 0x%02X", ccid_descriptor->bPINSupport);
		DEBUG_INFO2("dwMaxCCIDMessageLength: %d", ccid_descriptor->dwMaxCCIDMessageLength);
		DEBUG_INFO2("dwMaxIFSD: %d", ccid_descriptor->dwMaxIFSD);
		DEBUG_INFO2("dwDefaultClock: %d", ccid_descriptor->dwDefaultClock);
		DEBUG_INFO2("dwMaxDataRate: %d", ccid_descriptor->dwMaxDataRate);
		DEBUG_INFO2("bMaxSlotIndex: %d", ccid_descriptor->bMaxSlotIndex);
		DEBUG_INFO2("bCurrentSlotIndex: %d", ccid_descriptor->bCurrentSlotIndex);
		DEBUG_INFO2("bInterfaceProtocol: 0x%02X", ccid_descriptor->bInterfaceProtocol);
		DEBUG_INFO2("bNumEndpoints: %d", ccid_descriptor->bNumEndpoints);
		DEBUG_INFO2("bVoltageSupport: 0x%02X", ccid_descriptor->bVoltageSupport);
	}

	return return_value;
} /* IFDHCreateChannel */


EXTERNAL RESPONSECODE IFDHCloseChannel(DWORD Lun)
{
	/*
	 * This function should close the reader communication channel for the
	 * particular reader.  Prior to closing the communication channel the
	 * reader should make sure the card is powered down and the terminal
	 * is also powered down.
	 *
	 * returns:
	 *
	 * IFD_SUCCESS IFD_COMMUNICATION_ERROR
	 */
	int reader_index;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	DEBUG_INFO3("%s (lun: %X)", CcidSlots[reader_index].readerName, Lun);

	/* Restore the default timeout
	 * No need to wait too long if the reader disapeared */
	get_ccid_descriptor(reader_index)->readTimeout = DEFAULT_COM_READ_TIMEOUT;

	(void)CcidSlots[reader_index].pPowerOff(reader_index);
	/* No reader status check, if it failed, what can you do ? :) */

#ifdef HAVE_PTHREAD
	(void)pthread_mutex_lock(&ifdh_context_mutex);
#endif

	(void)ClosePort(reader_index);
	ReleaseReaderIndex(reader_index);

	free(CcidSlots[reader_index].readerName);
	memset(&CcidSlots[reader_index], 0, sizeof(CcidSlots[reader_index]));

#ifdef HAVE_PTHREAD
	(void)pthread_mutex_unlock(&ifdh_context_mutex);
#endif

	return IFD_SUCCESS;
} /* IFDHCloseChannel */


#if HAVE_DECL_TAG_IFD_POLLING_THREAD && !defined(TWIN_SERIAL) && defined(USE_USB_INTERRUPT)
static RESPONSECODE IFDHPolling(DWORD Lun)
{
	int reader_index;
	int ret;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	/* log only if DEBUG_LEVEL_PERIODIC is set */
	if (LogLevel & DEBUG_LEVEL_PERIODIC)
		DEBUG_INFO3("%s (lun: %X)", CcidSlots[reader_index].readerName, Lun);

	ret = InterruptRead(reader_index, 2*1000);	/* 2 seconds */
	if (ret > 0)
		return IFD_SUCCESS;
	if (0 == ret)
		return IFD_NO_SUCH_DEVICE;
	return IFD_COMMUNICATION_ERROR;
}

/* on an ICCD device the card is always inserted
 * so no card movement will ever happen: just do nothing */
static RESPONSECODE IFDHSleep(DWORD Lun)
{
	int reader_index;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	DEBUG_INFO3("%s (lun: %X)", CcidSlots[reader_index].readerName, Lun);

	/* just sleep for 5 seconds since the polling thread is NOT killable
	 * so pcscd event thread must loop to exit cleanly
	 *
	 * Once the driver (libusb in fact) will support
	 * TAG_IFD_POLLING_THREAD_KILLABLE then we could use a much longer delay
	 * and be killed before pcscd exits
	 */
	(void)sleep(600);	/* 10 minutes */
	return IFD_SUCCESS;
}
#endif


EXTERNAL RESPONSECODE IFDHGetCapabilities(DWORD Lun, DWORD Tag,
	PDWORD Length, PUCHAR Value)
{
	/*
	 * This function should get the slot/card capabilities for a
	 * particular slot/card specified by Lun.  Again, if you have only 1
	 * card slot and don't mind loading a new driver for each reader then
	 * ignore Lun.
	 *
	 * Tag - the tag for the information requested example: TAG_IFD_ATR -
	 * return the Atr and it's size (required). these tags are defined in
	 * ifdhandler.h
	 *
	 * Length - the length of the returned data Value - the value of the
	 * data
	 *
	 * returns:
	 *
	 * IFD_SUCCESS IFD_ERROR_TAG
	 */
	int reader_index;
	RESPONSECODE return_value = IFD_SUCCESS;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	DEBUG_INFO4("tag: 0x%X, %s (lun: %X)", Tag,
		CcidSlots[reader_index].readerName, Lun);

	switch (Tag)
	{
		case TAG_IFD_ATR:
		case SCARD_ATTR_ATR_STRING:
			/* If Length is not zero, powerICC has been performed.
			 * Otherwise, return NULL pointer
			 * Buffer size is stored in *Length */
			if ((int)*Length >= CcidSlots[reader_index].nATRLength)
			{
				*Length = CcidSlots[reader_index].nATRLength;

				memcpy(Value, CcidSlots[reader_index].pcATRBuffer, *Length);
			}
			else
				return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
			break;

		case SCARD_ATTR_ICC_INTERFACE_STATUS:
			*Length = 1;
			if (IFD_ICC_PRESENT == IFDHICCPresence(Lun))
				/* nonzero if contact is active */
				*Value = 1;
			else
				/* smart card electrical contact is not active */
				*Value = 0;
			break;

		case SCARD_ATTR_ICC_PRESENCE:
			*Length = 1;
			/* Single byte indicating smart card presence:
			 * 0 = not present
			 * 1 = card present but not swallowed (applies only if
			 *     reader supports smart card swallowing)
			 * 2 = card present (and swallowed if reader supports smart
			 *     card swallowing)
			 * 4 = card confiscated. */
			if (IFD_ICC_PRESENT == IFDHICCPresence(Lun))
				/* Card present */
				*Value = 2;
			else
				/* Not present */
				*Value = 0;
			break;

#ifdef HAVE_PTHREAD
		case TAG_IFD_SIMULTANEOUS_ACCESS:
			if (*Length >= 1)
			{
				*Length = 1;
				*Value = CCID_DRIVER_MAX_READERS;
			}
			else
				return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
			break;

		case TAG_IFD_THREAD_SAFE:
			if (*Length >= 1)
			{
				*Length = 1;
#ifdef __APPLE__
				*Value = 0; /* Apple pcscd is bogus (rdar://problem/5697388) */
#else
				*Value = 1; /* Can talk to multiple readers at the same time */
#endif
			}
			else
				return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
			break;
#endif

		case TAG_IFD_SLOTS_NUMBER:
			if (*Length >= 1)
			{
				*Length = 1;
				*Value = 1 + get_ccid_descriptor(reader_index) -> bMaxSlotIndex;
#ifdef USE_COMPOSITE_AS_MULTISLOT
				{
					/* On MacOS X or Linux+libusb we can simulate a
					 * composite device with 2 CCID interfaces by a
					 * multi-slot reader */
					int readerID =  get_ccid_descriptor(reader_index) -> readerID;

					// Simulate ACR1281 Dual Reader (composite device) as multi-slot reader
					if ((GEMALTOPROXDU == readerID) ||
						(GEMALTOPROXSU == readerID) ||
						(ACS_ACR1281_DUAL_READER_QPBOC == readerID) ||
						(ACS_ACR1281_DUAL_READER_BSI == readerID) ||
						(ACS_ACR1281_1S_PICC_READER == readerID))
						*Value = 2;
				}
#endif
				DEBUG_INFO2("Reader supports %d slot(s)", *Value);
			}
			else
				return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
			break;

		case TAG_IFD_SLOT_THREAD_SAFE:
			if (*Length >= 1)
			{
				*Length = 1;
				*Value = 0; /* Can NOT talk to multiple slots at the same time */
			}
			else
				return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
			break;

		case SCARD_ATTR_VENDOR_IFD_VERSION:
			/* Vendor-supplied interface device version (DWORD in the form
			 * 0xMMmmbbbb where MM = major version, mm = minor version, and
			 * bbbb = build number). */
			*Length = sizeof(DWORD);
			if (Value)
				*(DWORD *)Value = CCID_VERSION;
			break;

		case SCARD_ATTR_VENDOR_NAME:
#define VENDOR_NAME "Ludovic Rousseau"
			*Length = sizeof(VENDOR_NAME);
			if (Value)
				memcpy(Value, VENDOR_NAME, sizeof(VENDOR_NAME));
			break;

		case SCARD_ATTR_MAXINPUT:
			*Length = sizeof(uint32_t);
			if (Value)
				*(uint32_t *)Value = get_ccid_descriptor(reader_index) -> dwMaxCCIDMessageLength -10;
			break;

#if HAVE_DECL_TAG_IFD_POLLING_THREAD && !defined(TWIN_SERIAL) && defined(USE_USB_INTERRUPT)
		case TAG_IFD_POLLING_THREAD:
			{
				_ccid_descriptor *ccid_desc;

				/* default value: not supported */
				*Length = 0;

				ccid_desc = get_ccid_descriptor(reader_index);
				/* CCID and not ICCD */
				if ((0 == ccid_desc -> bInterfaceProtocol)
					/* 3 end points */
					&& (3 == ccid_desc -> bNumEndpoints))
				{
					*Length = sizeof(void *);
					if (Value)
						*(void **)Value = IFDHPolling;
				}

				if ((ICCD_A == ccid_desc->bInterfaceProtocol)
					|| (ICCD_B == ccid_desc->bInterfaceProtocol))
				{
					*Length = sizeof(void *);
					if (Value)
						*(void **)Value = IFDHSleep;
				}
			}
			break;

		case TAG_IFD_POLLING_THREAD_KILLABLE:
			{
				_ccid_descriptor *ccid_desc;

				/* default value: not supported */
				*Length = 0;

				ccid_desc = get_ccid_descriptor(reader_index);
				if ((ICCD_A == ccid_desc->bInterfaceProtocol)
					|| (ICCD_B == ccid_desc->bInterfaceProtocol))
				{
					*Length = 1;	/* 1 char */
					if (Value)
						*Value = 1;	/* TRUE */
				}
			}
			break;
#endif

		default:
			return_value = IFD_ERROR_TAG;
	}

	return return_value;
} /* IFDHGetCapabilities */


EXTERNAL RESPONSECODE IFDHSetCapabilities(DWORD Lun, DWORD Tag,
	/*@unused@*/ DWORD Length, /*@unused@*/ PUCHAR Value)
{
	/*
	 * This function should set the slot/card capabilities for a
	 * particular slot/card specified by Lun.  Again, if you have only 1
	 * card slot and don't mind loading a new driver for each reader then
	 * ignore Lun.
	 *
	 * Tag - the tag for the information needing set
	 *
	 * Length - the length of the returned data Value - the value of the
	 * data
	 *
	 * returns:
	 *
	 * IFD_SUCCESS IFD_ERROR_TAG IFD_ERROR_SET_FAILURE
	 * IFD_ERROR_VALUE_READ_ONLY
	 */

	(void)Length;
	(void)Value;

	int reader_index;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	DEBUG_INFO4("tag: 0x%X, %s (lun: %X)", Tag,
		CcidSlots[reader_index].readerName, Lun);

	return IFD_NOT_SUPPORTED;
} /* IFDHSetCapabilities */


EXTERNAL RESPONSECODE IFDHSetProtocolParameters(DWORD Lun, DWORD Protocol,
	UCHAR Flags, UCHAR PTS1, UCHAR PTS2, UCHAR PTS3)
{
	/*
	 * This function should set the PTS of a particular card/slot using
	 * the three PTS parameters sent
	 *
	 * Protocol - SCARD_PROTOCOL_T0 or SCARD_PROTOCOL_T1
	 * Flags - Logical OR of possible values:
	 *  IFD_NEGOTIATE_PTS1
	 *  IFD_NEGOTIATE_PTS2
	 *  IFD_NEGOTIATE_PTS3
	 * to determine which PTS values to negotiate.
	 * PTS1,PTS2,PTS3 - PTS Values.
	 *
	 * returns:
	 *  IFD_SUCCESS
	 *  IFD_ERROR_PTS_FAILURE
	 *  IFD_COMMUNICATION_ERROR
	 *  IFD_PROTOCOL_NOT_SUPPORTED
	 */

	BYTE pps[PPS_MAX_LENGTH];
	ATR_t atr;
	unsigned int len;
	int convention;
	int reader_index;

	BYTE Fl;
	BYTE Dl;
	DWORD numProtocols;
	DWORD protocolTypes;
	BYTE tmpProtocol;
	int specificMode;
	int i;

	UCHAR atrBuffer[MAX_ATR_SIZE];
	DWORD atrLen;

	/* Set ccid desc params */
	CcidDesc *ccid_slot;
	_ccid_descriptor *ccid_desc;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	DEBUG_INFO4("protocol T=%d, %s (lun: %X)", Protocol-SCARD_PROTOCOL_T0,
		CcidSlots[reader_index].readerName, Lun);

	/* Set to zero buffer */
	memset(pps, 0, sizeof(pps));
	memset(&atr, 0, sizeof(atr));

	/* Get ccid params */
	ccid_slot = get_ccid_slot(reader_index);
	ccid_desc = get_ccid_descriptor(reader_index);

	/* Do not send CCID command SetParameters or PPS to the CCID
	 * The CCID will do this himself */
	if (ccid_desc->dwFeatures & CCID_CLASS_AUTO_PPS_PROP)
		goto end;

	/* Get ATR of the card */
	(void)ATR_InitFromArray(&atr, ccid_slot->pcATRBuffer,
		ccid_slot->nATRLength);

	/* Apply Extra EGT patch for bogus cards */
	extra_egt(&atr, ccid_desc, Protocol);

	if (SCARD_PROTOCOL_T0 == Protocol)
		pps[1] |= ATR_PROTOCOL_TYPE_T0;
	else
		if (SCARD_PROTOCOL_T1 == Protocol)
			pps[1] |= ATR_PROTOCOL_TYPE_T1;
		else
			return IFD_PROTOCOL_NOT_SUPPORTED;

	/* TA2 present -> specific mode */
	if (atr.ib[1][ATR_INTERFACE_BYTE_TA].present)
	{
		if (pps[1] != (atr.ib[1][ATR_INTERFACE_BYTE_TA].value & 0x0F))
		{
			/* wrong protocol */
			DEBUG_COMM3("Specific mode in T=%d and T=%d requested",
				atr.ib[1][ATR_INTERFACE_BYTE_TA].value & 0x0F, pps[1]);

			return IFD_PROTOCOL_NOT_SUPPORTED;
		}
	}

	// Get Fl/Dl
	Fl = 1;
	Dl = 1;
	(void)ATR_GetIntegerValue(&atr, ATR_INTEGER_VALUE_FI, &Fl);
	(void)ATR_GetIntegerValue(&atr, ATR_INTEGER_VALUE_DI, &Dl);

	// Count number of protocols
	numProtocols = 0;
	protocolTypes = 0;
	for (i = 0; i < ATR_MAX_PROTOCOLS; i++)
	{
		if (atr.ib[i][ATR_INTERFACE_BYTE_TD].present)
		{
			tmpProtocol = atr.ib[i][ATR_INTERFACE_BYTE_TD].value & 0x0F;
			if (((1 << tmpProtocol) & protocolTypes) == 0)
				numProtocols++;

			protocolTypes |= 1 << tmpProtocol;
		}
	}

	// Set default supported protocol to T=0 if card does not support any protocol
	if (protocolTypes == 0)
	{
		numProtocols = 1;
		protocolTypes = SCARD_PROTOCOL_T0;
	}

	// Check if card supports this protocol (Mac OS X)
	if (!(protocolTypes & Protocol))
	{
		DEBUG_COMM2("T=%d not supported", Protocol-SCARD_PROTOCOL_T0);
		return IFD_ERROR_PTS_FAILURE;
	}

	// Check if card is specific mode or not
	if ((atr.ib[1][ATR_INTERFACE_BYTE_TA].present) ||
		((numProtocols <= 1) && (Fl == 1) && (Dl == 1)))
	{
		DEBUG_COMM("Specific mode");
		specificMode = TRUE;
	}
	else
	{
		DEBUG_COMM("Negotiable mode");
		specificMode = FALSE;
	}

	/* TCi (i>2) indicates CRC instead of LRC */
	if (SCARD_PROTOCOL_T1 == Protocol)
	{
		t1_state_t *t1 = &(ccid_slot -> t1);
		int i;

		/* TCi (i>2) present? */
		for (i=2; i<ATR_MAX_PROTOCOLS; i++)
			if (atr.ib[i][ATR_INTERFACE_BYTE_TC].present)
			{
				if (0 == atr.ib[i][ATR_INTERFACE_BYTE_TC].value)
				{
					DEBUG_COMM("Use LRC");
					(void)t1_set_param(t1, IFD_PROTOCOL_T1_CHECKSUM_LRC, 0);
				}
				else
					if (1 == atr.ib[i][ATR_INTERFACE_BYTE_TC].value)
					{
						DEBUG_COMM("Use CRC");
						(void)t1_set_param(t1, IFD_PROTOCOL_T1_CHECKSUM_CRC, 0);
					}
					else
						DEBUG_COMM2("Wrong value for TCi: %d",
							atr.ib[i][ATR_INTERFACE_BYTE_TC].value);

				/* only the first TCi (i>2) must be used */
				break;
			}
	}

	/* PTS1? */
	if (Flags & IFD_NEGOTIATE_PTS1)
	{
		/* just use the value passed in argument */
		pps[1] |= 0x10; /* PTS1 presence */
		pps[2] = PTS1;
	}
	else
	{
		/* TA1 present */
		if (atr.ib[0][ATR_INTERFACE_BYTE_TA].present)
		{
			unsigned int card_baudrate;
			double f, d;

			(void)ATR_GetParameter(&atr, ATR_PARAMETER_D, &d);
			(void)ATR_GetParameter(&atr, ATR_PARAMETER_F, &f);

			/* may happen with non ISO cards */
			if ((0 == f) || (0 == d))
			{
				/* values for TA1=11 */
				f = 372;
				d = 1;
			}

			/* Baudrate = f x D/F */
			card_baudrate = (unsigned int) (1000 * ccid_desc->dwDefaultClock
				* d / f);

			// Fix a problem that the optimal baud rate is not selected properly
			if (card_baudrate <= ccid_desc->dwMaxDataRate)
			{
				pps[1] |= 0x10; /* PTS1 presence */
				pps[2] = atr.ib[0][ATR_INTERFACE_BYTE_TA].value;

				DEBUG_COMM2("Set speed to %d bauds", card_baudrate);
			}
			else
			{
				unsigned char old_TA1;

				// Select default Fl/Dl
				pps[1] |= 0x10; /* PTS1 presence */
				pps[2] = 0x11;

				old_TA1 = atr.ib[0][ATR_INTERFACE_BYTE_TA].value;
				while (atr.ib[0][ATR_INTERFACE_BYTE_TA].value > 0x11)
				{
					/* use a lower TA1 */
					atr.ib[0][ATR_INTERFACE_BYTE_TA].value--;

					(void)ATR_GetParameter(&atr, ATR_PARAMETER_D, &d);
					(void)ATR_GetParameter(&atr, ATR_PARAMETER_F, &f);

					/* Baudrate = f x D/F */
					card_baudrate = (unsigned int) (1000 *
						ccid_desc->dwDefaultClock * d / f);

					/* the reader has no baud rates table */
					if ((ccid_desc->arrayOfSupportedDataRates == NULL) &&
						(card_baudrate <= ccid_desc->dwMaxDataRate)
						/* or explicitely support it */
						|| (ccid_desc->arrayOfSupportedDataRates != NULL) &&
						find_baud_rate(card_baudrate,
							ccid_desc->arrayOfSupportedDataRates))
					{
						pps[1] |= 0x10; /* PTS1 presence */
						pps[2] = atr.ib[0][ATR_INTERFACE_BYTE_TA].value;

						DEBUG_COMM2("Set adapted speed to %d bauds",
							card_baudrate);

						break;
					}
				}

				/* restore original TA1 value */
				atr.ib[0][ATR_INTERFACE_BYTE_TA].value = old_TA1;
			}
		}
	}

	/* PTS2? */
	if (Flags & IFD_NEGOTIATE_PTS2)
	{
		pps[1] |= 0x20; /* PTS2 presence */
		pps[3] = PTS2;
	}

	/* PTS3? */
	if (Flags & IFD_NEGOTIATE_PTS3)
	{
		pps[1] |= 0x40; /* PTS3 presence */
		pps[4] = PTS3;
	}

	/* Generate PPS */
	pps[0] = 0xFF;

again:
	/* Automatic PPS made by the ICC? */
	if ((! (ccid_desc->dwFeatures & CCID_CLASS_AUTO_PPS_CUR))
		&& (!specificMode))	// Negotiable mode
	{
		int default_protocol;

		if (ATR_MALFORMED == ATR_GetDefaultProtocol(&atr, &default_protocol))
			return IFD_PROTOCOL_NOT_SUPPORTED;

		/* if the requested protocol is not the default one
		 * or a TA1/PPS1 is present */
		if (((pps[1] & 0x0F) != default_protocol) || (PPS_HAS_PPS1(pps)))
		{
			unsigned char pps1;
#ifdef O2MICRO_OZ776_PATCH
			if ((OZ776 == ccid_desc->readerID)
				|| (OZ776_7772 == ccid_desc->readerID))
			{
				/* convert from ATR_PROTOCOL_TYPE_T? to SCARD_PROTOCOL_T? */
				Protocol = default_protocol +
				   	(SCARD_PROTOCOL_T0 - ATR_PROTOCOL_TYPE_T0);
				DEBUG_INFO2("PPS not supported on O2Micro readers. Using T=%d",
					Protocol - SCARD_PROTOCOL_T0);
			}
			else
#endif
			if (PPS_Exchange(reader_index, pps, &len, &pps1) != PPS_OK)
			{
				DEBUG_INFO("PPS_Exchange Failed");

				if (pps[2] != 0x11)
				{
					RESPONSECODE ret;

					// Cold reset
					atrLen = sizeof(atrBuffer);
					(void)IFDHPowerICC(Lun, IFD_POWER_DOWN, atrBuffer, &atrLen);
					usleep(100 * 1000);
					atrLen = sizeof(atrBuffer);
					ret = IFDHPowerICC(Lun, IFD_POWER_UP, atrBuffer, &atrLen);
					if (ret != IFD_SUCCESS)
						return ret;

					// Try default Fl/Dl
					pps[2] = 0x11;
					goto again;
				}
				else
					return IFD_ERROR_PTS_FAILURE;
			}
		}
	}

	/* Now we must set the reader parameters */
	(void)ATR_GetConvention(&atr, &convention);

	/* specific mode and implicit parameters? (b5 of TA2) */
	if (atr.ib[1][ATR_INTERFACE_BYTE_TA].present
		&& (atr.ib[1][ATR_INTERFACE_BYTE_TA].value & 0x10))
		return IFD_COMMUNICATION_ERROR;

	/* T=1 */
	if (SCARD_PROTOCOL_T1 == Protocol)
	{
		BYTE param[] = {
			0x11,	/* Fi/Di		*/
			0x10,	/* TCCKS		*/
			0x00,	/* GuardTime	*/
			0x4D,	/* BWI/CWI		*/
			0x00,	/* ClockStop	*/
			0x20,	/* IFSC			*/
			0x00	/* NADValue		*/
		};
		int i;
		t1_state_t *t1 = &(ccid_slot -> t1);
		RESPONSECODE ret;
		double f, d;
		int ifsc;

		/* TA1 is not default */
		if (PPS_HAS_PPS1(pps))
			param[0] = pps[2];

		/* CRC checksum? */
		if (2 == t1->rc_bytes)
			param[1] |= 0x01;

		/* the CCID should ignore this bit */
		if (ATR_CONVENTION_INVERSE == convention)
			param[1] |= 0x02;

		/* get TC1 Extra guard time */
		if (atr.ib[0][ATR_INTERFACE_BYTE_TC].present)
			param[2] = atr.ib[0][ATR_INTERFACE_BYTE_TC].value;

		/* TBi (i>2) present? BWI/CWI */
		for (i=2; i<ATR_MAX_PROTOCOLS; i++)
			if (atr.ib[i][ATR_INTERFACE_BYTE_TB].present)
			{
				DEBUG_COMM3("BWI/CWI (TB%d) present: 0x%02X", i+1,
					atr.ib[i][ATR_INTERFACE_BYTE_TB].value);
				param[3] = atr.ib[i][ATR_INTERFACE_BYTE_TB].value;

				{
					/* Hack for OpenPGP card */
					unsigned char openpgp_atr[] = { 0x3B, 0xFA, 0x13,
						0x00, 0xFF, 0x81, 0x31, 0x80, 0x45, 0x00, 0x31,
						0xC1, 0x73, 0xC0, 0x01, 0x00, 0x00, 0x90, 0x00, 0xB1 };

					if (0 == memcmp(ccid_slot->pcATRBuffer, openpgp_atr,
						ccid_slot->nATRLength))
						/* change BWI from 4 to 7 to increase BWT from
						 * 1.4s to 11s and avoid a timeout during on
						 * board key generation (bogus card) */
					{
						param[3] = 0x75;
						DEBUG_COMM2("OpenPGP hack, using 0x%02X", param[3]);
					}
				}

				/* only the first TBi (i>2) must be used */
				break;
			}

		/* compute communication timeout */
		(void)ATR_GetParameter(&atr, ATR_PARAMETER_F, &f);
		(void)ATR_GetParameter(&atr, ATR_PARAMETER_D, &d);
		ccid_desc->readTimeout = T1_card_timeout(f, d, param[2],
			(param[3] & 0xF0) >> 4 /* BWI */, param[3] & 0x0F /* CWI */,
			ccid_desc->dwDefaultClock);

		ifsc = get_IFSC(&atr, &i);
		if (ifsc > 0)
		{
			DEBUG_COMM3("IFSC (TA%d) present: %d", i, ifsc);
			param[5] = ifsc;
		}

		DEBUG_COMM2("Timeout: %d seconds", ccid_desc->readTimeout);

		// Set parameters if not specific mode
		if (!specificMode)
		{
			ret = ccid_slot->pSetParameters(reader_index, 1, sizeof(param), param);
			if (IFD_SUCCESS != ret)
			{
				DEBUG_INFO("SetParameters (T1) Failed");

				if (param[0] != 0x11)
				{
					// Cold reset
					atrLen = sizeof(atrBuffer);
					(void)IFDHPowerICC(Lun, IFD_POWER_DOWN, atrBuffer, &atrLen);
					usleep(100 * 1000);
					atrLen = sizeof(atrBuffer);
					ret = IFDHPowerICC(Lun, IFD_POWER_UP, atrBuffer, &atrLen);
					if (ret != IFD_SUCCESS)
						return ret;

					// Try default Fl/Dl
					pps[2] = 0x11;
					goto again;
				}
				else
					return ret;
			}
		}
	}
	else
	/* T=0 */
	{
		BYTE param[] = {
			0x11,	/* Fi/Di			*/
			0x00,	/* TCCKS			*/
			0x00,	/* GuardTime		*/
			0x0A,	/* WaitingInteger	*/
			0x00	/* ClockStop		*/
		};
		RESPONSECODE ret;
		double f, d;

		/* TA1 is not default */
		if (PPS_HAS_PPS1(pps))
			param[0] = pps[2];

		if (ATR_CONVENTION_INVERSE == convention)
			param[1] |= 0x02;

		/* get TC1 Extra guard time */
		if (atr.ib[0][ATR_INTERFACE_BYTE_TC].present)
			param[2] = atr.ib[0][ATR_INTERFACE_BYTE_TC].value;

		/* TC2 WWT */
		if (atr.ib[1][ATR_INTERFACE_BYTE_TC].present)
			param[3] = atr.ib[1][ATR_INTERFACE_BYTE_TC].value;

		/* compute communication timeout */
		(void)ATR_GetParameter(&atr, ATR_PARAMETER_F, &f);
		(void)ATR_GetParameter(&atr, ATR_PARAMETER_D, &d);

		ccid_desc->readTimeout = T0_card_timeout(f, d, param[2] /* TC1 */,
			param[3] /* TC2 */, ccid_desc->dwDefaultClock);

		DEBUG_COMM2("Communication timeout: %d seconds",
			ccid_desc->readTimeout);

		// Set parameters if not specific mode
		if (!specificMode)
		{
			ret = ccid_slot->pSetParameters(reader_index, 0, sizeof(param), param);
			if (IFD_SUCCESS != ret)
			{
				DEBUG_INFO("SetParameters (T0) Failed");

				if (param[0] != 0x11)
				{
					// Cold reset
					atrLen = sizeof(atrBuffer);
					(void)IFDHPowerICC(Lun, IFD_POWER_DOWN, atrBuffer, &atrLen);
					usleep(100 * 1000);
					atrLen = sizeof(atrBuffer);
					ret = IFDHPowerICC(Lun, IFD_POWER_UP, atrBuffer, &atrLen);
					if (ret != IFD_SUCCESS)
						return ret;

					// Try default Fl/Dl
					pps[2] = 0x11;
					goto again;
				}
				else
					return ret;
			}
		}
	}

	/* set IFSC & IFSD in T=1 */
	if (SCARD_PROTOCOL_T1 == Protocol)
	{
		t1_state_t *t1 = &(ccid_slot -> t1);
		int i, ifsc;

		ifsc = get_IFSC(&atr, &i);
		if (ifsc > 0)
		{
			DEBUG_COMM3("IFSC (TA%d) present: %d", i, ifsc);
			(void)t1_set_param(t1, IFD_PROTOCOL_T1_IFSC, ifsc);
		}

		// Valid only in TPDU exchange level
		if (ccid_desc->dwFeatures & CCID_CLASS_TPDU)
		{
			/* IFSD not negociated by the reader? */
			if (! (ccid_desc->dwFeatures & CCID_CLASS_AUTO_IFSD))
			{
				DEBUG_COMM2("Negociate IFSD at %d", ccid_desc -> dwMaxIFSD);
				if (t1_negotiate_ifsd(t1, 0, ccid_desc -> dwMaxIFSD) < 0)
					return IFD_COMMUNICATION_ERROR;
			}
		}
		(void)t1_set_param(t1, IFD_PROTOCOL_T1_IFSD, ccid_desc -> dwMaxIFSD);

		DEBUG_COMM3("T=1: IFSC=%d, IFSD=%d", t1->ifsc, t1->ifsd);
	}

end:
	/* store used protocol for use by the secure commands (verify/change PIN) */
	ccid_desc->cardProtocol = Protocol;

	return IFD_SUCCESS;
} /* IFDHSetProtocolParameters */


EXTERNAL RESPONSECODE IFDHPowerICC(DWORD Lun, DWORD Action,
	PUCHAR Atr, PDWORD AtrLength)
{
	/*
	 * This function controls the power and reset signals of the smartcard
	 * reader at the particular reader/slot specified by Lun.
	 *
	 * Action - Action to be taken on the card.
	 *
	 * IFD_POWER_UP - Power and reset the card if not done so (store the
	 * ATR and return it and it's length).
	 *
	 * IFD_POWER_DOWN - Power down the card if not done already
	 * (Atr/AtrLength should be zero'd)
	 *
	 * IFD_RESET - Perform a quick reset on the card.  If the card is not
	 * powered power up the card.  (Store and return the Atr/Length)
	 *
	 * Atr - Answer to Reset of the card.  The driver is responsible for
	 * caching this value in case IFDHGetCapabilities is called requesting
	 * the ATR and it's length.  This should not exceed MAX_ATR_SIZE.
	 *
	 * AtrLength - Length of the Atr.  This should not exceed
	 * MAX_ATR_SIZE.
	 *
	 * Notes:
	 *
	 * Memory cards without an ATR should return IFD_SUCCESS on reset but
	 * the Atr should be zero'd and the length should be zero
	 *
	 * Reset errors should return zero for the AtrLength and return
	 * IFD_ERROR_POWER_ACTION.
	 *
	 * returns:
	 *
	 * IFD_SUCCESS IFD_ERROR_POWER_ACTION IFD_COMMUNICATION_ERROR
	 * IFD_NOT_SUPPORTED
	 */

	unsigned int nlength;
	RESPONSECODE return_value = IFD_SUCCESS;
	unsigned char pcbuffer[10+MAX_ATR_SIZE];
	int reader_index;
	const char *actions[] = { "PowerUp", "PowerDown", "Reset" };
	unsigned int oldReadTimeout;
	_ccid_descriptor *ccid_descriptor;

	/* By default, assume it won't work :) */
	*AtrLength = 0;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	DEBUG_INFO4("action: %s, %s (lun: %X)", actions[Action-IFD_POWER_UP],
		CcidSlots[reader_index].readerName, Lun);

	switch (Action)
	{
		case IFD_POWER_DOWN:
			/* Clear ATR buffer */
			CcidSlots[reader_index].nATRLength = 0;
			*CcidSlots[reader_index].pcATRBuffer = '\0';

			/* Memorise the request */
			CcidSlots[reader_index].bPowerFlags |= MASK_POWERFLAGS_PDWN;

			/* send the command */
			if (IFD_SUCCESS != CcidSlots[reader_index].pPowerOff(reader_index))
			{
				DEBUG_CRITICAL("PowerDown failed");
				return_value = IFD_ERROR_POWER_ACTION;
				goto end;
			}

			/* clear T=1 context */
			t1_release(&(get_ccid_slot(reader_index) -> t1));
			break;

		case IFD_POWER_UP:
		case IFD_RESET:
			/* save the current read timeout computed from card capabilities */
			ccid_descriptor = get_ccid_descriptor(reader_index);
			oldReadTimeout = ccid_descriptor->readTimeout;

			/* use a very long timeout since the card can use up to
			 * (9600+12)*33 ETU in total
			 * 12 ETU per byte
			 * 9600 ETU max between each byte
			 * 33 bytes max for ATR
			 * 1 ETU = 372 cycles during ATR
			 * with a 4 MHz clock => 29 seconds
			 */
			ccid_descriptor->readTimeout = 10;	// 60 seconds is too long

			nlength = sizeof(pcbuffer);
			return_value = CcidSlots[reader_index].pPowerOn(reader_index, &nlength, pcbuffer,
				PowerOnVoltage);

			// Enable/Disable PICC
			if (DriverOptions & DRIVER_OPTION_DISABLE_PICC)
			{
				if ((ccid_descriptor->firmwareFixEnabled) &&
					(((ccid_descriptor->readerID == ACS_ACR1222_DUAL_READER) ||
					(ccid_descriptor->readerID == ACS_ACR1222_1SAM_DUAL_READER)) &&
					(ccid_descriptor->bCurrentSlotIndex == 0) ||
					(ccid_descriptor->readerID == ACS_ACR85_PINPAD_READER_ICC)))
				{
					int i = 0;

					// Perform cold reset after disabling PICC (Try 10 times)
					while ((return_value != IFD_SUCCESS) || (nlength == 0))
					{
						(void)CcidSlots[reader_index].pPowerOff(reader_index);
						usleep(100 * 1000);
						nlength = sizeof(pcbuffer);
						return_value = CcidSlots[reader_index].pPowerOn(reader_index, &nlength, pcbuffer,
							PowerOnVoltage);

						i++;
						if (i >= 10)
							break;
					}
				}
			}

			/* set back the old timeout */
			ccid_descriptor->readTimeout = oldReadTimeout;

			if ((return_value != IFD_SUCCESS) || (nlength == 0))	// ACR1222: No ATR is returned
			{
				/* used by GemCore SIM PRO: no card is present */
				get_ccid_descriptor(reader_index)->dwSlotStatus
					= IFD_ICC_NOT_PRESENT;;

				DEBUG_CRITICAL("PowerUp failed");
				return_value = IFD_ERROR_POWER_ACTION;
				goto end;
			}

			// Remove PUPI from ATR
			if (DriverOptions & DRIVER_OPTION_REMOVE_PUPI_FROM_ATR)
			{
				if ((ccid_descriptor->firmwareFixEnabled) &&
					(((ccid_descriptor->readerID == ACS_ACR1222_DUAL_READER) ||
					(ccid_descriptor->readerID == ACS_ACR1222_1SAM_DUAL_READER)) &&
					(ccid_descriptor->bCurrentSlotIndex == 1) ||
					(ccid_descriptor->readerID == ACS_ACR85_PINPAD_READER_PICC)))
				{
					// ATR: 3B 8N 80 01 50 XX XX XX XX ... TCK
					if ((nlength >= 9) &&
						(pcbuffer[0] == 0x3B) &&
						((pcbuffer[1] & 0xF0) == 0x80) &&
						(pcbuffer[2] == 0x80) &&
						(pcbuffer[3] == 0x01) &&
						(pcbuffer[4] == 0x50))
					{
						unsigned char numHistBytes;
						int i;

						// Get number of historical bytes
						numHistBytes = pcbuffer[1] & 0x0F;

						// Update number of historical bytes
						// Add 1 byte for MBLI (PC/SC 2.0 part 3)
						numHistBytes = numHistBytes - 5 + 1;
						pcbuffer[1] = 0x80 | numHistBytes;

						// Move remained bytes
						if (nlength > 9)
							memmove(pcbuffer + 4, pcbuffer + 9, nlength - 9);

						// Update ATR length
						// Add 1 byte for MBLI (PC/SC 2.0 part 3)
						nlength = nlength - 5 + 1;

						// Assume MBLI is zero here
						// Note: MBLI must be taken from response of ATTRIB command
						pcbuffer[nlength - 2] = 0;

						// Update TCK
						pcbuffer[nlength - 1] = 0;
						for (i = 1; i < nlength - 1; i++)
							pcbuffer[nlength - 1] ^= pcbuffer[i];
					}
				}
			}

			/* Power up successful, set state variable to memorise it */
			CcidSlots[reader_index].bPowerFlags |= MASK_POWERFLAGS_PUP;
			CcidSlots[reader_index].bPowerFlags &= ~MASK_POWERFLAGS_PDWN;

			/* Reset is returned, even if TCK is wrong */
			CcidSlots[reader_index].nATRLength = *AtrLength =
				(nlength < MAX_ATR_SIZE) ? nlength : MAX_ATR_SIZE;
			memcpy(Atr, pcbuffer, *AtrLength);
			memcpy(CcidSlots[reader_index].pcATRBuffer, pcbuffer, *AtrLength);

			/* initialise T=1 context */
			(void)t1_init(&(get_ccid_slot(reader_index) -> t1), reader_index);
			break;

		default:
			DEBUG_CRITICAL("Action not supported");
			return_value = IFD_NOT_SUPPORTED;
	}
end:

	return return_value;
} /* IFDHPowerICC */


EXTERNAL RESPONSECODE IFDHTransmitToICC(DWORD Lun, SCARD_IO_HEADER SendPci,
	PUCHAR TxBuffer, DWORD TxLength,
	PUCHAR RxBuffer, PDWORD RxLength, /*@unused@*/ PSCARD_IO_HEADER RecvPci)
{
	/*
	 * This function performs an APDU exchange with the card/slot
	 * specified by Lun.  The driver is responsible for performing any
	 * protocol specific exchanges such as T=0/1 ... differences.  Calling
	 * this function will abstract all protocol differences.
	 *
	 * SendPci Protocol - 0, 1, .... 14 Length - Not used.
	 *
	 * TxBuffer - Transmit APDU example (0x00 0xA4 0x00 0x00 0x02 0x3F
	 * 0x00) TxLength - Length of this buffer. RxBuffer - Receive APDU
	 * example (0x61 0x14) RxLength - Length of the received APDU.  This
	 * function will be passed the size of the buffer of RxBuffer and this
	 * function is responsible for setting this to the length of the
	 * received APDU.  This should be ZERO on all errors.  The resource
	 * manager will take responsibility of zeroing out any temporary APDU
	 * buffers for security reasons.
	 *
	 * RecvPci Protocol - 0, 1, .... 14 Length - Not used.
	 *
	 * Notes: The driver is responsible for knowing what type of card it
	 * has.  If the current slot/card contains a memory card then this
	 * command should ignore the Protocol and use the MCT style commands
	 * for support for these style cards and transmit them appropriately.
	 * If your reader does not support memory cards or you don't want to
	 * then ignore this.
	 *
	 * RxLength should be set to zero on error.
	 *
	 * returns:
	 *
	 * IFD_SUCCESS IFD_COMMUNICATION_ERROR IFD_RESPONSE_TIMEOUT
	 * IFD_ICC_NOT_PRESENT IFD_PROTOCOL_NOT_SUPPORTED
	 */

	RESPONSECODE return_value;
	unsigned int rx_length;
	int reader_index;
	int slot_index;
	_ccid_descriptor *ccid_descriptor;
	unsigned char pcbuffer[SIZE_GET_SLOT_STATUS];

	(void)RecvPci;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	DEBUG_INFO3("%s (lun: %X)", CcidSlots[reader_index].readerName, Lun);

	ccid_descriptor = get_ccid_descriptor(reader_index);
	slot_index = ccid_descriptor->bCurrentSlotIndex;

	// Fix reader hang problem by checking card status of ACR85 PICC before exchanging APDU
	if ((ccid_descriptor->readerID == ACS_ACR85_PINPAD_READER_PICC) &&
		(ccid_descriptor->firmwareFixEnabled))
	{
		if (CcidSlots[reader_index].pGetSlotStatus(reader_index, pcbuffer) == IFD_SUCCESS)
		{
			if ((pcbuffer[7] & CCID_ICC_STATUS_MASK) == CCID_ICC_ABSENT)
			{
				return_value = IFD_ICC_NOT_PRESENT;
				goto err;
			}
		}
	}
	// Add BSI miscellaneous command for ACR1281 BSI
	else if ((ccid_descriptor->readerID == ACS_ACR1281_PICC_READER_BSI) ||
		(ccid_descriptor->readerID == ACS_ACR1281_DUAL_READER_BSI))
	{
		if ((TxLength >= 4) &&
			(memcmp(TxBuffer, "\xFF\x9A\x01\x07", 4) == 0))
		{
			unsigned int versionLen = strlen(PACKAGE_VERSION);
			unsigned int Lc = 0;
			unsigned int Le = 0;
			unsigned int dataLen;

			// Check APDU
			if (TxLength == 5)
				Le = TxBuffer[4];
			else if (TxLength > 5)
			{
				Lc = TxBuffer[4];

				dataLen = TxLength - 5;
				if (dataLen == Lc + 1)
					Le = TxBuffer[TxLength - 1];
				else if ((dataLen < Lc) || (dataLen > Lc + 1))
				{
					// Length error: 67 00
					rx_length = 2;
					if (*RxLength < rx_length)
						return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
					else
					{
						RxBuffer[rx_length - 2] = 0x67;
						RxBuffer[rx_length - 1] = 0x00;
						return_value = IFD_SUCCESS;
					}

					goto err;
				}
			}

			if ((Le != 0) && (Le < versionLen))
			{
				// Length Le error: 6C XX
				rx_length = 2;
				if (*RxLength < rx_length)
					return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
				else
				{
					RxBuffer[rx_length - 2] = 0x6C;
					RxBuffer[rx_length - 1] = (unsigned char) versionLen;
					return_value = IFD_SUCCESS;
				}
			}
			else
			{
				rx_length = versionLen + 2;
				if (*RxLength < rx_length)
					return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
				else
				{
					// Return driver version
					memcpy(RxBuffer, PACKAGE_VERSION, versionLen);
					RxBuffer[rx_length - 2] = 0x90;
					RxBuffer[rx_length - 1] = 0x00;
					return_value = IFD_SUCCESS;
				}
			}

			goto err;
		}
	}

	rx_length = *RxLength;
	return_value = CcidSlots[reader_index].pXfrBlock(reader_index, TxLength, TxBuffer, &rx_length,
		RxBuffer, SendPci.Protocol);

	if (IFD_SUCCESS == return_value)
	{
		// Check card status of ACR85 PICC if SW1SW2 "63 00" is received
		if ((ccid_descriptor->readerID == ACS_ACR85_PINPAD_READER_PICC) &&
			(ccid_descriptor->firmwareFixEnabled))
		{
			if ((rx_length >= 2) &&
				(RxBuffer[0] == 0x63) &&
				(RxBuffer[1] == 0x00))
			{
				if (CcidSlots[reader_index].pGetSlotStatus(reader_index, pcbuffer) == IFD_SUCCESS)
				{
					if ((pcbuffer[7] & CCID_ICC_STATUS_MASK) == CCID_ICC_ABSENT)
						return_value = IFD_ICC_NOT_PRESENT;
				}
			}
		}
	}

err:
	if (IFD_SUCCESS == return_value)
		*RxLength = rx_length;
	else
		*RxLength = 0;

	return return_value;
} /* IFDHTransmitToICC */


EXTERNAL RESPONSECODE IFDHControl(DWORD Lun, DWORD dwControlCode,
	PUCHAR TxBuffer, DWORD TxLength, PUCHAR RxBuffer, DWORD RxLength,
	PDWORD pdwBytesReturned)
{
	/*
	 * This function performs a data exchange with the reader (not the
	 * card) specified by Lun.  Here XXXX will only be used. It is
	 * responsible for abstracting functionality such as PIN pads,
	 * biometrics, LCD panels, etc.  You should follow the MCT, CTBCS
	 * specifications for a list of accepted commands to implement.
	 *
	 * TxBuffer - Transmit data TxLength - Length of this buffer. RxBuffer
	 * - Receive data RxLength - Length of the received data.  This
	 * function will be passed the length of the buffer RxBuffer and it
	 * must set this to the length of the received data.
	 *
	 * Notes: RxLength should be zero on error.
	 */
	RESPONSECODE return_value = IFD_ERROR_NOT_SUPPORTED;
	int reader_index;
	int old_read_timeout;
	_ccid_descriptor *ccid_descriptor;

	reader_index = LunToReaderIndex(Lun);
	if ((-1 == reader_index) || (NULL == pdwBytesReturned))
		return IFD_COMMUNICATION_ERROR;

	ccid_descriptor = get_ccid_descriptor(reader_index);

	DEBUG_INFO4("ControlCode: 0x%X, %s (lun: %X)", dwControlCode,
		CcidSlots[reader_index].readerName, Lun);
	DEBUG_INFO_XXD("Control TxBuffer: ", TxBuffer, TxLength);

	/* Set the return length to 0 to avoid problems */
	*pdwBytesReturned = 0;

	if (ccid_descriptor->bInterfaceProtocol == PROTOCOL_ACR38)
	{
		// ACR38U, ACR38U-SAM and SCR21U specific I/O controls
		if ((ACS_ACR38U == ccid_descriptor -> readerID) ||
			(ACS_ACR38U_SAM == ccid_descriptor -> readerID) ||
			(IRIS_SCR21U == ccid_descriptor -> readerID) ||
			(ACS_AET65_1SAM_ICC_READER == ccid_descriptor -> readerID))
		{
			// Set card voltage
			if (IOCTL_SMARTCARD_SET_CARD_VOLTAGE == dwControlCode)
			{
				unsigned int iBytesReturned;

				iBytesReturned = RxLength;
				return_value = ACR38_SetCardVoltage(reader_index, TxBuffer, TxLength,
					RxBuffer, &iBytesReturned);
				*pdwBytesReturned = iBytesReturned;
			}

			// Set card type
			if (IOCTL_SMARTCARD_SET_CARD_TYPE == dwControlCode)
			{
				unsigned int iBytesReturned;

				iBytesReturned = RxLength;
				return_value = ACR38_SetCardType(reader_index, TxBuffer, TxLength,
					RxBuffer, &iBytesReturned);
				*pdwBytesReturned = iBytesReturned;
			}
		}
	}
	else
	{
		if (IOCTL_SMARTCARD_VENDOR_IFD_EXCHANGE == dwControlCode)
		{
			if (FALSE == (DriverOptions & DRIVER_OPTION_CCID_EXCHANGE_AUTHORIZED))
			{
				DEBUG_INFO("ifd exchange (Escape command) not allowed");
				return_value = IFD_COMMUNICATION_ERROR;
			}
			else
			{
				unsigned int iBytesReturned;

				iBytesReturned = RxLength;
				old_read_timeout = ccid_descriptor -> readTimeout;
				ccid_descriptor -> readTimeout = 0;	// Infinite
				return_value = CmdEscape(reader_index, TxBuffer, TxLength, RxBuffer,
					&iBytesReturned);
				ccid_descriptor -> readTimeout = old_read_timeout;
				*pdwBytesReturned = iBytesReturned;
			}
		}

		/* Implement the PC/SC v2.02.05 Part 10 IOCTL mechanism */

		/* Query for features */
		if (CM_IOCTL_GET_FEATURE_REQUEST == dwControlCode)
		{
			unsigned int iBytesReturned = 0;
			PCSC_TLV_STRUCTURE *pcsc_tlv = (PCSC_TLV_STRUCTURE *)RxBuffer;

			/* we need room for up to for records */
			if (RxLength < 4 * sizeof(PCSC_TLV_STRUCTURE))
				return IFD_COMMUNICATION_ERROR;

			/* We can only support direct verify and/or modify currently */
			if (ccid_descriptor -> bPINSupport & CCID_CLASS_PIN_VERIFY)
			{
				pcsc_tlv -> tag = FEATURE_VERIFY_PIN_DIRECT;
				pcsc_tlv -> length = 0x04; /* always 0x04 */
				pcsc_tlv -> value = htonl(IOCTL_FEATURE_VERIFY_PIN_DIRECT);

				pcsc_tlv++;
				iBytesReturned += sizeof(PCSC_TLV_STRUCTURE);
			}

			if (ccid_descriptor -> bPINSupport & CCID_CLASS_PIN_MODIFY)
			{
				pcsc_tlv -> tag = FEATURE_MODIFY_PIN_DIRECT;
				pcsc_tlv -> length = 0x04; /* always 0x04 */
				pcsc_tlv -> value = htonl(IOCTL_FEATURE_MODIFY_PIN_DIRECT);

				pcsc_tlv++;
				iBytesReturned += sizeof(PCSC_TLV_STRUCTURE);
			}

#ifdef FEATURE_IFD_PIN_PROPERTIES
			/* We can always forward wLcdLayout */
			pcsc_tlv -> tag = FEATURE_IFD_PIN_PROPERTIES;
			pcsc_tlv -> length = 0x04; /* always 0x04 */
			pcsc_tlv -> value = htonl(IOCTL_FEATURE_IFD_PIN_PROPERTIES);

			pcsc_tlv++;
			iBytesReturned += sizeof(PCSC_TLV_STRUCTURE);
#endif

			if (KOBIL_TRIBANK == ccid_descriptor -> readerID)
			{
				pcsc_tlv -> tag = FEATURE_MCT_READERDIRECT;
				pcsc_tlv -> length = 0x04; /* always 0x04 */
				pcsc_tlv -> value = htonl(IOCTL_FEATURE_MCT_READERDIRECT);

				pcsc_tlv++;
				iBytesReturned += sizeof(PCSC_TLV_STRUCTURE);
			}

			// APG8201Z supports vendor specific feature
			if (ACS_APG8201Z == ccid_descriptor -> readerID)
			{
				pcsc_tlv -> tag = 0x80;
				pcsc_tlv -> length = 0x04; /* always 0x04 */
				pcsc_tlv -> value = 0;

				pcsc_tlv++;
				iBytesReturned += sizeof(PCSC_TLV_STRUCTURE);
			}

			*pdwBytesReturned = iBytesReturned;
			return_value = IFD_SUCCESS;
		}

#ifdef FEATURE_IFD_PIN_PROPERTIES
		/* Get PIN handling capabilities */
		if (IOCTL_FEATURE_IFD_PIN_PROPERTIES == dwControlCode)
		{
			// pcsc-lite 1.4.x header files: missing PIN_PROPERTIES_STRUCTURE
			// pcsc-lite 1.5.x header files: incorrect PIN_PROPERTIES_STRUCTURE
			unsigned int wLcdLayout = ccid_descriptor -> wLcdLayout;

			if (RxLength >= 4)
			{
				RxBuffer[0] = wLcdLayout & 0xFF;
				RxBuffer[1] = (wLcdLayout >> 8) & 0xFF;
				RxBuffer[2] = 0x07;
				RxBuffer[3] = 0;

				*pdwBytesReturned = 4;
				return_value = IFD_SUCCESS;
			}
		}
#endif

		/* Verify a PIN, plain CCID */
		if (IOCTL_FEATURE_VERIFY_PIN_DIRECT == dwControlCode)
		{
			unsigned int iBytesReturned;

			iBytesReturned = RxLength;
			return_value = SecurePINVerify(reader_index, TxBuffer, TxLength,
				RxBuffer, &iBytesReturned);
			*pdwBytesReturned = iBytesReturned;
		}

		/* Modify a PIN, plain CCID */
		if (IOCTL_FEATURE_MODIFY_PIN_DIRECT == dwControlCode)
		{
			unsigned int iBytesReturned;

			iBytesReturned = RxLength;
			return_value = SecurePINModify(reader_index, TxBuffer, TxLength,
				RxBuffer, &iBytesReturned);
			*pdwBytesReturned = iBytesReturned;
		}

		/* MCT: Multifunctional Card Terminal */
		if (IOCTL_FEATURE_MCT_READERDIRECT == dwControlCode)
		{
			if ( (TxBuffer[0] != 0x20)	/* CLA */
				|| ((TxBuffer[1] & 0xF0) != 0x70)	/* INS */
				/* valid INS are
				 * 0x70: SECODER INFO
				 * 0x71: SECODER SELECT APPLICATION
				 * 0x72: SECODER APPLICATION ACTIVE
				 * 0x73: SECODER DATA CONFIRMATION
				 * 0x74: SECODER PROCESS AUTHENTICATION TOKEN */
				|| ((TxBuffer[1] & 0x0F) > 4)
				|| (TxBuffer[2] != 0x00)	/* P1 */
				|| (TxBuffer[3] != 0x00)	/* P2 */
				|| (TxBuffer[4] != 0x00)	/* Lind */
			   )
			{
				DEBUG_INFO("MCT Command refused by driver");
				return_value = IFD_COMMUNICATION_ERROR;
			}
			else
			{
				unsigned int iBytesReturned;

				/* we just transmit the buffer as a CCID Escape command */
				iBytesReturned = RxLength;
				old_read_timeout = ccid_descriptor -> readTimeout;
				ccid_descriptor -> readTimeout = 0;	// Infinite
				return_value = CmdEscape(reader_index, TxBuffer, TxLength, RxBuffer,
					&iBytesReturned);
				ccid_descriptor -> readTimeout = old_read_timeout;
				*pdwBytesReturned = iBytesReturned;
			}
		}

		// MS CCID I/O control code for escape command
		if (IOCTL_CCID_ESCAPE == dwControlCode)
		{
			unsigned int iBytesReturned;

			iBytesReturned = RxLength;
			old_read_timeout = ccid_descriptor -> readTimeout;
			ccid_descriptor -> readTimeout = 0;	// Infinite
			return_value = CmdEscape(reader_index, TxBuffer, TxLength, RxBuffer,
				&iBytesReturned);
			ccid_descriptor -> readTimeout = old_read_timeout;
			*pdwBytesReturned = iBytesReturned;
		}

		// ACR83U, ACR85, APG8201 and APG8201Z specific I/O controls
		if ((ACS_ACR83U == ccid_descriptor -> readerID) ||
			(ACS_ACR85_PINPAD_READER_ICC == ccid_descriptor -> readerID) ||
			(ACS_APG8201 == ccid_descriptor -> readerID) ||
			(ACS_APG8201Z == ccid_descriptor -> readerID))
		{
			// Get firmware version
			if (IOCTL_SMARTCARD_GET_FIRMWARE_VERSION == dwControlCode)
			{
				unsigned char command[] = { 0x04, 0x00, 0x00, 0x00, 0x00 };
				unsigned int commandLen = sizeof(command);
				unsigned char response[3 + 6];
				unsigned int responseLen = sizeof(response);

				return_value = CmdEscape(reader_index, command, commandLen, response, &responseLen);
				if (return_value == IFD_SUCCESS)
				{
					if ((responseLen > 3) && (response[0] == 0x84))
					{
						*pdwBytesReturned = responseLen - 3;
						if (RxLength < *pdwBytesReturned)
							return_value = IFD_COMMUNICATION_ERROR;
						else
							memcpy(RxBuffer, response + 3, *pdwBytesReturned);
					}
					else
						return_value = IFD_COMMUNICATION_ERROR;
				}
			}

			// Display LCD message
			if (IOCTL_SMARTCARD_DISPLAY_LCD_MESSAGE == dwControlCode)
			{
				unsigned char command[5 + 32] = { 0x05, 0x00, 0x20, 0x00, 0x00 };
				unsigned int commandLen = sizeof(command);
				unsigned char response[3 + 2];
				unsigned int responseLen = sizeof(response);

				if ((TxLength > 0) && (TxLength <= 32))
				{
					// Fill memory with spaces
					memset(command + 5, 0x20, 32);

					// Copy message to command
					memcpy(command + 5, TxBuffer, TxLength);

					return_value = CmdEscape(reader_index, command, commandLen, response, &responseLen);
					if (return_value == IFD_SUCCESS)
					{
						if ((responseLen > 3) && (response[0] == 0x85))
						{
							*pdwBytesReturned = responseLen - 3;
							if (RxLength < *pdwBytesReturned)
								return_value = IFD_COMMUNICATION_ERROR;
							else
								memcpy(RxBuffer, response + 3, *pdwBytesReturned);
						}
						else
							return_value = IFD_COMMUNICATION_ERROR;
					}
				}
			}

			// Read key
			if (IOCTL_SMARTCARD_READ_KEY == dwControlCode)
			{
				unsigned char command[5 + 6] = { 0x06, 0x00, 0x06, 0x00, 0x00 };
				unsigned int commandLen = sizeof(command);
				unsigned char response[3 + 35];
				unsigned int responseLen = sizeof(response);

				if (TxLength == 6)
				{
					// Copy message to command
					memcpy(command + 5, TxBuffer, TxLength);

					old_read_timeout = ccid_descriptor -> readTimeout;
					ccid_descriptor -> readTimeout = 0;	// Infinite
					return_value = CmdEscape(reader_index, command, commandLen, response, &responseLen);
					ccid_descriptor -> readTimeout = old_read_timeout;
					if (return_value == IFD_SUCCESS)
					{
						if ((responseLen > 3) && (response[0] == 0x86))
						{
							*pdwBytesReturned = responseLen - 3;
							if (RxLength < *pdwBytesReturned)
								return_value = IFD_COMMUNICATION_ERROR;
							else
								memcpy(RxBuffer, response + 3, *pdwBytesReturned);
						}
						else
							return_value = IFD_COMMUNICATION_ERROR;
					}
				}
			}
		}
		else
		{
			// ACR128 I/O control code for escape command
			if (IOCTL_ACR128_READER_COMMAND == dwControlCode)
			{
				unsigned char *command;
				unsigned int commandLen = 3 + TxLength;
				unsigned int iBytesReturned;

				// Allocate command
				command = (unsigned char *) malloc(commandLen);
				if (command == NULL)
					return_value = IFD_COMMUNICATION_ERROR;
				else
				{
					// Fill command header
					command[0] = 0xE0;
					command[1] = 0x00;
					command[2] = 0x00;

					// Copy command
					memcpy(command + 3, TxBuffer, TxLength);

					iBytesReturned = RxLength;
					old_read_timeout = ccid_descriptor -> readTimeout;
					ccid_descriptor -> readTimeout = 0;	// Infinite
					return_value = CmdEscape(reader_index, command, commandLen, RxBuffer,
						&iBytesReturned);
					ccid_descriptor -> readTimeout = old_read_timeout;
					*pdwBytesReturned = iBytesReturned;

					// Free command
					free(command);
				}
			}
		}
	}

	if (IFD_SUCCESS != return_value)
		*pdwBytesReturned = 0;

	DEBUG_INFO_XXD("Control RxBuffer: ", RxBuffer, *pdwBytesReturned);
	return return_value;
} /* IFDHControl */


EXTERNAL RESPONSECODE IFDHICCPresence(DWORD Lun)
{
	/*
	 * This function returns the status of the card inserted in the
	 * reader/slot specified by Lun.  It will return either:
	 *
	 * returns: IFD_ICC_PRESENT IFD_ICC_NOT_PRESENT
	 * IFD_COMMUNICATION_ERROR
	 */

	unsigned char pcbuffer[SIZE_GET_SLOT_STATUS];
	RESPONSECODE return_value = IFD_COMMUNICATION_ERROR;
	int oldLogLevel;
	int reader_index;
	int slot_index;
	_ccid_descriptor *ccid_descriptor;
	unsigned int oldReadTimeout;

	if (-1 == (reader_index = LunToReaderIndex(Lun)))
		return IFD_COMMUNICATION_ERROR;

	DEBUG_PERIODIC3("%s (lun: %X)", CcidSlots[reader_index].readerName, Lun);

	ccid_descriptor = get_ccid_descriptor(reader_index);

	// Get slot index
	slot_index = ccid_descriptor->bCurrentSlotIndex;

	// Return dwSlotstatus if it is a SAM slot or reader is GEMCORESIMPRO
	if ((ccid_descriptor->isSamSlot) ||
		(GEMCORESIMPRO == ccid_descriptor->readerID))
	{
		return_value = ccid_descriptor->dwSlotStatus;
		goto end;
	}

#ifndef __APPLE__
	// ACR1222 reader is required to read data from interrupt endpoint
	if ((ACS_ACR85_PINPAD_READER_PICC == ccid_descriptor->readerID) ||
		(ACS_ACR1222_DUAL_READER == ccid_descriptor->readerID) ||
		(ACS_ACR1222_1SAM_DUAL_READER == ccid_descriptor->readerID))
	{
		InterruptRead(reader_index, 10);
	}
#endif

	/* save the current read timeout computed from card capabilities */
	oldReadTimeout = ccid_descriptor->readTimeout;

	/* use default timeout since the reader may not be present anymore */
	ccid_descriptor->readTimeout = DEFAULT_COM_READ_TIMEOUT;

	/* if DEBUG_LEVEL_PERIODIC is not set we remove DEBUG_LEVEL_COMM */
	oldLogLevel = LogLevel;
	if (! (LogLevel & DEBUG_LEVEL_PERIODIC))
		LogLevel &= ~DEBUG_LEVEL_COMM;

	// ACR122U v2.00 - v2.04
	// Simulate bStatus by reading bmSlotIccState from interrupt endpoint
	if ((ccid_descriptor->readerID == ACS_ACR122U) &&
		(ccid_descriptor->bcdDevice >= 0x0200) &&
		(ccid_descriptor->bcdDevice <= 0x0204))
	{
#ifndef __APPLE__
		InterruptRead(reader_index, 100);
#endif
#ifdef __APPLE__
		pthread_mutex_lock(ccid_descriptor->pbStatusLock);
#endif
		// If bStatus is in initial state (0xFF)
		if (ccid_descriptor->bStatus[slot_index] == 0xFF)
		{
			// Get bStatus from GetSlotStatus
			return_value = CcidSlots[reader_index].pGetSlotStatus(reader_index, pcbuffer);
			if (return_value == IFD_SUCCESS)
				ccid_descriptor->bStatus[slot_index] = pcbuffer[7];
		}
		else
		{
			pcbuffer[7] = ccid_descriptor->bStatus[slot_index];
			return_value = IFD_SUCCESS;
		}
#ifdef __APPLE__
		pthread_mutex_unlock(ccid_descriptor->pbStatusLock);
#endif
	}
	// Enable/disable PICC
	else if ((ccid_descriptor->firmwareFixEnabled) &&
		(((ccid_descriptor->readerID == ACS_ACR1222_DUAL_READER) ||
		(ccid_descriptor->readerID == ACS_ACR1222_1SAM_DUAL_READER)) &&
		(ccid_descriptor->bCurrentSlotIndex == 1) ||
		(ccid_descriptor->readerID == ACS_ACR85_PINPAD_READER_PICC)))
	{
		if (*(ccid_descriptor->pPiccEnabled))
		{
			return_value = CcidSlots[reader_index].pGetSlotStatus(reader_index, pcbuffer);
		}
		else
		{
			// Return absent if PICC is disabled
			pcbuffer[7] = CCID_ICC_ABSENT;
			return_value = IFD_SUCCESS;
		}
	}
	else
		return_value = CcidSlots[reader_index].pGetSlotStatus(reader_index, pcbuffer);

	/* set back the old timeout */
	ccid_descriptor->readTimeout = oldReadTimeout;

	/* set back the old LogLevel */
	LogLevel = oldLogLevel;

	if (return_value != IFD_SUCCESS)
		return return_value;

	return_value = IFD_COMMUNICATION_ERROR;
	switch (pcbuffer[7] & CCID_ICC_STATUS_MASK)	/* bStatus */
	{
		case CCID_ICC_PRESENT_ACTIVE:
			return_value = IFD_ICC_PRESENT;
			/* use default slot */
			break;

		case CCID_ICC_PRESENT_INACTIVE:
			if ((CcidSlots[reader_index].bPowerFlags == POWERFLAGS_RAZ)
				|| (CcidSlots[reader_index].bPowerFlags & MASK_POWERFLAGS_PDWN))
				/* the card was previously absent */
				return_value = IFD_ICC_PRESENT;
			else
			{
				/* the card was previously present but has been
				 * removed and inserted between two consecutive
				 * IFDHICCPresence() calls */
				CcidSlots[reader_index].bPowerFlags = POWERFLAGS_RAZ;
				return_value = IFD_ICC_NOT_PRESENT;
			}
			break;

		case CCID_ICC_ABSENT:
			/* Reset ATR buffer */
			CcidSlots[reader_index].nATRLength = 0;
			*CcidSlots[reader_index].pcATRBuffer = '\0';

			/* Reset PowerFlags */
			CcidSlots[reader_index].bPowerFlags = POWERFLAGS_RAZ;

			return_value = IFD_ICC_NOT_PRESENT;
			break;
	}

#if 0
	/* SCR331-DI contactless reader */
	if (((SCR331DI == ccid_descriptor->readerID)
		|| (SDI010 == ccid_descriptor->readerID)
		|| (SCR331DINTTCOM == ccid_descriptor->readerID))
		&& (ccid_descriptor->bCurrentSlotIndex > 0))
	{
		unsigned char cmd[] = { 0x11 };
		/*  command: 11 ??
		 * response: 00 11 01 ?? no card
		 *           01 04 00 ?? card present */

		unsigned char res[10];
		unsigned int length_res = sizeof(res);
		RESPONSECODE ret;

		/* if DEBUG_LEVEL_PERIODIC is not set we remove DEBUG_LEVEL_COMM */
		oldLogLevel = LogLevel;
		if (! (LogLevel & DEBUG_LEVEL_PERIODIC))
			LogLevel &= ~DEBUG_LEVEL_COMM;

		ret = CmdEscape(reader_index, cmd, sizeof(cmd), res, &length_res);

		/* set back the old LogLevel */
		LogLevel = oldLogLevel;

		if (ret != IFD_SUCCESS)
		{
			DEBUG_INFO("CmdEscape failed");
			/* simulate a card absent */
			res[0] = 0;
		}

		if (0x01 == res[0])
			return_value = IFD_ICC_PRESENT;
		else
		{
			/* Reset ATR buffer */
			CcidSlots[reader_index].nATRLength = 0;
			*CcidSlots[reader_index].pcATRBuffer = '\0';

			/* Reset PowerFlags */
			CcidSlots[reader_index].bPowerFlags = POWERFLAGS_RAZ;

			return_value = IFD_ICC_NOT_PRESENT;
		}
	}
#endif

	// Enable/disable PICC
	if (DriverOptions & DRIVER_OPTION_DISABLE_PICC)
	{
		if ((ccid_descriptor->firmwareFixEnabled) &&
			(((ccid_descriptor->readerID == ACS_ACR1222_DUAL_READER) ||
			(ccid_descriptor->readerID == ACS_ACR1222_1SAM_DUAL_READER)) &&
			(ccid_descriptor->bCurrentSlotIndex == 0) ||
			(ccid_descriptor->readerID == ACS_ACR85_PINPAD_READER_ICC)))
		{
			int piccReaderIndex = *(ccid_descriptor->pPiccReaderIndex);
			if (piccReaderIndex >= 0)
			{
				// If card is present in ICC slot
				if (return_value == IFD_ICC_PRESENT)
				{
					// Disable PICC
					if (*(ccid_descriptor->pPiccEnabled))
					{
						DEBUG_INFO("Disabling PICC...");
						EnablePicc(piccReaderIndex, FALSE);
						*(ccid_descriptor->pPiccEnabled) = FALSE;
					}
				}
				else
				{
					if (!*(ccid_descriptor->pPiccEnabled))
					{
						// Enable PICC
						DEBUG_INFO("Enabling PICC...");
						EnablePicc(piccReaderIndex, TRUE);
						*(ccid_descriptor->pPiccEnabled) = TRUE;
					}
				}
			}
		}
	}

end:
	DEBUG_PERIODIC2("Card %s",
		IFD_ICC_PRESENT == return_value ? "present" : "absent");

	return return_value;
} /* IFDHICCPresence */


CcidDesc *get_ccid_slot(unsigned int reader_index)
{
	return &CcidSlots[reader_index];
} /* get_ccid_slot */


void init_driver(void)
{
	char keyValue[TOKEN_MAX_VALUE_SIZE];
	char infofile[FILENAME_MAX];
	char *e;

	DEBUG_INFO("Driver version: " VERSION);

	/* Info.plist full patch filename */
	(void)snprintf(infofile, sizeof(infofile), "%s/%s/Contents/Info.plist",
		PCSCLITE_HP_DROPDIR, BUNDLE);

	/* Log level */
	if (0 == LTPBundleFindValueWithKey(infofile, "ifdLogLevel", keyValue, 0))
	{
		/* convert from hex or dec or octal */
		LogLevel = strtoul(keyValue, NULL, 0);

		/* print the log level used */
		DEBUG_INFO2("LogLevel: 0x%.4X", LogLevel);
	}

	e = getenv("LIBCCID_ifdLogLevel");
	if (e)
	{
		/* convert from hex or dec or octal */
		LogLevel = strtoul(e, NULL, 0);

		/* print the log level used */
		DEBUG_INFO2("LogLevel from LIBCCID_ifdLogLevel: 0x%.4X", LogLevel);
	}

	/* Driver options */
	if (0 == LTPBundleFindValueWithKey(infofile, "ifdDriverOptions", keyValue, 0))
	{
		/* convert from hex or dec or octal */
		DriverOptions = strtoul(keyValue, NULL, 0);

		/* print the log level used */
		DEBUG_INFO2("DriverOptions: 0x%.4X", DriverOptions);
	}

	/* get the voltage parameter */
	switch ((DriverOptions >> 4) & 0x03)
	{
		case 0:
			PowerOnVoltage = VOLTAGE_5V;
			break;

		case 1:
			PowerOnVoltage = VOLTAGE_3V;
			break;

		case 2:
			PowerOnVoltage = VOLTAGE_1_8V;
			break;

		case 3:
			PowerOnVoltage = VOLTAGE_AUTO;
			break;
	}

	// Card voltage selection for ACR38U, ACR38U-SAM and SCR21U
	if (0 == LTPBundleFindValueWithKey(infofile, "ifdACR38CardVoltage", keyValue, 0))
	{
		ACR38CardVoltage = strtoul(keyValue, NULL, 0);
		DEBUG_INFO2("ACR38CardVoltage: %d", ACR38CardVoltage);
	}

	// Card type selection for ACR38U, ACR38U-SAM and SCR21U
	if (0 == LTPBundleFindValueWithKey(infofile, "ifdACR38CardType", keyValue, 0))
	{
		ACR38CardType = strtoul(keyValue, NULL, 0);
		DEBUG_INFO2("ACR38CardType: %d", ACR38CardType);
	}

	/* initialise the Lun to reader_index mapping */
	InitReaderIndex();

	DebugInitialized = TRUE;
} /* init_driver */


void extra_egt(ATR_t *atr, _ccid_descriptor *ccid_desc, DWORD Protocol)
{
	/* This function use an EGT value for cards who comply with followings
	 * criterias:
	 * - TA1 > 11
	 * - current EGT = 0x00 or 0xFF
	 * - T=0 or (T=1 and CWI >= 2)
	 *
	 * Without this larger EGT some non ISO 7816-3 smart cards may not
	 * communicate with the reader.
	 *
	 * This modification is harmless, the reader will just be less restrictive
	 */

	unsigned int card_baudrate;
	unsigned int default_baudrate;
	double f, d;
	int i;

	/* if TA1 not present */
	if (! atr->ib[0][ATR_INTERFACE_BYTE_TA].present)
		return;

	(void)ATR_GetParameter(atr, ATR_PARAMETER_D, &d);
	(void)ATR_GetParameter(atr, ATR_PARAMETER_F, &f);

	/* may happen with non ISO cards */
	if ((0 == f) || (0 == d))
		return;

	/* Baudrate = f x D/F */
	card_baudrate = (unsigned int) (1000 * ccid_desc->dwDefaultClock * d / f);

	default_baudrate = (unsigned int) (1000 * ccid_desc->dwDefaultClock
		* ATR_DEFAULT_D / ATR_DEFAULT_F);

	/* TA1 > 11? */
	if (card_baudrate <= default_baudrate)
		return;

	/* Current EGT = 0 or FF? */
	if (atr->ib[0][ATR_INTERFACE_BYTE_TC].present &&
		((0x00 == atr->ib[0][ATR_INTERFACE_BYTE_TC].value) ||
		(0xFF == atr->ib[0][ATR_INTERFACE_BYTE_TC].value)))
	{
		if (SCARD_PROTOCOL_T0 == Protocol)
		{
			/* Init TC1 */
			atr->ib[0][ATR_INTERFACE_BYTE_TC].present = TRUE;
			atr->ib[0][ATR_INTERFACE_BYTE_TC].value = 2;
			DEBUG_INFO("Extra EGT patch applied");
		}

		if (SCARD_PROTOCOL_T1 == Protocol)
		{
			/* TBi (i>2) present? BWI/CWI */
			for (i=2; i<ATR_MAX_PROTOCOLS; i++)
			{
				/* CWI >= 2 ? */
				if (atr->ib[i][ATR_INTERFACE_BYTE_TB].present &&
					((atr->ib[i][ATR_INTERFACE_BYTE_TB].value & 0x0F) >= 2))
				{
					/* Init TC1 */
					atr->ib[0][ATR_INTERFACE_BYTE_TC].present = TRUE;
					atr->ib[0][ATR_INTERFACE_BYTE_TC].value = 2;
					DEBUG_INFO("Extra EGT patch applied");

					/* only the first TBi (i>2) must be used */
					break;
				}
			}
		}
	}
} /* extra_egt */


static char find_baud_rate(unsigned int baudrate, unsigned int *list)
{
	int i;

	DEBUG_COMM2("Card baud rate: %d", baudrate);

	/* Does the reader support the announced smart card data speed? */
	for (i=0;; i++)
	{
		/* end of array marker */
		if (0 == list[i])
			break;

		DEBUG_COMM2("Reader can do: %d", list[i]);

		/* We must take into account that the card_baudrate integral value
		 * is an approximative result, computed from the d/f float result.
		 */
		if ((baudrate < list[i] + 2) && (baudrate > list[i] - 2))
			return TRUE;
	}

	return FALSE;
} /* find_baud_rate */


static unsigned int T0_card_timeout(double f, double d, int TC1, int TC2,
	int clock_frequency)
{
	unsigned int timeout = DEFAULT_COM_READ_TIMEOUT;
	double EGT, WWT;
	unsigned int t;

	/* Timeout applied on ISO_IN or ISO_OUT card exchange
	 * we choose the maximum computed value.
	 *
	 * ISO_IN timeout is the sum of:
	 * Terminal:					Smart card:
	 * 5 bytes header cmd  ->
	 *                    <-		Procedure byte
	 * 256 data bytes	   ->
	 * 					  <-		SW1-SW2
	 * = 261 EGT       + 3 WWT     + 3 WWT
	 *
	 * ISO_OUT Timeout is the sum of:
	 * Terminal:                    Smart card:
	 * 5 bytes header cmd  ->
	 * 					  <-        Procedure byte + 256 data bytes + SW1-SW2
	 * = 5 EGT          + 1 WWT     + 259 WWT
	 */

	/* clock_frequency is in kHz so the times are in milliseconds and not
	 * in seconds */

	/* may happen with non ISO cards */
	if ((0 == f) || (0 == d) || (0 == clock_frequency))
		return 60;	/* 60 seconds */

	/* EGT */
	/* see ch. 6.5.3 Extra Guard Time, page 12 of ISO 7816-3 */
	EGT = 12 * f / d / clock_frequency + (f / d) * TC1 / clock_frequency;

	/* card WWT */
	/* see ch. 8.2 Character level, page 15 of ISO 7816-3 */
	WWT = 960 * TC2 * f / clock_frequency;

	/* ISO in */
	t  = 261 * EGT + (3 + 3) * WWT;
	/* Convert from milliseonds to seconds rouned to the upper value
	 * use +1 instead of ceil() to round up to the nearest integer
	 * so we can avoid a dependency on the math library */
	t = t/1000 +1;
	if (timeout < t)
		timeout = t;

	/* ISO out */
	t = 5 * EGT + (1 + 259) * WWT;
	t = t/1000 +1;
	if (timeout < t)
		timeout = t;

	return timeout;
} /* T0_card_timeout  */


static unsigned int T1_card_timeout(double f, double d, int TC1,
	int BWI, int CWI, int clock_frequency)
{
	double EGT, BWT, CWT, etu;
	unsigned int timeout;

	/* Timeout applied on ISO in + ISO out card exchange
	 *
     * Timeout is the sum of:
	 * - ISO in delay between leading edge of the first character sent by the
	 *   interface device and the last one (NAD PCB LN APDU CKS) = 260 EGT,
	 * - delay between ISO in and ISO out = BWT,
	 * - ISO out delay between leading edge of the first character sent by the
	 *   card and the last one (NAD PCB LN DATAS CKS) = 260 CWT.
	 */

	/* clock_frequency is in kHz so the times are in milliseconds and not
	 * in seconds */

	/* may happen with non ISO cards */
	if ((0 == f) || (0 == d) || (0 == clock_frequency))
		return 60;	/* 60 seconds */

	/* see ch. 6.5.2 Transmission factors F and D, page 12 of ISO 7816-3 */
	etu = f / d / clock_frequency;

	/* EGT */
	/* see ch. 6.5.3 Extra Guard Time, page 12 of ISO 7816-3 */
	EGT = 12 * etu + (f / d) * TC1 / clock_frequency;

	/* card BWT */
	/* see ch. 9.5.3.2 Block Waiting Time, page 20 of ISO 7816-3 */
	BWT = 11 * etu + (1<<BWI) * 960 * 372 / clock_frequency;

	/* card CWT */
	/* see ch. 9.5.3.1 Caracter Waiting Time, page 20 of ISO 7816-3 */
	CWT = (11 + (1<<CWI)) * etu;

	timeout = 260*EGT + BWT + 260*CWT;

	/* Convert from milliseonds to seconds rounded to the upper value
	 * we use +1 instead of ceil() to round up to the nearest greater integer
	 * so we can avoid a dependency on the math library */
	timeout = timeout/1000 +1;

	return timeout;
} /* T1_card_timeout  */


static int get_IFSC(ATR_t *atr, int *idx)
{
	int i, ifsc, protocol = -1;

	/* default return values */
	ifsc = -1;
	*idx = -1;

	for (i=0; i<ATR_MAX_PROTOCOLS; i++)
	{
		/* TAi (i>2) present and protocol=1 => IFSC */
		if (i >= 2 && protocol == 1
			&& atr->ib[i][ATR_INTERFACE_BYTE_TA].present)
		{
			ifsc = atr->ib[i][ATR_INTERFACE_BYTE_TA].value;
			*idx = i+1;
			/* only the first TAi (i>2) must be used */
			break;
		}

		/* protocol T=? */
		if (atr->ib[i][ATR_INTERFACE_BYTE_TD].present)
			protocol = atr->ib[i][ATR_INTERFACE_BYTE_TD].value & 0x0F;
	}

	if (ifsc > 254)
	{
		/* 0xFF is not a valid value for IFSC */
		DEBUG_INFO2("Non ISO IFSC: 0x%X", ifsc);
		ifsc = 254;
	}

	return ifsc;
} /* get_IFSC */

