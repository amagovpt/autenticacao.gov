#ifndef PDF_SIGNATURE_CLI_H
#define PDF_SIGNATURE_CLI_H

#include "eidlib.h"
#include "ByteArray.h"

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


namespace eIDMW {

    //Forward declare this class to avoid deep-includes
    class CMDServices;

    class CMDProxyInfo {
        public:
            std::string host;
            long port;
            std::string user;
            std::string pwd;
    };

	class CMDSignature {
        public:
            PTEIDCMD_API CMDSignature();
			PTEIDCMD_API CMDSignature(PTEID_PDFSignature *in_pdf_handler);
			PTEIDCMD_API ~CMDSignature();

            //proxyinfo parameter is saved in m_proxyInfo so that we can reuse it later in signClose()
			PTEIDCMD_API int signOpen(CMDProxyInfo proxyinfo, std::string in_userId, std::string in_pin
                        , int page
                        , double coord_x, double coord_y
                        , const char *location
                        , const char *reason
                        , const char *outfile_path);

			PTEIDCMD_API int signClose(std::string in_code);
            PTEIDCMD_API void set_pdf_handler(PTEID_PDFSignature *in_pdf_handler);

            PTEIDCMD_API char * getCertificateCitizenName();
            PTEIDCMD_API char * getCertificateCitizenID();

        private:
            CMDServices *cmdService;
            PTEID_PDFSignature *m_pdf_handler;

            std::string m_userId;
            std::string m_pin;
            std::string m_receiveCode;
            CMDProxyInfo m_proxyInfo;
            int cli_getCertificate( std::string in_userId );
            int cli_sendDataToSign( std::string in_pin );
            int cli_getSignature(std::string in_code, PTEID_ByteArray &out_sign);
    };
}

#endif
