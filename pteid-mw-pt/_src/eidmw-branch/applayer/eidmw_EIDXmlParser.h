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
#ifndef _EIDMW_EIDMW_EIDXMLPARSER_H_
#define _EIDMW_EIDMW_EIDXMLPARSER_H_

#include <xercesc/framework/MemBufInputSource.hpp>

#include "eidmw_XMLParser.h"

XERCES_CPP_NAMESPACE_USE 

namespace eIDMW
{

//***************************************************
// Xerces implemented functions to interpret the tags
// of an EID XML file (EID)
// The object has a processor pool. This pool can be used to hold
// the different ptocessors for the tags in the XML file to be
// processed.
// The processors are recycled each time a tag is detected. In case
// the same tag can be used on different levels in the XML file, the
// processor pool can not be used because the same processor will be called.
// This can cause data to be destroyed in the tag processor since it is the
// each time same processor instance
// e.g.:
// <?xml ....>
// <root_tag>				--> root tag
//     <tag_1>				--> tag processor_1 is put on the processor stack
//			<tag_2>			--> tag processor_2 is put on the processor stack
//				</tag_1>	--> tag processor_1 is put on again the processor stack !!!
//				<tag_1>		--> tag processor_1 is removed from the processor stack
//			</tag_2>		--> tag processor_2 is removed from the processor stack
//     </tag_1>				--> tag processor_1 is removed from the processor stack
// </root_tag>
//***************************************************
class EIDMW_EIDXMLParser : public EIDMW_XMLParser
{
public:
	//***************************************************
	// ctor
	//***************************************************
	EIDMW_EIDXMLParser(void);

	//***************************************************
	// dtor
	//***************************************************
	virtual ~EIDMW_EIDXMLParser(void);

	//***************************************************
	// Parse an XML file
	//***************************************************
	bool parse( const char*		xmlFile	);						//!< full path to the XML file to be processed

private:
};

#define ID_MEMBUFFER "membuffer"

class EIDMW_EIDMemParser : public EIDMW_XMLParser
{
public:
	//***************************************************
	// ctor
	//***************************************************
	EIDMW_EIDMemParser(void);

	//***************************************************
	// ctor
	//***************************************************
	EIDMW_EIDMemParser( const char* buffer	//!< the buffer to parse
					  , unsigned int len		//!< the length of the buffer
					  );

	//***************************************************
	// dtor
	//***************************************************
	virtual ~EIDMW_EIDMemParser(void);

	//***************************************************
	// Parse an XML file in a buffer
	//***************************************************
	bool parse( void );

	//***************************************************
	// Parse an XML file in a buffer
	//***************************************************
	bool parse( const char*	pBuffer							//!< memory buffer to parse
			  , unsigned int len								//!< buffer size in bytes
			  );

private:
	//***************************************************
	// create a memory buffer ID
	//***************************************************
	void setMembufID( void );

	//***************************************************
	// Create the input source for the xerces parser
	//***************************************************
	void createInputSource( void );

private:
	const char*			m_buffer;			//<! memory buffer to parse
	unsigned int		m_BufLen;			//<! length of buffer in bytes
	char*				m_pMemBufId;		//<! a simple memory buffer ID
	MemBufInputSource*	m_memBufIS;			//<! memory input source
};


} // namespace

#endif
