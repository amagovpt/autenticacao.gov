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


#ifndef __control_file_access_h__
#define __control_file_access_h__

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "xml.h"
#include "util.h"

using namespace std;

#define CONTROL_FILE_PATH    "C:\\Documents and Settings\\All Users\\PTeidTestCtrl.xml"

enum showType {
	REAL_FIRST,
	REAL_LAST,
	HIDEREAL,
	HIDE_VIRTUAL,
	NON_SHOW_TYPE  //gaurd
};

namespace EidInfra
{
class ControlFile : public Xml
{
public:
std:: vector<string> m_readers;
ControlFile(const char * path, showType type);
virtual ~ControlFile();
void Add(const char *hardchipnr, const char *file);
void Show();
void SetPath(const char *path)
{
	strcpy_s((char *) filePath, 2048, path);
}

void Save(const char * fileName);
string GetVirtualFilePath(const char *hardchipnr);
showType GetShowType();         //{return m_type;}
private:
char     filePath[2048];
showType m_type;
};
}

#endif
