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

/**
 * Each Card subclass can return it's own implementation of
 * this CP15Correction interface in order to correct/create
 * the PKCS15 info that is/is not present in the card.
 * That implementation will be called in the PkCS15.cpp
 * class just after (trying to) read/parse the PKCS15
 * info about the PINs, keys and certs.
 */
#ifndef P15CORRECTION_H
#define P15CORRECTION_H

#include "../common/Export.h"
#include "P15Objects.h"
#include <vector>

namespace eIDMW
{
  class EIDMW_CAL_API CP15Correction
  {
  public:
    virtual ~CP15Correction() = 0;
    virtual void CheckPINs(std::vector<tPin>& pinVector) = 0;
    virtual void CheckCerts(std::vector<tCert>& certVector) = 0;
    virtual void CheckPrKeys(std::vector<tPrivKey>& keyVector) = 0;

  private:

  };
}
#endif
