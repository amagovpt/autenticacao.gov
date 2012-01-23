/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * XSEC
 *
 * XENCCipherImpl := Implementation of the main encryption worker class
 *
 * $Id: XENCCipherImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/transformers/TXFMSB.hpp>
#include <xsec/transformers/TXFMURL.hpp>
#include <xsec/transformers/TXFMDocObject.hpp>
#include <xsec/transformers/TXFMConcatChains.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/enc/XSECKeyInfoResolver.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/framework/XSECAlgorithmMapper.hpp>
#include <xsec/framework/XSECAlgorithmHandler.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>

#include "XENCCipherImpl.hpp"
#include "XENCEncryptedDataImpl.hpp"
#include "XENCEncryptedKeyImpl.hpp"
#include "XENCEncryptionMethodImpl.hpp"
#include "XENCAlgorithmHandlerDefault.hpp"

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/Janitor.hpp>

// With all the characters - just uplift entire thing

XERCES_CPP_NAMESPACE_USE

#include <iostream>
using std::cout;

// --------------------------------------------------------------------------------
//			Constant Strings
// --------------------------------------------------------------------------------


const XMLCh s_tagname[] = {

chLatin_f, chLatin_r, chLatin_a, chLatin_g, chLatin_m, chLatin_e, chLatin_n, chLatin_t, chNull };

const XMLCh s_noData[] = { chLatin_n, chLatin_o, chLatin_D, chLatin_a, chLatin_t, chLatin_a, chNull };

const XMLCh s_ds[] = { chLatin_d, chLatin_s, chNull };

// --------------------------------------------------------------------------------
//			Constructors
// --------------------------------------------------------------------------------

XENCCipherImpl::XENCCipherImpl(DOMDocument * doc) :
    mp_doc(doc), mp_encryptedData(NULL), mp_key(NULL), mp_kek(NULL), mp_keyInfoResolver(NULL) {

    XSECnew(mp_env, XSECEnv(doc));
    mp_env->setDSIGNSPrefix(s_ds);
    m_keyDerived = false;
    m_kekDerived = false;
    m_useExcC14nSerialisation = true;

}

XENCCipherImpl::~XENCCipherImpl() {

    if (mp_encryptedData != NULL)
        delete mp_encryptedData;

    if (mp_key != NULL)
        delete mp_key;

    if (mp_kek != NULL)
        delete mp_kek;

    if (mp_env != NULL)
        delete mp_env;

    if (mp_keyInfoResolver != NULL)
        delete mp_keyInfoResolver;

}

// --------------------------------------------------------------------------------
//			Initialiser
// --------------------------------------------------------------------------------

void XENCCipherImpl::Initialise(void) {

    XENCAlgorithmHandlerDefault def;

    // Register default encryption algorithm handlers

    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURI3DES_CBC, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIAES128_CBC, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIAES192_CBC, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIAES256_CBC, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIKW_AES128, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIKW_AES192, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIKW_AES256, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIKW_3DES, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIRSA_1_5, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIRSA_OAEP_MGFP1, def);

}

// --------------------------------------------------------------------------------
//			Set/get the namespace prefix to be used when creating nodes
// --------------------------------------------------------------------------------

void XENCCipherImpl::setXENCNSPrefix(const XMLCh * prefix) {

    mp_env->setXENCNSPrefix(prefix);

}

const XMLCh * XENCCipherImpl::getXENCNSPrefix(void) const {

    return mp_env->getXENCNSPrefix();

}
// --------------------------------------------------------------------------------
//			Key Info resolvers
// --------------------------------------------------------------------------------

void XENCCipherImpl::setKeyInfoResolver(const XSECKeyInfoResolver * resolver) {

    if (mp_keyInfoResolver != NULL)
        delete mp_keyInfoResolver;

    mp_keyInfoResolver = resolver->clone();

}

// --------------------------------------------------------------------------------
//			Key Info resolvers
// --------------------------------------------------------------------------------

XENCEncryptedData * XENCCipherImpl::getEncryptedData(void) {

    return mp_encryptedData;

}
// --------------------------------------------------------------------------------
//			Keys
// --------------------------------------------------------------------------------

