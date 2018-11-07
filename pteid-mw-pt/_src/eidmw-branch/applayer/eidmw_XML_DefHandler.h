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
#ifndef _EIDMW_XML_DEFHANDLER_H_
#define _EIDMW_XML_DEFHANDLER_H_

#include <xercesc/sax2/DefaultHandler.hpp>

XERCES_CPP_NAMESPACE_USE 

namespace eIDMW
{
#define TAG_PROCESSOR_EIDTAG		L"EIDTag"
#define TAG_PROCESSOR_DOCVERSION	L"doc_version"

//***************************************************
// Base class for tag handler
// It implements common methods for all the Xerces XML handlers
// (EID and SIS XML files)
//***************************************************
class XML_DefHandler : public DefaultHandler
{
public:
	//***************************************************
	// ctor
	//***************************************************
	XML_DefHandler( DataStorage& dataStorage );

	//***************************************************
	// ctor
	//***************************************************
	XML_DefHandler( const char* fileName, DataStorage& dataStorage );

	//***************************************************
	// dtor
	//***************************************************
	virtual ~XML_DefHandler( void );

	//***************************************************
	// Format the error given by the Xerces parser
	// The format is compatible to the gcc errors.
	// <filename>:<linenr>:<columnnr>: error: <message>
	//***************************************************
	static void formatError(const SAXParseException& e, const char* filename, std::string& errMsg );

public:
	const char*		m_fileName;							//!< XML file name we're processing

protected:
	DataStorage&	m_DataStorage;
};



} // namespace

#endif
