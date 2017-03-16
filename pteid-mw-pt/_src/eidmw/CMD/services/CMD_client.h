#ifndef CMD_CLIENT_H
#define CMD_CLIENT_H

//STD Library
#include <iostream>
#include <string>
#include <openssl/x509.h>
#include "soapH.h"

#include "WSHttpBinding_USCORECCMovelSignature.nsmap"
#include "soapWSHttpBinding_USCORECCMovelSignatureProxy.h"

char logBuf[512];
#ifdef WIN32
    #define _LOG_( level, mod, format, ... )    { sprintf( logBuf, "%s() - ", __FUNCTION__ );               \
                                                    sprintf( &logBuf[strlen(logBuf)], format, __VA_ARGS__ );\
                                                    MWLOG( level, mod, logBuf); }
    #define MWLOG_ERR( format, ...   )          _LOG_( LEV_ERROR, MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_WARN( format, ...  )          _LOG_( LEV_WARN , MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_INFO( format, ...  )          _LOG_( LEV_INFO , MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_DEBUG( format, ... )          _LOG_( LEV_DEBUG, MOD_CMD, format, __VA_ARGS__ )
#else
    #define _LOG_( level, mod, format, ... )    { sprintf( logBuf, "%s() - ", __FUNCTION__ );                   \
                                                    sprintf( &logBuf[strlen(logBuf)], format, ## __VA_ARGS__ ); \
                                                    MWLOG( level, mod, logBuf); }
    #define MWLOG_ERR( format, ...   )          _LOG_( LEV_ERROR, MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_WARN( format, ...  )          _LOG_( LEV_WARN , MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_INFO( format, ...  )          _LOG_( LEV_INFO , MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_DEBUG( format, ... )          _LOG_( LEV_DEBUG, MOD_CMD, format, ## __VA_ARGS__ )
#endif // WIN32

using namespace std;

namespace eIDMW
{

char *getCPtr( string inStr, int *outLen );
void printCPtr( char *c_str, int c_str_len );

class CMD_client
{
    public:
        CMD_client( const char *endpoint = NULL );
        virtual ~CMD_client();

        bool init( int recv_timeout, int send_timeout
                    , int connect_timeout, short mustUnderstand );

        soap *getSoap();
        void setSoap( soap * );

        WSHttpBinding_USCORECCMovelSignatureProxy getProxy();
        void setProxy( WSHttpBinding_USCORECCMovelSignatureProxy in_proxy
                        , const char *endpoint );

        void setEndPoint( const char *endpoint );
        const char *getEndPoint();

        string getProcessID();
        void setProcessID( string processID );

        string getApplicationID();
        void setApplicationID( string applicationID );

        string getPin();
        void setPin( string in_pin );

        string getUserId();
        void setUserId( string in_userId );

        // Get certificate
        X509 *getCertificateX509( string in_pin, string in_userId );

        // CCMovelSign
        int CCMovelSign( string in_hash, char **out_certificate, int *out_certificateLen );

        // ValidateOtp
        int ValidateOtp( string in_code
                        , unsigned char **out_Signature
                        , unsigned int *out_SignatureLen );

    protected:

    private:
        WSHttpBinding_USCORECCMovelSignatureProxy m_proxy;
        soap *m_soap;
        string m_applicationID;
        string m_processID;
        string m_pin;
        string m_userId;

        const char *m_endpoint;
        const SOAP_ENV__Fault *m_fault;

        xsd__base64Binary *encode_base64( string in_str );

        _ns2__CCMovelSign *get_CCMovelSignRequest( string in_hash
                                                , string in_pin
                                                , string *in_userId );
        int checkCCMovelSignResponse( _ns2__CCMovelSignResponse *response );

        _ns2__ValidateOtp *get_ValidateOtpRequest( string *in_code
                                                , string *in_processId );
        int checkValidateOtpResponse( _ns2__ValidateOtpResponse *response );
};

}
#endif // CMD_CLIENT_H