void XENCCipherImpl::setKey(XSECCryptoKey * key) {

    if (mp_key != NULL)
        delete mp_key;

    mp_key = key;
    m_keyDerived = false;

}

void XENCCipherImpl::setKEK(XSECCryptoKey * key) {

    if (mp_kek != NULL)
        delete mp_kek;

    mp_kek = key;
    m_kekDerived = false;

}

// --------------------------------------------------------------------------------
//			Serialise/Deserialise an element
// --------------------------------------------------------------------------------

DOMDocumentFragment * XENCCipherImpl::deSerialise(safeBuffer &content, DOMNode * ctx) {

    DOMDocumentFragment * result;

    // Create the context to parse the document against
    safeBuffer sb, sbt;
    sb.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);
    //sb.sbXMLChAppendCh(chUnicodeMarker);
    //sb.sbXMLChCat8("<?xml version=\"1.0\" encoding=\"UTF-16\"?><");
    sb.sbXMLChAppendCh(chOpenAngle);
    sb.sbXMLChCat(s_tagname);

    // Run through each node up to the document node and find any
    // xmlns: nodes that may be needed during the parse of the decrypted content

    DOMNode * ctxParent = ctx->getParentNode();
    DOMNode * wk = ctxParent;

    while (wk != NULL) {

        DOMNamedNodeMap * atts = wk->getAttributes();
        XMLSize_t length;
        if (atts != NULL)
            length = atts->getLength();
        else
            length = 0;

        for (XMLSize_t i = 0; i < length; ++i) {
            DOMNode * att = atts->item(i);
            if (strEquals(att->getNodeName(), DSIGConstants::s_unicodeStrXmlns) ||
                    (XMLString::compareNString(att->getNodeName(), DSIGConstants::s_unicodeStrXmlns, 5) == 0 &&
                            att->getNodeName()[5] == chColon)) {

                // Check to see if this node has already been found
                DOMNode * p = ctxParent;
                bool found = false;
                while (p != wk) {
                    DOMNamedNodeMap * tstAtts = p->getAttributes();
                    if (tstAtts != NULL && tstAtts->getNamedItem(att->getNodeName()) != NULL) {
                        found = true;
                        break;
                    }
                    p = p->getParentNode();
                }
                if (found == false) {

                    // This is an attribute node that needs to be added
                    sb.sbXMLChAppendCh(chSpace);
                    sb.sbXMLChCat(att->getNodeName());
                    sb.sbXMLChAppendCh(chEqual);
                    sb.sbXMLChAppendCh(chDoubleQuote);
                    sb.sbXMLChCat(att->getNodeValue());
                    sb.sbXMLChAppendCh(chDoubleQuote);
                }
            }
        }
        wk = wk->getParentNode();
    }
    sb.sbXMLChAppendCh(chCloseAngle);

    char* prefix = transcodeToUTF8(sb.rawXMLChBuffer());

    sbt = prefix;
    const char * crcb = content.rawCharBuffer();
    int offset = 0;
    if (crcb[0] == '<' && crcb[1] == '?') {
        // Have a PI prefix - get rid of it
        int i = 2;
        while (crcb[i] != '\0' && crcb[i] != '>')
            ++i;

        if (crcb[i] == '>')
            offset = i + 1;
    }

    sbt.sbStrcatIn(&crcb[offset]);

    // Now transform the content to UTF-8
    //sb.sbXMLChCat8(content.rawCharBuffer());

    // Terminate the string
    sb.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);
    sb.sbXMLChAppendCh(chOpenAngle);
    sb.sbXMLChAppendCh(chForwardSlash);
    sb.sbXMLChCat(s_tagname);
    sb.sbXMLChAppendCh(chCloseAngle);

    char* trailer = transcodeToUTF8(sb.rawXMLChBuffer());
    sbt.sbStrcatIn(trailer);
    XSEC_RELEASE_XMLCH(trailer);

    // Now we need to parse the document
    XercesDOMParser* parser = NULL;
    MemBufInputSource* memIS = NULL;
    try {
        parser = new XercesDOMParser;

        parser->setDoNamespaces(true);
        parser->setCreateEntityReferenceNodes(true);
        parser->setDoSchema(false);

        // Create an input source
        xsecsize_t bytes = XMLString::stringLen(sbt.rawCharBuffer());
        memIS = new MemBufInputSource((const XMLByte*) sbt.rawBuffer(), bytes, "XSECMem");
    }
    catch (...) {
        delete memIS;
        delete parser;
        XSEC_RELEASE_XMLCH(prefix);
        throw;
    }

    XSEC_RELEASE_XMLCH(prefix);
    Janitor<XercesDOMParser> j_parser(parser);
    Janitor<MemBufInputSource> j_memIS(memIS);

    parser->parse(*memIS);
    xsecsize_t errorCount = parser->getErrorCount();
    if (errorCount > 0)
        throw XSECException(XSECException::CipherError, "Errors occured during de-serialisation of decrypted element content");

    DOMDocument * doc = parser->getDocument();

    // Create a DocumentFragment to hold the children of the parsed doc element
    DOMDocument *ctxDocument = ctx->getOwnerDocument();
    result = ctxDocument->createDocumentFragment();
    Janitor<DOMDocumentFragment> j_result(result);

    // Now get the children of the document into a DOC fragment
    DOMNode * fragElt = doc->getDocumentElement();
    DOMNode * child;

    if (fragElt != NULL) {
        child = fragElt->getFirstChild();
    } else {

        throw XSECException(XSECException::CipherError, "XENCCipher::deSerialse - re-parsed document unexpectedly empty");
    }

    while (child != NULL) {
        result->appendChild(ctxDocument->importNode(child, true));
        child = child->getNextSibling();
    }

    // Done!

    j_result.release();
    return result;
}

