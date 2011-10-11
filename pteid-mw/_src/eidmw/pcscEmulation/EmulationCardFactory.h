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

#include "EmulationPTeidCard.h"
#include "EmulationSISCard.h"

namespace eIDMW
{

typedef enum {
	PTEID_V1_TEST1,
	PTEID_V1_TEST2,
	PTEID_V1_TEST3,
	PTEID_V2_TEST1,
	SIS_TEST1,
} EmulationCard;

class CEmulationCardFactory
{
public:
	~CEmulationCardFactory();

	static CEmulationCard *getInstance(EmulationCard type);
private:
	static CEmulationCard * MakePTeidV1Test1();
	static CEmulationCard * MakePTeidV1Test2();
	static CEmulationCard * MakePTeidV1Test3();
	static CEmulationCard * MakePTeidV2Test1();
	static void Init();
};

}
