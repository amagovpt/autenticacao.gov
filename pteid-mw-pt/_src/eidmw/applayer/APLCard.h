/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012-2014, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
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

#ifndef __SCCARDUTIL_H__
#define __SCCARDUTIL_H__

#include <openssl/x509.h>
#include <openssl/asn1t.h>
#include <openssl/asn1.h>
#include <openssl/err.h>
#include <openssl/cms.h>
#include <openssl/bio.h>

#include "APLPublicKey.h"
#include "APLReader.h"
#include "ByteArray.h"
#include "Export.h"
#include "P15Objects.h"
#include "PhotoPteid.h"
#include "SODParser.h"
#include <memory>
#include <string>
#include <unordered_map>

#define BEGIN_CAL_OPERATION(obj)                                                                                       \
	obj->CalLock();                                                                                                    \
	try {

#define END_CAL_OPERATION(obj)                                                                                         \
	}                                                                                                                  \
	catch (...) {                                                                                                      \
		obj->CalUnlock();                                                                                              \
		throw;                                                                                                         \
	}                                                                                                                  \
	obj->CalUnlock();

namespace eIDMW {

EIDMW_APL_API bool APLVerifySignature(const char *container_path, char *errors, unsigned long *error_len);
enum APL_DocumentType {
	APL_DOCTYPE_ID = 0,
	APL_DOCTYPE_ADDRESS,
	APL_DOCTYPE_SOD,
	APL_DOCTYPE_INFO,
	APL_DOCTYPE_PINS,
	APL_DOCTYPE_CERTIFICATES
};

enum APL_RawDataType {
	APL_RAWDATA_ID = 0,
	APL_RAWDATA_TRACE,
	APL_RAWDATA_ADDR,
	APL_RAWDATA_SOD,
	APL_RAWDATA_CARD_INFO,
	APL_RAWDATA_TOKEN_INFO,
	APL_RAWDATA_PERSO_DATA
};

enum APL_SignatureLevel { LEVEL_BASIC, LEVEL_TIMESTAMP, LEVEL_LT, LEVEL_LTV };

enum APL_PACEAuthenticationType {
	APL_PACE_CAN,
	APL_PACE_MRZ,
	APL_PACE_UNSUPPORTED,
};

class APDU;
class CReader;
class PDFSignature;
class APL_CryptoFwk;
class APL_ReaderContext;
class APL_Pin;
class APL_XMLDoc;
/******************************************************************************/ /**
  * Abstract base class for all type of card
  *********************************************************************************/
class APL_Card {
public:
	/**
	  * Pur virtual destructor
	  */
	EIDMW_APL_API virtual ~APL_Card() = 0;

	/**
	 * Return the type of the card
	 */
	EIDMW_APL_API virtual APL_CardType getType() const;

	/**
	 * Return rawdata from the card
	 */
	EIDMW_APL_API virtual const CByteArray &getRawData(APL_RawDataType type) = 0;

	EIDMW_APL_API virtual CByteArray sendAPDU(const APDU &apdu);

	EIDMW_APL_API virtual CByteArray sendAPDU(const CByteArray &cmd);

	EIDMW_APL_API virtual CByteArray Sign(const CByteArray &oData, bool signatureKey, const unsigned long paddingType);

	/* XADeS Signature Methods  */

	EIDMW_APL_API CByteArray &SignXades(const char **path, unsigned int n_paths, const char *output_path,
										APL_SignatureLevel level);

	EIDMW_APL_API CByteArray &SignXadesT(const char **path, unsigned int n_paths, const char *output_path);

	EIDMW_APL_API CByteArray &SignXadesA(const char **path, unsigned int n_paths, const char *output_path);

	EIDMW_APL_API void SignXadesIndividual(const char **, unsigned int, const char *);

	EIDMW_APL_API void SignXadesTIndividual(const char **, unsigned int, const char *);

	EIDMW_APL_API void SignXadesAIndividual(const char **, unsigned int, const char *);

	EIDMW_APL_API void SignASiC(const char *path, APL_SignatureLevel level);

	EIDMW_APL_API bool ChangeCapPin(const char *new_pin);

	/* PDF Signature */

	EIDMW_APL_API int SignPDF(PDFSignature *pdf_sig, const char *location, const char *reason,
							  const char *outfile_path);

	/**
	  * Read a file on the card
	  *
	  * @param csPath is the path of the file to be read
	  * @param bytearray will contain the content of the file
	  */
	EIDMW_APL_API virtual unsigned long readFile(const char *csPath, CByteArray &oData, unsigned long ulOffset = 0,
												 unsigned long ulMaxLength = 0);

