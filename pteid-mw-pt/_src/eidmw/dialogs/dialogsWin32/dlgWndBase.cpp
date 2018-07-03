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
#include "dlgWndBase.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"

TD_WNDMAP WndMap;
Win32Dialog *Win32Dialog::Active_lpWnd = NULL;
HWND Win32Dialog::Active_hWnd = NULL;
extern HMODULE g_hDLLInstance;// = (HMODULE)NULL;

HFONT Win32Dialog::loadFontFromResource(int font_pointsize, bool isBold) {
	/*
	const char * szFontFile = "Lato-Regular.ttf";
	int nResults = AddFontResourceA(szFontFile); 		// font file name
	*/

	if (m_fonthandle == NULL) {
	
	HRSRC res = FindResource(m_hInstance,
		MAKEINTRESOURCE(IDR_MYFONT), L"BINARY");
	if (res)
	{
		HGLOBAL mem = LoadResource(m_hInstance, res);
		void *data = LockResource(mem);
		size_t len = SizeofResource(m_hInstance, res);

		DWORD nFonts;
		m_fonthandle = AddFontMemResourceEx(
			data,       	// font resource
			len,       	// number of bytes in font resource 
			NULL,          	// Reserved. Must be 0.
			&nFonts      	// number of fonts installed
			);

		if (m_fonthandle == 0)
		{
			MWLOG(LEV_ERROR, MOD_DLG, L"  --> Win32Dialog::loadFontFromResource failed!");
		}
	}
	}

	HFONT TextFont;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	HDC screen = GetDC(0);

	lf.lfHeight = -MulDiv(font_pointsize, GetDeviceCaps(screen, LOGPIXELSY), 72);
	lf.lfWeight = isBold ? FW_BOLD : FW_NORMAL;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;

	wcscpy_s(lf.lfFaceName, L"Lato Regular");

	TextFont = CreateFont(lf.lfHeight, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	return TextFont;
}

Win32Dialog::Win32Dialog(const wchar_t *appName)
{
	m_ModalHold = true;
	m_hDC = NULL;					// Private GDI Device Context
	m_hWnd = NULL;					// Holds Our Window Handle
	m_hInstance = g_hDLLInstance;	// Grab An Instance From our DLL module to become able to Create our windows for/from
	//m_appName = "DialogBase";		// Application Core-Name
	dlgResult = eIDMW::DLG_CANCEL;	// Dialog Result
	m_fonthandle = NULL;
	m_appName=_wcsdup(appName);

	TextFontHeader = loadFontFromResource(16*.75, true);
	TextFont = loadFontFromResource(12 * .75, false);
}

Win32Dialog::~Win32Dialog()
{
	if( m_hWnd )
		KillWindow( );

	if(m_appName)
	{
		free(m_appName);
		m_appName=NULL;
	}

	m_ModalHold = false;
}


bool Win32Dialog::CreateWnd( const wchar_t* title, int width, int height, int Icon, HWND Parent )
{
	if( m_hWnd )
		return false;

	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> Win32Dialog::CreateWnd (Parent=%X)",Parent);

	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	RECT		DeskRect;

	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;			// Set Top Value To 0
	WindowRect.bottom = (long)height;	// Set Bottom Value To Requested Height
	GetClientRect( GetDesktopWindow(), &DeskRect );

	WNDCLASS	wc;						// Windows Class Structure
	HICON hIco;
	if( Icon == 0 )
		hIco = LoadIcon( NULL, IDI_WINLOGO );
	else
		hIco = LoadIcon( m_hInstance, MAKEINTRESOURCE(Icon) );

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW; // 	            // Redraw On Size, And Drop Shadow
	wc.lpfnWndProc		= (WNDPROC)WndProc;							// WndProc Handles Messages
	wc.cbClsExtra		= 0;										// No Extra Window Data
	wc.cbWndExtra		= 0; //DLGWINDOWEXTRA;							// No Extra Window Data
	wc.hInstance		= m_hInstance;								// Set The Instance
	wc.hIcon			= hIco;			// Load The Default Icon
	wc.hCursor			= LoadCursor( NULL, IDC_ARROW );			// Load The Arrow Pointer
	wc.hbrBackground =  CreateSolidBrush(RGB(255, 255, 255));        //CreateSolidBrush(RGB(255, 255, 255));        //(HBRUSH)GetSysColorBrush( COLOR_3DFACE );	// What Color we want in our background
	wc.lpszMenuName		= NULL;										// We Don't Want A Menu
	wc.lpszClassName	= m_appName;								// Set The Class Name

	if( !RegisterClass( &wc ) )									// Attempt To Register The Window Class
	{
		unsigned long err = GetLastError();
		MWLOG(LEV_WARN, MOD_DLG, L"  --> Win32Dialog::CreateWnd - Failed To Register The Window Class - Error=%ld",err);
		return false;											// Return FALSE
	}

	//dwStyle = WS_CAPTION | WS_VISIBLE |  WS_SYSMENU | WS_OVERLAPPED;
	dwStyle = WS_POPUP | WS_BORDER;

	dwExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_WINDOWEDGE | WS_EX_TOPMOST;
	if( m_ModalHold )
	{
		dwStyle |= WS_POPUP;
		dwExStyle |= WS_EX_DLGMODALFRAME;
	}

	AdjustWindowRectEx( &WindowRect, dwStyle, FALSE, dwExStyle );	// Adjust Window To True Requested Size

	Active_lpWnd = this;

	// Create The Window
	if (!(m_hWnd = CreateWindow(m_appName, title, dwStyle,
		DeskRect.right / 2 - (WindowRect.right - WindowRect.left) / 2,
		DeskRect.bottom / 2 - (WindowRect.bottom - WindowRect.top) / 2,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		  Parent,
		  NULL,
		  m_hInstance,
		  NULL)))
	{
		unsigned long err = GetLastError();
		KillWindow();								// Reset The Display
		MWLOG(LEV_WARN, MOD_DLG, L"  --> Win32Dialog::CreateWnd - Window Creation Error - Error=%ld", err);
		return false;
	}

	 SetWindowLong(m_hWnd, GWL_STYLE, 0);  //remove all window styles, check MSDN for details

	 //ShowWindow(m_hWnd, SW_SHOW);          //display window

	// Create The Window
	 /*
	if( !( m_hWnd = Active_hWnd = CreateWindowEx(	dwExStyle,			// Extended Style For The Window
								m_appName,							// Class Name
								title,								// Window Title
								dwStyle,							// Defined Window Style
								DeskRect.right/2 - (WindowRect.right-WindowRect.left)/2,
								DeskRect.bottom/2 - (WindowRect.bottom-WindowRect.top)/2,
								//CW_USEDEFAULT, CW_USEDEFAULT,		// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								Parent,								// No Parent Window
								NULL,								// No Menu
								m_hInstance,							// Instance
								(LPVOID)Active_lpWnd)))								// Dont Pass Anything To WM_CREATE
	{
		unsigned long err = GetLastError();
		KillWindow( );								// Reset The Display
		MWLOG(LEV_WARN, MOD_DLG, L"  --> Win32Dialog::CreateWnd - Window Creation Error - Error=%ld",err);
		return false;								// Return FALSE
	}
	MWLOG(LEV_DEBUG, MOD_DLG, L"  --> Win32Dialog::CreateWnd - CreateWindowEx (m_hWnd=%X)",m_hWnd);

	*/

	WndMap.insert( TD_WNDPAIR( m_hWnd, this ) );

	return true;
}

bool Win32Dialog::exec()
{
	if( !m_hWnd )
		return false;
	
	ShowWindow( m_hWnd, SW_SHOW );					// Show The Window
	//SetFocus( m_hWnd );								// Sets Keyboard Focus To The Window


	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) && m_ModalHold )
	{
		if( NULL == m_hWnd || !IsDialogMessage( m_hWnd, &msg ) )
		{
			TranslateMessage( &msg );			// Translate The Message
			DispatchMessage( &msg );			// Dispatch The Message
		}
		if( msg.hwnd == m_hWnd )
		{
			if( msg.message == 274 || msg.message == 161 ) // || msg.message == 513 ) // 161 == WM_RESIZE | WM_MOVE; 274 == WM_CLOSE; 513 == WM_COMMAND
			{
				//check if it's minimized 
				if( IsIconic( m_hWnd ) ) 
				{
					break; // Exit Loop
				}
			}
		}
	}
	return 	dlgResult != eIDMW::DLG_CANCEL;
}
void Win32Dialog::show()
{
	ShowWindow( m_hWnd, SW_SHOW );					// Show The Window
	SetFocus( m_hWnd );								// Sets Keyboard Focus To The Window
}

