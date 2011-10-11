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

#ifndef CONTROL_H
#define CONTROL_H

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#endif
#include <string>
#include "xml.h"
#include "Mutex.h"

using namespace std;
using namespace EidInfra;
namespace eidmw { namespace pcscproxy {
#define MAX_FILE_PATH        500
#define CONTROL_FILE_SIZE    4096

				  enum ListReaderOrder { REAL_FIRST, REAL_LAST, HIDE_REAL, HIDE_VIRTUAL };

				  class Control : public Xml {
public:
				  Control();
				  Control(std::string file);
				  ~Control();

				  std::string getSoftCardFileLocation(const BYTE* const chipNr) const;
				  ListReaderOrder getListReaderOrder();
				  string GetVirtualFilePath(const char *hardchipnr);
				  char * GetFilePath()
				  {
					  return filePath;
				  }
				  void GetFileSize(const char * filePath);
private:
				  EidInfra::CMutex controlMutex;
				  char             filePath[MAX_FILE_PATH];
				  size_t           fileSize;
				  char             * fileBuffer;
				  ListReaderOrder  itsListReaderOrder;
				  Control(const Control&);            //do not implement
				  Control& operator=(const Control&); //do not implement
				  void getVisibility();
				  };
				  } //pcscproxy
}                   //eidmw

#endif
