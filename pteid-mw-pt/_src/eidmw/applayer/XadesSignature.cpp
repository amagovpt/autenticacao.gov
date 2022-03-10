/*-****************************************************************************

 * Copyright (C) 2012-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2013 Vasco Dias - <vasco.dias@caixamagica.pt>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/*
 *  XAdES and XAdES-T signature generator and validator
 *
*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>

#include "XadesSignature.h"

#include "APLCard.h"
#include "APLConfig.h"
#include "APLCertif.h"
#include "ByteArray.h"
#include "CardPteidDef.h"
#include "CRLFetcher.h"
#include "cryptoFwkPteid.h"
#include "eidErrors.h"
#include "Log.h"
#include "MiscUtil.h"
#include "MWException.h"
#include "SigContainer.h"
#include "sign-pkcs7.h"
#include "Util.h"
#include "XercesUtils.h"

//for Timestamping
#include "TsaClient.h"

#include <zip.h>

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
#include <xsec/framework/XSECDefs.hpp>
#if _XSEC_VERSION_FULL < 20000L
#include <xsec/utils/XSECDOMUtils.hpp>
#else
#include <xsec/utils/XSECPlatformUtils.hpp>
#endif
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
#include <xsec/enc/XSECCryptoHash.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>

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

namespace eIDMW
{
#define ASIC_NAMESPACE "http://uri.etsi.org/2918/v1.2.1#"
#define XADES_NAMESPACE "http://uri.etsi.org/01903/v1.3.2#"
#define DSIG_NAMESPACE "http://www.w3.org/2000/09/xmldsig#"
#define CANON_ALGORITHM "http://www.w3.org/2001/10/xml-exc-c14n#"
#define SIGNED_PROPS_ID "S0-SignedProperties"

//Nasty Macros, beware !!
#define CREATE_DOM_NODE doc->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());
#define CREATE_DSIG_NODE doc->createElementNS(XMLString::transcode(DSIG_NAMESPACE), str.rawXMLChBuffer());

//Avoid confusion with unrelated class DOMDocument in Windows msxml header
using XERCES_CPP_NAMESPACE::DOMDocument;

//Implemented in sign-pkcs7.cpp should be moved to a common file though
unsigned int SHA256_Wrapper(unsigned char *data, unsigned long data_len, unsigned char *digest);

static XMLCh s_Id[] = {chLatin_I, chLatin_d, chNull};

static void initXMLUtils()
{
	try {
		XMLPlatformUtils::Initialize();
		XSECPlatformUtils::Initialise();
	}
	catch (const XMLException &e) {
		MWLOG(LEV_ERROR, MOD_APL, L"Error during initialisation of Xerces. Error Message: %s",
			e.getMessage());
		throw CMWEXCEPTION(EIDMW_XERCES_INIT_ERROR);
	}
}

static void terminateXMLUtils()
{
	XSECPlatformUtils::Terminate();
	XMLPlatformUtils::Terminate();
}

static CByteArray hashFileInContainer(zip_t *container, const char *filename, EVP_MD_CTX *digest_state)
{
	zip_stat_t zstat;
	if (zip_stat(container, filename, 0, &zstat) != 0) {
		MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::hashFileInContainer: zip_stat() failed");
		return CByteArray();
	}

	zip_file_t *zf;
	if ((zf = zip_fopen(container, filename, 0)) == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::hashFileInContainer: zip_fopen_index() failed");
		return CByteArray();
	}

	OpenSSL_add_all_digests();

	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);

	long long sum = 0;
	const int BUFSIZE = 4*1024;
	char buffer[BUFSIZE];
	while (sum != zstat.size) {
		zip_int64_t read = 0;
		if ((read = zip_fread(zf, buffer, BUFSIZE)) < 0) {
			MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::hashFileInContainer: zip_fread() failed");
			return CByteArray();
		}
		EVP_DigestUpdate(mdctx, buffer, read);
		if (digest_state) {
			EVP_DigestUpdate(digest_state, buffer, read);
		}
		sum += read;
	}
	zip_fclose(zf);

	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_free(mdctx);

	return CByteArray(md_value, SHA256_LEN);
}

static CByteArray hashFile(const char *filename, EVP_MD_CTX *digest_state)
{
#ifdef WIN32
	struct _stat64 sb;
	std::wstring utf16FileName = utilStringWiden(std::string(filename));
	_wstat64(utf16FileName.c_str(), &sb);
#else
	struct stat sb;
	stat(filename, &sb);
#endif

	if (sb.st_mode & S_IFDIR) {
		//it's a directory
		MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::hashFile: The path provided is a directory");
		return CByteArray();
	}

#ifdef WIN32
	FILE *fp = _wfopen(utf16FileName.c_str(), L"rb");
#else
	FILE *fp = fopen(filename, "rb");
#endif
	if (!fp) {
		MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::hashFile: Error opening file");
		return CByteArray();
	}

	OpenSSL_add_all_digests();

	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);

	const int BUFSIZE = 4*1024;
	char buffer[BUFSIZE];
	do {
		size_t read = fread(buffer, 1, BUFSIZE, fp);
		if (ferror(fp)) {
			MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::hashFile: Failed while reading file");
			return CByteArray();
		}
		EVP_DigestUpdate(mdctx, buffer, read);
		if (digest_state) {
			EVP_DigestUpdate(digest_state, buffer, read);
		}
	} while (!feof(fp));

	fclose(fp);

	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_free(mdctx);

	return CByteArray(md_value, SHA256_LEN);
}

static std::string x509GetSerialAsString(X509 *cert)
{
	std::string serial;

	BIGNUM *bn = ASN1_INTEGER_to_BN(X509_get_serialNumber(cert), 0);

	if (bn) {
		char *str = BN_bn2dec(bn);
		if (str) {
			serial = str;
			OPENSSL_free(str);
		}
		BN_free(bn);
	}
	return serial;
}

static std::string x509NameToString(X509_NAME *name)
{
	std::string str;

	BIO *mem = BIO_new(BIO_s_mem());

	// Convert the X509_NAME struct to string.
	if (X509_NAME_print_ex(mem, name, 0, XN_FLAG_RFC2253) < 0) {
		BIO_free(mem);
		MWLOG(LEV_ERROR, MOD_APL, L"x509NameToString: Failed to convert X509_NAME struct to string");
	}

	BUF_MEM *data = 0;
	BIO_get_mem_ptr(mem, &data);
	str.append(data->data, data->length);
	BIO_free(mem);

	return str;
}

static std::basic_string<XMLCh> generateNodeID()
{
	std::basic_string<XMLCh> id_buffer;
	id_buffer.append(XMLString::transcode("xades-"));

#if _XSEC_VERSION_FULL >= 20000L
	//Inline implementation of generateId() from xml-security-c 1.7
	unsigned char b[128] = {0};
	XMLCh id[258] = {0};
	unsigned int toGen = 20;

	if (XSECPlatformUtils::g_cryptoProvider->getRandom(b, toGen) != toGen) {
		throw XSECException(XSECException::CryptoProviderError,
			"generateNodeID - could not obtain enough random");
	}

	unsigned int i;
	for (i = 0; i < toGen; ++i) {
		makeHexByte(&id[i*2], b[i]);
	}

	id[1+(i*2)] = chNull;
	id_buffer.append(id);
#else
	id_buffer.append(generateId(20));
#endif
	return id_buffer;
}

static std::basic_string<XMLCh> createSignedPropertiesURI()
{
	std::basic_string<XMLCh> id_buffer;
	id_buffer.append(XMLString::transcode("#"));
	id_buffer.append(XMLString::transcode(SIGNED_PROPS_ID));

	return id_buffer;
}

static char *getUtcTime()
{
	char *date_outstr = (char *)malloc(100);
	time_t t = time(NULL);
	struct tm *tmp_date = gmtime(&t);
	//Date String for visible signature
	strftime(date_outstr, 100, "%Y-%m-%dT%H:%M:%SZ", tmp_date);

	return date_outstr;
}

static void appendCertRef(XERCES_CPP_NAMESPACE::DOMDocument *doc, APL_Certif *cert, DOMNode *parent)
{
	safeBuffer str;
	unsigned char cert_digest[SHA256_LEN];
	XMLCh *prefix = XMLString::transcode("etsi");
	XMLCh *prefix_ds = XMLString::transcode("");
	const std::string SHA1_ID = "http://www.w3.org/2000/09/xmldsig#sha1";
	const std::string SHA256_ID = "http://www.w3.org/2001/04/xmlenc#sha256";

	CByteArray certDataBa;
	cert->getFormattedData(certDataBa);
	SHA256_Wrapper(certDataBa.GetBytes(), certDataBa.Size(), cert_digest);

	makeQName(str, prefix, "Cert");
	DOMNode *n_Cert = CREATE_DOM_NODE
	makeQName(str, prefix, "CertDigest");
	DOMNode *n_CertDigest = CREATE_DOM_NODE
	makeQName(str, prefix_ds, "DigestMethod");
	DOMNode *n_CertDigestMeth = CREATE_DSIG_NODE;
	makeQName(str, prefix_ds, "DigestValue");
	DOMNode *n_CertDigestValue = CREATE_DSIG_NODE;
	makeQName(str, prefix, "IssuerSerial");
	DOMNode *n_CertIssuerSerial = CREATE_DOM_NODE;
	makeQName(str, prefix_ds, "X509IssuerName");
	DOMNode *n_CertIssuerName = CREATE_DSIG_NODE;
	makeQName(str, prefix_ds, "X509SerialNumber");
	DOMNode *n_CertSerialNumber = CREATE_DSIG_NODE;

	//Add SigningCertificate Element containing digest, serial and issuer
	const unsigned char *pcertData = certDataBa.GetBytes();
	X509 *x509Cert = d2i_X509(NULL, &pcertData, certDataBa.Size());
	if (x509Cert == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, L"appendCertRef() Error decoding certificate data");
		return;
	}

	std::string issuer = x509NameToString(X509_get_issuer_name(x509Cert));
	std::string serial = x509GetSerialAsString(x509Cert);
	X509_free(x509Cert);

	XMLCh *CertDigest = EncodeToBase64XMLCh(cert_digest, SHA256_LEN);

	((DOMElement *)n_CertDigestMeth)->setAttribute(XMLString::transcode("Algorithm"),
		XMLString::transcode(SHA256_ID.c_str()));
	n_CertDigestValue->appendChild(doc->createTextNode(CertDigest));
	n_CertSerialNumber->appendChild(doc->createTextNode(XMLString::transcode(serial.c_str())));
	n_CertIssuerName->appendChild(doc->createTextNode(XMLString::transcode(issuer.c_str())));

	parent->appendChild(n_Cert);
	n_Cert->appendChild(n_CertDigest);
	n_CertDigest->appendChild(n_CertDigestMeth);
	n_CertDigest->appendChild(n_CertDigestValue);
	n_Cert->appendChild(n_CertIssuerSerial);
	n_CertIssuerSerial->appendChild(n_CertIssuerName);
	n_CertIssuerSerial->appendChild(n_CertSerialNumber);
}

static void addSignature(unsigned char *signature, unsigned int siglen, XERCES_CPP_NAMESPACE::DOMDocument *doc)
{
	char *base64Sig = Base64Encode(signature, siglen);

	DOMNodeList *nodes = doc->getElementsByTagNameNS(XMLString::transcode(DSIG_NAMESPACE),
		XMLString::transcode("SignatureValue"));

	DOMNode *signatureValueNode = nodes->item(0);

	// Now we have the signature - place it in the DOM structures
	DOMNode *tmpElt = signatureValueNode->getFirstChild();
	while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == NULL) {
		// Need to create the underlying TEXT_NODE
		tmpElt = doc->createTextNode(XMLString::transcode((char *) base64Sig));
		signatureValueNode->appendChild(tmpElt);
	} else {
		tmpElt->setNodeValue(XMLString::transcode((char *) base64Sig));
	}

	free(base64Sig);
}

static DOMNode *addSignatureProperties(DSIGSignature *sig, XMLCh *sig_id, APL_Certif *signerCert,
	bool do_timestamping, bool do_long_term_validation)
{
	std::basic_string<XMLCh> props_target;
	char *utcTime = getUtcTime();

	props_target.append(XMLString::transcode("#"));
	props_target.append(sig_id);

	XMLCh *prefix = XMLString::transcode("etsi");
	XMLCh *prefix_ds = XMLString::transcode("");
	safeBuffer str;

	XERCES_CPP_NAMESPACE::DOMDocument *doc = sig->getParentDocument();
	DSIGObject *obj1 = sig->appendObject();

	makeQName(str, prefix, "QualifyingProperties");
	DOMNode *n1 = CREATE_DOM_NODE;

	((DOMElement *)n1)->setAttributeNS(XMLString::transcode(XADES_NAMESPACE),
		XMLString::transcode("Target"), props_target.c_str());

	makeQName(str, prefix, "SignedProperties");
	DOMNode *n2 = CREATE_DOM_NODE;
	((DOMElement *)n2)->setAttribute(s_Id, XMLString::transcode(SIGNED_PROPS_ID));
	makeQName(str, prefix, "SignedSignatureProperties");
	DOMNode *n_signSigProps = CREATE_DOM_NODE
	makeQName(str, prefix, "SigningTime");
	DOMNode *n_signingTime = CREATE_DOM_NODE
	makeQName(str, prefix, "SigningCertificate");
	DOMNode *n_signSigningCert = CREATE_DOM_NODE

	appendCertRef(doc, signerCert, n_signSigningCert);

	n_signingTime->appendChild(doc->createTextNode(XMLString::transcode(utcTime)));

	//Establish nodes structure
	n1->appendChild(n2);
	n2->appendChild(n_signSigProps);
	n_signSigProps->appendChild(n_signingTime);
	n_signSigProps->appendChild(n_signSigningCert);

	if (do_timestamping || do_long_term_validation) {
		makeQName(str, prefix, "UnsignedProperties");
		DOMNode *n3 = CREATE_DOM_NODE;

		makeQName(str, prefix, "UnsignedSignatureProperties");
		DOMNode *n4 = CREATE_DOM_NODE

		n1->appendChild(n3);
		n3->appendChild(n4);
	}

	obj1->appendChild(n1);

	free(utcTime);

	return n1;
}

static APL_Certifs *loadCerts(APL_Card *pcard, APL_Certifs *cmdCertificates)
{
	if (pcard) {
		APL_SmartCard *eid_card = static_cast<APL_SmartCard *> (pcard);
		return eid_card->getCertificates();
	}
	return cmdCertificates;
}

static XMLCh *createURI(const char *path)
{
	std::string filename = Basename((char *)path);
	std::string uri = urlEncode(filename);
	return XMLString::transcode(uri.c_str());
}

static DOMNode *findDOMNodeHelper(DOMDocument *dom, const char *ns, const char *tagname)
{
	DOMNode *node;
	DOMNodeList *nodes1 = dom->getElementsByTagNameNS(XMLString::transcode(ns),
		XMLString::transcode(tagname));

	if ((node = nodes1->item(0)) == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, L"findDOMNodeHelper(): Couldn't find %s node", tagname);
		return NULL;
	}

	return node;
}

static std::string canonicalNode(DOMNode *node, XERCES_CPP_NAMESPACE::DOMDocument *doc)
{
	XSECC14n20010315 canonicalizer(doc, node);
	canonicalizer.setCommentsProcessing(false);
	canonicalizer.setUseNamespaceStack(true);
	canonicalizer.setExclusive();

	string c14n;
	unsigned char buffer[1024];
	size_t bytes = 0;
	while ((bytes = canonicalizer.outputBuffer(buffer, 1024)) > 0) {
		c14n.append((char *)&buffer[0], size_t(bytes));
	}

	return c14n;
}

static DOMDocument *canonicalizationBugWorkaround(XERCES_CPP_NAMESPACE::DOMDocument *doc)
{
	// Parse and return a copy of the Xerces DOM tree.
	// Save to file and parse it again, to make XML Canonicalization work
	// correctly as expected by the Canonical XML 1.0 specification.
	// Hope, the next Canonical XML specification fixes the white spaces preserving "bug".

	std::unique_ptr<CByteArray> partial_xml_file(DOMDocumentToByteArray(doc));
	MemBufInputSource source((XMLByte *)partial_xml_file->GetBytes(), partial_xml_file->Size(), "temp");

	XercesDOMParser parser;
	parser.setDoNamespaces(true);
	parser.setValidationScheme(XercesDOMParser::Val_Always);
	parser.setDoSchema(true);
	parser.setCreateEntityReferenceNodes(false);
	parser.parse(source);

	return parser.adoptDocument();
}

static int hashNode(DOMDocument *doc, XMLByte *hash_buf, const char *ns, const char *tagname)
{
	DOMDocument *new_dom = canonicalizationBugWorkaround(doc);

	DOMNode *node = findDOMNodeHelper(new_dom, ns, tagname);
	if (node == NULL) {
		new_dom->release();
		return 0;
	}

	std::string c14n = canonicalNode(node, new_dom);

	//TODO: Should be configurable
	SHA256_Wrapper((unsigned char *)c14n.c_str(), c14n.size(), hash_buf);

	new_dom->release();

	return SHA256_LEN;
}

static void addCertificateToKeyInfo(const CByteArray &cert, DSIGKeyInfoX509 *keyInfo)
{
	const unsigned char *data_cert = cert.GetBytes();
	X509 *cert_ca_sign = d2i_X509(NULL, &data_cert, cert.Size());

	if (cert_ca_sign == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, L"loadCert() Error decoding certificate data (CA Sign)");
		return;
	}

	OpenSSLCryptoX509 *ssl_cert = new OpenSSLCryptoX509(cert_ca_sign);
	keyInfo->appendX509Certificate(ssl_cert->getDEREncodingSB().sbStrToXMLCh());
	delete ssl_cert;
	X509_free(cert_ca_sign);
}

static void addCertificateChain(DSIGKeyInfoX509 *keyInfo, APL_Certifs *certs)
{
	APL_Certif *certif = certs->getCert(APL_CERTIF_TYPE_SIGNATURE);
	while (!certif->isRoot()) {
		addCertificateToKeyInfo(certif->getData(), keyInfo);

		APL_Certif *issuer = NULL;
		if ((issuer = certif->getIssuer()) == NULL) {
			MWLOG(LEV_ERROR, MOD_APL, "XadesSignature: addCertificateChain() Couldn't find issuer "
				"for cert: %s", certif->getOwnerName());
			break;
		}

		MWLOG(LEV_DEBUG, MOD_APL, "XadesSignature: addCertificateChain() Loading cert: %s",
			issuer->getOwnerName());
		certif = issuer;
	}
}

static CByteArray parseTimestampTokenFromTSReply(CByteArray &ts_reply)
{
	unsigned char *tsResp = ts_reply.GetBytes();
	int tsRespLen = ts_reply.Size();

	unsigned char *outToken = NULL;
	int outTokenLen = 0;

	if (!getTokenFromTsResponse(tsResp, tsRespLen, &outToken, &outTokenLen)) {
		MWLOG(LEV_ERROR, MOD_APL, "Failed to get token from TS response");
		return CByteArray();
	}

	return CByteArray(outToken, outTokenLen);
}

static void addOCSPValue(DOMDocument *dom, DOMElement *ocsp_values_node, const CByteArray &ocsp_response)
{
	safeBuffer str;

	XMLCh *prefix_xades = XMLString::transcode("etsi");
	XMLCh *namepsace = XMLString::transcode(XADES_NAMESPACE);

	makeQName(str, prefix_xades, "EncapsulatedOCSPValue");
	DOMElement *ocsp_node = dom->createElementNS(namepsace, str.rawXMLChBuffer());

	char *base64_str = Base64Encode(ocsp_response.GetBytes(), ocsp_response.Size());
	ocsp_node->appendChild(dom->createTextNode(XMLString::transcode(base64_str)));
	ocsp_values_node->appendChild(ocsp_node);
	free(base64_str);
}

static void addCRLValue(DOMDocument *dom, DOMElement *crl_values_node, const CByteArray &crl_data)
{
	safeBuffer str;

	XMLCh *prefix_xades = XMLString::transcode("etsi");
	XMLCh *namepsace = XMLString::transcode(XADES_NAMESPACE);

	makeQName(str, prefix_xades, "EncapsulatedCRLValue");
	DOMElement *crl_node = dom->createElementNS(namepsace, str.rawXMLChBuffer());

	char *base64_str = Base64Encode(crl_data.GetBytes(), crl_data.Size());
	crl_node->appendChild(dom->createTextNode(XMLString::transcode(base64_str)));
	crl_values_node->appendChild(crl_node);
	free(base64_str);
}

/*
 * Add Revocation Info for all certificates in the chain up
 * to EC Raiz Estado which is trusted in the Portuguese TSL
 */
