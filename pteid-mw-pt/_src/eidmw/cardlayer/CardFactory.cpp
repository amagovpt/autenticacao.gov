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
#include "Config.h"

#include "PteidCard.h"
#include <vector>
#include <string>

#include "eac/eac.h"
#include "eac/pace.h"

namespace eIDMW
{

BUF_MEM *findObject(CByteArray &array, long tag) {
    const unsigned char *old_data = NULL;
    const unsigned char *desc_data = array.GetBytes();
    int xclass = 0;
    int ans1Tag = 0;
    int returnValue = 0;
    long size = 0;
    long genTag = 0;

    while(tag != genTag) {
        if(old_data == desc_data)
            return NULL;
        old_data = desc_data; // test this
        long returnValue = ASN1_get_object(&desc_data, &size, &ans1Tag, &xclass, array.Size());
        int constructed = returnValue == V_ASN1_CONSTRUCTED ? 1 : 0;
        genTag = xclass | (constructed & 0b1) << 5 | ans1Tag;
    }

    BUF_MEM * mem = BUF_MEM_new();
    mem->data = (char*)malloc(size * sizeof(char));
    memcpy(mem->data, desc_data, size);
    mem->length = size;
    mem->max = mem->length;
    return mem;
}

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
            CByteArray atr = poContext->m_oPCSC.GetATR(hCard);
            CByteArray atrContactLessCard(PTEID_CONTACTLESS_ATR, sizeof(PTEID_CONTACTLESS_ATR));
            bool isContactLess = atr.Equals(atrContactLessCard);

            MWLOG(LEV_DEBUG, MOD_CAL, "Using Reader: %s is the card contactless: %s", csReader.c_str(), isContactLess ? "true" : "false");
            MWLOG(LEV_DEBUG, MOD_CAL, "ATR input value: %s", atr.ToString(true, false).c_str());

			if (ret.second == SCARD_PROTOCOL_T0)
               param_structure = SCARD_PCI_T0;
        	else if (ret.second == SCARD_PROTOCOL_T1)
				param_structure = SCARD_PCI_T1;

			int appletVersion = 1;

