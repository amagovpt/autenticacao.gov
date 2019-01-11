/**
*
**  PTeID Middleware Project.
**  Copyright (C) 2011-2012
**  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
**
**  XAdES and XAdES-T signature generator and validator
**
*/


#include <iostream>
#include <fstream>
#include <cstdio>

#include "APLCard.h"
#include "APLConfig.h"
#include "APLCertif.h"

#include "CardPteidDef.h"
#include "XadesSignature.h"
#include "MWException.h"
#include "eidErrors.h"
#include "Util.h"
#include "cryptoFwkPteid.h"
#include "CRLFetcher.h"
#include "MiscUtil.h"

//for Timestamping
#include "TsaClient.h"

//For OCSP
#include "OCSP.h"

#include "Log.h"
#include "ByteArray.h"

// Xerces
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/Janitor.hpp>

// XML-Security-C (XSEC)

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/dsig/DSIGObject.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoBase64.hpp>
#include <xsec/enc/XSECKeyInfoResolverDefault.hpp>

#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/transformers/TXFMChain.hpp>


//OpenSSL
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pkcs7.h>

//stat
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#include <Shlwapi.h> //UrlCreateFromPath()
#endif

//Solve stupid differences in Windows standards compliance
#ifndef WIN32
#define _stat stat
#define _read read
#define _fileno fileno
#define _snprintf snprintf
#endif

#define SHA1_LEN 20
#define SHA256_LEN 32

XERCES_CPP_NAMESPACE_USE

namespace eIDMW
{


	#define ASIC_NAMESPACE "http://uri.etsi.org/2918/v1.2.1#"
	#define XADES_NAMESPACE "http://uri.etsi.org/01903/v1.3.2#"
	#define DSIG_NAMESPACE "http://www.w3.org/2000/09/xmldsig#"

	/*
	CByteArray XadesSignature::HashFile(const char *file_path)
	{

		std::ifstream file(file_path, std::ios::binary|std::ios::ate);
		char * in;
		unsigned char out[SHA1_LEN];

		int size = -1;
		if (file.is_open())
		{
			size = file.tellg();
			in = new char [size];
			file.seekg (0, std::ios::beg);
			file.read (in, size);
		}
		file.close();

		//Special-case empty files
		if (size == 0)
		{
			in = new char[SHA1_LEN];

		}
		//Save the referenced data for the ArchiveTimestamp calculation
		else if (m_do_long_term_validation)
		{
			//ifstream::read doesnt terminate the string!
			char * tmp = (char*)malloc(size+1);
			memcpy(tmp, in, size);
			tmp[size] = '\0';
			m_referenced_data += (const char *)tmp;
		}
		//OpenSSL call
		SHA1 ((unsigned char *)in, size, out);
		return CByteArray((const unsigned char*)out, 20L);

	}

	*/


	//Implemented in sign-pkcs7.cpp should be moved to a common file though
	unsigned int SHA256_Wrapper(unsigned char *data, unsigned long data_len, unsigned char *digest);

	CByteArray XadesSignature::HashFile(const char *filename)
	{
		const int BUFSIZE = 4*1024;
		EVP_MD_CTX *mdctx;
#ifdef WIN32
        struct _stat64 sb;
#else
		struct stat sb;
#endif
		long long filesize;
		unsigned char md_value[EVP_MAX_MD_SIZE];
		unsigned int md_len, i;
		char buffer[BUFSIZE];

		OpenSSL_add_all_digests();
#ifdef WIN32
		std::wstring utf16FileName = utilStringWiden(std::string(filename));
		FILE *fp = _wfopen(utf16FileName.c_str(), L"rb");
#else
		FILE *fp = fopen(filename, "rb");
#endif
		if (!fp)
		{
			fprintf(stderr, "Error opening file!\n");
			return CByteArray();
		}

		mdctx = EVP_MD_CTX_create();
		EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);

#ifdef WIN32
        _wstat64(utf16FileName.c_str(), &sb);
#else
        stat(filename, &sb);
#endif
        filesize = (long long)sb.st_size;

		if (filesize <= BUFSIZE)
		{
			size_t read_b = fread(buffer, 1, filesize, fp);
			if (read_b < filesize)
				fprintf(stderr, "IO error in first block!\n");
			EVP_DigestUpdate(mdctx, buffer, filesize);
		}
		else
		{
			while(1)
			{
				int read_b = fread(buffer, 1, BUFSIZE, fp);
				EVP_DigestUpdate(mdctx, buffer, read_b);
                if (ferror(fp)) {
                    MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::HashFile: Failed while reading file");
                }
				if (read_b < BUFSIZE)
					break;
			}

		}
		fclose(fp);

		if (m_do_long_term_validation)
		{
			//Cache the intermediate hash to reuse it later for ArchiveTimestamp
			m_digest_state = EVP_MD_CTX_create();
			EVP_MD_CTX_copy_ex(m_digest_state, mdctx);
		}

		EVP_DigestFinal_ex(mdctx, md_value, &md_len);
		EVP_MD_CTX_destroy(mdctx);

		return CByteArray(md_value, SHA256_LEN);
	}

	static XMLCh s_Id[] = {

	chLatin_I,
	chLatin_d,
	chNull
        };

        string x509_getSerial(X509 *cert)
        {
        	string serial;

        	BIGNUM *bn = ASN1_INTEGER_to_BN(X509_get_serialNumber(cert), 0);

        	if(bn)
        	{
        		char *str = BN_bn2dec(bn);
        		if(str)
        			serial = str;
        		OPENSSL_free(str);
        		BN_free(bn);
        	}
        	return serial;
        }


        static std::string x509_name_toString(X509_NAME* name)
        {

        	std::string str;

        	BIO* mem = BIO_new(BIO_s_mem());

            // Convert the X509_NAME struct to string.
        	if(X509_NAME_print_ex(mem, name, 0, XN_FLAG_RFC2253) < 0)
        	{
        		BIO_free(mem);
        		fprintf(stderr, "Failed to convert X509_NAME struct to string");
        	}

        	BUF_MEM *data = 0;
        	BIO_get_mem_ptr(mem, &data);
        	str.append(data->data, data->length);
        	BIO_free(mem);

        	return str;
        }

	//Nasty Macros, beware !!
