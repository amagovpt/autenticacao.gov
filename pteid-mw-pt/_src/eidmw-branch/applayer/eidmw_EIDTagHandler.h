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
#ifndef _EIDMW_EIDTAGHANDLER_H_
#define _EIDMW_EIDTAGHANDLER_H_

#include "eidmw_XML_DefHandler.h"

#include <stack>

XERCES_CPP_NAMESPACE_USE 

namespace eIDMW
{

class TagProcessor;

typedef std::stack<TagProcessor*> tProcessorStack;

//***************************************************
// the following tags will be used to create a corresponding processor
//***************************************************
#define XML_TAG_CARDTYPE_PROCESSOR		L"card_type"
#define XML_TAG_DOCVERSION_PROCESSOR	L"doc_version"

//***************************************************
// EID tag handler implementation of the Xerces parser
//***************************************************
class XML_EIDTagHandler : public XML_DefHandler
{
public:
	//***************************************************
	// ctor
	//***************************************************
	XML_EIDTagHandler( DataStorage&	dataStorage );

	//***************************************************
	// ctor
	//***************************************************
	XML_EIDTagHandler( const char* fileName, DataStorage& dataStorage );

	//***************************************************
	// dtor
	//***************************************************
	virtual ~XML_EIDTagHandler( void );

	//***************************************************
	// create a tag processor according to the tag name passed and assign the attributes to the processor
	//***************************************************
	TagProcessor* createProcessor(const XMLCh* const localname, const Attributes& attrs);

	//***************************************************
	// Xerces implemented functions to interpret the tags
	// of an EID XML file (EID)
	//***************************************************
	void startDocument( void );

	void endDocument( void );

	//***************************************************
	// A start tag is detected. create a processor and put it on the processor stack.
	// Put the tag name on a tag stack, such we can trace the tag path
	//***************************************************
	void startElement(	const   XMLCh* const    uri,
						const   XMLCh* const    localname,
						const   XMLCh* const    qname,
						const   Attributes&     attrs
					 );

	//***************************************************
	// An end tag is detected. Pull the tag processor from stack
	//***************************************************
	void endElement ( const XMLCh *const   uri,  
					  const XMLCh *const   localname,  
					  const XMLCh *const   qname   
					);

	//***************************************************
	// Tag data is detected. Call the processing function of the tag
	// processor on the processor stack
	//***************************************************
	void characters (	const XMLCh *const  chars,  
						const unsigned int  length   
					);

	//***************************************************
	// When no DTD is given, this comes through for each tag
	//***************************************************
	void error(const SAXParseException& e);

	void fatalError(const SAXParseException& e);

private:
	tProcessorStack m_ProcessorStack;			//!< local processor stack

};


} // namespace

#endif
