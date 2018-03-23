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
#include "Pinpad.h"
#include "Context.h"
#include "pinpad2.h"
#include "GempcPinpad.h"
#include "GenericPinpad.h"
#include "ACR83Pinpad.h"

#include "../common/Log.h"
#include "../common/Util.h"
#include "../common/Config.h"
#include <cstring>

namespace eIDMW
{

CPinpad::CPinpad(CContext *poContext,
			 const std::string & csReader)
{
	m_poContext = poContext;
	m_csReader = csReader;

}

//Factory method for Pinpad Implementations, detection is based on reader name
GenericPinpad *CPinpad::getPinpadHandler()
{
	GenericPinpad * pinpad_handler = NULL; 

	if (strstr(m_csReader.c_str(), "GemPC Pinpad") != NULL || 
        strstr(m_csReader.c_str(), "GemPCPinpad") != NULL)
		pinpad_handler = new GemPcPinpad(m_poContext, m_hCard, m_csReader);

	else if (strstr(m_csReader.c_str(), "ACR83U") != NULL)
		pinpad_handler = new ACR83Pinpad(m_poContext, m_hCard, m_csReader); 

	else 
		pinpad_handler = new GenericPinpad(m_poContext, m_hCard, m_csReader);

	return pinpad_handler;	

}

void CPinpad::Init(SCARDHANDLE hCard)
{
	 m_hCard = hCard;
}


bool CPinpad::UsePinpad()
{
	GetFeatureList();

	return m_bCanVerifyUnlock;
}

CByteArray CPinpad::PinpadControl(unsigned long ulControl, const CByteArray & oCmd,
	tPinOperation operation, unsigned char ucPintype,
	const std::string & csPinLabel,	bool bShowDlg, void *wndGeometry )
{

	bool bCloseDlg = bShowDlg;
	CByteArray oResp;

	try
	{
		oResp = m_poContext->m_oPCSC.Control(m_hCard, ulControl, oCmd);
	}
	catch (...)
	{
		throw ;
	}

	return oResp;
}

#define CHECK_FEATURE(feature, featureID, iotcl) \
	if (feature[0] == featureID) \
		iotcl = 256 * (256 * ((256 * feature[2]) + feature[3]) + feature[4]) + feature[5];

void CPinpad::GetFeatureList()
{
	m_bCanVerifyUnlock = false;
	m_bCanChangeUnlock = false;
	m_ioctlVerifyStart = m_ioctlVerifyFinish = m_ioctlVerifyDirect = 0;
	m_ioctlChangeStart = m_ioctlChangeFinish = m_ioctlChangeDirect = 0;

	try {
		CByteArray oFeatures = PinpadControl(CCID_IOCTL_GET_FEATURE_REQUEST,
			CByteArray(), PIN_OP_VERIFY, 0, "", false);

		// Example of a feature list: 06 04 00 31 20 30 07 04 00 31 20 34
		// Which means:
		//  - IOCTL for CCID_VERIFY_DIRECT = 0x00312030
		//  - IOCTL for CCID_CHANGE_DIRECT = 0x00312034
		unsigned long ulFeatureLen = oFeatures.Size();
		if ((ulFeatureLen % 6) == 0)
		{
			const unsigned char *pucFeatures = oFeatures.GetBytes();
			ulFeatureLen /= 6;
			for (unsigned long i = 0; i < ulFeatureLen; i++)
			{
				CHECK_FEATURE(pucFeatures, CCID_VERIFY_START, m_ioctlVerifyStart)
				CHECK_FEATURE(pucFeatures, CCID_VERIFY_FINISH, m_ioctlVerifyFinish)
				CHECK_FEATURE(pucFeatures, CCID_VERIFY_DIRECT, m_ioctlVerifyDirect)
				CHECK_FEATURE(pucFeatures, CCID_CHANGE_START, m_ioctlChangeStart)
				CHECK_FEATURE(pucFeatures, CCID_CHANGE_FINISH, m_ioctlChangeFinish)
				CHECK_FEATURE(pucFeatures, CCID_CHANGE_DIRECT, m_ioctlChangeDirect)
				pucFeatures += 6;
			}

			m_bCanVerifyUnlock = (m_ioctlVerifyStart && m_ioctlVerifyFinish) || m_ioctlVerifyDirect;
			m_bCanChangeUnlock = (m_ioctlChangeStart && m_ioctlChangeFinish) || m_ioctlChangeDirect;

		}
	}
	catch(const CMWException & e)
	{
		// very likely CCID_IOCTL_GET_FEATURE_REQUEST isn't supported
		// by this reader -> nothing to do
		e.GetError();
	}

}


}

