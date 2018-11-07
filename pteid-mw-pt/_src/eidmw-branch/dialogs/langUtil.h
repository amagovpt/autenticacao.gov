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

#ifndef __LANGUTIL_H__
#define __LANGUTIL_H__


#pragma once
#ifdef _WIN32
#include <windows.h>
#include "language_latin1.h"
#else
#include "language.h"
#endif
#include "dialogs.h"

namespace eIDMW
{

class CLang
{
public:
	static tLanguage GetLang()			{ if(!ms_InitOk) Init(); return ms_Lang; }
	static const wchar_t *GetLangW();
	static unsigned long GetLangL();
	static void SetLang(tLanguage lang)	{ ms_Lang=lang; ms_InitOk=true; }
	static void SetLangW(const wchar_t *lang);
	static void SetLangL(unsigned long lang);
	static void ResetInit()				{ ms_InitOk=false; }
	//static const wchar_t *GetString(const wchar_t *stringRef);

private:
	CLang() {};
	~CLang() {};
	static void Init();

private:
	static bool   ms_InitOk;
	static tLanguage   ms_Lang;

};

#ifdef _WIN32
	static HFONT GetSystemFont()
	{
		BOOL fResult;
		HFONT TextFont;
		NONCLIENTMETRICS nc;
		nc.cbSize = sizeof(NONCLIENTMETRICS);
		
		//Get the default system font taking into account theme and resolutions, etc...
		fResult = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nc, 0);
		
		if( fResult )     
		{
			LOGFONT lf = nc.lfMessageFont;

			TextFont = CreateFont(lf.lfHeight, lf.lfWidth, 
				lf.lfEscapement, lf.lfOrientation, lf.lfWeight, 
				lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet, 
				lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality, 
				lf.lfPitchAndFamily, lf.lfFaceName); 
		}

		return TextFont;

	};
#endif

}
#endif //__LANGUTIL_H__
