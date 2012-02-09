/*
    ccid_ifdhandler.h: non-generic ifdhandler functions
    Copyright (C) 2004-2009   Ludovic Rousseau
    Copyright (C) 2010-2012   Advanced Card Systems Ltd.

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
 * $Id: ccid_ifdhandler.h 4279 2009-06-26 14:58:03Z rousseau $
 */

#ifndef _ccid_ifd_handler_h_
#define _ccid_ifd_handler_h_

#define IOCTL_SMARTCARD_VENDOR_IFD_EXCHANGE	SCARD_CTL_CODE(1)

#define CLASS2_IOCTL_MAGIC 0x330000
#define IOCTL_FEATURE_VERIFY_PIN_DIRECT \
	SCARD_CTL_CODE(FEATURE_VERIFY_PIN_DIRECT + CLASS2_IOCTL_MAGIC)
#define IOCTL_FEATURE_MODIFY_PIN_DIRECT \
	SCARD_CTL_CODE(FEATURE_MODIFY_PIN_DIRECT + CLASS2_IOCTL_MAGIC)

// Fix problem using pcsc-lite 1.7.3 or later header files
#ifndef FEATURE_MCT_READERDIRECT
#define FEATURE_MCT_READERDIRECT	FEATURE_MCT_READER_DIRECT
#endif

#define IOCTL_FEATURE_MCT_READERDIRECT \
	SCARD_CTL_CODE(FEATURE_MCT_READERDIRECT + CLASS2_IOCTL_MAGIC)

// Fix problem using pcsc-lite 1.4.x header files
#ifdef FEATURE_IFD_PIN_PROP
#define FEATURE_IFD_PIN_PROPERTIES	FEATURE_IFD_PIN_PROP
#endif

#ifdef FEATURE_IFD_PIN_PROPERTIES
#define IOCTL_FEATURE_IFD_PIN_PROPERTIES \
	SCARD_CTL_CODE(FEATURE_IFD_PIN_PROPERTIES + CLASS2_IOCTL_MAGIC)
#endif

// MS CCID I/O control code for escape command
#define IOCTL_CCID_ESCAPE	SCARD_CTL_CODE(3500)

// ACR83U, ACR85 and APG8201 specific I/O controls
#define IOCTL_SMARTCARD_GET_FIRMWARE_VERSION	SCARD_CTL_CODE(2078)
#define IOCTL_SMARTCARD_DISPLAY_LCD_MESSAGE		SCARD_CTL_CODE(2079)
#define IOCTL_SMARTCARD_READ_KEY				SCARD_CTL_CODE(2080)

// ACR38U, ACR38U-SAM and SCR21U specific I/O controls
#define IOCTL_SMARTCARD_SET_CARD_TYPE		SCARD_CTL_CODE(2060)
#define IOCTL_SMARTCARD_SET_CARD_VOLTAGE	SCARD_CTL_CODE(2074)

// ACR128 I/O control code for escape command
#define IOCTL_ACR128_READER_COMMAND	SCARD_CTL_CODE(2079)

#define DRIVER_OPTION_CCID_EXCHANGE_AUTHORIZED 1
#define DRIVER_OPTION_GEMPC_TWIN_KEY_APDU 2
#define DRIVER_OPTION_USE_BOGUS_FIRMWARE 4
#define DRIVER_OPTION_RESET_ON_CLOSE 8

// ACR1222/ACR85 driver option
#define DRIVER_OPTION_REMOVE_PUPI_FROM_ATR	0x40
#define DRIVER_OPTION_DISABLE_PICC			0x80

extern int DriverOptions;

/*
 * Maximum number of CCID readers supported simultaneously
 *
 * The maximum number of readers is also limited in pcsc-lite (16 by default)
 * see the definition of PCSCLITE_MAX_READERS_CONTEXTS in src/PCSC/pcsclite.h
 */
#define CCID_DRIVER_MAX_READERS 16

/*
 * CCID driver specific functions
 */
CcidDesc *get_ccid_slot(unsigned int reader_index);

#endif

