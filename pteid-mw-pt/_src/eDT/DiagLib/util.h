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
#ifndef __DIAGLIB_UTIL_H__
#define __DIAGLIB_UTIL_H__

#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>

std::wstring wstring_From_string(std::string const& in);
std::string string_From_wstring(std::wstring const& in);
void wstring_TrimR(std::wstring *in);

void vector_From_bytes(std::vector<unsigned char> *out, const unsigned char *in, size_t len);

void TokenizeS (const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters, unsigned long maxToken=0);
void TokenizeW (const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters, unsigned long maxToken=0);

void wstring_to_lower(std::wstring& str);
wchar_t clean_char(wchar_t in);
void wstring_clean(std::wstring& str);

using namespace std;

template <class T>
inline wstring towstring (const T& t)
{
	wstringstream ss;
	ss << t;
	return ss.str();
}

template <class T>
inline string tostring (const T& t)
{
	stringstream ss;
	ss << t;
	return ss.str();
}

// template for joining elements into textual lists (but we should use boost instead)
template <typename rT, typename cT>
inline rT join (const cT& list, const rT& sep)
{
	rT result;
	typename cT::const_iterator i=list.begin();
	if(i!=list.end())
		result.append(*i++);
	for(;i!=list.end();i++)
	{
		result.append(sep);
		result.append(*i);
	}
	return result;
}






#endif //__DIAGLIB_UTIL_H__