// --------------------------------------------------------------------------------
//			Decrypt an Element and replace in original document
// --------------------------------------------------------------------------------

XSECCryptoKey * XENCCipherImpl::decryptKeyFromKeyInfoList(DSIGKeyInfoList * kil) {

    XSECCryptoKey * ret = NULL;
    XSECAlgorithmHandler *handler;

    int kLen = (int) kil->getSize();

    for (int i = 0; ret == NULL && i < kLen; ++i) {

        if (kil->item(i)->getKeyInfoType() == DSIGKeyInfo::KEYINFO_ENCRYPTEDKEY) {

            XENCEncryptedKey * ek = (XENCEncryptedKey*) (kil->item(i));
            volatile XMLByte buffer[1024];
            try {
                // Have to cast off volatile
                int keySize = decryptKey(ek, (XMLByte *) buffer, 1024);

                if (keySize > 0) {
                    // Try to map the key

                    XENCEncryptionMethod * encryptionMethod = mp_encryptedData->getEncryptionMethod();

                    if (encryptionMethod != NULL) {

                        handler = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(
                            mp_encryptedData->getEncryptionMethod()->getAlgorithm());

                        if (handler != NULL)
                            ret = handler->createKeyForURI(mp_encryptedData->getEncryptionMethod()->getAlgorithm(),
                                (XMLByte *) buffer, keySize);
                    }
                }
            }

            catch (XSECCryptoException &) {
                /* Do nothing - this is likely to be a bad decrypt on a public key */
            } catch (...) {
                memset((void *) buffer, 0, 1024);
                throw;
            }

            // Clear out the key buffer
            memset((void *) buffer, 0, 1024);
        }
    }

    return ret;
}

XENCEncryptedData * XENCCipherImpl::loadEncryptedData(DOMElement * element) {

    // First of all load the element
    if (mp_encryptedData != NULL)
        delete mp_encryptedData;

    XSECnew(mp_encryptedData, XENCEncryptedDataImpl(mp_env, element));

    // Load
    mp_encryptedData->load();

    return mp_encryptedData;

}

DOMDocument * XENCCipherImpl::decryptElement(DOMElement * element) {

    // First of all load the element
    if (mp_encryptedData != NULL)
        delete mp_encryptedData;

    XSECnew(mp_encryptedData, XENCEncryptedDataImpl(mp_env, element));

    // Load
    mp_encryptedData->load();

    return decryptElement();

}

