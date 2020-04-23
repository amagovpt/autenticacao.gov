/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
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

#include "stdafx.h"
#include "../dialogs.h"
#include "dlgWndAskPIN.h"
#include "dlgWndAskPINs.h"
#include "dlgWndBadPIN.h"
#include "dlgWndPinpadInfo.h"
#include "dlgWndAskCmd.h"
#include "dlgWndCmdMsg.h"
#include "resource.h"
#include "../langUtil.h"
#include "Config.h"
#include "Log.h"

using namespace eIDMW;

HMODULE g_hDLLInstance = (HMODULE)NULL;

typedef std::map< unsigned long, dlgWndPinpadInfo* > TD_WNDPINPAD_MAP;
typedef std::pair< unsigned long, dlgWndPinpadInfo* > TD_WNDPINPAD_PAIR;

dlgWndPinpadInfo *dlgModal = NULL;
TD_WNDPINPAD_MAP dlgPinPadInfoCollector;
unsigned long dlgPinPadInfoCollectorIndex = 0;

dlgWndCmdMsg *dlgCMDMsg = NULL;

std::wstring lang1 = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);

HWND appWindow;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hDLLInstance = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

std::wstring getPinName( DlgPinUsage usage, const wchar_t *inPinName ){
    std::wstring PinName;

    switch( usage ) {
        case DLG_PIN_AUTH:
            PinName = GETSTRING_DLG(AuthenticationPin);
            break;

        case DLG_PIN_SIGN:
            PinName = GETSTRING_DLG(SignaturePin);
            break;

        case DLG_PIN_ADDRESS:
            PinName = GETSTRING_DLG(AddressPin);
            break;

        default:
            if ( inPinName == NULL ) {
                PinName = GETSTRING_DLG(UnknownPin);
            }
			else {
                 if( wcslen( inPinName ) == 0 ){
                    PinName = GETSTRING_DLG(Pin);
                } else {
                    PinName = inPinName;
                }
            }
            break;
    }

    return PinName;
}

	/************************
	*       DIALOGS
	************************/
#ifdef WIN32
DLGS_EXPORT void eIDMW::SetApplicationWindow(HWND app) {
	appWindow = app;
}
#endif
DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation,
			DlgPinUsage usage, const wchar_t *csPinName,
			DlgPinInfo pinInfo, wchar_t *csPin, unsigned long ulPinBufferLen, void *wndGeometry)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgAskPin() called with arguments usage: %d, PinName: %s, Operation:%d",
		usage, csPinName,operation);

	CLang::ResetInit();				// Reset language to take into account last change

	dlgWndAskPIN *dlg = NULL;
	try
	{
		std::wstring PINName;
		PINName = getPinName( usage, csPinName );
		std::wstring title = PINName;
		std::wstring sMessage;

		switch( operation )
		{
		case DLG_PIN_OP_VERIFY:
			switch( usage )
			{
			case DLG_PIN_AUTH:
				title = GETSTRING_DLG(AuthenticateWith);
				break;
			case DLG_PIN_SIGN:
				title = GETSTRING_DLG(SigningWith);
				sMessage += GETSTRING_DLG(Caution);
				sMessage += L" ";
				sMessage += GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronic);

				break;
			case DLG_PIN_ADDRESS:
				title = GETSTRING_DLG(ReadAddressFrom);
				break;
			case DLG_PIN_ACTIVATE:
				title = GETSTRING_DLG(ActivationPinOf);
			default:
				sMessage += L"----Default PIN Type????----";
				sMessage += L"\n";
				break;
			}
			title.append(L" ");
			title.append(GETSTRING_DLG(CitizenCard));
			break;
		case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
			sMessage = GETSTRING_DLG(PleaseEnterYourPuk);
			sMessage += L", ";
			sMessage += GETSTRING_DLG(ToUnblock);
			sMessage += L" ";
			sMessage += GETSTRING_DLG(Your);
			sMessage += L" ";
			sMessage += getPinName(usage, csPinName);
			sMessage += L".";
			break;
		default:
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPin() returns DLG_BAD_PARAM");
			return DLG_BAD_PARAM;
			break;
		}

		dlg = new dlgWndAskPIN(pinInfo, usage, title, sMessage, PINName, appWindow);
		if( dlg->exec() )
		{
			eIDMW::DlgRet dlgResult = dlg->dlgResult;
			wcscpy_s(csPin,ulPinBufferLen,dlg->PinResult);

			delete dlg;
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPin() returns DLG_OK");
			return DLG_OK;
		}
		delete dlg;
	}
	catch( ... )
	{
		if( dlg )
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPin() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPin() returns DLG_CANCEL");
	return DLG_CANCEL;
}

DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation,
			DlgPinUsage usage, const wchar_t *csPinName,
			DlgPinInfo pin1Info, wchar_t *csPin1, unsigned long ulPin1BufferLen,
			DlgPinInfo pin2Info, wchar_t *csPin2, unsigned long ulPin2BufferLen, void *wndGeometry )
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgAskPins() called");

	CLang::ResetInit();				// Reset language to take into account last change

	dlgWndAskPINs *dlg = NULL;
	try
	{
		std::wstring title;
		std::wstring Header;
		bool isUnlock = true;

		switch( operation )
		{
		case DLG_PIN_OP_CHANGE:
			title = GETSTRING_DLG(Change);
			title += L" ";
			title += getPinName(usage, csPinName);
			title += L".";

			Header = GETSTRING_DLG(EnterYour);
			Header += L" ";
			Header += getPinName(usage, csPinName);
			Header += L" ";
			Header += GETSTRING_DLG(OfCitizenCard);
			Header += L".";
			isUnlock = false;
			break;
		case DLG_PIN_OP_UNBLOCK_CHANGE:
		case DLG_PIN_OP_UNBLOCK_CHANGE_NO_PUK:
			MWLOG(LEV_DEBUG, MOD_DLG, L"dlgsWin32: Performing operation=%d", operation);
			title = GETSTRING_DLG(Unblock);
			title += L" ";
			title += getPinName(usage, csPinName);

			Header = (operation == DLG_PIN_OP_CHANGE ? GETSTRING_DLG(UnlockDialogHeader) :
														GETSTRING_DLG(UnblockPinHeader));
			Header += L".";
			break;
		default:
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPins() returns DLG_BAD_PARAM");
			return DLG_BAD_PARAM;
			break;
		}

		dlg = new dlgWndAskPINs(pin1Info, pin2Info, Header, title, isUnlock, operation == DLG_PIN_OP_UNBLOCK_CHANGE_NO_PUK, appWindow);
		if( dlg->exec() )
		{
			eIDMW::DlgRet dlgResult = dlg->dlgResult;
			if (operation != DLG_PIN_OP_UNBLOCK_CHANGE_NO_PUK)
				wcscpy_s(csPin1, ulPin1BufferLen, dlg->Pin1Result);
			wcscpy_s(csPin2,ulPin2BufferLen,dlg->Pin2Result);

			delete dlg;
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPins() returns DLG_OK");
			return DLG_OK;
		}
		delete dlg;
	}
	catch( ... )
	{
		if( dlg )
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPins() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPins() returns DLG_CANCEL");
	return DLG_CANCEL;
}

DLGS_EXPORT DlgRet eIDMW::DlgBadPin(
			DlgPinUsage usage, const wchar_t *csPinName,
			unsigned long ulRemainingTries, void *wndGeometry)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgBadPin() called");

	CLang::ResetInit();				// Reset language to take into account last change

	dlgWndBadPIN *dlg = NULL;
	try
	{
		std::wstring PINName;
		if ( ( usage == DLG_PIN_UNKNOWN ) && ( wcslen(csPinName)==0 ) ){
            MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgBadPin() returns DLG_BAD_PARAM");
            return DLG_BAD_PARAM;
        }
        PINName = getPinName( usage, csPinName );

		dlg = new dlgWndBadPIN(PINName, ulRemainingTries, appWindow);
		if( dlg->exec() )
		{
			eIDMW::DlgRet dlgResult = dlg->dlgResult;
			//csPin = dlg->PinResult;

			delete dlg;
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgBadPin() returns %ld",dlgResult);
			return dlgResult;
		}
		delete dlg;
	}
	catch( ... )
	{
		if( dlg )
			delete dlg;
		MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgBadPin() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgBadPin() returns DLG_CANCEL");
	return DLG_CANCEL;
}

DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation,
			const wchar_t *csReader, DlgPinUsage usage, const wchar_t *csPinName,
			const wchar_t *csMessage,
			unsigned long *pulHandle, void *wndGeometry)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgDisplayPinpadInfo() called");

	CLang::ResetInit();				// Reset language to take into account last change

	try
	{
		std::wstring title = getPinName(usage, csPinName);
		title += L" ";
		title += GETSTRING_DLG(OfCitizenCard);
		std::wstring sMessage;
		if(wcslen(csMessage)!=0)
		{
			sMessage=csMessage;
		}
		else
		{
			switch( operation )
			{
			case DLG_PIN_OP_VERIFY:
				switch (usage)
				{
				case DLG_PIN_AUTH:
					title = GETSTRING_DLG(AuthenticateWith);
					break;
				case DLG_PIN_SIGN:
					title = GETSTRING_DLG(SigningWith);
					break;
				case DLG_PIN_ADDRESS:
					title = GETSTRING_DLG(ReadAddressFrom);
					break;
				case DLG_PIN_ACTIVATE:
					title = GETSTRING_DLG(ActivationPinOf);
					break;
				}
				title.append(L" ");
				title.append(GETSTRING_DLG(CitizenCard));
				sMessage = GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
				break;
			case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
				sMessage = GETSTRING_DLG(PleaseEnterYourPukOnThePinpadReader);
	
				sMessage += L", ";
				sMessage = GETSTRING_DLG(ToUnblock);
				sMessage += L" ";
				sMessage += GETSTRING_DLG(Your);
				sMessage += L" \"";
				sMessage += getPinName(usage, csPinName);
				sMessage += L"\"\n";

				break;
			case DLG_PIN_OP_CHANGE:
				sMessage += GETSTRING_DLG(EnterOldNewVerify);
				sMessage += L"\n";
				break;
			case DLG_PIN_OP_UNBLOCK_CHANGE:
			case DLG_PIN_OP_UNBLOCK_CHANGE_NO_PUK:
				title = GETSTRING_DLG(UnblockPinHeader);
				title += L": ";
				title += getPinName(usage, csPinName);
				sMessage = L"\n";
				sMessage += (operation == DLG_PIN_OP_UNBLOCK_CHANGE ?
								GETSTRING_DLG(UnlockDialogInstructions) : GETSTRING_DLG(UnlockWithoutPUKInstructions));
				break;
			default:
				MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgDisplayPinpadInfo() returns DLG_BAD_PARAM");
				return DLG_BAD_PARAM;
				break;
			}
		}

		dlgPinPadInfoCollectorIndex++;
		dlgModal = new dlgWndPinpadInfo(dlgPinPadInfoCollectorIndex, usage,
			operation, csReader, title, sMessage, appWindow);

		dlgPinPadInfoCollector.insert(TD_WNDPINPAD_PAIR(dlgPinPadInfoCollectorIndex, dlgModal));
		if (pulHandle)
			*pulHandle = dlgPinPadInfoCollectorIndex;

		// Loop
		dlgModal->exec();

		delete dlgModal;
		dlgModal = NULL;

		MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgDisplayPinpadInfo() returns DLG_OK");
		return DLG_OK;
	}
	catch(...)
	{
		if( dlgModal )
			delete dlgModal;
		MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgDisplayPinpadInfo() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgDisplayPinpadInfo() returns DLG_CANCEL");
	return DLG_CANCEL;
}

