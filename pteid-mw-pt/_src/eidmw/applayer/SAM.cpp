/* ****************************************************************************
 *
 *  PTeID Middleware Project.
 *  Copyright(C) 2014
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 *  Card interaction necessary for the Change Address Operation
 *  mainly Diffie-Hellman key agreement and mutual authentication with CVC certificates
 **/

#include "APLCard.h"
#include "APLCardPteid.h"
#include "Reader.h"
#include "SAM.h"
#include <string>
#include <cstring>
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/pkcs7.h>

namespace eIDMW
{
	
SAM::SAM(APL_Card *card)
{
	m_card = card;
}


void binToHex(const unsigned char *in, size_t in_len, char *out, size_t out_len)
{
        unsigned int        n;
        char                *pos;

        pos = out;
        for (n = 0; n < in_len; n++) {
                sprintf(pos, "%02x", in[n]);
                pos += 2;
        }
        *pos = '\0';
}

char * SAM::_getDH_Param(char specific_byte, unsigned long offset)
{
		char * hex_param = NULL;
		unsigned char apdu_dh[] = {0x00, 0xCB, 0x00, 0xFF, 0x0A, 0xA6, 0x03, 0x83, 0x01, 0x32, 0x7F, 0x49, 0x02,
		 0x00, 0x00};

		apdu_dh[13] = specific_byte;

		CByteArray dh_param_ba = m_card->getCalReader()->SendAPDU(CByteArray(apdu_dh, sizeof(apdu_dh)));

		//Parse the DH parameter out of the response APDU excluding also the SW12 bytes
		CByteArray dh_param_final = dh_param_ba.GetBytes(offset);
		dh_param_final.Chop(2);

		size_t hex_len = 2 * dh_param_final.Size()+ 1;
		hex_param = (char *)malloc(hex_len);
		//CByteArray::ToString(bool bAddSpace, bool bOneLine, unsigned long ulOffset, unsigned long ulLen)

		binToHex(dh_param_final.GetBytes(), dh_param_final.Size(), hex_param, hex_len);
		return hex_param;
}


//Return DER encoding of the pubkey
char * SAM::_getCVCPublicKey()
{
	const int offset_mod = 14;
	const int offset_exp = 13;
	unsigned char *rsa_der = NULL;
	char *cvc_key = NULL;
	RSA *key_tmp = RSA_new();
	EVP_PKEY *evp_key = EVP_PKEY_new();

	unsigned char apdu_cvc_pubkey_mod[] = {0x00, 0xCB, 0x00,
	0xFF, 0x0A, 0xB6, 0x03, 0x83, 0x01, 0x44, 0x7F, 0x49, 0x02, 0x81, 0x00};

	unsigned  char apdu_cvc_pubkey_exponent[] = {0x00, 0xCB, 0x00, 0xFF, 0x0A, 0xB6, 0x03,
	0x83, 0x01, 0x44, 0x7F, 0x49, 0x02, 0x82, 0x00};

	CByteArray cvc_modulus = m_card->getCalReader()->SendAPDU(
		CByteArray(apdu_cvc_pubkey_mod, sizeof(apdu_cvc_pubkey_mod)));

	CByteArray cvc_exponent = m_card->getCalReader()->SendAPDU
	  (CByteArray(apdu_cvc_pubkey_exponent, sizeof(apdu_cvc_pubkey_exponent)));

	//Remove SW12 bytes from the responses
	cvc_modulus.Chop(2);
	cvc_exponent.Chop(2);

	std::cerr << "DEBUG: _getCVCPublicKey(): modulus len=" << cvc_modulus.Size() << std::endl;
	std::cerr << "DEBUG: _getCVCPublicKey(): exp len=" << cvc_exponent.Size() << std::endl;

	unsigned char * mod_bytes = cvc_modulus.GetBytes();
	unsigned char * exp_bytes = cvc_exponent.GetBytes();

	key_tmp->n = BN_bin2bn(mod_bytes+offset_mod,
                                 128, key_tmp->n);
	key_tmp->e = BN_bin2bn(exp_bytes+offset_exp,
                                 3, key_tmp->e);

	EVP_PKEY_assign_RSA(evp_key, key_tmp);
	 
	int der_len = i2d_PUBKEY(evp_key, &rsa_der);
	size_t out_len = der_len*2 +1;
	cvc_key = (char *)malloc(out_len);

	binToHex(rsa_der, der_len, cvc_key, out_len);
	free(rsa_der);

	return cvc_key;
}

char * SAM::_getCardAuthPublicKey()
{
	EVP_PKEY *evp_key = EVP_PKEY_new();
	RSA *key_tmp = RSA_new();
	char *card_auth_pubkey = NULL;
	unsigned char *rsa_der = NULL;

	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);
	APLPublicKey * key = pcard->getID().getCardAuthKeyObj();

