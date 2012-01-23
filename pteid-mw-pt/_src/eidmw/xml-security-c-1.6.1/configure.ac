# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.

# Process this file with autoreconf
AC_PREREQ(2.50)
AC_INIT([[XML-Security-C]],[1.6.1],[santuario-dev@apache.org],[xml-security-c])
AC_CONFIG_SRCDIR(xsec)
AC_CONFIG_AUX_DIR(build-aux)
AC_CONFIG_MACRO_DIR(m4)
AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL

AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],[Have GCC compile with symbols (Default = no)]),
    enable_debug=$enableval, enable_debug=no)

if test "$enable_debug" = "yes" ; then
    GCC_CFLAGS="$CFLAGS -g -D_DEBUG"
    GCC_CXXFLAGS="$CXXFLAGS -g -D_DEBUG"
else
    GCC_CFLAGS="$CFLAGS -O2 -DNDEBUG"
    GCC_CXXFLAGS="$CXXFLAGS -O2 -DNDEBUG"
fi


# Define the files we wish to generate

AC_CONFIG_FILES([Makefile xsec/Makefile])
AC_CONFIG_HEADERS([config.h xsec/framework/XSECConfig.hpp])
AH_BOTTOM([#include <xsec/framework/XSECVersion.hpp>])

# Check for basic programs

AC_PROG_CC([gcc gcc3 cc])
AC_PROG_CXX([g++ g++3 c++ CC])
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_CHECK_PROG(AUTOCONF, autoconf, autoconf, true)

if test "$GCC" = "yes" ; then
    CFLAGS="-Wall $GCC_CFLAGS"
    CXXFLAGS="-Wall $GCC_CXXFLAGS"
fi

# pthreads
ACX_PTHREAD
CFLAGS="${CFLAGS} ${PTHREAD_CFLAGS}"
CXXFLAGS="${CXXFLAGS} ${PTHREAD_CFLAGS}"
LIBS="${LIBS} ${PTHREAD_LIBS}"

# Checks for required libraries
AC_CHECK_LIB(m, floor)
AC_CHECK_LIB(socket, getservent)

# Check for required includes
AC_CHECK_HEADERS(unistd.h direct.h)

AC_CHECK_DECL(strcasecmp,[AC_DEFINE([XSEC_HAVE_STRCASECMP],[1],[Define to 1 if strcasecmp present.])],,[#include <string.h>]) 

AC_LANG(C++)

# Xerces is required

AC_ARG_WITH(xerces,AS_HELP_STRING([--with-xerces=PATH], [where xerces-c is installed]),
    [if test x_$with_xerces != x_/usr; then
        LDFLAGS="-L${with_xerces}/lib $LDFLAGS"
        CPPFLAGS="-I${with_xerces}/include $CPPFLAGS"
    fi])
LIBS="-lxerces-c $LIBS"

AC_CHECK_HEADER([xercesc/dom/DOM.hpp],,AC_MSG_ERROR([unable to find xerces header files]))
AC_MSG_CHECKING([Xerces version])
AC_PREPROC_IFELSE(
    [AC_LANG_PROGRAM([#include <xercesc/util/XercesVersion.hpp>],
[#if  _XERCES_VERSION >= 20000
int i = 0;
#else
#error cannot use version 1.x
#endif])],
    [AC_MSG_RESULT(OK)],
    [AC_MSG_FAILURE([Xerces-C 2.x or 3.x is required])])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/util/PlatformUtils.hpp>]], [[xercesc::XMLPlatformUtils::Initialize()]])],
    [],[AC_MSG_ERROR([unable to link with Xerces])])

# Get user options

AC_ARG_WITH(xalan, 
    AS_HELP_STRING([--with-xalan],[Enable Xalan integration.  Values = 'yes' or installation directory (Default = yes)]), 
    use_xalan=$withval, use_xalan=yes)

if test x"$use_xalan" != "xyes" ; then
  if test x"$use_xalan" != "xno" ; then
    XALANCROOT=$use_xalan
  fi
fi

# Ensure we are always compiling using library defs

CFLAGS="${CFLAGS} -DXSEC_LIBRARY_BUILD"
CXXFLAGS="${CXXFLAGS} -DXSEC_LIBRARY_BUILD"

# Find out some properties of the version of Xerces we have

AC_MSG_CHECKING([whether Xerces is 64-bit clean])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/framework/MemBufInputSource.hpp>]], [[using namespace XERCES_CPP_NAMESPACE;
      XMLFilePos testvar;
    ]])],[AC_MSG_RESULT([yes])
    AC_DEFINE([XSEC_XERCES_64BITSAFE],[1],[Define to 1 if Xerces has a 64-bit-safe API.])],
    [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether Xerces BinInputStream requires getContentType])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/util/BinMemInputStream.hpp>]], [[using namespace XERCES_CPP_NAMESPACE;
      XMLByte buf[1024];
      BinMemInputStream in(buf,1024);
      in.getContentType();
    ]])],[AC_MSG_RESULT([yes])
    AC_DEFINE([XSEC_XERCES_INPUTSTREAM_HAS_CONTENTTYPE],[1],[Define to 1 if Xerces InputStream class requires getContentType.])],
    [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether Xerces XMLFormatter requires a version])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/framework/XMLFormatter.hpp>
	#include <xercesc/util/XercesDefs.hpp>
	#include <xercesc/util/XMLUniDefs.hpp>]], [[using namespace XERCES_CPP_NAMESPACE;
	  XMLCh s_encoding[] = {
		chLatin_U, chLatin_T, chLatin_F, chDash, chDigit_8, chNull
	  };
	  XMLFormatter f(s_encoding,
		NULL,
		NULL, 
		XMLFormatter::NoEscapes, 
		XMLFormatter::UnRep_CharRef);
	]])],[AC_MSG_RESULT([yes])
	AC_DEFINE([XSEC_XERCES_REQUIRES_MEMMGR],[1],[Define to 1 if Xerces XMLFormatter requires MemoryManager.])
	AC_DEFINE([XSEC_XERCES_FORMATTER_REQUIRES_VERSION],[1],[Define to 1 if Xerces XMLFormatter requires version.])],
	[AC_MSG_RESULT([no])])
	
