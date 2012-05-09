/* ****************************************************************************
 *
 *  PTeID Middleware Project.
 *  Copyright (C) 2011-2012
 *  Luis Medinas <lmedinas@gmail.com>
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 */

/*
 *  pteid-mw-ng is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  pteid-mw-ng is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include <iostream>
#include "APLCard.h"
#include "APLReader.h"
#include "Reader.h"
#include "Log.h"

#include "EMV-Cap-Helper.h"

namespace eIDMW
{

EMVCapHelper::EMVCapHelper(APL_Card *card)
{
	const unsigned char a1[] = {0x80, 0xA8, 0x00, 0x00, 0x02, 0x83, 0x00};
	const unsigned char a2[] = {0x80, 0xCA, 0x9F, 0x17, 0x08};
	const unsigned char a3[] = {0x80, 0xCA, 0x9F, 0x17, 0x04};
	m_card = card;

	CByteArray oresp;
	oresp.Append(0x00);
	oresp.Append(0xA4);
	oresp.Append(0x04);
	oresp.Append(0x00);
	oresp.Append(0x07);
	oresp.Append(0x60);
	oresp.Append(0x46);
	oresp.Append(0x32);
	oresp.Append(0xFF);
	oresp.Append(0x00);
	oresp.Append(0x00);
	oresp.Append(0x01);
	oresp = m_card->getCalReader()->SendAPDU(oresp);

	if (!checkSW12(oresp))
	{
	MWLOG(LEV_ERROR, MOD_APL, L"Failed to Select EMV-CAP Applet! We're probably dealing with an unknown/unsupported card.");
	}
	m_card->getCalReader()->SendAPDU(CByteArray(a1, sizeof(a1)));
	m_card->getCalReader()->SendAPDU(CByteArray(a2, sizeof(a2)));
	m_card->getCalReader()->SendAPDU(CByteArray(a3, sizeof(a3)));

	m_pan = (char *)malloc(30);
	m_arqc = (char *)malloc(20);
	m_atc = (char *)malloc(10);

}

bool EMVCapHelper::checkSW12(CByteArray &in)
{

	unsigned long ulRespLen = in.Size();

	unsigned int ulSW12 = (unsigned int)(256 * in.GetByte(ulRespLen - 2)
		       	+ in.GetByte(ulRespLen - 1));

	return ulSW12 == 0x9000;

}


bool EMVCapHelper::getOtpParams(OTPParams *otp_struct)
{

	GetPan();
	GetArqc(0x00);

	otp_struct->pan = m_pan;
	otp_struct->atc = m_atc;
	otp_struct->arqc = m_arqc;
	

	otp_struct->cdol1 = CDOL1;
	otp_struct->pan_seq_nr = PANSEQNUMBER;
	otp_struct->counter = COUNTER;
	otp_struct->pin_try_counter = PINTRYCOUNTER;

	if (m_pan == NULL || m_arqc == NULL || m_pan == NULL)
		return false;
	else 
		return true;

}

EMVCapHelper::~EMVCapHelper()
{

	free(m_pan);
	free(m_arqc); 
	free(m_atc); 
}

void EMVCapHelper::GetPan()
{
	CByteArray osecpanresp;

	osecpanresp.Append(0x00);
	osecpanresp.Append(0xB2);
	osecpanresp.Append(0x01);
	osecpanresp.Append(0x0C);
	osecpanresp.Append(0x5F);

	CByteArray osecpanrespget = m_card->getCalReader()->SendAPDU(osecpanresp);

	std::string pan_tmp = osecpanrespget.GetBytes(21,8).ToString(false);

	strcpy(m_pan, pan_tmp.c_str());

}

char *EMVCapHelper::changeCapPin(char * change_apdu_str)
{

	//Create APDU from hex-encoded string
	CByteArray change_pin_apdu(std::string(change_apdu_str), true);
	char *resp = (char *)malloc(5);

	CByteArray bresp = m_card->getCalReader()->SendAPDU(change_pin_apdu);

	unsigned long ulRespLen = bresp.Size();

	unsigned int ulSW12 = (unsigned int)(256 * bresp.GetByte(ulRespLen - 2) + bresp.GetByte(ulRespLen - 1));
	sprintf(resp, "%04x", ulSW12);

	return resp;

}

char *EMVCapHelper::resetScriptCounter(char * cdol2)
{
	//Example: 80 AE 40 00 11 00 00 00 00 00 00 00 00 00 00 5A 33 80 00 00 00 00
	const unsigned char apdu_header[] = {0x80, 0xAE, 0x40, 0x00, 0x11};
	//Create APDU from hex-encoded string
	CByteArray reset_counter_apdu;

	if (cdol2 == NULL)
	{
		MWLOG(LEV_DEBUG, MOD_APL, L"resetScriptCounter called with cdol2 = NULL, ignoring the request...");
		return "9000";

	}
	CByteArray body_apdu(std::string(cdol2), true);
	char *resp = (char *)malloc(5);

	reset_counter_apdu.Append(apdu_header, sizeof(apdu_header));
	reset_counter_apdu.Append(body_apdu);

	CByteArray bresp = m_card->getCalReader()->SendAPDU(reset_counter_apdu);

	unsigned long ulRespLen = bresp.Size();

	unsigned int ulSW12 = (unsigned int)(256 * bresp.GetByte(ulRespLen - 2) + bresp.GetByte(ulRespLen - 1));
	sprintf(resp, "%04x", ulSW12);

	return resp;

}

void EMVCapHelper::getOnlineTransactionParams(OTPParams *otp_struct)
{
	const unsigned char verify_apdu[] = {
		0x00, 0x20, 0x00, 0x80, 0x08, 0x24, 0x12, 0x34, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	//Zero-out the whole struct, just in case
	memset(otp_struct, 0, sizeof(OTPParams));

	CByteArray verify;
	verify.Append(verify_apdu, sizeof(verify_apdu));

	m_card->getCalReader()->SendAPDU(verify);
	GetPan();
	GetArqc(0x80);

	otp_struct->pan = m_pan;
	otp_struct->atc = m_atc;
	otp_struct->arqc = m_arqc;

	otp_struct->cdol1 = CDOL1;
	otp_struct->pan_seq_nr = PANSEQNUMBER;
	otp_struct->counter = COUNTER;
	otp_struct->pin_try_counter = PINTRYCOUNTER;
}

void EMVCapHelper::GetArqc(unsigned char p1)
{
	const unsigned char apdu[] = {0x80, 0xAE, p1, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x34, 0x00, 0x00, 0x01, 0x00, 0x01};

	CByteArray askarqc;
	askarqc.Append(apdu, sizeof(apdu));

	CByteArray aer= m_card->getCalReader()->SendAPDU(askarqc);
	//We could try to properly parse the answer according to the scarce documentation
	//but oh well, these offsets will never change in a million years...

	std::string tmp = aer.GetBytes(14, 8).ToString(false);
	strcpy(m_arqc, tmp.c_str());

	tmp = aer.GetBytes(9, 2).ToString(false);
	strcpy(m_atc, tmp.c_str());

}

}