#define CREATE_DOM_NODE doc->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());
#define CREATE_DSIG_NODE doc->createElementNS(XMLString::transcode(DSIG_NAMESPACE), str.rawXMLChBuffer());

#define SIGNED_PROPS_ID "S0-SignedProperties"


std::basic_string<XMLCh> generateNodeID()
{
	std::basic_string<XMLCh> id_buffer;
	id_buffer.append(XMLString::transcode("xades-"));
	id_buffer.append(generateId(20));

	return id_buffer;
}

std::basic_string<XMLCh> createSignedPropertiesURI()
{
	std::basic_string<XMLCh> id_buffer;
	id_buffer.append(XMLString::transcode("#"));
	id_buffer.append(XMLString::transcode(SIGNED_PROPS_ID));

	return id_buffer;
}

    char *getUtcTime()
    {
    	char * date_outstr = (char*)malloc(100);
    	time_t t = time(NULL);
        struct tm * tmp_date = gmtime(&t);
        //Date String for visible signature
        strftime(date_outstr, 100, "%Y-%m-%dT%H:%M:%SZ", tmp_date);

        return date_outstr;
    }


#ifdef DEBUG
    void WriteToFile(const char *path, const char *str)
    {
    	FILE *f=NULL;

        f = fopen(path, "wb");

        if(f)
        {
                size_t len = fwrite(str,strlen(str), 1, f);
                fclose(f);
        }

    }
#endif

	void appendCertRef(XERCES_NS DOMDocument *doc, CByteArray &cert_data, X509* cert, DOMNode* parent)
	{
		safeBuffer str;
		unsigned char cert_digest[SHA256_LEN];
		XMLCh *prefix = XMLString::transcode("etsi");
		XMLCh *prefix_ds = XMLString::transcode("");
		const std::string SHA1_ID = "http://www.w3.org/2000/09/xmldsig#sha1";
		const std::string SHA256_ID = "http://www.w3.org/2001/04/xmlenc#sha256";

		SHA256_Wrapper(cert_data.GetBytes(), cert_data.Size(), cert_digest);

		makeQName(str, prefix, "Cert");
		DOMNode * n_Cert = CREATE_DOM_NODE
		makeQName(str, prefix, "CertDigest");
		DOMNode * n_CertDigest = CREATE_DOM_NODE
		makeQName(str, prefix_ds, "DigestMethod");
		DOMNode * n_CertDigestMeth = CREATE_DSIG_NODE;
		makeQName(str, prefix_ds, "DigestValue");
		DOMNode * n_CertDigestValue = CREATE_DSIG_NODE;
		makeQName(str, prefix, "IssuerSerial");
		DOMNode * n_CertIssuerSerial = CREATE_DOM_NODE;
		makeQName(str, prefix_ds, "X509IssuerName");
		DOMNode * n_CertIssuerName = CREATE_DSIG_NODE;
		makeQName(str, prefix_ds, "X509SerialNumber");
		DOMNode * n_CertSerialNumber = CREATE_DSIG_NODE;

		//Add SigningCertificate Element containing digest, serial and issuer
		std::string issuer = x509_name_toString(X509_get_issuer_name(cert));
		std::string serial = x509_getSerial(cert);

		XMLCh *CertDigest = EncodeToBase64XMLCh(cert_digest, SHA256_LEN);

		((DOMElement *)n_CertDigestMeth)->setAttribute(XMLString::transcode("Algorithm"),
					 XMLString::transcode(SHA256_ID.c_str()));
		n_CertDigestValue->appendChild(doc->createTextNode(CertDigest));
		n_CertSerialNumber->appendChild(doc->createTextNode(
			XMLString::transcode(serial.c_str())) );
		n_CertIssuerName->appendChild(doc->createTextNode(
			XMLString::transcode(issuer.c_str())));

		parent->appendChild(n_Cert);
		n_Cert->appendChild(n_CertDigest);
		n_CertDigest->appendChild(n_CertDigestMeth);
		n_CertDigest->appendChild(n_CertDigestValue);
		n_Cert->appendChild(n_CertIssuerSerial);
		n_CertIssuerSerial->appendChild(n_CertIssuerName);
		n_CertIssuerSerial->appendChild(n_CertSerialNumber);
	}


	void XadesSignature::addCardSignature(unsigned char *signature, unsigned int siglen, XERCES_NS DOMDocument *doc)
	{

		char * base64Sig = Base64Encode(signature, siglen);
		DOMNode *signatureValueNode = NULL;

		DOMNodeList* nodes1 = doc->getElementsByTagNameNS(XMLString::transcode(DSIG_NAMESPACE),
	 		XMLString::transcode("SignatureValue"));

		signatureValueNode = nodes1->item(0);

		// Now we have the signature - place it in the DOM structures

		DOMNode *tmpElt = signatureValueNode->getFirstChild();

		while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
			tmpElt = tmpElt->getNextSibling();

		if (tmpElt == NULL) {
			// Need to create the underlying TEXT_NODE
			//DOMDocument * doc = signatureValueNode->getOwnerDocument();
			tmpElt = doc->createTextNode(MAKE_UNICODE_STRING((char *) base64Sig));
			signatureValueNode->appendChild(tmpElt);
		}
		else {
			tmpElt->setNodeValue(MAKE_UNICODE_STRING((char *) base64Sig));

		}

		free(base64Sig);

	}

	DOMNode *XadesSignature::addSignatureProperties(DSIGSignature *sig, XMLCh *sig_id, CByteArray &signing_cert_data)
	{
		std::basic_string<XMLCh> props_target;
		char *utcTime = getUtcTime();

		props_target.append(XMLString::transcode("#"));
		props_target.append(sig_id);

		X509 *signing_cert = mp_cert;
		XMLCh *prefix = XMLString::transcode("etsi");
		XMLCh *prefix_ds = XMLString::transcode("");
		safeBuffer str;

		XERCES_NS DOMDocument *doc = sig->getParentDocument();
		DSIGObject * obj1 = sig->appendObject();

		makeQName(str, prefix, "QualifyingProperties");
		DOMNode * n1 = CREATE_DOM_NODE;

		((DOMElement *)n1)->setAttributeNS(XMLString::transcode(XADES_NAMESPACE), XMLString::transcode("Target"),
			 props_target.c_str());

		makeQName(str, prefix, "UnsignedProperties");
		DOMNode * n2 = CREATE_DOM_NODE;

		makeQName(str, prefix, "UnsignedSignatureProperties");
		DOMNode * n3 = CREATE_DOM_NODE
		makeQName(str, prefix, "SignedProperties");
		DOMNode * n4 = CREATE_DOM_NODE;
		((DOMElement *)n4)->setAttribute(s_Id, XMLString::transcode(SIGNED_PROPS_ID));

		makeQName(str, prefix, "SignedSignatureProperties");
		DOMNode * n_signSigProps = CREATE_DOM_NODE
		makeQName(str, prefix, "SigningTime");
		DOMNode * n_signingTime = CREATE_DOM_NODE
		makeQName(str, prefix, "SigningCertificate");
		DOMNode * n_signSigningCert = CREATE_DOM_NODE

		appendCertRef(doc, signing_cert_data, signing_cert, n_signSigningCert);

		n_signingTime->appendChild(doc->createTextNode(XMLString::transcode(utcTime)));

		//Establish nodes structure
		n1->appendChild(n2);
		n1->appendChild(n4);
		n4->appendChild(n_signSigProps);
		n_signSigProps->appendChild(n_signingTime);
		n_signSigProps->appendChild(n_signSigningCert);


		n2->appendChild(n3);

		obj1->appendChild(n1);

		free(utcTime);

		return n1;
	}

	CByteArray *XadesSignature::WriteToByteArray(XERCES_NS DOMDocument * doc)
	{
		CByteArray * ba_out = new CByteArray();
		XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
		DOMImplementation *impl =
			            DOMImplementationRegistry::getDOMImplementation(tempStr);

		// construct the DOMWriter
	        DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
		DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();


		 // construct the MemBufFormatTarget
		 MemBufFormatTarget * myFormatTarget = new MemBufFormatTarget();
		 theOutputDesc->setByteStream(myFormatTarget);
		 theOutputDesc->setEncoding(XMLString::transcode("UTF-8"));
		 theSerializer->write(doc, theOutputDesc);

		// serialize the DOMNode to a UTF-8 string
		 const XMLByte* utf8_string = myFormatTarget->getRawBuffer();
		 XMLSize_t size = myFormatTarget->getLen();
		 MWLOG(LEV_DEBUG, MOD_APL, L"XadesSignature::WriteToByteArray: Returning XML byte array, size=%d", size);

		 ba_out->Append((const unsigned char *)utf8_string, size);
		 return ba_out;

	}


