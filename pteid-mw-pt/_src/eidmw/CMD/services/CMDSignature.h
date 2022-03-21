/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017, 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019-2021 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef PDF_SIGNATURE_CLI_H
#define PDF_SIGNATURE_CLI_H

#include "ByteArray.h"
#include "PDFSignature.h"
#include "cmdErrors.h"
#define IS_NETWORK_ERROR(error)    IS_SOAP_ERROR(error)

using namespace eIDMW;

#ifdef WIN32
#ifdef EIDMW_CMDSERVICES_EXPORTS
#define PTEIDCMD_API  __declspec(dllexport)
#else
#define PTEIDCMD_API  __declspec(dllimport)
#endif
#else
#define PTEIDCMD_API __attribute__ ((visibility ("default")))
#endif

#define MAX_PIN_SIZE 8
#define MAX_OTP_SIZE 6
#define MAX_DOCNAME_SIZE 50

namespace eIDMW {

    //Forward declare this class to avoid deep-includes
    class CMDServices;

    class CMDProxyInfo {
        public:
            PTEIDCMD_API static CMDProxyInfo buildProxyInfo();

            std::string host;
            long port;
            std::string user;
            std::string pwd;
    };

	class CMDSignature {
        public:
            PTEIDCMD_API CMDSignature(std::string basicAuthUser, std::string basicAuthPassword, std::string applicationId);
            PTEIDCMD_API CMDSignature(std::string basicAuthUser, std::string basicAuthPassword, std::string applicationId, PDFSignature *in_pdf_handler);
			PTEIDCMD_API ~CMDSignature();

            // The signOpen methods that do not use in_userId/in_pin open the SDK dialog.
            //proxyinfo parameter is saved in m_proxyInfo so that we can reuse it later in signClose()
            // signOpen used to sign hash
            PTEIDCMD_API int signOpen(CMDProxyInfo proxyinfo, std::string in_userId, std::string in_pin, CByteArray &in_hash, std::string docname);
            PTEIDCMD_API int signOpen(CMDProxyInfo proxyinfo, CByteArray &in_hash, std::string docname, std::string * mobile = NULL, const char *userName = NULL);
            // this signOpen should be used to sign PDFs
			PTEIDCMD_API int signOpen(CMDProxyInfo proxyinfo, std::string in_userId, std::string in_pin
                        , const char *location
                        , const char *reason
                        , const char *outfile_path);
            PTEIDCMD_API int signOpen(CMDProxyInfo proxyinfo
                        , const char *location
                        , const char *reason
                        , const char *outfile_path
                        , std::string * mobileCache = NULL); // mobileNumber used for placeholder (cache)

            PTEIDCMD_API int signClose();
            PTEIDCMD_API int signClose(std::string in_code);
            PTEIDCMD_API void cancelRequest();
            PTEIDCMD_API int sendSms();
            PTEIDCMD_API void set_pdf_handler(PDFSignature *in_pdf_handler);
            PTEIDCMD_API void add_pdf_handler(PDFSignature *in_pdf_handler);
            PTEIDCMD_API void clear_pdf_handlers();
            PTEIDCMD_API void set_string_handler(std::string in_docname_handle,
                                                 CByteArray in_array_handler);

            PTEIDCMD_API char * getCertificateCitizenName();
            PTEIDCMD_API char * getCertificateCitizenID();

            PTEIDCMD_API static std::string getEndpoint();

            /* Certificate chain from last call to cli_getCertificate. */
            std::vector<CByteArray> m_certificates;
            CByteArray m_signature;
            std::string m_string_certificate;

        private:
            CMDServices *cmdService;
            std::vector<PDFSignature*> m_pdf_handlers;
            std::string m_docname_handle;
            CByteArray m_array_handler;

            std::string m_userId;
            std::string m_pin;
            std::string m_receiveCode;
            bool m_computeHash = true;
            CMDProxyInfo m_proxyInfo;
            int cli_getCertificate( std::string in_userId );
            int cli_sendDataToSign( std::string in_pin );
            int cli_getSignatures(std::string in_code, std::vector<CByteArray *> out_sign);

            std::string m_basicAuthUser;
            std::string m_basicAuthPassword;
            std::string m_applicationId;
    };
}

#endif
