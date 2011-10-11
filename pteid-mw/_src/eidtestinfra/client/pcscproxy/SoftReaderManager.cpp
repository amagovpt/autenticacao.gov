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

#include "SoftReaderManager.h"
#include "SoftReader.h"
#include "SoftCard.h"
#include "Control.h"
#include "Logger.h"
#include "Mutex.h"
#include <algorithm>
#include <cstring>

using namespace eidmw::pcscproxy;

static EidInfra::CMutex srMutex;

SoftReaderManager::SoftReaderManager() : itsControl(0)
{
	logInfo("==========================================================\n");
	logInfo("BE eID PCSC proxy loaded on %s\n", logGetTimeStr("%Y-%m-%d %H:%M:%S").c_str());

	EidInfra::CAutoMutex autoMutex(&srMutex);

	itsControl = new Control();
}

SoftReaderManager::~SoftReaderManager()
{
	// delete SoftReaders
	for (std::vector<SoftReader*>::iterator iter = readers.begin(); iter != readers.end(); iter++)
	{
		if (*iter) //no need to erase vector elements because it will be done automaticaaly, just delete the pointers inside
			delete *iter;
	}
	if (itsControl)
	{
		delete itsControl;
	}
}

DWORD SoftReaderManager::calcListReadersLength(LPCTSTR mszReaders, DWORD len)
{
	typedef std::vector<std::string>   str_vector;
	str_vector hardnames;
	DWORD      totalLen = 0;
	size_t     cnt      = takeFromMultiString(mszReaders, len, hardnames);
	if (cnt > 0)
	{
		for (str_vector::const_iterator iter = hardnames.begin(); iter != hardnames.end(); iter++)
		{
			totalLen += (DWORD) (*iter).size();
		}
		totalLen *= 2;                                           //multiply by 2 because for each hard name we have a soft name
		totalLen += (DWORD) cnt * SOFTREADER_NAME_PREFIX.size(); //add prefixe lengths
		totalLen += (DWORD) (cnt * 2) + 1;                       //null terminators for each name + 1 to terminate multistring
	}
	else
	{
		totalLen = 2; //double null terminates the multistring
	}
	return totalLen;
}

size_t SoftReaderManager::takeFromMultiString(LPCTSTR mszReaders, DWORD len, std::vector<std::string>& strs)
{
	char   buf[1024];
	size_t addCount          = 0;
	size_t buf_offset        = 0;
	size_t mszReaders_offset = 0;
	for (unsigned int i = 0; i < len; i++)
	{
		char c = mszReaders[mszReaders_offset++];
		if (c != '\0')
		{
			buf[buf_offset++] = c;
		}
		else if (c == '\0')
		{
			if (buf_offset != 0)
			{
				buf[buf_offset] = '\0';
				strs.push_back(buf);
				buf_offset = 0;
				addCount++;
			}
			//check for null again because two null terminators indicate
			//the end of the multi string
			c = mszReaders[mszReaders_offset];
			if (c == '\0')
				break;
		}
	}
	return addCount;
}

size_t SoftReaderManager::createSoftReaders(LPTSTR mszReaders, LPDWORD pcchReaders)
{
	//Parse the multi string and place the name of
	//each hard reader inside a vector
	size_t     createCnt = 0;
	typedef std::vector<std::string>   str_vector;
	str_vector hardnames;
	takeFromMultiString(mszReaders, *pcchReaders, hardnames);

	EidInfra::CAutoMutex autoMutex(&srMutex);

	// remove the soft readers whos corresponding hard reader has been removed
again:
	for (std::vector<SoftReader*>::iterator it = readers.begin(); it != readers.end(); it++)
	{
		std::string hardReaderName = (*it)->getHardReaderName();
		bool hardReaderStilPresent = false;
		for (str_vector::const_iterator iter = hardnames.begin(); iter != hardnames.end() && !hardReaderStilPresent; iter++)
		{
			if (*iter == hardReaderName)
				hardReaderStilPresent = true;
		}
		if (!hardReaderStilPresent)
		{
			delete *it;
			readers.erase(it);
			goto again;
		}
	}

	//remove names of hard readers for which we already have a soft reader
	for (std::vector<SoftReader*>::const_iterator iter = readers.begin(); iter != readers.end(); iter++)
	{
		str_vector::iterator hardname = std::find(hardnames.begin(), hardnames.end(), (*iter)->getHardReaderName());
		if (hardname != hardnames.end())
		{
			hardnames.erase(hardname);
		}
	}

	//create a new soft reader for each hard reader
	for (str_vector::const_iterator iter = hardnames.begin(); iter != hardnames.end(); iter++)
	{
		std::string softname(SOFTREADER_NAME_PREFIX);
		softname.append(*iter);
		SoftReader  *sr = new SoftReader(*iter, softname);
		readers.push_back(sr);
		createCnt++;
	}

	ListReaderOrder order = itsControl->getListReaderOrder();

	//copy softnames and hardnames into a vector so that they can be sorted
	str_vector returnListNames;
	for (std::vector<SoftReader*>::const_iterator iter = readers.begin(); iter != readers.end(); iter++)
	{
		if (order != HIDE_REAL)
			returnListNames.push_back((*iter)->getHardReaderName());
		if (order != HIDE_VIRTUAL)
			returnListNames.push_back((*iter)->getSoftReaderName());
	}

	//sort it
	switch (order) {
	case REAL_FIRST:
		std::sort(returnListNames.begin(), returnListNames.end(), cmpVirtualNameLast);
		break;
	case REAL_LAST:
		std::sort(returnListNames.begin(), returnListNames.end(), cmpVirtualNameFirst);
		break;
	default:
		std::sort(returnListNames.begin(), returnListNames.end());
		break;
	}

	//copy softnames and hardnames into mszReaders buffer as a multistring
	size_t mszReaders_offset = 0;
	for (str_vector::const_iterator iter = returnListNames.begin(); iter != returnListNames.end(); iter++)
	{
		std::string name = *iter;
		memcpy(mszReaders + mszReaders_offset, name.c_str(), (name.size() + 1) * sizeof(char));
		mszReaders_offset += name.size() + 1;
	}
	//terminate the multistring with a second null terminator
	mszReaders[mszReaders_offset] = '\0';
	*pcchReaders                  = (DWORD) mszReaders_offset + 1;
	return createCnt;
}