DLGS_EXPORT void eIDMW::DlgClosePinpadInfo( unsigned long ulHandle )
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgClosePinpadInfo() called");
	TD_WNDPINPAD_MAP::iterator it_WndPinpad_Map = dlgPinPadInfoCollector.find(ulHandle);
	if (it_WndPinpad_Map != dlgPinPadInfoCollector.end())
		dlgModal = (*it_WndPinpad_Map).second;
	else
		dlgModal = NULL;

	if (dlgModal)
	{
		dlgModal->stopExec();
	}
	dlgPinPadInfoCollector.erase(ulHandle);
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgClosePinpadInfo() returns");
}

DLGS_EXPORT DlgRet eIDMW::DlgAskInputCMD(
			bool isValidateOtp,
			wchar_t *csOut, unsigned long ulOutBufferLen, wchar_t *csInId,
			const wchar_t *csUserName, unsigned long ulUserNameBufferLen, void * wndGeometry)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgAskCMD() called with arguments isValidateOtp=%d", isValidateOtp);

	CLang::ResetInit();				// Reset language to take into account last change

	dlgWndAskCmd *dlg = NULL;

	try
	{
		if ((!isValidateOtp && ulOutBufferLen < 9) || (isValidateOtp && ulOutBufferLen < 7))
		{
			MWLOG(LEV_ERROR, MOD_DLG, L"  --> DlgAskCMD() returns DLG_BAD_PARAM: buffer does not have enough size");
			return DLG_BAD_PARAM;
		}

		std::wstring sMessage;
		std::wstring userName;
		std::wstring userId = csInId;

		if (!isValidateOtp) {
			sMessage += GETSTRING_DLG(Caution);
			sMessage += L" ";
			sMessage += GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronicWithCmd);

			userName.append(csUserName, ulUserNameBufferLen);
		}
		else {
			sMessage += GETSTRING_DLG(InsertOtp);
		}

		dlg = new dlgWndAskCmd(isValidateOtp, sMessage, &userId, &userName, appWindow);
		if (dlg->exec())
		{
			eIDMW::DlgRet dlgResult = dlg->dlgResult;
			wcscpy_s(csOut, ulOutBufferLen, dlg->OutResult);

			delete dlg;
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskCMD() returns DLG_OK");
			return DLG_OK;
		}
		delete dlg;
	}
	catch (...)
	{
		if (dlg)
			delete dlg;
		MWLOG(LEV_ERROR, MOD_DLG, L"  --> DlgAskCMD() returns DLG_ERR");
		return DLG_ERR;
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskCMD() returns DLG_CANCEL");
	return DLG_CANCEL;
}

DLGS_EXPORT DlgRet eIDMW::DlgCMDMessage(DlgCmdMsgType msgType, const wchar_t *message)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgCMDMessage() called with argument msgType=%d, message=%S", msgType, message);
	CLang::ResetInit();				// Reset language to take into account last change

	eIDMW::DlgRet dlgResult;
	try
	{
		dlgCMDMsg = new dlgWndCmdMsg(msgType, message, appWindow);
		dlgCMDMsg->exec();
		dlgResult = dlgCMDMsg->dlgResult;
		if (dlgResult == DLG_CANCEL)
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgCMDMessage() returns DLG_CANCEL");
		}
		else
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgCMDMessage() returns dlgResult=%d", dlgResult);
		}
	}
	catch (...)
	{
		MWLOG(LEV_ERROR, MOD_DLG, L"  --> DlgCMDMessage() returns DLG_ERR");
		dlgResult = DLG_ERR;
	}

	if (dlgCMDMsg){
		delete dlgCMDMsg;
		dlgCMDMsg = NULL;
	}
	return dlgResult;
}

DLGS_EXPORT void eIDMW::DlgCloseCMDMessage()
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgCloseCMDMessage() called: =0x%p", dlgCMDMsg);
	if (dlgCMDMsg)
	{
		dlgCMDMsg->stopExec();
	}
}