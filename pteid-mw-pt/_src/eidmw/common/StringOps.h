/*-****************************************************************************

 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

/*
 * StringOps.h
 *
 *  Created on: Dec 26, 2011
 *      Author: ruim
 */

#pragma once
#include "Export.h"
#include <string>

namespace eIDMW 
{
EIDMW_CMN_API void replace(std::string& str, const std::string& from, const std::string& to);

}

