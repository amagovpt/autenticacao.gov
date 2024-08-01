#ifndef ICAODG1_H
#define ICAODG1_H

#include <string>

#include "Export.h"

namespace eIDMW {
class CByteArray;
class IcaoDg1 {
public:
	IcaoDg1(const CByteArray &array);

	EIDMW_APL_API const std::string &documentCode() const;
	EIDMW_APL_API const std::string &issuingOrg() const;
	EIDMW_APL_API const std::string &serialNumber() const;
	EIDMW_APL_API int serialNumberCheckDigit() const;
	EIDMW_APL_API const std::string &optionalData() const;
	EIDMW_APL_API const std::string &birthDay() const;
	EIDMW_APL_API int birthDayCheckDigit() const;
	EIDMW_APL_API char sex() const;
	EIDMW_APL_API const std::string &expireDay() const;
	EIDMW_APL_API int expireDayCheckDigit() const;
	EIDMW_APL_API const std::string &nationality() const;
	EIDMW_APL_API const std::string &optionalDataSecondLine() const;
	EIDMW_APL_API int optionalDataSecondLineCheckDigit() const;
	EIDMW_APL_API int compositeCheckDigit() const;
	EIDMW_APL_API const std::string &primaryIdentifier() const;
	EIDMW_APL_API const std::string &secondaryIdentifier() const;

private:
	std::string m_documentCode; // can have at most two length
	std::string m_issuingOrg;
	std::string m_serialNumber;
	int m_serialNumberCheckDigit = -1; // if -1 means it has optional data to complete the serial number
	std::string m_optionalData;
	std::string m_birthDay;
	int m_birthDayCheckDigit = -1;
	char m_sex;
	std::string m_expireDay;
	int m_expireDayCheckDigit = -1;
	std::string m_nationality;
	std::string m_optionalDataSecondLine;
	int m_optionalDataSecondLineCheckDigit = -1; // td3 only
	int m_compositeCheckDigit = -1;
	std::string m_primaryIdentifier;
	std::string m_secondaryIdentifier;
};
} // namespace eIDMW
#endif // ICAODG1_H
