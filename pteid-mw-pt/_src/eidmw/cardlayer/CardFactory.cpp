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
/**
 * The CardConnect() function returns a pointer to a CCard object
 * (that should free()-ed when no longer used) that can be used
 * to communicate to a specific card.
 *
 */
#include "CardFactory.h"
#include "UnknownCard.h"
#include "../common/Log.h"
#include "../common/Util.h"

#include "PteidCard.h"
#include <vector>
#include <string>

namespace eIDMW
{


CCard * CardConnect(const std::string &csReader, CContext *poContext, GenericPinpad *poPinpad)
{
	CCard *poCard = NULL;
	long lErrCode = EIDMW_ERR_CHECK; // should never be returned
	const char *strReader = NULL;

	if (poContext->m_ulConnectionDelay != 0)
		CThread::SleepMillisecs(poContext->m_ulConnectionDelay);

	// Try if we can connect to the card via a normal SCardConnect()
	SCARDHANDLE hCard = 0;
	try 
	{
		hCard = poContext->m_oPCSC.Connect(csReader);
		if (hCard == 0) {
			goto done;
		}
	}
	catch(CMWException &e)
	{
		if (e.GetError() == (long)EIDMW_ERR_NO_CARD)
			goto done;
		if (e.GetError() != (long)EIDMW_ERR_CANT_CONNECT && e.GetError() != (long)EIDMW_ERR_CARD_COMM)
			throw;
		lErrCode = e.GetError();
		hCard = 0;
	}

	strReader = csReader.c_str();

	if (hCard != 0) {
		if (poCard == NULL) {

			MWLOG(LEV_DEBUG, MOD_CAL, "Using Reader: %s", csReader.c_str());

			//2018-05 Gemsafe cards are the only ones in use by now
			int appletVersion = 1;
			poCard = PTeidCardGetVersion(appletVersion, strReader, hCard, poContext, poPinpad);
			// If no other CCard subclass could be found
	    	if (poCard == NULL)
	    	{
				poCard = new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
			}

			hCard = 0;
		}
	}
	else
	{
		throw CMWEXCEPTION(lErrCode);
	}

done:
	return poCard;
}


}
