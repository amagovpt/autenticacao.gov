/*
 * EMV-Cap-Helper.h
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

#ifndef EMVCAPHELPER_H_
#define EMVCAPHELPER_H_

namespace eIDMW
{

#define ATC "0002"
#define PANSEQNUMBER "00"
#define CDOL1 "0000000000000000000000000000800000000000000000000000000000"

class EMVCapHelper
{
public:
	EMVCapHelper(APL_SmartCard *card);
	/**
	  * Destructor
	  */
	~EMVCapHelper();

	CByteArray GetPan();
	CByteArray GetArqc();
private:
	APL_SmartCard *m_card;
};

}
#endif /* EMVCAPHELPER_H_ */
