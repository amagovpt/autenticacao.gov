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
/** \file SISCard.h
Read the social security SIS-card and decode the data into useable fields
*/

#ifndef __SISCARD_H__
#define __SISCARD_H__

#include "../Card.h"

using namespace eIDMW;

// If we want to 'hardcode' this plugin internally in the CAL, this function
// can't be present because it's the same for all plugins
#ifndef CARDPLUGIN_IN_CAL
EIDMW_CAL_API CCard * ConnectGetCardInstance(unsigned long ulVersion,const char *csReader,
	CContext *poContext, CPinpad *poPinpad);
EIDMW_CAL_API CCard *GetCardInstance(unsigned long ulVersion, const char *csReader,
	unsigned long hCard, CContext *poContext, CPinpad *poPinpad);
#endif

CCard * SISCardConnectGetInstance(unsigned long ulVersion,const char *csReader,
	CContext *poContext, CPinpad *poPinpad);
CCard *SISCardGetInstance(unsigned long ulVersion, const char *csReader,
	SCARDHANDLE hCard, CContext *poContext, CPinpad *poPinpad);

namespace eIDMW
{

class CSISCard : public CCard
{
public:
	CSISCard(SCARDHANDLE hCard, CContext *poContext,
		CPinpad *poPinpad, const CByteArray & oData);
    ~CSISCard(void);

    virtual CByteArray ReadUncachedFile(const std::string & csPath,
        unsigned long ulOffset = 0, unsigned long ulMaxLen = FULL_FILE);

    tCardType GetType();

    std::string GetSerialNr();

	std::string GetLabel();

    CByteArray GetATR();

private:
	tCacheInfo GetCacheInfo(const std::string &csPath);

    CByteArray  m_oCardAtr;
    CByteArray  m_oCardData;
};

}

#endif

