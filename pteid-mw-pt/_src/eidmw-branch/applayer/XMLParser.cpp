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
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/Base64.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <stack>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#include <sstream>
#include <fstream>
#else
#include <iostream.h>
#endif

#include "eidmw_EIDTag_processors.h"
#include "eidmw_EIDTagHandler.h"
#include "eidmw_EIDXmlParser.h"
#include "eidmw_TagProcessor.h"
#include "eidmw_XML_datastorage.h"
#include "eidmw_XML_DefHandler.h"
#include "eidmw_XMLParser.h"

#include "Util.h"
#include "ByteArray.h"

#include "CardPteidDef.h"

XERCES_CPP_NAMESPACE_USE 

namespace eIDMW
{

inline std::wstring trim_right(const std::wstring &source , const std::wstring& t = L" \\t")
{
	std::wstring str = source;
	return str.erase( str.find_last_not_of(t) + 1);
}

inline std::wstring trim_left( const std::wstring& source, const std::wstring& t = L" \\t")
{
	std::wstring str = source;
	return str.erase(0 , source.find_first_not_of(t) );
}

inline std::wstring trim(const std::wstring& source, const std::wstring& t = L" \t\n\r")
{
	std::wstring str = source;
	return trim_left( trim_right( str , t) , t );
} 

#define MAX_XML_STR_LEN 200
inline std::wstring FromXMLCh(const XMLCh *xmlStr)
{
#if defined(WIN32)
	return xmlStr;
#else
	// On Linux/Mac, XMLCh is an 'unsigned short', so no wchar_t (which is 4 bytes
	// with gcc). So we'll have to convert between XMLCh* and wchar_t*
	wchar_t wsOut[MAX_XML_STR_LEN];
	int i;
	for (i = 0; i < MAX_XML_STR_LEN - 1 && xmlStr[i] != 0; i++)
		wsOut[i] = (wchar_t) xmlStr[i];
	wsOut[i] = 0;
	return wsOut;
#endif
}

/*****************************************************************************************
---------------------------------------- TagProcessor --------------------------------------------
*****************************************************************************************/

//***************************************************
// Base class for the TAG processors
//***************************************************

//***************************************************
// ctor
//***************************************************
TagProcessor::TagProcessor()
{
}

TagProcessor::TagProcessor( std::wstring& tagName )
	: m_wtagName( tagName )
{
}

//***************************************************
// dtor
//***************************************************
TagProcessor::~TagProcessor()
{
}

//***************************************************
// Process the tag data
//***************************************************
void TagProcessor::process( const XMLCh *const  chars
					, const unsigned int  length 
					, DataStorage&        dataStorage
					)
{
}

//***************************************************
// Retrieve the Processor name (== tag name)
//***************************************************
std::string& TagProcessor::getProcessorName()
{
	if ( 0 == m_tagName.size() )
	{
		m_tagName = utilStringNarrow(m_wtagName);
	}
	return m_tagName;
}
//***************************************************
// Set the attributes for this tag data
//***************************************************
void TagProcessor::setAttributes(const Attributes& attributes)
{
}

void TagProcessor::displayName( void )
{
#ifdef DEBUG
	std::cout << "Processor:" << getProcessorName() << std::endl;
#endif
}

/*****************************************************************************************
---------------------------------------- EIDDocVersion_Processor --------------------------------------------
*****************************************************************************************/

static std::wstring wsTagProcDocVersion = std::wstring(TAG_PROCESSOR_DOCVERSION);
static std::wstring &wsTagProcDocVersionRef = wsTagProcDocVersion;

//***************************************************
//Tag processor: docversion
//This tag processor acts as a dummy. It can be used
//for any tag data that has not to be processed
//***************************************************
EIDDocVersion_Processor::EIDDocVersion_Processor( void )
	: TagProcessor( wsTagProcDocVersionRef )
{

}

EIDDocVersion_Processor::EIDDocVersion_Processor( std::wstring& tagName )
	: TagProcessor( tagName )
{
}

EIDDocVersion_Processor::~EIDDocVersion_Processor()
{

}

void EIDDocVersion_Processor::process( const XMLCh *const  chars
					, const unsigned int  length 
					, DataStorage&		  dataStorage
					)
{
	displayName();
	((EID_DataStorage&)dataStorage).m_DocVersion = FromXMLCh(chars);
}

/*****************************************************************************************
---------------------------------------- EIDTag_Processor --------------------------------------------
*****************************************************************************************/

static std::wstring wsTagProcEidTag = std::wstring(TAG_PROCESSOR_EIDTAG);
static std::wstring &wsTagProcEidTagRef = wsTagProcEidTag;

//***************************************************
// Tag processor: EIDTag_Processor
// This tag processor acts for any tag encounterd in the XML file.
// Depending on the path where the tag resides it will take some action.
//***************************************************
EIDTag_Processor::EIDTag_Processor( void )
	: TagProcessor( wsTagProcEidTagRef )
{
}

EIDTag_Processor::EIDTag_Processor( std::wstring& tagName )
	: TagProcessor( tagName )
{
}

EIDTag_Processor::~EIDTag_Processor( void )
{
}

void EIDTag_Processor::process( const XMLCh* const  chars
					, const unsigned int  length 
					, DataStorage&		  dataStorage
					)
{
	displayName();

	//-------------------------------------
	// if the current path matches what we have to save, copy over the data
	// to the data storage
	//-------------------------------------
	if (dataStorage.matchTagPath())
	{
		char*		pChars	= XMLString::transcode(chars);
		CByteArray* pData	= new CByteArray(pChars);
		XMLString::release(&pChars);
		dataStorage.store(dataStorage.getTagPath(), pData);
	}
}
/*****************************************************************************************
---------------------------------------- EIDCardType_Processor --------------------------------------------
*****************************************************************************************/


//***************************************************
// Tag processor: EIDTag_Processor
// This tag processor acts for any tag encounterd in the XML file.
// Depending on the path where the tag resides it will take some action.
//***************************************************
EIDCardType_Processor::EIDCardType_Processor( void )
	: TagProcessor( wsTagProcEidTagRef )
{
}

EIDCardType_Processor::EIDCardType_Processor( std::wstring& tagName )
	: TagProcessor( tagName )
{
}

EIDCardType_Processor::~EIDCardType_Processor()
{
}

void EIDCardType_Processor::process( const XMLCh *const  chars
	, const unsigned int  length 
	, DataStorage&		  dataStorage
	)
{
	displayName();

	std::wstring w_chars = FromXMLCh(chars);
	if ( 0 == w_chars.compare( utilStringWiden(CARDTYPE_NAME_PTEID_EID) ) )
	{
		((EID_DataStorage&)dataStorage).m_CardType = APL_CARDTYPE_PTEID_IAS101;
	}
	else
	{
		((EID_DataStorage&)dataStorage).m_CardType = APL_CARDTYPE_UNKNOWN;
	}

}

/*****************************************************************************************
---------------------------------------- DataStorage --------------------------------------------
*****************************************************************************************/

//******************************************
// ctor
//******************************************
DataStorage::DataStorage( void )
{
}

//******************************************
// dtor
//******************************************
DataStorage::~DataStorage( void )
{
	//-------------------------------------------
	// remove all the Data we've been collecting
	//-------------------------------------------
	for ( tDataFields::iterator tDataFields_it=m_Datafields.begin()
		; tDataFields_it != m_Datafields.end()
		; tDataFields_it++
		)
	{
		if ( 0 < tDataFields_it->second.size())
		{
			for ( tDataContainer::iterator tDataContainer_it=tDataFields_it->second.begin()
				; tDataContainer_it!=tDataFields_it->second.end()
				; tDataContainer_it++
				)
			{
				delete *tDataContainer_it;
			}
		}
	}
	m_Datafields.clear();
}

//******************************************
// get the current path path to the tag we're processing
// The path will be recovered from the tag stack and will
// have a layout as follows:
//	/<tag1>/<tag2>/...
//******************************************
std::wstring DataStorage::getTagPath( void )
{
	std::wstring tagPath;
	std::wstring sepa = L"/";

	for ( tTagStack::iterator it = m_TagStack.begin()
		; it != m_TagStack.end()
		; it++
		)
	{
		tagPath += sepa;
		tagPath += *it;
	}
	return tagPath;
}

//******************************************
// match the current tag path with the predefined
// tag paths
// returns T/F for match found or not
//******************************************
bool DataStorage::matchTagPath( void )
{
	std::wstring tagPath = getTagPath();

	for (size_t idx=0; idx<m_EIDStoreFields.size();idx++)
	{
		if ( tagPath == m_EIDStoreFields.at(idx) )
		{
			return true;
		}
	}
	return false;
}

//******************************************
// store the data pointer with its path in the data container
// Each path can have multiple data pointers. Therefore
// first the path will be looked up and the data will be added
// to the container or a new entry will be created if it is a new path
//******************************************
void DataStorage::store(std::wstring const& path, CByteArray* pData)
{
	tDataFields::iterator tDataFields_it = m_Datafields.find(path);
	if ( tDataFields_it != m_Datafields.end() )
	{
		tDataFields_it->second.push_back(pData);
	}
	else
	{
		tDataContainer tmp;
		tmp.push_back(pData);
		m_Datafields[path] = tmp;
	}
}

//******************************************
// get the data container for a specific path
// This data container can contain multiple data pointers.
// The size of the data container for the path can be requested by
// calling GetDataSize( path )
//******************************************
tDataContainer const& DataStorage::get(std::wstring const& path)
{
	tDataFields::iterator tDataFields_it = m_Datafields.find(path);
	if ( tDataFields_it != m_Datafields.end() )
	{
		return tDataFields_it->second;
	}
	else
	{
		return m_emptyContainer;
	}
}

//******************************************
// Get the idx-th data pointer for a given path
//******************************************
const CByteArray* DataStorage::get(std::wstring const& path, size_t idx)
{
	tDataFields::iterator tDataFields_it = m_Datafields.find(path);
	if ( tDataFields_it != m_Datafields.end() )
	{
		if (idx < tDataFields_it->second.size())
		{
			return tDataFields_it->second[idx];
		}
	}
	return NULL;
}

//******************************************
// get the size of the data container for a specific path
//******************************************
size_t DataStorage::getDataSize(std::wstring const& path)
{
	tDataFields::iterator tDataFields_it = m_Datafields.find(path);
	if ( tDataFields_it != m_Datafields.end() )
	{
		return tDataFields_it->second.size();
	}
	return 0;
}

/*****************************************************************************************
---------------------------------------- EID_DataStorage --------------------------------------------
*****************************************************************************************/
EID_DataStorage::EID_DataStorage( void )
	: m_CardType( APL_CARDTYPE_UNKNOWN )
{
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_ID).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_IDSIGN).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_ADDR).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_ADDRSIGN).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_PHOTO).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_CARDINFO).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_TOKENINFO).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_CHALLENGE).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_CHALLENGE_RESPONSE).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_CERTS).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_CERTS_P15).c_str());
	m_EIDStoreFields.push_back(utilStringWiden(PTEID_XML_PATH_FILE_PINS_P15).c_str());
}

