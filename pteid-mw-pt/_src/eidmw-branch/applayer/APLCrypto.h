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

#ifndef __SCCRYPTOUTIL_H__
#define __SCCRYPTOUTIL_H__

#include <string>
#include <map>
#include "Export.h"
#include "APLDoc.h"
#include "APLCard.h"
#include "P15Objects.h"

namespace eIDMW
{
/******************************************************************************//**
  * Abstract class for cryptographic feature
  * Inherits from APL_XMLDoc for implementing buildXML()
  *********************************************************************************/
class APL_Crypto : public APL_XMLDoc
{
public:
	/**
	  * Destructor
	  */
	EIDMW_APL_API virtual ~APL_Crypto()=0;
};

class APL_Pin;
/******************************************************************************//**
  * Class defining all the Pin on a smart card
  *
  * To get a APL_Pins object, we have to ask it from the APL_SmartCard 
  *********************************************************************************/
class APL_Pins : public APL_Crypto
{
public:
	/**
	  * Destructor
	  *
	  * Destroy all the certificate hold in m_certifsAll
	  */
	EIDMW_APL_API virtual ~APL_Pins();

	EIDMW_APL_API virtual bool isAllowed();							/**< The document is allowed*/

	EIDMW_APL_API virtual CByteArray getXML(bool bNoHeader=false);	/**< Build the XML document */
	EIDMW_APL_API virtual CByteArray getCSV();						/**< Build the CSV document */
	EIDMW_APL_API virtual CByteArray getTLV();						/**< Build the TLV document */

	/**
	  * Return the number of P15 pins on the card
	  */
	EIDMW_APL_API unsigned long count(bool bFromCard=true);

	/**
	  * Return the pin with the number ulIndex from the card
	  *
	  * If the APL_Pin object does not yet exist, 
	  * it is create and add to the map m_pins
	  */
	EIDMW_APL_API APL_Pin *getPinByNumber(unsigned long ulIndex);

	/**
	  * Return the pin with the pin reference from the card
	  *
	  * If the pin is not found an exception is thrown
	  */
	EIDMW_APL_API APL_Pin *getPinByPinRef(unsigned long pinRef);

	/**
	  * Add pin in the map for internal use (No need to export)
	  */
	APL_Pin *addPin(unsigned long ulIndex,const CByteArray *pin_tlv_struct);

protected:
	/**
	  * Constructor
	  *
	  * @param card is the smart card from which the pins come
	  */
	APL_Pins(APL_SmartCard *card);

private:
	APL_Pins(const APL_Pins& pins);				/**< Copy not allowed - not implemented */
	APL_Pins &operator= (const APL_Pins& pins);	/**< Copy not allowed - not implemented */

	APL_SmartCard *m_card;						/**< The smart card from which the pins come */

	/**
	  * Hold the APL_Pin pointers to the pins on the card
	  *
	  * The index is the P15 index
	  */
	std::map<unsigned long,APL_Pin *> m_pins;

friend APL_Pins *APL_SmartCard::getPins();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class defining one Pin on a smart card
  *
  * To get APL_Pin object, we have to ask it from APL_Pins 
  *********************************************************************************/
class APL_Pin : public APL_Crypto
{
public:
	/**
	  * Destructor
	  */
	EIDMW_APL_API virtual ~APL_Pin();

	EIDMW_APL_API virtual bool isAllowed();							/**< The document is allowed*/

	EIDMW_APL_API virtual CByteArray getXML(bool bNoHeader=false);	/**< Build the XML document */
	EIDMW_APL_API virtual CByteArray getCSV();						/**< Build the CSV document */
	EIDMW_APL_API virtual CByteArray getTLV();						/**< Build the TLV document */

	/**
	  * Return the remaining tries for giving the good pin
	  *
	  * This opperation is not supported by all card.
	  *
	  * @return -1 if not supported
	  * @return the number of remaining tries in the other case
	  */
	EIDMW_APL_API long getTriesLeft();			
	EIDMW_APL_API unsigned long getIndex();		/**< Return m_ulIndex */
	EIDMW_APL_API unsigned long getPinRef();
	EIDMW_APL_API unsigned long getType();		/**< Return P15 Type */	
	EIDMW_APL_API unsigned long getId();		/**< Return P15 ID */
	EIDMW_APL_API PinUsage getUsageCode();		/**< Return Usage code from the CAL */
	EIDMW_APL_API unsigned long getFlags();		/**< Return P15 Flags */
	EIDMW_APL_API const char *getLabel();		/**< Return P15 Label */
	
	/**
	  * Call the CAL for verifying the pin
	  *
	  * @param csPin is the pin code to verify (if csPin is empty, the CAL will ask for the pin)
	  * @param ulRemaining return the remaining tries (only if verifying failed)
	  *
	  * @return true if success and false if failed
	  */
	EIDMW_APL_API bool verifyPin(const char *csPin,unsigned long &ulRemaining, bool bShowDlg=true, void *wndGeometry = 0 );

	/**
	  * Call the CAL for changing the pin
	  *
	  * if csPin1 or csPin2 are empty, the CAL will ask for the pins
	  *
	  * @param csPin1 is the old pin code
	  * @param csPin2 is the new pin code
	  * @param ulRemaining return the remaining tries (only when operation failed)
	  * @param PinName is the Pin Name
	  *
	  * @return true if success and false if failed
	  */
 	EIDMW_APL_API bool changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName,bool bShowDlg=true, void *wndGeometry = 0 );
	EIDMW_APL_API bool unlockPin(const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft, unsigned long flags);

protected:
	/**
	  * Constructor
	  *
	  * @param card is the smart card from which the pin comes
	  * @param ulIndex is the P15 index of the pin
	  */
	APL_Pin(APL_SmartCard *card,unsigned long ulIndex,const CByteArray *pin_tlv_struct);

private:
	APL_Pin(const APL_Pin& pin);				/**< Copy not allowed - not implemented */
	APL_Pin &operator= (const APL_Pin& pin);	/**< Copy not allowed - not implemented */

	CByteArray getP15TLV();						/**< Return the P15 struct in a TLV CByteArray */
	void setP15TLV(const CByteArray *bytearray);/**< Fill the P15 struct with the TLV CByteArray */

	APL_SmartCard *m_card;						/**< The smart card from which the pin comes */
	unsigned long m_ulIndex;					/**< The P15 index of the pin */
	tPin m_pinP15;								/**< The P15 structure */

	CByteArray *m_signature;					/**< The Pin signature */

	PinUsage m_usagecode;						/**< usage code (for virtual card) */
	long m_triesleft;							/**< tries left (for virtual card) */

friend APL_Pin *APL_Pins::getPinByNumber(unsigned long ulIndex);	/**< This method must access protected constructor */
friend APL_Pin *APL_Pins::addPin(unsigned long ulIndex,const CByteArray *pin_tlv_struct); /**< This method must access protected constructor */
};

}

#endif //__SCCRYPTOUTIL_H__
