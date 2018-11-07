/* ***************************************************************************
 *
 *  PTeID Middleware Project.
 *  Copyright (C) 2016 - 2017
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 *
 *  Secure Messaging implementation for cards with IAS applet V3
 *  It mostly follows the open specification CWA 14890-1 specifically its chapter 9 
 *  It uses Diffie-Hellman key exchange, Retail MAC and 3DES-CBC for encryption
 **/


#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/rand.h>
#include <openssl/des.h>
#include <openssl/bn.h>

#include "APLCard.h"
#include "SecurityContext.h"
#include "Log.h"
#include "eidErrors.h"

//#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>

namespace eIDMW
{

	#define PRND2_SIZE 106
	#define SESSION_KEY_SIZE 16


	void SecurityContext::deriveSessionKeys()
	{
		unsigned char sha1_digest[20];
		unsigned char mac_key[SESSION_KEY_SIZE];
		unsigned char enc_key[SESSION_KEY_SIZE];

		unsigned char suffix_mac[] = {0x00, 0x00, 0x00, 0x02};
		unsigned char suffix_enc[] = {0x00, 0x00, 0x00, 0x01};

		if (m_kicc_ifd.Size() > 0)
		{

			EVP_MD_CTX *cmd_ctx;
			unsigned int md_len = 0;
			cmd_ctx = EVP_MD_CTX_create();

			EVP_DigestInit(cmd_ctx, EVP_sha1());
			EVP_DigestUpdate(cmd_ctx, m_kicc_ifd.GetBytes(), m_kicc_ifd.Size());
			EVP_DigestUpdate(cmd_ctx, suffix_mac, sizeof(suffix_mac));
			EVP_DigestFinal(cmd_ctx, sha1_digest, &md_len);

			//Copy only the 16 msbytes
			memcpy(mac_key, sha1_digest, SESSION_KEY_SIZE);

			memset(sha1_digest, 0, sizeof(sha1_digest));

			this->mac_key = CByteArray(mac_key, SESSION_KEY_SIZE);

			//Reuse the hashing context
			cmd_ctx = EVP_MD_CTX_create();

			EVP_DigestInit(cmd_ctx, EVP_sha1());
			EVP_DigestUpdate(cmd_ctx, m_kicc_ifd.GetBytes(), m_kicc_ifd.Size());
			EVP_DigestUpdate(cmd_ctx, suffix_enc, sizeof(suffix_enc));
			EVP_DigestFinal(cmd_ctx, sha1_digest, &md_len);

			//Copy only the 16 msbytes
			memcpy(enc_key, sha1_digest, SESSION_KEY_SIZE);

			this->encryption_key = CByteArray(enc_key, SESSION_KEY_SIZE);

		}
		else
		{
			fprintf(stderr, "deriveSessionKeys(): KICC/KIFD is not initialized! can't compute session keys.\n");
		}
	}

	void SecurityContext::computeInitialSSC()
	{
		uint64_t initial_ssc = 0;
		const int SSC_SIZE = 8;
		//4 LSB RND.ICC || 4 LSB RND.IFD
		CByteArray ssc_bytes(m_RNDICC.GetBytes(4, 4));
		ssc_bytes.Append(m_RNDIFD.GetBytes(4, 4));
		MWLOG(LEV_DEBUG, MOD_APL, "computeInitialSSC: ssc_bytes= 0x%s", ssc_bytes.ToString(false, true, 0).c_str());
		const unsigned char *ba = ssc_bytes.GetBytes();

		//Convert to 64-bit long from byte array in big-endian format
		for (size_t i = 0; i < sizeof(uint64_t); i++)
		{
			const uint8_t bit_shifts = (sizeof(uint64_t) - 1 - i) * 8;
			initial_ssc |= (uint64_t)ba[i] << bit_shifts;
		}

		this->m_ssc = initial_ssc;
	}
	
	/* 
	// Only used for debug purposes

	std::string byteArrayToString(CByteArray &ba)
	{
		std::string str_ba = ba.ToString(false, false, 0, 0xFFFFFFFF);

		str_ba.erase(std::remove(str_ba.begin(), str_ba.end(), '\t'), str_ba.end()); 
		str_ba.erase(std::remove(str_ba.begin(), str_ba.end(), '\n'), str_ba.end());

		return str_ba;
	}
	*/

#define MAC_KEYSIZE 8
	/*
	*	Input data must be a multiple of the block size: 8
	*/

