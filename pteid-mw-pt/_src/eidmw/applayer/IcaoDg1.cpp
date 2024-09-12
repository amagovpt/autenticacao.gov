#include "IcaoDg1.h"

#include <ByteArray.h>

#include <algorithm>

#define MRZ_DOC_CODE_SIZE 2
#define MRZ_SERIAL_NUMBER_SIZE 9
#define MRZ_DATE_SIZE 6
#define MRZ_ISSUER_ORG_SIZE 3
#define MRZ_NATIONALITY_SIZE 3
#define MRZ_TD1_LINE_SIZE 30
#define MRZ_TD1_FIRST_LINE_OPTIONAL_DATA_SIZE 15
#define MRZ_TD1_SECOND_LINE_OPTIONAL_DATA_SIZE 11
#define MRZ_TD3_NAME_FIRST_LINE_SIZE 39
#define MRZ_TD3_OPTIONAL_DATA_SIZE 14

namespace eIDMW {

void cleanUpString(std::string &str) { str.erase(std::remove(str.begin(), str.end(), '<'), str.end()); }

int getCheckDigits(const CByteArray &byteArray, unsigned int &offset) {
	int checkDigit = byteArray.GetByte(offset) - '0';
	offset++;
	return checkDigit;
}

std::string getDocumentInfo(const CByteArray &byteArray, unsigned int &offset, unsigned int sizeOfInfo) {
	std::string information = byteArray.GetBytes(offset, sizeOfInfo).hexToString();
	offset += sizeOfInfo;
	cleanUpString(information);
	return information;
}

void trim(std::string &str) {
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));
	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
			  str.end());
}

void getNameIdentifiers(const std::string &line, std::string &primaryIdentifier, std::string &secondaryIdentifier) {
	size_t dividerIdentifiers = line.find("<<");
	if (dividerIdentifiers != std::string::npos) {
		primaryIdentifier = line.substr(0, dividerIdentifiers);
		std::replace(primaryIdentifier.begin(), primaryIdentifier.end(), '<', ' ');
		trim(primaryIdentifier);
		secondaryIdentifier = line.substr(dividerIdentifiers + 2);
		std::replace(secondaryIdentifier.begin(), secondaryIdentifier.end(), '<', ' ');
		trim(secondaryIdentifier);
	} else {
		primaryIdentifier = line;
		trim(primaryIdentifier);
	}
}

