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

#include "APDURequest.h"
#include "Carddata.h"

using namespace eidmw::pcscproxy;

APDURequest::APDURequest(LPCBYTE pbSendBuffer, DWORD cbSendLength) : t(OTHER), buffer(pbSendBuffer), length(cbSendLength)
{
	using namespace eidmw::pcscproxy::carddata;

	if (length < 2)
		t = OTHER;
	else if (PROP_CLASS == buffer[0] && INS_GET_CARD_DATA == buffer[1])
		t = GET_CARD_DATA;
	else if (DEF_CLASS == buffer[0] && INS_READ_BINARY == buffer[1])
		t = READ_BINARY;
	else if (DEF_CLASS == buffer[0] && INS_SELECT_FILE == buffer[1])
		t = SELECT_FILE;
	else if (DEF_CLASS == buffer[0] && INS_GET_RESPONSE == buffer[1])
		t = GET_RESPONSE;

	else t = OTHER;
}

APDURequest::~APDURequest()
{
}

APDURequest APDURequest::createAPDURequest(LPCBYTE pbSendBuffer, DWORD cbSendLength)
{
	return APDURequest(pbSendBuffer, cbSendLength);
}

LPCBYTE APDURequest::getBuffer() const
{
	return buffer;
}

DWORD APDURequest::size() const
{
	return length;
}

APDURequest::TYPE APDURequest::getType() const
{
	return t;
}

bool APDURequest::isGetCardData() const
{
	return GET_CARD_DATA == t;
}

bool APDURequest::isSelectFile() const
{
	return SELECT_FILE == t;
}

bool APDURequest::isReadBinary() const
{
	return READ_BINARY == t;
}

bool APDURequest::isGetResponse() const
{
	return GET_RESPONSE == t;
}
