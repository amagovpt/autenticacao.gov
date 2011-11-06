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
/**
 * A pinpad lib is a DLL, SO file or DYLIB that implements
 * and export the 2 functions in pinpad2.h (or another
 * pinpad API that should be supported).
 * See pinpad2.h for more info about the functions and
 * the requirements for such pinpad libs.
 * This class takes care of searching and using a pinpad
 * lib for a specific type of eID card and a specific
 * type of pinpad reader: all pinpad libs for that specific
 * eID card are searched and are loaded in order to check
 * if they support the pinpad reader in question.
 */

#ifndef __PINPADLIB_H__
#define __PINPADLIB_H__

#include "stdlib.h"
#include "../common/Export.h"
#include "../common/DynamicLib.h"
#include "../common/ByteArray.h"
#include "pinpad2.h"
#if defined WIN32 && defined PTEID_OLD_PINPAD
#include "old/PinpadLibOldPteid.h" // For the old BE eID pinpad libs
#endif
#include "../dialogs/dialogs.h"

namespace eIDMW
{
#ifdef EIDMW_PF_EXPORT
class CPinpadLib
#else
class EIDMW_CAL_API CPinpadLib
#endif
{
public:
	CPinpadLib();
	~CPinpadLib();

	/** Search for a pinpad lib that supports this reader and load it */
	bool Load(unsigned long hContext, SCARDHANDLE hCard,
		const std::string & csReader, const std::string & csPinpadPrefix,
		unsigned long ulLanguage);

	/** Unload the currently loaded pinpad lib */
	void Unload();

	/** Send a command to the pinpad lib */
    CByteArray PinCmd(SCARDHANDLE hCard, unsigned long ulControl,
		CByteArray oCmd, unsigned char ucPintype, unsigned char ucOperation);

	/** This method is also called when no pinpad lib is used */
	bool ShowDlg(unsigned char pinpadOperation, unsigned char ucPintype,
		const std::string & csPinLabel, const std::string & csReader,
		unsigned long *pulDlgHandle);

	/** To close the dialog opened by PinCmd() */
	void CloseDlg(unsigned long ulDlgHandle);

private:
	bool CheckLib(const std::string & csPinpadDir, const char *csFileName,
		unsigned long ulLanguage, int iVersion,
		unsigned long hContext, SCARDHANDLE hCard, const char *csReader);
	tGuiInfo *InitGuiInfo();
	void ClearGuiInfo();
	const char *GetGuiMesg(unsigned char ucOperation);

	CDynamicLib m_oPinpadLib;
	EIDMW_PP2_COMMAND m_ppCmd2;
	tGuiInfo m_guiInfo;

#if defined WIN32 && defined PTEID_OLD_PINPAD
	CPinpadLibOldPteid m_oPinpadLibOldPteid; // For the old BE eID pinpad libs
#endif
};

}

#endif
