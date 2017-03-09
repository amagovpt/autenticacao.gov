#ifndef CMD_CLIENT_H
#define CMD_CLIENT_H

//STD Library
#include <iostream>
#include <string>
#include "soapH.h"
#include "ByteArray.h"

#include "WSHttpBinding_USCORECCMovelSignature.nsmap"
#include "soapWSHttpBinding_USCORECCMovelSignatureProxy.h"

using namespace std;

namespace eIDMW
{

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

        string getCertificate();
        void setCertificate( string certificate );

        string getApplicationID();
        void setApplicationID( string applicationID );

        CByteArray getSignature();
        void setSignature( CByteArray Signature );
        void setSignature( unsigned char *ptr, unsigned int size );

        // CCMovelSign
        int CCMovelSign( string in_hash, string in_pin, string in_userId );

        // ValidateOtp
        int ValidateOtp( string in_code );

    protected:

    private:
        WSHttpBinding_USCORECCMovelSignatureProxy m_proxy;
        soap *m_soap;
        string m_applicationID;
        string m_processID;
        string m_messageID;
        string m_userId;
        string m_hash;
        string m_pin;
        string m_certificate;
        const char *m_endpoint;
        const SOAP_ENV__Fault *m_fault;
        CByteArray m_Signature;

        char *getCPtr( string inStr, int *outLen );
        void printCPtr( char *c_str, int c_str_len );
        xsd__base64Binary *encode_base64Binary( string in_str );
        _ns2__CCMovelSign *get_CCMovelSignRequest( string in_hash
                                                    , string in_pin
                                                    , string in_userId );
        _ns2__ValidateOtp *get_ValidateOtpRequest( string in_code );
};

}
#endif // CMD_CLIENT_H
