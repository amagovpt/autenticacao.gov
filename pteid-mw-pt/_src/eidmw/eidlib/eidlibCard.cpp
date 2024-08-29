/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2011-2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2016-2022 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018-2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
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
#include "eidlib.h"

#include "InternalUtil.h"
#include "MWException.h"
#include "eidErrors.h"
#include "eidlibException.h"

#include "APLCard.h"
#include "APLCardPteid.h"
#include "APLCertif.h"
#include "APLCrypto.h"
#include "ByteArray.h"
#include "CardPteid.h"
#include "IcaoDg1.h"
#include "IcaoDg11.h"
#include "IcaoDg2.h"
#include "PDFSignature.h"
#include "PhotoPteid.h"
#include "SSLConnection.h"
#include "SecurityContext.h"
#include "Util.h"
#include "dialogs.h"
#include <sstream>
#include <cassert>

// UNIQUE INDEX FOR RETRIEVING OBJECT
#define INCLUDE_OBJECT_DOCEID 1
#define INCLUDE_OBJECT_SODID 2
#define INCLUDE_OBJECT_PINS 3
#define INCLUDE_OBJECT_CERTIFICATES 4
#define INCLUDE_OBJECT_DOCINFO 5
#define INCLUDE_OBJECT_CUSTOMDOC 9
#define INCLUDE_OBJECT_ROOT_CA_PK 10
#define INCLUDE_OBJECT_ADDRESS 11

#define BCD_DATE_LEN 4

// Global variable used by the compatibility functions defined in eidlibcompat.h
eIDMW::PTEID_ReaderContext *readerContext = NULL;

