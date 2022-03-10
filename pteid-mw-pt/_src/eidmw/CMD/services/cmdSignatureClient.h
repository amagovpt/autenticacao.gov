/*-****************************************************************************

 * Copyright (C) 2021 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef CMD_SIGNATURE_CLIENT_H
#define CMD_SIGNATURE_CLIENT_H

#include <vector>
#include <functional>
#include "CMDSignature.h"
#include "ByteArray.h"
#include "PDFSignature.h"
#include "APLCertif.h"
#include "dialogs.h"
#include "Thread.h"

using namespace eIDMW;

#define B64_ID_FROM_HASH_SIZE 8

namespace eIDMW {

    struct CertificatesCache {
        std::string mobile;
        std::vector<APL_Certifs *> certificates;
    };

    class CMDSignatureClient {
    public:
        PTEIDCMD_API ~CMDSignatureClient();

        PTEIDCMD_API void setMobileNumberCaching(bool enabled);

        /*- If docname is NULL, the doc id used is returned by getSignatureIdFromHash.
          - If mobileNumber is not NULL, the mobile edit field is not shown. mobileNumber and userName are used to
                indicate the mobile and user name associated with the certificate. Only Windows (for the KSP). It overrides mobile cache. */
        PTEIDCMD_API CByteArray Sign(const CByteArray& data, bool signatureKey = false, const char * docname = NULL, const char *mobileNumber = NULL, const char * userName = NULL);
        PTEIDCMD_API CByteArray& SignXades(const char *output_path, const char * const* paths, unsigned int n_paths, APL_SignatureLevel level);
        PTEIDCMD_API void SignXadesIndividual(const char *output_path, const char * const* paths, unsigned int n_paths);
        PTEIDCMD_API void SignASiC(const char *path, APL_SignatureLevel level);
        PTEIDCMD_API int SignPDF(PDFSignature &sig_handler, const char *location, const char *reason, const char *outfile_path);

        PTEIDCMD_API APL_Certifs* getCertificates();

        /* The PIN is stored in out_pin.
           If userName == NULL, the mobile number is stored in mobileNumber. If the input is a non empty string,
           it's contents are used as mobileNumber placeholder (for caching).
           If userName != NULL, the mobileNumber is not asked. Instead, show box with fixed mobile number and userName (subject of the cert). */
        static DlgRet openAuthenticationDialogPIN(DlgCmdOperation operation, std::string *out_pin, std::string *mobileNumber, const char * userName = NULL);
        /* The OTP is stored in out_otp. */
        static DlgRet openAuthenticationDialogOTP(DlgCmdOperation operation, std::string *out_otp, std::string *docId, std::function<void(void)> *fSendSmsCallback = NULL);

        PTEIDCMD_API static void setCredentials(const char* basicAuthUser, const char* basicAuthPassword, const char* applicationId);

    private:
        void handleErrorCode(int err, bool isOtp);
        /* Returns the first B64_ID_FROM_HASH_SIZE characters of the Base64 representation of hash.
        It is used as an id for the Sign methods (where no docname is provided).
        If it fails, no docname is sent to the CMD services. */
        std::string getSignatureIdFromHash(CByteArray hash);

        /* Update m_certificatesCache with certificates in certs. It is called after each signature. */
        void updateCertificateCache(std::vector<CByteArray> *certs);
        APL_Certifs* getCertificates(const std::string& mobileNumber);

        /* Stores certificate chain from the previous signatures and from calls to getCertificates to be deleted in destructor. */
        CertificatesCache m_certificatesCache;

        static void handleMissingCredentials();

        static std::string m_basicAuthUser, m_basicAuthPassword, m_applicationId; // CMD credentials

        bool m_shouldMobileCache = false;
        std::string m_mobileNumberCached;
        std::string m_pin;
        std::string m_mobileNumber;

    };

    /* Show progress dialog in a different thread. */
    class CMDProgressDlgThread : public CThread  {
    public:
        CMDProgressDlgThread(DlgCmdOperation operation, bool isOtp, std::function<void(void)> *fCancelCallback);

        void Run() override;
        void Stop(unsigned long ulSleepFrequency = 100) override;

        bool wasCancelled() { return m_wasCancelled; };

    private:
        DlgCmdOperation m_operation;
        bool m_isOtp;
        bool m_wasCancelled = false;
        std::function<void(void)> *m_fCancelCallback = NULL;
        unsigned long m_dlgHandle = 0;
        unsigned long m_oldDlgHandle = 0;
    };
}

#endif
