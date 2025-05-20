/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012, 2017-2024 André Guerreiro - <aguerreiro1985@gmail.com>
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
 * to communicate with a specific card.
 *
 */
#include "CardFactory.h"
#include "IcaoCard.h"
#include "UnknownCard.h"
#include "Log.h"
#include "Cache.h"
#include "Config.h"

#include "PteidCard.h"
#include <vector>
#include <string>

namespace eIDMW {

/*
  Card type detection function for use in modules where we are supplied with a SCARDHANDLE already created, e.g. minidriver  
*/

CCard *CardConnect(SCARDHANDLE hCard, DWORD protocol, const std::string &csReader, CContext *poContext,
				   GenericPinpad *poPinpad, bool &isContactLess) {
	CCard *poCard = NULL;
	long lErrCode = EIDMW_ERR_CHECK;
	const char *strReader = NULL;
	const void *paramStructure = NULL;

	if (poContext->m_ulConnectionDelay != 0)
		CThread::SleepMillisecs(poContext->m_ulConnectionDelay);

	strReader = csReader.c_str();

	if (hCard) {
		CByteArray atr = poContext->m_oPCSC.GetATR(hCard);
		CByteArray atrContactLessCard(PTEID_CONTACTLESS_ATR, sizeof(PTEID_CONTACTLESS_ATR));
		isContactLess = atr.Equals(atrContactLessCard);

		MWLOG(LEV_DEBUG, MOD_CAL, "Using Reader: %s is the card contactless: %s", csReader.c_str(),
			  isContactLess ? "true" : "false");
		MWLOG(LEV_DEBUG, MOD_CAL, "ATR input value: %s", atr.ToString(true, false).c_str());

		if (protocol == SCARD_PROTOCOL_T0)
			paramStructure = SCARD_PCI_T0;
		else if (protocol == SCARD_PROTOCOL_T1)
			paramStructure = SCARD_PCI_T1;

		const auto selectAppId = [&](const unsigned char *oAID, unsigned long size) -> bool {
			long lRetVal = 0;
			unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x0C};
			CByteArray oCmd(12);
			oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
			oCmd.Append((unsigned char)size);
			oCmd.Append(oAID, size);

			CByteArray oResp;
			oResp = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal, paramStructure);
			return (oResp.Size() == 2 && (oResp.GetByte(0) == 0x61 || oResp.GetByte(0) == 0x90));
		};

		int appletVersion = 1;
		if (!isContactLess) {
			bool aidStatus = selectAppId(PTEID_1_APPLET_AID, sizeof(PTEID_1_APPLET_AID));
			if (!aidStatus) {
				bool nationalDataStatus =
					selectAppId(PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA));
				if (nationalDataStatus)
					appletVersion = 3;
			}

			long cacheEnabled = CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHE_ENABLED);

			poCard = PteidCardGetInstance(appletVersion, strReader, hCard, poContext, poPinpad, paramStructure);
			if (cacheEnabled)
				poCard->InitEncryptionKey();
		} else {
			appletVersion = 3;
			poCard = new CPteidCard(hCard, poContext, poPinpad, paramStructure);
		}

		CCache::LimitDiskCacheFiles(10);

		// If no other CCard subclass could be found
		if (poCard == NULL) {
			poCard = new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
		}

		poCard->setProtocol(paramStructure);
	} else {
		throw CMWEXCEPTION(lErrCode);
	}

	return poCard;
}

CCard *CardConnect(const std::string &csReader, CContext *poContext, GenericPinpad *poPinpad, bool &isContactLess) {
	CCard *poCard = NULL;
	long lErrCode = EIDMW_ERR_CHECK; // should never be returned
	const char *strReader = NULL;
	const void *param_structure = NULL;

	if (poContext->m_ulConnectionDelay != 0)
		CThread::SleepMillisecs(poContext->m_ulConnectionDelay);

	// Try if we can connect to the card via a normal SCardConnect()
	SCARDHANDLE hCard = 0;
	std::pair<SCARDHANDLE, DWORD> ret;
	try {
		ret = poContext->m_oPCSC.Connect(csReader);
		hCard = ret.first;

		if (hCard == 0) {
			goto done;
		}
	} catch (CMWException &e) {
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
			CByteArray atr = poContext->m_oPCSC.GetATR(hCard);
			CByteArray atrContactLessCard(PTEID_CONTACTLESS_ATR, sizeof(PTEID_CONTACTLESS_ATR));
			isContactLess = atr.Equals(atrContactLessCard);

			MWLOG(LEV_DEBUG, MOD_CAL, "Using Reader: %s is the card contactless: %s", csReader.c_str(),
				  isContactLess ? "true" : "false");
			MWLOG(LEV_DEBUG, MOD_CAL, "ATR input value: %s", atr.ToString(true, false).c_str());

			if (ret.second == SCARD_PROTOCOL_T0)
				param_structure = SCARD_PCI_T0;
			else if (ret.second == SCARD_PROTOCOL_T1)
				param_structure = SCARD_PCI_T1;

			int appletVersion = 0;

			const auto selectAppId = [&](const unsigned char *oAID, unsigned long size) -> bool {
				long lRetVal = 0;
				unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x0C};
				CByteArray select_command(12);
				select_command.Append(tucSelectApp, sizeof(tucSelectApp));
				select_command.Append((unsigned char)size);
				select_command.Append(oAID, size);

				CByteArray oResp;
				oResp = poContext->m_oPCSC.Transmit(hCard, select_command, &lRetVal, param_structure);
				unsigned long ulRespLen = oResp.Size();
				if (ulRespLen < 2)
					return false;
				unsigned long select_sw12 = 256 * oResp.GetByte(ulRespLen - 2) + oResp.GetByte(ulRespLen - 1);
				//We only need to test for application presence
				return (select_sw12 == 0x9000 || select_sw12 == 0x6982);
			};

			if (!isContactLess) {
				bool aidStatus = selectAppId(PTEID_1_APPLET_AID, sizeof(PTEID_1_APPLET_AID));
				if (aidStatus) {
					appletVersion = 1;
				} else {
					bool nationalDataStatus =
						selectAppId(PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA));
					if (nationalDataStatus)
						appletVersion = 3;
				}
				if (appletVersion > 0) {
					long cacheEnabled = CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHE_ENABLED);

					poCard =
						PteidCardGetInstance(appletVersion, strReader, hCard, poContext, poPinpad, param_structure);
					if (cacheEnabled)
						poCard->InitEncryptionKey();
				}
			} else {
				appletVersion = 3;
				poCard = new CPteidCard(hCard, poContext, poPinpad, param_structure);
			}

			CCache::LimitDiskCacheFiles(10);

			if (poCard == nullptr) {
				bool icaoStatus = selectAppId(ICAO_APPLET_MRTD, sizeof(ICAO_APPLET_MRTD));
				if (icaoStatus) {
					poCard = new CIcaoCard(hCard, poContext, poPinpad, param_structure);
				}
			}

			// If no other CCard subclass could be found
			if (poCard == NULL) {
				poCard = new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
			}

			poCard->setProtocol(param_structure);

			hCard = 0;
		}
	} else {
		isContactLess = false;
		throw CMWEXCEPTION(lErrCode);
	}

done:
	return poCard;
}

} // namespace eIDMW