namespace eIDMW {

/*****************************************************************************************
---------------------------------------- Card --------------------------------------------
*****************************************************************************************/
PTEID_Card::PTEID_Card(const SDK_Context *context, APL_Card *impl) : PTEID_Object(context, impl) {}

PTEID_Card::~PTEID_Card() {}

PTEID_CardType PTEID_Card::getType() {
	PTEID_CardType out = PTEID_CARDTYPE_UNKNOWN;
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);
	out = ConvertCardType(pcard->getType());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_Card::sendAPDU(const PTEID_ByteArray &cmd) {
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray apdu(cmd.GetBytes(), cmd.Size());
	CByteArray result = pcard->sendAPDU(apdu);
	out.Append(result.GetBytes(), result.Size());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_Card::Sign(const PTEID_ByteArray &data, bool signatureKey) {
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray cData(data.GetBytes(), data.Size());
	CByteArray result = pcard->Sign(cData, signatureKey, SIGN_ALGO_RSA_PKCS);
	out.Append(result.GetBytes(), result.Size());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_Card::Sign(const PTEID_ByteArray &data, PTEID_RSAPaddingType paddingType, bool signatureKey) {
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray cData(data.GetBytes(), data.Size());
	CByteArray result;
	if (paddingType == PTEID_RSAPaddingType::PADDING_TYPE_RSA_PKCS)
		result = pcard->Sign(cData, signatureKey, SIGN_ALGO_RSA_PKCS);
	else if (paddingType == PTEID_RSAPaddingType::PADDING_TYPE_RSA_PSS)
		result = pcard->Sign(cData, signatureKey, SIGN_ALGO_RSA_PSS);
	else
		throw CMWEXCEPTION(EIDMW_ERR_ALGO_BAD);

	out.Append(result.GetBytes(), result.Size());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_Card::SignSHA256(const PTEID_ByteArray &data, bool signatureKey) {
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray cData(data.GetBytes(), data.Size());
	CByteArray result = pcard->Sign(cData, signatureKey, true);
	out.Append(result.GetBytes(), result.Size());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_Card::readFile(const char *fileID, unsigned long ulOffset, unsigned long ulMaxLength) {
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray oData;

	pcard->readFile(fileID, oData, ulOffset, ulMaxLength);

	out.Append(oData.GetBytes(), oData.Size());

	END_TRY_CATCH

	return out;
}

bool PTEID_Card::writeFile(const char *fileID, const PTEID_ByteArray &oData, unsigned long ulOffset) {
	bool out = false;
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray bytearray(oData.GetBytes(), oData.Size());
	out = pcard->writeFile(fileID, bytearray, ulOffset);

	END_TRY_CATCH
	return out;
}

void PTEID_Card::initPaceAuthentication(const char *secret, size_t length, PTEID_CardPaceSecretType secretType) {
	APL_PACEAuthenticationType type = APL_PACE_UNSUPPORTED;
	if (secretType == PTEID_CardPaceSecretType::PTEID_CARD_SECRET_CAN)
		type = APL_PACEAuthenticationType::APL_PACE_CAN;
	else if (secretType == PTEID_CardPaceSecretType::PTEID_CARD_SECRET_MRZ) {
		type = APL_PACEAuthenticationType::APL_PACE_MRZ;
	} else {
		// not supported
	}
	BEGIN_TRY_CATCH
	APL_Card *pcard = static_cast<APL_Card *>(m_impl);
	pcard->initPaceAuthentication(secret, length, type);
	END_TRY_CATCH
}

/*****************************************************************************************
---------------------------------------- PTEID_SmartCard --------------------------------------
*****************************************************************************************/
PTEID_SmartCard::PTEID_SmartCard(const SDK_Context *context, APL_Card *impl) : PTEID_Card(context, impl) {}

PTEID_SmartCard::~PTEID_SmartCard() {}

void PTEID_SmartCard::selectApplication(const PTEID_ByteArray &applicationId) {
	BEGIN_TRY_CATCH

	APL_SmartCard *pcard = static_cast<APL_SmartCard *>(m_impl);

	CByteArray bytearray(applicationId.GetBytes(), applicationId.Size());

	pcard->selectApplication(bytearray);

	END_TRY_CATCH
}

PTEID_ByteArray PTEID_SmartCard::sendAPDU(const PTEID_ByteArray &cmd, PTEID_Pin *pin, const char *csPinCode) {
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard = static_cast<APL_SmartCard *>(m_impl);

	APL_Pin *pimplPin = NULL;

	if (pin)
		pimplPin = pcard->getPins()->getPinByNumber(pin->getIndex());

	CByteArray apdu(cmd.GetBytes(), cmd.Size());

	CByteArray result = pcard->sendAPDU(apdu, pimplPin, csPinCode);

	out.Append(result.GetBytes(), result.Size());

	END_TRY_CATCH

	return out;
}

long PTEID_SmartCard::readFile(const char *fileID, PTEID_ByteArray &in, PTEID_Pin *pin, const char *csPinCode) {
	long out = 0;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard = static_cast<APL_SmartCard *>(m_impl);

	CByteArray bytearray;

	APL_Pin *pimplPin = NULL;

	if (pin)
		pimplPin = pcard->getPins()->getPinByNumber(pin->getIndex());

	out = pcard->readFile(fileID, bytearray, pimplPin, csPinCode);
	in = bytearray;

	END_TRY_CATCH

	return out;
}

// is_landscape is unused parameter, we keep it here to ensure backwards-compatibility
int PTEID_Card::SignPDF(PTEID_PDFSignature &sig_handler, int page, int page_sector, bool is_landscape,
						const char *location, const char *reason, const char *outfile_path) {
	PDFSignature *pdf_sig = NULL;
	int rc = 0;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	// Accessing PTEID_PDFSignature private parts because we're friends :)
	pdf_sig = sig_handler.mp_signature;

	if (page_sector != 0 && page != 0)
		pdf_sig->setVisible(page, page_sector);

	rc = pcard->SignPDF(pdf_sig, location, reason, outfile_path);

	END_TRY_CATCH

	return rc;
}

int PTEID_Card::SignPDF(PTEID_PDFSignature &sig_handler, int page, double coord_x, double coord_y, const char *location,
						const char *reason, const char *outfile_path)

{
	int rc = 0;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	PDFSignature *pdf_sig = sig_handler.mp_signature;

	if (coord_x >= 0 && coord_y >= 0 && page != 0) {
		pdf_sig->setVisibleCoordinates(page, coord_x, coord_y);
	}

	rc = pcard->SignPDF(pdf_sig, location, reason, outfile_path);

	END_TRY_CATCH

	return rc;
}

SSLConnection *PTEID_EIDCard::buildSSLConnection() {
	APL_Card *pcard = static_cast<APL_Card *>(m_impl);
	return new SSLConnection(pcard);
}

PTEID_PDFSignature::PTEID_PDFSignature() { mp_signature = new PDFSignature(); }

PTEID_PDFSignature::PTEID_PDFSignature(const char *input_path) { mp_signature = new PDFSignature(input_path); }

PTEID_PDFSignature::~PTEID_PDFSignature() { delete mp_signature; }

void PTEID_PDFSignature::addToBatchSigning(char *input_path) { mp_signature->batchAddFile(input_path, false); }

void PTEID_PDFSignature::setFileSigning(char *input_path) { mp_signature->setFile(input_path); }

void PTEID_PDFSignature::addToBatchSigning(char *input_path, bool last_page) {
	mp_signature->batchAddFile(input_path, last_page);
}

bool PTEID_PDFSignature::isLandscapeFormat() { return mp_signature->isLandscapeFormat(); }

void PTEID_PDFSignature::enableTimestamp() { mp_signature->enableTimestamp(); }

void PTEID_PDFSignature::setSignatureLevel(PTEID_SignatureLevel signLevel) {
	mp_signature->setSignatureLevel(ConvertSignatureLevel(signLevel));
}

// this function is not exposed to Java, however is keep for backward compatibility
void PTEID_PDFSignature::setCustomImage(unsigned char *image_data, unsigned long img_length) {
	mp_signature->setCustomImage(image_data, img_length);
}

void PTEID_PDFSignature::setCustomImage(const PTEID_ByteArray &image_data) {
	mp_signature->setCustomImage(const_cast<unsigned char *>(image_data.GetBytes()), image_data.Size());
}

void PTEID_PDFSignature::setCustomSealSize(unsigned int width, unsigned int height) {
	mp_signature->setCustomSealSize(width, height);
}

void PTEID_PDFSignature::enableSmallSignatureFormat() { mp_signature->enableSmallSignature(); }

char *PTEID_PDFSignature::getOccupiedSectors(int page) {
	if (page < 1)
		throw PTEID_ExParamRange();
	return mp_signature->getOccupiedSectors(page);
}

PDFSignature *PTEID_PDFSignature::getPdfSignature() { return mp_signature; }

int PTEID_PDFSignature::getPageCount() { return mp_signature->getPageCount(); }

int PTEID_PDFSignature::getOtherPageCount(const char *input_path) {
	return mp_signature->getOtherPageCount(input_path);
}

char *PTEID_PDFSignature::getCertificateCitizenName() { return mp_signature->getCitizenCertificateName(); }

char *PTEID_PDFSignature::getCertificateCitizenID() { return mp_signature->getCitizenCertificateID(); }

bool PTEID_SmartCard::writeFile(const char *fileID, const PTEID_ByteArray &baOut, PTEID_Pin *pin, const char *csPinCode,
								unsigned long ulOffset) {
	bool out = false;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard = static_cast<APL_SmartCard *>(m_impl);

	CByteArray bytearray(baOut.GetBytes(), baOut.Size());

	APL_Pin *pimplPin = NULL;

	if (pin)
		pimplPin = pcard->getPins()->getPinByNumber(pin->getIndex());

	out = pcard->writeFile(fileID, bytearray, pimplPin, csPinCode, ulOffset);

	END_TRY_CATCH

	return out;
}

unsigned long PTEID_SmartCard::pinCount() {
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard = static_cast<APL_SmartCard *>(m_impl);
	out = pcard->pinCount();

	END_TRY_CATCH

	return out;
}

PTEID_Pins &PTEID_SmartCard::getPins() {
	PTEID_Pins *out = NULL;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard = static_cast<APL_SmartCard *>(m_impl);

	out = dynamic_cast<PTEID_Pins *>(getObject(INCLUDE_OBJECT_PINS));

	if (!out) {
		// CAutoMutex autoMutex(m_mutex);

		// ppins=dynamic_cast<PTEID_Pins *>(getObject(INCLUDE_OBJECT_PINS));
		// if(!ppins)
		//{
		out = new PTEID_Pins(m_context, pcard->getPins());
		if (out)
			m_objects[INCLUDE_OBJECT_PINS] = out;
		else
			throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

unsigned long PTEID_SmartCard::certificateCount() {
	unsigned long out = 0;
	BEGIN_TRY_CATCH

	APL_SmartCard *pcard = static_cast<APL_SmartCard *>(m_impl);
	out = pcard->certificateCount();

	END_TRY_CATCH

	return out;
}

PTEID_Certificates &PTEID_Card::getCertificates() {
	PTEID_Certificates *out = NULL;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard = static_cast<APL_SmartCard *>(m_impl);

	out = dynamic_cast<PTEID_Certificates *>(getObject(INCLUDE_OBJECT_CERTIFICATES));

	if (!out) {
		// CAutoMutex autoMutex(m_mutex);

		// pcerts=dynamic_cast<PTEID_Certificates *>(getObject(INCLUDE_OBJECT_CERTIFICATES));
		// if(!pcerts)
		//{
		out = new PTEID_Certificates(m_context, pcard->getCertificates());
		if (out)
			m_objects[INCLUDE_OBJECT_CERTIFICATES] = out;
		else
			throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

/*****************************************************************************************
---------------------------------------- PTEID_EIDCard -----------------------------------------
*****************************************************************************************/
PTEID_EIDCard::PTEID_EIDCard(const SDK_Context *context, APL_Card *impl) : PTEID_SmartCard(context, impl) {
	persoNotesDirty = false;
}

PTEID_EIDCard::~PTEID_EIDCard() {}

PTEID_CCXML_Doc &PTEID_EIDCard::getXmlCCDoc(PTEID_XmlUserRequestedInfo &userRequestedInfo) {
	PTEID_CCXML_Doc *out = NULL;

	BEGIN_TRY_CATCH
	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);
	out = dynamic_cast<PTEID_CCXML_Doc *>(getObject(INCLUDE_OBJECT_CUSTOMDOC));
	if (out)
		delete out;
	out = new PTEID_CCXML_Doc(m_context, &pcard->getXmlCCDoc(*(userRequestedInfo.customXml)));
	if (out)
		m_objects[INCLUDE_OBJECT_CUSTOMDOC] = out;
	else
		throw PTEID_ExUnknown();

	END_TRY_CATCH

	return *out;
}

PTEID_EId &PTEID_EIDCard::getID() {
	PTEID_EId *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_EId *>(getObject(INCLUDE_OBJECT_DOCEID));

	if (!out) {
		// CAutoMutex autoMutex(m_mutex);

		// pdoc=dynamic_cast<PTEID_EId *>(getObject(INCLUDE_OBJECT_DOCEID));
		// if(!pdoc)
		//{
		out = new PTEID_EId(m_context, &pcard->getID());
		if (out)
			m_objects[INCLUDE_OBJECT_DOCEID] = out;
		else
			throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

PTEID_Address &PTEID_EIDCard::getAddr() {
	PTEID_Address *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_Address *>(getObject(INCLUDE_OBJECT_ADDRESS));

	if (!out) {

		out = new PTEID_Address(m_context, &pcard->getAddr());
		if (out)
			m_objects[INCLUDE_OBJECT_ADDRESS] = out;
		else
			throw PTEID_ExUnknown();
	}

	END_TRY_CATCH

	return *out;
}

PTEID_Sod &PTEID_EIDCard::getSod() {
	PTEID_Sod *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_Sod *>(getObject(INCLUDE_OBJECT_SODID));

	if (!out) {
		// CAutoMutex autoMutex(m_mutex);

		// ppicture=dynamic_cast<PTEID_Picture *>(getObject(INCLUDE_OBJECT_PICTUREEID));
		// if(!ppicture)
		//{
		out = new PTEID_Sod(m_context, &pcard->getSod());
		if (out)
			m_objects[INCLUDE_OBJECT_SODID] = out;
		else
			throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

PTEID_CardVersionInfo &PTEID_EIDCard::getVersionInfo() {
	PTEID_CardVersionInfo *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_CardVersionInfo *>(getObject(INCLUDE_OBJECT_DOCINFO));

	if (!out) {
		// CAutoMutex autoMutex(m_mutex);

		// pdoc=dynamic_cast<PTEID_CardVersionInfo *>(getObject(INCLUDE_OBJECT_DOCINFO));
		// if(!pdoc)
		//{
		out = new PTEID_CardVersionInfo(m_context, &pcard->getDocInfo());
		if (out)
			m_objects[INCLUDE_OBJECT_DOCINFO] = out;
		else
			throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

PTEID_Certificate &PTEID_Card::getCert(PTEID_CertifType type) { return getCertificates().getCert(type); }

PTEID_Certificate &PTEID_Card::getRoot() { return getCertificates().getCert(PTEID_CERTIF_TYPE_ROOT); }

PTEID_Certificate &PTEID_Card::getCA() { return getCertificates().getCert(PTEID_CERTIF_TYPE_ROOT_AUTH); }

PTEID_Certificate &PTEID_Card::getAuthentication() {
	return getCertificates().getCert(PTEID_CERTIF_TYPE_AUTHENTICATION);
}

PTEID_Certificate &PTEID_Card::getSignature() { return getCertificates().getCert(PTEID_CERTIF_TYPE_SIGNATURE); }

PTEID_PublicKey &PTEID_EIDCard::getRootCAPubKey() {
	PTEID_PublicKey *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_PublicKey *>(getObject(INCLUDE_OBJECT_ROOT_CA_PK));

	if (!out) {

		out = new PTEID_PublicKey(m_context, *pcard->getRootCAPubKey());
		if (out)
			m_objects[INCLUDE_OBJECT_ROOT_CA_PK] = out;
		else
			throw PTEID_ExUnknown();
	}

	END_TRY_CATCH

	return *out;
}

bool PTEID_EIDCard::isActive() {
	bool out = false;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);

	out = pcard->isActive();

	END_TRY_CATCH

	return out;
}

void PTEID_EIDCard::doSODCheck(bool check) {

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);
	pcard->doSODCheck(check);

	END_TRY_CATCH
}

bool PTEID_EIDCard::Activate(const char *pinCode, PTEID_ByteArray &BCDDate, bool blockActivationPIN) {
	bool out = false;
	CByteArray cBCDDate = CByteArray(BCDDate.GetBytes(), BCDDate.Size());

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);
	out = pcard->Activate(pinCode, cBCDDate, blockActivationPIN);

	END_TRY_CATCH

	return out;
}

bool PTEID_EIDCard::writePersonalNotes(const PTEID_ByteArray &out, PTEID_Pin *pin, const char *csPinCode) {
	const unsigned long notes_file_size = 1000;
	unsigned char *pucData = static_cast<unsigned char *>(calloc(notes_file_size, sizeof(char)));

	BEGIN_TRY_CATCH

	// ensure that pin asked is the authentication one
	if (pin != NULL && pin->getPinRef() != PTEID_Pin::AUTH_PIN) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	if (out.Size() > notes_file_size) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
	}

	/**
	 * Write 1000 bytes to replace all the data in the notes file.
	 * Avoids merging previous notes with new ones,
	 * leading to an inconsistent state
	 */
	memcpy((void *)pucData, out.GetBytes(), out.Size());

	const unsigned char *data = const_cast<unsigned char *>(pucData);
	const PTEID_ByteArray notes(data, notes_file_size);

	persoNotesDirty = writeFile("3F005F00EF07", notes, pin, csPinCode);

	END_TRY_CATCH

	free(pucData);
	return persoNotesDirty;
}

bool PTEID_EIDCard::clearPersonalNotes(PTEID_Pin *pin, const char *csPinCode) {
	unsigned long ulSize = 1000;
	unsigned char *pucData = static_cast<unsigned char *>(calloc(ulSize, sizeof(char)));
	bool cleared = false;

	BEGIN_TRY_CATCH

	// ensure that pin asked is the authentication one
	if (pin != NULL && pin->getPinRef() != PTEID_Pin::AUTH_PIN) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	const unsigned char *data = const_cast<unsigned char *>(pucData);
	const PTEID_ByteArray clear(data, ulSize);

	cleared = writeFile("3F005F00EF07", clear, pin, csPinCode);

	END_TRY_CATCH
	free(pucData);
	return cleared;
}

const char *PTEID_EIDCard::readPersonalNotes() {
	char *out = NULL;
	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);

	// ensure that the file is read after being written
	pcard->getFilePersoData()->getStatus(persoNotesDirty);
	// ensure that the attributes are mapped again persoNotesDirty = true.
	out = (char *)pcard->getPersonalNotes().getPersonalNotes(persoNotesDirty);

	END_TRY_CATCH

	return out;
}

/*****************************************************************************************
--------------------------------------- ICAO_Card ----------------------------------------
*****************************************************************************************/
ICAO_Card::ICAO_Card(const SDK_Context *context, APL_ICAO *impl) : PTEID_Object(context, impl) {
}

std::vector<PTEID_DataGroupID> ICAO_Card::getAvailableDatagroups() {
	std::vector<PTEID_DataGroupID> availableDatagroups;

	BEGIN_TRY_CATCH
	auto icao = static_cast<APL_ICAO *>(m_impl);
	auto datagroups = icao->getAvailableDatagroups();
	for (const auto &dg : datagroups) {
		availableDatagroups.emplace_back(static_cast<PTEID_DataGroupID>(dg));
	}

	END_TRY_CATCH

	return availableDatagroups;
}

void ICAO_Card::initPaceAuthentication(const char *secret, size_t length, PTEID_CardPaceSecretType secretType) {
	APL_PACEAuthenticationType type = APL_PACE_UNSUPPORTED;
	if (secretType == PTEID_CardPaceSecretType::PTEID_CARD_SECRET_CAN)
		type = APL_PACEAuthenticationType::APL_PACE_CAN;
	else if (secretType == PTEID_CardPaceSecretType::PTEID_CARD_SECRET_MRZ) {
		type = APL_PACEAuthenticationType::APL_PACE_MRZ;
	} else {
		// not supported
	}

	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	icao->initPaceAuthentication(secret, length, type);
	END_TRY_CATCH
}

PTEID_ByteArray ICAO_Card::readDatagroupRaw(PTEID_DataGroupID tag) {
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	CByteArray result = icao->readDatagroup(static_cast<APL_ICAO::DataGroupID>(tag));
	out.Append(result.GetBytes(), result.Size());
	END_TRY_CATCH

	return out;
}

PTEID_ICAO_DG1::PTEID_ICAO_DG1(const IcaoDg1 &dg1) : m_impl(dg1) {}

const char *PTEID_ICAO_DG1::documentCode() const { return m_impl.documentCode().c_str(); }

const char *PTEID_ICAO_DG1::issuingState() const { return m_impl.issuingOrg().c_str(); }

const char *PTEID_ICAO_DG1::documentNumber() const { return m_impl.serialNumber().c_str(); }

const char *PTEID_ICAO_DG1::optionalDataLine1() const { return m_impl.optionalData().c_str(); }

const char *PTEID_ICAO_DG1::dateOfBirth() const { return m_impl.birthDay().c_str(); }

char PTEID_ICAO_DG1::sex() const { return m_impl.sex(); }

const char *PTEID_ICAO_DG1::dateOfExpiry() const { return m_impl.expireDay().c_str(); }

const char *PTEID_ICAO_DG1::nationality() const { return m_impl.nationality().c_str(); }

const char *PTEID_ICAO_DG1::optionalDataLine2() const { return m_impl.optionalDataSecondLine().c_str(); }

const char *PTEID_ICAO_DG1::primaryIdentifier() const { return m_impl.primaryIdentifier().c_str(); }

const char *PTEID_ICAO_DG1::secondaryIdentifier() const { return m_impl.secondaryIdentifier().c_str(); }

bool PTEID_ICAO_DG1::isPassport() const { return m_impl.documentCode().front() == 'P'; }

PTEID_ICAO_DG1::~PTEID_ICAO_DG1() {}

unsigned char PTEID_FeaturePoint::type() const { return m_impl.type(); }

unsigned char PTEID_FeaturePoint::featurePoint() const { return m_impl.featurePoint(); }

unsigned char PTEID_FeaturePoint::majorCode() const { return m_impl.majorCode(); }

unsigned char PTEID_FeaturePoint::minorCode() const { return m_impl.minorCode(); }

unsigned short PTEID_FeaturePoint::x_coord() const { return m_impl.x_coord(); }

unsigned short PTEID_FeaturePoint::y_coord() const { return m_impl.y_coord(); }

unsigned short PTEID_FeaturePoint::reserved() const { return m_impl.reserved(); }

PTEID_FeaturePoint::PTEID_FeaturePoint(FeaturePoint &featurePoint) : m_impl(featurePoint) {}

long PTEID_FaceInfoData::facialRecordDataLength() const { return m_impl.facialRecordDataLength(); }

unsigned short PTEID_FaceInfoData::numberOfFeaturePoints() const { return m_impl.numberOfFeaturePoints(); }

unsigned char PTEID_FaceInfoData::gender() const { return m_impl.gender(); }

PTEID_Gender PTEID_FaceInfoData::genderDecode() const { return static_cast<PTEID_Gender>(m_impl.genderDecode()); }

unsigned char PTEID_FaceInfoData::eyeColor() const { return m_impl.eyeColor(); }

PTEID_EyeColor PTEID_FaceInfoData::eyeColorDecode() const {
	return static_cast<PTEID_EyeColor>(m_impl.eyeColorDecode());
}

unsigned char PTEID_FaceInfoData::hairColour() const { return m_impl.hairColour(); }

PTEID_HairColour PTEID_FaceInfoData::hairColourDecode() const {
	return static_cast<PTEID_HairColour>(m_impl.hairColourDecode());
}

long PTEID_FaceInfoData::featureMask() const { return m_impl.featureMask(); }

long PTEID_FaceInfoData::expression() const { return m_impl.expression(); }

long PTEID_FaceInfoData::poseAngle() const { return m_impl.poseAngle(); }

long PTEID_FaceInfoData::poseAngleUncertainty() const { return m_impl.poseAngleUncertainty(); }

std::vector<PTEID_FeaturePoint *> PTEID_FaceInfoData::featurePoints() const { return m_featurePoints; }

unsigned char PTEID_FaceInfoData::faceImgType() const { return m_impl.faceImgType(); }

PTEID_FaceImageType PTEID_FaceInfoData::faceImgTypeDecode() const {
	return static_cast<PTEID_FaceImageType>(m_impl.faceImgTypeDecode());
}

unsigned char PTEID_FaceInfoData::imgDataType() const { return m_impl.imgDataType(); }

PTEID_ImageDataType PTEID_FaceInfoData::imgDataTypeDecode() const {
	return static_cast<PTEID_ImageDataType>(m_impl.imgDataTypeDecode());
}

unsigned short PTEID_FaceInfoData::imgWidth() const { return m_impl.imgWidth(); }

unsigned short PTEID_FaceInfoData::imgHeight() const { return m_impl.imgHeight(); }

unsigned char PTEID_FaceInfoData::colourSpace() const { return m_impl.colourSpace(); }

PTEID_ImageColourSpace PTEID_FaceInfoData::colourSpaceDecode() const {
	return static_cast<PTEID_ImageColourSpace>(m_impl.colourSpaceDecode());
}

unsigned char PTEID_FaceInfoData::sourceType() const { return m_impl.sourceType(); }

PTEID_SourceType PTEID_FaceInfoData::sourceTypeDecode() const {
	return static_cast<PTEID_SourceType>(m_impl.sourceTypeDecode());
}

unsigned short PTEID_FaceInfoData::deviceType() const { return m_impl.deviceType(); }

unsigned short PTEID_FaceInfoData::quality() const { return m_impl.quality(); }

PTEID_ByteArray PTEID_FaceInfoData::photoRawData() const { return PTEID_ByteArray(m_context, m_impl.photoRawData()); }

PTEID_FaceInfoData::PTEID_FaceInfoData(const SDK_Context *context, FaceInfoData &data)
	: PTEID_Object(context, NULL), m_impl(data) {
	for (auto &fp : m_impl.featurePoints()) {
		m_featurePoints.push_back(new PTEID_FeaturePoint(*fp));
	}
}

PTEID_FaceInfoData::~PTEID_FaceInfoData() {
	for (auto *instance : m_featurePoints) {
		delete instance;
	}
}

const char *PTEID_FaceInfo::version() const { return m_impl.version().c_str(); }

unsigned short PTEID_FaceInfo::encodingBytes() const { return m_impl.encodingBytes(); }

long PTEID_FaceInfo::sizeOfRecord() const { return m_impl.sizeOfRecord(); }

long PTEID_FaceInfo::numberOfFacialImages() const { return m_impl.numberOfFacialImages(); }

std::vector<PTEID_FaceInfoData *> PTEID_FaceInfo::faceInfoData() const { return m_faceInfoDataVec; }

PTEID_FaceInfo::PTEID_FaceInfo(const SDK_Context *context, FaceInfo &face) : PTEID_Object(context, NULL), m_impl(face) {
	for (auto &instance : m_impl.faceInfoData()) {
		m_faceInfoDataVec.push_back(new PTEID_FaceInfoData(context, *instance));
	}
}

PTEID_FaceInfo::~PTEID_FaceInfo() {
	for (auto *instance : m_faceInfoDataVec) {
		delete instance;
	}
}

PTEID_ByteArray PTEID_BiometricInfomation::icaoHeaderVersion() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->icaoHeaderVersion());
}

PTEID_ByteArray PTEID_BiometricInfomation::type() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->type());
}

PTEID_ByteArray PTEID_BiometricInfomation::subType() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->subType());
}

PTEID_ByteArray PTEID_BiometricInfomation::creationDateAndtime() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->creationDateAndtime());
}

PTEID_ByteArray PTEID_BiometricInfomation::validPeriod() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->validPeriod());
}

