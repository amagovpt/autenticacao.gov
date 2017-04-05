//STD Library
#include <iostream>
#include <string>

#include "CMDServices.h"
#include "WSHttpBinding_USCORECCMovelSignature.nsmap"
#include "soapWSHttpBinding_USCORECCMovelSignatureProxy.h"

#include "MiscUtil.h"

#define CC_MOVEL_SERVICE_GET_CERTIFICATE    ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/GetCertificate" )
#define CC_MOVEL_SERVICE_SIGN               ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/CCMovelSign" )
#define CC_MOVEL_SERVICE_VALIDATE_OTP       ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/ValidateOtp" )
#define ENDPOINT_CC_MOVEL_SIGNATURE         ( (const char *)"https://dev.cmd.autenticacao.gov.pt/Ama.Authentication.Service/CCMovelSignature.svc" )

#define STR_EMPTY                           ""

#define SOAP_RECV_TIMEOUT_DEFAULT           20
#define SOAP_SEND_TIMEOUT_DEFAULT           20
#define SOAP_CONNECT_TIMEOUT_DEFAULT        20
#define SOAP_MUST_NO_UNDERSTAND             0
#define SOAP_MUST_UNDERSTAND                1

static char logBuf[512];

namespace eIDMW{

/*  *********************************************************
    ***    CMDServices::CMDServices()       ***
    ********************************************************* */
xsd__base64Binary *encode_base64( soap *sp, std::string in_str ){

    if ( sp == NULL ){
        MWLOG_ERR( logBuf, "Null soap" );
        return NULL;
    }/* if ( sp == NULL ) */

    xsd__base64Binary *encoded = NULL;
    if ( in_str.empty() ){
        MWLOG_ERR( logBuf, "Empty in_str" );
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
}/* CMDServices::encode_base64() */

/*  *********************************************************
    ***    handleError()                                  ***
    ********************************************************* */
bool handleError( WSHttpBinding_USCORECCMovelSignatureProxy proxy, int ret ){

    if ( ret != SOAP_OK ){
        if ( ret == SOAP_FAULT ){

            if ( proxy.soap_fault() != NULL ){
                MWLOG_ERR( logBuf, "SOAP Fault! %s", proxy.soap_fault()->faultstring );
            } else{
                MWLOG_ERR( logBuf, "Unknown SOAP Fault!" );
            }/* if ( fault != NULL ) */

        } else{
            MWLOG_ERR( logBuf, "Error code: %d", ret );
        }/* if ( ret == SOAP_FAULT ) */

        return false;

    }/* if ( ret != SOAP_OK ) */

    return true;
}/* handleError() */

/*  *******************************************************************************************************************
    ****
    **** CMDServices class - general methods
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::CMDServices()                     ***
    ********************************************************* */
CMDServices::CMDServices( const char *endpoint ){
    if ( !init( SOAP_RECV_TIMEOUT_DEFAULT
              , SOAP_SEND_TIMEOUT_DEFAULT
              , SOAP_CONNECT_TIMEOUT_DEFAULT
              , SOAP_MUST_NO_UNDERSTAND ) ) return;

    const char *new_endpoint = NULL;
    new_endpoint = ( NULL == endpoint ) ? ENDPOINT_CC_MOVEL_SIGNATURE : endpoint;

    setEndPoint( new_endpoint );
}/* CMDServices::CMDServices() */

/*  *********************************************************
    ***    CMDServices::~CMDServices()                    ***
    ********************************************************* */
CMDServices::~CMDServices(){
    soap *sp = getSoap();
    if ( NULL == sp ) return;

    soap_destroy( sp );
    soap_end( sp );

    setSoap( NULL );
}/* CMDServices::~CMDServices() */

/*  *********************************************************
    ***    CMDServices::init()                            ***
    ********************************************************* */
bool CMDServices::init( int recv_timeout, int send_timeout
                        , int connect_timeout, short mustUnderstand ){
    soap *sp = soap_new2( SOAP_C_UTFSTRING, SOAP_C_UTFSTRING );
    if ( sp == NULL ){
        MWLOG_ERR( logBuf, "Null soap" );
        return false;
    }/* if ( sp == NULL ) */

    setUserId( STR_EMPTY );

    //Don't change this: it serves as authentication for the service
    setApplicationID( std::string( "264af13f-c287-4703-9add-10a303b951a3" ) );

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

        MWLOG_ERR( logBuf, "soap_ssl_client_context() failed - code: %d", ret );
        return false;
    }/* if ( ret != SOAP_OK ) */

    setSoap( sp );

    return true;
}/* CMDServices::init() */

/*  *********************************************************
    ***    CMDServices::getSoap()                         ***
    ********************************************************* */
soap *CMDServices::getSoap(){
    return m_soap;
}/* CMDServices::getSoap() */

/*  *********************************************************
    ***    CMDServices::setSoap()                         ***
    ********************************************************* */
void CMDServices::setSoap( soap *in_soap ){
    m_soap = in_soap;
}/* CMDServices::setSoap() */

/*  *********************************************************
    ***    CMDServices::setEndPoint()                     ***
    ********************************************************* */
void CMDServices::setEndPoint( const char *endpoint ){
    m_endpoint = endpoint;
}/* CMDServices::m_endpoint() */

/*  *********************************************************
    ***    CMDServices::getEndPoint()                     ***
    ********************************************************* */
const char *CMDServices::getEndPoint(){
    return m_endpoint;
}/* CMDServices::getEndPoint() */

/*  *********************************************************
    ***    CMDServices::getProcessID()                    ***
    ********************************************************* */
std::string CMDServices::getProcessID(){
    return m_processID;
}/* CMDServices::getProcessID() */

/*  *********************************************************
    ***    CMDServices::setProcessID()                    ***
    ********************************************************* */
void CMDServices::setProcessID( std::string processID ){
    m_processID = processID;
}/* CMDServices::setProcessID() */

/*  *********************************************************
    ***    CMDServices::getApplicationID()                ***
    ********************************************************* */
std::string CMDServices::getApplicationID(){
    return m_applicationID;
}/* CMDServices::getApplicationID() */

/*  *********************************************************
    ***    CMDServices::setApplicationID()                ***
    ********************************************************* */
void CMDServices::setApplicationID( std::string applicationID ){
    m_applicationID = applicationID;
}/* CMDServices::setApplicationID() */

/*  *********************************************************
    ***    CMDServices::getUserId()                       ***
    ********************************************************* */
std::string CMDServices::getUserId(){
    return m_userId;
}/* CMDServices::getUserId() */

/*  *********************************************************
    ***    CMDServices::setUserId()                       ***
    ********************************************************* */
void CMDServices::setUserId( std::string in_userId ){
    m_userId = in_userId;
}/* CMDServices::setUserId() */

/*  *******************************************************************************************************************
    ****
    **** GetCertificate
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::get_GetCertificateRequest()       ***
    ********************************************************* */
_ns2__GetCertificate *CMDServices::get_GetCertificateRequest(
                                                  soap *sp
                                                , char *endpoint
                                                , std::string in_applicationID
                                                , std::string *in_userId ){
    SOAP_ENV__Header *soapHeader = soap_new_SOAP_ENV__Header( sp );
    soapHeader->wsa__To = endpoint;

    /* TODO
        generate random messageID - generate UUID
        We have to change the MessageID value for each call
    */
    char *messageID = (char *)"urn:uuid:50aafd78-f8ad-4744-8059-c0c4a935bca8";
    soapHeader->wsa__MessageID = messageID;
    soapHeader->wsa__Action = CC_MOVEL_SERVICE_GET_CERTIFICATE;

    //Set the created header in our soap structure
    sp->header = soapHeader;

    _ns2__GetCertificate *send = soap_new__ns2__GetCertificate( sp );
    if ( NULL == send ) return send;

    send->applicationId = encode_base64( sp, in_applicationID );
    send->userId        = in_userId;

    return send;
}/* CMDServices::get_GetCertificateRequest() */

/*  *********************************************************
    ***    CMDServices::checkGetCertificateResponse()     ***
    ********************************************************* */
bool CMDServices::checkGetCertificateResponse(
                                    _ns2__GetCertificateResponse *response ){
    if ( response == NULL ){
        MWLOG_ERR( logBuf, "Null response" );
        return false;
    }/* if ( response == NULL ) */

    if ( response->GetCertificateResult == NULL ){
        MWLOG_ERR( logBuf, "Null GetCertificateResult" );
        return false;
    }/* if ( response->GetCertificateResult == NULL ) */

    return true;
}/* CMDServices::checkGetCertificateResponse(() */

/*  *********************************************************
    ***    CMDServices::GetCertificate()                  ***
    ********************************************************* */
bool CMDServices::GetCertificate( std::string in_userId
                                , char **out_certificate
                                , int *out_certificateLen ){
    soap *sp = getSoap();
    if ( sp == NULL ){
        MWLOG_ERR( logBuf, "Null soap" );
        return false;
    }/* if ( sp == NULL ) */

    if ( in_userId.empty() ){
        MWLOG_ERR( logBuf, "Empty userId" );
        return false;
    }/* if ( in_userId.empty() ) */

    const char *endPoint = getEndPoint();
    WSHttpBinding_USCORECCMovelSignatureProxy proxy
                            = WSHttpBinding_USCORECCMovelSignatureProxy( sp );
    proxy.soap_endpoint = endPoint;

    /*
        Get GetCertificate request
    */
    _ns2__GetCertificate *send = get_GetCertificateRequest( sp
                                                        , (char*)endPoint
                                                        , getApplicationID()
                                                        , &in_userId );
    if ( send == NULL ){
        MWLOG_ERR( logBuf, "NULL send parameters" );
        return false;
    }/* if ( send == NULL ) */

    /*
        Call GetCertificate service
    */
    _ns2__GetCertificateResponse response;
    int ret = proxy.GetCertificate( send, response );

    /*
        Clean pointers before exit
    */
    if ( send->applicationId != NULL ){
        if ( send->applicationId->__ptr != NULL )
            free( send->applicationId->__ptr );
    }/* if ( send->applicationId != NULL ) */

    /*
        Handling errors
    */
    if ( !handleError( proxy, ret ) ) return false;

    /*
        Validate response
    */
    if ( !checkGetCertificateResponse( &response ) != SOAP_OK ) return false;

    /*
        Process X509Certificate
    */
    if ( out_certificate != NULL ){
        *out_certificate = getCPtr( *response.GetCertificateResult
                                    , out_certificateLen );
    }/* if ( out_certificate != NULL ) */

    return true;
}/* CMDServices::GetCertificate() */

/*  *******************************************************************************************************************
    ****
    **** CCMovelSign
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::get_CCMovelSignRequest()          ***
    ********************************************************* */
_ns2__CCMovelSign *CMDServices::get_CCMovelSignRequest(
                                                soap *sp
                                              , char *endpoint
                                              , std::string in_applicationID
                                              , std::string in_hash
                                              , std::string in_pin
                                              , std::string *in_userId ){
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

    soapBody->ApplicationId = encode_base64( sp, in_applicationID );
    soapBody->Hash          = encode_base64( sp, in_hash );
    soapBody->Pin           = encode_base64( sp, in_pin );
    soapBody->UserId        = in_userId;

    _ns2__CCMovelSign *send = soap_new_set__ns2__CCMovelSign( sp, soapBody );
    return send;
}/* CMDServices::get_CCMovelSignRequest() */

/*  *********************************************************
    ***    CMDServices::checkCCMovelSignResponse()        ***
    ********************************************************* */
bool CMDServices::checkCCMovelSignResponse( _ns2__CCMovelSignResponse *response ){
    if ( response == NULL ){
        MWLOG_ERR( logBuf, "Null response" );
        return false;
    }/* if ( response == NULL ) */

    if ( response->CCMovelSignResult == NULL ){
        MWLOG_ERR( logBuf, "Null CCMovelSignResult" );
        return false;
    }/* if ( response->CCMovelSignResult == NULL ) */

    return true;
}/* checkCCMovelSignResponse() */

/*  *********************************************************
    ***    CMDServices::CCMovelSign()                     ***
    ********************************************************* */
bool CMDServices::CCMovelSign( std::string in_hash, std::string in_pin ){

    soap *sp = getSoap();
    if ( sp == NULL ){
        MWLOG_ERR( logBuf, "Null soap" );
        return false;
    }/* if ( sp == NULL ) */

    if ( in_hash.empty() ){
        MWLOG_ERR( logBuf, "Empty hash" );
        return false;
    }/* if ( in_hash.empty() ) */

    if ( in_pin.empty() ){
        MWLOG_ERR( logBuf, "Empty pin" );
        return false;
    }/* if ( in_pin.empty() ) */

    std::string in_userId = getUserId();
    if ( in_userId.empty() ){
        MWLOG_ERR( logBuf, "Empty userId" );
        return false;
    }/* if ( in_userId.empty() ) */

    /*
        ProcessID initialization
    */
    setProcessID( STR_EMPTY );

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
        MWLOG_ERR( logBuf, "NULL send parameters" );
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
    if ( !handleError( proxy, ret ) ) return false;

    /*
        Validate response
    */
    if ( !checkCCMovelSignResponse( &response ) != SOAP_OK ) return false;

    /*
        Save ProcessId
    */
    cout << "ProcessId: " << *response.CCMovelSignResult->ProcessId << endl;
    setProcessID( *response.CCMovelSignResult->ProcessId );

    return true;
}/* CMDServices::CCMovelSign(() */

/*  *******************************************************************************************************************
    ****
    **** ValidateOtp
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::get_ValidateOtpRequest()          ***
    ********************************************************* */
_ns2__ValidateOtp *CMDServices::get_ValidateOtpRequest(  soap *sp
                                                      , char *endpoint
                                                      , std::string in_applicationID
                                                      , std::string *in_code
                                                      , std::string *in_processId ){
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

    _ns2__ValidateOtp *send = soap_new__ns2__ValidateOtp( sp );
    if ( send == NULL ) return NULL;

    send->code          = in_code;
    send->processId     = in_processId;
    send->applicationId = encode_base64( sp, in_applicationID );

    return send;
}/* CMDServices::get_ValidateOtpRequest() */

/*  *********************************************************
    ***    CMDServices::checkValidateOtpResponse()        ***
    ********************************************************* */
bool CMDServices::checkValidateOtpResponse( _ns2__ValidateOtpResponse *response ){
    if ( response == NULL ){
        MWLOG_ERR( logBuf, "Null response" );
        return false;
    }/* if ( response == NULL ) */

    if ( response->ValidateOtpResult == NULL ){
        MWLOG_ERR( logBuf, "Null ValidateOtpResult" );
        return false;
    }/* if ( response->ValidateOtpResult == NULL ) */

    if ( response->ValidateOtpResult->Signature == NULL ){
        MWLOG_ERR( logBuf, "Null Signature" );
        return false;
    }/* if ( response->ValidateOtpResult->Signature == NULL ) */

    if ( response->ValidateOtpResult->Signature->__ptr == NULL ){
        MWLOG_ERR( logBuf, "Null Signature pointer" );
        return false;
    }/* if ( response->ValidateOtpResult->Signature->__ptr == NULL ) */

    if ( response->ValidateOtpResult->Signature->__size < 1 ){
        MWLOG_ERR( logBuf, "Invalide Signature pointer size: %d"

                , response->ValidateOtpResult->Signature->__size );
        return false;
    }/* if ( response->ValidateOtpResult->Signature->__ptr == NULL ) */

    return true;
}/* CMDServices::checkValidateOtpResponse(() */

/*  *********************************************************
    ***    CMDServices::ValidateOtp()                     ***
    ********************************************************* */
bool CMDServices::ValidateOtp( std::string in_code
                            , unsigned char **outSignature
                            , unsigned int *outSignatureLen ){
    soap *sp = getSoap();
    if ( sp == NULL ){
        MWLOG_ERR( logBuf, "Null soap" );
        return false;
    }/* if ( sp == NULL ) */

    if ( in_code.empty() ){
        MWLOG_ERR( logBuf, "Empty code" );
        return false;
    }/* if ( in_code.empty() ) */

    std::string code = in_code;
    std::string processId = getProcessID();
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
        MWLOG_ERR( logBuf, "Null send parameters" );
        return false;
    }/* if ( send == NULL ) */

