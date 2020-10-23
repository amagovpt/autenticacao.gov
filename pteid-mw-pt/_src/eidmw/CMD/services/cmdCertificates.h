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

namespace eIDMW {

    class PTEIDCMD_API CMDCertificates
    {
    public:
        CMDCertificates();
        ~CMDCertificates();
        int ImportCertificatesOpen(std::string mobileNumber, std::string pin);
        int ImportCertificatesClose(std::string otp);
        void CancelImport();
        void GetRegisteredPhoneNumbers(std::vector<std::string> *phoneNumsOut);
        int sendSms();

    private:
        int fetchCertificates(std::string otp);
        bool StoreUserCert(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, std::string mobileNumber);
        bool StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits);
        bool SetPrivateKeyContainerInfo(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, std::string mobileNumber);
        bool SetCertificateFriendlyName(PCCERT_CONTEXT pCertContext, std::string mobileNumber);
        void RemoveOlderUserCerts(PCCERT_CONTEXT pCertContext);

        CMDServices *m_cmdService;
        std::vector<std::string *> m_certificates;
        std::string m_mobileNumber;
    };
}
#endif // CMD_CERTIFICATES_H
