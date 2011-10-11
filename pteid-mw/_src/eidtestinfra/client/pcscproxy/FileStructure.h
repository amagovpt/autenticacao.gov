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

#ifndef FILESTRUCTURE_H
#define FILESTRUCTURE_H

#include <string>
#include <vector>
#include "Mutex.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#endif

namespace eidmw { namespace pcscproxy {
				  static const BYTE   ROOT_ID[2]       = { 0x3F, 0x00 };
				  static const BYTE   BELPIC_DIR_ID[2] = { 0xDF, 0x00 };
				  static const size_t MAX_FILE_SIZE    = 98303;
				  static const size_t MAX_PATH_LEN     = 20; //allows for 10 levels deep which is more than we need
				  static const BYTE   BELPIC_AID[12]   = { 0xA0, 0x00, 0x00, 0x01, 0x77, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35 };
				  static const BYTE   APPLET_AID[15]   = { 0xA0, 0x00, 0x00, 0x00, 0x30, 0x29, 0x05, 0x70,0x00, 0xAD, 0x13, 0x10, 0x01, 0x01, 0xFF };

				  ////////////////////////////////////////////////////////////////

				  class File;
				  class FileStructure {
public:
				  FileStructure();
				  ~FileStructure();

				  size_t addAll(const std::vector<std::string>&);
				  bool selectByFID(const BYTE* _fid);
				  bool selectByAID(const BYTE* _aid, size_t len);
				  bool selectByPath(const BYTE* path, size_t len);
				  const File* const getSelected() const;

private:
				  FileStructure(const FileStructure&);            //do not implement
				  FileStructure& operator=(const FileStructure&); //do not implement

				  File             * root;
				  File             * selected;

				  EidInfra::CMutex fsMutex;
				  };

				  class File {
				  friend class FileStructure;
public:
				  ~File();
				  size_t size() const;
				  const BYTE* const getContent() const;
				  bool isDir() const;
				  size_t getPath(BYTE*, size_t* len) const;
				  size_t getFID(BYTE*, size_t* len) const;
				  size_t getAID(BYTE*, size_t* len) const;
				  bool isRoot() const;
private:
				  File(const BYTE* _id);
				  File* searchPath(const BYTE*, size_t len, bool createOnTheFly);
				  File* searchAid(const BYTE*, size_t len);
				  File* searchId(const BYTE*);
												//
				  File();                       //do not implement
				  File(const File&);            //do not implement
				  File& operator=(const File&); //do not implement
												//

				  File               * parent;
				  BYTE               * id;
				  BYTE               * aid;
				  BYTE               * content;
				  size_t             content_len;
				  size_t             aid_len;

				  std::vector<File*> files;
				  };
				  } //pcscproxy
}                   //eidmw

#endif
