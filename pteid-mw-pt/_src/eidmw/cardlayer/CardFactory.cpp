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
  Card type detection function for use in modules where we are supplied with a SCARDHANDLE already created, e.g.
  minidriver
*/

#ifdef __USE_PCSC__
CCard *CardConnect(SCARDHANDLE hCard, DWORD protocol, const std::string &csReader, CContext *poContext,
				   PinpadInterface *poPinpad, bool &isContactLess) {
	CCard *poCard = NULL;
	long lErrCode = EIDMW_ERR_CHECK;
	const char *strReader = NULL;

	PTEID_CardProtocol paramStructure = PTEID_CardProtocol::ANY;
	if (protocol == SCARD_PROTOCOL_T0)
		paramStructure = PTEID_CardProtocol::T0;
	else if (protocol == SCARD_PROTOCOL_T1)
		paramStructure = PTEID_CardProtocol::T1;

	if (poContext->m_ulConnectionDelay != 0)
		CThread::SleepMillisecs(poContext->m_ulConnectionDelay);

	strReader = csReader.c_str();

	PTEID_CardHandle handle = {0};
	if (hCard) {
		if (auto pcsc = dynamic_cast<CPCSC *>(poContext->m_oCardInterface.get())) {
			handle = pcsc->RegisterHandle(hCard);
		} else {
			// We do not support this type of card connect with card callbacks
			MWLOG_CTX(LEV_ERROR, MOD_CAL,
					  "CardConnect with card handle while card callbacks are setup is not allowed.");
			throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
		}

		if (auto pcsc = dynamic_cast<CPCSC *>(poContext->m_oCardInterface.get())) {
			pcsc->RegisterHandle(hCard);
		}

		CByteArray atr = poContext->m_oCardInterface->StatusWithATR(handle).second;
		CByteArray atrContactLessCard(PTEID_CONTACTLESS_ATR, sizeof(PTEID_CONTACTLESS_ATR));
		isContactLess = atr.Equals(atrContactLessCard);

		MWLOG(LEV_DEBUG, MOD_CAL, "Using Reader: %s is the card contactless: %s", csReader.c_str(),
			  isContactLess ? "true" : "false");
		MWLOG(LEV_DEBUG, MOD_CAL, "ATR input value: %s", atr.ToString(true, false).c_str());

		const auto selectAppId = [&](const unsigned char *oAID, unsigned long size) -> bool {
			long lRetVal = 0;
			unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x0C};
			CByteArray oCmd(12);
			oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
			oCmd.Append((unsigned char)size);
			oCmd.Append(oAID, size);

			CByteArray oResp;
			oResp = poContext->m_oCardInterface->Transmit(handle, oCmd, &lRetVal, paramStructure);
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

			poCard = PteidCardGetInstance(appletVersion, strReader, handle, poContext, poPinpad);
			if (cacheEnabled)
				poCard->InitEncryptionKey();
		} else {
			appletVersion = 3;
			poCard = new CPteidCard(handle, poContext, poPinpad);
		}

		CCache::LimitDiskCacheFiles(10);

		// If no other CCard subclass could be found
		if (poCard == NULL) {
			poCard = new CUnknownCard(handle, poContext, poPinpad, CByteArray());
		}

		poContext->setProtocol(handle, paramStructure);
	} else {
		throw CMWEXCEPTION(lErrCode);
	}

	return poCard;
}
#endif

CCard *CardConnect(const std::string &csReader, CContext *poContext, PinpadInterface *poPinpad, bool &isContactLess) {
	CCard *poCard = NULL;
	long lErrCode = EIDMW_ERR_CHECK; // should never be returned
	const char *strReader = NULL;
	PTEID_CardProtocol protocol = PTEID_CardProtocol::ANY;

	if (poContext->m_ulConnectionDelay != 0)
		CThread::SleepMillisecs(poContext->m_ulConnectionDelay);

	// Try if we can connect to the card via a normal SCardConnect()
	PTEID_CardHandle hCard = {PTEID_INVALID_HANDLE};
	std::pair<PTEID_CardHandle, PTEID_CardProtocol> ret;
	try {
		ret = poContext->m_oCardInterface->Connect(csReader);
		hCard = ret.first;
		protocol = ret.second;

		if (hCard == PTEID_INVALID_HANDLE) {
			goto done;
		}
	} catch (CMWException &e) {
		if (e.GetError() == (long)EIDMW_ERR_NO_CARD)
			goto done;
		if (e.GetError() != (long)EIDMW_ERR_CANT_CONNECT && e.GetError() != (long)EIDMW_ERR_CARD_COMM)
			throw;
		lErrCode = e.GetError();
		hCard = PTEID_INVALID_HANDLE;
	}

	strReader = csReader.c_str();

	if (hCard != PTEID_INVALID_HANDLE) {
		if (poCard == NULL) {
			CByteArray atr = poContext->m_oCardInterface->StatusWithATR(hCard).second;
			CByteArray atrContactLessCard(PTEID_CONTACTLESS_ATR, sizeof(PTEID_CONTACTLESS_ATR));
			isContactLess = atr.Equals(atrContactLessCard);

			MWLOG(LEV_DEBUG, MOD_CAL, "Using Reader: %s is the card contactless: %s", csReader.c_str(),
				  isContactLess ? "true" : "false");
			MWLOG(LEV_DEBUG, MOD_CAL, "ATR input value: %s", atr.ToString(true, false).c_str());

			int appletVersion = 0;

			const auto selectAppId = [&](const unsigned char *oAID, unsigned long size) -> bool {
				long lRetVal = 0;
				unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x0C};
				CByteArray select_command(12);
				select_command.Append(tucSelectApp, sizeof(tucSelectApp));
				select_command.Append((unsigned char)size);
				select_command.Append(oAID, size);

				CByteArray oResp;
				oResp = poContext->m_oCardInterface->Transmit(hCard, select_command, &lRetVal, protocol);
				unsigned long ulRespLen = oResp.Size();
				if (ulRespLen < 2)
					return false;
				unsigned long select_sw12 = 256 * oResp.GetByte(ulRespLen - 2) + oResp.GetByte(ulRespLen - 1);
				// We only need to test for application presence
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

					poCard = PteidCardGetInstance(appletVersion, strReader, hCard, poContext, poPinpad);
					if (cacheEnabled)
						poCard->InitEncryptionKey();
				}
			} else {
				appletVersion = 3;
				poCard = new CPteidCard(hCard, poContext, poPinpad);
			}

			CCache::LimitDiskCacheFiles(10);

			if (poCard == nullptr) {
				bool icaoStatus = selectAppId(ICAO_APPLET_MRTD, sizeof(ICAO_APPLET_MRTD));
				if (icaoStatus) {
					poCard = new CIcaoCard(hCard, poContext, poPinpad);

					// All ICAO cards are contactless
					isContactLess = true;
				}
			}

			// If no other CCard subclass could be found
			if (poCard == NULL) {
				poCard = new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
			}

			poContext->setProtocol(hCard, protocol);

			hCard = {0};
		}
	} else {
		isContactLess = false;
		throw CMWEXCEPTION(lErrCode);
	}

done:
	return poCard;
}

} // namespace eIDMW
