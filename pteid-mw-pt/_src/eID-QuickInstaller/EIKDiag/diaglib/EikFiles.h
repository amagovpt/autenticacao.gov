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

#ifndef __EIKFILES__
#define __EIKFILES__

#pragma once

#include <windows.h>
#include <string>
#include <vector>

using namespace std;

#include "PCSCCard.h"

static const BYTE EF_CERT_AUTH[] = {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x38
};
static const BYTE EF_CERT_NR[] = {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x39
};
static const BYTE EF_CERT_ROOT[] = {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x3B
};
static const BYTE EF_CERT_RRN[] = {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x3C
};
static const BYTE EF_ID_RN[] = {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x31
};
static const BYTE EF_SIG_ID_RN[] = {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x32
};
static const BYTE EF_ID_ADDRESS[] = {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x33
};
static const BYTE EF_SIG_ID_ADDR[] = {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x34
};
static const BYTE EF_ID_PHOTO[] = {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x35
};
static const BYTE EF_PREFS[] = {0x3F, 0x00, 0xDF, 0x01, 0x40, 0x39
};
static const BYTE EF_TI[] = {0x3F, 0x00, 0xDF, 0x00, 0x50, 0x32
};

class CEikFiles {
public:
    CEikFiles(void);
    virtual ~CEikFiles(void);
    bool Connect(const string& ReaderName);
    bool ReadTI(void);
    bool ReadID(void);
    bool ReadAddress(void);
    bool ReadPhoto(void);
    bool ReadAuthCert(void);
    bool ReadSigCert(void);

    ReaderList& Readers() {
        return _PCSC.Readers();
    };

    AtrString& Atr() {
        return _PCSC.Atr();
    };

    string LastError(void) {
        return _LastErrorMsg;
    };
    // EF(TI)

    Buffer IDTI() {
        return _IDTI;
    };
    // EF(ID#ID)

    string IDKaartNr() {
        return _IDKaartNr;
    };

    string IDChipNR() {
        return _IDChipNR;
    };

    string IDGeldigVan() {
        return _IDGeldigVan;
    };

    string IDGeldigTot() {
        return _IDGeldigTot;
    };

    string IDGemAfgifte() {
        return _IDGemAfgifte;
    };

    string IDNN() {
        return _IDNN;
    };

    string IDTitel() {
        return _IDTitel;
    };

    string IDNaam() {
        return _IDNaam;
    };

    string IDVoornaam1() {
        return _IDVoornaam1;
    };

    string IDVoornaam2() {
        return _IDVoornaam2;
    };

    string IDVoornamen() {
        return _IDVoornamen;
    };

    string IDNat() {
        return _IDNat;
    };

    string IDGeslacht() {
        return _IDGeslacht;
    };

    string IDGebDatum() {
        return _IDGebDatum;
    };

    string IDGebPlaats() {
        return _IDGebPlaats;
    };
    // EF(ID#ADRES) 

    string IDStraat() {
        return _IDStraat;
    };

    string IDGemeente() {
        return _IDGemeente;
    };

    string IDPostcode() {
        return _IDPostcode;
    };

    string IDDocType() {
        return _IDDocType;
    };

    string IDStatuut() {
        return _IDStatuut;
    };

    string IDHashPhoto() {
        return _IDHashPhoto;
    };
    // EF(ID#Photo) 

    Buffer IDFoto() {
        return _IDFoto;
    };
    // EF(Preferences) 

    string IDPrefFile() {
        return _IDPrefFile;
    };

    string IDPrefLG() {
        return _IDPrefLG;
    };

    string IDPrefDE() {
        return _IDPrefDE;
    };
    // EF(SIG#ADDRESS) 

    Buffer IDSigAdres() {
        return _IDSigAdres;
    };
    // EF(SIG#ID) 

    Buffer IDSigID() {
        return _IDSigID;
    };

    Buffer AutCert() {
        return _AuthCert;
    }

    Buffer SigCert() {
        return _SigCert;
    }

private:
    // EF(TokenInfo)
    Buffer _IDTI;
    // EF(ID#ID)
    string _IDKaartNr;
    string _IDChipNR;
    string _IDGeldigVan;
    string _IDGeldigTot;
    string _IDGemAfgifte;
    string _IDNN;
    string _IDTitel;
    string _IDNaam;
    string _IDVoornaam1;
    string _IDVoornaam2;
    string _IDVoornamen;
    string _IDNat;
    string _IDGeslacht;
    string _IDGebDatum;
    string _IDGebPlaats;
    // EF(ID#ADRES) 
    string _IDStraat;
    string _IDGemeente;
    string _IDPostcode;
    string _IDDocType;
    string _IDStatuut;
    string _IDHashPhoto;
    // EF(ID#Photo) 
    Buffer _IDFoto;
    // EF(Preferences) 
    string _IDPrefFile;
    string _IDPrefLG;
    string _IDPrefDE;
    // EF(SIG#ADDRESS) 
    Buffer _IDSigAdres;
    // EF(SIG#ID) 
    Buffer _IDSigID;
    // EF(CERT_AUTH)
    Buffer _AuthCert;
    // EF(CERT_SIG)
    Buffer _SigCert;

    string GetTlvValue(const Buffer& File, int Tag);

    string _LastErrorMsg;

    PCSCCard _PCSC;

public:
    bool Disconnect(void);
};

#endif __EIKFILES__

