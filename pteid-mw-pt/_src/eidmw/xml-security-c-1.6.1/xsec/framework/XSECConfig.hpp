/* xsec/framework/XSECConfig.hpp.  Generated from XSECConfig.hpp.in by configure.  */
/* xsec/framework/XSECConfig.hpp.in.  Generated from configure.ac by autoheader.  */

/* Define if NSS is in use. */
/* #undef XSEC_HAVE_NSS */

/* Define if OpenSSL is in use. */
#define XSEC_HAVE_OPENSSL 1

/* Define to 1 if strcasecmp present. */
#define XSEC_HAVE_STRCASECMP 1

/* Define to 1 if Xalan is unavailable. */
#define XSEC_NO_XALAN 1

/* Define to 1 if OpenSSL has EVP_CIPHER_CTX_set_padding. */
#define XSEC_OPENSSL_CANSET_PADDING 1

/* Define to 1 if OpenSSL uses const input buffers. */
#define XSEC_OPENSSL_CONST_BUFFERS 1

/* Define to 1 if OpenSSL X509 API has const input buffer. */
#define XSEC_OPENSSL_D2IX509_CONST_BUFFER 1

/* Define to 1 if OpenSSL has full AES support. */
#define XSEC_OPENSSL_HAVE_AES 1

/* Define to 1 if OpenSSL has CRYPTO_cleanup_all_ex_data. */
#define XSEC_OPENSSL_HAVE_CRYPTO_CLEANUP_ALL_EX_DATA 1

/* Define to 1 if OpenSSL has EC support. */
#define XSEC_OPENSSL_HAVE_EC 1

/* Define to 1 if OpenSSL has SHA2 support. */
#define XSEC_OPENSSL_HAVE_SHA2 1

/* Define to 1 if Xalan XPathEvaluator requires NodeRefList. */
/* #undef XSEC_SELECTNODELIST_REQS_NODEREFLIST */

/* Define to 1 if Xalan requires MemoryManager */
/* #undef XSEC_XALAN_REQS_MEMORYMANAGER */

/* Define to 1 if Xalan XercesParserLiaison ctor takes XercesDOMSupport. */
/* #undef XSEC_XERCESPARSERLIAISON_REQS_DOMSUPPORT */

/* Define to 1 if Xerces has a 64-bit-safe API. */
#define XSEC_XERCES_64BITSAFE 1

/* Define to 1 if Xerces DOMEntity has getInputEncoding. */
#define XSEC_XERCES_DOMENTITYINPUTENCODING 1

/* Define to 1 if Xerces has DOMLSSerializer. */
#define XSEC_XERCES_DOMLSSERIALIZER 1

/* Define to 1 if Xerces XMLFormatter requires version. */
#define XSEC_XERCES_FORMATTER_REQUIRES_VERSION 1

/* Define to 1 if Xerces DOM ID methods take extra parameter. */
#define XSEC_XERCES_HAS_BOOLSETIDATTRIBUTE 1

/* Define to 1 if Xerces has legacy setIdAttribute. */
/* #undef XSEC_XERCES_HAS_SETIDATTRIBUTE */

/* Define to 1 if Xerces InputStream class requires getContentType. */
#define XSEC_XERCES_INPUTSTREAM_HAS_CONTENTTYPE 1

/* Define to 1 if Xerces XMLFormatter requires MemoryManager. */
#define XSEC_XERCES_REQUIRES_MEMMGR 1

/* Define to 1 if Xerces XMLString has release method. */
#define XSEC_XERCES_XMLSTRING_HAS_RELEASE 1

/* Define to 1 if Xalan XSLException returns XalanDOMString. */
/* #undef XSEC_XSLEXCEPTION_RETURNS_DOMSTRING */

#include <xsec/framework/XSECVersion.hpp>
