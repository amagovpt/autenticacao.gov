//STD Library
#include <iostream>
#include <string>

#include "PDFSignatureClient.h"
#include "Log.h"
#include "MiscUtil.h"

#define CC_MOVEL_SERVICE_SIGN ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/CCMovelSign" )
#define CC_MOVEL_SERVICE_VALIDATE_OTP ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/ValidateOtp" )
#define ENDPOINT_CC_MOVEL_SIGNATURE ( (const char *)"https://dev.cmd.autenticacao.gov.pt/Ama.Authentication.Service/CCMovelSignature.svc" )

#define STR_EMPTY               ""

#define SOAP_RECV_TIMEOUT_DEFAULT       20
#define SOAP_SEND_TIMEOUT_DEFAULT       20
#define SOAP_CONNECT_TIMEOUT_DEFAULT    20
#define SOAP_MUST_NO_UNDERSTAND         0
#define SOAP_MUST_UNDERSTAND            1

namespace eIDMW{

/*  *********************************************************
    ***          printCPtr()                              ***
    ********************************************************* */
void printCPtr( char *c_str, int c_str_len ){
    if ( c_str == NULL ){
        fprintf(stderr, "PDFSignatureClient::printCPtr() - Null data\n" );
        return;
    }/* if ( c_str == NULL ) */

    for(int i = 0; i < c_str_len; i++ ){
        printf( "%x", c_str[i] );
    }
    printf( "\n" );
}/* printCPtr() */

/*  *********************************************************
    ***    PDFSignatureClient::PDFSignatureClient()       ***
    ********************************************************* */
xsd__base64Binary *PDFSignatureClient::encode_base64( string in_str ){

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
}/* PDFSignatureClient::encode_base64() */

/*  *********************************************************
    ***    PDFSignatureClient::PDFSignatureClient()       ***
    ********************************************************* */
PDFSignatureClient::PDFSignatureClient( const char *endpoint ){
    if ( !init( SOAP_RECV_TIMEOUT_DEFAULT
              , SOAP_SEND_TIMEOUT_DEFAULT
              , SOAP_CONNECT_TIMEOUT_DEFAULT
              , SOAP_MUST_NO_UNDERSTAND ) ) return;

    const char *new_endpoint = NULL;
    new_endpoint = ( NULL == endpoint ) ? ENDPOINT_CC_MOVEL_SIGNATURE : endpoint;

    setEndPoint( new_endpoint );
}/* PDFSignatureClient::PDFSignatureClient() */

/*  *********************************************************
    ***    PDFSignatureClient::~PDFSignatureClient()      ***
    ********************************************************* */
PDFSignatureClient::~PDFSignatureClient(){
    soap *sp = getSoap();
    if ( NULL == sp ) return;

    soap_destroy( sp );
    soap_end( sp );
}/* PDFSignatureClient::~PDFSignatureClient() */

/*  *********************************************************
    ***    PDFSignatureClient::init()                     ***
    ********************************************************* */
bool PDFSignatureClient::init( int recv_timeout, int send_timeout
                        , int connect_timeout, short mustUnderstand ){
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
        soap_destroy( sp );
        setSoap( NULL );

        MWLOG_ERR( "soap_ssl_client_context() failed - code: %d", ret );
        return false;
    }/* if ( ret != SOAP_OK ) */

    setSoap( sp );

    return true;
}/* PDFSignatureClient::init() */

/*  *********************************************************
    ***    PDFSignatureClient::getSoap()                  ***
    ********************************************************* */
soap *PDFSignatureClient::getSoap(){
    return m_soap;
}/* PDFSignatureClient::getSoap() */

/*  *********************************************************
    ***    PDFSignatureClient::setSoap()                  ***
    ********************************************************* */
void PDFSignatureClient::setSoap( soap *in_soap ){
    m_soap = in_soap;
}/* PDFSignatureClient::setSoap() */

/*  *********************************************************
    ***    PDFSignatureClient::setEndPoint()              ***
    ********************************************************* */
void PDFSignatureClient::setEndPoint( const char *endpoint ){
    m_endpoint = endpoint;
}/* PDFSignatureClient::m_endpoint() */

/*  *********************************************************
    ***    PDFSignatureClient::getEndPoint()              ***
    ********************************************************* */
const char *PDFSignatureClient::getEndPoint(){
    return m_endpoint;
}/* PDFSignatureClient::getEndPoint() */

/*  *********************************************************
    ***    PDFSignatureClient::getProcessID()             ***
    ********************************************************* */
