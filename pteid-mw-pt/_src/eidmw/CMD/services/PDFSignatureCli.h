#ifndef PDF_SIGNATURE_CLI_H
#define PDF_SIGNATURE_CLI_H

#include "PDFSignature.h"
#include "cmdServices.h"
#include "eidlib.h"

#include "cmdErrors.h"
#define IS_NETWORK_ERROR(error)    IS_SOAP_ERROR(error)

using namespace eIDMW;

namespace eIDMW{
    class PDFSignatureCli {
        public:
            PDFSignatureCli(PTEID_PDFSignature *in_pdf_handler);
            ~PDFSignatureCli();
            int signOpen( std::string in_userId, std::string in_pin
                        , int page
                        , double coord_x, double coord_y
                        , const char *location
                        , const char *reason
                        , const char *outfile_path );

            int signClose( std::string in_code );

            std::string getUserId();
            void setUserId( std::string in_userId );

            std::string getUserPin();
            void setUserPin( std::string in_pin );

            std::string getReceiveCode();
            void setReceiveCode( std::string in_receiveCode );

        private:
            CMDServices cmdService;
            PTEID_PDFSignature *m_pdf_handler;

            std::string m_userId;
            std::string m_pin;
            std::string m_receiveCode;
            int cli_getCertificate( std::string in_userId );
            int cli_sendDataToSign( std::string in_pin );
            int cli_getSignature( std::string in_code
                                , PTEID_ByteArray &out_sign );
    };
}/* namespace eIDMW */
#endif