PTEID_ByteArray PTEID_BiometricInfomation::creatorOfBiometricRefData() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->creatorOfBiometricRefData());
}

PTEID_ByteArray PTEID_BiometricInfomation::formatOwner() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->formatOwner());
}

PTEID_ByteArray PTEID_BiometricInfomation::formatType() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->formatType());
}

PTEID_FaceInfo *PTEID_BiometricInfomation::faceInfo() const { return m_faceInfo; }

PTEID_BiometricInfomation::~PTEID_BiometricInfomation() { delete m_faceInfo; }

PTEID_BiometricInfomation::PTEID_BiometricInfomation(const SDK_Context *context, BiometricInformation &bioInfo)
	: PTEID_Object(context, NULL), m_impl(bioInfo), m_faceInfo(new PTEID_FaceInfo(context, *bioInfo.faceInfo())) {}

PTEID_ICAO_DG2::PTEID_ICAO_DG2(const SDK_Context *context, const IcaoDg2 &dg2)
	: PTEID_Object(context, NULL), m_impl(dg2) {
	for (auto &instance : m_impl.biometricInstances()) {
		m_biometricInstances.push_back(new PTEID_BiometricInfomation(m_context, *instance));
	}
}

PTEID_ByteArray PTEID_ICAO_DG11::listOfTags() const { return PTEID_ByteArray(m_context, m_impl.listOfTags()); }