void XadesSignature::loadSignerCert(CByteArray &data, EVP_PKEY *pub_key)
{

	unsigned char * data_cert = data.GetBytes();
	//OpenSSL call
	mp_cert = d2i_X509(NULL, (const unsigned char**)(&data_cert), data.Size());
	m_certs.push_back(mp_cert);

	if (mp_cert == NULL)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"loadCert() Error decoding certificate data!");
	}

}
void XadesSignature::initXMLUtils()
{
	try {

		XMLPlatformUtils::Initialize();
		XSECPlatformUtils::Initialise();

	}
	catch (const XMLException &e) {

		MWLOG(LEV_ERROR, MOD_APL, L"Error during initialisation of Xerces. Error Message: %s",
		     e.getMessage()) ;
		throw CMWEXCEPTION(EIDMW_XERCES_INIT_ERROR);
	}
}

void XadesSignature::terminateXMLUtils()
{
	XSECPlatformUtils::Terminate();
	XMLPlatformUtils::Terminate();
}


XMLCh* XadesSignature::createURI(const char *path)
{
	char * filename = Basename((char *)path);
	string uri = urlEncode((unsigned char *)filename, strlen(filename));
	return XMLString::transcode(uri.c_str());
}

int XadesSignature::HashSignedInfoNode(XERCES_NS DOMDocument *doc, XMLByte *hash_buf)
{
	CByteArray *partial_xml_file = WriteToByteArray(doc);

	auto_ptr<XercesDOMParser> parser(new XercesDOMParser());
    parser->setDoNamespaces(true);
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoSchema(true);
    parser->setCreateEntityReferenceNodes(false);

    // Parse and return a copy of the Xerces DOM tree.
	// Save to file and parse it again, to make XML Canonicalization work
    // correctly as expected by the Canonical XML 1.0 specification.
    // Hope, the next Canonical XML specification fixes the white spaces preserving "bug".

    MemBufInputSource source((XMLByte*)partial_xml_file->GetBytes(), partial_xml_file->Size(), "temp");
    parser->parse(source);
    XERCES_NS DOMDocument *new_dom = parser->getDocument();

	DOMNodeList *dom_nodes = new_dom->getElementsByTagNameNS(XMLString::transcode(DSIG_NAMESPACE),
	 XMLString::transcode("SignedInfo"));

	//Get first SignedProperties
	DOMNode *node = dom_nodes->item(0);
	if (!node)
	{
		delete partial_xml_file;
		fprintf(stderr, "HashSignedInfoNode(): Failed to find SignedInfo node!\n");
		return 0;
	}

	XSECC14n20010315 canonicalizer(new_dom, node);
	//TODO: check these params
    canonicalizer.setCommentsProcessing(false);
    canonicalizer.setUseNamespaceStack(true);
    canonicalizer.setExclusive();

    string c14n;
    unsigned char buffer[1024];
    xsecsize_t bytes = 0;
    while((bytes = canonicalizer.outputBuffer(buffer, 1024)) > 0)
    {
        c14n.append( (char*)&buffer[0], size_t(bytes));
    }
    //fprintf(stderr, "DEBUG: C14n SignedProperties: %s\n", c14n.c_str());

    //TODO: Should be configurable
    SHA256_Wrapper((unsigned char*)c14n.c_str(), c14n.size(), hash_buf);

    delete partial_xml_file;
    return SHA256_LEN;

}


