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

#include "FileStructure.h"
#include "HexUtils.h"
#include <cstring>

using namespace eidmw::pcscproxy;

FileStructure::FileStructure() : root(NULL), selected(NULL)
{
	EidInfra::CAutoMutex autoMutex(&fsMutex);

	root     = new File(ROOT_ID);
	selected = root;
}

FileStructure::~FileStructure()
{
	if (root)
	{
		delete root;
	}
}

size_t FileStructure::addAll(const std::vector<std::string> &files)
{
	EidInfra::CAutoMutex autoMutex(&fsMutex);

	size_t               addCnt = 0;
	for (std::vector<std::string>::const_iterator iter = files.begin(); iter != files.end(); iter++)
	{
		const std::string &    s   = *iter;
		std::string::size_type loc = s.find(',', 0); //locate the comma separator

		if (loc == std::string::npos)
		{
			break; //missing content
		}

		std::string path    = s.substr(0, loc);
		std::string content = s.substr(loc + 1);

		if (path.size() > 2 * MAX_PATH_LEN)
		{
			break; //path too long
		}

		if (content.size() > 2 * MAX_FILE_SIZE)
		{
			break; //file size too large
		}

		BYTE   pathBuf[MAX_PATH_LEN];
		size_t pathBufLen = MAX_PATH_LEN;
		HexUtils::hexstring2hex(path.c_str(), pathBuf, &pathBufLen);

		if (pathBufLen < 4 || memcmp(pathBuf, ROOT_ID, 2) != 0)
		{
			break; //path does not start with root
		}
		File *f = root->searchPath(pathBuf + 2, pathBufLen - 2, true);
		if (!f)
		{
			break;
		}

		BYTE   *contentBuf   = new BYTE[MAX_FILE_SIZE];
		size_t contentBufLen = MAX_FILE_SIZE;
		HexUtils::hexstring2hex(content.c_str(), contentBuf, &contentBufLen);

		f->content     = new BYTE[contentBufLen];
		f->content_len = contentBufLen;
		memcpy(f->content, contentBuf, contentBufLen);
		addCnt++;
	}

	//Hard coded AID for the belpic directory
	File *f = root->searchId(BELPIC_DIR_ID);
	if (f && !f->aid)
	{
		f->aid_len = sizeof(BELPIC_AID);
		f->aid     = new BYTE[f->aid_len];
		memcpy(f->aid, BELPIC_AID, f->aid_len);
	}

	//Hard coded AID for the belpic applet
	if (!root->aid)
	{
		root->aid_len = sizeof(APPLET_AID);
		root->aid     = new BYTE[root->aid_len];
		memcpy(root->aid, APPLET_AID, root->aid_len);
	}

	return addCnt;
}

//////////////////////////////////////////////////////////////////////

File* File::searchId(const BYTE* _id)   //searches only below
{ for (std::vector<File*>::iterator iter = files.begin(); iter != files.end(); iter++)
  {
	  if (memcmp((*iter)->id, _id, 2) == 0)
	  {
		  return *iter;
	  }
	  File* f = (*iter)->searchId(_id);
	  if (f)
	  {
		  return f;
	  }
  }
  return NULL;
}

File* File::searchAid(const BYTE* _aid, size_t len)   //searches this and below
{   // Below we allow for len to be larger then aid_len and still
	// give a positive match. This is the according to the behaviour
	// of the real card.
	if (NULL != this->aid && len >= this->aid_len && memcmp(this->aid, _aid, aid_len) == 0)
	{
		return this;
	}
	for (std::vector<File*>::iterator iter = files.begin(); iter != files.end(); iter++)
	{
		File* f = (*iter)->searchAid(_aid, len);
		if (f)
		{
			return f;
		}
	}
	return NULL;
}

