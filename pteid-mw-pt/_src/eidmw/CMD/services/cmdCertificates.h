#ifndef CMD_CERTIFICATES_H
#define CMD_CERTIFICATES_H

#ifdef WIN32
#include <windows.h>
#include <Wincrypt.h>
#include <Cryptuiapi.h>
#endif

#include <string>
#include <vector>
#include "CMDSignature.h"
#include "ByteArray.h"

namespace eIDMW {

class PTEIDCMD_API CMDCertificates {
public:
	CMDCertificates(std::string m_basicAuthUser, std::string m_basicAuthPassword, std::string m_applicationId);
	~CMDCertificates();

#ifdef WIN32
	int ImportCertificatesOpen(std::string mobileNumber, std::string pin);
	int ImportCertificatesClose(std::string otp);
	void CancelImport();
	void GetRegisteredPhoneNumbers(std::vector<std::string> *phoneNumsOut);
	int sendSms();
#endif
	void setCertificates(const std::vector<CByteArray> &certs);
	void setCertificates(const std::vector<std::string *> &certs);
	void setMobileNumber(const std::string &mobileNumber);

	int getCertificates(std::vector<CByteArray> &outCerts);
	int getCertificates(std::vector<CByteArray> &outCerts, const std::string &mobileNumber);
#ifdef WIN32
	int updateCertChainIfChanged(bool replaceIfExists = true);
#endif

private:
#ifdef WIN32
	int fetchCertificates(std::string otp);
	bool StoreUserCert(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, std::string mobileNumber, bool replaceIfExists);
	bool StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits);
	bool SetPrivateKeyContainerInfo(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, std::string mobileNumber);
	bool SetCertificateFriendlyName(PCCERT_CONTEXT pCertContext, std::string mobileNumber);
	void RemoveOlderUserCerts(PCCERT_CONTEXT pCertContext);
	bool CertExistsForPhoneNumber(HCERTSTORE hMyStore, const std::string &mobileNumber);
#endif
	CMDServices *m_cmdService;
	std::vector<std::string *> m_certificates;
	std::string m_mobileNumber;

	std::string m_basicAuthUser, m_basicAuthPassword, m_applicationId;
};
} // namespace eIDMW
#endif // CMD_CERTIFICATES_H