int XadesSignature::HashSignedPropertiesNode(XERCES_NS DOMDocument *doc, XMLByte *hash_buf)
{
	CByteArray *partial_xml_file = WriteToByteArray(doc);

	auto_ptr<XercesDOMParser> parser(new XercesDOMParser());
    parser->setDoNamespaces(true);
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoSchema(true);
    parser->setCreateEntityReferenceNodes(false);

    // Parse and return a copy of the Xerces DOM tree.
	// Save to file and parse it again, to make XML Canonicalization work
    // correctly as expected by the Canonical XML 1.0 specification.
    // Hope, the next Canonical XML specification fixes the white spaces preserving "bug".

    MemBufInputSource source((XMLByte*)partial_xml_file->GetBytes(), partial_xml_file->Size(), "temp");
    parser->parse(source);
    XERCES_NS DOMDocument *new_dom = parser->getDocument();

	DOMNodeList *dom_nodes = new_dom->getElementsByTagNameNS(XMLString::transcode(XADES_NAMESPACE),
	 XMLString::transcode("SignedProperties"));

	//Get first SignedProperties
	DOMNode *node = dom_nodes->item(0);
	if (!node)
	{
		fprintf(stderr, "HashSignedPropertiesNode(): Failed to find SignedProperties node!\n");
		delete partial_xml_file;
		return 0;
	}

	XSECC14n20010315 canonicalizer(new_dom, node);
	//TODO: check these params
    canonicalizer.setCommentsProcessing(false);
    canonicalizer.setUseNamespaceStack(true);
    canonicalizer.setExclusive();

    string c14n;
    unsigned char buffer[1024];
    xsecsize_t bytes = 0;
    while((bytes = canonicalizer.outputBuffer(buffer, 1024)) > 0)
    {
        c14n.append( (char*)&buffer[0], size_t(bytes));
    }
    // fprintf(stderr, "DEBUG: C14n SignedProperties: %s\n", c14n.c_str());

    //TODO: Should be configurable
    SHA256_Wrapper((unsigned char*)c14n.c_str(), c14n.size(), hash_buf);

    delete partial_xml_file;
    return SHA256_LEN;
}

void XadesSignature::addCertificateToKeyInfo(const CByteArray &cert, DSIGKeyInfoX509 *keyInfo)
{
	OpenSSLCryptoX509 * ssl_cert = NULL;

	const unsigned char * data_cert = cert.GetBytes();

	X509 *cert_ca_sign = d2i_X509(NULL, &data_cert, cert.Size());

	if (cert_ca_sign == NULL)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"loadCert() Error decoding certificate data (CA Sign)");
		return;
	}

	m_certs.push_back(cert_ca_sign);

	ssl_cert = new OpenSSLCryptoX509(cert_ca_sign);

	keyInfo->appendX509Certificate(ssl_cert->getDEREncodingSB().sbStrToXMLCh());

	delete ssl_cert;
}

void XadesSignature::addCertificateChain(DSIGKeyInfoX509 *keyInfo)
{

    APL_SmartCard * eid_card = static_cast<APL_SmartCard *> (mp_card);
    APL_Certifs *certs = eid_card->getCertificates();
    APL_Certif *signature_cert = certs->getCert(APL_CERTIF_TYPE_SIGNATURE);

    APL_Certif *certif = signature_cert;
    
    while(!certif->isRoot())
    {
		APL_Certif * issuer = NULL;
        issuer = certif->getIssuer();

		if (issuer == NULL) {
			MWLOG(LEV_ERROR, MOD_APL, "XadesSignature: addCertificateChain() Couldn't find issuer for cert: %s", certif->getOwnerName());
            break;
        }

		MWLOG(LEV_DEBUG, MOD_APL, "XadesSignature: addCertificateChain: Loading cert: %s", issuer->getOwnerName());
		addCertificateToKeyInfo(issuer->getData(), keyInfo);
		m_cert_bas.push_back(issuer->getData());
		certif = issuer;
	}

}


/*
	Parse a timestamp token from a full TS Reply accounting for different elements appearing in the
	top-level SEQUENCE before the signedData Object as we've seen using various Timestamp servers
*/
CByteArray ParseTimestampTokenFromTSReply(CByteArray &ts_reply) {

	//This is the ASN.1 encoding of the Object Identifier for PKCS7 SignedData 1.2.840.113549.1.7.2
	const char * signedData_raw_oid = "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x07\x02";
	unsigned char *array_start = ts_reply.GetBytes();

	unsigned char * needle = (unsigned char *)memmem(array_start, ts_reply.Size(), signedData_raw_oid,
		strlen(signedData_raw_oid));

	if (needle == NULL)
	{
		//Broken TS Reply!
		MWLOG(LEV_ERROR, MOD_APL,
			L"An error occurred in timestamp_data. The returned data does not contain a PKCS7 signedData object");
		return CByteArray();
	}

	//4 bytes earlier to capture the Enclosing Sequence Tag and Length
	unsigned char * sequence_start = needle-4;

	if (*sequence_start != 0x30)
	{
		MWLOG(LEV_ERROR, MOD_APL,
			L"An error occurred in timestamp_data. Parsing failure! This should never happen...");
		return CByteArray();
	}

	long new_len = ts_reply.Size() - (int)(sequence_start-array_start);
	return CByteArray(sequence_start, new_len);
}