static bool addRevocationInfo(DOMDocument *dom, APL_Certif *signerCert)
{
	safeBuffer str;

	DOMNode *node_unsigned_props;
	XMLCh *prefix_xades = XMLString::transcode("etsi");

	DOMNodeList *nodes1 = dom->getElementsByTagNameNS(XMLString::transcode(XADES_NAMESPACE),
		XMLString::transcode("UnsignedSignatureProperties"));

	if ((node_unsigned_props = nodes1->item(0)) == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, L"addRevocationInfo(): Couldnt find UnsignedSignatureProperties node");
		return false;
	}

	makeQName(str, prefix_xades, "RevocationValues");
	DOMElement *revocation_node =
		dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

	makeQName(str, prefix_xades, "OCSPValues");
	DOMElement *ocsp_values_node =
		dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

	makeQName(str, prefix_xades, "CRLValues");
	DOMElement *crl_values_node =
		dom->createElementNS(XMLString::transcode(XADES_NAMESPACE), str.rawXMLChBuffer());

	APL_CryptoFwkPteid *cryptoFwk = AppLayer.getCryptoFwk();

	CByteArray ocsp_response = signerCert->getOCSPResponse();
	if (ocsp_response.Size() > 0) {
		CByteArray ocsp_responder_cert;
		bool no_check_extension = cryptoFwk->GetOCSPCert(ocsp_response, ocsp_responder_cert);

		addOCSPValue(dom, ocsp_values_node, ocsp_response);
		if (!no_check_extension) {
			CByteArray crl_data;
			if (cryptoFwk->GetCrlData(ocsp_responder_cert, crl_data)) {
				addCRLValue(dom, crl_values_node, crl_data);
			} else {
				MWLOG(LEV_ERROR, MOD_APL, L"addRevocationInfo(): Failed to fetch OCSP certificate CRL");
				return false;
			}
		}
	} else {
		CByteArray crl_data;
		if (cryptoFwk->GetCrlData(signerCert->getData(), crl_data)) {
			addCRLValue(dom, crl_values_node, crl_data);
		} else {
			MWLOG(LEV_ERROR, MOD_APL, L"addRevocationInfo(): Failed to fetch signer certificate CRL");
			return false;
		}
	}

	node_unsigned_props->appendChild(revocation_node);
	if (crl_values_node->getChildElementCount() > 0) {
		revocation_node->appendChild(crl_values_node);
	}
	if (ocsp_values_node->getChildElementCount() > 0) {
		revocation_node->appendChild(ocsp_values_node);
	}

	return true;
}