string PDFSignatureClient::getProcessID(){
    return m_processID;
}/* PDFSignatureClient::getProcessID() */

/*  *********************************************************
    ***    PDFSignatureClient::setProcessID()             ***
    ********************************************************* */
void PDFSignatureClient::setProcessID( string processID ){
    m_processID = processID;
}/* PDFSignatureClient::setProcessID() */

/*  *********************************************************
    ***    PDFSignatureClient::getApplicationID()         ***
    ********************************************************* */
string PDFSignatureClient::getApplicationID(){
    return m_applicationID;
}/* PDFSignatureClient::getApplicationID() */

/*  *********************************************************
    ***    PDFSignatureClient::setApplicationID()         ***
    ********************************************************* */
void PDFSignatureClient::setApplicationID( string applicationID ){
    m_applicationID = applicationID;
}/* PDFSignatureClient::setApplicationID() */

/*  *********************************************************
    ***    PDFSignatureClient::getPin()                   ***
    ********************************************************* */
string PDFSignatureClient::getPin(){
    return m_pin;
}/* PDFSignatureClient::getPin() */

/*  *********************************************************
    ***    PDFSignatureClient::setPin()                   ***
    ********************************************************* */
void PDFSignatureClient::setPin( string in_pin ){
    m_pin = in_pin;
}/* PDFSignatureClient::setPin() */

/*  *********************************************************
    ***    PDFSignatureClient::getUserId()                ***
    ********************************************************* */
string PDFSignatureClient::getUserId(){
    return m_userId;
}/* PDFSignatureClient::getUserId() */

/*  *********************************************************
    ***    PDFSignatureClient::setUserId()                ***
    ********************************************************* */
void PDFSignatureClient::setUserId( string in_userId ){
    m_userId = in_userId;
}/* PDFSignatureClient::setUserId() */

/*  *********************************************************
    ***    PDFSignatureClient::get_CCMovelSignRequest()   ***
    ********************************************************* */
_ns2__CCMovelSign *PDFSignatureClient::get_CCMovelSignRequest(  soap *sp
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
}/* PDFSignatureClient::get_CCMovelSignRequest() */

/*  *********************************************************
    ***    PDFSignatureClient::get_ValidateOtpRequest()   ***
    ********************************************************* */
_ns2__ValidateOtp *PDFSignatureClient::get_ValidateOtpRequest(  soap *sp
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
}/* PDFSignatureClient::get_ValidateOtpRequest() */

/*  *********************************************************
    ***    PDFSignatureClient::checkCCMovelSignResponse() ***
    ********************************************************* */
int PDFSignatureClient::checkCCMovelSignResponse( _ns2__CCMovelSignResponse *response ){
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
    ***    PDFSignatureClient::CCMovelSign()              ***
    ********************************************************* */
bool PDFSignatureClient::CCMovelSign( string in_hash, char **out_certificate, int *out_certificateLen ){

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
}/* PDFSignatureClient::CCMovelSign(() */

/*  *********************************************************
    ***    PDFSignatureClient::checkValidateOtpResponse() ***
    ********************************************************* */
int PDFSignatureClient::checkValidateOtpResponse( _ns2__ValidateOtpResponse *response ){
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
}/* PDFSignatureClient::checkValidateOtpResponse(() */

/*  *********************************************************
    ***    PDFSignatureClient::ValidateOtp()              ***
    ********************************************************* */
bool PDFSignatureClient::ValidateOtp( string in_code
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
}/* PDFSignatureClient::ValidateOtp() */


/*  *************************************************************************************************
    ****
    **** Functions visible to the outside
    ****
    ************************************************************************************************* */
/*  *********************************************************
    ***    PDFSignatureClient::getCertificate()           ***
    ********************************************************* */
CByteArray PDFSignatureClient::getCertificate( string in_pin, string in_userId ){
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

    printf( "before CCMovelSign\n" );

    bool bRet = CCMovelSign( in_hash, &p_certificate, &certificateLen );
    printf( "after CCMovelSign\n" );
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
}/* PDFSignatureClient::getCertificate() */

/*  *********************************************************
    ***    PDFSignatureClient::sendDataToSign()           ***
    ********************************************************* */
bool PDFSignatureClient::sendDataToSign( string in_hash ){
    return CCMovelSign( in_hash, NULL, NULL );
}/* PDFSignatureClient::sendDataToSign() */

/*  *********************************************************
    ***    PDFSignatureClient::getSignature()             ***
    ********************************************************* */
CByteArray PDFSignatureClient::getSignature( string in_code ){
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
}/* PDFSignatureClient::getSignature() */


}/* namespace */