EID_DataStorage::~EID_DataStorage( void )
{
}

//******************************************
// get the card type
//******************************************
APL_CardType EID_DataStorage::getCardType( void )
{
	return m_CardType;
}

//******************************************
// get the document version
//******************************************
const wchar_t* EID_DataStorage::getDocVersion( void )
{
	return m_DocVersion.c_str();
}

/*****************************************************************************************
---------------------------------------- EIDMW_XMLParser --------------------------------------------
*****************************************************************************************/

//******************************************
// Base class for XML parser to parse a file.
// The tag handler is created depending on the XML file type
//******************************************

//******************************************
// ctor
//******************************************
EIDMW_XMLParser::EIDMW_XMLParser( void )
		: m_bInitialized( false )
		, m_TagHandler( NULL )
		, m_parser( NULL )
		, m_xmlFile( NULL )
{
}

//******************************************
// ctor
//******************************************
EIDMW_XMLParser::EIDMW_XMLParser( XML_DefHandler* defaultHandler )
		: m_bInitialized(false)
		, m_parser(NULL)
		, m_xmlFile( NULL )
{
	m_TagHandler = defaultHandler;
}

//******************************************
// dtor
//******************************************
EIDMW_XMLParser::~EIDMW_XMLParser( void )
{
	terminate();
	m_bInitialized = false;
}