DOMNode * XENCCipherImpl::decryptElementDetached(DOMElement * element) {

    // First of all load the element
    if (mp_encryptedData != NULL)
        delete mp_encryptedData;

    XSECnew(mp_encryptedData, XENCEncryptedDataImpl(mp_env, element));

    // Load
    mp_encryptedData->load();

    return decryptElementDetached();

}

DOMNode * XENCCipherImpl::decryptElementDetached(void) {

    XSECAlgorithmHandler *handler;

    if (mp_encryptedData == NULL) {

        throw XSECException(XSECException::CipherError, "XENCCipherImpl::decryptElement - no element loaded for decryption");

    }

    // Check that this is a valid type
    const XMLCh * typeURI = mp_encryptedData->getType();

    if (typeURI != NULL && !strEquals(typeURI, DSIGConstants::s_unicodeStrURIXENC_ELEMENT) && !strEquals(typeURI,
        DSIGConstants::s_unicodeStrURIXENC_CONTENT)) {

        throw XSECException(XSECException::CipherError, "XENCCipherImpl::decryptElement - Type not Element or Content");

    }

    if (m_keyDerived && mp_key) {
        delete mp_key;
        mp_key = NULL;
    }

    // Make sure we have a key before we do anything else too drastic
    if (mp_key == NULL) {

        if (mp_keyInfoResolver != NULL)
            mp_key = mp_keyInfoResolver->resolveKey(mp_encryptedData->getKeyInfoList());

        if (mp_key == NULL) {

            mp_key = decryptKeyFromKeyInfoList(mp_encryptedData->getKeyInfoList());

        }

        if (mp_key == NULL) {

            throw XSECException(XSECException::CipherError, "XENCCipherImpl::decryptElement - No key set and cannot resolve");
        }

        m_keyDerived = true;
    }

    // Get the raw encrypted data
    TXFMChain * c = mp_encryptedData->createCipherTXFMChain();
    Janitor<TXFMChain> j_c(c);

    // Get the Algorithm handler for the algorithm
    XENCEncryptionMethod * encryptionMethod = mp_encryptedData->getEncryptionMethod();

    if (encryptionMethod != NULL) {

        handler
            = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(mp_encryptedData->getEncryptionMethod()->getAlgorithm());

    }

    else {

        handler = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(XSECAlgorithmMapper::s_defaultEncryptionMapping);

    }

    safeBuffer sb("");
    unsigned int decryptLen;

    if (handler != NULL) {

        decryptLen = handler->decryptToSafeBuffer(c, mp_encryptedData->getEncryptionMethod(), mp_key,
            mp_env->getParentDocument(), sb);
    } else {

        // Very strange if we get here - any problems should throw an
        // exception in the AlgorithmMapper.

        throw XSECException(XSECException::CipherError,
            "XENCCipherImpl::decryptElement - Error retrieving a handler for algorithm");

    }

    sb[decryptLen] = '\0';

    // Now de-serialise
    DOMElement * element = mp_encryptedData->getElement();
    DOMDocumentFragment * frag = deSerialise(sb, element);

    return frag;

}

DOMDocument * XENCCipherImpl::decryptElement(void) {

    // Call the worker
    DOMElement * element = mp_encryptedData->getElement();
    DOMDocumentFragment * frag = (DOMDocumentFragment *) decryptElementDetached();

    if (frag != NULL) {

        // Have something to replace current element with
        DOMNode * p = element->getParentNode();

        // By inserting the DocumentFragment, we effectively insert the children
        p->replaceChild(frag, element);

        // Delete the frag and the old element
        frag->release();
        element->release();

    }

    return mp_env->getParentDocument();

}

// --------------------------------------------------------------------------------
//			Decrypt data to an input stream
// --------------------------------------------------------------------------------