static bool appendTimestamp(DOMDocument *dom, DOMNode *parent, const char *tag_name,
	std::string to_timestamp, EVP_MD_CTX *digest_state)
{
	unsigned char signature_hash[SHA256_LEN];
	TSAClient tsa;
	safeBuffer str;
	std::basic_string<XMLCh> ts_id1 = generateNodeID();
	std::basic_string<XMLCh> ts_id2 = generateNodeID();

	if (strcmp(tag_name, "ArchiveTimeStamp") == 0) {
		unsigned int md_len;
		EVP_DigestUpdate(digest_state, to_timestamp.c_str(), to_timestamp.size());
		EVP_DigestFinal_ex(digest_state, signature_hash, &md_len);
	} else {
		//TODO: Should be configurable
		SHA256_Wrapper((unsigned char *)to_timestamp.c_str(), to_timestamp.size(), signature_hash);
	}

	tsa.timestamp_data(signature_hash, SHA256_LEN);

	CByteArray raw_response = tsa.getResponse();
	CByteArray timestamp_data = parseTimestampTokenFromTSReply(raw_response);

	if (timestamp_data.Size() == 0) {
		MWLOG(LEV_ERROR, MOD_APL, L"An error occurred in timestamp_data. "
			"It's possible that the timestamp service is down ");
		throw CMWEXCEPTION(EIDMW_TIMESTAMP_ERROR);
	} else {
		XMLCh *prefix_dsig = XMLString::transcode("");
		XMLCh *prefix_xades = XMLString::transcode("etsi");
		char *base64_str = Base64Encode(timestamp_data.GetBytes(), timestamp_data.Size());
		XMLCh *ts_base64 = XMLString::transcode(base64_str);

		makeQName(str, prefix_xades, tag_name);
		DOMElement *ts_node = dom->createElementNS(XMLString::transcode(XADES_NAMESPACE),
			str.rawXMLChBuffer());

		//We need to specify this because otherwise the timestamp validation can go wrong with different namespace handling
		makeQName(str, prefix_dsig, "CanonicalizationMethod");
		DOMElement *c14n_node = dom->createElementNS(XMLString::transcode(DSIG_NAMESPACE),
			str.rawXMLChBuffer());

		c14n_node->setAttribute(XMLString::transcode("Algorithm"), XMLString::transcode(CANON_ALGORITHM));

		makeQName(str, prefix_xades, "EncapsulatedTimeStamp");
		DOMElement *ts_token_node = dom->createElementNS(XMLString::transcode(XADES_NAMESPACE),
			str.rawXMLChBuffer());

		//XMLCh *ts_id = (XMLCh *)generateNodeID().c_str();
		ts_token_node->setAttribute(s_Id, (XMLCh *)ts_id1.c_str());
		ts_token_node->appendChild(dom->createTextNode(ts_base64));

		//XMLCh *ts_id2 = (XMLCh *)generateNodeID().c_str();
		ts_node->setAttribute(s_Id, (XMLCh *)ts_id2.c_str());
		ts_node->appendChild(c14n_node);

		ts_node->appendChild(ts_token_node);
		parent->appendChild(ts_node);

		free(base64_str);

		return true;
	}
}