	unsigned char * mod_bytes = key->getModulus()->GetBytes();
	unsigned char * exp_bytes = key->getExponent()->GetBytes();

	key_tmp->n = BN_bin2bn(mod_bytes, key->getModulus()->Size(), key_tmp->n);
	key_tmp->e = BN_bin2bn(exp_bytes, key->getExponent()->Size(), key_tmp->e);
	EVP_PKEY_assign_RSA(evp_key, key_tmp);

	int der_len = i2d_PUBKEY(evp_key, &rsa_der);
	//int der_len = i2d_RSAPublicKey(key_tmp, &rsa_der);
	size_t out_len = der_len*2 +1;
	card_auth_pubkey = (char *)malloc(out_len);

	binToHex(rsa_der, der_len, card_auth_pubkey, out_len);

	free(rsa_der);
	return card_auth_pubkey;
}

char * SAM::_getSODCert()
{
	char *sod_cert = NULL;
	PKCS7 * p7 = NULL;
	CByteArray sod_data = m_card->getRawData(APL_RAWDATA_SOD);
	const unsigned char *temp = sod_data.GetBytes();
	unsigned char *out = NULL; 
	long len = sod_data.Size();

	//Ignoring the first 4 bytes allows us to parse the SOD file as a PKCS7 object
	temp += 4;

	p7 = d2i_PKCS7(NULL, (const unsigned char **)&temp, len);

	STACK_OF(X509) *pSigners = PKCS7_get0_signers(p7, NULL, 0);

	X509 *signer_cert = sk_X509_value(pSigners, 0);
	int der_len = i2d_X509(signer_cert, &out);

	size_t hex_len = der_len*2 +1;
	sod_cert = (char *)malloc(hex_len);

	binToHex(out, der_len, sod_cert, hex_len);

	return sod_cert;
}

char *SAM::getSerialNumberIAS101()
{
	const unsigned char apdu_serial[] = {0x00, 0xCA, 0x02, 0x5A, 0x0D};

	CByteArray serial_apdu = CByteArray(apdu_serial,
		sizeof(apdu_serial));

	CByteArray serial_ba = m_card->getCalReader()->SendAPDU(serial_apdu);

	serial_ba.Chop(2);

	char *serial = (char *)malloc(serial_ba.Size()*2+1);

	binToHex(serial_ba.GetBytes(), serial_ba.Size(), serial, serial_ba.Size()*2+1);

	return serial;
}

bool checkResultSW12(CByteArray &result)
{
	unsigned long ulRespLen = result.Size();
	unsigned int ulSW12 = (unsigned int)(256 * result.GetByte(ulRespLen - 2) + result.GetByte(ulRespLen - 1));

	return ulSW12 == 0x9000;
}


bool SAM::sendKIFD(char *kifd)
{
	const unsigned char verify_apdu_key_agreement[] = {0x00, 0x22, 0x41, 0xA6, 0x89, 0x83, 0x01, 0x32, 0x95, 0x01, 0x80, 0x91, 0x81, 0x80}; 
	CByteArray sendKIFD_apdu = CByteArray(verify_apdu_key_agreement,
			sizeof(verify_apdu_key_agreement));

	//Create ByteArray from hex-encoded string
	CByteArray kifd_ba(std::string(kifd), true);
	sendKIFD_apdu.Append(kifd_ba);

	CByteArray resp = m_card->getCalReader()->SendAPDU(sendKIFD_apdu);

	if (!checkResultSW12(resp))
	{
		fprintf(stderr, "SendKIFD() failed!\n");
		return false;
	}

	return true;	
}

char *SAM::getKICC()
{
	char *kicc_hex = NULL;
	const int KICC_LEN = 128;
	const int KICC_HEX_LEN = 128*2 +1;
	const int KICC_OFFSET = 6;
	unsigned char apdu_kicc[] = {0x00, 0xCB, 0x00, 0xFF, 0x04, 0xA6, 0x02, 0x91, 0x00};

	CByteArray kicc_ba = m_card->getCalReader()->SendAPDU(CByteArray(apdu_kicc, sizeof(apdu_kicc)));
	if (!checkResultSW12(kicc_ba))
	{
		fprintf(stderr, "SendKIFD() failed!\n");
		return NULL;
	}
	kicc_ba.Chop(2);

	kicc_hex = (char *)malloc(KICC_HEX_LEN);

	binToHex(kicc_ba.GetBytes()+KICC_OFFSET, KICC_LEN, kicc_hex, KICC_HEX_LEN);

	return kicc_hex;
}