XSECBinTXFMInputStream * XENCCipherImpl::decryptToBinInputStream(
    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element
) {

    XSECAlgorithmHandler *handler;

    // First of all load the element
    if (mp_encryptedData != NULL)
    delete mp_encryptedData;

    XSECnew(mp_encryptedData,
        XENCEncryptedDataImpl(mp_env, element));

    // Load
    mp_encryptedData->load();

    // Check key is valid
    if (m_keyDerived && mp_key) {
        delete mp_key;
        mp_key = NULL;
    }

    // Make sure we have a key before we do anything else too drastic
    if (mp_key == NULL) {

        if (mp_keyInfoResolver != NULL)
        mp_key = mp_keyInfoResolver->resolveKey(mp_encryptedData->getKeyInfoList());

        if (mp_key == NULL) {

            mp_key = decryptKeyFromKeyInfoList(mp_encryptedData->getKeyInfoList());

        }

        if (mp_key == NULL) {

            throw XSECException(XSECException::CipherError,
                "XENCCipherImpl::decryptToBinInputStream - No key set and cannot resolve");
        }

        m_keyDerived = true;
    }

    // Get the raw encrypted data
    TXFMChain * c = mp_encryptedData->createCipherTXFMChain();
    Janitor<TXFMChain> j_c(c);

    // Get the Algorithm handler for the algorithm
    XENCEncryptionMethod * encryptionMethod = mp_encryptedData->getEncryptionMethod();

    if (encryptionMethod != NULL) {

        handler =
        XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(
            mp_encryptedData->getEncryptionMethod()->getAlgorithm());

    }

    else {

        handler =
        XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(
            XSECAlgorithmMapper::s_defaultEncryptionMapping);

    }

    safeBuffer sb("");

    if (handler != NULL) {

        if (handler->appendDecryptCipherTXFM(c,
                mp_encryptedData->getEncryptionMethod(),
                mp_key,
                mp_env->getParentDocument()) != true) {
            throw XSECException(XSECException::CipherError,
                "XENCCipherImpl::decryptToBinInputStream - error appending final transform");
        }

    }
    else {

        // Very strange if we get here - any problems should throw an
        // exception in the AlgorithmMapper.

        throw XSECException(XSECException::CipherError,
            "XENCCipherImpl::decryptElement - Error retrieving a handler for algorithm");

    }

    // Wrap in a Bin input stream
    XSECBinTXFMInputStream * ret;
    ret = new XSECBinTXFMInputStream(c); // Probs with MSVC++ mean no XSECnew
    j_c.release(); // Now owned by "ret"

    return ret;

}

// --------------------------------------------------------------------------------
//			Decrypt a key in an XENCEncryptedKey element
// --------------------------------------------------------------------------------

int XENCCipherImpl::decryptKey(XENCEncryptedKey * encryptedKey, XMLByte * rawKey, int maxKeySize) {

    // Check KEK is valid
    if (m_kekDerived && mp_kek) {
        delete mp_kek;
        mp_kek = NULL;
    }

    // Make sure we have a key before we do anything else too drastic
    if (mp_kek == NULL) {

        if (mp_keyInfoResolver != NULL)
            mp_kek = mp_keyInfoResolver->resolveKey(encryptedKey->getKeyInfoList());

        if (mp_kek == NULL) {

            throw XSECException(XSECException::CipherError, "XENCCipherImpl::decryptKey - No KEK set and cannot resolve");
        }
        m_kekDerived = true;
    }

    // Get the raw encrypted data
    TXFMChain * c = ((XENCEncryptedKeyImpl *) encryptedKey)->createCipherTXFMChain();
    Janitor<TXFMChain> j_c(c);

    // Get the Algorithm handler for the algorithm
    XENCEncryptionMethod * encryptionMethod = encryptedKey->getEncryptionMethod();
    XSECAlgorithmHandler *handler;

    if (encryptionMethod != NULL) {

        handler = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(encryptedKey->getEncryptionMethod()->getAlgorithm());

    }

    else {

        handler = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(XSECAlgorithmMapper::s_defaultEncryptionMapping);

    }

    safeBuffer sb("");
    unsigned int keySize;

    if (handler != NULL) {

        keySize = handler->decryptToSafeBuffer(c, encryptedKey->getEncryptionMethod(), mp_kek, mp_env->getParentDocument(), sb);
    } else {

        // Very strange if we get here - any problems should throw an
        // exception in the AlgorithmMapper.

        throw XSECException(XSECException::CipherError,
            "XENCCipherImpl::decryptElement - Error retrieving a handler for algorithm");

    }

    keySize = (keySize < (unsigned int) maxKeySize ? keySize : (unsigned int) maxKeySize);
    memcpy(rawKey, sb.rawBuffer(), keySize);

    return keySize;
}
// --------------------------------------------------------------------------------
//			Decrypt a key from a DOM structure
// --------------------------------------------------------------------------------