/*
 * Mandatory Property for a XAdES-A signature form
 * The algorithm to calculate the TS Input data is described in page 58 of ETSI TS 101 903
 * [ds:References+, ds:SignedInfo, ds:SignatureValue, ds:KeyInfo, xades:UnsignedSignatureProperties+]
 */
static bool addArchiveTimestamp(DOMDocument *dom, EVP_MD_CTX *digest_state)
{
	DOMDocument *new_dom = canonicalizationBugWorkaround(dom);

	DOMNode *node_unsigned_props = findDOMNodeHelper(new_dom, XADES_NAMESPACE, "UnsignedSignatureProperties");
	if (node_unsigned_props == NULL) {
		new_dom->release();
		return false;
	}

	//Concatenation of all the signed files which always come before SignedProperties in SignedInfo element
	std::string digest_input;
	digest_input += canonicalNode(findDOMNodeHelper(new_dom, XADES_NAMESPACE, "SignedProperties"), new_dom);
	digest_input += canonicalNode(findDOMNodeHelper(new_dom, DSIG_NAMESPACE, "SignedInfo"), new_dom);
	digest_input += canonicalNode(findDOMNodeHelper(new_dom, DSIG_NAMESPACE, "SignatureValue"), new_dom);
	digest_input += canonicalNode(findDOMNodeHelper(new_dom, DSIG_NAMESPACE, "KeyInfo"), new_dom);

	DOMNodeList *unsigned_props = node_unsigned_props->getChildNodes();
	for (size_t i = 0; i < unsigned_props->getLength(); ++i) {
		digest_input += canonicalNode(unsigned_props->item(i), new_dom);
	}

	new_dom->release();

	DOMNode *node_unsigned_props_orig = findDOMNodeHelper(dom, XADES_NAMESPACE, "UnsignedSignatureProperties");
	return appendTimestamp(dom, node_unsigned_props_orig, "ArchiveTimeStamp", digest_input, digest_state);
}