IcaoDg1::IcaoDg1(const CByteArray &byteArray) {
	unsigned int offsetCounter = 0;
	bool isPassport = false;
	m_documentCode = getDocumentInfo(byteArray, offsetCounter, MRZ_DOC_CODE_SIZE);
	isPassport = m_documentCode.front() == 'P';

	m_issuingOrg = getDocumentInfo(byteArray, offsetCounter, MRZ_ISSUER_ORG_SIZE);

	if (isPassport) {
		std::string nameElement = byteArray.GetBytes(offsetCounter, MRZ_TD3_NAME_FIRST_LINE_SIZE).hexToString();
		offsetCounter += MRZ_TD3_NAME_FIRST_LINE_SIZE;
		getNameIdentifiers(nameElement, m_primaryIdentifier, m_secondaryIdentifier);
		m_serialNumber = getDocumentInfo(byteArray, offsetCounter, MRZ_SERIAL_NUMBER_SIZE);
		m_serialNumberCheckDigit = getCheckDigits(byteArray, offsetCounter);
		m_nationality = getDocumentInfo(byteArray, offsetCounter, MRZ_NATIONALITY_SIZE);
		m_birthDay = getDocumentInfo(byteArray, offsetCounter, MRZ_DATE_SIZE);
		m_birthDayCheckDigit = getCheckDigits(byteArray, offsetCounter);
		m_sex = static_cast<char>(byteArray.GetByte(offsetCounter));
		offsetCounter++;
		m_expireDay = getDocumentInfo(byteArray, offsetCounter, MRZ_DATE_SIZE);
		m_expireDayCheckDigit = getCheckDigits(byteArray, offsetCounter);
		m_optionalDataSecondLine = byteArray.GetBytes(offsetCounter, MRZ_TD3_OPTIONAL_DATA_SIZE).hexToString();
		offsetCounter += MRZ_TD3_OPTIONAL_DATA_SIZE;
		std::replace(m_optionalDataSecondLine.begin(), m_optionalDataSecondLine.end(), '<', ' ');
		trim(m_optionalDataSecondLine);
		m_optionalDataSecondLineCheckDigit = getCheckDigits(byteArray, offsetCounter);
		m_compositeCheckDigit = getCheckDigits(byteArray, offsetCounter);

	} else {
		m_serialNumber = getDocumentInfo(byteArray, offsetCounter, MRZ_SERIAL_NUMBER_SIZE);
		if (byteArray.GetByte(offsetCounter) != '<') {
			m_serialNumberCheckDigit = getCheckDigits(byteArray, offsetCounter);
		} else {
			m_serialNumberCheckDigit = -1;
			offsetCounter++;
		}

		m_optionalData = getDocumentInfo(byteArray, offsetCounter, MRZ_TD1_FIRST_LINE_OPTIONAL_DATA_SIZE);

		m_birthDay = getDocumentInfo(byteArray, offsetCounter, MRZ_DATE_SIZE);
		m_birthDayCheckDigit = getCheckDigits(byteArray, offsetCounter);

		m_sex = static_cast<char>(byteArray.GetByte(offsetCounter));
		offsetCounter++;

		m_expireDay = getDocumentInfo(byteArray, offsetCounter, MRZ_DATE_SIZE);
		m_expireDayCheckDigit = getCheckDigits(byteArray, offsetCounter);
		m_nationality = getDocumentInfo(byteArray, offsetCounter, MRZ_NATIONALITY_SIZE);
		m_optionalDataSecondLine = getDocumentInfo(byteArray, offsetCounter, MRZ_TD1_SECOND_LINE_OPTIONAL_DATA_SIZE);

		m_compositeCheckDigit = getCheckDigits(byteArray, offsetCounter);

		std::string lastLine = byteArray.GetBytes(offsetCounter, MRZ_TD1_LINE_SIZE).hexToString();
		getNameIdentifiers(lastLine, m_primaryIdentifier, m_secondaryIdentifier);
	}
}

const std::string &IcaoDg1::documentCode() const { return m_documentCode; }

const std::string &IcaoDg1::issuingOrg() const { return m_issuingOrg; }

const std::string &IcaoDg1::serialNumber() const { return m_serialNumber; }

int IcaoDg1::serialNumberCheckDigit() const { return m_serialNumberCheckDigit; }

const std::string &IcaoDg1::optionalData() const { return m_optionalData; }

const std::string &IcaoDg1::birthDay() const { return m_birthDay; }

int IcaoDg1::birthDayCheckDigit() const { return m_birthDayCheckDigit; }

char IcaoDg1::sex() const { return m_sex; }

const std::string &IcaoDg1::expireDay() const { return m_expireDay; }

int IcaoDg1::expireDayCheckDigit() const { return m_expireDayCheckDigit; }

const std::string &IcaoDg1::nationality() const { return m_nationality; }

const std::string &IcaoDg1::optionalDataSecondLine() const { return m_optionalDataSecondLine; }

int IcaoDg1::optionalDataSecondLineCheckDigit() const { return m_optionalDataSecondLineCheckDigit; }

int IcaoDg1::compositeCheckDigit() const { return m_compositeCheckDigit; }

const std::string &IcaoDg1::primaryIdentifier() const { return m_primaryIdentifier; }

const std::string &IcaoDg1::secondaryIdentifier() const { return m_secondaryIdentifier; }
} // namespace eIDMW
