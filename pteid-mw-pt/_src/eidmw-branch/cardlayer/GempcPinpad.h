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

#ifndef GEMPC_PINPAD_H
#define GEMPC_PINPAD_H

#include "GenericPinpad.h"

#include "pinpad2-private.h"

#ifdef _WIN32
#define CM_IOCTL_VERIFY_PIN 0x00312110
#define CM_IOCTL_MODIFY_PIN 0x00312114
#else
#define CM_IOCTL_VERIFY_PIN 0x42330006
#define CM_IOCTL_MODIFY_PIN 0x42330007
#endif

namespace eIDMW
{

class GemPcPinpad : public GenericPinpad 
{
public:
    GemPcPinpad(CContext *poContext, SCARDHANDLE hCard,
		const std::string & csReader): GenericPinpad(poContext, hCard, csReader)
	{
	};

    virtual CByteArray PinCmd(tPinOperation operation,
		const tPin & pin, unsigned char ucPinType, 
		const CByteArray & oAPDU, unsigned long & ulRemaining, void *wndGeometry = 0 );

private:
	void fillVerifyControlStruct(PP_VERIFY_CCID * pin_verify);
	void fillModifyControlStruct(PP_CHANGE_CCID * pin_change, bool include_verify);
	void fillUnlockControlStruct(PP_CHANGE_CCID *pin_change);
	unsigned int fillStructIAS(unsigned char* apdu, unsigned char ucPintype, int changePIN);
	unsigned int fillStructGemsafe(unsigned char * apdu, unsigned char ucPintype, int changePin);
	DWORD loadStrings(SCARDHANDLE hCard, unsigned char ucPinType, tPinOperation operation);

};

}

#endif
