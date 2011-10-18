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

#include "SoftCard.h"
#include "HexUtils.h"
#include "Carddata.h"
#include "FileStructure.h"
#include "Transaction.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <memory>
#ifdef _WIN32
#include <direct.h> // for getcwd
#endif
#include <stdlib.h> // for MAX_PATH

#include <virtualCardAccess.h>

using namespace eidmw::pcscproxy;
using namespace eidmw::pcscproxy::carddata;

SoftCard::SoftCard(SCARDHANDLE hardhandle, SCARDHANDLE softhandle) : itsHardHandle(hardhandle), itsSoftHandle(softhandle),
	chipnr(NULL), versionnumbers(NULL), fs(NULL), responseBuffer(NULL)
{
	responseBuffer = new ArrayBuffer<BYTE>();
}

SoftCard::~SoftCard()
{
	if (chipnr)
		delete[] chipnr;
	if (versionnumbers)
		delete[] versionnumbers;
	if (fs)
		delete fs;
	if (responseBuffer)
		delete responseBuffer;
}

SCARDHANDLE SoftCard::getHardHandle() const
{
	return itsHardHandle;
}

SCARDHANDLE SoftCard::getSoftHandle() const
{
	return itsSoftHandle;
}

bool SoftCard::loadFromFile(const std::string &filename, const BYTE* hardChipNr)
{
	EidInfra::CAutoMutex autoMutex(&scMutex);

	//Test if file is readable
	std::ifstream inp(filename.c_str(), std::ifstream::in);
	if (!inp)
	{
		return false;
	}
	inp.close();

	EidInfra::VirtualCard vCard(filename.c_str());

	//Verify correct chipnr before continue
	size_t len = CHIP_NR_LEN;
	BYTE   hardChipNrFromFile[CHIP_NR_LEN];
	BYTE   softChipNrFromFile[CHIP_NR_LEN];
	BYTE   versionNrFromFile[VERSION_NR_LEN];

	HexUtils::hexstring2hex(vCard.GetDevCardChipNr(), hardChipNrFromFile, &len);
	if (CHIP_NR_LEN != len)
	{
		return false;
	}

	HexUtils::hexstring2hex(vCard.GetChipnr(), softChipNrFromFile, &len);
	if (CHIP_NR_LEN != len)
	{
		return false;
	}

	HexUtils::hexstring2hex(vCard.GetVersionnumbers(), versionNrFromFile, &len);
	if (VERSION_NR_LEN != len)
	{
		return false;
	}

	//Test chipnr match
	if (0 != memcmp(hardChipNr, hardChipNrFromFile, len))
	{
		return false;
	}

	EidInfra::logInfo("Found virtual card \"%s\" for the real card with serialnr %s\n",
		filename.c_str(), vCard.GetDevCardChipNr());

	std::vector<std::string>     files = vCard.GetAllFiles2(filename.c_str());

	std::auto_ptr<FileStructure> fs_aptr(new FileStructure());
	size_t                       addCnt = fs_aptr->addAll(files);

	if (addCnt != files.size())
	{
		EidInfra::logError("For virtual card \"%s\", only %d out of %d files could be parsed\n",
			addCnt, files.size());
		return false;
	}

	if (versionnumbers)
	{
		delete[] versionnumbers;
		versionnumbers = NULL;
	}

	if (chipnr)
	{
		delete[] chipnr;
		chipnr = NULL;
	}

	if (fs)
	{
		delete fs;
		fs = NULL;
	}

	fs             = fs_aptr.release();
	chipnr         = new BYTE[CHIP_NR_LEN];
	versionnumbers = new BYTE[VERSION_NR_LEN];
	memcpy(chipnr, softChipNrFromFile, CHIP_NR_LEN);
	memcpy(versionnumbers, versionNrFromFile, VERSION_NR_LEN);

	EidInfra::logInfo("  found %d files, virt. chipnr = %s\n", addCnt, vCard.GetChipnr());

	return true;
}

LONG SoftCard::transmit(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength, Transaction *t) const
{
	t->aquire();
	LONG result;

	switch (req.getType()) {
	case APDURequest::GET_CARD_DATA:
		result = getCardData(req, pbRecvBuffer, pcbRecvLength);
		break;
	case APDURequest::READ_BINARY:
		result = readBinary(req, pbRecvBuffer, pcbRecvLength);
		break;
	case APDURequest::SELECT_FILE:
		result = selectFile(req, pbRecvBuffer, pcbRecvLength);
		break;
	case APDURequest::GET_RESPONSE:
		result = getResponse(req, pbRecvBuffer, pcbRecvLength);
		break;
	default:
		result = SCARD_E_UNSUPPORTED_FEATURE;
	}

	t->release();
	return result;
}

LONG SoftCard::getCardData(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const
{
	//If no chip nr, load data must have failed
	if (NULL == chipnr || NULL == versionnumbers)
	{
		return Carddata::cmdNotAvailable(pbRecvBuffer, pcbRecvLength);
	}

	LPCBYTE reqBuffer = req.getBuffer();

	//TEST P1 and P2 params
	const BYTE P1 = reqBuffer[2];
	const BYTE P2 = reqBuffer[3];
	if (!(0 == P1 && 0 == P2))
	{
		return Carddata::wrongParamP1P2(pbRecvBuffer, pcbRecvLength);
	}

	const size_t la = 28;
	const size_t le = (0 == reqBuffer[4] ? 256 : reqBuffer[4]);

	if (le > la)
	{
		return Carddata::leTooLong(la, pbRecvBuffer, pcbRecvLength);
	}

	if (le + 2 > *pcbRecvLength)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}

	//Prepare the local buffer
	BYTE   tmpBuffer[la];
	size_t len = 0;
	memcpy(tmpBuffer, chipnr, CHIP_NR_LEN);             //Copy chip number
	len += CHIP_NR_LEN;
	memcpy(tmpBuffer + len, versionnumbers, VERSION_NR_LEN); //Copy version number
	len           += VERSION_NR_LEN;
	tmpBuffer[len] = DEACTIVATED;                                   //Copy Application Life cycle
	len           += 1;

	//Copy le bytes in return buffer
	memcpy(pbRecvBuffer, tmpBuffer, le);

	if (le < la)
	{
		responseBuffer->put(tmpBuffer + le, la - le);
		return Carddata::bytesRemaining(la - le, pbRecvBuffer, pcbRecvLength, le);
	}
	else
	{
		return Carddata::normalEndingOfCmd(pbRecvBuffer, pcbRecvLength, le);
	}
}