    /*
        Call ValidateOtp service
    */
    _ns2__ValidateOtpResponse response;
    int ret = proxy.ValidateOtp( NULL, NULL, send, response );

    /*
        Clean pointers before exit
    */
    if ( send->applicationId != NULL ){
        if ( send->applicationId->__ptr != NULL )
            free( send->applicationId->__ptr );
    }/* if ( send->applicationId != NULL ) */

    /*
        Handling errors
    */
    if ( !handleError( proxy, ret ) ) return false;

    /*
        Validate response
    */
    if ( !checkValidateOtpResponse( &response ) != SOAP_OK ) return false;

    /*
        Set signature
    */
    if ( ( outSignature != NULL ) && ( outSignatureLen != NULL ) ){
        *outSignature =
            (unsigned char*)malloc(
                            response.ValidateOtpResult->Signature->__size );

        if ( outSignature == NULL ){
            MWLOG_ERR( logBuf, "Malloc fail!" );
            return false;
        }/* if ( outSignature == NULL ) */

        memcpy( *outSignature
                , response.ValidateOtpResult->Signature->__ptr
                , response.ValidateOtpResult->Signature->__size );

        *outSignatureLen = response.ValidateOtpResult->Signature->__size;
    }/* if ( ( outSignature != NULL ) && ( outSignatureLen != NULL ) ) */

