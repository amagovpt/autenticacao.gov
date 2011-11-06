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

#include "langUtil.h"
#include "dialogs.h"
#include "Config.h"
#include "Util.h"

#define PATH_LENGHT 255

namespace eIDMW
{

bool CLang::ms_InitOk=false;
tLanguage CLang::ms_Lang=LANG_EN;

const wchar_t *CLang::GetLangW()
{
	if(!ms_InitOk) 
		Init();

	switch(ms_Lang)
	{
	case LANG_EN:
		return LANG_STRCODE_EN;
	case LANG_NL:
		return LANG_STRCODE_NL;
	case LANG_FR:
		return LANG_STRCODE_FR;
	case LANG_DE:
		return LANG_STRCODE_DE;
	//case LANG_PT:
	//	return LANG_STRCODE_PT;
	default:
		return LANG_STRCODE_EN;
	}
}

unsigned long CLang::GetLangL()
{
	if(!ms_InitOk) 
		Init();

	switch(ms_Lang)
	{
	case LANG_EN:
		return DLG_LANG_EN;
	case LANG_NL:
		return DLG_LANG_NL;
	case LANG_FR:
		return DLG_LANG_FR;
	case LANG_DE:
		return DLG_LANG_DE;
	//case LANG_PT:
	//	return DLG_LANG_PT;
	default:
		return DLG_LANG_EN;
	}
}
void CLang::SetLangW(const wchar_t *lang)
{
	ms_InitOk=true;

	if(wcscmp(lang,LANG_STRCODE_EN)==0)
		ms_Lang=LANG_EN;
	else if(wcscmp(lang,LANG_STRCODE_NL)==0)
		ms_Lang=LANG_NL;
	else if(wcscmp(lang,LANG_STRCODE_FR)==0)
		ms_Lang=LANG_FR;
	else if(wcscmp(lang,LANG_STRCODE_DE)==0)
		ms_Lang=LANG_DE;
	//else if(wcscmp(lang,LANG_STRCODE_PT)==0)
	//	ms_Lang=LANG_PT;
	else
		ms_InitOk=false;
}

void CLang::SetLangL(unsigned long lang)
{

	switch(lang)
	{
	case DLG_LANG_EN:
		ms_Lang=LANG_EN;
		break;
	case DLG_LANG_NL:
		ms_Lang=LANG_NL;
		break;
	case DLG_LANG_FR:
		ms_Lang=LANG_FR;
		break;
	case DLG_LANG_DE:
		ms_Lang=LANG_DE;
		break;
	//case DLG_LANG_PT:
	//	ms_Lang=LANG_PT;
	//	break;
	default:
		ms_Lang=LANG_EN;
		break;
	}
}

std::wstring CLang::GetMessageFromID(DlgMessageID messageID)
{
	std::wstring csMessage=L"";

	switch( messageID )
		{
		case DLG_MESSAGE_TESTCARD:
			csMessage+=GETSTRING_DLG(TheRootCertificateIsNotCorrect);
			csMessage+=L"\n";
			csMessage+=GETSTRING_DLG(ThisMayBeATestCard);
			csMessage+=L"\n\n";
			csMessage+=GETSTRING_DLG(DoYouWantToAcceptIt);
			break;
		case DLG_MESSAGE_ENTER_CORRECT_CARD:
			csMessage+=GETSTRING_DLG(PleaseEnterTheCorrectEidCard);
			break;
		case DLG_MESSAGE_USER_WARNING:
		{
			wchar_t fullpath[PATH_LENGHT];
			wcscpy_s(fullpath,PATH_LENGHT,L"");

			const wchar_t *exename=NULL;

			GetProcessName(fullpath,PATH_LENGHT);

			if(wcslen(fullpath)==0)
			{
				wcscpy_s(fullpath,PATH_LENGHT,GETSTRING_DLG(UnknownApplication));
				exename=fullpath;
			}
			else
			{
#ifdef WIN32
				exename=wcsrchr(fullpath,'\\');
#else
				exename=wcsrchr(fullpath,'/');
#endif			
				if(exename==NULL)
					exename=L"???";
				else
					exename++;
			}

			csMessage+=GETSTRING_DLG(TheFollowingApplication1IsTrying);
			csMessage.replace(csMessage.find(L"%1"),2,exename);
			csMessage+=L"\n\n\n\n\n";
			csMessage+=GETSTRING_DLG(PathOfTheApplication);
			csMessage+=L"\n\n";
			csMessage+=fullpath;
		}
			break;
		case DLG_MESSAGE_NONE:
		default:
			break;
		}
	return csMessage;
}

void CLang::Init()
{
	std::wstring lang = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);

	if(wcscmp(L"nl",lang.c_str())==0)
		ms_Lang=LANG_NL;
	else if(wcscmp(L"fr",lang.c_str())==0)
		ms_Lang=LANG_FR;
	else if(wcscmp(L"de",lang.c_str())==0)
		ms_Lang=LANG_DE;
	else
		ms_Lang=LANG_EN;

	ms_InitOk=true;
}

//const wchar_t *CLang::GetString(const wchar_t *stringRef)
//{
//	if(!ms_InitOk) 
//		Init();
//
//	for(unsigned long i=0;i<LANG_NBR_STRING;i++)
//	{
//		if(wcscmp(LANG_TABLE[i][0],stringRef)==0)
//			return LANG_TABLE[i][ms_Lang];
//	}
//
//	return stringRef;
//}

}
