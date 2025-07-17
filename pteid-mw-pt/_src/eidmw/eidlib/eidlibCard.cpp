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
#include "MiscUtil.h"
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
#include "IcaoDg3.h"
#include "PDFSignature.h"
#include "PhotoPteid.h"
#include "SSLConnection.h"
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

PTEID_ActiveAuthenticationReport::PTEID_ActiveAuthenticationReport(const SDK_Context *context,
																   const EIDMW_ActiveAuthenticationReport &report)
	: PTEID_Object(context, NULL), m_impl(report) {}

PTEID_ByteArray PTEID_ActiveAuthenticationReport::GetDG14() const { return PTEID_ByteArray(m_context, m_impl.dg14); }

PTEID_ByteArray PTEID_ActiveAuthenticationReport::GetDG14ComputedHash() const {
	return PTEID_ByteArray(m_context, m_impl.hashDg14);
}

PTEID_ByteArray PTEID_ActiveAuthenticationReport::GetDG14StoredHash() const {
	return PTEID_ByteArray(m_context, m_impl.storedHashDg14);
}

PTEID_ByteArray PTEID_ActiveAuthenticationReport::GetDG15() const { return PTEID_ByteArray(m_context, m_impl.dg15); }

PTEID_ByteArray PTEID_ActiveAuthenticationReport::GetDG15ComputedHash() const {
	return PTEID_ByteArray(m_context, m_impl.hashDg15);
}

PTEID_ByteArray PTEID_ActiveAuthenticationReport::GetDG15StoredHash() const {
	return PTEID_ByteArray(m_context, m_impl.storedHashDg15);
}

const char *PTEID_ActiveAuthenticationReport::GetOID() const { return m_impl.oid.c_str(); }

long PTEID_ActiveAuthenticationReport::GetStatus() const { return m_impl.error_code; }

const char *PTEID_ActiveAuthenticationReport::GetStatusMessage() const {
	m_statusMessage = CodeToString(m_impl.error_code);
	return m_statusMessage.c_str();
}

/*****************************************************************************************
---------------------------- PTEID_ChipAuthenticationReport ------------------------------
*****************************************************************************************/

PTEID_ChipAuthenticationReport::PTEID_ChipAuthenticationReport(const SDK_Context *context,
															   const EIDMW_ChipAuthenticationReport &report)
	: PTEID_Object(context, NULL), m_impl(report) {}

PTEID_ByteArray PTEID_ChipAuthenticationReport::GetPublicKey() const {
	return PTEID_ByteArray(m_context, m_impl.pubKey);
}

const char *PTEID_ChipAuthenticationReport::GetOID() const { return m_impl.oid.c_str(); }

long PTEID_ChipAuthenticationReport::GetStatus() const { return m_impl.error_code; }

const char *PTEID_ChipAuthenticationReport::GetStatusMessage() const {
	m_statusMessage = CodeToString(m_impl.error_code);
	return m_statusMessage.c_str();
}

/*****************************************************************************************
-------------------------------------- PTEID_SodReport -----------------------------------
*****************************************************************************************/
PTEID_SodReport::PTEID_SodReport(const SDK_Context *context, const EIDMW_SodReport &report)
	: PTEID_Object(context, NULL), m_impl(report) {}

PTEID_ByteArray PTEID_SodReport::GetSigner() const { return PTEID_ByteArray(m_context, m_impl.signer); }

long PTEID_SodReport::GetStatus() const { return m_impl.error_code; }

const char *PTEID_SodReport::GetStatusMessage() const {
	m_statusMessage = CodeToString(m_impl.error_code);
	return m_statusMessage.c_str();
}

/*****************************************************************************************
------------------------------------ PTEID_DocumentReport --------------------------------
*****************************************************************************************/
PTEID_DocumentReport::PTEID_DocumentReport(const SDK_Context *context, const EIDMW_DocumentReport &reports)
	: PTEID_Object(context, NULL), m_impl(reports) {}

PTEID_ActiveAuthenticationReport *PTEID_DocumentReport::GetActiveAuthenticationReport() const {
	PTEID_ActiveAuthenticationReport *report;
	BEGIN_TRY_CATCH

	report = new PTEID_ActiveAuthenticationReport(m_context, m_impl.getActiveAuthenticationReport());

	END_TRY_CATCH
	return report;
}

PTEID_ChipAuthenticationReport *PTEID_DocumentReport::GetChipAuthenticationReport() const {
	PTEID_ChipAuthenticationReport *report;
	BEGIN_TRY_CATCH

	report = new PTEID_ChipAuthenticationReport(m_context, m_impl.getChipAuthenticationReport());

	END_TRY_CATCH
	return report;
}

PTEID_SodReport *PTEID_DocumentReport::GetSodReport() const {
	PTEID_SodReport *report;
	BEGIN_TRY_CATCH

	report = new PTEID_SodReport(m_context, m_impl.getSodReport());

	END_TRY_CATCH
	return report;
}