    return true;
}/* CMDServices::ValidateOtp() */

/*  *******************************************************************************************************************
    ****
    **** Functions visible to the outside
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::getCertificate()                  ***
    ********************************************************* */
CByteArray CMDServices::getCertificate( std::string in_userId ){
    CByteArray empty_certificate;

    if ( in_userId.empty() ){
        MWLOG_ERR( logBuf, "Empty userId" );
        return empty_certificate;
    }/* if ( in_userId.empty() ) */

    char *p_certificate = NULL;
    int certificateLen = 0;

    if ( !GetCertificate( in_userId, &p_certificate, &certificateLen ) ){
        MWLOG_ERR( logBuf, "Get certificate failed" );
        return empty_certificate;
    }/* if ( !GetCertificate( in_userId, &p_certificate, &certificateLen ) ) */

    char *pem = toPEM( p_certificate, certificateLen );
    free( p_certificate );

    if ( NULL == pem ){
        MWLOG_ERR( logBuf, "Null pem" );
        return empty_certificate;
    }/* if ( NULL == pem ) */

    unsigned char *der = NULL;
    int derLen = PEM_to_DER( pem, &der );
    free( pem );

    if ( derLen < 0 ){
        MWLOG_ERR( logBuf, "PEM -> DER conversion failed - len: %d", derLen );
        return empty_certificate;
    }/* if ( derLen < 0 ) */

    CByteArray certificate( (const unsigned char *)der
                            , (unsigned long)derLen );

    /* Set variables */
    setUserId( in_userId );

    return certificate;
}/* CMDServices::getCertificate() */

