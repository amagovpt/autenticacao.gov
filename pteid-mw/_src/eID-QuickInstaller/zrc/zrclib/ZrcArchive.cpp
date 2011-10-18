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

#include "StdAfx.h"
#include "ZrcArchive.h"
#include "ZrcConsts.h"

#ifdef _WINDOWS_
#include <Winsock2.h>
#define FORMAT_ERR(m, l, s, p) (sprintf_s(m, l, s, p))
#else
#include <sys/socket.h>
#define FORMAT_ERR(m, l, s, p) (snprintf(m, l, s, p))
typedef unsigned long  u_long;
#endif

ZrcArchive::ZrcArchive(const string filename) 
	: _filename(filename)
	, _currenthdr(NULL)
	, _insize(0) 
{
}

ZrcArchive::~ZrcArchive(void) 
{
    CloseArchive();
}

void ZrcArchive::CreateArchive() 
{
    char msg[1024];
    if (_istream.is_open()) 
	{        
        FORMAT_ERR(msg, sizeof(msg), "Archive %s already open for input.", _filename.c_str());
        throw &msg;
    }
    _ostream.open(_filename.c_str(), ios::binary);
    if (! _ostream.good()) 
	{
        FORMAT_ERR(msg, sizeof(msg), "Could not open archive %s for output.", _filename.c_str());
        throw &msg;
    }
}

void ZrcArchive::OpenArchive() 
{
    char msg[1024];
    if (_ostream.is_open()) 
	{        
        FORMAT_ERR(msg, sizeof(msg), "Archive %s already open for output.", _filename.c_str());
        throw &msg;
    }
    _istream.open(_filename.c_str(), ios::binary);
    if (! _istream.good()) 
	{
        FORMAT_ERR(msg, sizeof(msg), "Could not open archive %s for input.", _filename.c_str());
        throw &msg;
    }
    _istream.seekg(0, ios::end);
    _insize = _istream.tellg();
    _istream.seekg(0, ios::beg);

}

void ZrcArchive::CloseArchive() 
{
    if (_ostream.is_open()) 
	{    
        _ostream.close();
    }
    if (_istream.is_open()) 
	{    
        _istream.close();
    }
}

void ZrcArchive::_writecookie() 
{
    _writestring(COOKIE);
}

void ZrcArchive::_writestring(const string& data) 
{ 
    INT32 len = (INT32)data.size();
    UINT32 no_len = htonl(len);  
    _ostream.write((char *)&no_len, sizeof(no_len)); 
    if (! _ostream.good()) 
	{
        throw _WRITE_ERR;
    }
    _ostream.write(data.c_str(), len);
    if (! _ostream.good()) 
	{
        throw _WRITE_ERR;
    }
}

void ZrcArchive::_writesize(const streamsize size) 
{
    streamsize len = sizeof(size);
    UINT32 no_len = htonl(len);  
    _ostream.write((char *)&no_len, sizeof(no_len)); 
    if (! _ostream.good()) 
	{
        throw _WRITE_ERR;
    }
    _ostream.write((char *)&size, len);
    if (! _ostream.good()) 
	{
        throw _WRITE_ERR;
    }

}

void ZrcArchive::AddFileHeader(ZrcFileHeader *hdr) {

    if (! _ostream.is_open())
        throw _NOT_OPEN_ERROR;
    _currenthdr = hdr;
    _writecookie();
    _writestring(hdr->get_Reader());
    _writestring(hdr->get_OS());
    _writestring(hdr->get_Filename());
    _writesize(hdr->get_Filesize());

}

void ZrcArchive::AddFileData(char *data) 
{
    _ostream.write(data, _currenthdr->get_Filesize());
    if (! _ostream.good()) 
	{
        throw _WRITE_ERR;
    }
}

string ZrcArchive::_readstring() 
{
    UINT32 no_len;
    _istream.read((char *)&no_len, sizeof(no_len));
    if (! _istream.good())
	{
        throw _READ_ERROR;
	}
    INT32 len = ntohl(no_len);
    auto_vec<char> buf(new char[len + 1]);
    char *data = buf.get();
    _istream.read(data, len);
    if (! _istream.good())
        throw _READ_ERROR;
    if ((len < 0) || (len > 512)) 
        throw _INVALID_STRING_LENGTH;
    data[len] = 0;
    string Result = data;
    return Result;
}

void ZrcArchive::_readcookie() 
{
    string scookie = _readstring();
    if (scookie.compare(COOKIE) != 0)
        throw _INVALID_FORMAT;
}

streamsize ZrcArchive::_readsize() 
{
    UINT32 no_len;
    _istream.read((char *)&no_len, sizeof(no_len));
    if (! _istream.good())
        throw _READ_ERROR;
    INT32 len = ntohl(no_len);    
    if (len != sizeof(UINT32)) 
        throw _INVALID_FORMAT;
    _istream.read((char *)&len, sizeof(len));
    if (! _istream.good())
        throw _READ_ERROR;
    return len;
}

ZrcFileHeader *ZrcArchive::ReadFileHeader() 
{
    if (! _istream.is_open())
        throw _NOT_OPEN_ERROR;

    long strpos = _istream.tellg();
    if (strpos >= _insize)
        return NULL;

    _readcookie();

    string reader = _readstring();
    string os = _readstring();
    string fn = _readstring();
    streamsize size = _readsize();

    ZrcFileHeader *Result = new ZrcFileHeader(reader, os, fn, size);

    return Result;
}

char *ZrcArchive::ReadFileData(const long count) 
{

    char *data = new char[count + 1];
    _istream.read(data, count);
    if (! _istream.good())
        throw _READ_ERROR;
    data[count] = 0;
    return data;
}

void ZrcArchive::SkipFileData(const long count) 
{
    long pos = _istream.tellg();
    if (! _istream.good())
        throw _READ_ERROR;
    _istream.seekg(pos + count);
    if (! _istream.good())
        throw _READ_ERROR;
}