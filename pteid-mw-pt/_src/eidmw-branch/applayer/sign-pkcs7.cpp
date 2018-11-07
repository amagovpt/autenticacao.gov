
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include "MWException.h"
#include "Log.h"
#include "MiscUtil.h"
#include "ByteArray.h"
#include "APLCard.h"
#include "APLCertif.h"
#include "TsaClient.h"

#include "CardPteidDef.h"
#include "cryptoFwkPteid.h"

#include "sign-pkcs7.h"
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

#ifdef WIN32
    #define _TRACE_(to, format, ... )           { fprintf( to, "%s, %s(), %d - ", __FILE__, __FUNCTION__, __LINE__ ); \
                                                    fprintf( to, format, __VA_ARGS__ ); }
    #define TRACE_ERR( format, ... )            _TRACE_( stderr, format, __VA_ARGS__ )
    #define TRACE_OUT( format, ... )            _TRACE_( stdout, format, __VA_ARGS__ )
#else
    #define _TRACE_(to, format, ... )           { fprintf( to, "%s, %s(), %d - ", __FILE__, __FUNCTION__, __LINE__ ); \
                                                    fprintf( to, format, ## __VA_ARGS__ ); }
    #define TRACE_ERR( format, ... )            _TRACE_( stderr, format, ## __VA_ARGS__ )
    #define TRACE_OUT( format, ... )            _TRACE_( stdout, format, ## __VA_ARGS__ )
#endif

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


CByteArray PteidSign(APL_Card *card, CByteArray &to_sign)
{
	//True for the signatureKey and SHA-256 params
	CByteArray output = card->Sign(to_sign, true, true);

	return output;
}


#if 0
/* It can be useful to debug the PKCS7 encoding   */
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


void add_certificate(PKCS7 *p7, const CByteArray &cert)
{
	const unsigned char * cert_data = cert.GetBytes();
	X509 *x509 = d2i_X509(NULL, &cert_data, cert.Size());
	if (x509 == NULL)
		goto err;

	PKCS7_add_certificate(p7, x509);

	return;
	err:
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add certificate from ByteArray");
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
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to add certificate.");
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

#ifdef DEBUG
    BIO *bio_out;
    bio_out = BIO_new(BIO_s_file());

    BIO_set_fp(bio_out, stderr, BIO_NOCLOSE);

    TS_RESP_print_bio(bio_out, tsp);
#endif

    TS_VERIFY_CTX * verify_ctx = TS_VERIFY_CTX_new();
    verify_ctx->flags = TS_VFY_VERSION;

    if(TS_RESP_verify_response(verify_ctx, tsp) != 1) {

        MWLOG(LEV_ERROR, MOD_APL, "Failed to verify TS response! Details: %s", 
            ERR_error_string(ERR_get_error(), NULL));
        return 1;
    }
	if (tsp != NULL)
	{

		PKCS7* token = tsp->token;

		int p7_len = i2d_PKCS7(token, NULL);
		unsigned char *p7_der = (unsigned char *)OPENSSL_malloc(p7_len);
		unsigned char *p = p7_der;
		i2d_PKCS7(token, &p);

		if (!PKCS7_type_is_signed(token))
		{
			MWLOG(LEV_ERROR, MOD_APL, L"Error in timestamp token: not signed!");
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
			L"Error decoding timestamp token! The TSA is returning bogus data or we have proxy issues and we are getting HTML messages here");
		return 1;
	}

	return 0;

}


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
	MWLOG(LEV_ERROR, MOD_APL, L"Failed to add SigningCertificateV2 attribute.");
}