	/**
	  * Write a file to the card
	  *
	  * @param csPath is the path of the file to be written
	  * @param oData will contain the content of the file
	  */
	EIDMW_APL_API virtual bool writeFile(const char *csPath, const CByteArray &oData, unsigned long ulOffset = 0);

	EIDMW_APL_API void initPaceAuthentication(const char *secret, size_t secretLen,
											  APL_PACEAuthenticationType secretType);

	void CalLock();	  /**< Lock the reader for exclusive atomic access */
	void CalUnlock(); /**< Unlock the reader */

	/**
	  * Give access to the CAL
	  * NOT FOR EXPORT
	  */
	CReader *getCalReader() const;

protected:
	/**
	  * Constructor
	  *
	  * @param reader is the reader from the card layer (CAL) in which the card is plugged
	  */
	APL_Card(APL_ReaderContext *reader);

	void SignIndividual(const char **, unsigned int, const char *, bool, bool);

	static APL_CryptoFwk *m_cryptoFwk; /**< Pointer to the crypto framework */
	APL_ReaderContext *m_reader;	   /**< Pointer to CAL reader (came from constructor) */

	CMutex m_Mutex; /**< Mutex */

private:
	APL_Card(const APL_Card &card);			   /**< Copy not allowed - not implemented */
	APL_Card &operator=(const APL_Card &card); /**< Copy not allowed - not implemented */

	friend bool APL_ReaderContext::connectCard(); /**< This method must access protected constructor */
};

class APDU;
class APL_Pins;
class APL_Certif;
class APL_Certifs;
class APL_CardFile_Info;
/******************************************************************************/ /**
  * Abstract base class for smart card (like eid card)
  *********************************************************************************/
class APL_SmartCard : public APL_Card {
public:
	/**
	  * Pur virtual destructor
	  */
	EIDMW_APL_API virtual ~APL_SmartCard() = 0;

	/**
	  * Tell the card to select an application on the card
	  */
	EIDMW_APL_API void selectApplication(const CByteArray &applicationId) const;

	EIDMW_APL_API virtual CByteArray sendAPDU(const APDU &apdu, APL_Pin *pin = NULL, const char *csPinCode = "");

	EIDMW_APL_API virtual CByteArray sendAPDU(const CByteArray &cmd, APL_Pin *pin = NULL, const char *csPinCode = "");

	/**
	  * Read a file on the card
	  *
	  * @param csPath is the path of the file to be read
	  * @param bytearray will contain the content of the file
	  */
	EIDMW_APL_API virtual unsigned long readFile(const char *csPath, CByteArray &oData, unsigned long ulOffset = 0,
												 unsigned long ulMaxLength = 0);

	/**
	  * Read a file on the card
	  *
	  * @param fileID : is the name/path of the file
	  * @param in : will return the content of the file
	  * @param pin : is the pin to verify before reading (if null the code will be asked)
	  * @param csPinCode : is the pin code (if empty the code will be asked)
	  */
	EIDMW_APL_API virtual unsigned long readFile(const char *fileID, CByteArray &in, APL_Pin *pin,
												 const char *csPinCode);

	/**
	  * Write a file to the card
	  *
	  * @param fileID : is the name/path of the file
	  * @param out : the content of the file
	  * @param pin : is the pin to verify before writing (if null the code will be asked)
	  * @param csPinCode : is the pin code (if empty the code will be asked)
	  * @param ulOffset: is the offset of the data to be written to the file
	  */
	EIDMW_APL_API virtual bool writeFile(const char *fileID, const CByteArray &out, APL_Pin *pin = NULL,
										 const char *csPinCode = "", unsigned long ulOffset = 0);

	/**
	  * Return the number of pin on the card
	  */
	EIDMW_APL_API virtual unsigned long pinCount();

	/**
	  * Return an object to access all the pins on the card
	  */
	EIDMW_APL_API virtual APL_Pins *getPins();

	/**
	  * Return structure containing the PKCS15 object from the CAL
	  */
	EIDMW_APL_API virtual tPin getPin(unsigned long ulIndex);

	/**
	  * Return pinStatus from the CAL
	  */
	EIDMW_APL_API virtual unsigned long pinStatus(const tPin &Pin);

	EIDMW_APL_API bool isPinVerified(const tPin &Pin);

	/* Get the CVC CA public key that
	 * this card uses to verify the CVC key; */
	EIDMW_APL_API virtual APLPublicKey *getRootCAPubKey() = 0;