static bool addSignatureTimestamp(DOMDocument *dom)
{
	DOMNode *node_unsigned_props = findDOMNodeHelper(dom, XADES_NAMESPACE, "UnsignedSignatureProperties");
	DOMNode *node_signature = findDOMNodeHelper(dom, DSIG_NAMESPACE, "SignatureValue");

	if (node_unsigned_props == NULL || node_signature == NULL) {
		return false;
	}

	std::string c14n = canonicalNode(node_signature, dom);
	return appendTimestamp(dom, node_unsigned_props, "SignatureTimeStamp", c14n, NULL);
}

static void setReferenceHash(XMLByte *hash, unsigned int hash_len, int ref_index, DOMDocument *doc)
{
	DOMNode *node_digest_value = NULL;
	char *base64Hash = Base64Encode(hash, hash_len);

	DOMNodeList *nodes1 = doc->getElementsByTagNameNS(XMLString::transcode(DSIG_NAMESPACE),
		XMLString::transcode("DigestValue"));

	if ((node_digest_value = nodes1->item(ref_index)) != NULL) {
		// Now find the correct text node to re-set
		DOMNode *tmpElt = node_digest_value->getFirstChild();
		while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE) {
			tmpElt = tmpElt->getNextSibling();
		}

		if (tmpElt == NULL) {
			// Need to create the underlying TEXT_NODE
			tmpElt = doc->createTextNode(XMLString::transcode((char *) base64Hash));
			node_digest_value->appendChild(tmpElt);
		} else {
			tmpElt->setNodeValue(XMLString::transcode((char *)base64Hash));
		}
	}

	free(base64Hash);
}

