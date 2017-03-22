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
#include <iostream>
#include "PKCS15.h"
#include "PKCS15Parser.h"
#include "Card.h"

namespace eIDMW
{

  const static tPin PinInvalid = {false, "",0,0,0,0,0,0, 0, 0, 0, 0, 0,PIN_ENC_ASCII,"",""};
  const static tCert CertInvalid = {false, "", 0, 0,0,0};
  const static tPrivKey PrivKeyInvalid = {false, "", 0,0,0,0,0,0,0,"", 0,false};

  // Hardcoded Pteid V1 PINs, keys, certs -- to be removed
  bool            bValid;
  std::string     csLabel;
  bool            bFlags[2];
  unsigned long   ulAuthID; // optional
  unsigned long   ulID;
  bool            bPinFlags[6];
  unsigned long   ulPinType;
  unsigned long   ulMinLen;
  unsigned long   ulStoredLen;
  unsigned long   ulMaxLen;  // optional ?
  unsigned char   ulPinRef;  // optional
  unsigned char   ucPadChar;
  tPinEncoding    encoding;
  std::string     csPath;

  const static tPin PinPteidV1       = {true, "Basic PIN",0,1,1,1,0,NO_ID, 4, 8, 12, 1, 0xFF,PIN_ENC_ASCII,"","3F00"};
  const static tPin pinPteidAuthV2   = {true, "Authentication",0,1,1,1,0,NO_ID, 4, 8, 12, 0x85, 0xFF,PIN_ENC_ASCII,"","3F00"};
  const static tPin pinPteidSignV2   = {true, "Signature",0,1,1,1,0,NO_ID,4, 8, 12, 0x86, 0xFF,PIN_ENC_ASCII,"","3F00"};

  //const static tPrivKey KeyAuthPteidV1 = {true, "Authentication", 0,0,0,0,0,0,0x82,"3F005F00", 1024 ,true};
  //const static tPrivKey KeySignPteidV1 = {true, "Signature", 0,0,0,0,0,0,0x83,"3F005F00", 1024,true};
  //const static tPrivKey KeyAuthPteidV2 = {true, "Authentication", 0,0,0,0,0,0,0x8A,"3F005F00", 1024,true};
  //const static tPrivKey KeySignPteidV2 = {true, "Signature", 0,0,0,0,0,0,0x89,"3F005F00", 1024,true};

  const std::string defaultEFTokenInfo = "3F004F005032";
  const std::string defaultEFODF       = "3F004F005031";


  CPKCS15::CPKCS15(void)
  {

  }

  CPKCS15::CPKCS15(CContext *poContext) :
    m_poContext(poContext)
  {
    Clear();
  }

  CPKCS15::~CPKCS15(void)
  {
  }

  void CPKCS15::Clear(CCard *poCard)
  {
    m_poCard = poCard;

    // clear everything
    m_csSerial = "";
    m_csLabel = "";

    m_oPins.clear();
    m_oCertificates.clear();
    m_oPrKeys.clear();

    m_xPin.setDefault();
    m_xDir.setDefault();
    m_xPrKey.setDefault();
    m_xCert.setDefault();

    m_xTokenInfo.setDefault();
    m_xODF.setDefault();
    m_xAODF.setDefault();
    m_xCDF.setDefault();
    m_xPrKDF.setDefault();
  }

  void CPKCS15::SetCard(CCard *poCard)
  {
    m_poCard = poCard;
  }


  std::string CPKCS15::GetSerialNr()
  {
    if (m_csSerial == "")
      {
#ifdef VERBOSE
	std::cerr<<"CPKCS15::GetSerialNr"<<std::endl;
#endif
	if(! m_xTokenInfo.isRead) ReadLevel2(TOKENINFO); 
      }
    return m_csSerial;
  }

  std::string CPKCS15::GetCardLabel()
  {
    // check if we know it already
    if (m_csLabel == ""){
      if(! m_xTokenInfo.isRead) ReadLevel2(TOKENINFO); 
    }
    return m_csLabel;
  }


  unsigned long CPKCS15::PinCount()
  {
    if(! m_xAODF.isRead)  ReadLevel3(AODF);
    return (unsigned long)m_oPins.size();
  }

  tPin CPKCS15::GetPin(unsigned long ulIndex)
  {
    if(!m_xAODF.isRead)  ReadLevel3(AODF);
    if(ulIndex >= m_oPins.size()){
      throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
	}
    return m_oPins.at(ulIndex);
  }

  tPin CPKCS15::GetPinByID(unsigned long ulID)
  {
    if(! m_xAODF.isRead)  ReadLevel3(AODF);

    for(std::vector<tPin>::const_iterator ip = m_oPins.begin();
	ip != m_oPins.end();++ip){
      if(ip->ulID == ulID) return *ip;
    }
    return PinInvalid;
  }

  tPin CPKCS15::GetPinByRef(unsigned long ulPinRef)
  {
    if(! m_xAODF.isRead)  ReadLevel3(AODF);

    for(std::vector<tPin>::const_iterator ip = m_oPins.begin();
	ip != m_oPins.end();++ip){
      if(ip->ulPinRef == ulPinRef) return *ip;
    }
    return PinInvalid;
  }