void addCertificateChain(PKCS7 *p7) 
{
    
    APL_Card *card = AppLayer.getReader().getCard();
    APL_SmartCard * eid_card = static_cast<APL_SmartCard *> (card);
    APL_Certifs *certs = eid_card->getCertificates();
    APL_Certif *signature_cert = certs->getCert(APL_CERTIF_TYPE_SIGNATURE);

    APL_Certif *certif = signature_cert;
    
    while(!certif->isRoot())
    {
        APL_Certif * issuer = NULL;
	    issuer = certif->getIssuer();

        if (issuer == NULL) {
            MWLOG(LEV_ERROR, MOD_APL, "addCertificateChain() Couldn't find issuer for cert: %s", certif->getOwnerName());
            break;
        }

        MWLOG(LEV_DEBUG, MOD_APL, "signPKCS7: addCertificateChain: Loading cert: %s", issuer->getOwnerName());
        add_certificate(p7, certif->getData());
        certif = issuer;
    }

}

void addExternalCertificateChain(PKCS7 *p7, std::vector<CByteArray> ca_certificates) {

    for (int i = 0; i != ca_certificates.size(); i++)
        add_certificate(p7, ca_certificates.at(i));
}

/*  *********************************************************
    ***          computeHash_pkcs7()                    ***
    ********************************************************* */
CByteArray computeHash_pkcs7( unsigned char *data, unsigned long dataLen
                            , CByteArray certificate
                            , std::vector<CByteArray> &ca_certificates
                            , bool timestamp
                            , PKCS7 *p7
                            , PKCS7_SIGNER_INFO **out_signer_info) {
    CByteArray outHash;
    bool isError = false;
    unsigned char *attr_buf = NULL;
    int auth_attr_len = 0;
    unsigned char *attr_digest = NULL;
    unsigned char *out = NULL;
    PKCS7_SIGNER_INFO *signer_info = NULL;
    X509 *x509 = NULL;

    //Function pointer to the correct hash function
    HashFunc hash_fn = &SHA256_Wrapper;

    if ( out_signer_info ) *out_signer_info = NULL;

    if ( NULL == data ) {
        TRACE_ERR( "Null data" );
        isError = true;

        goto err_hashCalculate;
    }

    if ( 0 == dataLen ){
        TRACE_ERR( "Invalid dataLen" );
        isError = true;

        goto err_hashCalculate;
    }

    if ( NULL == p7 ){
        TRACE_ERR( "Null p7" );

        isError = true;
        goto err_hashCalculate;
    }

    x509 = DER_to_X509( certificate.GetBytes(), certificate.Size() );
    if ( NULL == x509){
        MWLOG(LEV_ERROR, MOD_APL, "Error decoding certificate data!");
        isError = true;
        goto err_hashCalculate;
    }

    out = (unsigned char *)malloc( SHA256_LEN );
    if ( NULL == out ){
        TRACE_ERR( "Null out" );

        isError = true;
        goto err_hashCalculate;
    }

    attr_digest = (unsigned char *)malloc( SHA256_LEN );
    if ( NULL == attr_digest ){
        TRACE_ERR( "Null attr_digest" );
        isError = true;

        goto err_hashCalculate;
    }

    PKCS7_set_type( p7, NID_pkcs7_signed );

    if ( !PKCS7_content_new( p7, NID_pkcs7_data ) ){
        TRACE_ERR( "PKCS7_content_new failed" );
        isError = true;

        goto err_hashCalculate;
    }

    signer_info = PKCS7_add_signature(p7, x509, X509_get_pubkey(x509), EVP_sha256());

    if ( signer_info == NULL ) {
        TRACE_ERR("Null signer_info");
        isError = true;

        goto err_hashCalculate;
    }

    PKCS7_add_certificate( p7, x509 );

    //For non-card signatures we need to add the supplied CA certificates
    if (ca_certificates.size() > 0) {
        addExternalCertificateChain(p7, ca_certificates);
    }
    else {
        addCertificateChain(p7);
    }

    PKCS7_set_detached( p7, 1);

    hash_fn( data, dataLen, out);

    /* Add the signing time and digest authenticated attributes */
    //With authenticated attributes
    PKCS7_add_signed_attribute( signer_info
                                , NID_pkcs9_contentType
                                , V_ASN1_OBJECT
                                , OBJ_nid2obj(NID_pkcs7_data) );

    PKCS7_add1_attrib_digest( signer_info
                            , out
                            , SHA256_LEN);

    /*
        Add signing-certificate v2 attribute according to the
        specification ETSI TS 103 172 v2.1.1 - section 6.3.1
    */
    add_signingCertificate(signer_info
                            , x509
                            , certificate.GetBytes(), certificate.Size() );

    if ( !timestamp ) add_signed_time( signer_info );

    auth_attr_len = ASN1_item_i2d( (ASN1_VALUE *)signer_info->auth_attr
                                    , &attr_buf
                                    , ASN1_ITEM_rptr( PKCS7_ATTR_SIGN ) );

    hash_fn( (unsigned char *)attr_buf, auth_attr_len, attr_digest );
    outHash = CByteArray( (const unsigned char *)attr_digest, SHA256_LEN );

    if ( out_signer_info ) *out_signer_info = signer_info;

err_hashCalculate:
    if (x509 != NULL) X509_free(x509);
    if (attr_digest != NULL) free(attr_digest);
    if (out != NULL) free(out);

    if (isError) {
        ERR_load_crypto_strings();
        ERR_print_errors_fp(stderr);
    }

    return outHash;
}

