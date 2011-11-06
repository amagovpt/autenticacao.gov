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
#pragma once

#ifndef __ZrcFileHeader__
#define __ZrcFileHeader__

class ZrcFileHeader
{
public:
	ZrcFileHeader(std::string const& reader, std::string const& os, std::string const& filename, const long filesize);
    virtual ~ZrcFileHeader(void);
	const std::string& get_Reader()     { return _reader; }
	const std::string& get_OS()         { return _os; }
	const std::string& get_Filename()   { return _filename; }
    const long		   get_Filesize()	{ return _filesize; }

private:
    const string _reader;
    const string _os;
    const string _filename;
    const long	 _filesize;

};

#endif __ZrcFileHeader__
