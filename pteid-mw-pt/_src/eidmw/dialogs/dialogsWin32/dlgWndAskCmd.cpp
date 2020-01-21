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

#include <algorithm>    // std::max
#include "stdafx.h"
#include "dlgWndAskCmd.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"

#define IDC_STATIC 0
#define IDB_OK 1
#define IDB_CANCEL 2
#define IDC_EDIT_USERID 3
#define IDC_EDIT_PIN 4
#define IDC_EDIT 5
#define IDC_EDIT_AREACODE 6

#define MAX_USERNAME_LENGTH 90

const std::wstring dlgWndAskCmd::m_phoneAreaCodes[235] = {
    L"+351 - Portugal",
    L"+55 - Brazil",
    L"+34 - Spain",
    L"+93 - Afghanistan",
    L"+355 - Albania",
    L"+213 - Algeria",
    L"+684 - American Samoa",
    L"+376 - Andorra",
    L"+244 - Angola",
    L"+809 - Anguilla",
    L"+268 - Antigua",
    L"+54 - Argentina",
    L"+374 - Armenia",
    L"+297 - Aruba",
    L"+247 - Ascension Island",
    L"+61 - Australia",
    L"+672 - Australian External Territories",
    L"+43 - Austria",
    L"+994 - Azerbaijan",
    L"+242 - Bahamas",
    L"+246 - Barbados",
    L"+973 - Bahrain",
    L"+880 - Bangladesh",
    L"+375 - Belarus",
    L"+32 - Belgium",
    L"+501 - Belize",
    L"+229 - Benin",
    L"+809 - Bermuda",
    L"+975 - Bhutan",
    L"+284 - British Virgin Islands",
    L"+591 - Bolivia",
    L"+387 - Bosnia and Hercegovina",
    L"+267 - Botswana",
    L"+55 - Brazil",
    L"+284 - British V.I.",
    L"+673 - Brunei Darussalm",
    L"+359 - Bulgaria",
    L"+226 - Burkina Faso",
    L"+257 - Burundi",
    L"+855 - Cambodia",
    L"+237 - Cameroon",
    L"+1 - Canada",
    L"+238 - CapeVerde Islands",
    L"+1 - Caribbean Nations",
    L"+345 - Cayman Islands",
    L"+238 - Cape Verdi",
    L"+236 - Central African Republic",
    L"+235 - Chad",
    L"+56 - Chile",
    L"+86 - China (People's Republic)",
    L"+886 - China-Taiwan",
    L"+57 - Colombia",
    L"+269 - Comoros and Mayotte",
    L"+242 - Congo",
    L"+506 - Costa Rica",
    L"+385 - Croatia",
    L"+53 - Cuba",
    L"+357 - Cyprus",
    L"+420 - Czech Republic",
    L"+45 - Denmark",
    L"+246 - Diego Garcia",
    L"+767 - Dominca",
    L"+809 - Dominican Republic",
    L"+253 - Djibouti",
    L"+593 - Ecuador",
    L"+20 - Egypt",
    L"+503 - El Salvador",
    L"+240 - Equatorial Guinea",
    L"+291 - Eritrea",
    L"+372 - Estonia",
    L"+251 - Ethiopia",
    L"+500 - Falkland Islands",
    L"+298 - Faroe (Faeroe) Islands (Denmark)",
    L"+679 - Fiji",
    L"+358 - Finland",
    L"+33 - France",
    L"+596 - French Antilles",
    L"+594 - French Guiana",
    L"+241 - Gabon (Gabonese Republic)",
    L"+220 - Gambia",
    L"+995 - Georgia",
    L"+49 - Germany",
    L"+233 - Ghana",
    L"+350 - Gibraltar",
    L"+30 - Greece",
    L"+299 - Greenland",
    L"+473 - Grenada/Carricou",
    L"+671 - Guam",
    L"+502 - Guatemala",
    L"+224 - Guinea",
    L"+245 - Guinea-Bissau",
    L"+592 - Guyana",
    L"+509 - Haiti",
    L"+504 - Honduras",
    L"+852 - Hong Kong",
    L"+36 - Hungary",
    L"+354 - Iceland",
    L"+91 - India",
    L"+62 - Indonesia",
    L"+98 - Iran",
    L"+964 - Iraq",
    L"+353 - Ireland (Irish Republic; Eire)",
    L"+972 - Israel",
    L"+39 - Italy",
    L"+225 - Ivory Coast (La Cote d'Ivoire)",
    L"+876 - Jamaica",
    L"+81 - Japan",
    L"+962 - Jordan",
    L"+7 - Kazakhstan",
    L"+254 - Kenya",
    L"+855 - Khmer Republic (Cambodia/Kampuchea)",
    L"+686 - Kiribati Republic (Gilbert Islands)",
    L"+82 - Korea, Republic of (South Korea)",
    L"+850 - Korea, People's Republic of (North Korea)",
    L"+965 - Kuwait",
    L"+996 - Kyrgyz Republic",
    L"+371 - Latvia",
    L"+856 - Laos",
    L"+961 - Lebanon",
    L"+266 - Lesotho",
    L"+231 - Liberia",
    L"+370 - Lithuania",
    L"+218 - Libya",
    L"+423 - Liechtenstein",
    L"+352 - Luxembourg",
    L"+853 - Macao",
    L"+389 - Macedonia",
    L"+261 - Madagascar",
    L"+265 - Malawi",
    L"+60 - Malaysia",
    L"+960 - Maldives",
    L"+223 - Mali",
    L"+356 - Malta",
    L"+692 - Marshall Islands",
    L"+596 - Martinique (French Antilles)",
    L"+222 - Mauritania",
    L"+230 - Mauritius",
    L"+269 - Mayolte",
    L"+52 - Mexico",
    L"+691 - Micronesia (F.S. of Polynesia)",
    L"+373 - Moldova",
    L"+33 - Monaco",
    L"+976 - Mongolia",
    L"+473 - Montserrat",
    L"+212 - Morocco",
    L"+258 - Mozambique",
    L"+95 - Myanmar (former Burma)",
    L"+264 - Namibia (former South-West Africa)",
    L"+674 - Nauru",
    L"+977 - Nepal",
    L"+31 - Netherlands",
    L"+599 - Netherlands Antilles",
    L"+869 - Nevis",
    L"+687 - New Caledonia",
    L"+64 - New Zealand",
    L"+505 - Nicaragua",
    L"+227 - Niger",
    L"+234 - Nigeria",
    L"+683 - Niue",
    L"+850 - North Korea",
    L"+1 670 - North Mariana Islands (Saipan)",
    L"+47 - Norway",
    L"+968 - Oman",
    L"+92 - Pakistan",
    L"+680 - Palau",
    L"+507 - Panama",
    L"+675 - Papua New Guinea",
    L"+595 - Paraguay",
    L"+51 - Peru",
    L"+63 - Philippines",
    L"+48 - Poland",
    L"+1 787 - Puerto Rico",
    L"+974 - Qatar",
    L"+262 - Reunion (France)",
    L"+40 - Romania",
    L"+7 - Russia",
    L"+250 - Rwanda (Rwandese Republic)",
    L"+670 - Saipan",
    L"+378 - San Marino",
    L"+239 - Sao Tome and Principe",
    L"+966 - Saudi Arabia",
    L"+221 - Senegal",
    L"+381 - Serbia and Montenegro",
    L"+248 - Seychelles",
    L"+232 - Sierra Leone",
    L"+65 - Singapore",
    L"+421 - Slovakia",
    L"+386 - Slovenia",
    L"+677 - Solomon Islands",
    L"+252 - Somalia",
    L"+27 - South Africa",
    L"+34 - Spain",
    L"+94 - Sri Lanka",
    L"+290 - St. Helena",
    L"+869 - St. Kitts/Nevis",
    L"+508 - St. Pierre &(et) Miquelon (France)",
    L"+249 - Sudan",
    L"+597 - Suritext",
    L"+268 - Swaziland",
    L"+46 - Sweden",
    L"+41 - Switzerland",
    L"+963 - Syrian Arab Republic (Syria)",
    L"+689 - Tahiti (French Polynesia)",
    L"+886 - Taiwan",
    L"+7 - Tajikistan",
    L"+255 - Tanzania (includes Zanzibar)",
    L"+66 - Thailand",
    L"+228 - Togo (Togolese Republic)",
    L"+690 - Tokelau",
    L"+676 - Tonga",
    L"+1 868 - Trinidad and Tobago",
    L"+216 - Tunisia",
    L"+90 - Turkey",
    L"+993 - Turkmenistan",
    L"+688 - Tuvalu (Ellice Islands)",
    L"+256 - Uganda",
    L"+380 - Ukraine",
    L"+971 - United Arab Emirates",
    L"+44 - United Kingdom",
    L"+598 - Uruguay",
    L"+1 - USA",
    L"+7 - Uzbekistan",
    L"+678 - Vanuatu (New Hebrides)",
    L"+39 - Vatican City",
    L"+58 - Venezuela",
    L"+84 - Viet Nam",
    L"+1 340 - Virgin Islands",
    L"+681 - Wallis and Futuna",
    L"+685 - Western Samoa",
    L"+381 - Yemen (People's Democratic Republic of)",
    L"+967 - Yemen Arab Republic (North Yemen)",
    L"+381 - Yugoslavia (discontinued)",
    L"+243 - Zaire",
    L"+260 - Zambia",
    L"+263 - Zimbabwe"
};