AC_MSG_CHECKING([whether Xerces XMLString has ::release])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/util/XMLString.hpp>]], [[using namespace XERCES_CPP_NAMESPACE;
		XMLCh * tst;
		XMLString::release(&tst);
	]])],[AC_MSG_RESULT([yes])
	AC_DEFINE([XSEC_XERCES_XMLSTRING_HAS_RELEASE],[1],[Define to 1 if Xerces XMLString has release method.])],
	[AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether Xerces XMLElement has ::setIdAttribute(XMLCh*)])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/dom/DOM.hpp>]], [[using namespace XERCES_CPP_NAMESPACE;
		DOMElement * elt;
		elt->setIdAttribute(NULL);
	]])],[AC_MSG_RESULT([yes])
    xerces_has_setidattribute=yes
	AC_DEFINE([XSEC_XERCES_HAS_SETIDATTRIBUTE],[1],[Define to 1 if Xerces has legacy setIdAttribute.])],
	[AC_MSG_RESULT([no])])

if test -z "$xerces_has_setidattribute" ; then
  AC_MSG_CHECKING([whether Xerces XMLElement has ::setIdAttribute(XMLCh*, bool)])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/dom/DOM.hpp>]], [[using namespace XERCES_CPP_NAMESPACE;
		  DOMElement * elt;
		  elt->setIdAttribute(NULL, false);
	  ]])],[AC_MSG_RESULT([yes])
      xerces_has_setidattribute=yes
	  AC_DEFINE([XSEC_XERCES_HAS_BOOLSETIDATTRIBUTE],[1],[Define to 1 if Xerces DOM ID methods take extra parameter.])],
	  [AC_MSG_RESULT([no])])
