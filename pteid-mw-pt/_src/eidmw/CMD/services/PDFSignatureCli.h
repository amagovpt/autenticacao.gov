#ifndef PDF_SIGNATURE_CLI_H
#define PDF_SIGNATURE_CLI_H

#include "PDFSignature.h"
#include "CMDServices.h"
#include "eidlib.h"

using namespace eIDMW;

namespace eIDMW{
#define ERR_ADDR_BASE           1000
#define ERR_NONE                (ERR_ADDR_BASE + 0x00)
#define ERR_NULL_CARD           (ERR_ADDR_BASE + 0x01)
#define ERR_NULL_PDF            (ERR_ADDR_BASE + 0x02)
#define ERR_NULL_PDF_HANDLER    (ERR_ADDR_BASE + 0x03)
#define ERR_INV_CERTIFICATE     (ERR_ADDR_BASE + 0x04)
#define ERR_INV_CERTIFICATE_CA  (ERR_ADDR_BASE + 0x05)
#define ERR_GET_CERTIFICATE     (ERR_ADDR_BASE + 0x06)
#define ERR_SEND_HASH           (ERR_ADDR_BASE + 0x07)
#define ERR_GET_SIGNATURE       (ERR_ADDR_BASE + 0x08)
#define ERR_SIGN_PDF            (ERR_ADDR_BASE + 0x09)
#define ERR_GET_HASH            (ERR_ADDR_BASE + 0x0A)
#define ERR_SIGN_CLOSE          (ERR_ADDR_BASE + 0x0B)

    class PDFSignatureCli{
        public:
            PDFSignatureCli( PTEID_EIDCard *in_card, PTEID_PDFSignature *in_pdf_handler );
            ~PDFSignatureCli();
            int signOpen( std::string in_userId, std::string in_pin
                        , int page
                        , double coord_x, double coord_y
                        , const char *location
                        , const char *reason
                        , const char *outfile_path );

            int signOpen( std::string in_userId, std::string in_pin
                        , int page
                        , int page_sector
                        , bool is_landscape
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
            PTEID_EIDCard *m_card;
            std::string m_userId;
            std::string m_pin;
            std::string m_receiveCode;
            int cli_getCertificate( std::string in_userId );
            int cli_sendDataToSign( std::string in_pin );
            PTEID_ByteArray cli_getSignature( std::string in_code );
    };
}/* namespace eIDMW */
#endif /* PDF_SIGNATURE_CLI_H */
