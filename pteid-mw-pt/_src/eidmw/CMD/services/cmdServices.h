/*-****************************************************************************

 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2017-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#ifndef CMD_CLIENT_H
#define CMD_CLIENT_H

//STD Library
#include <iostream>
#include <string>
#include <openssl/x509.h>
#include "soapH.h"
#include "ByteArray.h"
#include "Log.h"
#include "CMDSignature.h"

#ifdef WIN32
    #define _LOG_( buf, level, mod, format, ... ) { sprintf( buf, "%s() - ", __FUNCTION__ );                \
                                                    sprintf( &buf[strlen(buf)], format, __VA_ARGS__ );      \
                                                    MWLOG( level, mod, buf);                                \
                                                    printf( "%s\n", buf ); }
    #define MWLOG_ERR( buf, format, ...   )     _LOG_( buf, LEV_ERROR, MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_WARN( buf, format, ...  )     _LOG_( buf, LEV_WARN , MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_INFO( buf, format, ...  )     _LOG_( buf, LEV_INFO , MOD_CMD, format, __VA_ARGS__ )
    #define MWLOG_DEBUG( buf, format, ... )     _LOG_( buf, LEV_DEBUG, MOD_CMD, format, __VA_ARGS__ )
#else
    #define _LOG_( buf, level, mod, format, ... ) { sprintf( buf, "%s() - ", __FUNCTION__ );                \
                                                    sprintf( &buf[strlen(buf)], format, ## __VA_ARGS__ );   \
                                                    MWLOG( level, mod, logBuf);                             \
                                                    printf( "%s\n", buf ); }
    #define MWLOG_ERR( buf, format, ...   )     _LOG_( buf, LEV_ERROR, MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_WARN( buf, format, ...  )     _LOG_( buf, LEV_WARN , MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_INFO( buf, format, ...  )     _LOG_( buf, LEV_INFO , MOD_CMD, format, ## __VA_ARGS__ )
    #define MWLOG_DEBUG( buf, format, ... )     _LOG_( buf, LEV_DEBUG, MOD_CMD, format, ## __VA_ARGS__ )
#endif // WIN32

namespace eIDMW {

void printCPtr( char *c_str, int c_str_len );
xsd__base64Binary *encode_base64( soap *sp, std::string in_str );

class CMDServices {
    public:
        CMDServices(std::string basicAuthUser, std::string basicAuthPassword, std::string applicationId);
        virtual ~CMDServices();

        void cancelRequest();

        // GetCertificate
        int getCertificate(CMDProxyInfo proxyInfo, std::string in_userId, std::vector<CByteArray> &out_certificate );

        // GetCertificateWithPin
        int askForCertificate(CMDProxyInfo proxyInfo, std::string in_userId, std::string in_pin);

        // CCMovelSign
		int ccMovelSign(CMDProxyInfo proxyInfo, unsigned char * in_hash, std::string docName, std::string in_pin);

        // CCMovelMultipleSign
		int ccMovelMultipleSign(CMDProxyInfo proxyInfo, std::vector<unsigned char *> in_hashs,
                                std::vector<std::string> docNames, std::string in_pin);

        // ValidateOtp
        int getSignatures(CMDProxyInfo proxyInfo, std::string in_code, std::vector<CByteArray *> out_signature );
        int getCMDCertificate(CMDProxyInfo proxyInfo, std::string in_code, std::vector<CByteArray> &out_certificate);

        static std::string getEndpoint();

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

        void enableBasicAuthentication();

    private:
        soap *m_soap;
        std::string m_applicationID;
        std::string m_processID;
        std::string m_userId; // this is the phone number.
        std::string m_basicAuthUser;
        std::string m_basicAuthPassword;
        const char *m_endpoint;

        bool init(int recv_timeout, int send_timeout, int connect_timeout, short mustUnderstand);

        // CCMovelSign
        _ns2__CCMovelSign *get_CCMovelSignRequest( soap *sp
												 , std::string in_applicationID, std::string *docName
                                                 , unsigned char * in_hash
                                                 , std::string *in_pin
                                                 , std::string *in_userId );

        int checkCCMovelSignResponse( _ns2__CCMovelSignResponse *response );

        // CCMovelMultipleSign
        _ns2__CCMovelMultipleSign *get_CCMovelMultipleSignRequest( soap *sp
												 , std::string in_applicationID, std::vector<std::string *> docNames
                                                 , std::vector<unsigned char *> in_hashes
                                                 , std::vector<std::string *> ids
                                                 , std::string *in_pin
                                                 , std::string *in_userId );

        int checkCCMovelMultipleSignResponse( _ns2__CCMovelMultipleSignResponse *response );

        // ValidateOtp
        int ValidateOtp(CMDProxyInfo proxyInfo, std::string in_code
                        , std::vector<unsigned char *> *outSignature
                        , std::vector<unsigned int> *outSignatureLen);
        int ValidateOtp(CMDProxyInfo proxyInfo, std::string in_code
                        , std::string *outCertificate);

        int sendValidateOtp(CMDProxyInfo proxyInfo, std::string in_code, _ns2__ValidateOtpResponse &response);

        _ns2__ValidateOtp *get_ValidateOtpRequest( soap *sp
                                                 , std::string in_applicationID
                                                 , std::string *in_code
                                                 , std::string *in_processId );

        int checkValidateOtpResponse( _ns2__ValidateOtpResponse *response );

        // GetCertificate
        _ns2__GetCertificate *get_GetCertificateRequest(soap *sp
            , std::string in_applicationID
            , std::string *in_userId);

        int checkGetCertificateResponse(_ns2__GetCertificateResponse *response);
        int GetCertificate(CMDProxyInfo proxyInfo, std::string in_userId
            , char **out_certificate, int *out_certificateLen);

        // GetCertificateWithPin
        _ns2__GetCertificateWithPin *get_GetCertificateWithPinRequest(soap *sp
                                                , std::string in_applicationID
                                                , std::string *in_userId
                                                , std::string *in_pin);

        int checkGetCertificateWithPinResponse( _ns2__GetCertificateWithPinResponse *response );
};

}
#endif // CMD_CLIENT_H