fi

# For Xerces 3.x we now have a stricter DOM L3 implementation
AC_MSG_CHECKING([whether Xerces DOMImplementationLS has DOMLSSerializer])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/dom/DOM.hpp>]], [[using namespace XERCES_CPP_NAMESPACE;
        DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(NULL);
        DOMLSSerializer *ls = ((DOMImplementationLS*)impl)->createLSSerializer();
    ]])],[AC_MSG_RESULT([yes])
    AC_DEFINE([XSEC_XERCES_DOMLSSERIALIZER],[1],[Define to 1 if Xerces has DOMLSSerializer.])],
    [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether Xerces DOMEntity uses getInputEncoding()])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xercesc/dom/DOM.hpp>]], [[using namespace XERCES_CPP_NAMESPACE;
        DOMEntity *t;
        t->getInputEncoding();
    ]])],[AC_MSG_RESULT([yes])
    AC_DEFINE([XSEC_XERCES_DOMENTITYINPUTENCODING],[1],[Define to 1 if Xerces DOMEntity has getInputEncoding.])],
    [AC_MSG_RESULT([no])])


# Now check for Xalan

if test x"$use_xalan" != x"no" ; then

  if test $XALANCROOT; then

    AC_MSG_CHECKING([for Xalan headers in XALANCROOT])

    OLD_CPPFLAGS=$CPPFLAGS
    # Updated to include nls/include as this is generally needed for
    # compilation against non-installed xalan.
    # Also now include XALANCROOT/include to cater for installed xalan
    CPPFLAGS=["-I${XALANCROOT}/src -I${XALANCROOT}/include -I${XALANCROOT}/nls/include ${CPPFLAGS}"]

    AC_PREPROC_IFELSE([AC_LANG_SOURCE([[#include <xalanc/Include/XalanVersion.hpp>]])],[xalan_found=yes 
      LIBS="-L${XALANCROOT}/lib -lxalan-c ${LIBS}"
      AC_MSG_RESULT([found])],[CPPFLAGS=$OLD_CPPFLAGS
      AC_MSG_RESULT([no])]);

  fi

  if test -z "$xalan_found" ; then

    AC_MSG_CHECKING([for Xalan in system includes])
    AC_PREPROC_IFELSE([AC_LANG_SOURCE([[#include <xalanc/Include/XalanVersion.hpp>]])],[xalan_found=yes 
      LIBS="${LIBS} -lxalan-c"
      AC_MSG_RESULT([found])],
      [AC_DEFINE([XSEC_NO_XALAN],[1],[Define to 1 if Xalan is unavailable.]) 
      AC_MSG_RESULT([no - WARNING - configuring for no Xalan])
      ]);

  fi

else

  # NO_XALAN is defined - so we skip

  AC_MSG_NOTICE([Warning - Xalan being configured out - XPath and XSLT will not be available])
  AC_DEFINE([XSEC_NO_XALAN],[1],[Define to 1 if Xalan is unavailable.])

fi

if test "${xalan_found}" = "yes" ; then
  # Do we need xalanMsg.so?
  AC_MSG_CHECKING([if libxalanMsg is required])
  old_libs=$LIBS
  LIBS="${LIBS} -lxalanMsg"
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[
    int test=1;
  ]])],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)
  LIBS=${old_libs}]);
fi

