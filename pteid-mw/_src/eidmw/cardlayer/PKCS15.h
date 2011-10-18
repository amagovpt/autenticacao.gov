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

#ifndef __PKCS15_H__
#define __PKCS15_H__

#include <vector>

#include "../common/eidErrors.h"
#include "../common/MWException.h"
#include "P15Objects.h"
#include "PKCS15Parser.h"

namespace eIDMW
{
  class CCard;
  class CContext;

  // token flags
  const unsigned long TOKENFLAG_READONLY       = 0x00000001;
  const unsigned long TOKENFLAG_LOGINREQUIRED  = 0x00000002;
  const unsigned long TOKENFLAG_PRNGENERATION  = 0x00000004;
  const unsigned long TOKENFLAG_EIDCOMPLIANT   = 0x00000008;

  typedef struct 
  {
    bool isRead;
    std::string path;
    CByteArray byteArray;

    void setDefault(){
      isRead = false;	
      path = "";
      byteArray.ClearContents();
    }
  } tPKCSFile;

  typedef enum {
    EFDIR,
    ODF,
    TOKENINFO,
    AODF,
    CDF,
    PRKDF,
  } tPKCSFileName;

  class EIDMW_CAL_API CPKCS15
  {	       
  public:      
    CPKCS15(void);
    CPKCS15(CContext *poContext);
    ~CPKCS15(void);

    void Clear(CCard *poCard = NULL);
    void SetCard(CCard *poCard);

    std::string GetSerialNr();
    std::string GetCardLabel();

    unsigned long PinCount();
    tPin GetPin(unsigned long ulIndex);
    tPin GetPinByID(unsigned long ulID);
    tPin GetPinByRef(unsigned long ulPinRef);

    unsigned long CertCount();
    tCert GetCert(unsigned long ulIndex);
    tCert GetCertByID(unsigned long ulID);

    unsigned long PrivKeyCount();
    tPrivKey GetPrivKey(unsigned long ulIndex);
    tPrivKey GetPrivKeyByID(unsigned long ulID);

  private:
    CCard *m_poCard;
    CContext *m_poContext;
    PKCS15Parser *m_poParser;

    std::string m_csSerial;
    std::string m_csLabel;

#ifdef WIN32
// Get rid of warnings like "warning C4251: 'eIDMW::CPKCS15::m_oPins' : class 'std::vector<_Ty>'
// needs to have dll-interface to be used by clients of class 'eIDMW::CPKCS15'"
// See http://groups.google.com/group/microsoft.public.vc.stl/msg/c4dfeb8987d7b8f0
#pragma warning(push)
#pragma warning(disable:4251)
#endif
	std::vector<tPin> m_oPins;
    std::vector<tCert> m_oCertificates;
    std::vector<tPrivKey> m_oPrKeys;
#ifdef WIN32
#pragma warning(pop)
#endif

    tDirInfo m_tDir;

    // files on the card

    tPKCSFile m_xDir;

    tPKCSFile m_xTokenInfo;
    tPKCSFile m_xODF;
    tPKCSFile m_xAODF;
    tPKCSFile m_xCDF;
    tPKCSFile m_xPrKDF;
    tPKCSFile m_xPuKDF;


    tPKCSFile m_xPin;  // there could be more than one
    tPKCSFile m_xPrKey;
    tPKCSFile m_xCert;

    // read methods for level1 (dir file), level 2 (token info and odf) and level 3 (aodf,cdf and prkdf)
    void ReadLevel1();
    void ReadLevel2(tPKCSFileName name);
    void ReadLevel3(tPKCSFileName name);

    void ReadFile(tPKCSFile* pFile,int upperLevel);
  };

}
#endif
