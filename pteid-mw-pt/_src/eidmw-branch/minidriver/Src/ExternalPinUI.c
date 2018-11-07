/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include "globmdrv.h"

#include "log.h"
#include "smartcard.h"
#include "externalpinui.h"
#include <commctrl.h>

// Callback function used by taskdialog
HRESULT CALLBACK TaskDialogCallbackProcPinEntry(      
    HWND hwnd,
    UINT uNotification,
    WPARAM wParam,
    LPARAM lParam,
    LONG_PTR dwRefData
	) 
{
	PEXTERNAL_PIN_INFORMATION pExternalPinInfo;
	LRESULT lResult;

	if (dwRefData != 0)
		pExternalPinInfo = (PEXTERNAL_PIN_INFORMATION)dwRefData;
	if (pExternalPinInfo->cardState != CS_PINENTRY) {
		// Dialog should close when pin entry stopped.
		SendMessage(hwnd, WM_CLOSE,0,0);
		return S_OK;
	}
	switch(uNotification) {
		case (TDN_TIMER):
			// progress bar 30 seconds.
			SendMessage(hwnd, TDM_SET_PROGRESS_BAR_POS, wParam / 300 , 0L);
			break;
		case (TDN_BUTTON_CLICKED):
			if( (int) wParam == IDCANCEL ) {
				if (pExternalPinInfo->cardState == CS_PINENTRY && pExternalPinInfo->uiState == US_PINENTRY) {
					lResult = SendMessage(hwnd, TDM_SET_ELEMENT_TEXT, TDE_CONTENT, (LPARAM)t[CANCEL_CONTENT][getLanguage()]);
					lResult = SendMessage(hwnd, TDM_SET_ELEMENT_TEXT, TDE_MAIN_INSTRUCTION, (LPARAM)t[CANCEL_MAININSTRUCTIONS][getLanguage()]);
					lResult = SendMessage(hwnd, TDM_UPDATE_ICON, TDIE_ICON_MAIN, (LPARAM)MAKEINTRESOURCE(TD_WARNING_ICON));
					pExternalPinInfo->uiState = US_PINCANCEL;
				}
				return S_FALSE;
			}
			break;
		default:
			break;
	}
	return S_OK;
}

// thread function called to show External PIN entry dialog box
DWORD WINAPI DialogThreadPinEntry(LPVOID lpParam)
{
    TASKDIALOGCONFIG tc = { 0 };

    int nButtonPressed = 0;
	HRESULT hr;

	PEXTERNAL_PIN_INFORMATION pExternalPinInfo = (PEXTERNAL_PIN_INFORMATION) lpParam;
	
	tc.hwndParent = pExternalPinInfo->hwndParentWindow;
	tc.hInstance = GetModuleHandle(NULL);
	tc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_SHOW_PROGRESS_BAR | TDF_CALLBACK_TIMER;
    tc.dwCommonButtons = TDCBF_CANCEL_BUTTON;
	tc.pszWindowTitle = t[WINDOW_TITLE][getLanguage()];
	tc.pszMainInstruction = t[ENTER_PIN_MAININSTRUCTIONS][getLanguage()];
	tc.pszContent = t[ENTER_PIN_CONTENT][getLanguage()];
	tc.pszVerificationText = NULL;
	tc.pszFooter = NULL;
	tc.pszMainIcon = MAKEINTRESOURCE(TD_INFORMATION_ICON);
    tc.cButtons = 0;
    tc.pButtons = NULL;
	tc.cRadioButtons = 0;
	tc.pRadioButtons = NULL;
	tc.pfCallback = TaskDialogCallbackProcPinEntry;
	tc.lpCallbackData = (LONG_PTR)pExternalPinInfo;
	tc.cbSize = sizeof(tc);
	pExternalPinInfo->uiState = US_PINENTRY;
	hr = TaskDialogIndirect(&tc, &nButtonPressed, NULL, NULL);

	return 0;
}

LANGUAGES getLanguage() {
	if ((GetUserDefaultUILanguage() & LANG_ENGLISH) == LANG_ENGLISH) 
		return en;
	if ((GetUserDefaultUILanguage() & LANG_DUTCH) == LANG_DUTCH) 
		return nl;
	if ((GetUserDefaultUILanguage() & LANG_FRENCH) == LANG_FRENCH) 
		return fr;
	//default language
	return en;
}