# Check for handling of XSLException types
if test "$xalan_found" = "yes" ; then

  AC_MSG_CHECKING([whether XSLException::getType() returns XalanDOMString])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xalanc/PlatformSupport/XSLException.hpp>]], [[ try {
          int x = 1;
      } 
      catch (XALAN_CPP_NAMESPACE_QUALIFIER XSLException &e) {
        e.getType().c_str();
      }
    ]])],[AC_MSG_RESULT([yes])
    AC_DEFINE([XSEC_XSLEXCEPTION_RETURNS_DOMSTRING],[1],[Define to 1 if Xalan XSLException returns XalanDOMString.])],
    [AC_MSG_RESULT([no])]);

  AC_MSG_CHECKING([whether XercesParserLiaison ctor takes XercesDOMSupport])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
    #include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>]], [[ 	XALAN_CPP_NAMESPACE_QUALIFIER XercesDOMSupport 
           theDOMSupport;
	    XALAN_CPP_NAMESPACE_QUALIFIER XercesParserLiaison 
           theParserLiaison(theDOMSupport);
	]])],[AC_MSG_RESULT([yes])
	AC_DEFINE([XSEC_XERCESPARSERLIAISON_REQS_DOMSUPPORT],[1],[Define to 1 if Xalan XercesParserLiaison ctor takes XercesDOMSupport.])],
	[AC_MSG_RESULT([no])]);

  AC_MSG_CHECKING([whether XPathEvaluator::selectNodeList requires NodeRefList])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
	#include <xalanc/XPath/XPathEvaluator.hpp>
	#include <xalanc/XPath/NodeRefList.hpp>
    #include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>]], [[ 	XALAN_CPP_NAMESPACE_QUALIFIER XercesDOMSupport 
           theDOMSupport;
		XALAN_CPP_NAMESPACE_QUALIFIER XPathEvaluator	theEvaluator;
		XALAN_CPP_NAMESPACE_QUALIFIER NodeRefList	theResult(
			theEvaluator.selectNodeList(
			theDOMSupport,
			NULL,
			NULL,
			NULL));
	]])],[AC_MSG_RESULT([no])],
	[AC_MSG_RESULT([yes])
	AC_DEFINE([XSEC_SELECTNODELIST_REQS_NODEREFLIST],[1],[Define to 1 if Xalan XPathEvaluator requires NodeRefList.])]);

  AC_MSG_CHECKING([whether Xalan classes require MemoryManager])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
 	#include <xalanc/XPath/XPathDefinitions.hpp>
    #include <xalanc/XPath/Function.hpp>]], [[
    }
	XALAN_CPP_NAMESPACE_BEGIN

	class FunctionTest : public Function
	{
		public:
 	       typedef Function        ParentType;

    	    FunctionTest();

        	virtual ~FunctionTest();      
	        virtual XObjectPtr
    	    execute(
        	   XPathExecutionContext& executionContext,
               XalanNode* context,
               const XObjectArgVectorType& args,
               const LocatorType* locator) const;

            XObjectPtr
            execute(
                XPathExecutionContext&          executionContext,
                XalanNode*                                      context,
                const XObjectPtr                        arg1,
                const XObjectPtr                        arg2,
                const LocatorType*                      locator) const;

            XObjectPtr
            execute(
                XPathExecutionContext&          executionContext,
                XalanNode*                                      context,
                   
                const XObjectPtr                        arg1,
                const XObjectPtr                        arg2,
                const XObjectPtr                        arg3,
                const LocatorType*                      locator) const;

	#if !defined(XALAN_NO_USING_DECLARATION)
        using ParentType::execute;
	#endif

	#if defined(XALAN_NO_COVARIANT_RETURN_TYPE)
        virtual Function*
	#else
        virtual FunctionTest*
	#endif
        clone() const;

	protected:

        virtual const XalanDOMString
        getError() const;

    private:

        // Not implemented...
        FunctionTest&
        operator=(const FunctionTest&);

        bool
        operator==(const FunctionTest&) const;
	};

  void main2(void){

	FunctionTest t;

  }

XALAN_CPP_NAMESPACE_END
  int main3(void) {

	]])],[AC_MSG_RESULT([no])],
	[AC_MSG_RESULT([yes])
	AC_DEFINE([XSEC_XALAN_REQS_MEMORYMANAGER],[1],[Define to 1 if Xalan requires MemoryManager])]);
fi