//******************************************
// Initialize the Xerces parser
//******************************************
bool EIDMW_XMLParser::init( void )
{
	if (!m_bInitialized)
	{
		try 
		{
			XMLPlatformUtils::Initialize();
			m_bInitialized = true;
		}
		catch (const XMLException& e) 
		{
			const XMLCh* msg = e.getMessage();
			msg = msg;
			// We should not get an exception during initialization
			// so, make sure we return false state
			m_bInitialized = false;
		}
	}
	return m_bInitialized;
}

//******************************************
// Terminate the Xerces parser
//******************************************
bool EIDMW_XMLParser::terminate( void )
{
	if (m_TagHandler)
	{
		delete m_TagHandler;
	}
	if ( m_parser )
	{
		delete m_parser;
	}

	XMLPlatformUtils::Terminate();
	return true;
}

//******************************************
// Keep the XML file name we're about to parse
//******************************************
void EIDMW_XMLParser::setXMLFile( const char* xmlFile )
{
	m_TagHandler->m_fileName = xmlFile;
}

	//******************************************
	// Parse the XML file
	//******************************************
bool EIDMW_XMLParser::parse( const char*		xmlFile							//<! The XML file to be parsed
		  )
{
	if( !xmlFile )
	{
		return false;
	}
	setXMLFile(xmlFile);
	m_xmlFile = xmlFile;

	return parse();
}

