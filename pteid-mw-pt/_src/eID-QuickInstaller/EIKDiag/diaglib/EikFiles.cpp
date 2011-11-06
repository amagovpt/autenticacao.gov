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

#include "StdAfx.h"

#include "eikfiles.h"


#pragma hdrstop

CEikFiles::CEikFiles(void) {

    _LastErrorMsg = "";

    if (!_PCSC.Initialize()) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
    }

}

CEikFiles::~CEikFiles(void) {

    _LastErrorMsg = "";

}

bool CEikFiles::Connect(const string& ReaderName) {

    if (!_PCSC.Connect(ReaderName)) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
        return false;
    }

    return true;

}

bool CEikFiles::ReadTI(void) {

    _LastErrorMsg = "";

    this->_IDTI = _PCSC.ReadCardFile(EF_TI, sizeof (EF_TI));
    if (this->_IDTI.size() == 0) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
        return false;
    }

    return true;
}

bool CEikFiles::ReadID(void) {

    _LastErrorMsg = "";

    Buffer b = _PCSC.ReadCardFile(EF_ID_RN, sizeof (EF_ID_RN));
    if (b.size() == 0) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
        return false;
    }

    this->_IDKaartNr = GetTlvValue(b, 1);
    this->_IDChipNR = GetTlvValue(b, 2);
    this->_IDGeldigVan = GetTlvValue(b, 3);
    this->_IDGeldigTot = GetTlvValue(b, 4);
    this->_IDGemAfgifte = GetTlvValue(b, 5); // UTF-8
    this->_IDNN = GetTlvValue(b, 6);
    this->_IDNaam = GetTlvValue(b, 7); // UTF-8
    this->_IDVoornaam1 = GetTlvValue(b, 8); // UTF-8
    this->_IDVoornaam2 = GetTlvValue(b, 9);
    this->_IDVoornamen = this->_IDVoornaam1 + " " + this->_IDVoornaam2;
    this->_IDNat = GetTlvValue(b, 10); // UTF-8
    this->_IDGebPlaats = GetTlvValue(b, 11); // UTF-8
    this->_IDGebDatum = GetTlvValue(b, 12); // UTF-8
    this->_IDGeslacht = GetTlvValue(b, 13);
    this->_IDTitel = GetTlvValue(b, 14); // UTF-8
    this->_IDDocType = GetTlvValue(b, 15);
    this->_IDStatuut = GetTlvValue(b, 16);
    this->_IDHashPhoto = GetTlvValue(b, 17);

    return true;

}

bool CEikFiles::ReadAddress(void) {

    _LastErrorMsg = "";

    Buffer b = _PCSC.ReadCardFile(EF_ID_ADDRESS, sizeof (EF_ID_ADDRESS));
    if (b.size() == 0) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
        return false;
    }

    this->_IDStraat = GetTlvValue(b, 01); // UTF-8
    this->_IDPostcode = GetTlvValue(b, 02); // UTF-8
    this->_IDGemeente = GetTlvValue(b, 03); // UTF-8

    return true;

}

bool CEikFiles::ReadPhoto(void) {

    _LastErrorMsg = "";

    this->_IDFoto = _PCSC.ReadCardFile(EF_ID_PHOTO, sizeof (EF_ID_PHOTO));
    if (this->_IDFoto.size() == 0) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
        return false;
    }

    return true;
}

bool CEikFiles::ReadAuthCert() {
    _LastErrorMsg = "";

    this->_AuthCert = _PCSC.ReadCardFile(EF_CERT_AUTH, sizeof (EF_CERT_AUTH));
    if (this->_AuthCert.size() == 0) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
        return false;
    }

    return true;
}

bool CEikFiles::ReadSigCert() {
    _LastErrorMsg = "";

    this->_SigCert = _PCSC.ReadCardFile(EF_CERT_NR, sizeof (EF_CERT_NR));
    if (this->_SigCert.size() == 0) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
        return false;
    }

    return true;
}

string CEikFiles::GetTlvValue(const Buffer& File, int Tag) {

    string Result = "";
    int ip;
    int currentTag;
    int size;

    ip = 0;
    while (ip < static_cast<int> (File.size())) {
        currentTag = File.at(ip++);
        size = File.at(ip++);
        if (size == 255) {
            size = (size << 8) | File.at(ip++);
        }
        if (currentTag == Tag) {
            int j = ip;
            for (int i = 0; i < size; ++i) {
                Result.push_back(File.at(j++));
            }
            break;
        }
        ip += size;
    }

    return Result;
}

bool CEikFiles::Disconnect(void) {

    if (!_PCSC.Disconnect(true)) {
        this->_LastErrorMsg = _PCSC.LastErrorMsg();
        return false;
    }
    return true;
}