const char *PTEID_ICAO_DG11::fullName() const { return m_impl.fullName().c_str(); }

const char *PTEID_ICAO_DG11::personalNumber() const
{
  return m_impl.personalNumber().c_str();
}

const char *PTEID_ICAO_DG11::fullDateOfBirth() const { return m_impl.fullDateOfBirth().c_str(); }

const char *PTEID_ICAO_DG11::placeOfBirth() const { return m_impl.placeOfBirth().c_str(); }

const char *PTEID_ICAO_DG11::permanentAddress() const { return m_impl.permanentAddress().c_str(); }

const char *PTEID_ICAO_DG11::telephone() const { return m_impl.telephone().c_str(); }

const char *PTEID_ICAO_DG11::profession() const { return m_impl.profession().c_str(); }

const char *PTEID_ICAO_DG11::title() const { return m_impl.title().c_str(); }

const char *PTEID_ICAO_DG11::personalSummary() const { return m_impl.personalSummary().c_str(); }

const char *PTEID_ICAO_DG11::proofOfCitizenship() const { return m_impl.proofOfCitizenship().c_str(); }

const char *PTEID_ICAO_DG11::otherValidTDNumbers() const { return m_impl.otherValidTDNumbers().c_str(); }

const char *PTEID_ICAO_DG11::custodyInformation() const { return m_impl.custodyInformation().c_str(); }

int PTEID_ICAO_DG11::numberOfOtherNames() const { return m_impl.numberOfOtherNames(); }

const char *PTEID_ICAO_DG11::otherNames() const { return m_impl.otherNames().c_str(); }

PTEID_ICAO_DG11::~PTEID_ICAO_DG11() {}

PTEID_ICAO_DG11::PTEID_ICAO_DG11(const SDK_Context *context, const IcaoDg11 &dg11)
	: PTEID_Object(context, NULL), m_impl(dg11) {}

unsigned int PTEID_ICAO_DG2::numberOfBiometrics() const { return m_impl.numberOfBiometrics(); }

std::vector<PTEID_BiometricInfomation *> PTEID_ICAO_DG2::biometricInstances() { return m_biometricInstances; }

PTEID_ICAO_DG2::~PTEID_ICAO_DG2() {
	for (auto *instance : m_biometricInstances) {
		delete instance;
	}
}

PTEID_ICAO_DG1 *ICAO_Card::readDataGroup1() {
	PTEID_ICAO_DG1 *dg1;
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	dg1 = new PTEID_ICAO_DG1(*icao->readDataGroup1());
	END_TRY_CATCH
	return dg1;
}

PTEID_ICAO_DG2 *ICAO_Card::readDataGroup2() {
	PTEID_ICAO_DG2 *dg2;
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	IcaoDg2 *dg2Impl = icao->readDataGroup2();
	dg2 = new PTEID_ICAO_DG2(m_context, *dg2Impl);
	END_TRY_CATCH
	return dg2;
}

PTEID_ICAO_DG11 *ICAO_Card::readDataGroup11() {
	PTEID_ICAO_DG11 *dg11;
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	IcaoDg11 *dg11Impl = icao->readDataGroup11();
	dg11 = new PTEID_ICAO_DG11(m_context, *dg11Impl);
	END_TRY_CATCH
	return dg11;
}

void ICAO_Card::loadMasterList(const char *filePath) {
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	icao->loadMasterList(filePath);
	END_TRY_CATCH
}

/*****************************************************************************************
----------------------------- PTEID_XmlUserRequestedInfo ---------------------------------
*****************************************************************************************/
PTEID_XmlUserRequestedInfo::PTEID_XmlUserRequestedInfo() : PTEID_Object(NULL, NULL) {
	customXml = new APL_XmlUserRequestedInfo();
}

PTEID_XmlUserRequestedInfo::PTEID_XmlUserRequestedInfo(const char *timeStamp, const char *serverName,
													   const char *serverAddress)
	: PTEID_Object(NULL, NULL) {
	customXml = new APL_XmlUserRequestedInfo(timeStamp, serverName, serverAddress);
}

PTEID_XmlUserRequestedInfo::PTEID_XmlUserRequestedInfo(const char *timeStamp, const char *serverName,
													   const char *serverAddress, const char *tokenID)
	: PTEID_Object(NULL, NULL) {
	customXml = new APL_XmlUserRequestedInfo(timeStamp, serverName, serverAddress, tokenID);
}

PTEID_XmlUserRequestedInfo::~PTEID_XmlUserRequestedInfo() {
	if (customXml)
		delete customXml;
}

void PTEID_XmlUserRequestedInfo::add(XMLUserData xmlUData) { customXml->add(xmlUData); }

PTEIDSDK_API PTEID_ByteArray PTEID_CVC_Init(PTEID_ByteArray cvc_cert) {
	unsigned char challenge[128];
	long ret = ::PTEID_CVC_Init(cvc_cert.GetBytes(), cvc_cert.Size(), challenge, sizeof(challenge));

	if (ret != PTEID_OK) {
		throw PTEID_Exception(ret);
	}

	return PTEID_ByteArray(challenge, sizeof(challenge));
}

