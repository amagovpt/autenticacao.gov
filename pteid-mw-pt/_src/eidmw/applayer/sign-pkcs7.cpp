#include <stdio.h>
#include <string.h>
//#include <unistd.h>

#include <fstream>
#include <iostream>
#include <iomanip>

#include "MWException.h"
#include "Log.h"
#include "ByteArray.h"
#include "APLCard.h"
#include "TsaClient.h"

#include "CardPteidDef.h"
#include "static_pteid_certs.h"
#include "cryptoFwkPteid.h"


/* Conditionally check some features on the OpenSSL API  */
#include <openssl/opensslv.h>

#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs7.h>
#include <openssl/asn1.h>
#include <openssl/sha.h>
#include <openssl/ts.h>

#define SHA1_LEN 20
#define SHA256_LEN 32

namespace eIDMW
{

char * BinaryToHexString(unsigned char * argbuf, unsigned int len)
{
	char * retbuf = (char *)malloc(len*2 +1);
	unsigned int i, j;

	for (i=j=0; i < len; i++) {
		char c;
		c = (argbuf[i] >> 4) & 0xf;
		c = (c>9) ? c+'a'-10 : c + '0';
		retbuf[j++] = c;
		c = argbuf[i] & 0xf;
		c = (c>9) ? c+'a'-10 : c + '0';
		retbuf[j++] = c;
	}

	//Terminate the string
	retbuf[j] = 0;
	return retbuf;
}


CByteArray PteidSign(APL_Card *card, CByteArray &to_sign, bool use_sha256)
{
	CByteArray output;

	unsigned char sha1OID[] = {
		0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 
		0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14
	};

	unsigned char sha256OID[] = {
	       	0x30, 0x31, 0x30, 0x0d, 0x06, 0x09,
		0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
		0x05, 0x00, 0x04, 0x20 };

	
	//Add PKCS1-v1_5 padding (according to http://tools.ietf.org/html/rfc3447#section-9.2)
	CByteArray to_sign_padded;
	if (use_sha256)	
		to_sign_padded.Append(sha256OID, sizeof(sha256OID));
	else
		to_sign_padded.Append(sha1OID, sizeof(sha1OID));

	to_sign_padded.Append(to_sign);

	output = card->Sign(to_sign_padded, true);

	return output;
}


/*
 * sig_struct: IN param
 * length: OUT param
 */
unsigned char *dump_signature(PKCS7 * sig_struct, int *length)
{

	STACK_OF(PKCS7_SIGNER_INFO) * signer_info = PKCS7_get_signer_info(sig_struct);
	unsigned char * buf = NULL;

	if (signer_info != NULL)
	{
		PKCS7_SIGNER_INFO * s = sk_PKCS7_SIGNER_INFO_value(signer_info, 0);
		ASN1_OCTET_STRING *signature = s->enc_digest; 
		int len = i2d_ASN1_OCTET_STRING(signature, NULL);
		*length = len;
		buf = (unsigned char *)malloc(len);
		unsigned char *p = buf;
		i2d_ASN1_OCTET_STRING(signature, &p);


	}
	
	return buf;
}


void add_cert_from_file(PKCS7 *p7, const char *path)
{
	BIO *in;
	X509* x509;
	if ((in=BIO_new_file(path,"r")) == NULL) goto err;
	if ((x509=PEM_read_bio_X509(in,NULL,NULL,NULL)) == NULL) goto err;

	PKCS7_add_certificate(p7,x509);

	BIO_free(in);
	return;
	err:
		fprintf(stderr, "Failed to add certificate %s\n", path);
}

// If our version of openssl is older than 1.0
// we need to provide this
#if OPENSSL_VERSION_NUMBER < 0x10000000L
int PKCS7_add1_attrib_digest (PKCS7_SIGNER_INFO *si,
		const unsigned char *md, int mdlen)
{ 
	ASN1_OCTET_STRING *os;
	os = ASN1_OCTET_STRING_new();
	if (!os)
		return 0;
	if (!ASN1_STRING_set(os, md, mdlen)
			|| !PKCS7_add_signed_attribute(si, NID_pkcs9_messageDigest,
				V_ASN1_OCTET_STRING, os))
	{   
		ASN1_OCTET_STRING_free(os);
		return 0;
	}   
	return 1;
}
#endif

void add_signed_time(PKCS7_SIGNER_INFO *si)
{
	ASN1_UTCTIME *sign_time;

	/* The last parameter is the amount to add/subtract from the current
	 * time (in seconds) */
	sign_time=X509_gmtime_adj(NULL,0);
	PKCS7_add_signed_attribute(si,NID_pkcs9_signingTime,
			V_ASN1_UTCTIME,(char *)sign_time);
}


unsigned int SHA256_Wrapper(unsigned char *data, unsigned long data_len, unsigned char *digest)
{

	EVP_MD_CTX cmd_ctx;
	unsigned int md_len = 0;

	//Calculate the hash from the data
	EVP_DigestInit(&cmd_ctx, EVP_sha256());
	EVP_DigestUpdate(&cmd_ctx, data, data_len);
        EVP_DigestFinal(&cmd_ctx, digest, &md_len);

	return md_len;

}

unsigned int SHA1_Wrapper(unsigned char *data, unsigned long data_len, unsigned char *digest)
{

	EVP_MD_CTX cmd_ctx;
	unsigned int md_len = 0;

	//Calculate the hash from the data
	EVP_DigestInit(&cmd_ctx, EVP_sha1());
	EVP_DigestUpdate(&cmd_ctx, data, data_len);
    EVP_DigestFinal(&cmd_ctx, digest, &md_len);

	return md_len;

}


void add_certificate(PKCS7 *p7, CByteArray &cert)
{
	unsigned char * cert_data = cert.GetBytes();
	X509 *x509 = d2i_X509(NULL, (const unsigned char**)&cert_data, cert.Size());
	if (x509 == NULL)
		goto err;
	
	PKCS7_add_certificate(p7, x509);

	return;
	err:
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add certificate from ByteArray\n");
}

void add_certificate(PKCS7 *p7, unsigned char * cert_data,
					 unsigned int data_size)
{

	X509 *x509 = d2i_X509(NULL, (const unsigned char**)&cert_data, data_size);
	if (x509 == NULL)
		goto err;
	
	PKCS7_add_certificate(p7, x509);

	return;

	err:
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add certificate.\n");
}


typedef unsigned int 
    (*HashFunc)(unsigned char *data, unsigned long data_len, unsigned char *digest);


/*
 * Appends the TSP TimestampToken to the existing PKCS7 signature as an unsigned attribute
 *
*/

int append_tsp_token(PKCS7_SIGNER_INFO *sinfo, unsigned char *token, int token_len)
{

	TS_RESP *tsp = d2i_TS_RESP(NULL, (const unsigned char**)&token, token_len);

	if (tsp != NULL)
	{
		PKCS7* token = tsp->token;

		int p7_len = i2d_PKCS7(token, NULL); 
		unsigned char *p7_der = (unsigned char *)OPENSSL_malloc(p7_len); 
		unsigned char *p = p7_der; 
		i2d_PKCS7(token, &p);

		if (!PKCS7_type_is_signed(token))
		{
			MWLOG(LEV_ERROR, MOD_APL, L"Error in timestamp token: not signed!\n");
			return 1;
		}

		//Add timestamp token to the PKCS7 signature object
		ASN1_STRING *value = ASN1_STRING_new();
		ASN1_STRING_set(value, p7_der, p7_len);

		int rc = PKCS7_add_attribute(sinfo, NID_id_smime_aa_timeStampToken,
				V_ASN1_SEQUENCE, value);

	}
	else
	{
		MWLOG(LEV_ERROR, MOD_APL, 
			L"Error decoding timestamp token! The TSA is returning bogus data or we have proxy issues and we are getting HTML messages here\n");
		return 1;
	}

	return 0;

}



/*
typedef struct ESS_issuer_serial
	{
	STACK_OF(GENERAL_NAME)	*issuer;
	ASN1_INTEGER		*serial;
	} ESS_ISSUER_SERIAL;
}

*/
void add_signingCertificate(PKCS7_SIGNER_INFO *si, X509 *x509, unsigned char * cert_data, unsigned long cert_len)
{
	ASN1_STRING *seq = NULL;
	unsigned char cert_sha256_sum[SHA256_LEN];
	unsigned char *p, *pp = NULL;
	int len;
	int signed_string_nid = -1;
	ESS_SIGNING_CERT *sc = NULL;
	ESS_CERT_ID *cid;

	GENERAL_NAME * name = NULL;

	SHA256_Wrapper(cert_data, cert_len, cert_sha256_sum);

	/* Create the SigningCertificateV2 attribute. */

	if (!(sc = ESS_SIGNING_CERT_new()))
		goto end;

	/* Adding the signing certificate id. */
	if (!(cid = ESS_CERT_ID_new())) 
		goto end;
	if (!ASN1_OCTET_STRING_set(cid->hash, cert_sha256_sum,
		sizeof(cert_sha256_sum)))
		goto end;

	//Add Issuer and Serial Number

	if (!(cid->issuer_serial = ESS_ISSUER_SERIAL_new()))
		goto end; 
                /* Creating general name from the certificate issuer. */
	if (!(name = GENERAL_NAME_new())) 
		goto end; 
		name->type = GEN_DIRNAME;
	if (!(name->d.dirn = X509_NAME_dup(x509->cert_info->issuer))) 
		goto end; 
	if (!sk_GENERAL_NAME_push(cid->issuer_serial->issuer, name))
		goto end; 

	cid->issuer_serial->serial = X509_get_serialNumber(x509);

	if (!sk_ESS_CERT_ID_push(sc->cert_ids, cid))
		goto end;

	/* Add SigningCertificateV2 signed attribute to the signer info. */

	len = i2d_ESS_SIGNING_CERT(sc, NULL);
	if (!(pp = (unsigned char *) OPENSSL_malloc(len))) goto end;

	p = pp;
	i2d_ESS_SIGNING_CERT(sc, &p);
	if (!(seq = ASN1_STRING_new()) || !ASN1_STRING_set(seq, pp, len))
		goto end;

	OPENSSL_free(pp); pp = NULL;

	signed_string_nid = OBJ_create("1.2.840.113549.1.9.16.2.47",
		"id-aa-signingCertificateV2",
		"id-aa-signingCertificateV2");

	PKCS7_add_signed_attribute(si, signed_string_nid, V_ASN1_SEQUENCE, seq);
	return;

	end:
	MWLOG(LEV_ERROR, MOD_APL, L"Failed to add SigningCertificateV2 attribute.\n");
}

/*
void WriteToFile(const char *path, const unsigned char *content, size_t len)
{
    	FILE *f=NULL;
    	fprintf(stderr, "Writing buffer %p with length=%lu\n", content, len);

        f = fopen(path, "wb");

        if (f)
        {
        	int fd = fileno(f);
            size_t retlen = write(fd, content, len);

            if (retlen != len)
            {
                fprintf(stderr, "WriteToFile failed! Errno: %d\n", errno);
            }
            fclose(f);
        }

 }
 */


/*
 * Returns as hex-encoded string the PKCS7 signature of the input data
 * The data is hashed with SHA-256 or SHA-1 and then signed on PTEID card
 *
 */

int pteid_sign_pkcs7 (APL_Card *card, unsigned char * data, unsigned long data_len,
	    bool timestamp, const char ** signature_contents)
{
	X509 *x509 = NULL;
	PKCS7 *p7 = NULL;
	PKCS7_SIGNER_INFO *signer_info;
	CByteArray hash, attr_hash, signature, certData, certData2, cc01, cc02, cc03, tsresp;
	char * signature_hex_string = NULL;
	unsigned char *attr_buf = NULL;
	unsigned char *timestamp_token = NULL;
	int return_code = 0;
	int tsp_token_len = 0;
	int auth_attr_len = 0;
	unsigned int len = 0;
	APL_CryptoFwkPteid *fwk = AppLayer.getCryptoFwk();
	

	/* Use stronger SHA-256 Hash with IAS 1.01 applet, SHA-1 otherwise */
	bool use_sha256 = card->getType() == APL_CARDTYPE_PTEID_IAS101;
	int hash_size = use_sha256 ?  SHA256_LEN : SHA1_LEN;
	
	//Function pointer to the correct hash function
	HashFunc my_hash = use_sha256 ? &SHA256_Wrapper : &SHA1_Wrapper;


	unsigned char *out = (unsigned char *)malloc(hash_size);
	unsigned char *attr_digest = (unsigned char *)malloc(hash_size);

	OpenSSL_add_all_algorithms();

	card->readFile(PTEID_FILE_CERT_SIGNATURE, certData);

	//Trim the padding zero bytes which are useless and affect the certificate digest computation
	certData.TrimRight(0);

	BIO *in = BIO_new_mem_buf(certData.GetBytes(), certData.Size());
	unsigned char * cert_data = certData.GetBytes();

	x509 = d2i_X509(NULL, (const unsigned char**)&cert_data, certData.Size());

	if (x509 == NULL)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"loadCert() Error decoding certificate data!");
		goto err;
	}

	BIO_reset(in);
	BIO_free(in);

	p7 = PKCS7_new();
	PKCS7_set_type(p7, NID_pkcs7_signed);
	 
	if (!PKCS7_content_new(p7, NID_pkcs7_data))
		goto err;

	signer_info = PKCS7_add_signature(p7, x509, X509_get_pubkey(x509), 
		use_sha256 ? EVP_sha256(): EVP_sha1());

	if (signer_info == NULL) goto err;

	/* we may want to add more */
	PKCS7_add_certificate(p7,x509);

	//Add Signature CA Cert
	card->readFile(PTEID_FILE_CERT_ROOT_SIGN, certData2);

	add_certificate(p7, certData2);
	
	//CartaodeCidadao Root CA certificates as of July 2015 (https://pki.cartaodecidadao.pt/publico/certificado/cc_ec_cidadao/)
	cc01 = CByteArray(PTEID_CERTS[22].cert_data, PTEID_CERTS[22].cert_len);
	cc02 = CByteArray(PTEID_CERTS[23].cert_data, PTEID_CERTS[23].cert_len);
	cc03 = CByteArray(PTEID_CERTS[24].cert_data, PTEID_CERTS[24].cert_len);

	// Add issuer of Signature SubCA
	if (fwk->isIssuer(certData2, cc01))
		add_certificate(p7, cc01);
	else if (fwk->isIssuer(certData2, cc02))
		add_certificate(p7, cc02);
	else if (fwk->isIssuer(certData2, cc03))
		add_certificate(p7, cc03);
	else 
		MWLOG(LEV_ERROR, MOD_APL, L"Couldn't find issuer for certificate SIGNATURE_SUBCA.The validation will be broken!");

	// Add ECRaizEstado certificate
	add_certificate(p7, PTEID_CERTS[21].cert_data, PTEID_CERTS[21].cert_len);

	PKCS7_set_detached(p7, 1);

	my_hash(data, data_len, out);
	
	/* Add the signing time and digest authenticated attributes */
	//With authenticated attributes
	
	PKCS7_add_signed_attribute(signer_info, NID_pkcs9_contentType, V_ASN1_OBJECT,
			OBJ_nid2obj(NID_pkcs7_data));
	   
	PKCS7_add1_attrib_digest(signer_info, out, hash_size);
	
	//Add signing-certificate v2 attribute according to the specification ETSI TS 103 172 v2.1.1 - section 6.3.1 
	add_signingCertificate(signer_info, x509, certData.GetBytes(), certData.Size());

	if (!timestamp)
		add_signed_time(signer_info);

	auth_attr_len = ASN1_item_i2d((ASN1_VALUE *)signer_info->auth_attr, &attr_buf,
				                                ASN1_ITEM_rptr(PKCS7_ATTR_SIGN));

	my_hash((unsigned char *)attr_buf, auth_attr_len, attr_digest);
	attr_hash = CByteArray((const unsigned char *)attr_digest, hash_size);

	signature = PteidSign(card, attr_hash, use_sha256);

	if (signature.Size() == 0)
		goto err;

	//Manually fill-in the signedData structure
	signer_info->enc_digest = ASN1_OCTET_STRING_new();
	ASN1_OCTET_STRING_set(signer_info->enc_digest, (unsigned char*)signature.GetBytes(),
			signature.Size());

	if (timestamp)
	{
		TSAClient tsp;
		unsigned char *digest_tp = (unsigned char *)malloc(SHA1_LEN);
		//Compute SHA-1 of the signed digest
		SHA1_Wrapper(signature.GetBytes(), signature.Size(), digest_tp);

		tsp.timestamp_data(digest_tp, SHA1_LEN);
		tsresp = tsp.getResponse();

		if (tsresp.Size() == 0)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"PKCS7 Sign: Timestamp Error - response is empty\n");
			return_code = 1;
		
		}
		else
		{
			timestamp_token = tsresp.GetBytes();
			tsp_token_len = tsresp.Size();
		}

		free(digest_tp);
	}
	
	if (timestamp_token && tsp_token_len > 0)
	{
		return_code = append_tsp_token(signer_info, timestamp_token, tsp_token_len);

	}

	unsigned char *buf2, *p;

	len = i2d_PKCS7(p7, NULL); 
	buf2 = (unsigned char *)OPENSSL_malloc(len); 
	p = buf2; 
	i2d_PKCS7(p7, &p);

	signature_hex_string = BinaryToHexString(buf2, len);

	OPENSSL_free(buf2);
	X509_free(x509);
	PKCS7_free(p7);
	free(out);
	free(attr_digest);

	*signature_contents = signature_hex_string;

	return return_code;

err:
	X509_free(x509);
	PKCS7_free(p7);
	free(attr_digest);
	free(out);

	ERR_load_crypto_strings();
	ERR_print_errors_fp(stderr);
	return 2;
}

}