  unsigned long CPKCS15::CertCount()
  {
    if( ! m_xCDF.isRead) ReadLevel3(CDF);
    return (unsigned long)m_oCertificates.size();
  }

  tCert CPKCS15::GetCert(unsigned long ulIndex)
  {
    if( ! m_xCDF.isRead) ReadLevel3(CDF);
    if(ulIndex >= m_oCertificates.size())
      throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
    return m_oCertificates.at(ulIndex);
  }

  tCert CPKCS15::GetCertByID(unsigned long ulID)
  {
    if( ! m_xCDF.isRead) ReadLevel3(CDF);
    for( std::vector<tCert>::const_iterator ic = m_oCertificates.begin();
	 ic != m_oCertificates.end(); ++ic){
      if(ic->ulID == ulID) return *ic;
    }
    return CertInvalid;
  }


  unsigned long CPKCS15::PrivKeyCount()
  {
    if(! m_xPrKDF.isRead){  ReadLevel3(PRKDF);}
    return (unsigned long)m_oPrKeys.size();
  }

  tPrivKey CPKCS15::GetPrivKey(unsigned long ulIndex)
  {
    if(! m_xPrKDF.isRead)  ReadLevel3(PRKDF);
    if(ulIndex >= m_oPrKeys.size())
      throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
    return m_oPrKeys.at(ulIndex);
  }

  tPrivKey CPKCS15::GetPrivKeyByID(unsigned long ulID)
  {
    if(! m_xPrKDF.isRead)  ReadLevel3(PRKDF);
    
    for(std::vector<tPrivKey>::const_iterator ik = m_oPrKeys.begin();
	ik != m_oPrKeys.end(); ++ik){
      if(ik->ulID == ulID) return *ik;
    }
    return PrivKeyInvalid;
  }

  void CPKCS15::ReadLevel1(){
    // read info in 3f002f00
    if(m_xDir.path == "") { m_xDir.path = "3F002F00";}
    

	m_xDir.byteArray = m_poCard->ReadFile(m_xDir.path);
    // parse
    m_tDir = m_poParser->ParseDir(m_xDir.byteArray);
    m_xDir.isRead = true;

    // store paths for tokenInfo and for ODF
    m_xTokenInfo.path = m_tDir.csAppPath + "5032";
    m_xODF.path = m_tDir.csAppPath + "5031";
  }

  void CPKCS15::ReadLevel2(tPKCSFileName name){
    // read info for the requested file (ODF or tokenInfo)
    // propagate the information about the path of the corresponding level 3 (only ODF)
    tOdfInfo resultOdf;
    tTokenInfo resultTokenInfo;

    switch(name){
    case ODF:
      ReadFile(&m_xODF,1);
      // parse
      m_xODF.byteArray.TrimRight();
      //Trim trailing NUL bytes (the actual ASN.1 content is 48 bytes)
      resultOdf = m_poParser->ParseOdf(m_xODF.byteArray);
      // propagate the path info  
      m_xAODF.path = resultOdf.csAodfPath;
      m_xCDF.path = resultOdf.csCdfPath;
      m_xPrKDF.path = resultOdf.csPrkdfPath;
      m_xPuKDF.path = resultOdf.csPukdfPath;
      break;
    case TOKENINFO:
      ReadFile(&m_xTokenInfo,1);
      // parse
      resultTokenInfo = m_poParser->ParseTokenInfo(m_xTokenInfo.byteArray);
      m_csSerial = resultTokenInfo.csSerial; 
      m_csLabel = resultTokenInfo.csLabel; 
      break;
    default:
      // error: this method can only be called with ODF or TOKENINFO
      return;
    }
  }

  void CPKCS15::ReadLevel3(tPKCSFileName name){
     
      switch(name){
      case AODF:
          ReadFile(&m_xAODF,2);
          // parse
          //Trim trailing NUL bytes (the actual ASN.1 content is 168 bytes)
          m_xAODF.byteArray.TrimRight();
          m_oPins = m_poParser->ParseAodf(m_xAODF.byteArray);
          break;
      case CDF:
          ReadFile(&m_xCDF,2);
          // parse
          m_oCertificates = m_poParser->ParseCdf(m_xCDF.byteArray);
          // correct
          break;
      case PRKDF:
          ReadFile(&m_xPrKDF,2);
          // parse
          m_oPrKeys = m_poParser->ParsePrkdf(m_xPrKDF.byteArray);
          m_oPrKeys.pop_back();
          break;
      default:
          // error: this method can only be called with AODF, CDF or PRKDF
          return;
      }
  }

  void CPKCS15::ReadFile(tPKCSFile* pFile,int upperLevel){
	
    if(pFile->path == "") {
      switch(upperLevel){
      case 1:
	ReadLevel1();
	break;
      case 2:
    ReadLevel2(ODF);
	break;
      default:
	 //error: no other levels alllowed
	return;
      }
    } 
    pFile->byteArray = m_poCard->ReadFile(pFile->path);
    pFile->isRead = true;
  }
  
}
