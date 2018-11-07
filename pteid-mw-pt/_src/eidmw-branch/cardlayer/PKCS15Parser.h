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
#ifndef __PKCS15PARSER_H__
#define __PKCS15PARSER_H__

#include "../common/ByteArray.h"
#include <vector>
#include "P15Objects.h"

namespace eIDMW
{

  typedef struct{
    int iRef;
    // add: id, parameters, supported operations, algID
    int iAlgRef;
  } tAlgoInfo;

  typedef struct
  {
    //CByteArray oAppID;
    std::string oAppID;
    std::string csAppPath;
    std::string csLabel;
  } tDirInfo;

  typedef struct
  {
    int iVersion;
    std::string     csSerial;
    std::string     csLabel;
    std::string     csManufactID;
    unsigned long   ulFlags; // readonly, login required, prn generation, eid compliant
    int             iAppVersion;
    int             iSecEnv;            // security env. info
    std::string     csOwner;    // security env. info
    int             piRecordInfo[7];    // record info 
    std::vector< tAlgoInfo > oAlgoInfo; // array of algorithm info
    std::string     csIssuer;
    std::string     csHolder;
    std::string     csLastUpdate;
  } tTokenInfo;

  typedef struct
  {
    std::string csPrkdfPath; // contains the private key info
    std::string csPukdfPath; // contains the public key info
    std::string csTrustedPukdfPath; 
    std::string csSecretPukdfPath;
    std::string csCdfPath;   // contains the cert info
    std::string csTrustedCdfPath;  
    std::string csUsefulCdfPath;  
    std::string csDodfPath;   // contains the cert info
    std::string csAodfPath;  // contains the PIN info
    
  } tOdfInfo;

  typedef struct{
    std::string csLabel;
    unsigned long ulFlags;
    unsigned long ulAuthID;
    unsigned long ulUserConsent;

    int lastPosition;

    void Reset(){
      csLabel = "";
      ulFlags = 0x0;
      ulAuthID = 0x0;
      ulUserConsent = 0x0;
      lastPosition = 0;
    }
  } tCommonObjAttr;

  class PKCS15Parser
  {
  public:
    tDirInfo ParseDir(const CByteArray & contents);

    tTokenInfo ParseTokenInfo(const CByteArray & contents);

    tOdfInfo ParseOdf(const CByteArray & contents);

    std::vector <tPin> ParseAodf(const CByteArray & contents);

    std::vector <tPrivKey> ParsePrkdf(const CByteArray & contents);

    std::vector <tCert> ParseCdf(const CByteArray & contents);

  private:

	void FillCOAPin(tPin* pin,tCommonObjAttr *coa);
    void FillCOAPrKey(tPrivKey* key,tCommonObjAttr *coa);
    void FillCOACert(tCert* cert,tCommonObjAttr *coa);
  };

}

#endif
