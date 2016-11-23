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

#include "stdafx.h"
#include "../dialogs.h"
#include "dlgWndAskPIN.h"
#include "dlgWndAskPINs.h"
#include "dlgWndBadPIN.h"
#include "dlgWndPinpadInfo.h"
#include "resource.h"
#include "../langUtil.h"
#include "Config.h"
#include "Log.h"

using namespace eIDMW;

HMODULE g_hDLLInstance = (HMODULE)NULL;

int g_UseKeyPad = -1;

typedef std::map< unsigned long, dlgWndPinpadInfo* > TD_WNDPINPAD_MAP;
typedef std::pair< unsigned long, dlgWndPinpadInfo* > TD_WNDPINPAD_PAIR;

dlgWndPinpadInfo *dlgModal = NULL;
TD_WNDPINPAD_MAP dlgPinPadInfoCollector;
unsigned long dlgPinPadInfoCollectorIndex = 0;

std::wstring lang1 = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);

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

bool DlgGetKeyPad()
{	
	if(g_UseKeyPad==-1)
	{
		g_UseKeyPad = (CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_VIRTUALKBD)?1:0);
	}
    return (g_UseKeyPad==0 ? false: true);
}

std::wstring translatePinName(std::wstring &PinName)
{
	if(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE)
		== L"en")
	{
	if (PinName.find(L"Autentic") != std::wstring::npos)
		return std::wstring(L"Authentication PIN");
	if (PinName.find(L"Assinatura") != std::wstring::npos)
		return std::wstring(L"Signature PIN");
	if (PinName.find(L"Morada") != std::wstring::npos)
		return std::wstring(L"Address PIN");
	}

	return PinName;

}

	/************************
	*       DIALOGS
	************************/
