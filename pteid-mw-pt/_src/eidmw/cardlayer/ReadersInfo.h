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

#include <string>
#include "PCSC.h"

namespace eIDMW
{

class CCardLayer;

class EIDMW_CAL_API CReadersInfo
{
public:

	/**
	 * Creates an empty object
	 */
    CReadersInfo();

	/**
	 * Return the amount of readers, this value never changes.
	 */
    unsigned long ReaderCount();

	/**
	 * Return the reader name, this value never changes.
	 * - ulIndex ranges from 0 to (but not included) ReaderCount.
	 */
	std::string ReaderName(unsigned long ulIndex);

	/**
	 * Checks if cards have been inserted/removed (by calling
	 * the SCardGetStatusChange function).
	 * Specify ulTimeout = TIMEOUT_INFINITE to never time out,
 	 * ulIndex ranges from 0 to (but not included) ReaderCount;
	 * or can be ALL_READERS to check for all readers.
	 * Returns true if something changed, false otherwise (this
	 * corresponds to whether or not the timeout has been reached).
	 */
	bool CheckReaderEvents(
		unsigned long ulTimeout = TIMEOUT_INFINITE,
		unsigned long ulIndex = ALL_READERS);

	/**
	 * Whether or not the state of the reader has changed since the last Update()
	 * - ulIndex ranges from 0 to (but not included) ReaderCount.
	 */
	bool ReaderStateChanged(unsigned long ulIndex); // card inserted or removed

	/**
	 * Whether or not the last Update() found a card in the reader
	 * - ulIndex ranges from 0 to (but not included) ReaderCount.
	 */
    bool CardPresent(unsigned long ulIndex);

private:
    CReadersInfo(CPCSC *poPCSC, const CByteArray & oReaders);

	bool bFirstTime;
	unsigned long m_ulReaderCount;
    tReaderInfo m_tInfos[MAX_READERS];

	friend class CCardLayer; // calls the CReadersInfo constructor

    CPCSC *m_poPCSC;
};

}