//******************************************
// Parse the XML input buffer
//******************************************
bool EIDMW_XMLParser::parse( MemBufInputSource* pMemBufIS				//<! The memory input buffer source
	      )
{
	m_TagHandler->m_fileName = NULL;
	m_xmlFile				 = NULL;

	if (!pMemBufIS)
	{
		return false;
	}

	if ( !m_TagHandler )
	{
		return false;
	}

	//--------------------------------
	// create and initialize Xerces parser
	//--------------------------------
	m_parser = XMLReaderFactory::createXMLReader();

	m_parser->setFeature(XMLUni::fgSAX2CoreValidation, true);   
	m_parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
	m_parser->setContentHandler(m_TagHandler);
	m_parser->setLexicalHandler(m_TagHandler);
	m_parser->setErrorHandler(m_TagHandler);

	try 
	{
		m_parser->parse(*pMemBufIS);
	}
	catch (const XMLException& e) 
	{
		char* message = XMLString::transcode(e.getMessage());
		XMLString::release(&message);
		return false;
	}
	catch (const SAXParseException& e) 
	{
		std::string msg;
		XML_DefHandler::formatError(e,m_xmlFile,msg);
		//throw e; 
		return false;
	}
	return true;
}

//******************************************
// Parse the XML file
//******************************************
bool EIDMW_XMLParser::parse( void )
{
	if ( !m_TagHandler )
	{
		return false;
	}

	//--------------------------------
	// Check if we can find the file
	//--------------------------------
	std::filebuf* strBuf;
	std::ifstream infile;

	strBuf = infile.rdbuf();

	infile.open(m_xmlFile,std::ifstream::in);

	if (infile.fail())
	{
		return false;
	}
	infile.close();

	//--------------------------------
	// create and initialize Xerces parser
	//--------------------------------
	m_parser = XMLReaderFactory::createXMLReader();

	m_parser->setFeature(XMLUni::fgSAX2CoreValidation, true);   
	m_parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
	m_parser->setContentHandler(m_TagHandler);
	m_parser->setLexicalHandler(m_TagHandler);
	m_parser->setErrorHandler(m_TagHandler);

	try 
	{
		m_parser->parse(m_xmlFile);
	}
	catch (const XMLException& e) 
	{
		char* message = XMLString::transcode(e.getMessage());
		XMLString::release(&message);
		return false;
	}
	catch (const SAXParseException& e) 
	{
		std::string msg;
		XML_DefHandler::formatError(e,m_xmlFile,msg);
		//throw e;
		return false;
	}
	return true;
}