/*  *********************************************************
    ***    CMDServices::sendDataToSign()                  ***
    ********************************************************* */
bool CMDServices::sendDataToSign( std::string in_hash, std::string in_pin ){
    return CCMovelSign( in_hash, in_pin );
}/* CMDServices::sendDataToSign() */

/*  *********************************************************
    ***    CMDServices::getSignature()                    ***
    ********************************************************* */
CByteArray CMDServices::getSignature( std::string in_code ){
    CByteArray empty_certificate;
    unsigned int signLen = 0;
    unsigned char *sign = NULL;

    if ( !ValidateOtp( in_code, &sign, &signLen ) ){
        MWLOG_ERR( logBuf, "ValidateOtp failed" );
        return empty_certificate;
    }/* if ( !ValidateOtp( in_code, &sign, &signLen ) ) */

    if ( NULL == sign ){
        MWLOG_ERR( logBuf, "Null signature" );
        return empty_certificate;
    }/* if ( NULL == sign ) */

    if ( signLen <= 0 ){
        free( sign );

        MWLOG_ERR( logBuf, "Invalid signature length: %d", signLen );
        return empty_certificate;
    }/* if ( signLen <= 0 ) */

    CByteArray signature( (const unsigned char *)sign
                        , (unsigned long)signLen );
    free( sign );

    return signature;
}/* CMDServices::getSignature() */


}/* namespace */
