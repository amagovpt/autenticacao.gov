#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

#include "APLCard.h"
#include "SecurityContext.h"
#include "Log.h"


namespace eIDMW
{

	#define PRND2_SIZE 106
	#define SESSION_KEY_SIZE 16


	//TODO: Untested!
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

			//Reuse the hashing context
			EVP_MD_CTX_init(cmd_ctx);
			EVP_DigestUpdate(cmd_ctx, m_kicc_ifd.GetBytes(), m_kicc_ifd.Size());
			EVP_DigestUpdate(cmd_ctx, suffix_enc, sizeof(suffix_enc));
			EVP_DigestFinal(cmd_ctx, sha1_digest, &md_len);

			//Copy only the 16 msbytes
			memcpy(enc_key, sha1_digest, SESSION_KEY_SIZE);


		}
	}

	/*TODO

	void SecurityContext::computeInitialSSC(CByteArray &rnd_icc, CByteArray &rnd_ifd)
	{
		//4 LSB RND.ICC || 4 LSB RND.IFD
		CByteArray ssc_bytes(rnd_icc.GetBytes()+4, 4);
		ssc_bytes.Append(rnd_ifd.GetBytes()+4, 4);
		BN *ssc_bn = BN_bin2bn(ssc_bytes.GetBytes(), ssc_bytes.Size(), NULL);

		long ssc = BN_get_word();
		this->m_ssc = ssc;
	}

	*/

/*  Adapted from the OpenPACE implementation
*	
*	Input data must be a multiple of the block size: 8
*/

/* ISO 9797-1 algorithm 3 retail mac without any padding:
	The Input is assumed to be a multiple of 8-bytes
 */
CByteArray 
retail_mac_des(CByteArray &key, CByteArray &in)
{
	std::cout << "in.Size(): " << in.Size() << std::endl;

	unsigned char *complete_buf = (unsigned char *) malloc(in.Size());

	size_t len = EVP_CIPHER_block_size(EVP_des_cbc());

	unsigned char *outbuf = (unsigned char *) malloc(8 + len);

	unsigned char last_block[8];
	unsigned char last_block_final[8];

	memset(last_block, 0, sizeof(last_block));
	memset(last_block_final, 0, sizeof(last_block_final));
	int outlen = 0;
	int i = 0;

    EVP_CIPHER_CTX * ctx = NULL;

    if (key.Size() == 0)
    {
    	fprintf(stderr, "retail_mac_des(): Empty key!\n");
    	return CByteArray();
    }
    //DES BlockSize
    //check(key->length >= 2*len, "Key too short");

    ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        goto err;
    EVP_CIPHER_CTX_init(ctx);

    if (!EVP_CipherInit_ex(ctx, EVP_des_cbc(), NULL,
            (unsigned char *) key.GetBytes(), NULL, 1) ||
            !EVP_CIPHER_CTX_set_padding(ctx, 0))
        goto err;

    
    for ( ; i < in.Size() / len; i++)
    {
      if(!EVP_CipherUpdate(ctx, outbuf, &outlen, in.GetBytes()+i*len, len))
      {
            /* Error */
            return 0;
      }
      memcpy(complete_buf + i*len, outbuf, len);
    }

    EVP_CipherFinal_ex(ctx, outbuf, &outlen);
    //memcpy(complete_buf + i*len, outbuf, len);

    /* decrypt last block with the rest of the key (key.GetBytes() + len) */
    /* IV is always NULL */
    
    if (!EVP_CipherInit_ex(ctx, EVP_des_cbc(), NULL,
            (unsigned char *) key.GetBytes() + len, NULL, 0) ||
            !EVP_CIPHER_CTX_set_padding(ctx, 0))
        goto err;

    if(!EVP_CipherUpdate(ctx, last_block, &outlen, complete_buf + in.Size()-len , len))
    {
            /* Error */
            return 0;
    }

    EVP_CipherFinal_ex(ctx, last_block, &outlen);

    /* encrypt last block with the first key */
    /* IV is always NULL */
    
    if (!EVP_CipherInit_ex(ctx, EVP_des_cbc(), NULL,
            (unsigned char *) key.GetBytes(), NULL, 1) ||
            !EVP_CIPHER_CTX_set_padding(ctx, 0))
        goto err;

    if(!EVP_CipherUpdate(ctx, last_block_final, &outlen, last_block, len))
    {
            /* Error */
            return 0;
    }

    EVP_CipherFinal_ex(ctx, last_block_final, &outlen);

    EVP_CIPHER_CTX_free(ctx);

    return CByteArray(last_block_final, len);

err:
/*
    if (block)
        BUF_MEM_free(block);
    if (c_tmp)
        BUF_MEM_free(c_tmp);
    if (d_tmp)
        BUF_MEM_free(d_tmp);
        */
    if (ctx)
        EVP_CIPHER_CTX_free(ctx);

    return CByteArray();
}

