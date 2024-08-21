#ifndef ICAODG11_H
#define ICAODG11_H

#include "ByteArray.h"

namespace eIDMW {

class IcaoDg11 {
public:
	IcaoDg11(const CByteArray &information);

	EIDMW_APL_API const CByteArray &listOfTags() const;
	EIDMW_APL_API const std::string &fullName() const;
	EIDMW_APL_API const std::string &personalName() const;
	EIDMW_APL_API const std::string &fullDateOfBirth() const;
	EIDMW_APL_API const std::string &placeOfBirth() const;
	EIDMW_APL_API const std::string &permanentAddress() const;
	EIDMW_APL_API const std::string &telephone() const;
	EIDMW_APL_API const std::string &profession() const;
	EIDMW_APL_API const std::string &title() const;
	EIDMW_APL_API const std::string &personalSummary() const;
	EIDMW_APL_API const std::string &proofOfCitizenship() const;
	EIDMW_APL_API const std::string &otherValidTDNumbers() const;
	EIDMW_APL_API const std::string &custodyInformation() const;
	EIDMW_APL_API int numberOfOtherNames() const;
	EIDMW_APL_API const std::string &otherNames() const;

private:
	CByteArray m_listOfTags;
	std::string m_fullName;
	std::string m_personalName;
	std::string m_fullDateOfBirth;
	std::string m_placeOfBirth;
	std::string m_permanentAddress;
	std::string m_telephone;
	std::string m_profession;
	std::string m_title;
	std::string m_personalSummary;
	std::string m_proofOfCitizenship;
	std::string m_otherValidTDNumbers;
	std::string m_custodyInformation;
	int m_numberOfOtherNames;
	std::string m_otherNames;
};
} // namespace eIDMW

#endif // ICAODG11_H