	/**
	  * Execute a pin command from the CAL
	  */
	EIDMW_APL_API virtual bool pinCmd(tPinOperation operation, const tPin &Pin, const char *csPin1, const char *csPin2,
									  unsigned long &ulRemaining, bool bShowDlg = true, void *wndGeometry = 0);

	/**
	  * Return the number of certificate on the card
	  */
	EIDMW_APL_API virtual unsigned long certificateCount();

	/**
	  * Return an object to access all the certificates on the card
	  */
	EIDMW_APL_API virtual APL_Certifs *getCertificates();

	/**
	  * Return structure containing the PKCS15 object from the CAL
	  */
	EIDMW_APL_API virtual tCert getP15Cert(unsigned long ulIndex);

	APL_CardFile_Info *getFileInfo(); /**< Return a pointer to the pseudo file info */

	virtual const char *getTokenSerialNumber() = 0; /**< Return the token serial number (pkcs15 parse) (NOT EXPORTED) */
	virtual const char *getTokenLabel() = 0;		/**< Return the token label (pkcs15 parse) (NOT EXPORTED) */

protected:
	/**
	  * Constructor
	  *
	  * @param reader is the reader from the card layer (CAL) in which the card is plugged
	  */
	APL_SmartCard(APL_ReaderContext *reader);

	APL_Pins *m_pins;	  /**< Pointer to the collection of pins */
	APL_Certifs *m_certs; /**< Pointer to the certificate store */

	APL_CardFile_Info *m_fileinfo; /**< Pointer to the "pseudo file" Info */

	bool m_allowTestParam;	/**< Allow test card (from config) */
	bool m_allowTestAnswer; /**< User's answer to allow test card */
	bool m_allowTestAsked;	/**< Already asked for allowing test card */

	unsigned long m_certificateCount;
	unsigned long m_pinCount;

	APLPublicKey *m_RootCAPubKey;
	std::string *m_tokenSerial;
	std::string *m_tokenLabel;
	std::string *m_appletVersion;
};

class IcaoDg1;
class IcaoDg2;
class IcaoDg3;
class IcaoDg11;
class PhotoPteid;
class APL_ICAO : public APL_SmartCard {
public:
	enum DataGroupID { DG1 = 0x01, DG2, DG3, DG4, DG5, DG6, DG7, DG8, DG9, DG10, DG11, DG12, DG13, DG14, DG15, DG16 };

	~APL_ICAO();
	
	EIDMW_APL_API virtual std::vector<DataGroupID> getAvailableDatagroups();
	EIDMW_APL_API virtual CByteArray readDatagroup(DataGroupID tag);
	EIDMW_APL_API virtual IcaoDg1 *readDataGroup1();
	EIDMW_APL_API virtual IcaoDg2 *readDataGroup2();
	EIDMW_APL_API virtual IcaoDg3 *readDataGroup3();
	EIDMW_APL_API virtual IcaoDg11 *readDataGroup11();

	EIDMW_APL_API virtual void loadMasterList(const char *filePath);
	EIDMW_APL_API virtual APL_CardType getType() const override;

	virtual const CByteArray &getRawData(APL_RawDataType type) override;
	virtual APLPublicKey *getRootCAPubKey() override;
	virtual const char *getTokenSerialNumber() override;
	virtual const char *getTokenLabel() override;

private:
	APL_ReaderContext *m_reader; /**< Pointer to CAL reader (came from constructor) */
	std::unique_ptr<SODAttributes> m_SodAttributes;
	std::unique_ptr<IcaoDg1> m_mrzDg1;
	std::unique_ptr<IcaoDg2> m_faceDg2;
	std::unique_ptr<IcaoDg3> m_fingersDg3;
	std::unique_ptr<IcaoDg11> m_infoDg11;

	static constexpr unsigned char MRTD_APPLICATION[] = {0xA0, 0x00, 0x00, 0x02, 0x47, 0x10, 0x01};
	static constexpr const char *SOD_PATH = "011D";
	static const std::unordered_map<APL_ICAO::DataGroupID, std::string> DATAGROUP_PATHS;
	static const std::vector<int> EXPECTED_TAGS;

	bool verifySodFileIntegrity(const CByteArray &data, CByteArray &out_sod);
	void loadAvailableDataGroups();
	bool verifySOD(DataGroupID tag, const CByteArray& data);
	bool performActiveAuthentication();

protected:
	APL_ICAO(APL_ReaderContext *reader);

	friend APL_ICAO *APL_ReaderContext::getICAOCard();
	friend bool APL_ReaderContext::connectCard();
};

} // namespace eIDMW

#endif //__SCCARDUTIL_H__
