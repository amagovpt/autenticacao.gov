/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#ifndef _EIDMW_EIDMW_XMLPARSER_H_
#define _EIDMW_EIDMW_XMLPARSER_H_

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>

#include "APLReader.h"
#include "eidmw_XML_datastorage.h"

XERCES_CPP_NAMESPACE_USE 

namespace eIDMW
{

class XML_DefHandler;
class CByteArray;

//******************************************
// Base class for XML parser to parse a file.
// The tag handler is created depending on the XML file type
//******************************************
class EIDMW_XMLParser
{
public:
	//******************************************
	// ctor
	//******************************************
	EIDMW_XMLParser( void );

	//******************************************
	// ctor
	//******************************************
	EIDMW_XMLParser( XML_DefHandler* defaultHandler );

	//******************************************
	// dtor
	//******************************************
	virtual ~EIDMW_XMLParser( void );

	//******************************************
	// Initialize the Xerces parser
	//******************************************
	bool init( void );

	//******************************************
	// Terminate the Xerces parser
	//******************************************
	bool terminate( void );

	//******************************************
	// Keep the XML file name we're about to parse
	//******************************************
	void setXMLFile( const char* xmlFile );

	//******************************************
	// Parse the XML file
	//******************************************
	bool parse( const char*		xmlFile);							//<! The XML file to be parsed

	//******************************************
	// Parse the XML input buffer
	//******************************************
	bool parse( MemBufInputSource* pMemBufIS);				//<! The memory input buffer source

	//******************************************
	// Parse the XML file
	//******************************************
	bool parse( void );

	//******************************************
	// Set the default tag handler
	//******************************************
	void setDefaultHandler( XML_DefHandler* defaultHandler);		//<! Tag handler (derived from Xerces default handler)

	//******************************************
	// check if Xerces is initialized
	//******************************************
	bool isInitialized( void );

	const CByteArray* getData(std::string const& path, size_t idx);

	const CByteArray* getData(const char* pPath, size_t idx);

	size_t getDataSize(std::string const& path);

	size_t getDataSize(const char* pPath);

	APL_CardType getDataCardType( void );

	const wchar_t* getDocVersion( void );

protected:
	EID_DataStorage	m_DataStorage;		//!< data storage for the tags we encounter

private:
	bool			m_bInitialized;		//<! Xerces initialization status
	XML_DefHandler*	m_TagHandler;		//<! Tag handler to be used
	SAX2XMLReader*  m_parser;			//<! Xerces XML parser
	const char*		m_xmlFile;			//<! XML file name to be parsed

};


} // namespace

#endif