PTEID_DataGroupReport *PTEID_DocumentReport::GetDataGroupReport(PTEID_DataGroupID tag) const {
	PTEID_DataGroupReport *report;
	BEGIN_TRY_CATCH

	report = new PTEID_DataGroupReport(m_context, m_impl.getDataGroupReport(static_cast<DataGroupID>(tag)));

	END_TRY_CATCH
	return report;
}

/*****************************************************************************************
------------------------------------- PTEID_BaseDGReport ---------------------------------
*****************************************************************************************/
PTEID_BaseDGReport::PTEID_BaseDGReport(const SDK_Context *context, const EIDMW_DocumentReport &report)
	: PTEID_Object(context, NULL), m_documentReport(report) {}

PTEID_DocumentReport *PTEID_BaseDGReport::GetDocumentReport() const {
	PTEID_DocumentReport *report;
	BEGIN_TRY_CATCH

	report = new PTEID_DocumentReport(m_context, m_documentReport);

	END_TRY_CATCH
	return report;
}

const PTEID_DataGroupReport *PTEID_BaseDGReport::GetReportByID(PTEID_DataGroupID id) const {
	PTEID_DataGroupReport *report;
	BEGIN_TRY_CATCH

	report = new PTEID_DataGroupReport(m_context, m_documentReport.getDataGroupReport(static_cast<DataGroupID>(id)));

	END_TRY_CATCH
	return report;
}

/*****************************************************************************************
------------------------------------- PTEID_RawDataGroup ---------------------------------
*****************************************************************************************/
PTEID_RawDataGroup::PTEID_RawDataGroup(const SDK_Context *context, PTEID_DataGroupID id, PTEID_ByteArray data,
									   const EIDMW_DocumentReport &report)
	: PTEID_BaseDGReport(context, report), m_data(data), m_id(id) {}

PTEID_ByteArray PTEID_RawDataGroup::GetData() const { return m_data; }

const PTEID_DataGroupReport *PTEID_RawDataGroup::GetReport() const { return GetReportByID(m_id); }

/*****************************************************************************************
------------------------------------ PTEID_DataGroupReport -------------------------------
*****************************************************************************************/
PTEID_DataGroupReport::PTEID_DataGroupReport(const SDK_Context *context, const EIDMW_DataGroupReport &report)
	: PTEID_Object(context, NULL), m_impl(report) {}

PTEID_ByteArray PTEID_DataGroupReport::GetStoredHash() const { return PTEID_ByteArray(m_context, m_impl.storedHash); }

PTEID_ByteArray PTEID_DataGroupReport::GetComputedHash() const {
	return PTEID_ByteArray(m_context, m_impl.computedHash);
}

long PTEID_DataGroupReport::GetStatus() const { return m_impl.error_code; }

const char *PTEID_DataGroupReport::GetStatusMessage() const {
	m_statusMessage = CodeToString(m_impl.error_code);
	return m_statusMessage.c_str();
}

/*****************************************************************************************
--------------------------------------- ICAO_Card ----------------------------------------
*****************************************************************************************/
ICAO_Card::ICAO_Card(const SDK_Context *context, APL_ICAO *impl) : PTEID_Object(context, impl) {}

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

PTEID_DocumentReport *ICAO_Card::GetDocumentReport() const {
	auto icao = static_cast<APL_ICAO *>(m_impl);
	return new PTEID_DocumentReport(m_context, icao->getDocumentReport());
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

void ICAO_Card::initBACAuthentication(const char *mrz_info) {
	BEGIN_TRY_CATCH
	APL_Card *pcard = static_cast<APL_Card *>(m_impl);
	pcard->initBACAuthentication(mrz_info);
	END_TRY_CATCH
}

PTEID_RawDataGroup *ICAO_Card::readDatagroupRaw(PTEID_DataGroupID tag) {
	PTEID_RawDataGroup *out = nullptr;

	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	auto result = icao->readDatagroup(static_cast<DataGroupID>(tag));

	PTEID_ByteArray content(m_context, result.second);
	out = new PTEID_RawDataGroup(m_context, tag, content, icao->getDocumentReport());
	END_TRY_CATCH;

	return out;
}

void ICAO_Card::resetCardState() {
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	icao->resetCardState();
	END_TRY_CATCH;
}

PTEID_ICAO_DG1::PTEID_ICAO_DG1(const SDK_Context *context, const IcaoDg1 &dg1,
							   const EIDMW_DocumentReport &documentReport)
	: PTEID_BaseDGReport(context, documentReport), m_impl(dg1) {}

const PTEID_DataGroupReport *PTEID_ICAO_DG1::GetReport() const { return GetReportByID(PTEID_DATA_GROUP_ID_DG1); }

const char *PTEID_ICAO_DG1::documentCode() const { return m_impl.documentCode().c_str(); }

const char *PTEID_ICAO_DG1::issuingState() const { return m_impl.issuingOrg().c_str(); }

const char *PTEID_ICAO_DG1::documentNumber() const { return m_impl.serialNumber().c_str(); }

int PTEID_ICAO_DG1::serialNumberCheckDigit() const { return m_impl.serialNumberCheckDigit(); }

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

PTEID_ByteArray PTEID_FaceInfoData::photoRawDataPNG() const { return PTEID_ByteArray(m_context, m_impl.photoRawDataPNG()); }

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

PTEID_ICAO_DG2::PTEID_ICAO_DG2(const SDK_Context *context, const IcaoDg2 &dg2, const EIDMW_DocumentReport &report)
	: PTEID_BaseDGReport(context, report), m_impl(dg2) {
	for (auto &instance : m_impl.biometricInstances()) {
		m_biometricInstances.push_back(new PTEID_BiometricInfomation(m_context, *instance));
	}
}

const PTEID_DataGroupReport *PTEID_ICAO_DG2::GetReport() const { return GetReportByID(PTEID_DATA_GROUP_ID_DG2); }

PTEID_ByteArray PTEID_BiometricInfomationDg3::icaoHeaderVersion() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->icaoHeaderVersion());
}

