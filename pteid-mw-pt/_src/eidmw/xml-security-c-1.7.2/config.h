/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the <direct.h> header file. */
/* #undef HAVE_DIRECT_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Have PTHREAD_PRIO_INHERIT. */
#define HAVE_PTHREAD_PRIO_INHERIT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "xml-security-c"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "dev@santuario.apache.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "XML-Security-C"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "XML-Security-C 1.7.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "xml-security-c"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.7.2"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.7.2"

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

/* Define to 1 if OpenSSL has GCM support. */
#define XSEC_OPENSSL_HAVE_GCM 1

/* Define to 1 if OpenSSL has PKCS1_MGF1 function. */
#define XSEC_OPENSSL_HAVE_MGF1 1

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
