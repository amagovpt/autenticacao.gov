/*
 * EMV-Cap-Helper.cpp
 *
 *  Created on: 2011/10/24
 *      Author: Luis Medinas <luis.medinas@caixamagica.pt>
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

#include "EMV-Cap-Helper.h"

namespace eIDMW
{

EMVCapHelper::EMVCapHelper(APL_SmartCard *card)
{
	std::cout << "EMVCapHelper" << std::endl;
	m_card=card;

	CByteArray oresp,oresp2;
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
	oresp2 = m_card->getCalReader()->SendAPDU(oresp);
}

EMVCapHelper::~EMVCapHelper()
{

}
CByteArray EMVCapHelper::GetPan()
{
	CByteArray osecpan, osecpansend, osecpanresp, osecpanrespget;
	osecpan.Append(0x80);
	osecpan.Append(0xA8);
	osecpan.Append(0x00);
	osecpan.Append(0x00);
	osecpan.Append(0x02);
	osecpan.Append(0x83);
	osecpan.Append(0x00);

	osecpansend = m_card->getCalReader()->SendAPDU(osecpan);

	osecpanresp.Append(0x00);
	osecpanresp.Append(0xB2);
	osecpanresp.Append(0x01);
	osecpanresp.Append(0x0C);
	osecpanresp.Append(0x5F);

	osecpanrespget = m_card->getCalReader()->SendAPDU(osecpanresp);

	CByteArray pannr = osecpanrespget.GetBytes(21,8);
	std::cout << "PAN " << pannr.ToString() << std::endl;

	return pannr;
}

CByteArray EMVCapHelper::GetArqc()
{
	const unsigned char apdu[] = {0x80, 0xAE, 0x80, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x34, 0x00, 0x00, 0x01, 0x00, 0x01};

	GetPan();

	CByteArray askarqc;
	askarqc.Append(apdu, sizeof(apdu));

	CByteArray aer= m_card->getCalReader()->SendAPDU(askarqc);
	CByteArray arqnr = aer.GetBytes(15,8);
	std::cout << "ARQC  " << arqnr.ToString() << std::endl;

	return arqnr;

}

}