PTEID_ByteArray PTEID_BiometricInfomationDg3::type() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->type());
}

PTEID_ByteArray PTEID_BiometricInfomationDg3::subType() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->subType());
}

PTEID_ByteArray PTEID_BiometricInfomationDg3::creationDateAndtime() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->creationDateAndtime());
}

PTEID_ByteArray PTEID_BiometricInfomationDg3::validPeriod() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->validPeriod());
}

PTEID_ByteArray PTEID_BiometricInfomationDg3::creatorOfBiometricRefData() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->creatorOfBiometricRefData());
}

PTEID_ByteArray PTEID_BiometricInfomationDg3::formatOwner() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->formatOwner());
}

PTEID_ByteArray PTEID_BiometricInfomationDg3::formatType() const {
	return PTEID_ByteArray(m_context, m_impl.biometricTemplate()->formatType());
}

const char *PTEID_BiometricInfomationDg3::specVersion() const { return m_impl.specVersion().c_str(); }

unsigned long long PTEID_BiometricInfomationDg3::recordLength() const { return m_impl.recordLength(); }

unsigned short PTEID_BiometricInfomationDg3::scannerId() const { return m_impl.scannerId(); }

unsigned short PTEID_BiometricInfomationDg3::imageAcquisitionLevel() const { return m_impl.imageAcquisitionLevel(); }

unsigned int PTEID_BiometricInfomationDg3::numFingersOrPalmImages() const { return m_impl.numFingersOrPalmImages(); }

unsigned int PTEID_BiometricInfomationDg3::scaleUnits() const { return m_impl.scaleUnits(); }

unsigned short PTEID_BiometricInfomationDg3::xScanResolution() const { return m_impl.xScanResolution(); }

unsigned short PTEID_BiometricInfomationDg3::yScanResolution() const { return m_impl.yScanResolution(); }

unsigned short PTEID_BiometricInfomationDg3::xImageResolution() const { return m_impl.xImageResolution(); }

unsigned short PTEID_BiometricInfomationDg3::yImageResolution() const { return m_impl.yImageResolution(); }

unsigned int PTEID_BiometricInfomationDg3::pixelDepth() const { return m_impl.pixelDepth(); }

unsigned int PTEID_BiometricInfomationDg3::imageCompressionAlgorithm() const {
	return m_impl.imageCompressionAlgorithm();
}

unsigned short PTEID_BiometricInfomationDg3::reserved() const { return m_impl.reserved(); }

PTEID_BiometricInfomationDg3::PTEID_BiometricInfomationDg3(const SDK_Context *context, const BiometricInfoDG3 &bioInfo)
	: PTEID_Object(context, NULL), m_impl(bioInfo) {}

PTEID_BiometricInfomationDg3::~PTEID_BiometricInfomationDg3() {
	for (auto *instance : m_bioFingerImageVec) {
		delete instance;
	}
}

PTEID_ICAO_DG3::PTEID_ICAO_DG3(const SDK_Context *context, const IcaoDg3 &dg3, const EIDMW_DocumentReport &report)
	: PTEID_BaseDGReport(context, report), m_impl(dg3) {
	for (const auto &instance : dg3.biometricInstanceVec()) {
		m_biometricInformation.push_back(new PTEID_BiometricInfomationDg3(context, *instance.get()));
	}
}

const PTEID_DataGroupReport *PTEID_ICAO_DG3::GetReport() const { return GetReportByID(PTEID_DATA_GROUP_ID_DG2); }