void Win32Dialog::close()
{
	//CloseWindow( m_hWnd ) and ShowWindow( m_hWnd, SW_MINIMIZE ) should do the same
	//but they don't.
	//This is a workarond for signing mail with Oulook 2003 (with Word as editor and 1 processor)
	//If the ShowWindow is used in place of the CloseWindow, Word hangs ?????

	CloseWindow( m_hWnd );					
	//ShowWindow( m_hWnd, SW_MINIMIZE );					// Show The Window

	m_ModalHold = false;							// Sets Keyboard Focus To The Window
}

void Win32Dialog::CreateBitapMask( HBITMAP & BmpSource, HBITMAP & BmpMask )
{
	BITMAP bm;
	// Get the dimensions of the source bitmap
	GetObject( BmpSource, sizeof(BITMAP), &bm);

	HDC hdcSrc, hdcDst;
	hdcSrc = CreateCompatibleDC( NULL );
	hdcDst = CreateCompatibleDC( NULL );

	// Create the mask bitmap
	BmpMask = CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL );

	// Load the bitmaps into memory DC
	HBITMAP hbmSrcT = (HBITMAP) SelectObject( hdcSrc, BmpSource );
	HBITMAP hbmDstT = (HBITMAP) SelectObject( hdcDst, BmpMask );

	// Set the transparent color
	COLORREF clrTrans = RGB( 255, 255, 255 );

	// Change the background to trans color
	COLORREF clrSaveBk  = SetBkColor( hdcSrc, clrTrans );

	// This call sets up the mask bitmap.
	BitBlt( hdcDst, 0, 0, bm.bmWidth, bm.bmHeight, hdcSrc, 0, 0, NOTSRCCOPY ); // NOTSRCCOPY );


	// Now, we need to paint onto the original image, making
	// sure that the "transparent" area is set to black. What
	// we do is AND the monochrome image onto the color Image
	// first. When blitting from mono to color, the monochrome
	// pixel is first transformed as follows:
	// if  1 (black) it is mapped to the color set by SetTextColor().
	// if  0 (white) is is mapped to the color set by SetBkColor().
	// Only then is the raster operation performed.

	COLORREF clrSaveDstText = SetTextColor( hdcSrc, 0xFFFFFFFF );
	SetBkColor( hdcSrc, 0x00000000 );

	SetTextColor( hdcDst, clrSaveDstText );

	SetBkColor( hdcSrc, clrSaveBk );
	SelectObject( hdcSrc, hbmSrcT );
	SelectObject( hdcDst, hbmDstT );

	DeleteDC( hdcSrc );
	DeleteDC( hdcDst );

	// connect the bitmap to it's handle
	GetObject( BmpMask, sizeof(BITMAP), &bm);

}