            if(isContactLess) {
                EAC_CTX *eac_ctx = NULL;
                BUF_MEM *eax_mapping_data = NULL, *cardMappingData = NULL, *pubkey = NULL, *cardPubKey = NULL;
                BUF_MEM *token = NULL, *cardToken = NULL;
                BUF_MEM *comp_pub = NULL, *comp_pub_opp = NULL;

                EAC_init();
                eac_ctx = EAC_CTX_new();

                CByteArray oCmd(8);
                oCmd.Append(SELECT_ADF, sizeof(SELECT_ADF));
                const unsigned char SIZE_AND_EF_ACCESS[] = {0x02, 0x01, 0x01C, 0x00};
                oCmd.Append(SIZE_AND_EF_ACCESS, sizeof(SIZE_AND_EF_ACCESS));
                long fileReturn = 0;
                CByteArray oReadBinary = poContext->m_oPCSC.Transmit(hCard, oCmd, &fileReturn, param_structure);
                const unsigned char READ_BINARY[] = {0x00, 0xB0, 0x9C, 0x00, 0x00};
                CByteArray readBinary(READ_BINARY, sizeof(READ_BINARY));
                CByteArray readBinEFAccess = poContext->m_oPCSC.Transmit(hCard, readBinary, &fileReturn, param_structure);
                readBinEFAccess.Chop(2);
                if (!eac_ctx
                    || !EAC_CTX_init_ef_cardaccess(readBinEFAccess.GetBytes(), readBinEFAccess.Size(), eac_ctx)
                    || !eac_ctx->pace_ctx) {
                    MWLOG(LEV_ERROR, MOD_CAL, "Couldn't init ef cardaccess");
                }

                const char secret[] = "123456";
                PACE_SEC* pace_secret = PACE_SEC_new(secret, strlen(secret), PACE_CAN);

                const unsigned char setPace[] = {0x00 ,0x22 ,0xC1 ,0xA4 ,0x0F ,0x80 ,0x0A ,0x04 ,0x00 ,0x7F ,0x00 ,0x07 ,0x02 ,0x02 ,0x04 ,0x02 ,0x04 ,0x83 ,0x01 ,0x02};
                const unsigned char finalAuth[] = {0x00, 0x86, 0x00, 0x00, 0x0C, 0x7C, 0x0A, 0x85, 0x08};
                CByteArray setPaceAuth;
                CByteArray sendMapData;
                CByteArray responseMappingData;
                CByteArray sendEphePubKey;
                CByteArray responseEphePubKey;
                CByteArray verifyToken;
                CByteArray responseverifyToken;

                setPaceAuth.Append(setPace, sizeof(setPace));

                CByteArray cardNonceResponse = poContext->m_oPCSC.Transmit(hCard, setPaceAuth, &fileReturn, param_structure);

                CByteArray encryptedNonce;
                const unsigned char authEncrypt[] = {0x10, 0x86, 0x00, 0x00};
                const unsigned char authType[] = {0x02, 0x7C, 0x00, 0x00};
                encryptedNonce.Append(authEncrypt, sizeof(authEncrypt));
                encryptedNonce.Append(authType, sizeof(authType));

                CByteArray responseEncryptedNonce = poContext->m_oPCSC.Transmit(hCard, encryptedNonce, &fileReturn, param_structure);

                BUF_MEM *dec_nonce = findObject(responseEncryptedNonce, 0x80);

                if(!PACE_STEP2_dec_nonce(eac_ctx, pace_secret, dec_nonce)) {
                    MWLOG(LEV_ERROR, MOD_CAL, "Couldn't decrypt card nonce");
                    goto err;
                }

                eax_mapping_data = PACE_STEP3A_generate_mapping_data(eac_ctx);

                sendMapData.Append(authEncrypt, sizeof(authEncrypt));
                sendMapData.Append(eax_mapping_data->length + 4);
                sendMapData.Append(0x7C);
                sendMapData.Append(eax_mapping_data->length + 2);
                sendMapData.Append(0x81);
                sendMapData.Append(eax_mapping_data->length);
                sendMapData.Append(reinterpret_cast<unsigned char*>(eax_mapping_data->data), eax_mapping_data->length);
                sendMapData.Append(0x00);

                responseMappingData = poContext->m_oPCSC.Transmit(hCard, sendMapData, &fileReturn, param_structure);
                cardMappingData = findObject(responseMappingData, 0x82);
                if(!PACE_STEP3A_map_generator(eac_ctx, cardMappingData)) {
                    MWLOG(LEV_ERROR, MOD_CAL, "Couldn't generate map");
                    goto err;
                }

                pubkey = PACE_STEP3B_generate_ephemeral_key(eac_ctx);

                sendEphePubKey.Append(authEncrypt, sizeof(authEncrypt));
                sendEphePubKey.Append(pubkey->length + 4);
                sendEphePubKey.Append(0x7C);
                sendEphePubKey.Append(pubkey->length + 2);
                sendEphePubKey.Append(0x83);
                sendEphePubKey.Append(pubkey->length);
                sendEphePubKey.Append(reinterpret_cast<unsigned char*>(pubkey->data), pubkey->length);
                sendEphePubKey.Append(0x00);

                responseEphePubKey = poContext->m_oPCSC.Transmit(hCard, sendEphePubKey, &fileReturn, param_structure);

                cardPubKey = findObject(responseEphePubKey, 0x84);

                if(!PACE_STEP3B_compute_shared_secret(eac_ctx, cardPubKey) || !PACE_STEP3C_derive_keys(eac_ctx)) {
                    MWLOG(LEV_ERROR, MOD_CAL, "Couldn't compute shared secret or derive keys");
                }

                token = PACE_STEP3D_compute_authentication_token(eac_ctx, cardPubKey);

                verifyToken.Append(finalAuth, sizeof(finalAuth));
                verifyToken.Append(reinterpret_cast<unsigned char*>(token->data), token->length);
                verifyToken.Append(0x00);

                responseverifyToken = poContext->m_oPCSC.Transmit(hCard, verifyToken, &fileReturn, param_structure);

                cardToken = findObject(responseverifyToken, 0x86);

                if (!PACE_STEP3D_verify_authentication_token(eac_ctx, cardToken)) {
                    MWLOG(LEV_ERROR, MOD_CAL, "Couldn't verify authentication");
                }

                if (!EAC_CTX_set_encryption_ctx(eac_ctx, EAC_ID_PACE)) {
                    MWLOG(LEV_ERROR, MOD_CAL, "Couldn't initialize encryption");
                }

                comp_pub = EAC_Comp(eac_ctx, EAC_ID_PACE, pubkey);
                comp_pub_opp = EAC_Comp(eac_ctx, EAC_ID_PACE, cardPubKey);
err:
                if (eax_mapping_data) {
                    BUF_MEM_clear_free(eax_mapping_data);
                }
                if (pubkey) {
                    BUF_MEM_clear_free(pubkey);
                }
                if (dec_nonce) {
                    BUF_MEM_clear_free(dec_nonce);
                }
                if (cardMappingData) {
                    BUF_MEM_clear_free(cardMappingData);
                }
                if (cardPubKey) {
                    BUF_MEM_clear_free(cardPubKey);
                }
                if (token) {
                    BUF_MEM_clear_free(token);
                }
                if(cardToken) {
                    BUF_MEM_clear_free(cardToken);
                }
                if (comp_pub) {
                    BUF_MEM_clear_free(comp_pub);
                }
                if(comp_pub_opp) {
                    BUF_MEM_clear_free(comp_pub_opp);
                }
            }

			const auto selectAppId = [&](const unsigned char* oAID, unsigned long size) -> bool
			{
				long lRetVal = 0;
				unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x00};
				CByteArray oCmd(12);
				oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
				oCmd.Append((unsigned char)size);
				oCmd.Append(oAID, size);

				CByteArray oResp = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal, param_structure);
				return (oResp.Size() == 2 && (oResp.GetByte(0) == 0x61 || oResp.GetByte(0) == 0x90));
			};

			bool aidStatus = selectAppId(PTEID_1_APPLET_AID, sizeof(PTEID_1_APPLET_AID));
			if (!aidStatus) {
				bool nationalDataStatus = selectAppId(PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA));
				if (nationalDataStatus)
					appletVersion = 3;
			}

			poCard = PteidCardGetInstance(appletVersion, strReader, hCard, poContext, poPinpad, param_structure);

			CCache::LimitDiskCacheFiles(10);

			// If no other CCard subclass could be found
	    	if (poCard == NULL)
	    	{
				poCard = new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
			}

			poCard->setProtocol(param_structure);

			long cacheEnabled = CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHE_ENABLED);
			if (cacheEnabled)
				poCard->InitEncryptionKey();

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
