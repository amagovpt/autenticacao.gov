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

#include "Reader.h"
#include "ReadersInfo.h"
#include "Context.h"
#include "CardLayerConst.h"
#include "../dialogs/dialogs.h"

namespace eIDMW
{

	DlgPinOperation PinOperation2Dlg(tPinOperation operation);

class EIDMW_CAL_API CCardLayer
{
public:
	/**
	 * Calling the CAL (Card Abstraction Layer) starts with the
	 * constructing a CCardLayer object.
	 * Calling the constructor doesn't cause a connection to the smart
	 * card resource manager yet (no SCardEstablishContext() yet)
	 */
    CCardLayer(void);

	~CCardLayer(void);

	/**
	 * Release the connection to the smart card resource manager (a
	 * call to SCardReleaseContext(); this is also done in the destructor.
	 * WARNING: this will cause all CReadersInfo and CReader object to
	 * become unusable. Calling afterwards one of the functions below will
	 * cause a re-connection to the smart card resource manager.
	 */
	void ForceRelease();

	/**
	 * Return the list of all available readers, plus info on the
	 * presence of cards in those readers.
	 */
	CReadersInfo ListReaders();

	/**
	 * Get a CReader object for the reader with name csReaderName;
	 * no connection is made yet to the card that might be present
	 * in the reader.
	 * The list of csReaderNames can be found with ListReaers().
	 * If csReader == "", then the first found reader is taken
	 */
	CReader & getReader(const std::string &csReaderName);

	/**
	 * Delete the cache files for the card with serial number 'csSerialNr'.
	 * To delete all cache files, leave csSerialNr empty ("").
	 * Returns true is something was deleted, false otherwise.
	 */
	bool DeleteFromCache(const std::string & csSerialNr);

private:
    // No copies allowed
    CCardLayer(const CCardLayer & oCardLayer);
    CCardLayer & operator = (const CCardLayer & oCardLayer);
	std::string * GetDefaultReader();

    CContext m_oContext;

	std::string m_szDefaultReaderName;
	unsigned long m_ulReaderCount;
	CReader *m_tpReaders[MAX_READERS];
};

}