PTEIDSDK_API PTEID_ByteArray PTEID_CVC_ReadFile(PTEID_ByteArray fileID) {
	// Should be enough for any file on the card
	unsigned char buffer[15000];
	unsigned long outlen = sizeof(buffer);

	long ret = ::PTEID_CVC_ReadFile(fileID.GetBytes(), fileID.Size(), buffer, &outlen);

	if (ret != PTEID_OK) {
		throw PTEID_Exception(ret);
	}
	return PTEID_ByteArray(buffer, outlen);
}

PTEIDSDK_API void PTEID_CVC_Authenticate(PTEID_ByteArray signedChallenge) {
	long ret = ::PTEID_CVC_Authenticate((unsigned char *)signedChallenge.GetBytes(), signedChallenge.Size());

	if (ret != PTEID_OK)
		throw PTEID_Exception(ret);
}

// This is needed so that the native PTEID_CVC_* methods can be called without calling PTEID_Init()
PTEIDSDK_API void setCompatReaderContext(PTEID_ReaderContext *ctx) { ::readerContext = ctx; }

} // Namespace eIDMW

/*****************************************************************************************
----------------------------- pteid compatibility layer ---------------------------------
*****************************************************************************************/

// The compatibility functions live outside the eIDMW namespace so we have to explicitly qualify these classes
using eIDMW::APL_Card;
using eIDMW::CByteArray;
using eIDMW::CMWException;
using eIDMW::PTEID_Address;
using eIDMW::PTEID_ByteArray;
using eIDMW::PTEID_CardType;
using eIDMW::PTEID_CARDTYPE_IAS07;
using eIDMW::PTEID_CARDTYPE_IAS101;
using eIDMW::PTEID_CardVersionInfo;
using eIDMW::PTEID_Certificate;
using eIDMW::PTEID_Certificates;
using eIDMW::PTEID_Config;
using eIDMW::PTEID_EId;
using eIDMW::PTEID_EIDCard;
using eIDMW::PTEID_Exception;
using eIDMW::PTEID_ExNoCardPresent;
using eIDMW::PTEID_ExNoReader;
using eIDMW::PTEID_PARAM_GENERAL_LANGUAGE;
using eIDMW::PTEID_Photo;
using eIDMW::PTEID_Pin;
using eIDMW::PTEID_Pins;
using eIDMW::PTEID_PublicKey;
using eIDMW::PTEID_ReaderContext;
using eIDMW::PTEID_ReaderSet;
using eIDMW::SecurityContext;

// This object should be initialized by calling PTEID_CVC_Init() and freed after a CVC_WriteFile() or CVC_ReadFile()
// operation It will hold all the keys and other values necessary for Secure Messaging operations
SecurityContext *securityContext = NULL;

PTEIDSDK_API long PTEID_Init(char *ReaderName) {

	try {
		if (NULL == ReaderName || strlen(ReaderName) == 0)
			readerContext = &ReaderSet.getReader();
		else
			readerContext = &ReaderSet.getReaderByName(ReaderName);

		PTEID_EIDCard &card = readerContext->getEIDCard();

		// Init the compatibility method with PT language
		PTEID_Config config(PTEID_PARAM_GENERAL_LANGUAGE);
		config.setString("nl");
	} catch (PTEID_ExNoCardPresent &) {
		return SC_ERROR_CARD_NOT_PRESENT;
	} catch (PTEID_ExNoReader &) {
		return SC_ERROR_NO_READERS_FOUND;
	} catch (PTEID_Exception &ex) {
		std::cerr << "PTEID_Exception exception " << ex.GetError() << std::endl;
	} catch (...) {
		return -1;
	}
	return PTEID_OK;
}

PTEIDSDK_API long PTEID_Exit(unsigned long ulMode) {
	if (readerContext != NULL) {
		readerContext->Release();
		readerContext = NULL;
	}

	PTEID_ReleaseSDK();
	return PTEID_OK;
}

PTEIDSDK_API tCompCardType PTEID_GetCardType() {

	if (readerContext != NULL) {
		PTEID_CardType type = readerContext->getCardType();
		switch (type) {
		case PTEID_CARDTYPE_IAS07:
			return COMP_CARD_TYPE_IAS07;
			break;
		case PTEID_CARDTYPE_IAS101:
			return COMP_CARD_TYPE_IAS101;
			break;
		default:
			return COMP_CARD_TYPE_ERR;
		}
	}
	return COMP_CARD_TYPE_ERR;
}