CByteArray &XadesSignature::sign(const char** paths, unsigned int pathCount, zip_t *container)
{
	XSECProvider prov;
	DSIGSignature *sig;

	EVP_MD_CTX *digest_state = NULL;
	if (m_doLTV) {
		digest_state = EVP_MD_CTX_new();
		EVP_DigestInit_ex(digest_state, EVP_sha256(), NULL);
	}

	std::basic_string<XMLCh> signature_id = generateNodeID();

	DOMImplementation *impl =
		DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Core"));

	DOMDocument *doc = impl->createDocument(XMLString::transcode(ASIC_NAMESPACE),
		XMLString::transcode("XAdESSignatures"), NULL);

	try {
		// Create a signature object
		sig = prov.newSignature();

		// Use it to create a blank signature DOM structure from the doc
		DOMElement *sigNode = sig->createBlankSignature(doc,
			DSIGConstants::s_unicodeStrURIEXC_C14N_NOC, DSIGConstants::s_unicodeStrURIRSA_SHA256);

		//Add Id attribute to signature
		sigNode->setAttribute(s_Id, (XMLCh *)signature_id.c_str());

		// Insert the signature DOM nodes into the doc
		doc->getDocumentElement()->appendChild(sigNode);

		std::vector<std::string *> unique_paths;
		for (unsigned int i = 0; i != pathCount; i++) {
			unique_paths.push_back(new std::string(paths[i]));
		}

		CPathUtil::generate_unique_filenames("temporary_folder_name", unique_paths);

		int references_count = 0;
		for (unsigned int i = 0; i != pathCount ; i++) {
			const char *path = unique_paths[i]->c_str();
			//Create a reference to the external file
			DSIGReference *ref = sig->createReference(createURI(path),
				DSIGConstants::s_unicodeStrURISHA256);

			MWLOG(LEV_DEBUG, MOD_APL, "SignXades(): Hashing file %s", path);

			CByteArray fileHash;
			if (container) {
				fileHash = hashFileInContainer(container, paths[i], digest_state);
			} else {
				fileHash = hashFile(paths[i], digest_state);
			}

			if (fileHash.Size() == 0) {
				throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
			}
			setReferenceHash(fileHash.GetBytes(), fileHash.Size(), references_count++, doc);

			delete unique_paths[i];
		}

		APL_Certifs *certs = loadCerts(m_pcard, m_cmdCertificates);
		APL_Certif *signerCert;
		if (certs == NULL || (signerCert = certs->getCert(APL_CERTIF_TYPE_SIGNATURE)) == NULL) {
			MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::sign(): Failed to load certificates.");
			throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
		}
		addSignatureProperties(sig, (XMLCh *)signature_id.c_str(), signerCert, m_doTimestamp, m_doLTV);

		//Append to KeyInfo element all the needed CA certificates
		DSIGKeyInfoX509 *keyInfoX509 = sig->appendX509Data();
		addCertificateChain(keyInfoX509, certs);

		DSIGReference *ref_signed_props = sig->createReference(createSignedPropertiesURI().c_str(),
			DSIGConstants::s_unicodeStrURISHA256);
		ref_signed_props->setType(XMLString::transcode("http://uri.etsi.org/01903#SignedProperties"));

		XMLByte signedPropertiesHash[SHA256_LEN];
		hashNode(sig->getParentDocument(), signedPropertiesHash, XADES_NAMESPACE, "SignedProperties");
		setReferenceHash(signedPropertiesHash, SHA256_LEN, references_count, doc);
		ref_signed_props->appendCanonicalizationTransform(XMLString::transcode(CANON_ALGORITHM));

		XMLByte bytesToSign[SHA256_LEN] = {0x2f};
		try {
			// This is a somewhat hackish way of getting the canonicalized hash of the reference
			hashNode(sig->getParentDocument(), bytesToSign, DSIG_NAMESPACE, "SignedInfo");
		}
		catch (const XMLException &e) {
			MWLOG(LEV_ERROR, MOD_APL, "Exception in calculateSignedInfoHash(), error message: %s",
				XMLString::transcode(e.getMessage()));
		}

		CByteArray rsa_signature;
		try {
			if (m_pcard) {
				rsa_signature = m_pcard->Sign(CByteArray(bytesToSign, SHA256_LEN), true, true);
			} else {
				rsa_signature = m_signCallback(CByteArray(bytesToSign, SHA256_LEN));
			}
		}
		catch(...) {
			MWLOG(LEV_ERROR, MOD_APL, L"APLCard::SignSHA256() failed, can't generate XADES signature");
			throw;
		}

		addSignature(rsa_signature.GetBytes(), rsa_signature.Size(), doc);

		//XAdES-T level
		if (m_doTimestamp || m_doLTV) {
			try {
				addSignatureTimestamp(sig->getParentDocument());
			}
			catch (CMWException &e) {
				MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::Sign: Failed to add timestamp. "
					"Error code: %08x", e.GetError());

				if (e.GetError() != EIDMW_TIMESTAMP_ERROR) {
					throw e;
				}
				m_throwTimestampException = true;
			}
		}

		//XAdES-LTA level stuff
		if (m_doLTV && !m_throwTimestampException) {
			if (!addRevocationInfo(sig->getParentDocument(), signerCert)) {
				m_throwLTVException = true;
			}

			try {
				//Add XAdES-LTA timestamp
				addArchiveTimestamp(sig->getParentDocument(), digest_state);
			}
			catch (CMWException &e) {
				MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::Sign: Failed to add archive timestamp. "
					"Error code: %08x", e.GetError());

				if (e.GetError() != EIDMW_TIMESTAMP_ERROR) {
					EVP_MD_CTX_free(digest_state);
					throw e;
				}
				m_throwLTVException = true;
			}
		}
	}
	catch (XSECCryptoException &e) {
		MWLOG(LEV_ERROR, MOD_APL, L"An error occurred in XSec Crypto Functions. Message: %s\n",
			e.getMsg());

		if (digest_state) {
			EVP_MD_CTX_free(digest_state);
		}
		return *(new CByteArray());
	}
	catch (XSECException &e) {
		MWLOG(LEV_ERROR, MOD_APL, L"An error occured during a signature load. Message: %s\n",
			e.getMsg());

		if (digest_state) {
			EVP_MD_CTX_free(digest_state);
		}
		throw;
	}

	if (digest_state) {
		EVP_MD_CTX_free(digest_state);
	}

	CByteArray *xml_output = DOMDocumentToByteArray(doc);
	doc->release();

	return *xml_output;
}

