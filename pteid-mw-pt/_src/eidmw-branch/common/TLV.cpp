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
// TLV.cpp: implementation of the CTLV class.
//
//////////////////////////////////////////////////////////////////////

#include "TLV.h"

namespace eIDMW
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTLV::CTLV()
{
    SetTag(0x00);
}

CTLV::CTLV(const CTLV & oTlv)
{
    SetTag(oTlv.m_ucTag);
    SetData(oTlv.m_Data); 
}

CTLV::CTLV(unsigned char ucTag, const unsigned char *pucData, unsigned long ulLen)
{
    SetTag(ucTag);
    SetData(pucData, ulLen); 
}

CTLV::~CTLV()
{

}

}  // namespace eIDMW
