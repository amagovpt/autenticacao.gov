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
#include "APLDoc.h"
#include "CardFile.h"
#include "cryptoFwkPteid.h"
#include "../common/Util.h"

namespace eIDMW
{

/*****************************************************************************************
------------------------------------ APL_XMLDoc ---------------------------------------
*****************************************************************************************/
	
APL_XMLDoc::APL_XMLDoc()
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
}

APL_XMLDoc::~APL_XMLDoc()
{
}

bool APL_XMLDoc::writeXmlToFile(const char * csFilePath)
{
	CByteArray doc=getXML();
	return writeToFile(doc,csFilePath);
}

bool APL_XMLDoc::writeCsvToFile(const char * csFilePath)
{
	CByteArray doc=getCSV();
	return writeToFile(doc,csFilePath);
}

bool APL_XMLDoc::writeTlvToFile(const char * csFilePath)
{
	CByteArray doc=getTLV();
	return writeToFile(doc,csFilePath);
}

bool APL_XMLDoc::writeToFile(const CByteArray &doc,const char * csFilePath)
{

	FILE *f=NULL;

	fopen_s(&f,csFilePath,"wb");

	if(f)
	{
		size_t len = fwrite(doc.GetBytes(),doc.Size(),1,f);
		len = len;
		fclose(f);

		return true;
	}
	return false;
}

/*****************************************************************************************
------------------------------------ APL_Biometric ---------------------------------------
*****************************************************************************************/

APL_Biometric::~APL_Biometric()
{
}

}