/*
* 	Add Revocation Info for all certificates in the chain up
*	to EC Raiz Estado which is trusted in the Portuguese TSL
*/
bool XadesSignature::AddRevocationInfo(XERCES_NS DOMDocument * dom)
{
	safeBuffer str;
	CByteArray ocsp_token;
	APL_CryptoFwkPteid *crypto_fwk = AppLayer.getCryptoFwk();

	DOMNode *node_unsigned_props;
	XMLCh * prefix_xades = XMLString::transcode("etsi");

	DOMNodeList* nodes1 = dom->getElementsByTagNameNS(XMLString::transcode(XADES_NAMESPACE),
		XMLString::transcode("UnsignedSignatureProperties"));

	if ((node_unsigned_props = nodes1->item(0)) == NULL)
	{
		fprintf(stderr, "Couldnt find UnsignedSignatureProperties node!\n" );
		return false;
	}
	makeQName(str, prefix_xades, "RevocationValues");
	DOMElement * revocation_node =
	dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

	makeQName(str, prefix_xades, "OCSPValues");
	DOMElement * ocsp_values_node =
	dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

	makeQName(str, prefix_xades, "CRLValues");
	DOMElement * crl_values_node =
	dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

	for (int i = 0; i != m_certs.size()-1; i++)
	{
		char * ocsp_url = crypto_fwk->GetOCSPUrl(m_certs.at(i));
		bool try_ocsp = ocsp_url != NULL && strlen(ocsp_url) > 0;

		if (try_ocsp)
		{
			ocsp_token = sendOCSPRequest(m_certs.at(i), m_certs.at(i+1), ocsp_url);

			if (ocsp_token.Size() > 0)
			{

				char *base64_str = Base64Encode(ocsp_token.GetBytes(), ocsp_token.Size());
				makeQName(str, prefix_xades, "EncapsulatedOCSPValue");
				DOMElement * ocsp_node =
				dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

				XMLCh * ocsp_base64 = XMLString::transcode(base64_str);
				ocsp_node->appendChild(dom->createTextNode(ocsp_base64));
				ocsp_values_node->appendChild(ocsp_node);

				free(base64_str);
			}

		}
		free(ocsp_url);
		//Try to download a CRL if the cert doesnt point to a OCSP responder
		//or if it failed to provide a valid response
		if (!try_ocsp || ocsp_token.Size() == 0)
		{
			char *crl_url = crypto_fwk->GetCDPUrl(m_certs.at(i));
			if (crl_url != NULL && strlen(crl_url) > 0)
			{
				CRLFetcher crl_fetcher;

				CByteArray crl_data = crl_fetcher.fetch_CRL_file(crl_url);

				if (crl_data.Size() == 0)
				{
					MWLOG(LEV_WARN, MOD_APL,
						L"XadesSignature: failed to fetch CRL for certificate! Revocation data in Xades-A will be incomplete...");
                    free(crl_url);
					continue;
				}

				char *base64_str = Base64Encode(crl_data.GetBytes(), crl_data.Size());
				makeQName(str, prefix_xades, "EncapsulatedCRLValue");
				DOMElement * crl_node =
				dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());
				crl_node->appendChild(dom->createTextNode(XMLString::transcode(base64_str)));
				crl_values_node->appendChild(crl_node);
				free(base64_str);


			}
			if (crl_url != NULL) free(crl_url);
		}
	}

	node_unsigned_props->appendChild(revocation_node);
	revocation_node->appendChild(ocsp_values_node);
	revocation_node->appendChild(crl_values_node);

	return true;
}

// Mandatory for XAdES-A signature. Applying a Timestamp to these references
// protects all the CA certificates against later key compromise or algorithm obsolescence
bool XadesSignature::addCompleteCertificateRefs(XERCES_NS DOMDocument *dom)
{
	DOMNode * node_unsigned_props = NULL;
	safeBuffer str;
	XMLCh * prefix_xades = XMLString::transcode("etsi");

	DOMNodeList* nodes1 = dom->getElementsByTagNameNS(XMLString::transcode(XADES_NAMESPACE),
	 XMLString::transcode("UnsignedSignatureProperties"));

	if ((node_unsigned_props = nodes1->item(0)) == NULL)
	{
		fprintf(stderr, "Couldn't find UnsignedSignatureProperties node!\n" );
		return false;
	}

	makeQName(str, prefix_xades, "CompleteCertificateRefs");
	DOMElement * complete_cert_refs_node =
			dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());
	makeQName(str, prefix_xades, "CertRefs");
	DOMElement * cert_refs_node =
			dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

	for (int i = 1; i!= m_certs.size(); i++)
	{
		//m_cert_bas doesn't contain the Signer certificate
		appendCertRef(dom, m_cert_bas.at(i-1), m_certs.at(i), cert_refs_node);
	}

	complete_cert_refs_node->appendChild(cert_refs_node);
	node_unsigned_props->appendChild(complete_cert_refs_node);

	//TODO: maybe add some content to this later??
	makeQName(str, prefix_xades, "CompleteRevocationRefs");
	DOMElement * complete_revocation_refs_node =
			dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

	node_unsigned_props->appendChild(complete_revocation_refs_node);
	return true;
}

DOMNode *findDOMNodeHelper(XERCES_NS DOMDocument *dom, const char *ns, const char *tagname)
{
	DOMNode * node;
	DOMNodeList* nodes1 = dom->getElementsByTagNameNS(XMLString::transcode(ns),
	 XMLString::transcode(tagname));

	if ((node = nodes1->item(0)) == NULL)
	{
		fprintf(stderr, "Couldn't find %s node!\n", tagname);
		return NULL;
	}

	return node;
}