CByteArray &XadesSignature::signXades(const char **paths, unsigned int pathCount)
{
	initXMLUtils();
	CByteArray &result = sign(paths, pathCount);
	terminateXMLUtils();

	return result;
}

void XadesSignature::signASiC(const char *path)
{
	int status = 0;
	long error_code = 0;
	std::vector<const char *> paths;
	zip_t *container = NULL;
	SigContainer asic(path);
	
	std::vector<std::string> paths_str = asic.listInputFiles();
	if (paths_str.size() == 0) {
		MWLOG(LEV_ERROR, MOD_APL, "signASiC(): no input files found in ASiC container");
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}
	
	for (const std::string &s: paths_str) {
		paths.push_back(s.c_str());
	}

	if ((container = zip_open(path, ZIP_CHECKCONS | ZIP_RDONLY, &status)) == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "zip_open() failed with error code: %d", status);
		throw CMWEXCEPTION(EIDMW_PERMISSION_DENIED);
	}

	initXMLUtils();
	CByteArray &sigXml = sign(&paths[0], paths.size(), container);
	terminateXMLUtils();

	const char *uniqueFileName = NULL;
	if ((uniqueFileName = asic.getNextSignatureFileName()) == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "signASiC(): failed to generate new filename for signature xml");
		free(container);
		throw CMWEXCEPTION(EIDMW_XADES_UNKNOWN_ERROR);
	}

	if (zip_close(container) < 0) {
		MWLOG(LEV_ERROR, MOD_APL, "signASiC(): zip_close() failed with error msg: %s",
			zip_error_strerror(zip_get_error(container)));
		free(container);
		free((char *)uniqueFileName);
		throw CMWEXCEPTION(EIDMW_PERMISSION_DENIED);
	}

	asic.addSignature(uniqueFileName, sigXml);

}

}