File* File::searchPath(const BYTE* path, size_t len, bool createOnTheFly)
{
	if ((len & 1) != 0 || len < 2)
	{
		return NULL;
	}
	BYTE _id[2] = { path[0], path[1] };
	for (std::vector<File*>::iterator iter = files.begin(); iter != files.end(); iter++)
	{
		File* f = *iter;
		if (memcmp(f->id, _id, 2) == 0)
		{
			if (len > 2)
			{
				return f->searchPath(path + 2, len - 2, createOnTheFly);
			}
			else
			{
				return f;
			}
		}
	}
	if (createOnTheFly)
	{
		File * f = new File(_id);
		f->parent = this;
		files.push_back(f);
		if (len > 2)
		{
			return f->searchPath(path + 2, len - 2, true);
		}
		else
		{
			return f;
		}
	}
	return NULL;
}

bool FileStructure::selectByFID(const BYTE* _id)
{
	EidInfra::CAutoMutex autoMutex(&fsMutex);

	if (memcmp(_id, ROOT_ID, 2) == 0)
	{
		selected = root;
		return true;
	}
	File *searchDir;
	if (!selected->isDir())
	{
		searchDir = selected->parent;
	}
	else
	{
		searchDir = selected;
	}
	File *f = searchDir->searchId(_id);
	if (f)
	{
		selected = f;
		return true;
	}
	return false;
}

bool FileStructure::selectByAID(const BYTE* _aid, size_t len)
{
	EidInfra::CAutoMutex autoMutex(&fsMutex);

	File                 *f = root->searchAid(_aid, len);
	if (f)
	{
		selected = f;
		return true;
	}
	return false;
}

bool FileStructure::selectByPath(const BYTE* path, size_t len)
{
	EidInfra::CAutoMutex autoMutex(&fsMutex);

	File                 *f = root->searchPath(path, len, false);
	if (f)
	{
		selected = f;
		return true;
	}
	return false;
}

const File* const FileStructure::getSelected() const
{
	return selected;
}

File::File(const BYTE* _id) : parent(NULL), id(NULL), aid(NULL), content(NULL), content_len(0), aid_len(0)
{
	id = new BYTE[2];
	memcpy(id, _id, 2);
}

File::~File()
{
	for (std::vector<File*>::iterator iter = files.begin(); iter != files.end(); iter++)
	{
		if (*iter) //no need to erase vector elements because it will be done automaticaaly, just delete the pointers inside
			delete *iter;
	}
	if (id)
		delete[] id;
	if (aid)
		delete[] aid;
	if (content)
		delete[] content;
}

bool File::isDir() const
{
	return(0x3F == id[0] || 0xDF == id[0]);
}

size_t File::size() const
{
	return content_len;
}

const BYTE* const File::getContent() const
{
	return content;
}

bool File::isRoot() const
{
	return(memcmp(this->id, ROOT_ID, 2) == 0);
}

size_t File::getPath(BYTE* path, size_t* len) const
{
	BYTE       tmpPath[MAX_PATH_LEN];
	const File *f = this;
	size_t     la = 0;
	while (!f->isRoot())
	{
		la += 2;
		memcpy(tmpPath + MAX_PATH_LEN - la, f->id, 2);
		f = f->parent;
	}
	if (*len >= la)
	{
		memcpy(path, tmpPath + MAX_PATH_LEN - la, la);
		*len = la;
	}
	else
	{
		*len = 0;
	}
	//return length available
	//if buffer len is insufficient the user use the
	//returned length to create a new buffer and call
	//this method again
	return la;
}

size_t File::getFID(BYTE* _id, size_t* len) const
{
	if (*len >= 2)
	{
		memcpy(_id, id, 2);
		*len = 2;
	}
	else
	{
		*len = 0;
	}
	return 2;
}

size_t File::getAID(BYTE* _aid, size_t* len) const
{
	if (*len >= aid_len)
	{
		memcpy(_aid, aid, aid_len);
		*len = aid_len;
	}
	else
	{
		*len = 0;
	}
	return aid_len;
}