dlgWndAskCmd::dlgWndAskCmd(DlgUserIdType userIdUsage, DlgPinUsage pinUsage,
    std::wstring & Header, std::wstring & PINName, std::wstring *userId,
    std::wstring *userName, HWND Parent) : Win32Dialog(L"WndAskCmd")
{
    hbrBkgnd = NULL;
    UserIdResult[0] = ' ';
    UserIdResult[1] = (char)0;
    PinResult[0] = ' ';
    PinResult[1] = (char)0;

    std::wstring tmpTitle = L"";

    // Length of PINs for CMD
    m_ulPinMinLen = 4;
    m_ulPinMaxLen = 8;

    // Assume the userId is the mobile number.
    bool userIdIsNumeric = (userIdUsage == DLG_USERID_MOBILE);
    m_ulUserIdMinLen = 0;
    m_ulUserIdMaxLen = 15;

    szHeader = Header.c_str();
    szPIN = PINName.c_str();

    // Added for accessibility
    tmpTitle += szHeader;
    tmpTitle += szPIN;

    int Height = 263;
    int Width = 700;
    ScaleDimensions(&Width, &Height);

    if (CreateWnd(tmpTitle.c_str(), Width, Height, IDI_APPICON, Parent))
    {
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);

        int buttonWidth = clientRect.right * 0.1;
        int buttonHeight = clientRect.bottom * 0.1;

        OK_Btn = CreateWindow(
            L"BUTTON", GETSTRING_DLG(Ok), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_FLAT,
            clientRect.right * 0.74, clientRect.bottom * 0.85, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_OK, m_hInstance, NULL);
        EnableWindow(OK_Btn, false);

        Cancel_Btn = CreateWindow(
            L"BUTTON", GETSTRING_DLG(Cancel), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_FLAT,
            clientRect.right * 0.85, clientRect.bottom * 0.85, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, NULL);

        DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;
        if (userIdIsNumeric)
            dwStyle |= ES_NUMBER;

        int editFieldsX = clientRect.right / 2 +20;
        int editFieldsWidth = 300;
        int comboboxWidth = 200;
        int labelWidth = clientRect.right / 2 - 100;
        int spacing = 5;
        int editUserIdY = clientRect.bottom - 135;
        int labelsX = clientRect.right * 0.08;
        if (userIdUsage == DLG_USERID_MOBILE)
        {
            // make space for area code
            editFieldsX = clientRect.right / 2 - 100;
            editFieldsWidth = editFieldsX + 170;

            labelWidth = 180;
            // combo box for the are code
            HWND hComboboxAreaCode = CreateWindowEx(WS_EX_CLIENTEDGE,
                L"Combobox", L"", CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
                editFieldsX, editUserIdY, comboboxWidth, 26,
                m_hWnd, (HMENU)IDC_EDIT_AREACODE, m_hInstance, NULL);
            SendMessage(hComboboxAreaCode, WM_SETFONT, (WPARAM)TextFont, 0);

            // load the item list
            for (const std::wstring &item : m_phoneAreaCodes)
            {
                SendMessage(hComboboxAreaCode, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)item.c_str());
            }
            SendMessage(hComboboxAreaCode, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
        }
        else if (userIdUsage == DLG_USERID_MOBILE_FIXED)
        {
            editFieldsWidth = clientRect.right * 0.21;
            *userName = userName->substr(0, MAX_USERNAME_LENGTH);
            int textHeight =clientRect.bottom * 0.23;
            std::wstring mobileNumberFixedString = GETSTRING_DLG(TheChosenCertificateIsFrom);
            mobileNumberFixedString += L" ";
            mobileNumberFixedString += userName->c_str();
            mobileNumberFixedString += L", ";
            mobileNumberFixedString += GETSTRING_DLG(AssociatedWithNumber);
            mobileNumberFixedString += L" ";
            mobileNumberFixedString += userId->c_str();
            mobileNumberFixedString += L".";

            editUserIdY = clientRect.bottom * 0.38;
            HWND hStaticTextUserId = CreateWindow(
                L"STATIC", mobileNumberFixedString.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
                labelsX,
                editUserIdY, 
                clientRect.right-2*labelsX,
                textHeight,
                m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);
            SendMessage(hStaticTextUserId, WM_SETFONT, (WPARAM)TextFont, 0);
        }
        if (userIdUsage != DLG_USERID_MOBILE_FIXED)
        {
            HWND hTextEditUserId = CreateWindowEx(WS_EX_CLIENTEDGE,
                L"EDIT", L"", dwStyle,
                editFieldsX + (userIdUsage == DLG_USERID_MOBILE ? comboboxWidth + spacing : 0),
                editUserIdY,
                editFieldsWidth - (userIdUsage == DLG_USERID_MOBILE ? comboboxWidth + spacing : 0), 26,
                m_hWnd, (HMENU)IDC_EDIT_USERID, m_hInstance, NULL);
            SendMessage(hTextEditUserId, EM_LIMITTEXT, m_ulUserIdMaxLen, 0);
            SendMessage(hTextEditUserId, WM_SETFONT, (WPARAM)TextFont, 0);

            HWND hStaticTextUserId = CreateWindow(
                L"STATIC", GETSTRING_DLG(MobileNumber), WS_CHILD | WS_VISIBLE | SS_LEFT,
                labelsX, editUserIdY + 4, labelWidth, 22,
                m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);
            SendMessage(hStaticTextUserId, WM_SETFONT, (WPARAM)TextFont, 0);
        }

        dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_PASSWORD | ES_NUMBER;

        int editPinY = clientRect.bottom * 0.66;
        HWND hTextEditPin = CreateWindowEx(WS_EX_CLIENTEDGE,
            L"EDIT", L"", dwStyle,
            clientRect.right * 0.74, editPinY, editFieldsWidth, buttonHeight,
            m_hWnd, (HMENU)IDC_EDIT_PIN, m_hInstance, NULL);
        SendMessage(hTextEditPin, EM_LIMITTEXT, m_ulPinMaxLen, 0);

        HWND hStaticTextPin = CreateWindow(
            L"STATIC", GETSTRING_DLG(SignaturePinCmd), WS_CHILD | WS_VISIBLE | SS_LEFT,
            labelsX, editPinY + 4, clientRect.right - 2*labelsX - editFieldsWidth, clientRect.bottom * 0.16,
            m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);

        SendMessage(hStaticTextPin, WM_SETFONT, (WPARAM)TextFont, 0);

        SendMessage(hTextEditPin, WM_SETFONT, (WPARAM)TextFont, 0);

        SendMessage(OK_Btn, WM_SETFONT, (WPARAM)TextFont, 0);
        SendMessage(Cancel_Btn, WM_SETFONT, (WPARAM)TextFont, 0);

        if (userIdUsage == DLG_USERID_MOBILE_FIXED)
        {
            SetFocus(GetDlgItem(m_hWnd, IDC_EDIT_PIN));
        }
        else
        {
            SetFocus(GetDlgItem(m_hWnd, IDC_EDIT_USERID));
        }

    }
}

