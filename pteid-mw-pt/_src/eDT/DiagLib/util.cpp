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
#include "util.h"
#include <string>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring wstring_From_string(std::string const& in)
{
	//----------------------------------------------------------
	// just put every char to a wchar and prevent sign extension
	//----------------------------------------------------------
	std::wstring wzString;
	for(size_t i=0;i<in.length();i++)
	{
		wchar_t c = in.at(i);
		wzString += c & 0x00ff;
	}
	return wzString;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void wstring_TrimR(std::wstring *in)
{
	if(in == NULL)
		return;
		
	for(size_t i=in->length();i>0;)
	{
		i--;
		if(in->at(i) == L' ')
		{
			in->at(i) = L'\0';
		}
		else
		{
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void vector_From_bytes(std::vector<unsigned char> *out, const unsigned char *in, size_t len)
{
	out->clear();
	out->reserve(len);

    for(size_t i = 0; i<len; i++)
		out->push_back(in[i]);

}

////////////////////////////////////////////////////////////////////////////////////////////////
void TokenizeS (const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters, unsigned long maxToken)
{
	tokens.clear();
	
	// Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	size_t count = 0;
    while ((maxToken == 0 || count < maxToken) && (std::string::npos != pos || std::string::npos != lastPos))
    {
        // Found a token, add it to the vector.
		// For the last token accepted we take up to the end of string
		if (pos != (size_t) -1 && (maxToken == 0 || count < (maxToken-1)))
			tokens.push_back(str.substr(lastPos, pos - lastPos));
        else 
			tokens.push_back(str.substr(lastPos, str.length() - lastPos));
		// Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
		
		count++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void TokenizeW (const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters, unsigned long maxToken)
{
	tokens.clear();
	
    // Skip delimiters at beginning.
    std::wstring::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::wstring::size_type pos     = str.find_first_of(delimiters, lastPos);

	size_t count = 0;
    while ((maxToken == 0 || count < maxToken) && (std::string::npos != pos || std::string::npos != lastPos))
    {
        // Found a token, add it to the vector.
		// For the last token accepted we take up to the end of string
		if (pos != (size_t) -1 && (maxToken == 0 || count < (maxToken-1)))
			tokens.push_back(str.substr(lastPos, pos - lastPos));
        else 
			tokens.push_back(str.substr(lastPos, str.length() - lastPos));
		// Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
		
		count++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

void wstring_to_lower(std::wstring& str)
{
       std::transform(str.begin(),str.end(),str.begin(),(int(*)(int))tolower);
}

wchar_t clean_char(wchar_t in)
{
       switch(in)
       {
               case L'.':
                       return '_';
               case L' ':
                       return '_';
               default:
                       return in;
       }
}


void wstring_clean(std::wstring& str)
{
       std::transform(str.begin(),str.end(),str.begin(),clean_char);
}