void Win32Dialog::Destroy()
{
}

void Win32Dialog::KillWindow( void )							// Properly Kill The Window
{
	if( !m_hWnd )
		return;

	if( m_hWnd && !DestroyWindow( m_hWnd ) )				// Are We Able To Destroy The Window?
	{
		MessageBox( NULL,L"Could Not Release hWnd.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION );
	}

	if( !UnregisterClass( m_appName, m_hInstance ) )		// Are We Able To Unregister Class
	{
		MessageBox( NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION );
	}

	WndMap.erase( m_hWnd );
	if( Active_hWnd == m_hWnd )
		Active_hWnd = NULL;
	
	m_hInstance = NULL;								// Set hInstance To NULL
	m_hWnd = NULL;									// Set hWnd To NULL
	Destroy();
}

bool Win32Dialog::isFriend( HWND f_hWnd )
{
	if( !WndMap.empty() )
	{
		try
		{
			TD_WNDMAP::iterator it_WndMap = WndMap.find( f_hWnd );
			if( it_WndMap != WndMap.end() )
				return true;
		}
		catch( ... ){}
	}

	return false;
}

LRESULT CALLBACK Win32Dialog::WndProc(	HWND	hWnd,			// Handle For This Window
						UINT	uMsg,			// Message For This Window
						WPARAM	wParam,			// Additional Message Information
						LPARAM	lParam)			// Additional Message Information
{
	if( !WndMap.empty() )
	{
		try // Call the WndProc Function from the 'HWND-Owner' Window Class 
		{
			TD_WNDMAP::iterator it_WndMap = WndMap.find( hWnd );
			if( it_WndMap != WndMap.end() )
				return (*it_WndMap).second->ProcecEvent( uMsg, wParam, lParam );
		}
		catch( ... ){}
	}
	if( uMsg == WM_CREATE || uMsg == WM_NCCREATE )  // Call the WndProc Function from the 'HWND-Owner' Window Class Retrieved from the lParam->(CREATESTRUCT *)lpCreateParams
	{
		try
		{
			if( ((CREATESTRUCT *)lParam)->lpCreateParams == (LPVOID)Active_lpWnd )
			{	
				((CREATESTRUCT *)lParam)->lpCreateParams = (LPVOID)hWnd; // Replace the Extra parameter with the handle to the window so the class resolving the message knows it's own handle too ( [Win32Dialog::CreateWnd(..)]CreateWindowEx did not yet return a value here )
				Active_lpWnd->ProcecEvent( uMsg, wParam, lParam );
			}
		}
		catch( ... ){}
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

LRESULT Win32Dialog::ProcecEvent
			(	UINT		uMsg,			// Message For This Window
				WPARAM		wParam,			// Additional Message Information
				LPARAM		lParam )		// Additional Message Information
{
	if( m_hWnd == NULL )
		return 0;

	PAINTSTRUCT ps;
	RECT rect;

	switch( uMsg )
	{
		case WM_PAINT:
		{
			m_hDC = BeginPaint( m_hWnd, &ps );
				SetBkColor( m_hDC, GetSysColor( COLOR_3DFACE ) );
				GetClientRect( m_hWnd, &rect );
				DrawText( m_hDC, L"Virtual Hello World!", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
			EndPaint( m_hWnd, &ps );

			SetForegroundWindow( m_hWnd );

			return 0;
		}

		case WM_NCACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> Win32Dialog::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);
			
			if( !IsIconic( m_hWnd ) && m_ModalHold && Active_hWnd == m_hWnd )
			{
				ShowWindow( m_hWnd, SW_SHOW );
				SetFocus( m_hWnd );
				return 0;
			}
			break;
		}

		case WM_KILLFOCUS:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> Win32Dialog::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)",wParam,lParam);

			if( !IsIconic( m_hWnd ) && m_ModalHold && Active_hWnd == m_hWnd )
			{
				if( GetParent((HWND)wParam ) != m_hWnd )
				{
					SetFocus( m_hWnd );
					return 0;
				}
			}
			break;
		}

		case WM_CREATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> Win32Dialog::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)",wParam,lParam);

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu( m_hWnd, FALSE);
			RemoveMenu( hSysMenu, 2, MF_BYPOSITION );
			return 0;
		}

		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> Win32Dialog::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			if( m_ModalHold )
			{
				ShowWindow( m_hWnd, SW_MINIMIZE );
				return 0;
			}
		}

		case WM_DESTROY: 
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> Win32Dialog::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		default:
		{
			return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}

