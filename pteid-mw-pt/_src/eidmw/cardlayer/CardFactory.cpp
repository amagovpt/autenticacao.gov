/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012, 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
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
#include "Log.h"
#include "Util.h"
#include "Cache.h"

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
	const void * param_structure = NULL;

	if (poContext->m_ulConnectionDelay != 0)
		CThread::SleepMillisecs(poContext->m_ulConnectionDelay);

	// Try if we can connect to the card via a normal SCardConnect()
	SCARDHANDLE hCard = 0;
	std::pair<SCARDHANDLE, DWORD> ret;
	try 
	{
		ret = poContext->m_oPCSC.Connect(csReader);
		hCard = ret.first;

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
			MWLOG(LEV_DEBUG, MOD_CAL, "ATR input value: %s",
			poContext->m_oPCSC.GetATR(hCard).ToString(true, false).c_str());

			if (ret.second == SCARD_PROTOCOL_T0)
               param_structure = SCARD_PCI_T0;
        	else if (ret.second == SCARD_PROTOCOL_T1)
				param_structure = SCARD_PCI_T1;


			//2018-05 Gemsafe cards are the only ones in use by now
			int appletVersion = 1;
			poCard = PteidCardGetInstance(appletVersion, strReader, hCard, poContext, poPinpad, param_structure);

			CCache::LimitDiskCacheFiles(10);

			// If no other CCard subclass could be found
	    	if (poCard == NULL)
	    	{
				poCard = new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
			}

			poCard->setProtocol(param_structure);
			poCard->GenerateEncryptionKey();

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
