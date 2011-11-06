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
#include "CardReaderInfo.h"

#include "CardLayer.h"

namespace eIDMW
{

CCardReaderInfo *CCardReaderInfo::m_CardReaderInfo = NULL;

// CCardReaderInfo::CCardReaderInfo()
CCardReaderInfo::CCardReaderInfo(void)
: m_NoOfReaders(-1)
{
}

// CCardReaderInfo *CCardReaderInfo::GetCardReaderInfo()
CCardReaderInfo *CCardReaderInfo::GetCardReaderInfo()
{
	if (m_CardReaderInfo == NULL)
	{
		m_CardReaderInfo = new CCardReaderInfo();
	}
	return m_CardReaderInfo;
}

// void CCardReaderInfo::CollectInfo(void)
void CCardReaderInfo::CollectInfo(void)
{
	CCardLayer oCardLayer;
	CReadersInfo oReadersInfo = oCardLayer.ListReaders();
	m_NoOfReaders = oReadersInfo.ReaderCount();
}

} // namespace eidMW