bool SAM::verifyCert_CV_IFD(char * cv_cert)
{
	unsigned char ba_apdu_pso_verify[] = {0x00, 0x2A, 0x00, 0xBE, 0xD1};
	unsigned char apdu_se_verify_cert[] = {0x00, 0x22, 0x41, 0xB6, 0x06, 0x83, 0x01, 0x44, 0x95, 0x01, 0x80};
	CByteArray apdu_pso_verify(ba_apdu_pso_verify, sizeof(ba_apdu_pso_verify));

	if (cv_cert == NULL || strlen(cv_cert) == 0)
	{
		fprintf(stderr, "Invalid cv_cert in SAM::VerifyCert_CV_IFD()!\n");
		return false;
	}

	CByteArray resp = m_card->getCalReader()->SendAPDU(CByteArray(apdu_se_verify_cert, sizeof(apdu_se_verify_cert)));

	if (!checkResultSW12(resp))
	{
		fprintf(stderr, "VerifyCert_CV_IFD() p1 failed!\n");
		return false;
	}

	CByteArray cvcert_ba(std::string(cv_cert), true);
	apdu_pso_verify.Append(cvcert_ba);

	resp = m_card->getCalReader()->SendAPDU(apdu_pso_verify);
	if (!checkResultSW12(resp))
	{
		fprintf(stderr, "VerifyCert_CV_IFD() p2 failed!\n");
		return false;
	}
	
	return true;
}


/*
The parameter external_auth indicates the Security environment that's going to be used in the MSE SET command
TODO: This should be parameterized to be reused in both cards scenarios
*/
char *SAM::generateChallenge()
{
	char *challenge = NULL;
	//MSE SET External auth or Mutual Auth

	// This includes the terminal SN associated with the server CVC cert
	//unsigned char ba1_test[] = {0x00, 0x22, 0x41 ,0xA4 ,0x0D ,0x95 ,0x01 ,0x80 ,0x83 ,0x08 ,0x04 ,0x06 ,0x02 ,0x02 ,0x00 ,0x04 ,0x03 ,0x07};
	unsigned char ba1_production[] = {0x00, 0x22, 0x41 ,0xA4 ,0x0D ,0x95 ,0x01, 0x80, 0x83, 0x08, 0x02, 0x06, 0x00, 0x07, 0x01, 0x07, 0x08, 0x08};

	//GET CHALLENGE
	unsigned char ba2[] = {0x80, 0x84, 0x00, 0x00, 0x08};

	m_card->getCalReader()->SendAPDU(CByteArray(ba1_production, sizeof(ba1_production)));

	CByteArray resp = m_card->getCalReader()->SendAPDU(CByteArray(ba2, sizeof(ba2)));

	resp.Chop(2);

	challenge = (char *)malloc(resp.Size()*2 +1);

	binToHex(resp.GetBytes(), 8, challenge, resp.Size()*2 +1);

	return challenge;

}

std::vector<char *> SAM::sendSequenceOfPrebuiltAPDUs(std::vector<char *> &apdu_array)
{
	int i = 0;
	std::vector <char *> responses;
	while(i != apdu_array.size())
	{
		char * tmp = sendPrebuiltAPDU(apdu_array.at(i));
		fprintf(stderr, "APDU %d -> result: %s\n", i, tmp);
		responses.push_back(tmp);
		i++;
	}
	return responses;
}

char *SAM::sendPrebuiltAPDU(char *apdu_string)
{
	char *resp_string = NULL;

	CByteArray mse_ba(std::string(apdu_string), true);
	CByteArray resp = m_card->getCalReader()->SendAPDU(mse_ba);

	resp_string = (char *)malloc(resp.Size()*2 +1);

	binToHex(resp.GetBytes(), resp.Size(), resp_string, resp.Size()*2 +1);

	return resp_string;
}

bool SAM::verifySignedChallenge(char *signed_challenge)
{
	unsigned char external_authenticate[] = {0x80, 0x82, 0x00, 0x00, 0x88};
	CByteArray ba1(external_authenticate, sizeof(external_authenticate));

	if (signed_challenge == NULL || strlen(signed_challenge) == 0)
	{
		fprintf(stderr, "Invalid signed_challenge in SAM::verifySignedChallenge()!\n");
		return false;
	}

	CByteArray signed_ba(std::string(signed_challenge), true);
	ba1.Append(signed_ba);

	CByteArray resp = m_card->getCalReader()->SendAPDU(ba1);

	return checkResultSW12(resp);
}

/* Fetch all the parameters needed from the card */
bool SAM::getDHParams(DHParams *dh_struct)
{

	dh_struct->dh_g = _getDH_Param(0x88, 12);
	dh_struct->dh_p = _getDH_Param(0x86, 12);
	dh_struct->dh_q = _getDH_Param(0x87, 10);

	dh_struct->cvc_ca_public_key = _getCVCPublicKey();
	dh_struct->card_auth_public_key = _getCardAuthPublicKey();
	dh_struct->certificateChain = _getSODCert();
	dh_struct->version = m_card->getType() == APL_CARDTYPE_PTEID_IAS07 ? 1 : 2;

	return true;
}


}




