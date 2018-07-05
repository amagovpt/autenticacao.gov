/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2012 FedICT, Caixa Magica Software
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
/**
 * Pinpad support based on CCID (which uses the SCardControl()
 * function in PCSC).
 * Also supported is the loading of pinpad libraries for those
 * readers who aren't CCID compliant or to offer extra functionality
 * such as specifying the messages to be displayed.
 * See pinpad2.h
 */

#pragma once

#ifndef GENERIC_PINPAD_H
#define GENERIC_PINPAD_H

#include "P15Objects.h"
#include "InternalConst.h"

#include "../common/ByteArray.h"

#include <winscard.h>
#include <iostream>

namespace eIDMW
{
class CContext;

class EIDMW_CAL_API GenericPinpad
{
public:
   //Warning: This ctor shouldn't be used, it's just a workaround for a convoluted Pinpad detection process
   GenericPinpad()
   {
	   m_ulLangCode = 0x0409;
   }

   GenericPinpad(CContext *poContext, SCARDHANDLE hCard,
		const std::string & csReader);

   virtual CByteArray PinCmd(tPinOperation operation,
		const tPin & pin, unsigned char ucPinType,
        const CByteArray & oAPDU, unsigned long & ulRemaining, void *wndGeometry = 0 );

   unsigned int m_fully_built;

protected:
	CByteArray PinpadControl(unsigned long ulControl, const CByteArray & oCmd,
		tPinOperation operation, unsigned char ucPintype,
		const std::string & csPinLabel, void *wndGeometry = 0 );

	void GetFeatureList();

	bool IsGemsafe(CByteArray &atr);

	bool ShowDlg(tPinOperation operation, unsigned char ucPintype,
		const std::string & csPinLabel, const std::string & csReader,
		unsigned long *pulDlgHandle, void *wndGeometry = 0);
	/** To close the dialog opened by PinCmd() */
	void CloseDlg(unsigned long ulDlgHandle);

	unsigned long GetLanguage();

	unsigned char ToFormatString(const tPin & pin);
	unsigned char ToPinBlockString(const tPin & pin);
	unsigned char ToPinLengthFormat(const tPin & pin);
	unsigned char GetMaxPinLen(const tPin & pin);

	// For PIN verify and unblock without PIN change (1 PIN needed)
	virtual CByteArray PinCmd1(tPinOperation operation,
			const tPin & pin, unsigned char ucPinType,
			const CByteArray & oAPDU, unsigned long & ulRemaining, void *wndGeometry = 0 );

	// For PIN change and unblock with PIN change (2 PINs needed)
	virtual CByteArray PinCmd2(tPinOperation operation,
			const tPin & pin, unsigned char ucPinType,
			const CByteArray & oAPDU, unsigned long & ulRemaining, void *wndGeometry = 0 );

	CContext *m_poContext;
	SCARDHANDLE m_hCard;
	std::string m_csReader;
	unsigned short m_usReaderFirmVers;
	bool m_bUsePinpadLib;
	unsigned long m_ulLangCode;

	bool m_bCanVerifyUnlock;  // Can do operations with 1 PIN
	bool m_bCanChangeUnlock;  // Can do operations with 2 PINs

	unsigned long m_ioctlVerifyStart;
	unsigned long m_ioctlVerifyFinish;
	unsigned long m_ioctlVerifyDirect;
	unsigned long m_ioctlChangeStart;
	unsigned long m_ioctlChangeFinish;
	unsigned long m_ioctlChangeDirect;
};

}
#endif
