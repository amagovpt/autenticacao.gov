#include "PaceAuthentication.h"

#include "ByteArray.h"
#include "Log.h"
#include "eidErrors.h"

#include "eac/eac.h"
#include "eac/pace.h"

#include "Log.h"

namespace eIDMW
{

    class PaceAuthenticationImpl {

        const unsigned char Tcg = 0x87;
        const unsigned char TcgOdd = 0x85;
        const unsigned char Tcc = 0x8E;
        const unsigned char Tle = 0x97;
        const unsigned char paddingIndicator = 0x01;
        const unsigned char controlByte = 0x0C;

        BUF_MEM *findObject(const CByteArray &array, long tag) {
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
        explicit PaceAuthenticationImpl(CContext *poContext)
            : m_context(poContext)
        {

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
            CByteArray encryptedAPDU;
            CByteArray responseEncryptedAPDU;
            CByteArray inputForMac;
            CByteArray mac;
            CByteArray TlvLe;
            int lc = plainAPDU.GetByte(4);
            bool isInsOdd = plainAPDU.GetByte(1) & 1;

            CByteArray command_header(plainAPDU.GetBytes(0, 4));
            command_header.SetByte(command_header.GetByte(0) | controlByte, 0);
            BUF_MEM *memCommandHeader = bufMemFromByteArray(command_header);
            BUF_MEM *memPaddedCommandHeader = EAC_add_iso_pad(m_ctx, memCommandHeader);
            CByteArray paddedCommandHeader = arrayFromBufMem(memPaddedCommandHeader);
            inputForMac.Append(paddedCommandHeader);

            if((command_header.Size() + lc + 2 == plainAPDU.Size()) || plainAPDU.Size() == 5) {
                TlvLe.Append((unsigned char)Tle);
                TlvLe.Append((unsigned char)0x01);
                TlvLe.Append((unsigned char)plainAPDU.GetByte(plainAPDU.Size() - 1));
            }

            //Pre-increment SSC
            EAC_increment_ssc(m_ctx);

            if(plainAPDU.Size() > 5) {
                BUF_MEM *memInputData = NULL;
                BUF_MEM *paddedMemInputData = NULL;
                CByteArray inputData(plainAPDU.GetBytes(5, plainAPDU.Size() - 5 - (TlvLe.Size() > 0 ? 1 : 0)));
                memInputData = bufMemFromByteArray(inputData);
                paddedMemInputData = EAC_add_iso_pad(m_ctx, memInputData);

                BUF_MEM *memEncrytedInput = EAC_encrypt(m_ctx, paddedMemInputData);
                CByteArray encryptedInput = arrayFromBufMem(memEncrytedInput);
                CByteArray paddedInputArray = arrayFromBufMem(paddedMemInputData);
                //LCg = Len(Cg) + Len(PI = 1)
                int lcg = encryptedInput.Size() + (isInsOdd ? 0 : 1);

                CByteArray paddedCryptogram;
                paddedCryptogram.Append(isInsOdd ? TcgOdd : Tcg);
                //This is safe because Lcg will be always lower than
                paddedCryptogram.Append((unsigned char)lcg);
                if(!isInsOdd)
                    paddedCryptogram.Append(paddingIndicator);
                paddedCryptogram.Append(encryptedInput);
                if(TlvLe.Size() > 0)
                    paddedCryptogram.Append(TlvLe);

                BUF_MEM *memCryptogram = bufMemFromByteArray(paddedCryptogram);

                BUF_MEM *memPaddedCryptogram = EAC_add_iso_pad(m_ctx, memCryptogram);

                paddedCryptogram = arrayFromBufMem(memPaddedCryptogram);

                // [Tcg Lcg PI,CG]
                inputForMac.Append(paddedCryptogram);

                BUF_MEM *memInputForMac = bufMemFromByteArray(inputForMac);
                BUF_MEM *memMac = EAC_authenticate(m_ctx, memInputForMac);

                mac = arrayFromBufMem(memMac);

                int lc_final = mac.Size() + 2 + encryptedInput.Size() + 3 + TlvLe.Size() - (isInsOdd ? 1 : 0); // CG + Tcg + Lcg + PI
                encryptedAPDU.Append(command_header);
                encryptedAPDU.Append((unsigned char)lc_final);
                encryptedAPDU.Append(isInsOdd ? TcgOdd : Tcg);
                encryptedAPDU.Append((unsigned char)lcg);
                if(!isInsOdd)
                    encryptedAPDU.Append(paddingIndicator);
                encryptedAPDU.Append(encryptedInput);

                if(TlvLe.Size())
                    encryptedAPDU.Append(TlvLe);

                if(memMac)
                    BUF_MEM_clear_free(memMac);
                if(memInputForMac)
                    BUF_MEM_clear_free(memInputForMac);
                if(memPaddedCryptogram)
                    BUF_MEM_clear_free(memPaddedCryptogram);
                if(memCryptogram)
                    BUF_MEM_clear_free(memCryptogram);
                if(memEncrytedInput)
                    BUF_MEM_clear_free(memEncrytedInput);
                if(paddedMemInputData)
                    BUF_MEM_clear_free(paddedMemInputData);
                if(memInputData)
                    BUF_MEM_clear_free(memInputData);
            }
            else {
                BUF_MEM *memTlvLe = bufMemFromByteArray(TlvLe);

                BUF_MEM *paddedTlvLe = EAC_add_iso_pad(m_ctx, memTlvLe);

                inputForMac.Append((unsigned char*)paddedTlvLe->data, paddedTlvLe->length);
                BUF_MEM *inputMac = bufMemFromByteArray(inputForMac);

                BUF_MEM *memMac = EAC_authenticate(m_ctx, inputMac);
                mac = arrayFromBufMem(memMac);


                int lc_final = mac.Size() + 2 + TlvLe.Size();
                encryptedAPDU.Append(command_header);
                encryptedAPDU.Append((unsigned char)lc_final);
                encryptedAPDU.Append(TlvLe);

                if(memTlvLe)
                    BUF_MEM_clear_free(memTlvLe);
                if(paddedTlvLe)
                    BUF_MEM_clear_free(paddedTlvLe);
                if(inputMac)
                    BUF_MEM_clear_free(inputMac);
                if(memMac)
                    BUF_MEM_clear_free(memMac);
            }

            encryptedAPDU.Append(Tcc);
            encryptedAPDU.Append(mac.Size()); //Lcc
            encryptedAPDU.Append(mac);
            encryptedAPDU.Append(0x00);

            EAC_increment_ssc(m_ctx);

            return encryptedAPDU;
        }

        BUF_MEM *copyFromArray(const CByteArray &array, long index, long length) {
            BUF_MEM *copiedMem = BUF_MEM_new();
            copiedMem->data = (char*)malloc(length * sizeof(char));
            memcpy(copiedMem->data, array.GetBytes(index, length).GetBytes(), length);
            copiedMem->length = length;
            copiedMem->max = copiedMem->length;
            return copiedMem;
        }

        CByteArray decryptAPDU(const CByteArray &encryptedAPDU) {
            CByteArray decryptedResponse;
            CByteArray encryptedResponseToAuthenticate;
            BUF_MEM *encryptedResponse = NULL;
            BUF_MEM *encryptedResponseContent = NULL;
            BUF_MEM *macAPDUResponse = NULL;
            BUF_MEM *unpadDecryptedResponse = NULL;
            bool isOdd = encryptedAPDU.GetByte(0) == TcgOdd;

            if(encryptedAPDU.Size() <= 2) // error from security
                return encryptedAPDU;

            if(encryptedAPDU.GetByte(0) == Tcg || isOdd)
            {
                long hex = encryptedAPDU.GetByte(1);
                encryptedResponseContent = copyFromArray(encryptedAPDU, 2, hex);
            }
            int indexStatusCode = encryptedResponseContent ? encryptedResponseContent->length + 4 : 2;
            encryptedResponse = copyFromArray(encryptedAPDU, indexStatusCode, 2);

            if(encryptedResponseContent != NULL) {
                encryptedResponseToAuthenticate.Append(isOdd ? TcgOdd : Tcg);
                encryptedResponseToAuthenticate.Append(encryptedResponseContent->length);
                encryptedResponseToAuthenticate.Append(arrayFromBufMem(encryptedResponseContent));
            }

            if(encryptedResponse != NULL) {
                encryptedResponseToAuthenticate.Append(0x99);
                encryptedResponseToAuthenticate.Append(encryptedResponse->length);
                encryptedResponseToAuthenticate.Append(arrayFromBufMem(encryptedResponse));
            }

            BUF_MEM *memResponseToAuthenticate = bufMemFromByteArray(encryptedResponseToAuthenticate);

            BUF_MEM *paddedResponse = EAC_add_iso_pad(m_ctx, memResponseToAuthenticate);
            BUF_MEM *authenticateResponse = EAC_authenticate(m_ctx, paddedResponse);
            long macIndex = encryptedResponseToAuthenticate.Size() + 2;
            macAPDUResponse = copyFromArray(encryptedAPDU, macIndex, (long)encryptedAPDU.GetByte(macIndex - 1));

            if(!macAPDUResponse || !authenticateResponse || (authenticateResponse->length != macAPDUResponse->length)
                ||  memcmp(macAPDUResponse->data, authenticateResponse->data, macAPDUResponse->length) != 0)
            {
                MWLOG(LEV_ERROR, MOD_CAL, "Response from encrypted APDU is invalid! APDU: %s", encryptedAPDU.ToString().c_str());
                decryptedResponse.Append(encryptedAPDU);
                goto err;
            }
            if(encryptedResponseContent != NULL) {
                BUF_MEM *decryptResponseRemovePadding = BUF_MEM_new();
                decryptResponseRemovePadding->length = encryptedResponseContent->length - (isOdd ? 0 : 1); // first byte of an encrypted message is padding indicator unless odd INS
                decryptResponseRemovePadding->data = (char*)malloc(decryptResponseRemovePadding->length * sizeof(char));
                memcpy(decryptResponseRemovePadding->data, &encryptedResponseContent->data[isOdd ? 0 : 1], decryptResponseRemovePadding->length);
                decryptResponseRemovePadding->max = decryptResponseRemovePadding->length;
                BUF_MEM *paddedEncryptedResponse = NULL;
                if(decryptResponseRemovePadding->length % 16) {
                    paddedEncryptedResponse = EAC_add_iso_pad(m_ctx, decryptResponseRemovePadding);
                }
                else {
                    paddedEncryptedResponse = decryptResponseRemovePadding;
                    decryptResponseRemovePadding = NULL;
                }

                BUF_MEM *memDecryptedResponse = EAC_decrypt(m_ctx, paddedEncryptedResponse);
                unpadDecryptedResponse = EAC_remove_iso_pad(memDecryptedResponse);
                decryptedResponse.Append(arrayFromBufMem(unpadDecryptedResponse));

                if(unpadDecryptedResponse)
                    BUF_MEM_clear_free(unpadDecryptedResponse);
                if(decryptResponseRemovePadding)
                    BUF_MEM_clear_free(decryptResponseRemovePadding);
                if(paddedEncryptedResponse)
                    BUF_MEM_clear_free(paddedEncryptedResponse);
                if(memDecryptedResponse)
                    BUF_MEM_clear_free(memDecryptedResponse);
            }

            decryptedResponse.Append(arrayFromBufMem(encryptedResponse));
err:
            if(macAPDUResponse)
                BUF_MEM_clear_free(macAPDUResponse);
            if(encryptedResponseContent)
                BUF_MEM_clear_free(encryptedResponseContent);
            if(encryptedResponse)
                BUF_MEM_clear_free(encryptedResponse);
            if(memResponseToAuthenticate)
                BUF_MEM_clear_free(memResponseToAuthenticate);
            if(paddedResponse)
                BUF_MEM_clear_free(paddedResponse);
            if(authenticateResponse)
                BUF_MEM_clear_free(authenticateResponse);

            return decryptedResponse;
        }

        void initAuthentication(SCARDHANDLE &hCard, const void *param_structure) {
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
            CByteArray oReadBinary = m_context->m_oPCSC.Transmit(hCard, oCmd, &fileReturn, param_structure);
            const unsigned char READ_BINARY[] = {0x00, 0xB0, 0x9C, 0x00, 0x00};
            CByteArray readBinary(READ_BINARY, sizeof(READ_BINARY));
            CByteArray readBinEFAccess = m_context->m_oPCSC.Transmit(hCard, readBinary, &fileReturn, param_structure);
            readBinEFAccess.Chop(2);
            if (!m_ctx
                || !EAC_CTX_init_ef_cardaccess(readBinEFAccess.GetBytes(), readBinEFAccess.Size(), m_ctx)
                || !m_ctx->pace_ctx) {
                MWLOG(LEV_ERROR, MOD_CAL, "Couldn't init ef cardaccess");
            }

            char* secret = getenv("PACE_CAN");
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

            CByteArray cardNonceResponse = m_context->m_oPCSC.Transmit(hCard, setPaceAuth, &fileReturn, param_structure);

            CByteArray encryptedNonce;
            const unsigned char authEncrypt[] = {0x10, 0x86, 0x00, 0x00};
            const unsigned char authType[] = {0x02, 0x7C, 0x00, 0x00};
            const unsigned char enc[] = {0x60 ,0x46 ,0x32 ,0xFF ,0x00 ,0x00 ,0x04 ,0x80 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
            encryptedNonce.Append(authEncrypt, sizeof(authEncrypt));
            encryptedNonce.Append(authType, sizeof(authType));

            CByteArray responseEncryptedNonce = m_context->m_oPCSC.Transmit(hCard, encryptedNonce, &fileReturn, param_structure);

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

            responseMappingData = m_context->m_oPCSC.Transmit(hCard, sendMapData, &fileReturn, param_structure);
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

            responseEphePubKey = m_context->m_oPCSC.Transmit(hCard, sendEphePubKey, &fileReturn, param_structure);

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

            responseverifyToken = m_context->m_oPCSC.Transmit(hCard, verifyToken, &fileReturn, param_structure);
            if(responseverifyToken.Size() <=2) {
                MWLOG(LEV_ERROR, MOD_CAL, "Error verifying generated token, perhaps wrong CAN code, error response: %s", responseverifyToken.ToString().c_str());
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
                m_ctx = NULL;
                EAC_cleanup();
                if(r == -2) {
                    throw CMWEXCEPTION(EIDMW_PACE_ERR_BAD_TOKEN);
                }
                else if(r == -1) {
                    throw CMWEXCEPTION(EIDMW_PACE_ERR_UNKNOWN);
                }
            }
        }

        CByteArray sendAPDU(const CByteArray &plainAPDU, SCARDHANDLE &hCard, long &lRetVal, const void *param_structure) {
            CByteArray encryptedAPDU = formatAPDU(plainAPDU);
            CByteArray decriptedArray = decryptAPDU(m_context->m_oPCSC.Transmit(hCard, encryptedAPDU, &lRetVal, param_structure));
            return decriptedArray;
        }

        CByteArray formatAPDU(const APDU &apdu) {
            CByteArray encryptedAPDU;
            CByteArray command_header = apdu.getHeader();
            CByteArray inputForMac;
            CByteArray TlvLe;
            CByteArray mac;
            CByteArray encryptedInput;
            int lcg = 0;

            command_header.SetByte(apdu.cls() | controlByte, 0);
            BUF_MEM *memCommandHeader = bufMemFromByteArray(command_header);
            BUF_MEM *memPaddedCommandHeader = EAC_add_iso_pad(m_ctx, memCommandHeader);
            CByteArray paddedCommandHeader = arrayFromBufMem(memPaddedCommandHeader);
            inputForMac.Append(paddedCommandHeader);
            CByteArray le = apdu.getLe(true);

            bool isInsOdd = apdu.ins() & 1;

            EAC_increment_ssc(m_ctx);

            if(le.Size() >= 0) {
                {
                    TlvLe.Append((unsigned char)Tle);
                    TlvLe.Append((unsigned char)le.Size());
                    TlvLe.Append(le);
                }
            }
            CByteArray paddedCryptogram;
            int lc_final = 0;
            if(apdu.dataExists()) {
                BUF_MEM *memInputData = bufMemFromByteArray(apdu.data());
                BUF_MEM *paddedMemInputData = EAC_add_iso_pad(m_ctx, memInputData);
                BUF_MEM *memEncryptedInput = EAC_encrypt(m_ctx, paddedMemInputData);
                encryptedInput = arrayFromBufMem(memEncryptedInput);

                lcg = memEncryptedInput->length + (isInsOdd ? 0 : 1);
                paddedCryptogram.Append(isInsOdd ? TcgOdd : Tcg);
                paddedCryptogram.Append((unsigned char)lcg);
                if(!isInsOdd)
                    paddedCryptogram.Append(paddingIndicator);

                paddedCryptogram.Append(encryptedInput);
            }
            if(TlvLe.Size() > 0)
                paddedCryptogram.Append(TlvLe);

            BUF_MEM *memCryptogram = bufMemFromByteArray(paddedCryptogram);
            BUF_MEM *finalToEncrypt =  EAC_add_iso_pad(m_ctx, memCryptogram);
            inputForMac.Append(arrayFromBufMem(finalToEncrypt));

            BUF_MEM *inputMac = bufMemFromByteArray(inputForMac);

            BUF_MEM *memMac = EAC_authenticate(m_ctx, inputMac);
            mac = arrayFromBufMem(memMac);

            lc_final += mac.Size() + 2 ; // CG + Tcg + Lcg + PI
            if(lcg > 0) {
                lc_final += encryptedInput.Size() + 3 - (isInsOdd ? 1 : 0);
            }
            lc_final += TlvLe.Size();
            encryptedAPDU.Append(command_header);
            if(apdu.isExtended() && !apdu.dataExists() && apdu.getLe().Size() > 0) {
                encryptedAPDU.Append(0x00);
                encryptedAPDU.Append(0x00);
            }
            encryptedAPDU.Append(lc_final);
            if(lcg > 0) {
                encryptedAPDU.Append(isInsOdd ? TcgOdd : Tcg);
                encryptedAPDU.Append((unsigned char)lcg);
                if(!isInsOdd)
                    encryptedAPDU.Append(paddingIndicator);
                encryptedAPDU.Append(encryptedInput);
            }

            if(TlvLe.Size())
                encryptedAPDU.Append(TlvLe);

            encryptedAPDU.Append(Tcc);
            encryptedAPDU.Append(mac.Size()); //Lcc
            encryptedAPDU.Append(mac);
            encryptedAPDU.Append(0x00);
            if(apdu.isExtended())
                encryptedAPDU.Append(0x00);

            EAC_increment_ssc(m_ctx);

            return encryptedAPDU;

        }


        CByteArray sendAPDU(const APDU &apdu, SCARDHANDLE &hCard, long &lRetVal, const void *param_structure)
        {
            CByteArray encryptedAPDU = formatAPDU(apdu);
            CByteArray decriptedArray = decryptAPDU(m_context->m_oPCSC.Transmit(hCard, encryptedAPDU, &lRetVal, param_structure));
            return decriptedArray;
        }

        ~PaceAuthenticationImpl()
        {
            EAC_CTX_clear_free(m_ctx);
            EAC_cleanup();
        }

    private:
        friend class PaceAuthentication;
        CContext *m_context;
        EAC_CTX *m_ctx;
    };

    PaceAuthentication::PaceAuthentication(CContext *poContext)
        : m_impl(new PaceAuthenticationImpl(poContext))
    {
    }

    PaceAuthentication::~PaceAuthentication()
    {
    }

    void PaceAuthentication::initPaceAuthentication(SCARDHANDLE &hCard, const void *param_structure)
    {
        m_impl->initAuthentication(hCard, param_structure);
    }

    CByteArray PaceAuthentication::sendAPDU(const CByteArray &plainAPDU, SCARDHANDLE &hCard, long &lRetVal, const void *param_structure)
    {
        return m_impl->sendAPDU(plainAPDU, hCard, lRetVal, param_structure);
    }

    CByteArray PaceAuthentication::sendAPDU(const APDU &apdu, SCARDHANDLE &hCard, long &lRetVal, const void *param_structure)
    {
        return m_impl->sendAPDU(apdu, hCard, lRetVal, param_structure);
    }
}