/*  *********************************************************
    ***          getSignedData_pkcs7()                    ***
    ********************************************************* */
int getSignedData_pkcs7( unsigned char *signature, unsigned int signatureLen
                        , PKCS7_SIGNER_INFO *signer_info
                        , bool timestamp
                        , PKCS7 *p7
                        , const char **signature_contents ){
    int return_code = 0;
    unsigned char *timestamp_token = NULL;
    int tsp_token_len = 0;
    char *signature_hex_string = NULL;
    CByteArray tsresp;

    if ( NULL == signature ){
        TRACE_ERR( "Null signature" );
        return 2;
    }

    if ( 0 == signatureLen ){
        TRACE_ERR( "Zero signatureLen" );
        return 2;
    }

    if ( NULL == signer_info ){
        TRACE_ERR( "Null signer_info" );
        return 2;
    }

    if ( NULL == p7 ){
        TRACE_ERR( "Null p7" );
        return 2;
    }

    //Manually fill-in the signedData structure
    signer_info->enc_digest = ASN1_OCTET_STRING_new();
    ASN1_OCTET_STRING_set( signer_info->enc_digest, signature, signatureLen );

    if (timestamp) {
        TSAClient tsp;
        unsigned char *digest_tp = (unsigned char *)malloc( SHA256_LEN );
        if ( NULL == digest_tp ){
            TRACE_ERR( "digest_tp: malloc failed" );
            return 2;
        }

        //Compute SHA-256 of the signed digest
        SHA256_Wrapper( signature, signatureLen, digest_tp );

        tsp.timestamp_data( digest_tp, SHA256_LEN );
        tsresp = tsp.getResponse();

        if (tsresp.Size() == 0) {
            MWLOG( LEV_ERROR, MOD_APL, L"PKCS7 Sign: Timestamp Error - response is empty\n" );
            return_code = 1;
        } else {
            timestamp_token = tsresp.GetBytes();
            tsp_token_len = tsresp.Size();
        }
        free(digest_tp);
    }

    if ( timestamp_token && tsp_token_len > 0 ) {
        return_code = append_tsp_token(signer_info, timestamp_token, tsp_token_len);
    }

    unsigned char *buf2, *p;
    unsigned int len = i2d_PKCS7( p7, NULL );
    p = buf2 = (unsigned char *)OPENSSL_malloc( len );
    i2d_PKCS7( p7, &p );

    *signature_contents = BinaryToHexString( buf2, len );
    OPENSSL_free( buf2 );

   return return_code;
}

}