dlgWndAskCmd::~dlgWndAskCmd()
{
    EnableWindow(m_parent, TRUE);
    KillWindow();
}

void dlgWndAskCmd::GetResult()
{
    wchar_t nameBuf[RESULT_BUFFER_SIZE];
    int areaCodeIndex;
    std::wstring userIdString;
    // if we have combobox, add the area code to the userId String before reading the mobile number
    if (GetDlgItem(m_hWnd, IDC_EDIT_AREACODE))
    {
        areaCodeIndex  = SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_AREACODE), CB_GETCURSEL, 0, 0);
        userIdString = m_phoneAreaCodes[areaCodeIndex];
        int codeEnd = userIdString.find('-');
        userIdString = userIdString.substr(0, codeEnd);
    }

    // ensure both userid and pin fit in buffers
    long len = (std::max)((long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_USERID), WM_GETTEXTLENGTH, 0, 0) + (long)userIdString.length(),
                          (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_PIN), WM_GETTEXTLENGTH, 0, 0));
    if (len < RESULT_BUFFER_SIZE)
    {
        SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_USERID), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf);
        userIdString += nameBuf;
        wcscpy_s(UserIdResult, userIdString.c_str());

        SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_PIN), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf);
        wcscpy_s(PinResult, nameBuf);
    }
}

