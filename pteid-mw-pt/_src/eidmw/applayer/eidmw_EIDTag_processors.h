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
#ifndef _EIDMW_EIDTAG_PROCESSORS_H_
#define _EIDMW_EIDTAG_PROCESSORS_H_

#include "eidmw_TagProcessor.h"

XERCES_CPP_NAMESPACE_USE 

namespace eIDMW
{

//***************************************************
//Tag processor: doc°version
//This tag processor acts as a dummy. It can be used
//for any tag data that has not to be processed
//***************************************************
class EIDDocVersion_Processor : public TagProcessor
{
public:
	EIDDocVersion_Processor( void );

	EIDDocVersion_Processor( std::wstring& tagName );

	virtual ~EIDDocVersion_Processor();

	virtual void process( const XMLCh *const  chars
						, const unsigned int  length 
						, DataStorage&		  dataStorage
						);
};

//***************************************************
// Tag processor: EIDTag_Processor
// This tag processor acts for any tag encounterd in the XML file.
// Depending on the path where the tag resides it will take some action.
//***************************************************
class EIDTag_Processor : public TagProcessor
{
public:
	EIDTag_Processor( void );

	EIDTag_Processor( std::wstring& tagName );

	virtual ~EIDTag_Processor( void );

	virtual void process( const XMLCh* const  chars
						, const unsigned int  length 
						, DataStorage&		  dataStorage
						);
};

//***************************************************
// Tag processor: EIDTag_Processor
// This tag processor acts for any tag encounterd in the XML file.
// Depending on the path where the tag resides it will take some action.
//***************************************************
class EIDCardType_Processor : public TagProcessor
{
public:
	EIDCardType_Processor( void );

	EIDCardType_Processor( std::wstring& tagName );

	virtual ~EIDCardType_Processor();

	virtual void process( const XMLCh *const  chars
		, const unsigned int  length 
		, DataStorage&		  dataStorage
		);
};


} // namespace

#endif

