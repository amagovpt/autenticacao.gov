#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include "util.h"
#include "WinRegKey.h"
using namespace std;

WinRegKeyExeption::WinRegKeyExeption(LONG errorcode, HKEY rootkey, wstring subkey, wstring label=L"")
:m_errorcode(errorcode),m_key(towstring<HKEY>(rootkey))
{
	m_key.append(L"\\");
	m_key.append(subkey);
	if(label.length()>0)
	{
		m_key.append(L"\\");
		m_key.append(label);
	}
}

const LONG		WinRegKeyExeption::errorcode(void) const {return m_errorcode;}
const wstring	WinRegKeyExeption::key(void)		 const {return m_key;}

WinRegKey::WinRegKey(const HKEY rootkey,const wstring subkey)
:m_rootkey(rootkey),m_subkey(subkey),m_key(0)
{
	LONG res=RegOpenKeyEx(rootkey,subkey.c_str(),0,KEY_QUERY_VALUE,&m_key);
	if(res!=ERROR_SUCCESS)
		throw WinRegKeyExeption(res,rootkey,subkey);
}

WinRegKey::~WinRegKey() throw()
{
	if(m_key!=0)
		RegCloseKey(m_key);
}

const wstring WinRegKey::operator()(wstring label) const
{
	wchar_t		buf[255]	={0};
	DWORD		dwType		=0;
	DWORD		dwBufSize	=255;
	LONG res=RegQueryValueEx(m_key,label.c_str(),0,&dwType,(BYTE*)buf,&dwBufSize);
	if(res!=ERROR_SUCCESS)
		throw WinRegKeyExeption(res,m_rootkey,m_subkey,label);
	return wstring(buf);
}

const HKEY WinRegKey::rootkey(void)		 const { return m_rootkey; }
const wstring WinRegKey::subkey(void) const { return m_subkey; }