int XENCCipherImpl::decryptKey(DOMElement * keyNode, XMLByte * rawKey, int maxKeySize) {

    XENCEncryptedKey * encryptedKey = loadEncryptedKey(keyNode);
    Janitor<XENCEncryptedKey> j_encryptedKey(encryptedKey);

    // Now decrypt!
    return decryptKey(encryptedKey, rawKey, maxKeySize);

}

XENCEncryptedKey * XENCCipherImpl::loadEncryptedKey(DOMElement * keyNode) {

    XENCEncryptedKeyImpl * encryptedKey;
    XSECnew(encryptedKey, XENCEncryptedKeyImpl(mp_env, keyNode));
    Janitor<XENCEncryptedKeyImpl> j_encryptedKey(encryptedKey);

    // Load
    encryptedKey->load();

    j_encryptedKey.release();
    return encryptedKey;

}
// --------------------------------------------------------------------------------
//			Encrypt a BinInputStream
// --------------------------------------------------------------------------------

XENCEncryptedData * XENCCipherImpl::encryptBinInputStream(
    XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * plainText,
    encryptionMethod em,
    const XMLCh * algorithmURI) {

    TXFMURL * uri;
    XSECnew(uri, TXFMURL(mp_doc, NULL));

    uri->setInput(plainText);
    TXFMChain c(uri);

    return encryptTXFMChain(&c, em, algorithmURI);

}

// --------------------------------------------------------------------------------
//			Encrypt a TXFMChain
// --------------------------------------------------------------------------------

XENCEncryptedData * XENCCipherImpl::encryptTXFMChain(TXFMChain * plainText, encryptionMethod em, const XMLCh * algorithmURI) {

    // Make sure we have a key before we do anything too drastic
    if (mp_key == NULL) {
        throw XSECException(XSECException::CipherError, "XENCCipherImpl::encryptElement - No key set");
    }

    // Map the encryption method to a URI
    safeBuffer algorithmSB;
    const XMLCh * algorithm;

    if (em == ENCRYPT_NONE) {
        algorithm = algorithmURI;
    } else {
        if (encryptionMethod2URI(algorithmSB, em) != true) {
            throw XSECException(XSECException::CipherError, "XENCCipherImpl::encryptElement - Unknown encryption method");
        }
        algorithm = algorithmSB.sbStrToXMLCh();
    }

    // Create the element with a dummy encrypted value

    if (mp_encryptedData != NULL) {
        delete mp_encryptedData;
        mp_encryptedData = NULL;
    }

    XSECnew(mp_encryptedData, XENCEncryptedDataImpl(mp_env));
    mp_encryptedData->createBlankEncryptedData(XENCCipherData::VALUE_TYPE, algorithm, s_noData);

    // Perform the encryption
    XSECAlgorithmHandler *handler;

    if (algorithm != NULL) {

        handler = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(algorithm);

    }

    else {

        handler = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(XSECAlgorithmMapper::s_defaultEncryptionMapping);

    }

    safeBuffer sb;

    if (handler != NULL) {

        handler->encryptToSafeBuffer(plainText, mp_encryptedData->getEncryptionMethod(), mp_key, mp_env->getParentDocument(),
            sb);
    } else {

        // Very strange if we get here - any problems should throw an
        // exception in the AlgorithmMapper.

        throw XSECException(XSECException::CipherError,
            "XENCCipherImpl::encryptTXFMChain - Error retrieving a handler for algorithm");

    }

    // Set the value
    XENCCipherValue * val = mp_encryptedData->getCipherData()->getCipherValue();

    val->setCipherString(sb.sbStrToXMLCh());

    return mp_encryptedData;

}

// --------------------------------------------------------------------------------
//			Encrypt a key
// --------------------------------------------------------------------------------