/*

	bool SecurityContext::internalAuthenticate()
	{

	}
*/


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
			fprintf(stderr, "Couldn't generate CRnd random bytes, aborting!\n");
			return CByteArray();
		}

		CByteArray crnd_bytes(std::string(cRnd), true);
		CByteArray sn_ifd_bytes(std::string(snIFD), true);
		m_snIFD = sn_ifd_bytes;

		challenge[0] = 0x6A;
		challenge[127] = 0xBC;

        if (RAND_status() != 1) {
        	//TODO
        }
        

        if (RAND_bytes(prnd2, PRND2_SIZE) == 0) {

            fprintf(stderr, "Error obtaining PRND2 bytes of random from OpenSSL\n");
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

		return CByteArray(challenge, sizeof(challenge));
	}

	void SecurityContext::initMuthualAuthProcess()
	{
		DHParams dh_params;
		sam_helper->getDHParams(&dh_params);
		this->dh_p = CByteArray(std::string(dh_params.dh_p), true);
		this->dh_g = CByteArray(std::string(dh_params.dh_g), true);
		this->dh_q = CByteArray(std::string(dh_params.dh_q), true);

		DH * dh = DH_new();

		DH * dh_key = DH_new();
		BN_hex2bn(&(dh_key->p), dh_params.dh_p);
		BN_hex2bn(&(dh_key->g), dh_params.dh_g);

	/*
		For some reason DH_check fails for the DH parameters used in CC cards 
		int rc, codes = 0;
		rc = DH_check(dh_key, &codes);

		if (rc != 1)
		{
			printf("DH_check could not be performed! ABORT\n");
		}

		if(BN_is_word(dh_key->g, DH_GENERATOR_2))
		{
	    	long residue = BN_mod_word(dh_key->p, 24);
	    	if(residue == 11 || residue == 23) {
	        	codes &= ~DH_NOT_SUITABLE_GENERATOR;
	    		}
		}

	if (codes & DH_UNABLE_TO_CHECK_GENERATOR)
	    printf("DH_check: failed to test generator\n");

	if (codes & DH_NOT_SUITABLE_GENERATOR)
	    printf("DH_check: g is not a suitable generator\n");

	if (codes & DH_CHECK_P_NOT_PRIME)
	    printf("DH_check: p is not prime\n");

	if (codes & DH_CHECK_P_NOT_SAFE_PRIME)
	    printf("DH_check: p is not a safe prime\n");
	*/

	/* Generate the public and private key pair */
		if (DH_generate_key(dh_key) != 1)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"DH_generate_key failed!");
		}

	 	char * kifd = BN_bn2hex(dh_key->pub_key);
	 	unsigned char * kifd_bytes = (unsigned char *) OPENSSL_malloc(BN_num_bytes(dh_key->pub_key));

	 	//int BN_bn2bin(const BIGNUM *a, unsigned char *to);
	 	//Store the byte array version for further computations
	 	BN_bn2bin(dh_key->pub_key, kifd_bytes);
	 	m_kifd = CByteArray(kifd_bytes, BN_num_bytes(dh_key->pub_key));

	 	if(!sam_helper->sendKIFD(kifd))
	 	{

	 		fprintf(stderr, "SendKIFD() failed, possible error in DH code!\n");
	 		return;
	 	}


		/* Receive the public key from the peer. In this example we're just hard coding a value */
		BIGNUM *kicc = NULL;

		char * kicc_str = sam_helper->getKICC();
		m_kicc = CByteArray(kicc_str, true);
		//int BN_hex2bn(BIGNUM **a, const char *str); 
		int rc = BN_hex2bn(&kicc, kicc_str);

		if (rc == 0)
		{
			//TODO
		}

	    //DH_compute_key() computes the shared secret from the private DH value in dh and the other party's public value in pub_key and stores it in
        //key. key must point to DH_size(dh) bytes of memory.
        unsigned int shared_secret_len = DH_size(dh_key);
		unsigned char * kicc_ifd = (unsigned char *) OPENSSL_malloc(shared_secret_len);
		DH_compute_key(kicc_ifd, kicc, dh_key);

		m_kicc_ifd = CByteArray(kicc_ifd, shared_secret_len);

	}

	bool SecurityContext::writeFile(char *fileID, CByteArray file_content, unsigned int offset)
	{
		return true;		
	}


	bool SecurityContext::verifySignedChallenge(CByteArray signed_challenge)
	{
		CByteArray externalAuthInput(m_snIFD);
		externalAuthInput.Append(signed_challenge);

		this->deriveSessionKeys();

		return sam_helper->verifySignedChallenge(externalAuthInput);
	}

	bool SecurityContext::verifyCVCCertificate(CByteArray ifd_cvc)
	{
		initMuthualAuthProcess();
		m_ifd_cvc = ifd_cvc;

		return sam_helper->verifyCert_CV_IFD(ifd_cvc);
		
	}


}