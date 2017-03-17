//STD Library
#include <iostream>
#include <string>

#include "CMD_client.h"
#include "Log.h"
#include "MiscUtil.h"

#define CC_MOVEL_SERVICE_SIGN ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/CCMovelSign" )
#define CC_MOVEL_SERVICE_VALIDATE_OTP ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/ValidateOtp" )

#define STR_EMPTY               ""

namespace eIDMW{

/*  *********************************************************
    ***          printCPtr()                              ***
    ********************************************************* */
void printCPtr( char *c_str, int c_str_len ){
    if ( c_str == NULL ){
        fprintf(stderr, "CMD_client::printCPtr() - Null data\n" );
        return;
    }/* if ( c_str == NULL ) */

    for(int i = 0; i < c_str_len; i++ ){
        printf( "%x", c_str[i] );
    }
    printf( "\n" );
}/* printCPtr() */

/*  *********************************************************
    ***          CMD_client::CMD_client()                 ***
    ********************************************************* */
xsd__base64Binary *CMD_client::encode_base64( string in_str ){

    soap *sp = getSoap();
    if ( sp == NULL ){
        MWLOG_ERR( "Null soap" );
        return NULL;
    }/* if ( sp == NULL ) */

    xsd__base64Binary *encoded = NULL;
    if ( in_str.empty() ){
        MWLOG_ERR( "Empty in_str" );
        return NULL;
    }/* if ( in_str.empty() ) */

    int len;
    char *c_ptr = getCPtr( in_str, &len );
    encoded = soap_new_set_xsd__base64Binary( sp
                                            , (unsigned char *)c_ptr, len
                                            , NULL, NULL, NULL);

    if ( encoded == NULL ){
        /* this is the same pointer as encoded->__ptr */
        if ( c_ptr != NULL ) free( c_ptr );
    }/* if ( encoded == NULL ) */

    return encoded;
}/* CMD_client::encode_base64() */

/*  *********************************************************
    ***          CMD_client::CMD_client()                 ***
    ********************************************************* */
CMD_client::CMD_client( const char *endpoint )
{
    setEndPoint( endpoint );
}/* CMD_client::CMD_client() */

/*  *********************************************************
    ***          CMD_client::~CMD_client()                ***
    ********************************************************* */
CMD_client::~CMD_client()
{
    soap *sp = getSoap();
    if ( sp != NULL ) soap_destroy( sp );
}/* CMD_client::~CMD_client() */

/*  *********************************************************
    ***          CMD_client::init()              ***
    ********************************************************* */
bool CMD_client::init( int recv_timeout, int send_timeout
                        , int connect_timeout, short mustUnderstand )
{
    soap *sp = soap_new2( SOAP_C_UTFSTRING, SOAP_C_UTFSTRING );
    if ( sp == NULL ){
        MWLOG_ERR( "Null soap" );
        return false;
    }/* if ( sp == NULL ) */

    setPin( STR_EMPTY );
    setUserId( STR_EMPTY );

    //Don't change this: it serves as authentication for the service
    setApplicationID( string( "264af13f-c287-4703-9add-10a303b951a3" ) );

    //Define appropriate network timeouts
    sp->recv_timeout = recv_timeout;
    sp->send_timeout = send_timeout;
    sp->connect_timeout = connect_timeout;

    //Dont output mustUnderstand attributes
    sp->mustUnderstand = mustUnderstand;

    //TODO: this disables server certificate verification!!
    int ret = soap_ssl_client_context( sp
                                    , SOAP_SSL_NO_AUTHENTICATION
                                    , NULL, NULL, NULL, NULL, NULL );
    if ( ret != SOAP_OK ){
        MWLOG_ERR( "soap_ssl_client_context() failed - code: %d", ret );
        return false;
    }/* if ( ret != SOAP_OK ) */

    setSoap( sp );

    return true;
}/* CMD_client::init() */

/*  *********************************************************
    ***          CMD_client::getSoap()                    ***
    ********************************************************* */
soap *CMD_client::getSoap(){
    return m_soap;
}/* CMD_client::getSoap() */

/*  *********************************************************
    ***          CMD_client::setSoap()                    ***
    ********************************************************* */
void CMD_client::setSoap( soap *in_soap ){
    m_soap = in_soap;
}/* CMD_client::setSoap() */

/*  *********************************************************
    ***          CMD_client::setEndPoint()                ***
    ********************************************************* */
void CMD_client::setEndPoint( const char *endpoint ){
    m_endpoint = endpoint;
}/* CMD_client::m_endpoint() */

/*  *********************************************************
    ***          CMD_client::getEndPoint()                ***
    ********************************************************* */
const char *CMD_client::getEndPoint(){
    return m_endpoint;
}/* CMD_client::getEndPoint() */

/*  *********************************************************
    ***          CMD_client::getProcessID()               ***
    ********************************************************* */
string CMD_client::getProcessID(){
    return m_processID;
}/* CMD_client::getProcessID() */

/*  *********************************************************
    ***          CMD_client::setProcessID()               ***
    ********************************************************* */
void CMD_client::setProcessID( string processID ){
    m_processID = processID;
}/* CMD_client::setProcessID() */

/*  *********************************************************
    ***          CMD_client::getApplicationID()           ***
    ********************************************************* */
string CMD_client::getApplicationID(){
    return m_applicationID;
}/* CMD_client::getApplicationID() */

/*  *********************************************************
    ***          CMD_client::setApplicationID()           ***
    ********************************************************* */
void CMD_client::setApplicationID( string applicationID ){
    m_applicationID = applicationID;
}/* CMD_client::setApplicationID() */

/*  *********************************************************
    ***          CMD_client::getPin()                     ***
    ********************************************************* */
string CMD_client::getPin(){
    return m_pin;
}/* CMD_client::getPin() */

/*  *********************************************************
    ***          CMD_client::setPin()                     ***
    ********************************************************* */
void CMD_client::setPin( string in_pin ){
    m_pin = in_pin;
}/* CMD_client::setPin() */

/*  *********************************************************
    ***          CMD_client::getUserId()                     ***
    ********************************************************* */
string CMD_client::getUserId(){
    return m_userId;
}/* CMD_client::getUserId() */

/*  *********************************************************
    ***          CMD_client::setUserId()                  ***
    ********************************************************* */
void CMD_client::setUserId( string in_userId ){
    m_userId = in_userId;
}/* CMD_client::setUserId() */

/*  *********************************************************
    ***          CMD_client::get_CCMovelSignRequest()     ***
    ********************************************************* */
_ns2__CCMovelSign *CMD_client::get_CCMovelSignRequest(  soap *sp
                                                      , char *endpoint
                                                      , string in_applicationID
                                                      , string in_hash
                                                      , string in_pin
                                                      , string *in_userId ){
    SOAP_ENV__Header *soapHeader = soap_new_SOAP_ENV__Header( sp );
    soapHeader->wsa__To = endpoint;

    /* TODO
        generate random messageID - generate UUID
        We have to change the MessageID value for each call
    */
    char *messageID = (char *)"urn:uuid:50aafd78-f8ad-4744-8059-c0c4a935bca9";
    soapHeader->wsa__MessageID = messageID;
    soapHeader->wsa__Action = CC_MOVEL_SERVICE_SIGN;

    //Set the created header in our soap structure
    sp->header = soapHeader;

    _ns3__SignRequest *soapBody = soap_new_ns3__SignRequest( sp );

    soapBody->UserId        = in_userId;
    soapBody->Pin           = encode_base64( in_pin );
    soapBody->Hash          = encode_base64( in_hash );
    soapBody->ApplicationId = encode_base64( in_applicationID );

    _ns2__CCMovelSign *send = soap_new_set__ns2__CCMovelSign( sp, soapBody );
    return send;
}/* CMD_client::get_CCMovelSignRequest() */

/*  *********************************************************
    ***          CMD_client::get_ValidateOtpRequest()     ***
    ********************************************************* */
_ns2__ValidateOtp *CMD_client::get_ValidateOtpRequest(  soap *sp
                                                      , char *endpoint
                                                      , string in_applicationID
                                                      , string *in_code
                                                      , string *in_processId ){
    SOAP_ENV__Header *soapHeader = soap_new_SOAP_ENV__Header( sp );
    soapHeader->wsa__To = endpoint;

    /* TODO
        generate random messageID - generate UUID
        We have to change the MessageID value for each call
    */
    char *messageID = (char *)"urn:uuid:50aafd78-f8ad-4744-8059-c0c4a935bcaa";
    soapHeader->wsa__MessageID = messageID;
    soapHeader->wsa__Action = CC_MOVEL_SERVICE_VALIDATE_OTP;

    //Set the created header in our soap structure
    sp->header = soapHeader;

    _ns2__ValidateOtp *soapBody = soap_new__ns2__ValidateOtp( sp );
    if ( soapBody == NULL ) return soapBody;

    soapBody->code          = in_code;
    soapBody->processId     = in_processId;
    soapBody->applicationId = encode_base64( in_applicationID );

    _ns2__ValidateOtp *send = soapBody;
    return send;
}/* CMD_client::get_ValidateOtpRequest() */

/*  *********************************************************
    ***          CMD_client::checkCCMovelSignResponse()   ***
    ********************************************************* */
int CMD_client::checkCCMovelSignResponse( _ns2__CCMovelSignResponse *response ){
    if ( response == NULL ){
        MWLOG_ERR( "Null response" );
        return SOAP_NULL;
    }/* if ( response == NULL ) */

    if ( response->CCMovelSignResult == NULL ){
        MWLOG_ERR( "Null CCMovelSignResult" );
        return SOAP_NULL;
    }/* if ( response->CCMovelSignResult == NULL ) */

    if ( response->CCMovelSignResult->X509Certificate == NULL ){
        MWLOG_ERR( "Null X509Certificate" );
        return SOAP_NULL;
    }/* if ( response->CCMovelSignResult->X509Certificate == NULL ) */

    if ( ( (string)( *response->CCMovelSignResult->X509Certificate ) ).empty() ){
        MWLOG_ERR( "Empty certificate" );
        return SOAP_NO_DATA;
    }/* if ( strResp.empty() ) */

    return SOAP_OK;
}/* checkCCMovelSignResponse() */

/*  *********************************************************
    ***          CMD_client::CCMovelSign()                ***
    ********************************************************* */
bool CMD_client::CCMovelSign( string in_hash, char **out_certificate, int *out_certificateLen ){

    soap *sp = getSoap();
    if ( sp == NULL ){
        MWLOG_ERR( "Null soap" );
        return false;
    }/* if ( sp == NULL ) */

    if ( in_hash.empty() ){
        MWLOG_ERR( "Empty hash" );
        return false;
    }/* if ( in_hash.empty() ) */

    string in_pin = getPin();
    if ( in_pin.empty() ){
        MWLOG_ERR( "Empty pin" );
        return false;
    }/* if ( in_pin.empty() ) */

    string in_userId = getUserId();
    if ( in_userId.empty() ){
        MWLOG_ERR( "Empty userId" );
        return false;
    }/* if ( in_userId.empty() ) */

    /*
        ProcessID initialization
    */
    setProcessID( "" );

    const char *endPoint = getEndPoint();
    WSHttpBinding_USCORECCMovelSignatureProxy proxy
                            = WSHttpBinding_USCORECCMovelSignatureProxy( sp );
    proxy.soap_endpoint = endPoint;

    /*
        Get CCMovelSign request
    */
    _ns2__CCMovelSign *send = get_CCMovelSignRequest( sp
                                                    , (char*)endPoint
                                                    , getApplicationID()
                                                    , in_hash, in_pin
                                                    , &in_userId );
    if ( send == NULL ){
        MWLOG_ERR( "NULL send parameters" );
        return false;
    }/* if ( send == NULL ) */

    /*
        Call CCMovelSign service
    */
    _ns2__CCMovelSignResponse response;
    int ret = proxy.CCMovelSign( NULL, NULL, send, response );

    /*
        Clean pointers before exit
    */
    if ( send->request != NULL ){
        /* Clean pointers before leaving function */
        if ( send->request->Pin != NULL ){
            if ( send->request->Pin->__ptr != NULL )
                free( send->request->Pin->__ptr );
        }/* if ( send->request->Pin != NULL ) */

        if ( send->request->Hash != NULL ){
            if ( send->request->Hash->__ptr != NULL )
                free( send->request->Hash->__ptr );
        }/* if ( send->request->Hash != NULL ) */

        if ( send->request->ApplicationId != NULL ){
            if ( send->request->ApplicationId->__ptr != NULL )
                free( send->request->ApplicationId->__ptr );
        }/* if ( send->request->ApplicationId != NULL ) */
    }/* if ( send->request != NULL ) */

    /*
        Handling errors
    */
    if ( ret != SOAP_OK ){
        if ( ret == SOAP_FAULT ){

            if ( proxy.soap_fault() != NULL ){
                MWLOG_ERR( "SOAP Fault! %s", proxy.soap_fault()->faultstring );
            } else{
                MWLOG_ERR( "Unknown SOAP Fault!" );
            }/* if ( fault != NULL ) */

        } else{
            MWLOG_ERR( "Error code: %d", ret );
        }/* if ( ret == SOAP_FAULT ) */
        return false;
    }/* if ( ret != SOAP_OK ) */

    /*
        Validate response
    */
    ret = checkCCMovelSignResponse( &response ) ;
    if ( ret != SOAP_OK ) return ret;

    /*
        Save ProcessId
    */
    cout << "ProcessId: " << *response.CCMovelSignResult->Status->ProcessId << endl;
    setProcessID( *response.CCMovelSignResult->Status->ProcessId );

    /*
        Process X509Certificate
    */
    if ( out_certificate != NULL ){
        *out_certificate = getCPtr( *response.CCMovelSignResult->X509Certificate
                                    , out_certificateLen );
    }/* if ( out_certificate != NULL ) */

    return true;
}/* CMD_client::CCMovelSign(() */

/*  *********************************************************
    ***          CMD_client::checkValidateOtpResponse()   ***
    ********************************************************* */
int CMD_client::checkValidateOtpResponse( _ns2__ValidateOtpResponse *response ){
    if ( response == NULL ){
        MWLOG_ERR( "Null response" );
        return SOAP_NULL;
    }/* if ( response == NULL ) */

    if ( response->ValidateOtpResult == NULL ){
        MWLOG_ERR( "Null ValidateOtpResult" );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult == NULL ) */

    if ( response->ValidateOtpResult->Signature == NULL ){
        MWLOG_ERR( "Null Signature" );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult->Signature == NULL ) */

    if ( response->ValidateOtpResult->Signature->__ptr == NULL ){
        MWLOG_ERR( "Null Signature pointer" );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult->Signature->__ptr == NULL ) */

    if ( response->ValidateOtpResult->Signature->__size < 1 ){
        MWLOG_ERR( "Invalide Signature pointer size: %d"

                , response->ValidateOtpResult->Signature->__size );
        return SOAP_LENGTH;
    }/* if ( response->ValidateOtpResult->Signature->__ptr == NULL ) */

    return SOAP_OK;
}/* CMD_client::checkValidateOtpResponse(() */

/*  *********************************************************
    ***          CMD_client::ValidateOtp()                ***
    ********************************************************* */
bool CMD_client::ValidateOtp( string in_code
                            , unsigned char **outSignature
                            , unsigned int *outSignatureLen ){
    soap *sp = getSoap();
    if ( sp == NULL ){
        MWLOG_ERR( "Null soap" );
        return false;
    }/* if ( sp == NULL ) */

    if ( in_code.empty() ){
        MWLOG_ERR( "Empty code" );
        return false;
    }/* if ( in_code.empty() ) */

    string code = in_code;
    string processId = getProcessID();
    const char *endPoint = getEndPoint();

    WSHttpBinding_USCORECCMovelSignatureProxy proxy
                            = WSHttpBinding_USCORECCMovelSignatureProxy( sp );
    proxy.soap_endpoint = endPoint;
    /*
        Get ValidateOtp request
    */
    _ns2__ValidateOtp *send = get_ValidateOtpRequest(  sp
                                                     , (char*)endPoint
                                                     , getApplicationID()
                                                     , &code
                                                     , &processId );

    if ( send == NULL ){
        MWLOG_ERR( "Null send parameters" );
        return false;
    }/* if ( send == NULL ) */

    /*
        Call ValidateOtp service
    */
    _ns2__ValidateOtpResponse response;
    int ret = proxy.ValidateOtp( NULL, NULL, send, response );
    if ( ret != SOAP_OK ){
        if ( ret == SOAP_FAULT ){

            if ( proxy.soap_fault() != NULL ){
                MWLOG_ERR( "SOAP Fault! %s", proxy.soap_fault()->faultstring );
            } else{
                MWLOG_ERR( "Unknown SOAP Fault!" );
            }/* if ( fault != NULL ) */

        } else{
            MWLOG_ERR( "Error code: %d", ret );
        }/* if ( ret == SOAP_FAULT ) */
        return false;
    }/* if ( ret != SOAP_OK ) */

    /*
        Validate response
    */
    ret = checkValidateOtpResponse( &response ) ;
    if ( ret != SOAP_OK ) return false;

    /*
        Set signature
    */
    if ( ( outSignature != NULL ) && ( outSignatureLen != NULL ) ){
        *outSignature =
            (unsigned char*)malloc( response.ValidateOtpResult->Signature->__size );

        if ( outSignature == NULL ){
            MWLOG_ERR( "Malloc fail!" );
            return false;
        }/* if ( outSignature == NULL ) */

        memcpy( *outSignature
                , response.ValidateOtpResult->Signature->__ptr
                , response.ValidateOtpResult->Signature->__size );

        *outSignatureLen = response.ValidateOtpResult->Signature->__size;
    }/* if ( ( outSignature != NULL ) && ( outSignatureLen != NULL ) ) */

    return true;
}/* CMD_client::ValidateOtp() */


/*  *************************************************************************************************
    ****
    **** Functions visible to the outside
    ****
    ************************************************************************************************* */
/*  *********************************************************
    ***          CMD_client::getCertificate()             ***
    ********************************************************* */
CByteArray CMD_client::getCertificate( string in_pin, string in_userId ){
    CByteArray empty_certificate;

    if ( in_pin.empty() ){
        MWLOG_ERR( "Empty PIN" );
        return empty_certificate;
    }/* if ( in_pin.empty() ) */

    if ( in_userId.empty() ){
        MWLOG_ERR( "Empty userId" );
        return empty_certificate;
    }/* if ( in_userId.empty() ) */

    // Set variables
    setPin( in_pin );
    setUserId( in_userId );

    char *p_certificate = NULL;
    int certificateLen = 0;
    string in_hash = "\xde\xb2\x53\x63\xff\x9c\x44\x2b\x67\xcb\xa3\xd9\xc5\xef\x21\x6e\x47\x22\xca\xd5";

    bool bRet = CCMovelSign( in_hash, &p_certificate, &certificateLen );
    if ( !bRet ){
        MWLOG_ERR( "Get certificate failed" );
        return empty_certificate;
    }/* if ( !bRet ) */

    char *pem = toPEM( p_certificate, certificateLen );
    free( p_certificate );

    if ( NULL == pem ){
        MWLOG_ERR( "Null pem" );
        return empty_certificate;
    }/* if ( NULL == pem ) */

    unsigned char *der = NULL;
    int derLen = PEM_to_DER( pem, &der );
    free( pem );

    if ( derLen < 0 ){
        MWLOG_ERR( "PEM -> DER conversion failed - len: %d", derLen );
        return empty_certificate;
    }/* if ( derLen < 0 ) */

    CByteArray certificate( (const unsigned char *)der
                            , (unsigned long)derLen );

    return certificate;
}/* CMD_client::getCertificate() */

/*  *********************************************************
    ***          CMD_client::sendDataToSign()                   ***
    ********************************************************* */
bool CMD_client::sendDataToSign( string in_hash ){
    return CCMovelSign( in_hash, NULL, NULL );
}/* CMD_client::sendDataToSign() */

/*  *********************************************************
    ***          CMD_client::getSignature()               ***
    ********************************************************* */
CByteArray CMD_client::getSignature( string in_code ){
    CByteArray empty_certificate;
    unsigned int signLen = 0;
    unsigned char *sign = NULL;

    if ( !ValidateOtp( in_code, &sign, &signLen ) ){
        MWLOG_ERR( "ValidateOtp failed" );
        return empty_certificate;
    }/* if ( !ValidateOtp( in_code, &sign, &signLen ) ) */

    if ( NULL == sign ){
        MWLOG_ERR( "Null signature" );
        return empty_certificate;
    }/* if ( NULL == sign ) */

    if ( signLen <= 0 ){
        free( sign );

        MWLOG_ERR( "Invalid signature length: %d", signLen );
        return empty_certificate;
    }/* if ( signLen <= 0 ) */

    CByteArray signature( (const unsigned char *)sign
                        , (unsigned long)signLen );
    free( sign );

    return signature;
}/* CMD_client::getSignature() */


}/* namespace */