XENCEncryptedKey * XENCCipherImpl::encryptKey(const unsigned char * keyBuffer, unsigned int keyLen, encryptionMethod em,
    const XMLCh * algorithmURI) {

    if (mp_kek == NULL) {
        throw XSECException(XSECException::CipherError, "XENCCipherImpl::encryptKey - No KEK set");
    }

    // Map the encryption method to a URI
    safeBuffer algorithmSB;
    const XMLCh * algorithm;

    if (em == ENCRYPT_NONE) {
        algorithm = algorithmURI;
    } else {
        if (encryptionMethod2URI(algorithmSB, em) != true) {
            throw XSECException(XSECException::CipherError, "XENCCipherImpl::encryptKey - Unknown encryption method");
        }
        algorithm = algorithmSB.sbStrToXMLCh();
    }

    // Create the element with a dummy encrypted value

    XENCEncryptedKeyImpl * encryptedKey;

    XSECnew(encryptedKey, XENCEncryptedKeyImpl(mp_env));
    Janitor<XENCEncryptedKeyImpl> j_encryptedKey(encryptedKey);

    encryptedKey->createBlankEncryptedKey(XENCCipherData::VALUE_TYPE, algorithm, s_noData);

    // Create a transform chain to do pass the key to the encrypto

    safeBuffer rawKey;
    rawKey.isSensitive();
    rawKey.sbMemcpyIn(keyBuffer, keyLen);

    TXFMSB * tsb;
    XSECnew(tsb, TXFMSB(mp_doc));

    TXFMChain * c;
    XSECnew(c, TXFMChain(tsb));
    Janitor<TXFMChain> j_c(c);

    tsb->setInput(rawKey, keyLen);

    // Perform the encryption
    XSECAlgorithmHandler *handler;

    if (algorithm != NULL) {

        handler = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(algorithm);

    }

    else {

        handler = XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(XSECAlgorithmMapper::s_defaultEncryptionMapping);

    }

    safeBuffer sb;

    if (handler != NULL) {

        handler->encryptToSafeBuffer(c, encryptedKey->getEncryptionMethod(), mp_kek, mp_env->getParentDocument(), sb);
    } else {

        // Very strange if we get here - any problems should throw an
        // exception in the AlgorithmMapper.

        throw XSECException(XSECException::CipherError, "XENCCipherImpl::encryptKey - Error retrieving a handler for algorithm");

    }

    // Set the value
    XENCCipherValue * val = encryptedKey->getCipherData()->getCipherValue();

    val->setCipherString(sb.sbStrToXMLCh());

    j_encryptedKey.release();
    return encryptedKey;
}

// --------------------------------------------------------------------------------
//			Create an EncryptedData element
// --------------------------------------------------------------------------------

XENCEncryptedData * XENCCipherImpl::createEncryptedData(XENCCipherData::XENCCipherDataType type, const XMLCh * algorithm,
    const XMLCh * value) {

    // Clean out anything currently being used
    if (mp_encryptedData != NULL) {
        delete mp_encryptedData;
        mp_encryptedData = NULL;
    }
    // Create a new EncryptedData element

    XSECnew(mp_encryptedData, XENCEncryptedDataImpl(mp_env));
    mp_encryptedData->createBlankEncryptedData(type, algorithm, value);

    return mp_encryptedData;
}

// --------------------------------------------------------------------------------
//			Encrypt an element
// --------------------------------------------------------------------------------

DOMNode * XENCCipherImpl::encryptElementDetached(DOMElement * element, encryptionMethod em, const XMLCh * algorithmURI) {

    // Make sure we have a key before we do anything too drastic
    if (mp_key == NULL) {
        throw XSECException(XSECException::CipherError, "XENCCipherImpl::encryptElement - No key set");
    }

    // Create a transform chain to do the encryption
    TXFMDocObject * tdocObj;
    XSECnew(tdocObj, TXFMDocObject(mp_doc));
    TXFMChain * c;
    XSECnew(c, TXFMChain(tdocObj));

    Janitor<TXFMChain> j_c(c);

    tdocObj->setInput(mp_doc, element);

    // Now need to serialise the element - easiest to just use a canonicaliser
    TXFMC14n *tc14n;
    XSECnew(tc14n, TXFMC14n(mp_doc));
    c->appendTxfm(tc14n);

    tc14n->activateComments();
    if (m_useExcC14nSerialisation)
        tc14n->setExclusive();

    // Do the hard work

    encryptTXFMChain(c, em, algorithmURI);

    mp_encryptedData->setType(DSIGConstants::s_unicodeStrURIXENC_ELEMENT);
    return mp_encryptedData->getElement();

}

