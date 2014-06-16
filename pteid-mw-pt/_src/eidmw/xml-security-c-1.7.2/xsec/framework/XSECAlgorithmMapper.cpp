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
 * XSECAlgorithmMapper := Provides a table of AlgorithmHandlers
 *						  Mapped by Type URI
 *
 * $Id: XSECAlgorithmMapper.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECAlgorithmHandler.hpp>
#include <xsec/framework/XSECAlgorithmMapper.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

// Xerces

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// Default strings

const XMLCh XSECAlgorithmMapper::s_defaultEncryptionMapping [] = {
	chLatin_D,
	chLatin_e,
	chLatin_f,
	chLatin_a,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chLatin_E,
	chLatin_n,
	chLatin_c,
	chLatin_r,
	chLatin_y,
	chLatin_p,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chNull
};



XSECAlgorithmMapper::XSECAlgorithmMapper(void) {

}


XSECAlgorithmMapper::~XSECAlgorithmMapper() {

	MapperEntryVectorType::iterator it = m_mapping.begin();

	while (it != m_mapping.end()) {
		
		XSEC_RELEASE_XMLCH(((*it)->mp_uri));
		delete (*it)->mp_handler;
		delete (*it);

		it++;

	}

	m_mapping.clear();

	XMLCh* ptr;
	for (WhitelistVectorType::const_iterator i = m_whitelist.begin(); i != m_whitelist.end(); ++i) {
	    ptr = *i;
	    XSEC_RELEASE_XMLCH(ptr);
	}
	m_whitelist.clear();

    for (WhitelistVectorType::const_iterator i = m_blacklist.begin(); i != m_blacklist.end(); ++i) {
        ptr = *i;
        XSEC_RELEASE_XMLCH(ptr);
    }
    m_blacklist.clear();
}

XSECAlgorithmMapper::MapperEntry * XSECAlgorithmMapper::findEntry(const XMLCh * URI) const {

	MapperEntryVectorType::const_iterator it = m_mapping.begin();

	while (it != m_mapping.end()) {
		
		if (strEquals((*it)->mp_uri, URI)) {
			return (*it);
		}

		it++;

	}

	return NULL;

}


XSECAlgorithmHandler * XSECAlgorithmMapper::mapURIToHandler(const XMLCh * URI) const {

    bool allowed = true;
    if (!m_whitelist.empty()) {
        allowed = false;
        for (WhitelistVectorType::const_iterator i = m_whitelist.begin(); !allowed && i != m_whitelist.end(); ++i) {
            if (XMLString::equals(URI, *i))
                allowed = true;
        }
    }

    if (allowed && !m_blacklist.empty()) {
        for (WhitelistVectorType::const_iterator i = m_blacklist.begin(); allowed && i != m_blacklist.end(); ++i) {
            if (XMLString::equals(URI, *i))
                allowed = false;
        }
    }

    if (!allowed) {
        safeBuffer output;
        output.sbTranscodeIn("XSECAlgorithmMapper::mapURIToHandler - URI ");
        output.sbXMLChCat(URI);
        output.sbXMLChCat(" disallowed by whitelist/blacklist policy");
        throw XSECException(XSECException::AlgorithmMapperError,
            output.rawXMLChBuffer());
    }

	MapperEntry * entry = findEntry(URI);

	if (entry == NULL) {
		safeBuffer output;
		output.sbTranscodeIn("XSECAlgorithmMapper::mapURIToHandler - URI ");
		output.sbXMLChCat(URI);
		output.sbXMLChCat(" not found");
		throw XSECException(XSECException::AlgorithmMapperError,
			output.rawXMLChBuffer());
	}

	return entry->mp_handler;
}

void XSECAlgorithmMapper::registerHandler(const XMLCh * URI, const XSECAlgorithmHandler & handler) {

	MapperEntry * entry = findEntry(URI);

	if (entry != NULL) {

		delete entry->mp_handler;

	}
	else {
		XSECnew(entry, MapperEntry);

		entry->mp_uri = XMLString::replicate(URI);
		m_mapping.push_back(entry);

	}
	entry->mp_handler = handler.clone();

}

void XSECAlgorithmMapper::whitelistAlgorithm(const XMLCh* URI)
{
    m_whitelist.push_back(XMLString::replicate(URI));
}

void XSECAlgorithmMapper::blacklistAlgorithm(const XMLCh* URI)
{
    m_blacklist.push_back(XMLString::replicate(URI));
}