LRESULT dlgWndAskCmd::ProcecEvent
(UINT		uMsg,			// Message For This Window
WPARAM		wParam,			// Additional Message Information
LPARAM		lParam)		// Additional Message Information
{
    PAINTSTRUCT ps;
    RECT rect;

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_EDIT_USERID:
        {
            if (EN_CHANGE == HIWORD(wParam))
            {
                EnableWindow(GetDlgItem(m_hWnd, IDOK), ((unsigned int)AreFieldsFilled()));
            }
            return TRUE;
        }

        case IDC_EDIT_PIN:
        {
            if (EN_CHANGE == HIWORD(wParam))
            {
                EnableWindow(GetDlgItem(m_hWnd, IDOK), ((unsigned int)AreFieldsFilled()));
            }
            return TRUE;
        }

        case IDB_OK:
            GetResult();
            dlgResult = eIDMW::DLG_OK;
            close();
            return TRUE;

        case IDB_CANCEL:
            dlgResult = eIDMW::DLG_CANCEL;
            close();
            return TRUE;

        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }
    }


    case WM_SIZE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return 0;
        break;
    }

    //Set the TextColor for the subwindows hTextEdit and hStaticText
    case WM_CTLCOLORSTATIC:
    {
        //TODO: grey button
        COLORREF grey = RGB(0xD6, 0xD7, 0xD7);
        COLORREF white = RGB(0xFF, 0xFF, 0xFF);
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0x3C, 0x5D, 0xBC));

        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);
        if ((HWND)lParam == OK_Btn || (HWND)lParam == Cancel_Btn)
        {
            SetBkColor(hdcStatic, grey);
            return (INT_PTR)CreateSolidBrush(grey);
        }

        SetBkColor(hdcStatic, white);

        if (hbrBkgnd == NULL)
        {
            hbrBkgnd = CreateSolidBrush(white);
        }

        return (INT_PTR)hbrBkgnd;
    }

    case WM_PAINT:
    {
        m_hDC = BeginPaint(m_hWnd, &ps);
        SetTextColor(m_hDC, RGB(0x3C, 0x5D, 0xBC));

        //Change top header dimensions
        GetClientRect(m_hWnd, &rect);
        rect.left = rect.right * 0.08;
        rect.top = rect.bottom * 0.08;
        rect.right -= rect.left;
        rect.bottom = rect.bottom * 0.25;

        SetBkColor(m_hDC, RGB(255, 255, 255));
        SelectObject(m_hDC, TextFontHeader);
        MWLOG(LEV_DEBUG, MOD_DLG, L"Processing event WM_PAINT - Mapping mode: %d", GetMapMode(m_hDC));

        //The first call is needed to calculate the needed bounding rectangle
        DrawText(m_hDC, szHeader, -1, &rect, DT_WORDBREAK | DT_CALCRECT);
        DrawText(m_hDC, szHeader, -1, &rect, DT_WORDBREAK);

        EndPaint(m_hWnd, &ps);

        SetForegroundWindow(m_hWnd);

        return 0;
    }

    case WM_ACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_ACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_NCACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);

        if (!wParam)
        {
            SetFocus(m_hWnd);
            return 0;
        }
        break;
    }

    case WM_SETFOCUS:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_SETFOCUS (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_KILLFOCUS:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_CREATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)", wParam, lParam);

        HMENU hSysMenu;

        hSysMenu = GetSystemMenu(m_hWnd, FALSE);
        EnableMenuItem(hSysMenu, 3, MF_BYPOSITION | MF_GRAYED);
        SendMessage(m_hWnd, DM_SETDEFID, (WPARAM)IDC_EDIT, (LPARAM)0);

        return DefWindowProc((HWND)((CREATESTRUCT *)lParam)->lpCreateParams, uMsg, wParam, lParam);
    }

    case WM_CLOSE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);

        ShowWindow(m_hWnd, SW_MINIMIZE);
        return 0;
    }

    case WM_DESTROY:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    default:
    {
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
    }
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

bool dlgWndAskCmd::AreFieldsFilled()
{
    long userIdLen = (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_USERID), WM_GETTEXTLENGTH, 0, 0);
    long pinLen = (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_PIN), WM_GETTEXTLENGTH, 0, 0);

    return userIdLen >= m_ulUserIdMinLen && pinLen >= m_ulPinMinLen;
}