	/* 'Retail MAC' according to ISO 9797-1 algorithm 3 without any padding
	 */
	CByteArray
	retail_mac_des(CByteArray &key, CByteArray &mac_input, uint64_t ssc)
	{
		CByteArray macInputWithSSC;
		CByteArray secondKey = key.GetBytes(MAC_KEYSIZE, MAC_KEYSIZE);

		unsigned char ssc_block[] = {0,0,0,0,0,0,0,0};
		unsigned char xx[MAC_KEYSIZE];
	    unsigned char des_out[MAC_KEYSIZE];
	    unsigned char *msg = NULL;
	    des_key_schedule ks_a;
	    des_key_schedule ks_b;
	    size_t i, j;

		CByteArray in;
		//std::cerr << "DEBUG: retail_mac_des: mac_input.Size(): " << mac_input.Size() << std::endl;
		//std::cerr << "mac_input: " << byteArrayToString(mac_input) << std::endl;
		//std::cerr << "Using MAC key: " << key.ToString(false, true, 0, 0xFFFFFFFF) << std::endl;

		//SSC is an 8-byte counter that serves as the first block of the MAC input
		for (i = 0; i < sizeof(ssc_block); i++)
	       ssc_block[i] = (0xFF & ssc >> (7 - i) * 8);

		  //unsigned char output[8];
	   	macInputWithSSC.Append(ssc_block, sizeof(ssc_block));
	   	macInputWithSSC.Append(mac_input);

	   	msg = macInputWithSSC.GetBytes();
	   
	    memset(des_out, 0, sizeof(des_out));

	   
	    DES_set_key_unchecked((const_DES_cblock*) key.GetBytes(), &ks_a);
	    DES_set_key_unchecked((const_DES_cblock*) secondKey.GetBytes(), &ks_b);

	  for (j=0; j < (macInputWithSSC.Size() / MAC_KEYSIZE); j++) {
	      //fprintf(stderr, "Running DES_ENCRYPT iteration: %d\n", j);
	    for (i=0; i < MAC_KEYSIZE; i++)
	      xx[i] = msg[i+j*MAC_KEYSIZE]^des_out[i];

	    DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);  
	  }

