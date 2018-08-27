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
#include "APLCrypto.h"
#include "APLCard.h"
#include "APLReader.h"
#include "cryptoFwkPteid.h"
#include "CardPteidDef.h"
#include "Reader.h"
#include "MiscUtil.h"
#include "../common/Util.h"

namespace eIDMW
{
/*****************************************************************************************
---------------------------------------- APL_Crypto -------------------------------------------
*****************************************************************************************/
APL_Crypto::~APL_Crypto()
{
}

/*****************************************************************************************
---------------------------------------- APL_Pins -------------------------------------------
*****************************************************************************************/
APL_Pins::APL_Pins(APL_SmartCard *card)
{
	m_card=card;

	unsigned long ulCount=m_card->pinCount();
	for(unsigned long i=0;i<ulCount;i++)
		addPin(i,NULL);
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

CByteArray APL_Pins::getXML(bool bNoHeader)
{
/*
	<pins count=��>
		<pin>
		</pin>
	</pins>
*/
	char buffer[50];

	CByteArray xml;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<pins count=\"";
	sprintf_s(buffer,sizeof(buffer),"%ld",count(false));
	xml+=buffer;
	xml+="\">\n";
	for(unsigned long i=0;i<count(false);i++)
	{
		xml+=getPinByNumber(i)->getXML(true);
	}
	xml+="</pins>\n";

	return xml;
}

CByteArray APL_Pins::getCSV()
{
/*
pinscount;pin1;pin2;...
*/
	char buffer[50];
	CByteArray csv;

	sprintf_s(buffer,sizeof(buffer),"%ld",count(false));
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	for(unsigned long i=0;i<count(false);i++)
	{
		csv+=getPinByNumber(i)->getCSV();
		csv+=CSV_SEPARATOR;
	}

	return csv;
}

CByteArray APL_Pins::getTLV()
{
	//First we add all the pins in a tlv
	CTLVBuffer tlvNested;

	CByteArray baCount;
	baCount.AppendLong(count(false));
	tlvNested.SetTagData(0x00,baCount.GetBytes(),baCount.Size());	//Tag 0x00 contain the number of pins
	
	unsigned char j=1;
	for(unsigned long i=0;i<count(false);i++)
	{
		APL_Pin *pin=getPinByNumber(i);
		CByteArray baPin=pin->getTLV();
		tlvNested.SetTagData(j++,baPin.GetBytes(),baPin.Size());
	}

	unsigned long ulLen=tlvNested.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlvNested.Extract(pucData,ulLen);
	CByteArray baPins(pucData,ulLen);

	delete[] pucData;

	//We nest the tlv into the enclosing tlv
	CTLVBuffer tlv;
	tlv.SetTagData(PTEID_TLV_TAG_FILE_PINS,baPins.GetBytes(),baPins.Size());

	ulLen=tlv.GetLengthNeeded();
	pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;
	
	return ba;
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

APL_Pin *APL_Pins::addPin(unsigned long ulIndex,const CByteArray *pin_tlv_struct)
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
		pin = new APL_Pin(m_card,ulIndex,pin_tlv_struct);
		m_pins[ulIndex]=pin;

		return pin;
	}

}
/*****************************************************************************************
---------------------------------------- APL_Pin -------------------------------------------
*****************************************************************************************/
APL_Pin::APL_Pin(APL_SmartCard *card,unsigned long ulIndex,const CByteArray *pin_tlv_struct)
{
	m_card=card;
	m_ulIndex=ulIndex;

	m_signature=NULL;

	m_triesleft=-1;
	m_usagecode=PIN_USG_UNKNOWN;

	if(pin_tlv_struct)
		setP15TLV(pin_tlv_struct);
	else
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

CByteArray APL_Pin::getXML(bool bNoHeader)
{
/*
	<pin>
		<type></type>
		<id></id>
		<usage_code></usage_code>
		<tries_left></tries_left>
		<flags></flags>
		<label></label>
		<p15_struct></p15_struct>
	</pin>
*/

	char buffer[50];
	CByteArray xml;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<pin>\n";

	xml+="	<type>";
		sprintf_s(buffer,sizeof(buffer),"%ld",getType());
		xml+=buffer;
	xml+=	"</type>\n";

	xml+="	<id>";
		sprintf_s(buffer,sizeof(buffer),"%ld",getId());
		xml+=buffer;
	xml+=	"</id>\n";

	xml+="	<usage_code>";
		sprintf_s(buffer,sizeof(buffer),"%ld",getUsageCode());
		xml+=buffer;
	xml+=	"</usage_code>\n";

	xml+="	<tries_left>";
		sprintf_s(buffer,sizeof(buffer),"%ld",getTriesLeft());
		xml+=buffer;
	xml+=	"</tries_left>\n";

	xml+="	<flags>";
		sprintf_s(buffer,sizeof(buffer),"%ld",getFlags());
		xml+=buffer;
	xml+=	"</flags>\n";

	xml+="	<label>";
	xml+=		getLabel();
	xml+=	"</label>\n";
	CByteArray baFileB64;
	if(m_cryptoFwk->b64Encode(getP15TLV(),baFileB64,false))
	{
		xml+="	<p15_struct>";
		xml+=baFileB64;
		xml+=	"</p15_struct>\n";
	}

	xml+="</pin>\n";

	return xml;
}

CByteArray APL_Pin::getCSV()
{
/*
type;id;usage_code;tries_left;flags;label;p15_struct
*/
	char buffer[50];
	CByteArray csv;

	sprintf_s(buffer,sizeof(buffer),"%ld",getType());
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	sprintf_s(buffer,sizeof(buffer),"%ld",getId());
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	sprintf_s(buffer,sizeof(buffer),"%ld",getUsageCode());
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	sprintf_s(buffer,sizeof(buffer),"%ld",getTriesLeft());
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	sprintf_s(buffer,sizeof(buffer),"%ld",getFlags());
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	csv+=getLabel();
	csv+=CSV_SEPARATOR;

	CByteArray baFileB64;
	if(m_cryptoFwk->b64Encode(getP15TLV(),baFileB64,false))
		csv+=baFileB64;
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_Pin::getTLV()
{
	CTLVBuffer tlv;

	CByteArray baP15=getP15TLV();
	tlv.SetTagData(0x00,baP15.GetBytes(),baP15.Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray baPin(pucData,ulLen);

	delete[] pucData;
	
	return baPin;
}

CByteArray APL_Pin::getP15TLV()
{
	CTLVBuffer tlv;
/*
	typedef struct
	{
		bool bValid;					0x00
		std::string csLabel;			0x01
		unsigned long ulFlags;			0x02
		unsigned long ulAuthID;			0x03
		unsigned long ulUserConsent;	0x04
		unsigned long ulID;				0x05
		unsigned long ulPinFlags;		0x06
		unsigned long ulPinType;		0x07
		unsigned long ulMinLen;			0x08
		unsigned long ulStoredLen;		0x09
		unsigned long ulMaxLen;			0x0A
		unsigned long ulPinRef;			0x0B
		unsigned char ucPadChar;		0x0C
		tPinEncoding encoding;			0x0D
		std::string csLastChange;		0x0E
		std::string csPath;				0x0F
	} tPin;
	+	m_usagecode						0x10
	+	m_triesleft						0x11
*/
	char buffer[50];

	sprintf_s(buffer,sizeof(buffer),"%d",m_pinP15.bValid);
	tlv.SetTagData(0x00,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%s",m_pinP15.csLabel.c_str());
	tlv.SetTagData(0x01,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulFlags);
	tlv.SetTagData(0x02,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulAuthID);
	tlv.SetTagData(0x03,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulUserConsent);
	tlv.SetTagData(0x04,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulID);
	tlv.SetTagData(0x05,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulPinFlags);
	tlv.SetTagData(0x06,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulPinType);
	tlv.SetTagData(0x07,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulMinLen);
	tlv.SetTagData(0x08,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulStoredLen);
	tlv.SetTagData(0x09,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulMaxLen);
	tlv.SetTagData(0x0A,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.ulPinRef);
	tlv.SetTagData(0x0B,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%c",m_pinP15.ucPadChar);
	tlv.SetTagData(0x0C,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_pinP15.encoding);
	tlv.SetTagData(0x0D,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%s",m_pinP15.csLastChange.c_str());
	tlv.SetTagData(0x0E,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%s",m_pinP15.csPath.c_str());
	tlv.SetTagData(0x0F,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",getUsageCode());
	tlv.SetTagData(0x10,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",getTriesLeft());
	tlv.SetTagData(0x11,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray baPin(pucData,ulLen);

	delete[] pucData;
	
	return baPin;

}

void APL_Pin::setP15TLV(const CByteArray *pin_tlv_struct)
{
/*
	typedef struct
	{
		bool bValid;					0x00
		std::string csLabel;			0x01
		unsigned long ulFlags;			0x02
		unsigned long ulAuthID;			0x03
		unsigned long ulUserConsent;	0x04
		unsigned long ulID;				0x05
		unsigned long ulPinFlags;		0x06
		unsigned long ulPinType;		0x07
		unsigned long ulMinLen;			0x08
		unsigned long ulStoredLen;		0x09
		unsigned long ulMaxLen;			0x0A
		unsigned long ulPinRef;			0x0B
		unsigned char ucPadChar;		0x0C
		tPinEncoding encoding;			0x0D
		std::string csLastChange;		0x0E
		std::string csPath;				0x0F
	} tPin;
	+	m_usagecode						0x10
	+	m_triesleft						0x11
*/
	char *stop;
	char cBuffer[250];
	unsigned long ulLen=0;
	CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(pin_tlv_struct->GetBytes(), pin_tlv_struct->Size());

	//bValid
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x00, cBuffer, &ulLen);
	m_pinP15.bValid=(strcmp(cBuffer,"1")==0?true:false);

	//csLabel
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x01, cBuffer, &ulLen);
	m_pinP15.csLabel.clear();
	m_pinP15.csLabel.append(cBuffer);

	//ulFlags
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x02, cBuffer, &ulLen);
	m_pinP15.ulFlags=strtoul(cBuffer,&stop,10);

	//ulAuthID
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x03, cBuffer, &ulLen);
	m_pinP15.ulAuthID=strtoul(cBuffer,&stop,10);

	//ulUserConsent
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x04, cBuffer, &ulLen);
	m_pinP15.ulUserConsent=strtoul(cBuffer,&stop,10);

	//ulID
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x05, cBuffer, &ulLen);
	m_pinP15.ulID=strtoul(cBuffer,&stop,10);

	//ulPinFlags
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x06, cBuffer, &ulLen);
	m_pinP15.ulPinFlags=strtoul(cBuffer,&stop,10);

	//ulPinType
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x07, cBuffer, &ulLen);
	m_pinP15.ulPinType=strtoul(cBuffer,&stop,10);

	//ulMinLen
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x08, cBuffer, &ulLen);
	m_pinP15.ulMinLen=strtoul(cBuffer,&stop,10);

	//ulStoredLen
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x09, cBuffer, &ulLen);
	m_pinP15.ulStoredLen=strtoul(cBuffer,&stop,10);

	//ulMaxLen
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x0A, cBuffer, &ulLen);
	m_pinP15.ulMaxLen=strtoul(cBuffer,&stop,10);

	//ulPinRef
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x0B, cBuffer, &ulLen);
	m_pinP15.ulPinRef=strtoul(cBuffer,&stop,10);

	//ucPadChar
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x0C, cBuffer, &ulLen);
	m_pinP15.ucPadChar=cBuffer[0];

	//encoding
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x0D, cBuffer, &ulLen);
	m_pinP15.encoding=(tPinEncoding)strtoul(cBuffer,&stop,10);

	//csLastChange
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x0E, cBuffer, &ulLen);
	m_pinP15.csLastChange.clear();
	m_pinP15.csLastChange.append(cBuffer);

	//csPath
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x0F, cBuffer, &ulLen);
	m_pinP15.csPath.clear();
	m_pinP15.csPath.append(cBuffer);

	//usagecode
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x10, cBuffer, &ulLen);
	m_usagecode=(PinUsage)strtoul(cBuffer,&stop,10);

	//triesleft
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x11, cBuffer, &ulLen);
	m_triesleft=strtol(cBuffer,&stop,10);
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

	return m_card->pinCmd(PIN_OP_VERIFY,m_pinP15,csPin,"",ulRemaining,bShowDlg, wndGeometry );

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