std::string canonicalNode(DOMNode *node, XERCES_NS DOMDocument *doc)
{
	XSECC14n20010315 canonicalizer(doc, node);
    canonicalizer.setCommentsProcessing(false);
    canonicalizer.setUseNamespaceStack(true);
    canonicalizer.setExclusive();

    string c14n;
    unsigned char buffer[1024];
    xsecsize_t bytes = 0;
    while((bytes = canonicalizer.outputBuffer(buffer, 1024)) > 0)
    {
        c14n.append((char*)&buffer[0], size_t(bytes));
    }

    return c14n;
}

// This TS applies to the concatenation of the following elements
//(ds:SignatureValue, [SignatureTimeStamp+, CompleteCertificateRefs, CompleteRevocationRefs]).
bool XadesSignature::AddSigAndRefsTimestamp(XERCES_NS DOMDocument *doc)
{
	std::string digest_input;
	CByteArray *partial_xml_file = WriteToByteArray(doc);

	auto_ptr<XercesDOMParser> parser(new XercesDOMParser());
    parser->setDoNamespaces(true);
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoSchema(true);
    parser->setCreateEntityReferenceNodes(false);

    // Parse and return a copy of the Xerces DOM tree.
	// Save to file and parse it again, to make XML Canonicalization work
    // correctly as expected by the Canonical XML 1.0 specification.
    // Hope, the next Canonical XML specification fixes the white spaces preserving "bug".

    MemBufInputSource source((XMLByte*)partial_xml_file->GetBytes(), partial_xml_file->Size(), "temp");
    parser->parse(source);
    XERCES_NS DOMDocument *new_dom = parser->getDocument();

	DOMNode *node_unsigned_props = findDOMNodeHelper(doc, XADES_NAMESPACE, "UnsignedSignatureProperties");
	if (node_unsigned_props == NULL)
	{
		delete partial_xml_file;
		return false;
	}

	DOMNode * signature_node = findDOMNodeHelper(new_dom, DSIG_NAMESPACE, "SignatureValue");
	digest_input += canonicalNode(signature_node, new_dom);

	DOMNode * sig_timestamp_node = findDOMNodeHelper(new_dom, XADES_NAMESPACE, "SignatureTimeStamp");
	digest_input += canonicalNode(sig_timestamp_node, new_dom);

	DOMNode * complete_certs_node = findDOMNodeHelper(new_dom, XADES_NAMESPACE, "CompleteCertificateRefs");
	digest_input += canonicalNode(complete_certs_node, new_dom);
	DOMNode * complete_revoc_node = findDOMNodeHelper(new_dom, XADES_NAMESPACE, "CompleteRevocationRefs");
	digest_input += canonicalNode(complete_revoc_node, new_dom);

	//fprintf(stderr, "SigRefsTimestamp:\n%s\n", digest_input.c_str());
	delete partial_xml_file;

	return appendTimestamp(doc, node_unsigned_props, "SigAndRefsTimeStamp", digest_input);
}

// Mandatory Property for a XAdES-A signature form
// The algorithm to calculate the TS Input data is described in page 58 of ETSI TS 101 903
// [ds:References+, ds:SignedInfo, ds:SignatureValue, ds:KeyInfo, xades:UnsignedSignatureProperties+]
bool XadesSignature::AddArchiveTimestamp(XERCES_NS DOMDocument *dom)
{
	std::string digest_input;

	DOMNode *node_unsigned_props_orig = findDOMNodeHelper(dom, XADES_NAMESPACE, "UnsignedSignatureProperties");

	CByteArray *partial_xml_file = WriteToByteArray(dom);

	auto_ptr<XercesDOMParser> parser(new XercesDOMParser());
    parser->setDoNamespaces(true);
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoSchema(true);
    parser->setCreateEntityReferenceNodes(false);

    // Parse and return a copy of the Xerces DOM tree.
	// Save to file and parse it again, to make XML Canonicalization work
    // correctly as expected by the Canonical XML 1.0 specification.
    // Hope, the next Canonical XML specification fixes the white spaces preserving "bug".

    MemBufInputSource source((XMLByte*)partial_xml_file->GetBytes(), partial_xml_file->Size(), "temp");
    parser->parse(source);
    XERCES_NS DOMDocument *new_dom = parser->getDocument();


	DOMNode *node_unsigned_props = findDOMNodeHelper(new_dom, XADES_NAMESPACE, "UnsignedSignatureProperties");
	if (node_unsigned_props == NULL)
	{
		delete partial_xml_file;
		return false;
	}

	//Concatenation of all the signed files which always come before SignedProperties in SignedInfo element

	digest_input += canonicalNode(findDOMNodeHelper(new_dom, XADES_NAMESPACE, "SignedProperties"), new_dom);
	digest_input += canonicalNode(findDOMNodeHelper(new_dom, DSIG_NAMESPACE, "SignedInfo"), new_dom);
	digest_input += canonicalNode(findDOMNodeHelper(new_dom, DSIG_NAMESPACE, "SignatureValue"), new_dom);
	digest_input += canonicalNode(findDOMNodeHelper(new_dom, DSIG_NAMESPACE, "KeyInfo"), new_dom);

	DOMNodeList * unsigned_props = node_unsigned_props->getChildNodes();

	int i = 0;
	while(i != unsigned_props->getLength())
	{
		digest_input += canonicalNode(unsigned_props->item(i), dom);
		i++;
	}

	delete partial_xml_file;

	return appendTimestamp(dom, node_unsigned_props_orig, "ArchiveTimeStamp", digest_input);
}