# Crypto provider options (OpenSSL / NSS)

AC_ARG_WITH(openssl,
    AS_HELP_STRING([--with-openssl],[Use Openssl.  Values = 'yes' or installation directory (Default = yes)]),
    use_openssl=$withval, use_openssl=yes)

if test x"$use_openssl" != "xno" ; then

    # Check for explicit location or try pkg-config
    if test x"$use_openssl" = "xyes" ; then
        AC_PATH_PROG(PKG_CONFIG, pkg-config)
        if test "x$PKG_CONFIG" != x && test "x$PKG_CONFIG" != "xno" ; then
            if pkg-config openssl ; then
                SSLLIBS="`$PKG_CONFIG --libs openssl`"
                SSLFLAGS="`$PKG_CONFIG --cflags openssl`"
            else
                AC_MSG_WARN([OpenSSL not supported by pkg-config, try --with-openssl=PATH instead])
            fi
        fi
    else
        if test x_$use_openssl != x_/usr; then
            SSLFLAGS="-I${use_openssl}/include"
            SSLLIBS="-L${use_openssl}/lib -lcrypto -lssl"
        else
            SSLLIBS="-lcrypto -lssl"
        fi
    fi
    
    AC_MSG_CHECKING(for OpenSSL cflags)
    AC_MSG_RESULT($SSLFLAGS)
    CPPFLAGS="$CPPFLAGS $SSLFLAGS"
    
    AC_PREPROC_IFELSE([AC_LANG_SOURCE([[#include <openssl/opensslv.h>]])],
        [AC_DEFINE([XSEC_HAVE_OPENSSL],[1],[Define if OpenSSL is in use.])],
        [AC_MSG_ERROR([Unable to find OpenSSL headers])]);
    
    AC_MSG_CHECKING(for OpenSSL libraries)
    AC_MSG_RESULT($SSLLIBS)
    LIBS="$LIBS $SSLLIBS"
    
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <openssl/evp.h>]], [[EVP_EncryptInit(NULL, NULL, NULL, NULL) ; return 0; ]])],,
        [AC_MSG_ERROR(["Unable to find libcrypto"])])
    
    # Now try to find out some things about this version of OpenSSL
    
    AC_MSG_CHECKING([for const input buffers in OpenSSL])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <openssl/rsa.h>]], [[ const unsigned char * buf;
    	  unsigned char * outbuf;
    	  RSA rsa;
    	  RSA_private_decrypt(1,buf,outbuf,&rsa,RSA_PKCS1_PADDING);]])],
    	[AC_MSG_RESULT([yes])
    	AC_DEFINE([XSEC_OPENSSL_CONST_BUFFERS],[1],[Define to 1 if OpenSSL uses const input buffers.])],
    	[AC_MSG_RESULT([no])])
    
    AC_MSG_CHECKING([for non-broken AES support])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <openssl/evp.h>]], [[ EVP_aes_256_cbc();
    	]])],[AC_MSG_RESULT([yes])
    	AC_DEFINE([XSEC_OPENSSL_HAVE_AES],[1],[Define to 1 if OpenSSL has full AES support.])],
    	[AC_MSG_RESULT([no])])

    AC_CHECK_DECL(PKCS1_MGF1,
        [AC_DEFINE([XSEC_OPENSSL_HAVE_MGF1],[1],[Define to 1 if OpenSSL has PKCS1_MGF1 function.])],
        ,[#include <openssl/rsa.h>])
    
    AC_CHECK_DECL(EVP_PKEY_set1_EC_KEY,
        [AC_DEFINE([XSEC_OPENSSL_HAVE_EC],[1],[Define to 1 if OpenSSL has EC support.])],
        ,[#include <openssl/evp.h>])
    
    AC_CHECK_DECL(EVP_sha512,
        [AC_DEFINE([XSEC_OPENSSL_HAVE_SHA2],[1],[Define to 1 if OpenSSL has SHA2 support.])],
        ,[#include <openssl/evp.h>])
    
    AC_CHECK_DECL(EVP_CIPHER_CTX_set_padding,
    	[AC_DEFINE([XSEC_OPENSSL_CANSET_PADDING],[1],[Define to 1 if OpenSSL has EVP_CIPHER_CTX_set_padding.])],
    	,[#include <openssl/evp.h>])
    
    AC_CHECK_DECL(CRYPTO_cleanup_all_ex_data,
    	[AC_DEFINE([XSEC_OPENSSL_HAVE_CRYPTO_CLEANUP_ALL_EX_DATA],[1],[Define to 1 if OpenSSL has CRYPTO_cleanup_all_ex_data.])],
    	,[#include <openssl/crypto.h>])
    
    AC_MSG_CHECKING([for const input buffer in loadX509Base64Bin])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <openssl/x509.h>]], [[ const unsigned char * buf; d2i_X509(NULL, &buf, 1);]])],
        [AC_MSG_RESULT([yes])
    	AC_DEFINE([XSEC_OPENSSL_D2IX509_CONST_BUFFER],[1],[Define to 1 if OpenSSL X509 API has const input buffer.])],
    	[AC_MSG_RESULT([no])])
fi

AC_ARG_WITH(nss,
    AS_HELP_STRING([--with-nss],[Use NSS.  Values = 'yes' or installation directory (Default = no)]),
    use_nss=$withval, use_nss=no)

if test x"$use_nss" != "xno" ; then

    # Check for explicit location or try pkg-config
    if test x"$use_nss" = "xyes" ; then
        AC_PATH_PROG(PKG_CONFIG, pkg-config)
        if test "x$PKG_CONFIG" != x && test "x$PKG_CONFIG" != "xno" ; then
            if pkg-config nss ; then
                NSSLIBS="`$PKG_CONFIG --libs nss`"
                NSSFLAGS="`$PKG_CONFIG --cflags nss`"
            else
                AC_MSG_WARN([NSS not supported by pkg-config, try --with-nss=PATH instead])
            fi
        else
            check=`nss-config --version 2>/dev/null`
            if test -n "$check"; then
                NSSLIBS=`nss-config --libs`
                NSSFLAGS=`nss-config --cflags`
            else
                AC_MSG_ERROR([Unable to use pkg-config or find nss-config, try --with-nss=PATH])
            fi
        fi
    else
        if test x_$use_nss != x_/usr; then
            NSSLIBS="-L${use_nss}/lib"
        fi
        # NSS is broken and doesn't use a standard include path.
        NSSFLAGS="-I${use_nss}/include/nss3 -I${use_nss}/include/nss"
        NSSLIBS="$NSSLIBS -lssl3 -lsmime3 -lnss3 -lplds4 -lplc4 -lnspr4"
    fi
    
    AC_MSG_CHECKING(for NSS cflags)
    AC_MSG_RESULT($NSSFLAGS)
    CPPFLAGS="$CPPFLAGS $NSSFLAGS"
    
    AC_PREPROC_IFELSE([AC_LANG_SOURCE([[#include <nss.h>]])],
        [AC_DEFINE([XSEC_HAVE_NSS],[1],[Define if NSS is in use.])],
        [AC_MSG_ERROR([Unable to find NSS headers])])
    
    AC_MSG_CHECKING(for NSS libraries)
    AC_MSG_RESULT($NSSLIBS)
    LIBS="$LIBS $NSSLIBS"
    
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <nss.h>]], [[NSS_NoDB_Init(NULL);]])],,
        [AC_MSG_ERROR([Unable to link with NSS])])
fi

# Populate the Makefile conditionals
AM_CONDITIONAL(XSEC_AM_HAVE_OPENSSL, test x"$use_openssl" != "xno")
AM_CONDITIONAL(XSEC_AM_HAVE_NSS, test x"$use_nss" != "xno")

# output the Makefiles
AC_OUTPUT
