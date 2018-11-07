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

#ifndef __CARDFILE_H__
#define __CARDFILE_H__

#include <string>
#include "ByteArray.h"
#include "APLCardFile.h"
#include "APLCard.h"

namespace eIDMW
{


static CByteArray EmptyByteArray;

class APL_Card;
class APL_CryptoFwk;

/******************************************************************************//**
  * Base class that represent one file on a card
  *********************************************************************************/
class APL_CardFile
{
public:
	/**
	  * Constructor 
	  *
	  * - card and path are kept in private member
	  * - status is set to CARDFILESTATUS_UNREAD
	  * - if csPath is NULL, data is filled with file
	  *
	  * @param card is a pointer to the card containing the file 
	  * @param csPath is the path of the file to be read
	  * @param file : The content of the file to put in m_data 
	  */
	APL_CardFile(APL_Card *card,const char *csPath,const CByteArray *file=NULL);

	/**
	  * Destructor 
	  */
	virtual ~APL_CardFile();

	/**
	  * Return the status of the file
	  * @param bForceRead if true, the file is read before returning the status 
	  */
	virtual tCardFileStatus getStatus(bool bForceRead=false);

	/**
	  * Return the content of the file
	  */
	virtual const CByteArray& getData();

protected:
	/**
	  * ReadFile ask the card object to read the file
	  *   then VerifyFile
	  *   and if file is ok, MapFields is call
	  */
	virtual tCardFileStatus LoadData(bool bForceReload=false);

	/**
	  * Put the content of the file in m_data
	  */
	virtual void ReadFile();

	/**
	  * Must return CARDFILESTATUS_OK if the validation is ok 
	  */
	virtual tCardFileStatus VerifyFile()=0;

	virtual void doSODCheck(bool check)=0;

	/**
	  * Return true if data can be showned 
	  */
	virtual bool ShowData();

	/**
	  * Map the field with the content of the file
	  * (Fields are implemented in derived class)
	  * Must return true if map is ok
	  */
	virtual bool MapFields()=0;

	/**
	  * Empty all fields
	  */
	virtual void EmptyFields()=0;

	APL_Card *m_card;				/**< Pointer to card object, needed in constructor, use to read the file */
	std::string m_path;				/**< Path of the file to be read, needed in constructor */
	CByteArray m_data;				/**< Content of the file (result of ReadFile) */
	tCardFileStatus m_status;		/**< Status of the file */

	bool m_keepdata;				/**< m_data could not be read from card or clear (comes from constructor) */

	APL_CryptoFwk *m_cryptoFwk;		/**< Pointer to the crypto framework */

	CMutex m_Mutex;					/**< Mutex */
	bool m_mappedFields;			/**< are the fields mapped already? */
	bool m_isVerified;				/**< verify file only once */
	bool m_SODCheck;


private:
	APL_CardFile(const APL_CardFile &file);				/**< Copy not allowed - not implemented */
	APL_CardFile &operator=(const APL_CardFile &file);	/**< Copy not allowed - not implemented */
};

class APL_SmartCard;
/******************************************************************************//**
  * Class for one certificate file
  *
  * The file may comes from a card or from a CByteArray
  *********************************************************************************/
class APL_CardFile_Certificate : public APL_CardFile
{
public:
	/**
	  * Constructor 
	  * @param card is a pointer to the card containing the file 
	  * @param csPath is the path of the file to be read
	  * @param file : The content of the file to put in m_data 
	  */
	APL_CardFile_Certificate(APL_SmartCard *card,const char *csPath,const CByteArray *file=NULL);

	/**
	  * Destructor 
	  */
	virtual ~APL_CardFile_Certificate();

	/**
	  * Calculate if necessary and return the UniqueId of the certificate 
	  */
	unsigned long getUniqueId();

private:
	APL_CardFile_Certificate(const APL_CardFile_Certificate& file);				/**< Copy not allowed - not implemented */
	APL_CardFile_Certificate &operator= (const APL_CardFile_Certificate& file);	/**< Copy not allowed - not implemented */


	virtual tCardFileStatus VerifyFile();	/**< Always return CARDFILESTATUS_OK */
	virtual bool MapFields() {return true;}	/**< Nothing to do m_data contains the file */
	virtual void EmptyFields() {}	/**< Nothing to do m_data contains the file */
	void doSODCheck(bool check) {}

	unsigned long m_ulUniqueId;				/**< The unique ID of the certificate */
};

/******************************************************************************//**
  * Class for one pseudo info file
  *
  * The file comes from a APL_SmartCard
  *********************************************************************************/
class APL_CardFile_Info : public APL_CardFile
{
public:
	/**
	  * Constructor 
	  * @param card is a pointer to the card containing the file 
	  */
	APL_CardFile_Info(APL_SmartCard *card);

	/**
	  * Destructor 
	  */
	virtual ~APL_CardFile_Info();

	const char *getSerialNumber();			/**< Return the Serial number of the card */
	const char *getComponentCode();			/**< Return the Component code  of the card */
	const char *getOsNumber();				/**< Return the Os Number of the card */
	const char *getOsVersion();				/**< Return the Os Version of the card */
	const char *getSoftmaskNumber();		/**< Return the Softmask Number of the card */
	const char *getSoftmaskVersion();		/**< Return the Softmask Version of the card */
	const char *getAppletVersion();			/**< Return the Applet Version of the card */
	const char *getGlobalOsVersion();		/**< Return the Global OS Version of the card */
	const char *getAppletInterfaceVersion();/**< Return the Applet Interface Version of the card */
	const char *getPKCS1Support();			/**< Return the PKCS#1 Support of the card */
	const char *getKeyExchangeVersion();	/**< Return the Key Exchange Version of the card */
	const char *getAppletLifeCicle();		/**< Return the Applet Life Cicle of the card */

protected:
	/**
	  * Put the content of the file in m_data
	  */
	virtual void ReadFile();

private:
	APL_CardFile_Info(const APL_CardFile_Info& file);				/**< Copy not allowed - not implemented */
	APL_CardFile_Info &operator= (const APL_CardFile_Info& file);	/**< Copy not allowed - not implemented */

	virtual tCardFileStatus VerifyFile();	/**< Always return CARDFILESTATUS_OK */
	virtual bool MapFields();				/**< Map the fields with the content of the file */
	virtual void EmptyFields();				/**< Empty all fields */
	void doSODCheck(bool check) {}

	std::string m_ComponentCode;			/**< The Component Code of the card */
	std::string m_OsNumber;					/**< The Os Number of the card */
	std::string m_OsVersion;				/**< The Os Version of the card */
	std::string m_SoftmaskNumber;			/**< The Softmask Number of the card */
	std::string m_SoftmaskVersion;			/**< The Softmask Version of the card */
	std::string m_AppletVersion;			/**< The Applet Version of the card */
	std::string m_GlobalOsVersion;			/**< The Global Os Version of the card */
	std::string m_AppletInterfaceVersion;	/**< The Applet Interface Version of the card */
	std::string m_PKCS1Support;				/**< The PKCS#1 Supporte of the card */
	std::string m_KeyExchangeVersion;		/**< The Key Exchange Version of the card */
	std::string m_AppletLifeCicle;			/**< The Applet Life Cicle of the card */

friend 	APL_CardFile_Info *APL_SmartCard::getFileInfo();	/**< This method must access protected constructor */

};
}

#endif //__CARDFILE_H__