bool XadesSignature::appendTimestamp(XERCES_NS DOMDocument * dom, DOMNode *parent, const char * tag_name, std::string to_timestamp)
{
	unsigned char signature_hash[SHA256_LEN];
	TSAClient tsa;
	safeBuffer str;
	std::basic_string<XMLCh> ts_id1 = generateNodeID();
	std::basic_string<XMLCh> ts_id2 = generateNodeID();


	if (strcmp(tag_name, "ArchiveTimeStamp") == 0)
	{
		unsigned int md_len;
		EVP_DigestUpdate(m_digest_state, to_timestamp.c_str(), to_timestamp.size());
		EVP_DigestFinal_ex(m_digest_state, signature_hash, &md_len);
	}
	else
	//TODO: Should be configurable
    	SHA256_Wrapper((unsigned char*)to_timestamp.c_str(), to_timestamp.size(), signature_hash);

	tsa.timestamp_data(signature_hash, SHA256_LEN);

	CByteArray raw_response = tsa.getResponse();
	//Try to get rid of the PKIStatusInfo integer from the ASN1 response!
	mp_timestamp_data = ParseTimestampTokenFromTSReply(raw_response);

	if (mp_timestamp_data.Size() == 0)
	{
		MWLOG(LEV_ERROR, MOD_APL,
			L"An error occurred in timestamp_data. It's possible that the timestamp service is down ");
		throw CMWEXCEPTION(EIDMW_TIMESTAMP_ERROR);
	}
	else
	{
		XMLCh *prefix_dsig = XMLString::transcode("");
		XMLCh * prefix_xades = XMLString::transcode("etsi");
		char *base64_str = Base64Encode(mp_timestamp_data.GetBytes(), mp_timestamp_data.Size());
		XMLCh * ts_base64 = XMLString::transcode(base64_str);

		makeQName(str, prefix_xades, tag_name);
		DOMElement * ts_node =
			dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

		//We need to specify this because otherwise the timestamp validation can go wrong with different namespace handling
		makeQName(str, prefix_dsig, "CanonicalizationMethod");
		DOMElement * c14n_node =
			dom->createElementNS(XMLString::transcode(DSIG_NAMESPACE), str.rawXMLChBuffer());
		c14n_node->setAttribute(
				XMLString::transcode("Algorithm"),
				XMLString::transcode("http://www.w3.org/2001/10/xml-exc-c14n#"));

		makeQName(str, prefix_xades, "EncapsulatedTimeStamp");
		DOMElement * ts_token_node =
			dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

		//XMLCh *ts_id = (XMLCh*)generateNodeID().c_str();
		ts_token_node->setAttribute(s_Id, (XMLCh*)ts_id1.c_str());
		ts_token_node->appendChild(dom->createTextNode(ts_base64));

		//XMLCh *ts_id2 = (XMLCh*)generateNodeID().c_str();
		ts_node->setAttribute(s_Id, (XMLCh*)ts_id2.c_str());
		ts_node->appendChild(c14n_node);

		ts_node->appendChild(ts_token_node);
		parent->appendChild(ts_node);

		free(base64_str);

		return true;
	}
}


bool XadesSignature::AddSignatureTimestamp(XERCES_NS DOMDocument *dom)
{

	DOMNode *node_signature, *node_unsigned_props;

	DOMNodeList* nodes1 = dom->getElementsByTagNameNS(XMLString::transcode(XADES_NAMESPACE),
	 XMLString::transcode("UnsignedSignatureProperties"));

	DOMNodeList *nodes2 = dom->getElementsByTagNameNS(XMLString::transcode(DSIG_NAMESPACE),
	 XMLString::transcode("SignatureValue"));

	if ((node_unsigned_props = nodes1->item(0)) == NULL)
	{
		fprintf(stderr, "Couldnt find UnsignedSignatureProperties node!\n" );
		return false;
	}
	if ((node_signature = nodes2->item(0)) == NULL)
	{
		fprintf(stderr, "Couldnt find SignatureValue node!\n" );
		return false;
	}

	std::string c14n = canonicalNode(node_signature, dom);

   return appendTimestamp(dom, node_unsigned_props, "SignatureTimeStamp", c14n);
}

void XadesSignature::setReferenceHash(XMLByte *hash, unsigned int hash_len, int ref_index, XERCES_NS DOMDocument *doc)
{
	DOMNode * node_digest_value = NULL;
	char * base64Hash = Base64Encode(hash, hash_len);

	DOMNodeList* nodes1 = doc->getElementsByTagNameNS(XMLString::transcode(DSIG_NAMESPACE),
	 XMLString::transcode("DigestValue"));

	if ((node_digest_value = nodes1->item(ref_index)) != NULL)
	{
		fprintf(stderr, "setReferenceHash: SUCCESS adding reference hash!\n");

		// Now find the correct text node to re-set

		DOMNode *tmpElt = node_digest_value;

		tmpElt = node_digest_value->getFirstChild();

		while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
			tmpElt = tmpElt->getNextSibling();

		if (tmpElt == NULL) {
			// Need to create the underlying TEXT_NODE
			//DOMDocument *doc = mp_referenceNode->getOwnerDocument();
			tmpElt = doc->createTextNode(MAKE_UNICODE_STRING((char *) base64Hash));
			node_digest_value->appendChild(tmpElt);
		}
		else {
			tmpElt->setNodeValue(MAKE_UNICODE_STRING((char *) base64Hash));
		}
	}

	free(base64Hash);
}

