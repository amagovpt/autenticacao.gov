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

#ifndef __ZrcArchive__
#define __ZrcArchive__

#include "ZrcFileHeader.h"


class ZrcArchive {
public:
    ZrcArchive(const string filename);
    virtual ~ZrcArchive(void);
    void CreateArchive(void);
    void OpenArchive(void);
    void CloseArchive(void);
    void AddFileHeader(ZrcFileHeader *hdr);
    void AddFileData(char *data);
    ZrcFileHeader *ReadFileHeader();
    char *ReadFileData(const long count);
    void ZrcArchive::SkipFileData(const long count);
   

private:

    void _writestring(const string& data);
    void _writesize(const streamsize size);
    void _writecookie(void);
    void _readcookie(void);
    string _readstring(void);
    streamsize _readsize();

    const string _filename;
    ofstream _ostream;
    ifstream _istream;
    streamsize _insize;

    ZrcFileHeader* _currenthdr;


};

#endif __ZrcArchive__
