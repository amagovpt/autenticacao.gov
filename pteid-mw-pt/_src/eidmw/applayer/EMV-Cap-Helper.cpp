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

EMVCapHelper::EMVCapHelper(APL_Card *card, const char *new_pin)
{
	const unsigned char a1[] = {0x80, 0xA8, 0x00, 0x00, 0x02, 0x83, 0x00};
	const unsigned char a3[] = {0x80, 0xCA, 0x9F, 0x17, 0x04};
	m_card = card;
	m_new_pin = new_pin;

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
	//m_card->getCalReader()->SendAPDU(CByteArray(a2, sizeof(a2)));
	CByteArray try_counter_ba = m_card->getCalReader()->SendAPDU(CByteArray(a3, sizeof(a3)));

	m_trycounter = try_counter_ba.GetByte(3);

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

	//GetPan();
	GetArqc(0x00);

	otp_struct->pan = m_pan;
	otp_struct->atc = m_atc;
	otp_struct->arqc = m_arqc;
	

	otp_struct->cdol1 = CDOL1;
	otp_struct->pan_seq_nr = PANSEQNUMBER;
	otp_struct->counter = COUNTER;
	otp_struct->pin_try_counter = m_trycounter;

	if (m_pan == NULL || m_arqc == NULL || m_atc == NULL)
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


/*
 * This plaintext PIN block format is documented in EMV spec
 * book 3 - Section 6.5.12.2
 */
char *getEMVPinBlock(const char *pin)
{
	char tmp[2];
	char * buffer = (char*)malloc(8);

	int i = 0;

	buffer[0] = 0x20;
	buffer[0] |= strlen(pin);

	memset(buffer+1, 0xFF, 7);
	int digit = 0;
	int high_nibble = 1;
	for(; i < strlen(pin); i++)
	{
		tmp[0] = pin[i];
		tmp[1] = 0;
		digit = atoi(tmp);
		int pos = i/2;

		if (high_nibble)
		{
			buffer[pos+1] &= 0x0F;
			buffer[pos+1] |= (digit << 4);
		}
		else
		{
			buffer[pos +1] &= 0xF0;
			buffer[pos +1] |= digit;
		}
		high_nibble = !high_nibble;


	}
	
	return buffer;

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

	//Zero-out the whole struct, just in case
	memset(otp_struct, 0, sizeof(OTPParams));

	const unsigned char verify_apdu[] = {
		0x00, 0x20, 0x00, 0x80, 0x08
	};
	CByteArray verify_apdu_ba(verify_apdu, sizeof(verify_apdu));
	char * pin_block = getEMVPinBlock(m_new_pin);

	verify_apdu_ba.Append((const unsigned char*)pin_block, 8);

	m_card->getCalReader()->SendAPDU(verify_apdu_ba);
	//GetPan();
	GetArqc(0x80);

	otp_struct->pan = m_pan;
	otp_struct->atc = m_atc;
	otp_struct->arqc = m_arqc;

	otp_struct->cdol1 = CDOL1;
	otp_struct->pan_seq_nr = PANSEQNUMBER;
	otp_struct->counter = COUNTER;
	otp_struct->pin_try_counter = m_trycounter;
}

void EMVCapHelper::GetArqc(unsigned char p1)
{
	const unsigned char apdu[] = {0x80, 0xAE, p1, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x34, 0x00, 0x00, 0x01, 0x00, 0x01};

	CByteArray askarqc(apdu, sizeof(apdu));

tryagain:

	CByteArray arqc_response = m_card->getCalReader()->SendAPDU(askarqc);
	
	const unsigned char * resp = arqc_response.GetBytes();
	int len = arqc_response.Size();

	/* Not sure about this...
	//Handle SW12=6985
	if (resp[len-2] == 0x69 && resp[len-1] == 0x85)
	{
		const unsigned char verify_apdu[] = {
			0x00, 0x20, 0x00, 0x80, 0x08
		};
		CByteArray verify_apdu_ba(verify_apdu, sizeof(verify_apdu));
		char * pin_block = getEMVPinBlock(m_old_pin);

		verify_apdu_ba.Append((const unsigned char*)pin_block, 8);

		fprintf(stderr, "EMV Verify PIN and then retry...\n");

		CByteArray verify_resp = m_card->getCalReader()->SendAPDU(verify_apdu_ba);

		if (checkSW12(verify_resp))
			goto tryagain;
		else
		{
			fprintf(stderr, "VERIFY Failed! \n");
			return;
		}
	}

	*/
	
	//We could try to properly parse the answer according to the scarce documentation
	//but oh well, these offsets will never change in a million years...

	std::string tmp = arqc_response.GetBytes(14, 8).ToString(false);
	strcpy(m_arqc, tmp.c_str());

	tmp = arqc_response.GetBytes(9, 2).ToString(false);
	strcpy(m_atc, tmp.c_str());

}

}
