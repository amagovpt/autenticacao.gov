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
#pragma once

#ifndef __SCDOCUTIL_H__
#define __SCDOCUTIL_H__

#include <string>
#include "Export.h"
#include "ByteArray.h"
#include "APLCard.h"

namespace eIDMW
{

class APL_CryptoFwk;

/******************************************************************************//**
  * Abstract base class for extracting XML Document
  *********************************************************************************/
class APL_XMLDoc
{
public:
	/**
	  * Constructor
	  */
	EIDMW_APL_API APL_XMLDoc();

	/**
	  * Pur virtual destructor
	  */
	EIDMW_APL_API virtual ~APL_XMLDoc()=0;

	/**
	  * Return the XML document
	  */
	EIDMW_APL_API virtual CByteArray getXML(bool bNoHeader=false)=0;

	/**
	  * Return the CSV document
	  */
	EIDMW_APL_API virtual CByteArray getCSV()=0;

	/**
	  * Return the CSV document
	  */
	EIDMW_APL_API virtual CByteArray getTLV()=0;

	/**
	  * Write the xml document into the file csFilePath
	  * @return true if succeded
	  */
	EIDMW_APL_API virtual bool writeXmlToFile(const char * csFilePath);

	/**
	  * Write the csv document into the file csFilePath
	  * @return true if succeded
	  */
	EIDMW_APL_API virtual bool writeCsvToFile(const char * csFilePath);

	/**
	  * Write the tlv document into the file csFilePath
	  * @return true if succeded
	  */
	EIDMW_APL_API virtual bool writeTlvToFile(const char * csFilePath);

	/**
	  * Generic function for writing to a file
	  */
	EIDMW_APL_API static bool writeToFile(const CByteArray &doc,const char * csFilePath);

protected:
	APL_CryptoFwk *m_cryptoFwk;						/**< Pointer to the crypto framework */

	CMutex m_Mutex;									/**< Mutex */

private:
	APL_XMLDoc(const APL_XMLDoc &doc);				/**< Copy not allowed - not implemented */
	APL_XMLDoc &operator=(const APL_XMLDoc &doc);	/**< Copy not allowed - not implemented */
};

/******************************************************************************//**
  * Abstract base class for biometric document (picture, finger print,...)
  *********************************************************************************/
class APL_Biometric : public APL_XMLDoc
{
public:
	/**
	  * Pur virtual destructor
	  */
	EIDMW_APL_API virtual ~APL_Biometric()=0;
};


}

#endif //__SCDOCUTIL_H__
