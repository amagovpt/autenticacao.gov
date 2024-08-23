#include "IcaoDg11.h"

#include <ByteArray.h>

#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/err.h>

#include <algorithm>
#include <string>

namespace eIDMW {

struct OTHER_NAMES_st {
	ASN1_INTEGER *number_of_other_names;
	ASN1_STRING *other_names;
};

typedef struct OTHER_NAMES_st OTHER_NAMES;

struct DG11_st {
	ASN1_STRING *list_of_tags_present;
	OTHER_NAMES *other_names;
	ASN1_STRING *full_name;
	ASN1_STRING *personal_number;
	ASN1_STRING *full_date_of_birth;
	ASN1_STRING *place_of_birth;
	ASN1_STRING *permanent_address;
	ASN1_STRING *telephone;
	ASN1_STRING *profession;
	ASN1_STRING *title;
	ASN1_STRING *personal_summary;
	ASN1_STRING *proof_of_citizenship;
	ASN1_STRING *other_valid_td_numbers;
	ASN1_STRING *custody_information;
};

typedef struct DG11_st DG11;

ASN1_SEQUENCE(OTHER_NAMES) = {
	ASN1_EX_TYPE(ASN1_TFLG_UNIVERSAL, 0x02, OTHER_NAMES, number_of_other_names, ASN1_INTEGER),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG, 0x0F, OTHER_NAMES, other_names, ASN1_UTF8STRING),
} ASN1_SEQUENCE_END(OTHER_NAMES);

IMPLEMENT_ASN1_FUNCTIONS(OTHER_NAMES);

ASN1_SEQUENCE(DG11) = {
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG, 0x1C, DG11, list_of_tags_present, ASN1_UTF8STRING),

	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x0E, DG11, full_name, ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x10, DG11, personal_number,
				 ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x2B, DG11, full_date_of_birth,
				 ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x11, DG11, place_of_birth,
				 ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x42, DG11, permanent_address,
				 ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x12, DG11, telephone, ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x13, DG11, profession,
				 ASN1_UTF8STRING),

	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x14, DG11, title, ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x15, DG11, personal_summary,
				 ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x16, DG11, proof_of_citizenship,
				 ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x17, DG11, other_valid_td_numbers,
				 ASN1_UTF8STRING),
	ASN1_EX_TYPE(ASN1_TFLG_APPLICATION | ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x18, DG11, custody_information,
				 ASN1_UTF8STRING),
	ASN1_IMP_EX(DG11, other_names, OTHER_NAMES, 0x0, ASN1_TFLG_OPTIONAL),

} ASN1_SEQUENCE_END(DG11);

IMPLEMENT_ASN1_FUNCTIONS(DG11);
DG11 *decodeDg11(const CByteArray &biometricData) {
	CByteArray process(biometricData);
	long size;

	unsigned char *asn1_data = process.GetBytes();
	asn1_data[0] = 0x30;

	DG11 *processDG11 = d2i_DG11(NULL, (const unsigned char **)&asn1_data, process.Size());
	if (!processDG11) {
		long error = ERR_get_error();
		char *errorRead = ERR_error_string(error, NULL);
		printf("Error processing DG11 errorCode:%lu error: %s\n", error, errorRead);
	}
	return processDG11;
}

void copyToString(std::string &placeToCopy, ASN1_STRING *copy, bool cleanString = false) {
	if (!copy || copy->length == 0)
		return;

	CByteArray arrayHelper;
	arrayHelper.Append(copy->data, copy->length);
	placeToCopy = arrayHelper.hexToString();
	if (cleanString) {
		std::replace(placeToCopy.begin(), placeToCopy.end(), '<', ' ');
		auto eraseSpaces = std::unique(placeToCopy.begin(), placeToCopy.end(),
									   [](char left, char right) { return (left == ' ' && right == ' '); });
		placeToCopy.erase(eraseSpaces, placeToCopy.end());
	}
}

IcaoDg11::IcaoDg11(const CByteArray &information) {
	DG11 *processedDg11 = decodeDg11(information);
	if (processedDg11->list_of_tags_present && processedDg11->list_of_tags_present->length > 0) {
		m_listOfTags.Append(processedDg11->list_of_tags_present->data, processedDg11->list_of_tags_present->length);
	}
	m_numberOfOtherNames = -1;
	copyToString(m_fullName, processedDg11->full_name, true);
	copyToString(m_personalNumber, processedDg11->personal_number, true);
	copyToString(m_fullDateOfBirth, processedDg11->full_date_of_birth);
	copyToString(m_placeOfBirth, processedDg11->place_of_birth, true);
	copyToString(m_permanentAddress, processedDg11->permanent_address, true);
	copyToString(m_telephone, processedDg11->telephone);
	copyToString(m_profession, processedDg11->profession, true);
	copyToString(m_title, processedDg11->title, true);
	copyToString(m_personalSummary, processedDg11->personal_summary, true);
	copyToString(m_proofOfCitizenship, processedDg11->proof_of_citizenship);
	copyToString(m_otherValidTDNumbers, processedDg11->other_valid_td_numbers, true);
	copyToString(m_custodyInformation, processedDg11->custody_information, true);
	if (processedDg11->other_names && processedDg11->other_names->number_of_other_names &&
		processedDg11->other_names->number_of_other_names->length) {
		m_numberOfOtherNames = (unsigned int)processedDg11->other_names->number_of_other_names->data[0];
	}

	if (processedDg11->other_names && processedDg11->other_names->other_names &&
		processedDg11->other_names->other_names->length) {
		copyToString(m_otherNames, processedDg11->other_names->other_names, true);
	}
}

const CByteArray &IcaoDg11::listOfTags() const { return m_listOfTags; }

const std::string &IcaoDg11::fullName() const { return m_fullName; }

const std::string &IcaoDg11::personalNumber() const { return m_personalNumber; }

const std::string &IcaoDg11::fullDateOfBirth() const { return m_fullDateOfBirth; }

const std::string &IcaoDg11::placeOfBirth() const { return m_placeOfBirth; }

const std::string &IcaoDg11::permanentAddress() const { return m_permanentAddress; }

const std::string &IcaoDg11::telephone() const { return m_telephone; }

const std::string &IcaoDg11::profession() const { return m_profession; }

const std::string &IcaoDg11::title() const { return m_title; }

const std::string &IcaoDg11::personalSummary() const { return m_personalSummary; }

const std::string &IcaoDg11::proofOfCitizenship() const { return m_proofOfCitizenship; }

const std::string &IcaoDg11::otherValidTDNumbers() const { return m_otherValidTDNumbers; }

const std::string &IcaoDg11::custodyInformation() const { return m_custodyInformation; }

int eIDMW::IcaoDg11::numberOfOtherNames() const { return m_numberOfOtherNames; }

const std::string &IcaoDg11::otherNames() const { return m_otherNames; }
} // namespace eIDMW
