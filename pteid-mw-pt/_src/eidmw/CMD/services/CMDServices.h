#ifndef CMD_CLIENT_H
#define CMD_CLIENT_H

//STD Library
#include <iostream>
#include <string>
#include <openssl/x509.h>
#include "soapH.h"
#include "ByteArray.h"
#include "Log.h"

#define ERR_NONE 0
#define ERR_FAIL -1

#ifdef WIN32
    #define _LOG_( buf, level, mod, format, ... ) { sprintf( buf, "%s() - ", __FUNCTION__ );                \
                                                    sprintf( &buf[strlen(buf)], format, __VA_ARGS__ );      \
                                                    MWLOG( level, mod, buf); }
    #define MWLOG_ERR( buf, format, ...   )     _LOG_( buf, LEV_ERROR, MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_WARN( buf, format, ...  )     _LOG_( buf, LEV_WARN , MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_INFO( buf, format, ...  )     _LOG_( buf, LEV_INFO , MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_DEBUG( buf, format, ... )     _LOG_( buf, LEV_DEBUG, MOD_CMD, format, __VA_ARGS__ )
#else
    #define _LOG_( buf, level, mod, format, ... ) { sprintf( buf, "%s() - ", __FUNCTION__ );                \
                                                    sprintf( &buf[strlen(buf)], format, ## __VA_ARGS__ );   \
                                                    MWLOG( level, mod, logBuf); }
    #define MWLOG_ERR( buf, format, ...   )     _LOG_( buf, LEV_ERROR, MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_WARN( buf, format, ...  )     _LOG_( buf, LEV_WARN , MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_INFO( buf, format, ...  )     _LOG_( buf, LEV_INFO , MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_DEBUG( buf, format, ... )     _LOG_( buf, LEV_DEBUG, MOD_CMD, format, ## __VA_ARGS__ )
#endif // WIN32

namespace eIDMW
{

void printCPtr( char *c_str, int c_str_len );
xsd__base64Binary *encode_base64( soap *sp, std::string in_str );

class CMDServices{
    public:
        CMDServices( const char *endpoint = NULL );
        virtual ~CMDServices();

        // Get certificate
        CByteArray getCertificate( std::string in_userId );

        // CCMovelSign
        bool sendDataToSign( std::string in_hash, std::string in_pin );

        // ValidateOtp
        CByteArray getSignature( std::string in_code );

    protected:
        soap *getSoap();
        void setSoap( soap * );

        void setEndPoint( const char *endpoint );
        const char *getEndPoint();

        std::string getProcessID();
        void setProcessID( std::string processID );

        std::string getApplicationID();
        void setApplicationID( std::string applicationID );

        std::string getUserId();
        void setUserId( std::string in_userId );

    private:
        soap *m_soap;
        std::string m_applicationID;
        std::string m_processID;
        std::string m_pin;
        std::string m_userId;
        const char *m_endpoint;

        bool init( int recv_timeout, int send_timeout
                          , int connect_timeout, short mustUnderstand );

        // CCMovelSign
        bool CCMovelSign( std::string in_hash, std::string in_pin );

        _ns2__CCMovelSign *get_CCMovelSignRequest( soap *sp
                                                 , char *endpoint
                                                 , std::string in_applicationID
                                                 , std::string in_hash
                                                 , std::string in_pin
                                                 , std::string *in_userId );

        bool checkCCMovelSignResponse( _ns2__CCMovelSignResponse *response );

        // ValidateOtp
        bool ValidateOtp( std::string in_code
                        , unsigned char **outSignature
                        , unsigned int *outSignatureLen );

        _ns2__ValidateOtp *get_ValidateOtpRequest( soap *sp
                                                 , char *endpoint
                                                 , std::string in_applicationID
                                                 , std::string *in_code
                                                 , std::string *in_processId );

        bool checkValidateOtpResponse( _ns2__ValidateOtpResponse *response );

        _ns2__GetCertificate *get_GetCertificateRequest(  soap *sp
                                                , char *endpoint
                                                , std::string in_applicationID
                                                , std::string *in_userId );

        bool checkGetCertificateResponse( _ns2__GetCertificateResponse *response );
        bool GetCertificate( std::string in_userId
                            , char **out_certificate, int *out_certificateLen );
};

}
#endif // CMD_CLIENT_H