CByteArray &XadesSignature::Sign(const char ** paths, unsigned int n_paths)
{
	bool xerces_exception = false;
	XSECProvider prov;
	DSIGSignature *sig;
	DOMElement *sigNode;
	OpenSSLCryptoX509 *ssl_cert = NULL;
	DSIGKeyInfoX509 * keyInfoX509 = NULL;
	EVP_PKEY *pub_key = NULL;
	//XMLCh * signature_id = NULL;
	CByteArray * emptyBa = new CByteArray();

	CByteArray sha1_hash;
	CByteArray rsa_signature;

	XMLByte sha1_hash_signed_props[SHA256_LEN];
	std::basic_string<XMLCh> signature_id = generateNodeID();


	XMLByte toFill[SHA256_LEN* sizeof(XMLByte)]; //SHA-1 Hash prepended with Algorithm ID as by PKCS#1 standard
	// unsigned char signature_hash[SHA1_LEN];

        DOMImplementation *impl =
		DOMImplementationRegistry::getDOMImplementation(MAKE_UNICODE_STRING("Core"));

	//XERCES_NS XERCES_NS DOMDocument *doc = impl->createDocument(MAKE_UNICODE_STRING(ASIC_NAMESPACE), MAKE_UNICODE_STRING("asic:XAdESSignatures"), NULL);
	XERCES_NS DOMDocument *doc = impl->createDocument(NULL, MAKE_UNICODE_STRING("Document"), NULL);
	DOMElement *rootElem = doc->getDocumentElement();


	memset(toFill, 0x2f, SHA256_LEN);

	try {

		// Create a signature object

		sig = prov.newSignature();
		//sig->setDSIGNSPrefix(MAKE_UNICODE_STRING("ds"));

		// Use it to create a blank signature DOM structure from the doc
		sigNode = sig->createBlankSignature(doc, CANON_C14NE_NOC, SIGNATURE_RSA, HASH_SHA256);

		//Add Id attribute to signature
		//signature_id = (XMLCh*)generateNodeID().c_str();
		sigNode->setAttribute(s_Id, (XMLCh*)signature_id.c_str());

		// Insert the signature DOM nodes into the doc
		rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));
		rootElem->appendChild(sigNode);
		rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));

		int references_count = 0;
		for (unsigned int i = 0; i != n_paths ; i++)
		{
			const char * path = paths[i];
			//Create a reference to the external file
			DSIGReference * ref = sig->createReference(createURI(path), HASH_SHA256);
			MWLOG(LEV_DEBUG, MOD_APL, "SignXades(): Hashing file %s", path);
			sha1_hash = HashFile(path);

			//Fill the hash value as base64-encoded string
			//ref->setExternalHash(sha1_hash.GetBytes());
			setReferenceHash(sha1_hash.GetBytes(), sha1_hash.Size(), references_count, doc);
			references_count++;
		}
		CByteArray certData;
	    mp_card->readFile(PTEID_FILE_CERT_SIGNATURE, certData);
	    loadSignerCert(certData, pub_key);
	    certData.TrimRight(0);

		addSignatureProperties(sig, (XMLCh*)signature_id.c_str(), certData);

		HashSignedPropertiesNode(sig->getParentDocument(), sha1_hash_signed_props);

		DSIGReference * ref_signed_props = sig->createReference(createSignedPropertiesURI().c_str(), HASH_SHA256);
		ref_signed_props->setType(XMLString::transcode("http://uri.etsi.org/01903#SignedProperties"));

		setReferenceHash(sha1_hash_signed_props, SHA256_LEN, references_count, doc);
		//ref_signed_props->setExternalHash(sha1_hash_signed_props);
		ref_signed_props->appendCanonicalizationTransform(
			XMLString::transcode("http://www.w3.org/2001/10/xml-exc-c14n#"));

		try
		{
			// This is a somewhat hackish way of getting the canonicalized hash
			// of the reference
			HashSignedInfoNode(sig->getParentDocument(), toFill);
			//sig->calculateSignedInfoHash(toFill, SHA256_LEN);
		}
		catch (const XMLException &e)
		{
			MWLOG(LEV_ERROR, MOD_APL, "Exception in calculateSignedInfoHash(), error message: %s",
				XMLString::transcode(e.getMessage()));

		}

		keyInfoX509 = sig->appendX509Data();

		ssl_cert = new OpenSSLCryptoX509(mp_cert);

		keyInfoX509->appendX509Certificate(ssl_cert->getDEREncodingSB().sbStrToXMLCh());

		//Append to KeyInfo element all the needed CA certificates
		addCertificateChain(keyInfoX509);

		try
		{
			rsa_signature = mp_card->Sign(CByteArray(toFill, SHA256_LEN), true, true);
		}
		catch(...)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"APLCard::SignSHA256() failed, can't generate XADES signature");
			delete ssl_cert;
			throw;
		}

		//RSA Signature with length=1024/2048 bits
		addCardSignature(rsa_signature.GetBytes(), rsa_signature.Size(), doc);

		//XAdES-T level
		if (m_do_timestamping || m_do_long_term_validation)
		{
			AddSignatureTimestamp(sig->getParentDocument());
		}

		//XAdES-A level stuff
		if (m_do_long_term_validation)
		{
			AddRevocationInfo(sig->getParentDocument());

			addCompleteCertificateRefs(sig->getParentDocument());

			//Add XAdES-XL timestamp
			AddSigAndRefsTimestamp(sig->getParentDocument());

			//Add XAdES-A timestamp
			AddArchiveTimestamp(sig->getParentDocument());
		}
	}
	catch (XSECCryptoException &e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"An error occurred in XSec Crypto Functions. Message: %s\n",
		 e.getMsg());
		xerces_exception = true;

	}
	catch (XSECException &e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"An error occured during a signature load. Message: %s\n",
		 e.getMsg());
		xerces_exception = true;
        delete emptyBa;
		throw;
	}
	delete ssl_cert;

	if (xerces_exception)
		return *emptyBa;

	CByteArray * xml_output = WriteToByteArray(doc);
	doc->release();


    delete emptyBa;
	return *xml_output;
}

CByteArray &XadesSignature::SignXades(const char ** paths, unsigned int n_paths)
{
	initXMLUtils();
	CByteArray &result = Sign(paths, n_paths);
	terminateXMLUtils();

	return result;
}

}
