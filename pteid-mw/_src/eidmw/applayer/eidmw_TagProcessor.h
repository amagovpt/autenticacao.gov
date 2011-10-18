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
#ifndef _EIDMW_TAGPROCESSOR_H_
#define _EIDMW_TAGPROCESSOR_H_

XERCES_CPP_NAMESPACE_USE 

namespace eIDMW
{

class DataStorage;

//***************************************************
// Base class for the TAG processors
//***************************************************
class TagProcessor
{
public:
	//***************************************************
	// ctor
	//***************************************************
	TagProcessor();

	TagProcessor( std::wstring& tagName );

	//***************************************************
	// dtor
	//***************************************************
	virtual ~TagProcessor();

	//***************************************************
	// Process the tag data
	//***************************************************
	virtual void process( const XMLCh *const  chars
						, const unsigned int  length 
						, DataStorage&        dataStorage
						);

	//***************************************************
	// Retrieve the Processor name (== tag name)
	//***************************************************
	std::string& getProcessorName();

	//***************************************************
	// Set the attributes for this tag data
	//***************************************************
	void setAttributes(const Attributes& attributes);

	void displayName( void );

public:
	std::wstring m_wtagName;

private:
	std::string  m_tagName;
};


} // namespace

#endif
