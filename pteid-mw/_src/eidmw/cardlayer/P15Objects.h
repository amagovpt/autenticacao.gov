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
#ifndef P15OBJECTS_H
#define P15OBJECTS_H

#include <string>
#include "CardLayerConst.h"

namespace eIDMW
{
  // common object flags
  const unsigned long COFLAG_PRIVATE    = 0x00000001;
  const unsigned long COFLAG_MODIFIABLE = 0x00000002;

  // A card-independent way to indentify cards
  typedef enum {
    PIN_USG_UNKNOWN,
    DLG_USG_PIN_AUTH,
    DLG_USG_PIN_SIGN,
    DLG_USG_PIN_ADDRESS,
  } PinUsage;

  // pin flags
  const unsigned long PINFLAG_CASE_SENSITIVE            = 0x00000001;
  const unsigned long PINFLAG_LOCAL                     = 0x00000002;
  const unsigned long PINFLAG_CHANGE_DISABLED           = 0x00000004;
  const unsigned long PINFLAG_UNBLOCK_DISABLED          = 0x00000008;
  const unsigned long PINFLAG_INITIALIZED               = 0x00000010;
  const unsigned long PINFLAG_NEEDS_PADDING             = 0x00000020;
  const unsigned long PINFLAG_UNBLOCKINGPIN             = 0x00000040;
  const unsigned long PINFLAG_SOPIN                     = 0x00000080;
  const unsigned long PINFLAG_DISABLE_ALLOWED           = 0x00000100;
  const unsigned long PINFLAG_INTEGRITY_PROTECTED       = 0x00000200;
  const unsigned long PINFLAG_CONFIDENTIALITY_PROTECTED = 0x00000400;
  const unsigned long PINFLAG_EXCHANGEREFDATA           = 0x00000800;

  // key usage flags
  const unsigned long KEYUFLAG_ENCRYPT        = 0x00000001;
  const unsigned long KEYUFLAG_DECRYPT	      = 0x00000002;
  const unsigned long KEYUFLAG_SIGN	      = 0x00000004;
  const unsigned long KEYUFLAG_SIGNRECOVER    = 0x00000008;
  const unsigned long KEYUFLAG_WRAP	      = 0x00000010;
  const unsigned long KEYUFLAG_UNWRAP	      = 0x00000020;
  const unsigned long KEYUFLAG_VERIFY	      = 0x00000040;
  const unsigned long KEYUFLAG_VERIFYRECOVER  = 0x00000080;
  const unsigned long KEYUFLAG_DERIVE	      = 0x00000100;
  const unsigned long KEYUFLAG_NONREPUDIATION = 0x00000200;

  // key access flags 
  const unsigned long KEYAFLAG_SENSITIVE        = 0x00000001;
  const unsigned long KEYAFLAG_EXTRACTABLE      = 0x00000002;
  const unsigned long KEYAFLAG_ALWAYSSENSITIVE  = 0x00000004;
  const unsigned long KEYAFLAG_NEVEREXTRACTABLE = 0x00000008;
  const unsigned long KEYAFLAG_LOCAL	        = 0x00000010;

  const unsigned long NO_ID = 0xFFFFFFFF;

  typedef struct
  {
    bool bValid;
    // common object attributes
    std::string csLabel;
    unsigned long ulFlags;
    unsigned long ulAuthID;
    unsigned long ulUserConsent;

    // pin attributes
    unsigned long ulID;
    unsigned long ulPinFlags;
    unsigned long ulPinType;
    unsigned long ulMinLen;
    unsigned long ulStoredLen;
    unsigned long ulMaxLen;  
    unsigned long ulPinRef;  
    unsigned char ucPadChar;
    tPinEncoding encoding;
    std::string csLastChange;
    std::string csPath;
  } tPin;

  typedef struct
  {
    bool bValid;
    // common object attributes
    std::string csLabel;
    unsigned long ulFlags;
    unsigned long ulAuthID;
    unsigned long ulUserConsent;

    // common key attributes
    unsigned long ulID;
    unsigned long ulKeyUsageFlags;
    unsigned long ulKeyAccessFlags;
    unsigned long ulKeyRef;

    // private RSA key attributes
    std::string csPath;
    unsigned long ulKeyLenBytes;

    bool bUsedInP11;
  } tPrivKey;

  typedef struct
  {
    bool bValid;
    // common object attributes
    std::string csLabel;
    unsigned long ulFlags;
    unsigned long ulAuthID;
    unsigned long ulUserConsent;

    // common certificate attributes
    unsigned long ulID;   
    bool bAuthority;
    bool bImplicitTrust;

    // X509 certificate attributes
    std::string csPath;

  } tCert;

}
#endif
