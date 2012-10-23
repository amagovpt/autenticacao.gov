#include <stdio.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <iomanip>

#include "MWException.h"
#include "Log.h"
#include "ByteArray.h"
#include "APLCard.h"
#include "CardPteidDef.h"
#include "static_pteid_certs.h"


/* Conditionally check some features on the OpenSSL API  */
#include <openssl/opensslv.h>

#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs7.h>
#include <openssl/asn1.h>
#include <openssl/sha.h>

using namespace eIDMW;

#define SHA1_LEN 20
#define SHA256_LEN 32


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
 * Returns as hex-encoded string the PKCS7 signature of the input data
 * The data is hashed with SHA-256 or SHA-1 and then signed on PTEID card
 *
 */

char * pteid_sign_pkcs7 (APL_Card *card, unsigned char * data, unsigned long data_len)
{
	X509 *x509;
	PKCS7 *p7;
	PKCS7_SIGNER_INFO *signer_info;
	CByteArray hash, attr_hash, signature, certData, certData2;
	char * signature_hex_string = NULL;
	unsigned char *attr_buf = NULL;
	int auth_attr_len = 0;
	unsigned int len = 0;
	

	/* Use stronger SHA-256 Hash with IAS 1.01 applet, SHA-1 otherwise */
	bool use_sha256 = card->getType() == APL_CARDTYPE_PTEID_IAS101;
	int hash_size = use_sha256 ?  SHA256_LEN : SHA1_LEN;
	
	//Function pointer to the correct hash function
	HashFunc my_hash = use_sha256 ? &SHA256_Wrapper : &SHA1_Wrapper;


	unsigned char *out = (unsigned char *)malloc(hash_size);
	unsigned char *attr_digest = (unsigned char *)malloc(hash_size);

	OpenSSL_add_all_algorithms();

	card->readFile(PTEID_FILE_CERT_SIGNATURE, certData);

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
	

	// Add the top 3 certificates to complete the chain
	add_certificate(p7, PTEID_CERTS[0].cert_data, PTEID_CERTS[0].cert_len);
	add_certificate(p7, PTEID_CERTS[13].cert_data, PTEID_CERTS[13].cert_len);
	add_certificate(p7, PTEID_CERTS[14].cert_data, PTEID_CERTS[14].cert_len);

	PKCS7_set_detached(p7, 1);

	//SHA1 (data, data_len, out);
	my_hash(data, data_len, out);
	
	//ASN1_OCTET_STRING_set(p7->d.sign->contents->d.data, out, SHA1_LEN); 
	
	/* 
	//Without authenticated attributes
        hash = PTEID_ByteArray((const unsigned char*)out, 20L);
	signature = PteidSign(hash);

	*/


	/* Add the signing time and digest authenticated attributes */
	//With authenticated attributes
	
	PKCS7_add_signed_attribute(signer_info, NID_pkcs9_contentType, V_ASN1_OBJECT,
			OBJ_nid2obj(NID_pkcs7_data));
	   
	PKCS7_add1_attrib_digest(signer_info, out, hash_size); 

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

	unsigned char *buf2, *p, *sig_buf;
	sig_buf = NULL;

	len = i2d_PKCS7(p7, NULL); 
	buf2 = (unsigned char *)OPENSSL_malloc(len); 
	p = buf2; 
	i2d_PKCS7(p7, &p);

	signature_hex_string = BinaryToHexString(buf2, len);
	
	PKCS7_free(p7);

	return signature_hex_string;
err:
	ERR_load_crypto_strings();
	ERR_print_errors_fp(stderr);
	return NULL;
}