SoftReader* const SoftReaderManager::getSoftReaderByName(LPCTSTR szReader, T_NAME reader_type) const
{
	EidInfra::CAutoMutex autoMutex(&srMutex);

	for (std::vector<SoftReader*>::const_iterator iter = readers.begin(); iter != readers.end(); iter++)
	{
		SoftReader *sr = *iter;

		switch (reader_type) {
		case N_SOFT_OR_HARD:
			if (sr->getSoftReaderName().compare(szReader) == 0 || sr->getHardReaderName().compare(szReader) == 0)
				return sr;
			break;
		case N_SOFT:
			if (sr->getSoftReaderName().compare(szReader) == 0)
				return sr;
			break;
		case N_HARD:
			if (sr->getHardReaderName().compare(szReader) == 0)
				return sr;
			break;
		}
	}
	return NULL;
}

SoftReader* const SoftReaderManager::getSoftReaderByCardHandle(SCARDHANDLE hCard, T_HANDLE handle_type) const
{
	EidInfra::CAutoMutex autoMutex(&srMutex);

	for (std::vector<SoftReader*>::const_iterator iter = readers.begin(); iter != readers.end(); iter++)
	{
		SoftReader *sr = *iter;
		if (sr->getSoftCard())
		{
			switch (handle_type) {
			case H_SOFT_OR_HARD:
				if (sr->getSoftCard()->getSoftHandle() == hCard || sr->getSoftCard()->getHardHandle() == hCard)
					return sr;
				break;
			case H_SOFT:
				if (sr->getSoftCard()->getSoftHandle() == hCard)
					return sr;
				break;
			case H_HARD:
				if (sr->getSoftCard()->getHardHandle() == hCard)
					return sr;
				break;
			}
		}
	}
	return NULL;
}

const Control* const SoftReaderManager::getControl() const
{
	return itsControl;
}

bool SoftReaderManager::cmpVirtualNameFirst(const std::string& a, const std::string& b)
{
	if (0 == a.find_first_of(SOFTREADER_NAME_PREFIX) && 0 == b.find_first_of(SOFTREADER_NAME_PREFIX))
	{
		return a.compare(b) < 0;    //a and b are both soft names
	}
	if (0 != a.find_first_of(SOFTREADER_NAME_PREFIX) && 0 == b.find_first_of(SOFTREADER_NAME_PREFIX))
	{
		return false;   //a is hard, b is soft
	}
	if (0 == a.find_first_of(SOFTREADER_NAME_PREFIX) && 0 != b.find_first_of(SOFTREADER_NAME_PREFIX))
	{
		return true;            //a is soft, b is hard
	}
	return a.compare(b) < 0;    //a and b are both hard names
}

bool SoftReaderManager::cmpVirtualNameLast(const std::string& a, const std::string& b)
{
	if (0 == a.find_first_of(SOFTREADER_NAME_PREFIX) && 0 == b.find_first_of(SOFTREADER_NAME_PREFIX))
	{
		return a.compare(b) < 0;    //a and b are both soft names
	}
	if (0 != a.find_first_of(SOFTREADER_NAME_PREFIX) && 0 == b.find_first_of(SOFTREADER_NAME_PREFIX))
	{
		return true;    //a is hard, b is soft
	}
	if (0 == a.find_first_of(SOFTREADER_NAME_PREFIX) && 0 != b.find_first_of(SOFTREADER_NAME_PREFIX))
	{
		return false;           //a is soft, b is hard
	}
	return a.compare(b) < 0;    //a and b are both hard names
}

