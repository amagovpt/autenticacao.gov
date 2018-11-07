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
#ifndef _EIDMW_XML_DATASTORAGE_H_
#define _EIDMW_XML_DATASTORAGE_H_

#include <map>
#include <vector>

XERCES_CPP_NAMESPACE_USE 

namespace eIDMW
{
typedef std::vector<std::wstring>				tTagStack;
typedef std::vector<CByteArray*>				tDataContainer;
typedef std::map<std::wstring,tDataContainer>	tDataFields;
typedef std::vector<std::wstring>				tStoreFields;

class DataStorage
{
public:
	//******************************************
	// ctor
	//******************************************
	DataStorage( void );

	//******************************************
	// dtor
	//******************************************
	virtual ~DataStorage( void );

	//******************************************
	// get the current path path to the tag we're processing
	// The path will be recovered from the tag stack and will
	// have a layout as follows:
	//	/<tag1>/<tag2>/...
	//******************************************
	std::wstring getTagPath( void );

	//******************************************
	// match the current tag path with the predefined
	// tag paths
	// returns T/F for match found or not
	//******************************************
	bool matchTagPath( void );

	//******************************************
	// store the data pointer with its path in the data container
	// Each path can have multiple data pointers. Therefore
	// first the path will be looked up and the data will be added
	// to the container or a new entry will be created if it is a new path
	//******************************************
	void store(std::wstring const& path, CByteArray* pData);

	//******************************************
	// get the data container for a specific path
	// This data container can contain multiple data pointers.
	// The size of the data container for the path can be requested by
	// calling GetDataSize( path )
	//******************************************
	tDataContainer const& get(std::wstring const& path);

	//******************************************
	// Get the idx-th data pointer for a given path
	//******************************************
	const CByteArray* get(std::wstring const& path, size_t idx);

	//******************************************
	// get the size of the data container for a specific path
	//******************************************
	size_t getDataSize(std::wstring const& path);

public:
	tTagStack		m_TagStack;				//!< keeps the start tags on a stack suck we can trace the nesting

protected:
	tStoreFields	m_EIDStoreFields;		//!< the fields we need to keep

private:
	tDataFields		m_Datafields;			//!< storage for all the data found by the parser
	tDataContainer  m_emptyContainer;

};

class EID_DataStorage : public DataStorage
{
public:
	EID_DataStorage( void );

	virtual ~EID_DataStorage( void );

	//******************************************
	// get the card type
	//******************************************
	APL_CardType getCardType( void );

	//******************************************
	// get the document version
	//******************************************
	const wchar_t* getDocVersion( void );

public:
	APL_CardType	m_CardType;				//!< keep the card type we're working on
	std::wstring	m_DocVersion;			//!< keep the document version
};

} // namespace
#endif