unsigned int PTEID_BiometricInfoFingerImage::length() const { return m_impl.length(); }

unsigned int PTEID_BiometricInfoFingerImage::fingerPalmPosition() const { return m_impl.fingerPalmPosition(); }

unsigned int PTEID_BiometricInfoFingerImage::countOfViews() const { return m_impl.countOfViews(); }

unsigned int PTEID_BiometricInfoFingerImage::viewMumber() const { return m_impl.viewMumber(); }

unsigned int PTEID_BiometricInfoFingerImage::quality() const { return m_impl.quality(); }

unsigned int PTEID_BiometricInfoFingerImage::impressionType() const { return m_impl.impressionType(); }

unsigned short PTEID_BiometricInfoFingerImage::horizontalLineLength() const { return m_impl.horizontalLineLength(); }

unsigned short PTEID_BiometricInfoFingerImage::verticalLineLength() const { return m_impl.verticalLineLength(); }

unsigned char PTEID_BiometricInfoFingerImage::reserved() const { return m_impl.reserved(); }

PTEID_ByteArray PTEID_BiometricInfoFingerImage::imageData() const {
	return PTEID_ByteArray(m_context, m_impl.imageData());
}

PTEID_BiometricInfoFingerImage::PTEID_BiometricInfoFingerImage(const SDK_Context *context,
															   const BiometricInfoImage &bioInfo)
	: PTEID_Object(context, NULL), m_impl(bioInfo)

{}

unsigned int PTEID_ICAO_DG3::numberOfbiometrics() const { return m_impl.numberOfbiometrics(); }

std::vector<PTEID_BiometricInfomationDg3 *> PTEID_ICAO_DG3::biometricInformation() const {
	return m_biometricInformation;
}

PTEID_ByteArray PTEID_ICAO_DG11::listOfTags() const { return PTEID_ByteArray(m_context, m_impl.listOfTags()); }

const char *PTEID_ICAO_DG11::fullName() const { return m_impl.fullName().c_str(); }

const char *PTEID_ICAO_DG11::personalNumber() const { return m_impl.personalNumber().c_str(); }

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

PTEID_ICAO_DG3::~PTEID_ICAO_DG3() {
	for (auto *instance : m_biometricInformation) {
		delete instance;
	}
}

PTEID_ICAO_DG11::PTEID_ICAO_DG11(const SDK_Context *context, const IcaoDg11 &dg11, const EIDMW_DocumentReport &report)
	: PTEID_BaseDGReport(context, report), m_impl(dg11) {}

const PTEID_DataGroupReport *PTEID_ICAO_DG11::GetReport() const { return GetReportByID(PTEID_DATA_GROUP_ID_DG2); }

unsigned int PTEID_ICAO_DG2::numberOfBiometrics() const { return m_impl.numberOfBiometrics(); }

std::vector<PTEID_BiometricInfomation *> PTEID_ICAO_DG2::biometricInstances() { return m_biometricInstances; }

PTEID_ICAO_DG2::~PTEID_ICAO_DG2() {
	for (auto *instance : m_biometricInstances) {
		delete instance;
	}
}

PTEID_ICAO_DG1 *ICAO_Card::readDataGroup1() {
	PTEID_ICAO_DG1 *dg1 = NULL;
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	auto *dg = icao->readDataGroup1();
	if (dg != NULL)
		dg1 = new PTEID_ICAO_DG1(m_context, *dg, icao->getDocumentReport());
	END_TRY_CATCH
	return dg1;
}

PTEID_ICAO_DG2 *ICAO_Card::readDataGroup2() {
	PTEID_ICAO_DG2 *dg2 = NULL;
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	IcaoDg2 *dg2Impl = icao->readDataGroup2();
	if (dg2Impl != NULL)
		dg2 = new PTEID_ICAO_DG2(m_context, *dg2Impl, icao->getDocumentReport());
	END_TRY_CATCH
	return dg2;
}

PTEID_ICAO_DG3 *ICAO_Card::readDataGroup3() {
	PTEID_ICAO_DG3 *dg3 = NULL;
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	IcaoDg3 *impl = icao->readDataGroup3();
	if (impl != NULL)
		dg3 = new PTEID_ICAO_DG3(m_context, *impl, icao->getDocumentReport());
	END_TRY_CATCH
	return dg3;
}

PTEID_ICAO_DG11 *ICAO_Card::readDataGroup11() {
	PTEID_ICAO_DG11 *dg11 = NULL;
	BEGIN_TRY_CATCH
	APL_ICAO *icao = static_cast<APL_ICAO *>(m_impl);
	IcaoDg11 *dg11Impl = icao->readDataGroup11();
	if (dg11Impl != NULL)
		dg11 = new PTEID_ICAO_DG11(m_context, *dg11Impl, icao->getDocumentReport());
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


} // Namespace eIDMW