//TODO: Add Keypad possibility in DlgAskPin(s)     
DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation,
			DlgPinUsage usage, const wchar_t *csPinName,
			DlgPinInfo pinInfo, wchar_t *csPin, unsigned long ulPinBufferLen)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgAskPin() called with arguments usage: %d, PinName: %s, Operation:%d",
		usage, csPinName,operation);

	CLang::ResetInit();				// Reset language to take into account last change
	g_UseKeyPad=-1;					// Reset the keypad

	dlgWndAskPIN *dlg = NULL;
	try
	{
		std::wstring PINName;

		/*if(wcscmp(L"nl",lang1.c_str())==0)
		{ */
			if (usage == DLG_PIN_AUTH)
				PINName.append(L"Pin da Autenticação");
			else if (usage == DLG_PIN_ADDRESS)
				PINName.append(L"Pin da Morada");
			else
				PINName.append(L"Pin da Assinatura");
		/*}
		else
			PINName = GETSTRING_DLG(Pin); */

		std::wstring sMessage;
		switch( operation )
		{
		case DLG_PIN_OP_VERIFY:
			switch( usage )
			{
			case DLG_PIN_AUTH:
			
				sMessage += L"\n\n";
				sMessage += GETSTRING_DLG(PleaseEnterYourPin);
				sMessage += L", ";
				sMessage += GETSTRING_DLG(InOrderToAuthenticateYourself);
				sMessage += L"\n";
				break;
			case DLG_PIN_SIGN:
					
				sMessage += L"\n\n";
				sMessage += GETSTRING_DLG(Caution);
				sMessage += L" ";
				sMessage += GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronic);
				
				break;
			case DLG_PIN_ADDRESS:
				sMessage += L"\n\n";
				sMessage += GETSTRING_DLG(PleaseEnterYourPin);
				sMessage += L"\n";
				break;

			default:
				sMessage += L"----Default PIN Type????----";
				sMessage += L"\n";
				break;
			}
			break;
		case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
			sMessage = GETSTRING_DLG(PleaseEnterYourPuk);
			sMessage += L", ";
			sMessage = GETSTRING_DLG(ToUnblock);
			sMessage += L" ";
			sMessage = GETSTRING_DLG(Your);
			sMessage += L" \"";
			if( wcslen(csPinName)==0 )
				sMessage += csPinName;
			else
				sMessage += GETSTRING_DLG(Pin);
			sMessage += L"\"\n";

			break;
		default:
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPin() returns DLG_BAD_PARAM");
			return DLG_BAD_PARAM;
			break;
		}


		dlg = new dlgWndAskPIN( pinInfo, usage, sMessage, translatePinName(PINName), DlgGetKeyPad() );
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
			DlgPinInfo pin2Info, wchar_t *csPin2, unsigned long ulPin2BufferLen)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgAskPins() called");

	CLang::ResetInit();				// Reset language to take into account last change
	g_UseKeyPad=-1;					// Reset the keypad

	dlgWndAskPINs *dlg = NULL;
	try
	{
		std::wstring PINName;
		std::wstring Header;
		
		switch( operation )
		{
		case DLG_PIN_OP_CHANGE:
			if( usage == DLG_PIN_AUTH )
			{
				if(wcscmp(L"nl",lang1.c_str())==0)
					PINName = (L"Pin da Autenticação");
				else
					PINName = csPinName;
			}
			else
				PINName = csPinName;
			Header = GETSTRING_DLG(EnterYour);
			Header += L" ";
			Header += PINName;
			Header += L" ";
			break;
		case DLG_PIN_OP_UNBLOCK_CHANGE:
			if( usage == DLG_PIN_UNKNOWN )
				PINName = csPinName;
			else
				PINName = GETSTRING_DLG(Puk);
			Header += GETSTRING_DLG(UnlockDialogHeader);
			break;
		default:
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgAskPins() returns DLG_BAD_PARAM");
			return DLG_BAD_PARAM;
			break;
		}

		dlg = new dlgWndAskPINs( pin1Info, pin2Info, Header, translatePinName(PINName), DlgGetKeyPad() );
		if( dlg->exec() )
		{
			eIDMW::DlgRet dlgResult = dlg->dlgResult;
			wcscpy_s(csPin1,ulPin1BufferLen,dlg->Pin1Result);
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
			unsigned long ulRemainingTries)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgBadPin() called");

	CLang::ResetInit();				// Reset language to take into account last change

	dlgWndBadPIN *dlg = NULL;
	try
	{
		std::wstring PINName;
		switch( usage )
		{
		case DLG_PIN_UNKNOWN:
			if( wcslen(csPinName)==0 )
			{
				MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgBadPin() returns DLG_BAD_PARAM");
				return DLG_BAD_PARAM;
			}
			PINName = csPinName;
			break;
		default:
			if( wcslen(csPinName)==0 )
				PINName = GETSTRING_DLG(Pin);
			else
				PINName = csPinName;
			break;
		}

		dlg = new dlgWndBadPIN( translatePinName(PINName), ulRemainingTries );
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
			unsigned long *pulHandle)
{
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgDisplayPinpadInfo() called");

	CLang::ResetInit();				// Reset language to take into account last change

	try
	{
		std::wstring PINName;
		switch( usage )
		{
		case DLG_PIN_UNKNOWN:
			if( wcslen(csPinName)==0 )
			{
				MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgDisplayPinpadInfo() returns DLG_BAD_PARAM");
				return DLG_BAD_PARAM;
			}
			PINName = csPinName;
			break;
		default:
			if( wcslen(csPinName)==0 )
				PINName = GETSTRING_DLG(Pin);
			else
				PINName = csPinName;
			break;
		}

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
				switch( usage )
				{
				case DLG_PIN_AUTH:
					sMessage = GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
					if(wcslen(csReader)!=0)
					{
						sMessage += L" \"";
						sMessage += csReader;
						sMessage += L"\"";
					}
					sMessage += L", ";
					sMessage += GETSTRING_DLG(InOrderToAuthenticateYourself);
					sMessage += L"\n";
					break;
				case DLG_PIN_SIGN:
					/*
					sMessage = GETSTRING_DLG(Caution);
					sMessage += L" ";
					sMessage += GETSTRING_DLG(YouAreAboutToMakeALegallyBindingElectronic);
					sMessage += L"\n";*/
					sMessage += GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
					if(wcslen(csReader)!=0)
					{
						sMessage += L" \"";
						sMessage += csReader;
						sMessage += L"\"";
					}
					/*
					sMessage += L", ";
					sMessage += GETSTRING_DLG(ToContinueOrClickTheCancelButton);
					sMessage += L"\n\n";
					sMessage += GETSTRING_DLG(Warning);
					sMessage += L" ";
					sMessage += GETSTRING_DLG(IfYouOnlyWantToLogOnToA);
					sMessage += L"\n";
					*/
					break;
				default:
					sMessage = GETSTRING_DLG(PleaseEnterYourPinOnThePinpadReader);
					if(wcslen(csReader)!=0)
					{
						sMessage += L" \"";
						sMessage += csReader;
						sMessage += L"\"";
					}
					sMessage += L"\n";
					break;
				}
				break;
			case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
				sMessage = GETSTRING_DLG(PleaseEnterYourPukOnThePinpadReader);
				if(wcslen(csReader)!=0)
				{
					sMessage += L" \"";
					sMessage += csReader;
					sMessage += L"\"";
				}
				sMessage += L", ";
				sMessage = GETSTRING_DLG(ToUnblock);
				sMessage += L" ";
				sMessage += GETSTRING_DLG(Your);
				sMessage += L" \"";
				if( wcslen(csPinName)!=0 )
					sMessage += csPinName;
				else
					sMessage += GETSTRING_DLG(Pin);
				sMessage += L"\"\n";

				break;
			case DLG_PIN_OP_CHANGE:
				sMessage = GETSTRING_DLG(ChangeYourPin);
				sMessage += L" \"";
				if( wcslen(csPinName)!=0 )
					sMessage += PINName;
				else
					sMessage += GETSTRING_DLG(Pin);
				sMessage += L"\" ";
				sMessage += GETSTRING_DLG(OnTheReader);
				if(wcslen(csReader)!=0)
				{
					sMessage += L" \"";
					sMessage += csReader;
					sMessage += L"\"";
				}
				sMessage += L"\n";
				sMessage += GETSTRING_DLG(EnterOldNewVerify);
				sMessage += L"\n";
				break;
			case DLG_PIN_OP_UNBLOCK_CHANGE:
				sMessage = L"\n";
				sMessage += GETSTRING_DLG(UnlockDialogHeader);
						
				//sMessage += L"\" ";
				//sMessage += GETSTRING_DLG(OnTheReader);
				/*
				if(wcslen(csReader)!=0)
				{
					sMessage += L" \"";
					sMessage += csReader;
					sMessage += L"\"";
				}
				*/
				
				break;
			default:
				MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgDisplayPinpadInfo() returns DLG_BAD_PARAM");
				return DLG_BAD_PARAM;
				break;
			}
		}
		//Small hack for the PIN unlock dialog :)
		std::wstring pin_name_label;
		if (operation == DLG_PIN_OP_UNBLOCK_CHANGE)
		{
		 pin_name_label +=  GETSTRING_DLG(UnblockPinHeader);
		 pin_name_label	+= L" ";
		 pin_name_label += csPinName;
		}
		else {
			pin_name_label += translatePinName(PINName);
		}

		//QString buf = "dlg num: " + QString().setNum( dlgPinPadInfoCollectorIndex );
		dlgPinPadInfoCollectorIndex++;
		dlgModal = new dlgWndPinpadInfo( dlgPinPadInfoCollectorIndex, usage,
			operation, csReader, pin_name_label, sMessage);

		dlgModal->show();
		dlgModal->ProcecEvent(WM_PAINT,NULL,NULL);

		dlgPinPadInfoCollector.insert(TD_WNDPINPAD_PAIR( dlgPinPadInfoCollectorIndex, dlgModal ) );
		if( pulHandle )
			*pulHandle = dlgPinPadInfoCollectorIndex;

		//delete dlgModal;
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
	TD_WNDPINPAD_MAP::iterator it_WndPinpad_Map = dlgPinPadInfoCollector.find( ulHandle );
	if( it_WndPinpad_Map != dlgPinPadInfoCollector.end() )
		dlgModal = (*it_WndPinpad_Map).second;
	else 
		dlgModal = NULL;

	if( dlgModal )
	{
		delete dlgModal;
		dlgModal = NULL;
	}
	dlgPinPadInfoCollector.erase( ulHandle );
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> DlgClosePinpadInfo() returns");
}


