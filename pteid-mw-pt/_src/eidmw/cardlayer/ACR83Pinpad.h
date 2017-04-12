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

#ifndef ACR83_PINPAD_H
#define ACR83_PINPAD_H

#include "GenericPinpad.h"
#include "pinpad2-private.h"

#ifdef _WIN32
#define CM_IOCTL_VERIFY_PIN 0x00313538
#define CM_IOCTL_MODIFY_PIN 0x0031353C
#else
#define CM_IOCTL_VERIFY_PIN 0x42330006
#define CM_IOCTL_MODIFY_PIN 0x42330007
#endif

namespace eIDMW
{

class ACR83Pinpad : public GenericPinpad 
{
public:
    ACR83Pinpad(CContext *poContext, SCARDHANDLE hCard,
		const std::string & csReader): GenericPinpad(poContext, hCard, csReader)
	{
	};

    virtual CByteArray PinCmd(tPinOperation operation,
		const tPin & pin, unsigned char ucPinType,
        const CByteArray & oAPDU, unsigned long & ulRemaining, void *wndGeometry = 0 );

private:

void fillVerifyControlStruct(PP_VERIFY_CCID * pin_verify);
void fillModifyControlStruct(PP_CHANGE_CCID * pin_change);
unsigned int fillStructIAS(unsigned char* apdu, unsigned char ucPintype, int changePIN);
unsigned int fillStructGemsafe(unsigned char * apdu, unsigned char ucPintype, int changePin);



};

}

#endif


