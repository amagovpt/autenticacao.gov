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
#include <string.h>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////////////////////
std::string string_From_wstring(std::wstring const& in)
{
    std::string out(in.size(), 0);
    for(std::wstring::size_type i = 0; in.size() > i; ++i)
      out[i] = std::use_facet<std::ctype<wchar_t> >(std::locale()).narrow(in[i]);
    return out;
}

