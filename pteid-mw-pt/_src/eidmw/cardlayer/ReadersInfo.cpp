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
#include "ReadersInfo.h"

namespace eIDMW
{

CReadersInfo::CReadersInfo()
{
	bFirstTime = true;
	m_ulReaderCount = 0;
}

unsigned long CReadersInfo::ReaderCount()
{
	return m_ulReaderCount;
}

std::string CReadersInfo::ReaderName(unsigned long ulIndex)
{
	if (ulIndex >= m_ulReaderCount)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

    return m_tInfos[ulIndex].csReader;
}

bool CReadersInfo::CheckReaderEvents(unsigned long ulTimeout,
	unsigned long ulIndex)
{
	bool bChanged = false;

	if (bFirstTime)
	{
		m_poPCSC->GetStatusChange(0, m_tInfos, m_ulReaderCount);
		bFirstTime = false;
	}

wait_again:
	if (ulIndex == ALL_READERS)
	{
		bChanged = m_poPCSC->GetStatusChange(ulTimeout,	m_tInfos, m_ulReaderCount);
	}
	else
	{
		if (ulIndex >= m_ulReaderCount)
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

		bChanged = m_poPCSC->GetStatusChange(ulTimeout, &m_tInfos[ulIndex], 1);
	}

	// Extra safety: if nothing changed and we should wait forever
	// (for a change), the we go waiting again
	if (!bChanged &&  (ulTimeout == TIMEOUT_INFINITE))
		goto wait_again;

	return bChanged;
}

bool CReadersInfo::ReaderStateChanged(unsigned long ulIndex)
{
	if (ulIndex >= m_ulReaderCount)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

    return 0 != (m_tInfos[ulIndex].ulEventState & EIDMW_STATE_CHANGED);
}

bool CReadersInfo::CardPresent(unsigned long ulIndex)
{
	if (bFirstTime)
	{
		m_poPCSC->GetStatusChange(0, m_tInfos, m_ulReaderCount);
		bFirstTime = false;
	}

	if (ulIndex >= m_ulReaderCount)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

    return 0 != (m_tInfos[ulIndex].ulEventState & EIDMW_STATE_PRESENT);
}

/** Constructor */
CReadersInfo::CReadersInfo(CPCSC *poPCSC, const CByteArray & oReaders)
{
	m_poPCSC = poPCSC;
	bFirstTime = true;
    m_ulReaderCount = 0;

	//Parse the string reader-list into the array "m_tInfos"
	const char *csReaders = (const char *) oReaders.GetBytes();
    for (size_t i = 0;
		csReaders != NULL && csReaders[0] != '\0' && i < MAX_READERS;
		i++, m_ulReaderCount++)
    {
		//Ignore readers associated to Windows Virtual Smart Cards
                if (strstr(csReaders, "Virtual Smart Card") == NULL
                        && strstr(csReaders, "Windows Hello for Business") == NULL) {
			m_tInfos[m_ulReaderCount].csReader = csReaders;
			m_tInfos[m_ulReaderCount].ulCurrentState = 0;
			m_tInfos[m_ulReaderCount].ulEventState = 0;
			csReaders += m_tInfos[m_ulReaderCount].csReader.length() + 1;
		}
		else {
			csReaders += strlen(csReaders) + 1;
			m_ulReaderCount -= 1;
		}
        
    }
}

}
