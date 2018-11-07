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
#include "../CardLayerConst.h"

#include "PteidP15Correction.h"


namespace eIDMW
{
  void CPteidP15Correction::CheckPINs(std::vector<tPin>& pinVector) {
    for (std::vector<tPin>::iterator ip = pinVector.begin();
	 ip != pinVector.end(); ++ip){
      // correct the encoding
      if( (*ip).encoding != PIN_ENC_GP ) (*ip).encoding = PIN_ENC_GP;
	  if( (*ip).ulMaxLen != 12 ) (*ip).ulMaxLen = 12;
    }
  };

  void CPteidP15Correction::CheckCerts(std::vector<tCert>& certVector) {
    // do nothing
  };
  
  void CPteidP15Correction::CheckPrKeys(std::vector<tPrivKey>& keyVector) { 
    // do nothing
  };
}