PTEIDSDK_API long PTEID_GetID(PTEID_ID *IDData) {

	if (readerContext != NULL) {
		try {
			PTEID_EId &id = readerContext->getEIDCard().getID();

			memset(IDData, '\0', sizeof(PTEID_ID));

			IDData->version = 0;
			strncpy(IDData->deliveryEntity, id.getIssuingEntity(), PTEID_MAX_DELIVERY_ENTITY_LEN - 1);
			strncpy(IDData->country, id.getCountry(), PTEID_MAX_COUNTRY_LEN - 1);
			strncpy(IDData->documentType, id.getDocumentType(), PTEID_MAX_DOCUMENT_TYPE_LEN - 1);
			strncpy(IDData->cardNumber, id.getDocumentNumber(), PTEID_MAX_CARDNUMBER_LEN - 1);
			strncpy(IDData->cardNumberPAN, id.getDocumentPAN(), PTEID_MAX_CARDNUMBER_PAN_LEN - 1);
			strncpy(IDData->cardVersion, id.getDocumentVersion(), PTEID_MAX_CARDVERSION_LEN - 1);
			strncpy(IDData->deliveryDate, id.getValidityBeginDate(), PTEID_MAX_DATE_LEN - 1);
			strncpy(IDData->locale, id.getLocalofRequest(), PTEID_MAX_LOCALE_LEN - 1);
			strncpy(IDData->validityDate, id.getValidityEndDate(), PTEID_MAX_DATE_LEN - 1);
			strncpy(IDData->name, id.getSurname(), PTEID_MAX_NAME_LEN - 1);
			strncpy(IDData->firstname, id.getGivenName(), PTEID_MAX_NAME_LEN - 1);
			strncpy(IDData->sex, id.getGender(), PTEID_MAX_SEX_LEN - 1);
			strncpy(IDData->nationality, id.getNationality(), PTEID_MAX_NATIONALITY_LEN - 1);
			strncpy(IDData->birthDate, id.getDateOfBirth(), PTEID_MAX_DATE_LEN - 1);
			strncpy(IDData->height, id.getHeight(), PTEID_MAX_HEIGHT_LEN - 1);
			strncpy(IDData->numBI, id.getCivilianIdNumber(), PTEID_MAX_NUMBI_LEN - 1);
			strncpy(IDData->nameFather, id.getSurnameFather(), PTEID_MAX_NAME_LEN - 1);
			strncpy(IDData->firstnameFather, id.getGivenNameFather(), PTEID_MAX_NAME_LEN - 1);
			strncpy(IDData->nameMother, id.getSurnameMother(), PTEID_MAX_NAME_LEN - 1);
			strncpy(IDData->firstnameMother, id.getGivenNameMother(), PTEID_MAX_NAME_LEN - 1);
			strncpy(IDData->numNIF, id.getTaxNo(), PTEID_MAX_NUMNIF_LEN - 1);
			strncpy(IDData->numSS, id.getSocialSecurityNumber(), PTEID_MAX_NUMSS_LEN - 1);
			strncpy(IDData->numSNS, id.getHealthNumber(), PTEID_MAX_NUMSNS_LEN - 1);
			strncpy(IDData->notes, id.getAccidentalIndications(), PTEID_MAX_INDICATIONEV_LEN - 1);
			strncpy(IDData->mrz1, id.getMRZ1(), PTEID_MAX_MRZ_LEN - 1);
			strncpy(IDData->mrz2, id.getMRZ2(), PTEID_MAX_MRZ_LEN - 1);
			strncpy(IDData->mrz3, id.getMRZ3(), PTEID_MAX_MRZ_LEN - 1);
		} catch (PTEID_Exception &ex) {
			long errorCode = ex.GetError();

			if (errorCode >= EIDMW_SOD_UNEXPECTED_VALUE && errorCode <= EIDMW_SOD_ERR_VERIFY_SOD_SIGN) {
				return SC_ERROR_OBJECT_NOT_VALID;
			} else {
				return SC_ERROR_INTERNAL;
			}
		}
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_GetAddr(PTEID_ADDR *AddrData) {
	unsigned long remaining_tries;

	if (readerContext != NULL) {

		try {

			PTEID_Pins &pins = readerContext->getEIDCard().getPins();
			for (unsigned long PinIdx = 0; PinIdx < pins.count(); PinIdx++) {
				PTEID_Pin &Pin = pins.getPinByNumber(PinIdx);
				if (strstr(Pin.getLabel(), "PIN da Morada")) {
					if (!Pin.verifyPin("", remaining_tries))
						return -1;
					break;
				}
			}

			PTEID_Address &addr = readerContext->getEIDCard().getAddr();

			memset(AddrData, '\0', sizeof(PTEID_ADDR));

			AddrData->version = 0;
			strncpy(AddrData->country, addr.getCountryCode(), PTEID_ADDR_COUNTRY_LEN);

			if (addr.isNationalAddress()) {
				strncpy(AddrData->addrType, COMP_LAYER_NATIONAL_ADDRESS, PTEID_ADDR_TYPE_LEN);
				strncpy(AddrData->district, addr.getDistrictCode(), PTEID_DISTRICT_LEN);
				strncpy(AddrData->districtDesc, addr.getDistrict(), PTEID_DISTRICT_DESC_LEN);
				strncpy(AddrData->municipality, addr.getMunicipalityCode(), PTEID_DISTRICT_CON_LEN);
				strncpy(AddrData->municipalityDesc, addr.getMunicipality(), PTEID_DISTRICT_CON_DESC_LEN);
				strncpy(AddrData->freguesia, addr.getCivilParishCode(), PTEID_DISTRICT_FREG_LEN);
				strncpy(AddrData->freguesiaDesc, addr.getCivilParish(), PTEID_DISTRICT_FREG_DESC_LEN);
				strncpy(AddrData->streettypeAbbr, addr.getAbbrStreetType(), PTEID_ROAD_ABBR_LEN);
				strncpy(AddrData->streettype, addr.getStreetType(), PTEID_ROAD_LEN);
				strncpy(AddrData->street, addr.getStreetName(), PTEID_ROAD_DESIG_LEN);
				strncpy(AddrData->buildingAbbr, addr.getAbbrBuildingType(), PTEID_HOUSE_ABBR_LEN);
				strncpy(AddrData->building, addr.getBuildingType(), PTEID_HOUSE_LEN);
				strncpy(AddrData->door, addr.getDoorNo(), PTEID_NUMDOOR_LEN);
				strncpy(AddrData->floor, addr.getFloor(), PTEID_FLOOR_LEN);
				strncpy(AddrData->side, addr.getSide(), PTEID_SIDE_LEN);
				strncpy(AddrData->place, addr.getPlace(), PTEID_PLACE_LEN);
				strncpy(AddrData->locality, addr.getLocality(), PTEID_LOCALITY_LEN);
				strncpy(AddrData->cp4, addr.getZip4(), PTEID_CP4_LEN);
				strncpy(AddrData->cp3, addr.getZip3(), PTEID_CP3_LEN);
				strncpy(AddrData->postal, addr.getPostalLocality(), PTEID_POSTAL_LEN);
				strncpy(AddrData->numMor, addr.getGeneratedAddressCode(), PTEID_NUMMOR_LEN);
			} else {
				strncpy(AddrData->addrType, COMP_LAYER_FOREIGN_ADDRESS, PTEID_ADDR_TYPE_LEN);
				strncpy(AddrData->countryDescF, addr.getForeignCountry(), PTEID_ADDR_COUNTRYF_DESC_LEN);
				strncpy(AddrData->addressF, addr.getForeignAddress(), PTEID_ADDRF_LEN);
				strncpy(AddrData->cityF, addr.getForeignCity(), PTEID_CITYF_LEN);
				strncpy(AddrData->regioF, addr.getForeignRegion(), PTEID_REGIOF_LEN);
				strncpy(AddrData->localityF, addr.getForeignLocality(), PTEID_LOCALITYF_LEN);
				strncpy(AddrData->postalF, addr.getForeignPostalCode(), PTEID_POSTALF_LEN);
				strncpy(AddrData->numMorF, addr.getGeneratedAddressCode(), PTEID_NUMMOR_LEN);
			}
		} catch (PTEID_Exception &ex) {
			long errorCode = ex.GetError();
			if (errorCode >= EIDMW_SOD_UNEXPECTED_VALUE && errorCode <= EIDMW_SOD_ERR_VERIFY_SOD_SIGN) {
				return SC_ERROR_OBJECT_NOT_VALID;
			} else if (errorCode == EIDMW_ERR_PIN_CANCEL) {
				return SC_ERROR_KEYPAD_CANCELLED;
			} else {
				return SC_ERROR_INTERNAL;
			}
		}
	}
	return PTEID_OK;
}

PTEIDSDK_API long PTEID_GetPic(PTEID_PIC *PicData) {

	if (readerContext != NULL) {

		PTEID_Photo &photoOjb = readerContext->getEIDCard().getID().getPhotoObj();

		memset(PicData, 0, sizeof(PTEID_PIC));

		PTEID_ByteArray &scratch = photoOjb.getphotoRAW();
		memcpy(PicData->picture, scratch.GetBytes(),
			   (PTEID_MAX_PICTUREH_LEN >= scratch.Size()) ? scratch.Size() : PTEID_MAX_PICTUREH_LEN);
		PicData->piclength = scratch.Size();
		scratch = photoOjb.getphotoCbeff();
		memcpy(PicData->cbeff, scratch.GetBytes(),
			   (PTEID_MAX_CBEFF_LEN >= scratch.Size()) ? scratch.Size() : PTEID_MAX_CBEFF_LEN);
		scratch = photoOjb.getphotoFacialrechdr();
		memcpy(PicData->facialrechdr, scratch.GetBytes(),
			   (PTEID_MAX_FACRECH_LEN >= scratch.Size()) ? scratch.Size() : PTEID_MAX_FACRECH_LEN);
		scratch = photoOjb.getphotoFacialinfo();
		memcpy(PicData->facialinfo, scratch.GetBytes(),
			   (PTEID_MAX_FACINFO_LEN >= scratch.Size()) ? scratch.Size() : PTEID_MAX_FACINFO_LEN);
		scratch = photoOjb.getphotoImageinfo();
		memcpy(PicData->imageinfo, scratch.GetBytes(),
			   (PTEID_MAX_IMAGEINFO_LEN >= scratch.Size()) ? scratch.Size() : PTEID_MAX_IMAGEINFO_LEN);
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_GetCertificates(PTEID_Certifs *Certifs) {
	if (readerContext != NULL) {
		PTEID_Certificates &certificates = readerContext->getEIDCard().getCertificates();
		PTEID_ByteArray ba;
		unsigned long i = 0;
		memset(Certifs, 0, sizeof(PTEID_Certifs));

		for (; i < certificates.countAll(); i++) {
			if (i == PTEID_MAX_CERT_NUMBER)
				break;
			PTEID_Certificate &cert = certificates.getCert(i);
			cert.getFormattedData(ba);
			memcpy(Certifs->certificates[i].certif, ba.GetBytes(),
				   (PTEID_MAX_CERT_LEN >= ba.Size()) ? ba.Size() : PTEID_MAX_CERT_LEN);
			Certifs->certificates[i].certifLength = (PTEID_MAX_CERT_LEN >= ba.Size()) ? ba.Size() : PTEID_MAX_CERT_LEN;
			strncpy(Certifs->certificates[i].certifLabel, cert.getLabel(),
					(PTEID_MAX_CERT_LABEL_LEN >= strlen(cert.getLabel()) ? strlen(cert.getLabel())
																		 : PTEID_MAX_CERT_LABEL_LEN - 1));
		}
		Certifs->certificatesLength = i;
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_VerifyPIN(unsigned char PinId, char *Pin, long *triesLeft) {
	unsigned long tleft = -1;
	bool ret;

	if (readerContext != NULL) {
		if (PinId != 1 && PinId != 129 && PinId != 130 && PinId != 131) {
			return PTEID_OK;
		}

		try {
			PTEID_Pins &pins = readerContext->getEIDCard().getPins();
			for (unsigned long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
				PTEID_Pin &pin = pins.getPinByNumber(pinIdx);
				if (pin.getPinRef() == PinId) {
					if (Pin != NULL)
						ret = pin.verifyPin(Pin, tleft, false);
					else
						ret = pin.verifyPin("", tleft);
					// martinho: verify pin is not working properly for readers without pinpad at this moment,
					// this is a workaround
					*triesLeft = pin.getTriesLeft();

					if (ret)
						return PTEID_OK;
					else if (*triesLeft == 0)
						return SC_ERROR_AUTH_METHOD_BLOCKED;
					else
						return SC_ERROR_PIN_CODE_INCORRECT;
				}
			}
		} catch (PTEID_Exception &ex) {
			long errorCode = ex.GetError();
			if (errorCode == EIDMW_ERR_TIMEOUT) {
				return SC_ERROR_KEYPAD_TIMEOUT;
			} else if (errorCode == EIDMW_ERR_PIN_CANCEL) {
				return SC_ERROR_KEYPAD_CANCELLED;
			} else {
				return SC_ERROR_AUTH_METHOD_BLOCKED;
			}
		}
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_VerifyPIN_No_Alert(unsigned char PinId, char *Pin, long *triesLeft) {

	if (readerContext != NULL) {
		// martinho: in this phase we do not have a distinct functionality for this feature.
		return PTEID_VerifyPIN(PinId, Pin, triesLeft);
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_ChangePIN(unsigned char PinId, char *pszOldPin, char *pszNewPin, long *triesLeft) {

	const char *a1 = pszOldPin, *a2 = pszNewPin;
	unsigned long int tries = -1;

	if (readerContext != NULL) {
		if (PinId != 1 && PinId != 129 && PinId != 130 && PinId != 131) {
			return PTEID_OK;
		}

		PTEID_Pins &pins = readerContext->getEIDCard().getPins();
		for (unsigned long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
			PTEID_Pin &pin = pins.getPinByNumber(pinIdx);
			if (pin.getPinRef() == PinId) {
				if (pin.changePin(pszOldPin, pszNewPin, tries, pin.getLabel())) {
					*triesLeft = pin.getTriesLeft();
					return PTEID_OK;
				} else
					return -1;
			} else
				return PTEID_E_BAD_PARAM;
		}
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_GetPINs(PTEIDPins *Pins) {
	long int i = 0;
	unsigned long currentId = 0;

	if (readerContext != NULL) {
		PTEID_Pins &pins = readerContext->getEIDCard().getPins();
		for (unsigned long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
			PTEID_Pin &pin = pins.getPinByNumber(pinIdx);
			if (pin.getId() == 1 || pin.getId() == 2 || pin.getId() == 3) {
				currentId = pin.getId() - 1;
				Pins->pins[currentId].flags = pin.getFlags();
				Pins->pins[currentId].usageCode =
					pin.getId(); // martinho: might not be the intended use, but gives the expected compatible result.
				Pins->pins[currentId].pinType = pin.getType();
				memset(Pins->pins[currentId].label, '\0', PTEID_MAX_PIN_LABEL_LEN);
				strncpy(Pins->pins[currentId].label, pin.getLabel(),
						(PTEID_MAX_PIN_LABEL_LEN > strlen(pin.getLabel()) ? strlen(pin.getLabel())
																		  : PTEID_MAX_PIN_LABEL_LEN - 1));
				Pins->pins[currentId].triesLeft = pin.getTriesLeft();
				assert(pin.getPinRef() <= UCHAR_MAX);
				Pins->pins[currentId].id = (unsigned char) pin.getPinRef();
				Pins->pins[currentId].shortUsage =
					NULL; // martinho don't know where it is used, current MW returns NULL also
				Pins->pins[currentId].longUsage =
					NULL; // martinho don't know where it is used, current MW returns NULL also
				i++;
			}
		}
		Pins->pinsLength = i;
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_GetTokenInfo(PTEID_TokenInfo *tokenData) {

	if (readerContext != NULL) {
		PTEID_CardVersionInfo &versionInfo = readerContext->getEIDCard().getVersionInfo();

		memset(tokenData->label, '\0', PTEID_MAX_CERT_LABEL_LEN);
		strncpy(tokenData->label, versionInfo.getTokenLabel(),
				(PTEID_MAX_CERT_LABEL_LEN > strlen(versionInfo.getTokenLabel()) ? strlen(versionInfo.getTokenLabel())
																				: PTEID_MAX_CERT_LABEL_LEN - 1));
		memset(tokenData->serial, '\0', PTEID_MAX_ID_NUMBER_LEN);
		strncpy(tokenData->serial, versionInfo.getSerialNumber(),
				(PTEID_MAX_ID_NUMBER_LEN > strlen(versionInfo.getSerialNumber()) ? strlen(versionInfo.getSerialNumber())
																				 : PTEID_MAX_ID_NUMBER_LEN - 1));
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_ReadSOD(unsigned char *out, unsigned long *outlen) {
	if (readerContext != NULL) {
		PTEID_ByteArray temp;
		PTEID_EIDCard &card = readerContext->getEIDCard();

		temp = card.getSod().getData();
		CByteArray cb((unsigned char *)temp.GetBytes(), temp.Size());
		/*cb.TrimRight(0);*/
		memset(out, 0, *outlen);
		if (cb.Size() < *outlen)
			*outlen = cb.Size();
		memcpy(out, cb.GetBytes(), *outlen);
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_UnblockPIN(unsigned char PinId, char *pszPuk, char *pszNewPin, long *triesLeft) {

	return PTEID_UnblockPIN_Ext(PinId, pszPuk, pszNewPin, triesLeft, UNBLOCK_FLAG_NEW_PIN);
}

PTEIDSDK_API long PTEID_UnblockPIN_Ext(unsigned char PinId, char *pszPuk, char *pszNewPin, long *triesLeft,
									   unsigned long ulFlags) {
	unsigned long tleft;

	if (readerContext != NULL) {
		if (PinId != 1 && PinId != 129 && PinId != 130 && PinId != 131) {
			return PTEID_OK;
		}

		try {
			PTEID_Pins &pins = readerContext->getEIDCard().getPins();
			for (unsigned long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
				PTEID_Pin &pin = pins.getPinByNumber(pinIdx);
				if (pin.getPinRef() == PinId) {
					bool ret = pin.unlockPin(pszPuk, pszNewPin, tleft, ulFlags);
					*triesLeft = tleft;
					return ret ? PTEID_OK : SC_ERROR_PIN_CODE_INCORRECT;
				}
			}
		} catch (PTEID_Exception &ex) {
			long errorCode = ex.GetError();
			if (errorCode == EIDMW_ERR_TIMEOUT) {
				return SC_ERROR_KEYPAD_TIMEOUT;
			} else if (errorCode == EIDMW_ERR_PIN_CANCEL) {
				return SC_ERROR_KEYPAD_CANCELLED;
			} else {
				return PTEID_E_NOT_INITIALIZED;
			}
		}
	}

	return PTEID_E_NOT_INITIALIZED;
}

PTEIDSDK_API long PTEID_SelectADF(unsigned char *adf, long adflen) {

	if (readerContext != NULL) {

		try {

			PTEID_EIDCard &card = readerContext->getEIDCard();
			unsigned char ap[4] = {0x00, 0xA4, 0x00, 0x0C};
			PTEID_ByteArray apdu(ap, (unsigned long)(sizeof(ap) / sizeof(unsigned char)));
			apdu.Append((unsigned char *)&adflen, sizeof(unsigned char));
			apdu.Append(adf, (unsigned long)adflen);
			card.sendAPDU(apdu);
		} catch (PTEID_Exception &) {
			return PTEID_E_NOT_INITIALIZED;
		}
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_ReadFile(unsigned char *file, int filelen, unsigned char *out, unsigned long *outlen,
								 unsigned char PinId) {

	if (readerContext != NULL && (PinId == PTEID_ADDRESS_PIN_ID || PinId == PTEID_NO_PIN_NEEDED)) {
		PTEID_EIDCard &card = readerContext->getEIDCard();
		CByteArray filePath;
		PTEID_ByteArray in;
		PTEID_Pin *pin = NULL;

		if (PinId != PTEID_NO_PIN_NEEDED) {
			PTEID_Pins &pins = readerContext->getEIDCard().getPins();
			for (unsigned long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
				pin = &(pins.getPinByNumber(pinIdx));
				if (pin->getPinRef() == PTEID_ADDRESS_PIN_ID) {
					break;
				}
			}
		}

		filePath.Append(file, filelen);
		card.readFile(filePath.ToString(false).c_str(), in, pin, "");

		CByteArray cb(in.GetBytes(), in.Size());

		*outlen = (*outlen > cb.Size() ? cb.Size() : *outlen);
		memcpy(out, cb.GetBytes(), *outlen);
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_WriteFile(unsigned char *file, int filelen, unsigned char *in, unsigned long inlen,
								  unsigned char PinId) {
	return PTEID_WriteFile_inOffset(file, filelen, in, 0 /* inOffset */, inlen, PinId);
}

PTEIDSDK_API long PTEID_WriteFile_inOffset(unsigned char *file, int filelen, unsigned char *in, unsigned long inOffset,
										   unsigned long inlen, unsigned char PinId) {
	if (readerContext != NULL && (PinId == 1 || PinId == 129)) {
		PTEID_EIDCard &card = readerContext->getEIDCard();
		CByteArray temp;
		PTEID_ByteArray out;
		PTEID_Pin *pin = NULL;

		if (PinId != PTEID_NO_PIN_NEEDED) {
			PinId = 1;
			PTEID_Pins &pins = readerContext->getEIDCard().getPins();
			for (unsigned long pinIdx = 0; pinIdx < pins.count(); pinIdx++) {
				pin = &(pins.getPinByNumber(pinIdx));
				if (pin->getId() == PinId)
					break;
			}
		}

		out.Append(in, inlen);
		temp.Append(file, filelen);
		if (card.writeFile(temp.ToString(false).c_str(), out, pin, "", inOffset)) {
			return PTEID_OK;
		}
		return -1;
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_IsActivated(unsigned long *pulStatus) {

	if (readerContext != NULL) {
		*pulStatus = (readerContext->getEIDCard().isActive() ? PTEID_ACTIVE_CARD : PTEID_INACTIVE_CARD);
	}
	return PTEID_OK;
}

PTEIDSDK_API long PTEID_Activate(char *pszPin, unsigned char *pucDate, unsigned long ulMode) {
	long retval = 0;
	if (readerContext != NULL) {
		try {

			PTEID_ByteArray bcd(pucDate, BCD_DATE_LEN);
			if (readerContext->getEIDCard().Activate(pszPin, bcd, ulMode == MODE_ACTIVATE_BLOCK_PIN)) {
				return PTEID_OK;
			}
			return -1;
		} catch (PTEID_Exception &) {
			return PTEID_E_INTERNAL;
		}
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_SetSODChecking(int bDoCheck) {
	if (readerContext != NULL)
		readerContext->getEIDCard().doSODCheck(bDoCheck != 0);

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_SetSODCAs(PTEID_Certifs *Certifs) {
	if (readerContext != NULL) {

		PTEID_Certificates &_certifs = readerContext->getEIDCard().getCertificates();

		// if called with Certifs = NULL revert to use the default SOD CAs
		if (Certifs == NULL) {
			_certifs.resetSODCAs();
			return PTEID_OK;
		}

		for (int i = 0; i < Certifs->certificatesLength; i++) {
			PTEID_ByteArray *pba =
				new PTEID_ByteArray(Certifs->certificates[i].certif, Certifs->certificates[i].certifLength);
			_certifs.addToSODCAs(*pba);
			delete pba;
		}
		return PTEID_OK;
	}

	return PTEID_E_NOT_INITIALIZED;
}

PTEIDSDK_API long PTEID_GetCardAuthenticationKey(PTEID_RSAPublicKey *pCardAuthPubKey) {
	if (readerContext != NULL) {
		PTEID_PublicKey &cardKey = readerContext->getEIDCard().getID().getCardAuthKeyObj();

		memcpy(pCardAuthPubKey->modulus, cardKey.getCardAuthKeyModulus().GetBytes(),
			   cardKey.getCardAuthKeyModulus().Size());
		pCardAuthPubKey->modulusLength = cardKey.getCardAuthKeyModulus().Size();
		memcpy(pCardAuthPubKey->exponent, cardKey.getCardAuthKeyExponent().GetBytes(),
			   cardKey.getCardAuthKeyExponent().Size());
		assert(cardKey.getCardAuthKeyExponent().Size() <= UCHAR_MAX);
		pCardAuthPubKey->exponentLength = (unsigned char) cardKey.getCardAuthKeyExponent().Size();
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_GetCVCRoot(PTEID_RSAPublicKey *pCVCRootKey) {
	if (readerContext != NULL) {
		PTEID_PublicKey &rootCAKey = readerContext->getEIDCard().getRootCAPubKey();

		memcpy(pCVCRootKey->modulus, rootCAKey.getCardAuthKeyModulus().GetBytes(),
			   rootCAKey.getCardAuthKeyModulus().Size());
		pCVCRootKey->modulusLength = rootCAKey.getCardAuthKeyModulus().Size();
		memcpy(pCVCRootKey->exponent, rootCAKey.getCardAuthKeyExponent().GetBytes(),
			   rootCAKey.getCardAuthKeyExponent().Size());
		assert(rootCAKey.getCardAuthKeyExponent().Size() <= UCHAR_MAX);
		pCVCRootKey->exponentLength = (unsigned char) rootCAKey.getCardAuthKeyExponent().Size();
	}

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_SendAPDU(const unsigned char *ucRequest, unsigned long ulRequestLen, unsigned char *ucResponse,
								 unsigned long *ulResponseLen) {
	if (readerContext != NULL) {
		PTEID_EIDCard &card = readerContext->getEIDCard();
		PTEID_ByteArray sApdu(ucRequest, ulRequestLen);
		PTEID_ByteArray resp;

		resp = card.sendAPDU(sApdu);

		*ulResponseLen = (*ulResponseLen > resp.Size()) ? resp.Size() : *ulResponseLen;
		memcpy(ucResponse, resp.GetBytes(), *ulResponseLen);
	}

	return PTEID_OK;
}

PTEIDSDK_API int PTEID_IsPinpad() {

	if (readerContext != NULL) {
		try {
			return readerContext->isPinpad() ? 1 : 0;
		} catch (PTEID_Exception &) {
			return 0;
		}
	}

	return 0;
}

// Not even sure what this should check in the reader, this function is available here only for backwards compat
PTEIDSDK_API int PTEID_IsEMVCAP() { return PTEID_OK; }

// This should implement all the Mutual Auth Process and generate the input to the Signature that's sent in the
// EXTERNAL AUTHENTICATE command
PTEIDSDK_API long PTEID_CVC_Init(const unsigned char *pucCert, int iCertLen, unsigned char *pucChallenge,
								 int iChallengeLen) {
	unsigned long buffer_len = iChallengeLen;

	if (readerContext != NULL) {
		try {

			PTEID_EIDCard &card = readerContext->getEIDCard();

			// Accessing PTEID_EIDCard internal object, hope he doesn't mind :)
			APL_Card *card_impl = static_cast<APL_Card *>(card.m_impl);
			securityContext = new SecurityContext(card_impl);

			CByteArray cvc_certificate(pucCert, iCertLen);
			securityContext->verifyCVCCertificate(cvc_certificate);

			CByteArray signature_input = securityContext->getExternalAuthenticateChallenge();

			if (buffer_len < signature_input.Size()) {
				// No space in supplied buffer: return error
				return PTEID_E_BAD_PARAM;
			}
			memcpy(pucChallenge, signature_input.GetBytes(), signature_input.Size());
			return PTEID_OK;
		} catch (CMWException &) {
			return PTEID_E_INTERNAL;
		}
	}

	return PTEID_E_NOT_INITIALIZED;
}

PTEIDSDK_API long PTEID_CVC_Authenticate(unsigned char *pucSignedChallenge, int iSignedChallengeLen) {
	if (securityContext != NULL) {
		try {

			CByteArray signed_challenge(pucSignedChallenge, iSignedChallengeLen);

			bool ret = securityContext->verifySignedChallenge(signed_challenge);

			if (ret)
				return PTEID_OK;
			else
				return PTEID_E_INTERNAL;
		} catch (CMWException &) {
			return PTEID_E_INTERNAL;
		}
	}

	return PTEID_E_NOT_INITIALIZED;
}

PTEIDSDK_API long PTEID_CVC_GetAddr(PTEID_ADDR *AddrData) {
	unsigned char addressFile[] = {0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x05};
	unsigned char fileBuffer[1200];
	unsigned int offset = 0;
	unsigned long outLen = sizeof(fileBuffer);

	if (securityContext != NULL) {

		memset(fileBuffer, 0, outLen);

		if (PTEID_CVC_ReadFile(addressFile, sizeof(addressFile), fileBuffer, &outLen) != PTEID_OK) {
			return PTEID_E_INTERNAL;
		}

		AddrData->version = 0;

		// Evil cast but this should work because the PTEID_ADDR struct is 1-byte aligned
		char *ptr = (char *)AddrData;

		if (fileBuffer[0] == 'N') {
			// Skip the manually filled version field
			memcpy(ptr + 2, fileBuffer, outLen);
		} else {
			// Buffer size for the Foreign address specific fields
			size_t foreign_addr_len = 812;
			// Manually write first 2 fields
			strncpy(AddrData->addrType, (const char *)fileBuffer, PTEID_ADDR_TYPE_LEN);
			strncpy(AddrData->country, (const char *)fileBuffer + PTEID_ADDR_TYPE_LEN, PTEID_ADDR_COUNTRY_LEN);

			ptr = (char *)&(AddrData->countryDescF);
			memcpy(ptr, fileBuffer + 6, foreign_addr_len);
		}

		return PTEID_OK;
	}

	return PTEID_E_NOT_INITIALIZED;
}

PTEIDSDK_API long PTEID_CVC_ReadFile(const unsigned char *file, int filelen, unsigned char *out,
									 unsigned long *outlen) {
	CByteArray file_contents;

	if (securityContext != NULL) {
		try {
			file_contents = securityContext->readFile((unsigned char *)file, filelen, *outlen);
		} catch (CMWException &) {
			return PTEID_E_INTERNAL;
		}

		memcpy(out, file_contents.GetBytes(), file_contents.Size());
		*outlen = file_contents.Size();

		return PTEID_OK;
	}

	return PTEID_E_NOT_INITIALIZED;
}

/* The following 5 functions are not implemented on purpose, kept just for backwards compatibility */

PTEIDSDK_API long PTEID_CVC_WriteFile(unsigned char *file, int filelen, unsigned long ulFileOffset,
									  const unsigned char *in, unsigned long inlen, unsigned long ulMode) {

	return PTEID_OK;
}

PTEIDSDK_API long PTEID_CAP_ChangeCapPin(const char *csServer, const unsigned char *ucServerCaCert,
										 unsigned long ulServerCaCertLen, tProxyInfo *proxyInfo, const char *pszOldPin,
										 const char *pszNewPin, long *triesLeft) {
	return PTEID_OK;
}

PTEIDSDK_API tCapPinChangeState PTEID_CAP_GetCapPinChangeProgress() { return CAP_INITIALISING; }

PTEIDSDK_API void PTEID_CAP_SetCapPinChangeCallback(void(_USERENTRY *callback)(tCapPinChangeState state)) { return; }

PTEIDSDK_API void PTEID_CAP_CancelCapPinChange() { return; }
