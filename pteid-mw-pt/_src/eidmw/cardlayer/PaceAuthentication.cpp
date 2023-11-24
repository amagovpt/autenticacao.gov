#include "PaceAuthentication.h"

#include "ByteArray.h"

#include "Log.h"

#include "eac/eac.h"
#include "eac/pace.h"

namespace eIDMW
{

    class PaceAuthenticationImpl {

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
    public:
        explicit PaceAuthenticationImpl(CContext *poContext, SCARDHANDLE* hCard, const void* param_structure) {
            BUF_MEM *mappingData = NULL, *cardMappingData = NULL, *pubkey = NULL, *cardPubKey = NULL;
            BUF_MEM *token = NULL, *cardToken = NULL;
            int r = 0;

            EAC_init();
            m_ctx = EAC_CTX_new();

            CByteArray oCmd(8);
            oCmd.Append(SELECT_ADF, sizeof(SELECT_ADF));
            const unsigned char SIZE_AND_EF_ACCESS[] = {0x02, 0x01, 0x01C, 0x00};
            oCmd.Append(SIZE_AND_EF_ACCESS, sizeof(SIZE_AND_EF_ACCESS));
            long fileReturn = 0;
            CByteArray oReadBinary = poContext->m_oPCSC.Transmit(*hCard, oCmd, &fileReturn, param_structure);
            const unsigned char READ_BINARY[] = {0x00, 0xB0, 0x9C, 0x00, 0x00};
            CByteArray readBinary(READ_BINARY, sizeof(READ_BINARY));
            CByteArray readBinEFAccess = poContext->m_oPCSC.Transmit(*hCard, readBinary, &fileReturn, param_structure);
            readBinEFAccess.Chop(2);
            if (!m_ctx
                || !EAC_CTX_init_ef_cardaccess(readBinEFAccess.GetBytes(), readBinEFAccess.Size(), m_ctx)
                || !m_ctx->pace_ctx) {
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

            CByteArray cardNonceResponse = poContext->m_oPCSC.Transmit(*hCard, setPaceAuth, &fileReturn, param_structure);

            CByteArray encryptedNonce;
            const unsigned char authEncrypt[] = {0x10, 0x86, 0x00, 0x00};
            const unsigned char authType[] = {0x02, 0x7C, 0x00, 0x00};
            const unsigned char enc[] = {0x60 ,0x46 ,0x32 ,0xFF ,0x00 ,0x00 ,0x04 ,0x80 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
            encryptedNonce.Append(authEncrypt, sizeof(authEncrypt));
            encryptedNonce.Append(authType, sizeof(authType));

            CByteArray responseEncryptedNonce = poContext->m_oPCSC.Transmit(*hCard, encryptedNonce, &fileReturn, param_structure);

            BUF_MEM *dec_nonce = findObject(responseEncryptedNonce, 0x80);

            if(!PACE_STEP2_dec_nonce(m_ctx, pace_secret, dec_nonce)) {
                MWLOG(LEV_ERROR, MOD_CAL, "Couldn't decrypt card nonce");
                r = -1;
                goto err;
            }

            mappingData = PACE_STEP3A_generate_mapping_data(m_ctx);

            sendMapData.Append(authEncrypt, sizeof(authEncrypt));
            sendMapData.Append(mappingData->length + 4);
            sendMapData.Append(0x7C);
            sendMapData.Append(mappingData->length + 2);
            sendMapData.Append(0x81);
            sendMapData.Append(mappingData->length);
            sendMapData.Append(reinterpret_cast<unsigned char*>(mappingData->data), mappingData->length);
            sendMapData.Append(0x00);

            responseMappingData = poContext->m_oPCSC.Transmit(*hCard, sendMapData, &fileReturn, param_structure);
            cardMappingData = findObject(responseMappingData, 0x82);
            if(!PACE_STEP3A_map_generator(m_ctx, cardMappingData)) {
                MWLOG(LEV_ERROR, MOD_CAL, "Couldn't generate map");
                r = -1;
                goto err;
            }

            pubkey = PACE_STEP3B_generate_ephemeral_key(m_ctx);

            sendEphePubKey.Append(authEncrypt, sizeof(authEncrypt));
            sendEphePubKey.Append(pubkey->length + 4);
            sendEphePubKey.Append(0x7C);
            sendEphePubKey.Append(pubkey->length + 2);
            sendEphePubKey.Append(0x83);
            sendEphePubKey.Append(pubkey->length);
            sendEphePubKey.Append(reinterpret_cast<unsigned char*>(pubkey->data), pubkey->length);
            sendEphePubKey.Append(0x00);

            responseEphePubKey = poContext->m_oPCSC.Transmit(*hCard, sendEphePubKey, &fileReturn, param_structure);

            cardPubKey = findObject(responseEphePubKey, 0x84);

            if(!PACE_STEP3B_compute_shared_secret(m_ctx, cardPubKey) || !PACE_STEP3C_derive_keys(m_ctx)) {
                MWLOG(LEV_ERROR, MOD_CAL, "Couldn't compute shared secret or derive keys");
                r = -1;
                goto err;
            }

            token = PACE_STEP3D_compute_authentication_token(m_ctx, cardPubKey);

            verifyToken.Append(finalAuth, sizeof(finalAuth));
            verifyToken.Append(reinterpret_cast<unsigned char*>(token->data), token->length);
            verifyToken.Append(0x00);

            responseverifyToken = poContext->m_oPCSC.Transmit(*hCard, verifyToken, &fileReturn, param_structure);
            if(responseverifyToken.Size() <=2) {
                MWLOG(LEV_ERROR, MOD_CAL, "Error verifying generated token, perhaps wrong CAN code.");
                r = -2;
                goto err;
            }


            cardToken = findObject(responseverifyToken, 0x86);

            if (!PACE_STEP3D_verify_authentication_token(m_ctx, cardToken)) {
                MWLOG(LEV_ERROR, MOD_CAL, "Couldn't verify authentication from card");
                r = -1;
                goto err;
            }

            if (!EAC_CTX_set_encryption_ctx(m_ctx, EAC_ID_PACE)) {
                MWLOG(LEV_ERROR, MOD_CAL, "Couldn't initialize encryption");
                r = -1;
            }

        err:
            if (mappingData) {
                BUF_MEM_clear_free(mappingData);
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
            if (r < 0) {
                EAC_CTX_clear_free(m_ctx);
                EAC_cleanup();
                if(r == -2) {
                    // throw error with some more info from responseverifytoken perhaps with error codes from index 0 and 1
                }
            }
        }

        BUF_MEM *bufMemFromByteArray(const CByteArray &array) {
            BUF_MEM *buf = BUF_MEM_new();
            if(buf == NULL) {
                //no memory
                return NULL;
            }

            buf->data = (char*)malloc(array.Size() * sizeof(char));
            if(buf->data == NULL) {
                // no memory
                return NULL;
            }
            memcpy(buf->data, (char*)array.GetBytes(), array.Size());
            buf->length = array.Size();
            buf->max = buf->length;

            return buf;
        }

        CByteArray arrayFromBufMem(BUF_MEM *mem) {
            return CByteArray ((unsigned char*)mem->data, mem->length);
        }

        CByteArray formatAPDU(const CByteArray &plainAPDU)
        {
            BIO *bio = NULL;
            std::string padde;
            CByteArray encryptedAPDU;
            CByteArray responseEncryptedAPDU;
            CByteArray inputForMac;
            CByteArray TlvLe;
            int le = 0;
            BUF_MEM *headerData = NULL;
            BUF_MEM *memInputData = NULL;
            BUF_MEM *paddedMemInputData = NULL;

            const unsigned char Tcg = 0x87;
            const unsigned char Tcc = 0x8e;
            const unsigned char Tle = 0x97;
            const unsigned char paddingIndicator = 0x01;
            const unsigned char controlByte = 0x0C;

            CByteArray command_header(plainAPDU.GetBytes(0, 4));
            command_header.SetByte(command_header.GetByte(0) | controlByte, 0);
            BUF_MEM *memCommandHeader = bufMemFromByteArray(command_header);
            BUF_MEM *memPaddedCommandHeader = EAC_add_iso_pad(m_ctx, memCommandHeader);
            CByteArray paddedCommandHeader = arrayFromBufMem(memPaddedCommandHeader);

            //Pre-increment SSC
            EAC_increment_ssc(m_ctx);

            if(plainAPDU.Size() > 5) {
                CByteArray inputData(plainAPDU.GetBytes(5, plainAPDU.Size()-5));
                memInputData = bufMemFromByteArray(inputData);
                paddedMemInputData = EAC_add_iso_pad(m_ctx, memInputData);

                BUF_MEM *memEncrytedInput = EAC_encrypt(m_ctx, paddedMemInputData);
                CByteArray encryptedInput = arrayFromBufMem(memEncrytedInput);
                CByteArray paddedInputArray = arrayFromBufMem(paddedMemInputData);
                //LCg = Len(Cg) + Len(PI = 1)
                int lcg = encryptedInput.Size() + 1;
                inputForMac.Append(paddedCommandHeader);

                CByteArray paddedCryptogram;
                paddedCryptogram.Append(Tcg);
                //This is safe because Lcg will be always lower than
                paddedCryptogram.Append((unsigned char)lcg);
                paddedCryptogram.Append(paddingIndicator);
                paddedCryptogram.Append(encryptedInput);

                BUF_MEM *memCryptogram = bufMemFromByteArray(paddedCryptogram);

                BUF_MEM *memPaddedCryptogram = EAC_add_iso_pad(m_ctx, memCryptogram);

                paddedCryptogram = arrayFromBufMem(memPaddedCryptogram);

                // [Tcg Lcg PI,CG]
                inputForMac.Append(paddedCryptogram);

                MWLOG(LEV_ERROR, MOD_CAL, "print debug this things to authenticate, paddedCommandHeader: %s paddedCryptogram: %s", paddedCommandHeader.ToString().c_str(), paddedCryptogram.ToString().c_str());

                MWLOG(LEV_ERROR, MOD_CAL, "print debug this paddedMemInputData: %s encryptedInput: %s, inputData: %s", paddedInputArray.ToString().c_str(), encryptedInput.ToString().c_str(), inputData.ToString().c_str());

                BUF_MEM *memInputForMac = bufMemFromByteArray(inputForMac);
                BUF_MEM *memMac = EAC_authenticate(m_ctx, memInputForMac);

                CByteArray mac = arrayFromBufMem(memMac);

                //Build final APDU including Cryptogram and MAC
                int lc_final = mac.Size() + 2 + encryptedInput.Size() + 3; // CG + Tcg + Lcg + PI
                encryptedAPDU.Append(command_header);
                encryptedAPDU.Append((unsigned char)lc_final);
                encryptedAPDU.Append(Tcg);
                encryptedAPDU.Append((unsigned char)lcg);
                encryptedAPDU.Append(paddingIndicator);
                encryptedAPDU.Append(encryptedInput);
                encryptedAPDU.Append(Tcc);
                encryptedAPDU.Append(mac.Size()); //Lcc
                encryptedAPDU.Append(mac);
                encryptedAPDU.Append(0x00);
            }
            else {

                CByteArray TlvLe;
                int le = plainAPDU.GetByte(4);
                inputForMac.Append(paddedCommandHeader);

                TlvLe.Append(0x97);
                TlvLe.Append(0x01);
                TlvLe.Append(le);
                BUF_MEM *memTlvLe = BUF_MEM_new();
                memTlvLe->data = (char*)TlvLe.GetBytes();
                memTlvLe->length = TlvLe.Size();

                BUF_MEM *paddedTlvLe = EAC_add_iso_pad(m_ctx, memTlvLe);

                inputForMac.Append((unsigned char*)paddedTlvLe->data, paddedTlvLe->length);
                BUF_MEM *inputMac = BUF_MEM_new();
                inputMac->data = (char*)inputForMac.GetBytes();
                inputMac->length = inputForMac.Size();

                //Pre-increment SSC
                EAC_increment_ssc(m_ctx);
                BUF_MEM *mac = EAC_authenticate(m_ctx, inputMac);


                int lc_final = mac->length + 2 + TlvLe.Size();
                encryptedAPDU.Append(command_header);
                encryptedAPDU.Append((unsigned char)lc_final);
                encryptedAPDU.Append(TlvLe);
                encryptedAPDU.Append(0x8e);
                encryptedAPDU.Append(mac->length); //Lcc
                encryptedAPDU.Append((unsigned char*)mac->data, mac->length);
            }

            return encryptedAPDU;
        }

        ~PaceAuthenticationImpl()
        {
            EAC_CTX_clear_free(m_ctx);
            EAC_cleanup();
        }

    private:
        friend class PaceAuthentication;
        EAC_CTX *m_ctx;
    };

    PaceAuthentication::PaceAuthentication(CContext *poContext, SCARDHANDLE* hCard, const void *param_structure)
        : m_impl(new PaceAuthenticationImpl(poContext, hCard, param_structure))
    {
    }

    PaceAuthentication::~PaceAuthentication()
    {
    }

    CByteArray PaceAuthentication::formatAPDU(const CByteArray &plainAPDU)
    {
        return m_impl->formatAPDU(plainAPDU);
    }
}