LONG SoftCard::getResponse(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const
{
	LPCBYTE reqBuffer = req.getBuffer();

	//TEST P1 and P2 params
	const BYTE P1 = reqBuffer[2];
	const BYTE P2 = reqBuffer[3];
	if (!(0 == P1 && 0 == P2))
	{
		return Carddata::wrongParamP1P2(pbRecvBuffer, pcbRecvLength);
	}

	const size_t la = responseBuffer->size();
	const size_t le = (0 == reqBuffer[4] ? 256 : reqBuffer[4]);

	if (0 == la)
	{
		return Carddata::cmdNotAvailable(pbRecvBuffer, pcbRecvLength);
	}

	if (le > la)
	{
		return Carddata::leTooLong(la, pbRecvBuffer, pcbRecvLength);
	}

	if (le + 2 > *pcbRecvLength)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}

	//Copy le bytes in return reqBuffer
	memcpy(pbRecvBuffer, responseBuffer->get(), le);

	if (le < la)
	{
		responseBuffer->put(responseBuffer->get() + le, la - le);
		return Carddata::bytesRemaining(la - le, pbRecvBuffer, pcbRecvLength, le);
	}
	else
	{
		responseBuffer->clear();
		return Carddata::normalEndingOfCmd(pbRecvBuffer, pcbRecvLength, le);
	}
}

LONG SoftCard::readBinary(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const
{
	//If no file structure, load data must have failed
	if (NULL == fs)
	{
		return Carddata::cmdNotAvailable(pbRecvBuffer, pcbRecvLength);
	}

	LPCBYTE    reqBuffer = req.getBuffer();
	const BYTE P1        = reqBuffer[2];
	const BYTE P2        = reqBuffer[3];
	size_t     offset    = (P1 * 256) + P2;

	const File * file = fs->getSelected();

	if (file->isDir())
	{
		return Carddata::cmdNotAllowedFileNoEF(pbRecvBuffer, pcbRecvLength);
	}

	const size_t la = file->size();
	const size_t le = (0 == reqBuffer[4] ? 256 : reqBuffer[4]);

	if (0 == la)
	{
		return Carddata::cmdNotAvailable(pbRecvBuffer, pcbRecvLength);
	}

	if (offset >= la)
	{
		return Carddata::wrongParamP1P2(pbRecvBuffer, pcbRecvLength);
	}

	if (offset + le > la)
	{
		return Carddata::leTooLong(la - offset, pbRecvBuffer, pcbRecvLength);
	}

	if (le + 2 > *pcbRecvLength)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}

	//no check is needed to see if (le < la)
	memcpy(pbRecvBuffer, file->getContent() + offset, le);
	return Carddata::normalEndingOfCmd(pbRecvBuffer, pcbRecvLength, le);
}

LONG SoftCard::selectFile(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const
{
	//If no file structure, load data must have failed
	if (NULL == fs)
	{
		return Carddata::cmdNotAvailable(pbRecvBuffer, pcbRecvLength);
	}
	LPCBYTE      reqBuffer = req.getBuffer();
	const BYTE   P1        = reqBuffer[2];
	const size_t len       = *(reqBuffer + 4);

	#ifdef _WIN32
	if (len + 5 != req.size())
	{
		return ERROR_INVALID_PARAMETER;
	}
	#endif

	bool selected;
	switch (P1) {
	case FID:
		if (len != 2)
		{
			return Carddata::fileLcInconsistentWithP1P2(pbRecvBuffer, pcbRecvLength);
		}
		selected = fs->selectByFID(reqBuffer + 5);
		break;
	case ABSOLUTE_PATH:
		if (memcmp(ROOT_ID, reqBuffer + 5, 2) == 0)
			selected = fs->selectByPath(reqBuffer + 7, len - 2);
		else
			selected = fs->selectByPath(reqBuffer + 5, len);
		break;
	case AID:
		if (len < 5 || len > 16)
		{
			return Carddata::fileLcInconsistentWithP1P2(pbRecvBuffer, pcbRecvLength);
		}
		selected = fs->selectByAID(reqBuffer + 5, len);
		break;
	default:
		return Carddata::fileWrongParamP1P2(pbRecvBuffer, pcbRecvLength);
	}

	if (selected)
	{
		return Carddata::normalEndingOfCmd(pbRecvBuffer, pcbRecvLength);
	}
	else
	{
		return Carddata::fileNotFound(pbRecvBuffer, pcbRecvLength);
	}
}

bool SoftCard::isDataLoaded() const
{
	return chipnr != NULL;
}

void SoftCard::reset()
{
	EidInfra::CAutoMutex autoMutex(&scMutex);

	responseBuffer->clear();
	fs->selectByFID(ROOT_ID);
}

const File* const SoftCard::getSelected() const
{
	if (fs)
	{
		return fs->getSelected();
	}
	return NULL;
}