	  memcpy(xx, des_out, MAC_KEYSIZE);
	  DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_b, 0);

	  memcpy(xx, des_out, MAC_KEYSIZE);
	  DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);

	  CByteArray result(des_out, MAC_KEYSIZE);
	  //fprintf(stderr, "DEBUG: retail_mac_des(): %s", byteArrayToString());
	  //std::cerr << "retail_mac_des result: " << byteArrayToString(result) << std::endl;

	  return result;
	}
	
	CByteArray decrypt_data_3des(CByteArray &key, CByteArray &in)
	{
		CByteArray plain_text;
		unsigned char * out = (unsigned char * ) malloc(in.Size());
		EVP_CIPHER_CTX * ctx = NULL;
		unsigned int len = 0;

		//std::cerr << "Using encryption key: " << key.ToString(false, true, 0, 0xFFFFFFFF) << std::endl;

		if (key.Size() == 0)
	    {
	    	fprintf(stderr, "decrypt_data_3des(): Empty key!\n");
            goto err;
	    }

	    ctx = EVP_CIPHER_CTX_new();
	    if (!ctx)
	        goto err;

	    //Null as the 5th param means that IV is empty
	    // 0 as last param means decryption
	    if (!EVP_CipherInit_ex(ctx, EVP_des_ede_cbc(), NULL,
	            (unsigned char *) key.GetBytes(), NULL, 0) ||
	            	!EVP_CIPHER_CTX_set_padding(ctx, 0))
	        goto err;

	    if (EVP_DecryptUpdate(ctx, out, (int *)&len, in.GetBytes(), in.Size()) == 0) {
	        fprintf(stderr, "Error in decrypt_data_3des() !\n");
	    }

	    if (len != in.Size())
	    {
	    	fprintf(stderr, "decrypt_data_3des() Error: len < in.size()\n");
	    }

	    plain_text = CByteArray((const unsigned char*)out, (unsigned long)len);
		free(out);
	    return plain_text;


	err:
		free(out);
	    return CByteArray();
	}

	CByteArray encrypt_data_3des(CByteArray &key, CByteArray &in)
	{
		CByteArray cipher_text;
		unsigned char * out = (unsigned char * ) malloc(in.Size());
		EVP_CIPHER_CTX * ctx = NULL;
		unsigned int len = 0;

		//std::cerr << "Using encryption key: " << key.ToString(false, true, 0, 0xFFFFFFFF) << std::endl;
		//std::cerr << "encrypt_data_3des() input: " << in.ToString(false, true, 0, 0xFFFFFFFF) << std::endl;

		if (key.Size() == 0)
	    {
	    	fprintf(stderr, "encrypt_data_3des(): Empty key!\n");
            goto err;
	    }

	    ctx = EVP_CIPHER_CTX_new();
	    if (!ctx)
	        goto err;

	    //Null as the 5th param means that IV is empty
	    // 1 as last param means encryption
	    if (!EVP_CipherInit_ex(ctx, EVP_des_ede_cbc(), NULL,
	            (unsigned char *) key.GetBytes(), NULL, 1) ||
	            	!EVP_CIPHER_CTX_set_padding(ctx, 0))
	        goto err;

	    if (EVP_EncryptUpdate(ctx, out, (int *)&len, in.GetBytes(), in.Size()) == 0) {
	        fprintf(stderr, "Error in encrypt_data_3des() !\n");
	    }

	    if (len != in.Size())
	    {
	    	fprintf(stderr, "encrypt_data_3des() error: len < in.size()\n");
	    }

	    cipher_text = CByteArray((const unsigned char*)out, (unsigned long)len);
		free(out);
	    return cipher_text;


	err:
		free(out);
	    return CByteArray();
	}

	CByteArray paddedByteArray(CByteArray &input)
	{
		int padLen = 8 - input.Size() % 8;


		CByteArray paddedContent;
		paddedContent.Append(input);
		paddedContent.Append(0x80);

       	    //Padlen - 1 because one byte was already spent with 0x80 
		for (int i=0; i < padLen -1; i++)
			paddedContent.Append(0x00);

		return paddedContent;
	}


	bool checkSW12(CByteArray &result)
	{
		unsigned long ulRespLen = result.Size();
		unsigned int ulSW12 = (unsigned int)(256 * result.GetByte(ulRespLen - 2) + result.GetByte(ulRespLen - 1));

		return ulSW12 == 0x9000;
	}

	#define MAC_LEN 8

	/*
		Encrypt and add Message Authentication Code (MAC) to the input APDU
	*/
	CByteArray SecurityContext::buildSecureAPDU(CByteArray &plaintext_apdu)
	{
		CByteArray final_apdu;
		const unsigned char Tcg = 0x87;
		const unsigned char Tcc = 0x8e;
		const unsigned char Tle = 0x97;
		const unsigned char paddingIndicator = 0x01;
		/*
		Tcg = 0x87
		Tcc = 0x8E
		Example of Encrypted INTERNAL AUTH APDU
		8c 88 00 00 1d 87 11 010156bdbb34981e26497009df84a85811 [8e 08 15 a3 27 8f cc d5 24 6e]
		Structure of encrypted APDU: XC INS P1-P2 Lc Tcg Lcg PI,CG Tcc Lcc MAC
		
		PI = 01 (In IAS Classic Applet V3, this is always 01h to indicate that padding is as defined in ISO/IEC 7816-4,
		That is one byte of 80h followed by as many bytes of 00h as necessary to make the data a multiple of eight bytes)
		Command Header= XC INS P1-P2
		Input data for MAC: CH || PB || Tcg || Lcg || PI,CG || PB
		PB: padding 80h, 00h,...00h until the input data becomes a multiple of eight bytes
		*/

		//Contains data
		if (plaintext_apdu.Size() > 5)
		{

			CByteArray command_header(plaintext_apdu.GetBytes(0, 4));

			CByteArray input_data(plaintext_apdu.GetBytes(5, plaintext_apdu.Size()-5));
			CByteArray paddedInput = paddedByteArray(input_data);

			CByteArray cryptogram = encrypt_data_3des(this->encryption_key, paddedInput);
			//LCg = Len(Cg) + Len(PI = 1)
			int lcg = cryptogram.Size() +1;
			CByteArray inputForMac = paddedByteArray(command_header);

			CByteArray paddedCryptogram;
			paddedCryptogram.Append(Tcg);
			//This is safe because Lcg will be always lower than
			paddedCryptogram.Append((unsigned char)lcg);
			paddedCryptogram.Append(paddingIndicator);
			paddedCryptogram.Append(cryptogram);

			paddedCryptogram = paddedByteArray(paddedCryptogram);

			//Padded Command Header + Padded [Tcg Lcg PI,CG]
			inputForMac.Append(paddedCryptogram);

			//Pre-increment SSC
			m_ssc++;

			CByteArray mac = retail_mac_des(this->mac_key, inputForMac, m_ssc);

			//Build final APDU including Cryptogram and MAC
			int lc_final = MAC_LEN +2 + cryptogram.Size() + 3; // CG + Tcg + Lcg + PI
			final_apdu.Append(command_header);
			final_apdu.Append((unsigned char)lc_final);
			final_apdu.Append(Tcg);
			final_apdu.Append((unsigned char)lcg);
			final_apdu.Append(paddingIndicator);
			final_apdu.Append(cryptogram);
			final_apdu.Append(Tcc);
			final_apdu.Append(MAC_LEN); //Lcc
			final_apdu.Append(mac);

			return final_apdu;
		}
		else
		{

			// For commands sending no data the format should be
			// XC INS P1 P2 Lc Tle 01 Le Tcc Lcc MAC
			CByteArray TlvLe;
			CByteArray command_header(plaintext_apdu.GetBytes(0, 4));
			int le = plaintext_apdu.GetByte(4);
			CByteArray inputForMac = paddedByteArray(command_header);

			TlvLe.Append(Tle);
			TlvLe.Append(0x01);
			TlvLe.Append(le);

			inputForMac.Append(paddedByteArray(TlvLe));

			//Pre-increment SSC
			m_ssc++;

			CByteArray mac = retail_mac_des(this->mac_key, inputForMac, m_ssc);

			int lc_final = MAC_LEN + 2 + TlvLe.Size();
			final_apdu.Append(command_header);
			final_apdu.Append((unsigned char)lc_final);
			final_apdu.Append(TlvLe);
			final_apdu.Append(Tcc);
			final_apdu.Append(MAC_LEN); //Lcc
			final_apdu.Append(mac);

			return final_apdu;

		}
	}


	bool SecurityContext::checkMacInResponseAPDU(CByteArray &resp)
	{
		CByteArray receivedMac;
		CByteArray paddedInput;

		/*
			Deal with 2 different formats of response APDUs: with just SW12 or also with data 
		*/
		if (resp.GetByte(0) == 0x99)
		{
			receivedMac = (resp.GetBytes(6, 8));
			CByteArray inputForMac = resp.GetBytes(0, 4);
			paddedInput = paddedByteArray(inputForMac);

		}
		else if (resp.GetByte(0) == 0x87 || resp.GetByte(0) == 0x81)
		{
			resp.Chop(2);
			int mac_offset = resp.Size() - 8;
			int lcg = resp.GetByte(1) == 0x81 ? resp.GetByte(2): resp.GetByte(1);
			CByteArray inputForMac = resp.GetByte(1) == 0x81 ? resp.GetBytes(0, lcg+3) : resp.GetBytes(0, lcg+2);
			receivedMac = (resp.GetBytes(mac_offset, 8));
			paddedInput = paddedByteArray(inputForMac);
		}

		//Pre-increment SSC
		m_ssc++;

		CByteArray mac = retail_mac_des(this->mac_key, paddedInput, m_ssc);
		return mac.Equals(receivedMac);
	}

	#define SIG_LEN 128
	bool SecurityContext::validateInternalAuth(CByteArray &resp)
	{
		unsigned char computed_digest[20];

		unsigned char inputToSignature[SIG_LEN];
		memset(inputToSignature, 0, sizeof(inputToSignature));

		//We need to skip the Padding indicator '01' at offset 3
		CByteArray cryptogram(resp.GetBytes(4, resp.GetByte(2)-1));

		if (!checkMacInResponseAPDU(resp))
		{
			fprintf(stderr, "[validateInternalAuth] MAC mismatch in response APDU!\n");
		}

		CByteArray internalAuth_resp = decrypt_data_3des(this->encryption_key, cryptogram);

		//std::cerr << "internalAuth_resp " << byteArrayToString(internalAuth_resp) << std::endl;
		
		const unsigned char * iccAuthBytes = pkIccAuth.GetBytes();

		EVP_PKEY * pkey = d2i_PUBKEY(NULL, &iccAuthBytes, pkIccAuth.Size());
		if (!pkey)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"[validateInternalAuth] failed to decode card public key");
			return false;
		}

		RSA* rsa_key = EVP_PKEY_get1_RSA(pkey);

		//8-byte offset because Internal Auth Resp = SN.ICC + Signature
		CByteArray signature = internalAuth_resp.GetBytes(8, SIG_LEN);
		CByteArray snICC = internalAuth_resp.GetBytes(0, 8);
		
		int ret = RSA_public_decrypt(signature.Size(), signature.GetBytes(),
           inputToSignature, rsa_key, RSA_NO_PADDING);

		if (ret == -1)
		{
			fprintf(stderr, "Error validating Internal Auth signature: %s\n",
				ERR_error_string(ERR_get_error(), NULL));
			return false;
		}

        if (inputToSignature[0] != 0x6A || inputToSignature[127] != 0xBC)
        {
        	fprintf(stderr, "[validateInternalAuth] Start or End padding byte incorrect\n");
        	return false;
        }

        //Verify hash C = SHA-1 [PRND1 || KICC || SN.ICC || TRnd || KIFD || DH.Params]

        EVP_MD_CTX cmd_ctx;
		unsigned int md_len = 0;
		const int prnd1_len = 106;

		const unsigned char *prnd1 = inputToSignature + 1;
		const unsigned char *c_hash = inputToSignature + 1 + prnd1_len;
		

		CByteArray dh_params(this->dh_g);
		dh_params.Append(this->dh_p);
		dh_params.Append(this->dh_q);

		EVP_DigestInit(&cmd_ctx, EVP_sha1());
		EVP_DigestUpdate(&cmd_ctx, prnd1, prnd1_len);
		EVP_DigestUpdate(&cmd_ctx, m_kicc.GetBytes(), m_kicc.Size());
		EVP_DigestUpdate(&cmd_ctx, snICC.GetBytes(), snICC.Size());
		EVP_DigestUpdate(&cmd_ctx, m_RNDIFD.GetBytes(), m_RNDIFD.Size());
		EVP_DigestUpdate(&cmd_ctx, m_kifd.GetBytes(), m_kifd.Size());
		EVP_DigestUpdate(&cmd_ctx, dh_params.GetBytes(), dh_params.Size());


    	EVP_DigestFinal(&cmd_ctx, computed_digest, &md_len);

    	if (md_len != 20)
    	{
    		fprintf(stderr, "Should be SHA-1 hash, Abort!\n");
    		return false;
    	}

    	//Check the calculated hash against the one returned by the card
		return memcmp(computed_digest, c_hash, 20) == 0;
	}

	bool SecurityContext::internalAuthenticate()
	{
		//Example of encrypted MSE SET Internal Auth:
		unsigned char mse_internal_auth[] = {0x0C, 0x22, 0x41, 0xA4, 0x06, 0x84, 0x01, 0x77, 0x95, 0x01, 0x80};
		CByteArray mseInternal(mse_internal_auth, sizeof(mse_internal_auth));

		CByteArray secure_apdu = buildSecureAPDU(mseInternal);

		CByteArray resp = m_card->sendAPDU(secure_apdu);

		if (!checkSW12(resp))
		{
			MWLOG(LEV_ERROR, MOD_APL, L"MSE_internal_auth failed!");
			return false;
		}

		if (!checkMacInResponseAPDU(resp))
		{
			MWLOG(LEV_ERROR, MOD_APL, L"[internalAuthenticate MSE SET Internal] MAC mismatch in response APDU!\n");
			return false;
		}

		unsigned char tRnd[8];
		unsigned char internal_auth_apdu[] = {0x8C, 0x88, 0x00, 0x00, 0x08};
		CByteArray internalAuth(internal_auth_apdu, sizeof(internal_auth_apdu));

		if (RAND_bytes(tRnd, sizeof(tRnd)) == 0) {

            fprintf(stderr, "Error obtaining PRND2 bytes of random from OpenSSL\n");
            return false;
        }

        //Save this to reuse in computing the new SSC value after Internal Auth
        m_RNDIFD = CByteArray(tRnd, 8);

        internalAuth.Append(tRnd, 8);

        CByteArray secure_apdu2 = buildSecureAPDU(internalAuth);

        resp = m_card->sendAPDU(secure_apdu2);

		if (!checkSW12(resp))
		{
			MWLOG(LEV_ERROR, MOD_APL, L"internalAuthenticate() failed!");
			return false;
		}

		if (!validateInternalAuth(resp))
		{
			MWLOG(LEV_ERROR, MOD_APL, L"validateInternalAuth failed!");
			return false;
		}

		computeInitialSSC();
		MWLOG(LEV_DEBUG, MOD_APL, L"[internalAuthenticate] Finished INTERNAL AUTHENTICATE. Updated value of SSC to %llu\n", this->m_ssc);

        return true;
	}


	/*
		 This is used to build the External Signature Input Byte Array
		  in the following format: 0x6A || PRND2 || C || 0xBC
		  where:
		  C = SHA1(PRND2 || Kifd || SN.IFD || CRnd || Kicc || DH.Params)
		  DH.params = [G || P || Q]
		*/
	CByteArray SecurityContext::getExternalAuthenticateChallenge()
	{
		unsigned char sha1_digest[20];

		unsigned char prnd2[PRND2_SIZE];
		unsigned char challenge[128];

		CByteArray dh_params(this->dh_g);
		dh_params.Append(this->dh_p);
		dh_params.Append(this->dh_q);

		char *snIFD = sam_helper->getPK_IFD_AUT(m_ifd_cvc);
		char *cRnd = sam_helper->generateChallenge(snIFD);

		if (cRnd == NULL || strlen(cRnd) == 0)
		{
			fprintf(stderr, "Couldn't get CRnd random bytes from the card, aborting!\n");
			
			free(cRnd);
			free(snIFD);
			return CByteArray();
		}

		CByteArray crnd_bytes(std::string(cRnd), true);
		CByteArray sn_ifd_bytes(std::string(snIFD), true);
		m_snIFD = sn_ifd_bytes;
		m_RNDICC = crnd_bytes;

		challenge[0] = 0x6A;
		challenge[127] = 0xBC;

        if (RAND_status() != 1) {
			MWLOG(LEV_ERROR, MOD_APL, L"SecurityContext: RNG is not seeded yet!");
        }


        if (RAND_bytes(prnd2, PRND2_SIZE) == 0) {

            fprintf(stderr, "Error obtaining PRND2 bytes of random from OpenSSL\n");
            free(cRnd);
            free(snIFD);
            return CByteArray();
        }

		EVP_MD_CTX cmd_ctx;
		unsigned int md_len = 0;

		EVP_DigestInit(&cmd_ctx, EVP_sha1());
		EVP_DigestUpdate(&cmd_ctx, prnd2, PRND2_SIZE);
		EVP_DigestUpdate(&cmd_ctx, m_kifd.GetBytes(), m_kifd.Size());
		EVP_DigestUpdate(&cmd_ctx, sn_ifd_bytes.GetBytes(), sn_ifd_bytes.Size());
		EVP_DigestUpdate(&cmd_ctx, crnd_bytes.GetBytes(), crnd_bytes.Size());
		EVP_DigestUpdate(&cmd_ctx, m_kicc.GetBytes(), m_kicc.Size());
		EVP_DigestUpdate(&cmd_ctx, dh_params.GetBytes(), dh_params.Size());


    	EVP_DigestFinal(&cmd_ctx, sha1_digest, &md_len);

    	if (md_len != 20)
    	{
    		fprintf(stderr, "Should be SHA-1 hash, Abort!\n");
    	}

    	for (int i=0; i!=PRND2_SIZE; i++)
		{

			challenge[i+1] = prnd2[i];
		}

		for (int i=0; i!=20; i++)
			challenge[i+107] = sha1_digest[i];

		free(cRnd);
        free(snIFD);

		return CByteArray(challenge, sizeof(challenge));
	}

	static const char rnd_seed[] = "string to make the random number generator think it has entropy";

	void SecurityContext::initMuthualAuthProcess()
	{
		DHParams dh_params;

		//Before generating DH keys, init the RNG
		RAND_seed(rnd_seed, sizeof(rnd_seed));

		sam_helper->getDHParams(&dh_params);
		this->dh_p = CByteArray(std::string(dh_params.dh_p), true);
		this->dh_g = CByteArray(std::string(dh_params.dh_g), true);
		this->dh_q = CByteArray(std::string(dh_params.dh_q), true);

		DH * dh_key = DH_new();
		BN_hex2bn(&(dh_key->p), dh_params.dh_p);
		BN_hex2bn(&(dh_key->g), dh_params.dh_g);

		//Store the card auth key for later
		this->pkIccAuth = CByteArray(std::string(dh_params.card_auth_public_key), true);

		if (DH_generate_key(dh_key) != 1)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"DH_generate_key failed!");
			throw CMWEXCEPTION(EIDMW_ERR_CVC_GENERIC_ERROR);
		}

	 	char * kifd = BN_bn2hex(dh_key->pub_key);
	 	unsigned char * kifd_bytes = (unsigned char *) OPENSSL_malloc(BN_num_bytes(dh_key->pub_key));

	 	//int BN_bn2bin(const BIGNUM *a, unsigned char *to);
	 	//Store the byte array version for further computations
	 	BN_bn2bin(dh_key->pub_key, kifd_bytes);
	 	m_kifd = CByteArray(kifd_bytes, BN_num_bytes(dh_key->pub_key));

	 	if(!sam_helper->sendKIFD(kifd))
	 	{

	 		MWLOG(LEV_ERROR, MOD_APL, L"SendKIFD() failed, possible error in DH code!");
	 		throw CMWEXCEPTION(EIDMW_ERR_CVC_GENERIC_ERROR);
	 	}

		/* Receive the public key from the peer. In this example we're just hard coding a value */
		BIGNUM *kicc = NULL;

		char * kicc_str = sam_helper->getKICC();
		m_kicc = CByteArray(kicc_str, true);
		//int BN_hex2bn(BIGNUM **a, const char *str);
		int rc = BN_hex2bn(&kicc, kicc_str);
		free(kicc_str);

		MWLOG(LEV_DEBUG, MOD_APL, L"[initMuthualAuthProcess] BN_hex2bn(kicc) returned: %d", rc);
		
	    //DH_compute_key() computes the shared secret from the private DH value in dh and the other party's public value in pub_key and stores it in
        //key. key must point to DH_size(dh) bytes of memory.
        unsigned int shared_secret_len = DH_size(dh_key);
		unsigned char * kicc_ifd = (unsigned char *) OPENSSL_malloc(shared_secret_len);
		rc = DH_compute_key(kicc_ifd, kicc, dh_key);

		MWLOG(LEV_DEBUG, MOD_APL, L"[initMuthualAuthProcess] DH_compute_key returned: %d", rc);

		m_kicc_ifd = CByteArray(kicc_ifd, shared_secret_len);

		//std::cerr << "KICC/KIFD " << byteArrayToString(m_kicc_ifd) << std::endl;

	}

	bool SecurityContext::writeFile(char *fileID, CByteArray file_content, unsigned int offset)
	{
		if (fileID == NULL || strlen(fileID) < 2)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"SecurityContext::writeFile: invalid fileID!");
			return false;
		}

		return true;
	}

	CByteArray SecurityContext::readBinary(unsigned long bytesToRead, unsigned int fileSize)
	{

		//The blocksize is 230 because of the overhead of secure messaging response APDUs
		const double blockSize = 230.0;
		unsigned long bytesRead = 0;

		unsigned char read_binary[] = {0x0C, 0xB0, 0x00, 0x00, 0xE6};
		double bytes_to_read = bytesToRead < fileSize ? bytesToRead : fileSize;

		CByteArray readBinaryAPDU(read_binary, sizeof(read_binary));
		CByteArray fileContents;

		int iterations = ceil(bytes_to_read / blockSize);

		for (int i=0; i != iterations; i++)
		{
			//Set the file offset in P1-P2
			readBinaryAPDU.SetByte(0xFF & bytesRead >> 8, 2);
			readBinaryAPDU.SetByte(0xFF & bytesRead,      3);

			//Last iteration: we will read less than a complete block
			if (i == iterations-1)
			{
				readBinaryAPDU.SetByte(bytes_to_read-bytesRead, 4);
			}

			CByteArray secure_apdu = buildSecureAPDU(readBinaryAPDU);

	        CByteArray resp = m_card->sendAPDU(secure_apdu);

			if (!checkSW12(resp))
			{
				unsigned long sw12 = 256 * resp.GetByte(resp.Size() - 2) + resp.GetByte(resp.Size() - 1);
				MWLOG(LEV_ERROR, MOD_APL, L"[SecurityContext::readBinary iteration %d] card returned error: %04x", i, sw12);
				if (sw12 == 0x6982)
					throw CMWEXCEPTION(EIDMW_ERR_CVC_PERMISSION_DENIED);
				else
					throw CMWEXCEPTION(EIDMW_ERR_CVC_GENERIC_ERROR);
			}
			if (!checkMacInResponseAPDU(resp))
			{
				MWLOG(LEV_ERROR, MOD_APL, L"SecurityContext::readBinary MAC mismatch in response APDU!");
				throw CMWEXCEPTION(EIDMW_ERR_CVC_GENERIC_ERROR);
			}

			//Data is not encrypted Tpv = 81
			if (resp.GetByte(0) == 0x81)
			{
				int lpv = resp.GetByte(1) == 0x81 ? resp.GetByte(2): resp.GetByte(1);
				CByteArray fileData = resp.GetByte(1) == 0x81 ? resp.GetBytes(3, lpv): resp.GetBytes(2, lpv);
				fileContents.Append(fileData);
			}
			else
			{
				CByteArray cryptogram(resp.GetBytes(4, resp.GetByte(2)-1));

				//Decrypt a block of the file data
				CByteArray fileData = decrypt_data_3des(this->encryption_key, cryptogram);
				fileContents.Append(fileData);
				
			}

			bytesRead += blockSize;
		}

		return fileContents;
	}

	void SecurityContext::selectFile(CByteArray &fileID, unsigned int *fileSize)
	{
		unsigned char select_file[] = {0x0C, 0xA4, 0x00, 0x00, 0x02};

		CByteArray selectFileAPDU(select_file, sizeof(select_file));
		selectFileAPDU.Append(fileID);

		CByteArray secure_apdu = buildSecureAPDU(selectFileAPDU);

        CByteArray resp = m_card->sendAPDU(secure_apdu);

		if (!checkSW12(resp))
		{
			MWLOG(LEV_ERROR, MOD_APL, L"SelectFile under Secure Context failed!");
			throw CMWEXCEPTION(EIDMW_ERR_CVC_PATH_NOT_FOUND);
		}

		if (!checkMacInResponseAPDU(resp))
		{
			fprintf(stderr, "[SelectFile] MAC mismatch in response APDU!\n");
			throw CMWEXCEPTION(EIDMW_ERR_CVC_GENERIC_ERROR);
		}

		CByteArray cryptogram(resp.GetBytes(3, resp.GetByte(1)-1));

		//Decrypt the SELECT File data (FCI template)
		CByteArray fciData = decrypt_data_3des(this->encryption_key, cryptogram);
		//std::cerr << "selectFile: FCI data: " << byteArrayToString(fciData) << std::endl;

		*fileSize = (fciData.GetByte(1) << 8) + fciData.GetByte(2);
	}


	bool SecurityContext::verifySignedChallenge(CByteArray signed_challenge)
	{
		CByteArray externalAuthInput(m_snIFD);
		externalAuthInput.Append(signed_challenge);

		this->deriveSessionKeys();

		//Initial value of SCC
		this->m_ssc = 1;

		bool resp = sam_helper->verifySignedChallenge(externalAuthInput);
		resp = internalAuthenticate();

		return resp;
	}

	CByteArray SecurityContext::readFile(unsigned char *file, int filelen, unsigned long bytesToRead)
	{
		unsigned int fileSize = 0;
		for (int i=0; i!= filelen / 2; i++)
		{
			CByteArray fileToSelect(file+i*2, 2);
			selectFile(fileToSelect, &fileSize);
		}

		MWLOG(LEV_DEBUG, MOD_APL, L"SecurityContext::readFile: parsed file length=%d", fileSize);
		return readBinary(bytesToRead, fileSize);
	}

	bool SecurityContext::verifyCVCCertificate(CByteArray ifd_cvc)
	{
		initMuthualAuthProcess();
		m_ifd_cvc = ifd_cvc;

		return sam_helper->verifyCert_CV_IFD(ifd_cvc);

	}

}