DOMDocument * XENCCipherImpl::encryptElement(DOMElement * element, encryptionMethod em, const XMLCh * algorithmURI) {

    // Do the actual encryption work
    encryptElementDetached(element, em, algorithmURI);

    // Replace original element
    DOMNode * p = element->getParentNode();

    if (p == NULL) {
        throw XSECException(XSECException::CipherError, "XENCCipherImpl::encryptElement - Passed in element has no parent");
    }

    p->replaceChild(mp_encryptedData->getElement(), element);

    // Clear up the old child
    element->release();

    return mp_doc;

}

// --------------------------------------------------------------------------------
//			Encrypt an element's children
// --------------------------------------------------------------------------------

DOMDocument * XENCCipherImpl::encryptElementContent(
    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
    encryptionMethod em,
    const XMLCh * algorithmURI) {

    // Do the work
    encryptElementContentDetached(element, em, algorithmURI);

    // Delete current children
    DOMNode * n = element->getFirstChild();
    while (n != NULL) {

        element->removeChild(n);
        n->release();

        n = element->getFirstChild();

    }

    // Now add the EncryptedData
    element->appendChild(mp_encryptedData->getElement());

    return mp_doc;

}

DOMNode * XENCCipherImpl::encryptElementContentDetached(
    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
    encryptionMethod em,
    const XMLCh * algorithmURI) {

    // Make sure we have a key before we do anything too drastic
    if (mp_key == NULL) {
        throw XSECException(XSECException::CipherError,
            "XENCCipherImpl::encryptElement - No key set");
    }

    // Create a transform chain to do the encryption
    // We use a concat transformer so we can concatinate the bytestreams
    // from the serialisation of each child in turn

    TXFMConcatChains * tcat;
    XSECnew(tcat, TXFMConcatChains(mp_doc));
    TXFMChain * c;
    XSECnew(c, TXFMChain(tcat));
    Janitor<TXFMChain> j_c(c);

    DOMNode *n = element->getFirstChild();

    while (n != NULL) {

        TXFMDocObject * tdocObj;
        XSECnew(tdocObj, TXFMDocObject(mp_doc));
        TXFMChain * tc;
        XSECnew(tc, TXFMChain(tdocObj));

        // Add to the concat object, which will own it, so if anything throws
        // the memory will be released.

        tcat->setInput(tc);
        tdocObj->setInput(mp_doc, n);

        // Now need to serialise the element - easiest to just use a canonicaliser
        TXFMC14n *tc14n;
        XSECnew(tc14n, TXFMC14n(mp_doc));
        tc->appendTxfm(tc14n);

        tc14n->activateComments();
        if (m_useExcC14nSerialisation)
        tc14n->setExclusive();

        n = n->getNextSibling();

    }

    encryptTXFMChain(c, em, algorithmURI);

    mp_encryptedData->setType(DSIGConstants::s_unicodeStrURIXENC_CONTENT);

    return mp_encryptedData->getElement();

}

// --------------------------------------------------------------------------------
//			Pretty Print functions
// --------------------------------------------------------------------------------

void XENCCipherImpl::setExclusiveC14nSerialisation(bool flag) {

    m_useExcC14nSerialisation = flag;
}

bool XENCCipherImpl::getExclusiveC14nSerialisation(void) {

    return m_useExcC14nSerialisation;

}

// --------------------------------------------------------------------------------
//			Exclusive C14n serialisation setting
// --------------------------------------------------------------------------------

void XENCCipherImpl::setPrettyPrint(bool flag) {

    mp_env->setPrettyPrintFlag(flag);

}

bool XENCCipherImpl::getPrettyPrint(void) {

    return mp_env->getPrettyPrintFlag();

}
