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

#include "SoftReader.h"
#include "SoftCard.h"
#include "Transaction.h"

using namespace eidmw::pcscproxy;

SoftReader::SoftReader(const std::string& hardName, const std::string& softName) : itsSoftCard(NULL),
	itsHardName(hardName), itsSoftName(softName), itsTransaction(NULL)
{
	EidInfra::CAutoMutex autoMutex(&srMutex);

	itsTransaction = new Transaction(itsHardName.c_str());
	lastCmdToCard  = false;
}

SoftReader::~SoftReader()
{
	if (itsSoftCard)
	{
		delete itsSoftCard;
	}
	if (itsTransaction)
	{
		delete itsTransaction;
	}
}

const std::string& SoftReader::getHardReaderName() const
{
	return itsHardName;
}

const std::string& SoftReader::getSoftReaderName() const
{
	return itsSoftName;
}

SoftCard* const SoftReader::getSoftCard() const
{
	return itsSoftCard;
}

LONG SoftReader::transmit(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const
{
	return itsSoftCard->transmit(req, pbRecvBuffer, pcbRecvLength, itsTransaction);
}

SoftCard* SoftReader::createSoftCard(SCARDHANDLE hardHandle)
{
	EidInfra::CAutoMutex autoMutex(&srMutex);

	if (itsSoftCard)
	{
		if (itsSoftCard->getHardHandle() != hardHandle)
		{
			SoftCard *tmpSoftCard = itsSoftCard;
			itsSoftCard = new SoftCard(hardHandle, hardHandle / 2);
			delete tmpSoftCard;
		}
	}
	else
	{
		itsSoftCard = new SoftCard(hardHandle, hardHandle / 2);
	}
	return itsSoftCard;
}

LONG SoftReader::disconnect(DWORD dwDisposition)
{
	EidInfra::CAutoMutex autoMutex(&srMutex);

	//Just delete the soft card. It makes no sence to reset the soft card because
	//this is a local instance only
	switch (dwDisposition) {
	case SCARD_LEAVE_CARD:
	case SCARD_RESET_CARD:
	case SCARD_UNPOWER_CARD:
	case SCARD_EJECT_CARD:
		if (itsSoftCard)
		{
			delete itsSoftCard;
			itsSoftCard = NULL;
		}
		return SCARD_S_SUCCESS;
	}
	return SCARD_E_INVALID_PARAMETER;
}

LONG SoftReader::reconnect(DWORD dwInitialization)
{
	lastCmdToCard = false;
	switch (dwInitialization) {
	case SCARD_LEAVE_CARD:
		//do nothing
		return SCARD_S_SUCCESS;
	case SCARD_RESET_CARD:
	case SCARD_UNPOWER_CARD:
		itsSoftCard->reset();
		return SCARD_S_SUCCESS;
	case SCARD_EJECT_CARD:
		if (itsSoftCard)
		{
			delete itsSoftCard;
			itsSoftCard = NULL;
		}
		return SCARD_S_SUCCESS;
	}
	return SCARD_E_INVALID_PARAMETER;
}

LONG SoftReader::beginTransaction()
{
	itsTransaction->aquire();
	return SCARD_S_SUCCESS;
}

LONG SoftReader::endTransaction(DWORD dwDisposition)
{
	//if(!itsTransaction->isAlive()) {
	//	return SCARD_E_NOT_TRANSACTED;
	//}
	itsTransaction->release();

	EidInfra::CAutoMutex autoMutex(&srMutex);

	switch (dwDisposition) {
	case SCARD_LEAVE_CARD:
		//do nothing
		return SCARD_S_SUCCESS;
	case SCARD_RESET_CARD:
	case SCARD_UNPOWER_CARD:
		itsSoftCard->reset();
		return SCARD_S_SUCCESS;
	case SCARD_EJECT_CARD:
		if (itsSoftCard)
		{
			delete itsSoftCard;
			itsSoftCard = NULL;
		}
		return SCARD_S_SUCCESS;
	}
	return SCARD_E_INVALID_PARAMETER;
}

bool SoftReader::firstTimeInTransaction() const
{
	return itsTransaction->justStarted();
}

void SoftReader::setLastCmdToCard(bool lastCmdToCard)
{
	EidInfra::CAutoMutex autoMutex(&srMutex);

	this->lastCmdToCard = lastCmdToCard;
}

bool SoftReader::lastCmdWasToCard() const
{
	return lastCmdToCard;
}
