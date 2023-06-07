/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
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
#include "APLCrypto.h"
#include "APLCard.h"
#include "APLCardPteid.h"
#include "APLReader.h"
#include "cryptoFwkPteid.h"
#include "CardPteidDef.h"
#include "Reader.h"
#include "MiscUtil.h"
#include "../common/Util.h"

namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- APL_Pins -------------------------------------------
*****************************************************************************************/
APL_Pins::APL_Pins(APL_SmartCard *card)
{
	m_card=card;

	unsigned long ulCount=m_card->pinCount();
	for(unsigned long i=0;i<ulCount;i++)
		addPin(i);
}

APL_Pins::~APL_Pins()
{
	std::map<unsigned long,APL_Pin *>::const_iterator itr;
	
	itr = m_pins.begin();
	while(itr!=m_pins.end())
	{
		delete itr->second;
		m_pins.erase(itr->first);
		itr = m_pins.begin();
	} 
}

bool APL_Pins::isAllowed()
{
	return true;
}

unsigned long APL_Pins::count(bool bFromCard)
{
	if(bFromCard)
		return m_card->pinCount();
	else
		return (unsigned long)m_pins.size();
}

APL_Pin *APL_Pins::getPinByNumber(unsigned long ulIndex)
{
	std::map<unsigned long,APL_Pin *>::const_iterator itr;

	itr = m_pins.find(ulIndex);
	if (itr!=m_pins.end())
		return m_pins[ulIndex];

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
}

APL_Pin *APL_Pins::getPinByPinRef(unsigned long pinRef)
{
	std::map<unsigned long,APL_Pin *>::const_iterator itr;

	//auth pin on IAS101 is 1
	if (m_card->getType() == APL_CARDTYPE_PTEID_IAS101)
		pinRef = (pinRef & 2 ) == 2 ? pinRef : 1;

	for(itr = m_pins.begin(); itr != m_pins.end(); itr++){
	    if (itr->second->getPinRef() == pinRef)
	    	return itr->second;
	}

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
}

APL_Pin *APL_Pins::addPin(unsigned long ulIndex)
{
	std::map<unsigned long,APL_Pin *>::const_iterator itr;

	itr = m_pins.find(ulIndex);
	if(itr!=m_pins.end())
		return m_pins[ulIndex];

	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for unly one instanciation

		itr = m_pins.find(ulIndex);
		if(itr!=m_pins.end())
			return m_pins[ulIndex];

		APL_Pin *pin=NULL;
		pin = new APL_Pin(m_card, ulIndex);
		m_pins[ulIndex]=pin;

		return pin;
	}

}

/*****************************************************************************************
---------------------------------------- APL_Pin -------------------------------------------
*****************************************************************************************/
APL_Pin::APL_Pin(APL_SmartCard *card, unsigned long ulIndex)
{
	m_card=card;
	m_ulIndex=ulIndex;

	m_signature=NULL;

	m_triesleft=-1;
	m_usagecode=PIN_USG_UNKNOWN;

	m_pinP15=card->getPin(ulIndex);
}

APL_Pin::~APL_Pin()
{
	if(m_signature)
	{
		delete m_signature;
		m_signature=NULL;
	}
}

bool APL_Pin::isAllowed()
{
	return true;
}

unsigned long APL_Pin::getIndex()
{
	return m_ulIndex;
}

unsigned long APL_Pin::getPinRef(){
	return m_pinP15.ulPinRef;
}

unsigned long APL_Pin::getType()
{
	return m_pinP15.ulPinType;
}

unsigned long APL_Pin::getId()
{
	return m_pinP15.ulID;
}

PinUsage APL_Pin::getUsageCode()
{

		BEGIN_CAL_OPERATION(m_card)
		m_usagecode = m_card->getCalReader()->GetPinUsage(m_pinP15);
		END_CAL_OPERATION(m_card)
	

	return m_usagecode;
}

long APL_Pin::getTriesLeft()
{
	
	unsigned long status=PIN_STATUS_UNKNOWN;

	try
	{
		status=m_card->pinStatus(m_pinP15);
	}
	catch(CMWException & e)
	{
		unsigned long err = e.GetError();
		if (err != EIDMW_ERR_NOT_SUPPORTED)
			throw e;
	}

	if(status==PIN_STATUS_UNKNOWN)
		m_triesleft=-1;
	else
		m_triesleft=status;

	return m_triesleft;
}

bool APL_Pin::isVerified()
{
	return m_card->isPinVerified(m_pinP15);
}

unsigned long APL_Pin::getFlags()
{
	return m_pinP15.ulPinFlags;
}

const char *APL_Pin::getLabel()
{
	return m_pinP15.csLabel.c_str();
}

bool APL_Pin::verifyPin(const char *csPin,unsigned long &ulRemaining,bool bShowDlg, void *wndGeometry )
{

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	return m_card->pinCmd(PIN_OP_VERIFY, m_pinP15, csPin, "",ulRemaining, bShowDlg, wndGeometry );

}

bool APL_Pin::changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName,bool bShowDlg, void *wndGeometry )
{

	return m_card->pinCmd(PIN_OP_CHANGE,m_pinP15,csPin1,csPin2,ulRemaining, bShowDlg, wndGeometry );
}


bool APL_Pin::unlockPin(const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft, unsigned long flags) {

	if (m_card->getType() == APL_CARDTYPE_PTEID_IAS07){ //gemsafe
		return m_card->getCalReader()->unlockPIN(m_pinP15, NULL, pszPuk, pszNewPin, triesLeft, flags);

	} else if (m_card->getType() == APL_CARDTYPE_PTEID_IAS101){ //ias
		tPin puk;
		for (unsigned long idx=0; idx < m_card->pinCount(); idx++){
			puk = m_card->getPin(idx); // get the puk for this pin
			if (((puk.ulPinRef & 0x0F) == (m_pinP15.ulPinRef & 0x0F)) && puk.ulPinRef > m_pinP15.ulPinRef){
				return m_card->getCalReader()->unlockPIN(m_pinP15, &puk, pszPuk, pszNewPin, triesLeft, flags);
			}
		}
	}

	return true;
}


}