//******************************************
// Set the default tag handler
//******************************************
void EIDMW_XMLParser::setDefaultHandler( XML_DefHandler* defaultHandler		//<! Tag handler (derived from Xerces default handler)
					  )
{
	if (m_TagHandler)
	{
		delete m_TagHandler;
		m_TagHandler = NULL;
	}
	m_TagHandler  = defaultHandler;
}

//******************************************
// check if Xerces is initialized
//******************************************
bool EIDMW_XMLParser::isInitialized( void )
{
	return m_bInitialized;
}

const CByteArray* EIDMW_XMLParser::getData(std::string const& path, size_t idx)
{
	return getData(path.c_str(),idx);
}

const CByteArray* EIDMW_XMLParser::getData(const char* pPath, size_t idx)
{
	std::wstring wPath = utilStringWiden(std::string(pPath));
	return m_DataStorage.get(wPath,idx);
}

size_t EIDMW_XMLParser::getDataSize(std::string const& path)
{
	return getDataSize(path.c_str());
}

size_t EIDMW_XMLParser::getDataSize(const char* pPath)
{
	std::wstring wPath = utilStringWiden(std::string(pPath));
	return m_DataStorage.getDataSize(wPath);
}

APL_CardType EIDMW_XMLParser::getDataCardType( void )
{
	return m_DataStorage.getCardType();
}

const wchar_t* EIDMW_XMLParser::getDocVersion( void )
{
	return m_DataStorage.getDocVersion();
}

/*****************************************************************************************
---------------------------------------- EIDMW_EIDXMLParser --------------------------------------------
*****************************************************************************************/

//***************************************************
// ctor
//***************************************************
EIDMW_EIDXMLParser::EIDMW_EIDXMLParser(void)
{
	//----------------------------------------------------------
	// create all the processors needed for the EID card XML file
	// The dummy processor must always be present. It will process
	// the tag if no specific processor is installed in the pool
	//----------------------------------------------------------

	XML_DefHandler* defaultHandler = new XML_EIDTagHandler(m_DataStorage);

	setDefaultHandler( defaultHandler/*, m_DataStorage*/ );
	EIDMW_XMLParser::init();
}

//***************************************************
// dtor
//***************************************************
EIDMW_EIDXMLParser::~EIDMW_EIDXMLParser(void)
{
}

//***************************************************
// Parse an XML file
//***************************************************
bool EIDMW_EIDXMLParser::parse( const char*		xmlFile							//!< full path to the XML file to be processed
		  )
{
	return EIDMW_XMLParser::parse( xmlFile );
}

/*****************************************************************************************
---------------------------------------- EIDMW_EIDMemParser --------------------------------------------
*****************************************************************************************/

//***************************************************
// ctor
//***************************************************
EIDMW_EIDMemParser::EIDMW_EIDMemParser(void)
	: m_buffer(NULL)
	, m_BufLen(0)
	, m_pMemBufId(NULL)
	, m_memBufIS(NULL)
{
	//----------------------------------------------------------
	// create all the processors needed for the EID card XML file
	// The dummy processor must always be present. It will process
	// the tag if no specific processor is installed in the pool
	//----------------------------------------------------------

	XML_DefHandler* defaultHandler = new XML_EIDTagHandler(m_DataStorage);

	setDefaultHandler( defaultHandler/*, m_DataStorage*/ );
	init();
}

