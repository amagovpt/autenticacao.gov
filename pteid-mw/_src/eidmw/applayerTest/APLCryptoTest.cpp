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
#include "UnitTest++/src/UnitTest++.h"
#include "../applayer/APLCrypto.h"
#include "../applayer/APLCardPTeid.h"
#include "../applayer/APLReader.h"

using namespace eIDMW;

TEST(PinCount)
{
	const char * const *list=AppLayer.readerList();
	APL_ReaderContext &reader=AppLayer.getReader(list[0]);	

	APL_EIDCard *card=reader.getEIDCard();				

	CHECK_EQUAL(1,card->pinCount());

	APL_Pins *pins=card->getPins();

	CHECK_EQUAL(1,pins->count());
}