//***************************************************
// ctor
//***************************************************
EIDMW_EIDMemParser::EIDMW_EIDMemParser( const char* buffer	//!< the buffer to parse
				  , unsigned int len		//!< the length of the buffer
				  )
	: m_buffer(buffer)
	, m_BufLen(len)
	, m_pMemBufId(NULL)
	, m_memBufIS(NULL)
{
	setMembufID();
	XML_DefHandler*    defaultHandler = new XML_EIDTagHandler(m_DataStorage);
	setDefaultHandler( defaultHandler/*, m_DataStorage*/ );
	if (!init())
	{
		return;
	}
	createInputSource();
}
//***************************************************
// dtor
//***************************************************
EIDMW_EIDMemParser::~EIDMW_EIDMemParser(void)
{
	if (m_pMemBufId)
	{
		delete [] m_pMemBufId;
		m_pMemBufId = NULL;
	}
	if (m_memBufIS)
	{
		delete m_memBufIS;
		m_memBufIS = NULL;
	}
	m_buffer = NULL;
	m_BufLen = 0;
}
//***************************************************
// Parse an XML file in a buffer
//***************************************************
bool EIDMW_EIDMemParser::parse( void )
{
	if ( !m_buffer )
	{
		return false;
	}
	if (0 == m_BufLen)
	{
		return false;
	}
	setMembufID();
	createInputSource();
	return EIDMW_XMLParser::parse( m_memBufIS );
}
//***************************************************
// Parse an XML file in a buffer
//***************************************************
bool EIDMW_EIDMemParser::parse( const char*	pBuffer							//!< memory buffer to parse
		  , unsigned int len								//!< buffer size in bytes
		  )
{
	m_buffer = pBuffer;
	m_BufLen = len;
	setMembufID();
	createInputSource();
	return EIDMW_XMLParser::parse( m_memBufIS );
}

//***************************************************
// create a memory buffer ID
//***************************************************
void EIDMW_EIDMemParser::setMembufID( void )
{
	if (m_pMemBufId)
	{
		delete [] m_pMemBufId;
		m_pMemBufId = NULL;
	}
	m_pMemBufId = new char [strlen(ID_MEMBUFFER)+1];
	strcpy_s(m_pMemBufId,strlen(ID_MEMBUFFER)+1,ID_MEMBUFFER);
}

//***************************************************
// Create the input source for the xerces parser
//***************************************************
void EIDMW_EIDMemParser::createInputSource( void )
{
	if (!isInitialized())
	{
		return;
	}
	
	if (m_memBufIS)
	{
		delete m_memBufIS;
		m_memBufIS = NULL;
	}
	m_memBufIS = new MemBufInputSource((const XMLByte*)m_buffer
		, m_BufLen			// nr of bytes of the buffer
		, m_pMemBufId	// just an ID
		, false			// just use the buffer, do not adopt it
		);
}

/*****************************************************************************************
---------------------------------------- XML_DefHandler --------------------------------------------
*****************************************************************************************/

//***************************************************
// Base class for tag handler
// It implements common methods for all the Xerces XML handlers
// (EID and SIS XML files)
//***************************************************

//***************************************************
// ctor
//***************************************************
XML_DefHandler::XML_DefHandler( DataStorage& dataStorage )
	: m_fileName(NULL)
	, m_DataStorage(dataStorage)
{
}

//***************************************************
// ctor
//***************************************************
XML_DefHandler::XML_DefHandler( const char* fileName, DataStorage& dataStorage )
	: m_fileName(fileName)
	, m_DataStorage(dataStorage)
{
}

//***************************************************
// dtor
//***************************************************
XML_DefHandler::~XML_DefHandler( void )
{
}

//***************************************************
// Format the error given by the Xerces parser
// The format is compatible to the gcc errors.
// <filename>:<linenr>:<columnnr>: error: <message>
//***************************************************
void XML_DefHandler::formatError(const SAXParseException& e, const char* filename, std::string& errMsg )
{
	std::stringstream str(std::stringstream::out);
	char* msg = XMLString::transcode(e.getMessage());

	if(filename){
		str << filename ;
	}
	else {
		str << "file" ;
	}

	str	<< ":"
		<< std::dec << e.getLineNumber()
		<< ":"
		<< std::dec <<e.getColumnNumber()
		<< ": error"
		<< ": "
		<< msg
		<< std::endl
		;

	errMsg = str.rdbuf()->str();
	XMLString::release(&msg);

}

/*****************************************************************************************
---------------------------------------- XML_EIDTagHandler --------------------------------------------
*****************************************************************************************/

//***************************************************
// ctor
//***************************************************
XML_EIDTagHandler::XML_EIDTagHandler( DataStorage&	dataStorage )
	: XML_DefHandler(dataStorage)
{
}

//***************************************************
// ctor
//***************************************************
XML_EIDTagHandler::XML_EIDTagHandler( const char* fileName, DataStorage& dataStorage )
	: XML_DefHandler( fileName, dataStorage )
{
}

//***************************************************
// dtor
//***************************************************
XML_EIDTagHandler::~XML_EIDTagHandler( void )
{
}

//***************************************************
// create a tag processor according to the tag name passed and assign the attributes to the processor
//***************************************************
TagProcessor* XML_EIDTagHandler::createProcessor(const XMLCh* const localname, const Attributes& attrs)
{
	TagProcessor* pProcessor = NULL;
	//----------------------------------
	// create a tag processor depending on the name of the tag
	//----------------------------------
	std::wstring tagName = FromXMLCh(localname);

	if ( tagName == XML_TAG_CARDTYPE_PROCESSOR )
	{
		pProcessor = new EIDCardType_Processor();
	}
	else if ( tagName == XML_TAG_DOCVERSION_PROCESSOR )
	{
		pProcessor = new EIDDocVersion_Processor();
	}
	else
	{
		pProcessor = new EIDTag_Processor(tagName);
	}

	pProcessor->setAttributes(attrs);
	return pProcessor;
}

//***************************************************
// Xerces implemented functions to interpret the tags
// of an EID XML file (EID)
//***************************************************
void XML_EIDTagHandler::startDocument( void )
{
}

void XML_EIDTagHandler::endDocument( void )
{
}

//***************************************************
// A start tag is detected. create a processor and put it on the processor stack.
// Put the tag name on a tag stack, such we can trace the tag path
//***************************************************
void XML_EIDTagHandler::startElement(	const   XMLCh* const    uri,
					const   XMLCh* const    localname,
					const   XMLCh* const    qname,
					const   Attributes&     attrs
				 )
{
	std::wstring localName = FromXMLCh( localname );
	localName = trim(localName);

	TagProcessor* pProcessor = createProcessor(localname, attrs);

	m_ProcessorStack.push(pProcessor);
	m_DataStorage.m_TagStack.push_back(localName);

}

//***************************************************
// An end tag is detected. Pull the tag processor from stack
//***************************************************
void XML_EIDTagHandler::endElement ( const XMLCh *const   uri,  
				  const XMLCh *const   localname,  
				  const XMLCh *const   qname   
				) 
{
	std::wstring localName = FromXMLCh( localname );
	localName = trim(localName);

	TagProcessor* pProcessor    = m_ProcessorStack.top();
	std::wstring  ProcessorName = pProcessor->m_wtagName;
	if (ProcessorName == localName )
	{
		delete pProcessor;
		pProcessor = NULL;
		m_ProcessorStack.pop();
	}
	m_DataStorage.m_TagStack.pop_back();
}

//***************************************************
// Tag data is detected. Call the processing function of the tag
// processor on the processor stack
//***************************************************
void XML_EIDTagHandler::characters (	const XMLCh *const  chars,  
					const unsigned int  length   
				) 
{
	std::wstring str = FromXMLCh( chars );
	str = trim(str);
	if ( 0 == str.size() )
	{
		return;
	}

	m_ProcessorStack.top()->process( chars, length, m_DataStorage);
}

//***************************************************
// When no DTD is given, this comes through for each tag
//***************************************************
void XML_EIDTagHandler::error(const SAXParseException& e)
{
}

void XML_EIDTagHandler::fatalError(const SAXParseException& e)
{
	std::string errMsg;
	formatError(e,m_fileName,errMsg);
	throw e;
}


} // namespace

