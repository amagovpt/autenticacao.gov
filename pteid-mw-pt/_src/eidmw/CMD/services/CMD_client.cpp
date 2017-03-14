//STD Library
#include <iostream>
#include <string>

#include "CMD_client.h"
#include "Log.h"

#define CC_MOVEL_SERVICE_SIGN ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/CCMovelSign" )
#define CC_MOVEL_SERVICE_VALIDATE_OTP ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/ValidateOtp" )

namespace eIDMW{

/*  *********************************************************
    ***          CMD_client::getCPtr()                    ***
    ********************************************************* */
char *CMD_client::getCPtr( string inStr, int *outLen ){
    char *c_str;

    c_str = new char[ inStr.length() + 1];
    strcpy( c_str, inStr.c_str() );

    if ( outLen != NULL ) *outLen = strlen( c_str );

    return c_str;
}/* CMD_client::getCPtr() */

/*  *********************************************************
    ***          CMD_client::printCPtr()                  ***
    ********************************************************* */
void CMD_client::printCPtr( char *c_str, int c_str_len ){
    if ( c_str == NULL ){
        fprintf(stderr, "CMD_client::printCPtr() - Null data\n" );
        return;
    }/* if ( c_str == NULL ) */

    for(int i = 0; i < c_str_len; i++ ){
        printf( "%x", c_str[i] );
    }
    printf( "\n" );
}/* CMD_client::printCPtr() */

/*  *********************************************************
    ***          CMD_client::CMD_client()                 ***
    ********************************************************* */
xsd__base64Binary *CMD_client::encode_base64( string in_str ){

    xsd__base64Binary *encoded = NULL;
    if ( in_str.empty() ){
        MWLOG( LEV_ERROR, MOD_CMD, "%s - Empty in_str", __FUNCTION__ );
        return NULL;
    }/* if ( in_str.empty() ) */

    int len;
    char *c_ptr = getCPtr( in_str, &len );
    encoded = soap_new_set_xsd__base64Binary( getSoap()
                                            , (unsigned char *)c_ptr, len
                                            , NULL, NULL, NULL);

    if ( encoded == NULL ){
        /* this is the same pointer as encoded->__ptr */
        if ( c_ptr != NULL ) delete[] c_ptr;
    }/* if ( encoded == NULL ) */

    return encoded;
}/* CMD_client::encode_base64() */

/*  *********************************************************
    ***          CMD_client::CMD_client()                 ***
    ********************************************************* */
CMD_client::CMD_client( const char *endpoint )
{
    soap *sp = soap_new2( SOAP_C_UTFSTRING, SOAP_C_UTFSTRING );
    setSoap( sp );

    if ( sp != NULL ){
        WSHttpBinding_USCORECCMovelSignatureProxy proxy
                    = WSHttpBinding_USCORECCMovelSignatureProxy( getSoap() );
        setProxy( proxy, endpoint );
        setEndPoint( endpoint );
        /*
            Variables initialization
        */
        m_fault = NULL;

        //Don't change this: it serves as authentication for the service
        setApplicationID( string( "264af13f-c287-4703-9add-10a303b951a3" ) );
    }/* if ( sp != NULL ) */
}/* CMD_client::CMD_client() */

/*  *********************************************************
    ***          CMD_client::~CMD_client()                ***
    ********************************************************* */
CMD_client::~CMD_client()
{
    setEndPoint( NULL );
}/* CMD_client::~CMD_client() */

/*  *********************************************************
    ***          CMD_client::init()                       ***
    ********************************************************* */
bool CMD_client::init( int recv_timeout, int send_timeout
                        , int connect_timeout, short mustUnderstand )
{
    if ( getSoap() == NULL ) return false;

    //Define appropriate network timeouts
    getSoap()->recv_timeout = recv_timeout;
    getSoap()->send_timeout = send_timeout;
    getSoap()->connect_timeout = connect_timeout;

    //Dont output mustUnderstand attributes
    getSoap()->mustUnderstand = mustUnderstand;

    //TODO: this disables server certificate verification!!
    soap_ssl_client_context( getSoap()
                            , SOAP_SSL_NO_AUTHENTICATION
                            , NULL, NULL, NULL, NULL, NULL );

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
    ***          CMD_client::setProxy()                   ***
    ********************************************************* */
void CMD_client::setProxy( WSHttpBinding_USCORECCMovelSignatureProxy in_proxy
                            , const char *endpoint ){
    m_proxy = in_proxy;
    m_proxy.soap_endpoint = endpoint;
}/* CMD_client::setProxy() */

/*  *********************************************************
    ***          CMD_client::getProxy()                   ***
    ********************************************************* */
WSHttpBinding_USCORECCMovelSignatureProxy CMD_client::getProxy(){
    return m_proxy;
}/* CMD_client::getProxy() */

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
    ***          CMD_client::get_CCMovelSignRequest()     ***
    ********************************************************* */
_ns2__CCMovelSign * CMD_client::get_CCMovelSignRequest( string in_hash
                                                      , string in_pin
                                                      , string *in_userId ){
    SOAP_ENV__Header *soapHeader = soap_new_SOAP_ENV__Header( getSoap() );
    soapHeader->wsa__To = (char *)getEndPoint();

    /* TODO
        generate random messageID - generate UUID
        We have to change the MessageID value for each call
    */
    char *messageID = (char *)"urn:uuid:50aafd78-f8ad-4744-8059-c0c4a935bca9";
    soapHeader->wsa__MessageID = messageID;
    soapHeader->wsa__Action = CC_MOVEL_SERVICE_SIGN;

    //Set the created header in our soap structure
    getSoap()->header = soapHeader;

    _ns3__SignRequest *soapBody = soap_new_ns3__SignRequest( getSoap() );

    soapBody->UserId        = in_userId;
    soapBody->Pin           = encode_base64( in_pin );
    soapBody->Hash          = encode_base64( in_hash );
    soapBody->ApplicationId = encode_base64( getApplicationID() );

    _ns2__CCMovelSign *send = soap_new_set__ns2__CCMovelSign( getSoap()
                                                            , soapBody );
    return send;
}/* CMD_client::get_CCMovelSignRequest() */

/*  *********************************************************
    ***          CMD_client::get_ValidateOtpRequest()     ***
    ********************************************************* */
_ns2__ValidateOtp *CMD_client::get_ValidateOtpRequest( string *in_code
                                                    , string *in_processId ){

    SOAP_ENV__Header *soapHeader = soap_new_SOAP_ENV__Header( getSoap() );
    soapHeader->wsa__To = (char *)getEndPoint();

    /* TODO
        generate random messageID - generate UUID
        We have to change the MessageID value for each call
    */
    char *messageID = (char *)"urn:uuid:50aafd78-f8ad-4744-8059-c0c4a935bcaa";
    soapHeader->wsa__MessageID = messageID;
    soapHeader->wsa__Action = CC_MOVEL_SERVICE_VALIDATE_OTP;

    //Set the created header in our soap structure
    getSoap()->header = soapHeader;

    _ns2__ValidateOtp *soapBody = soap_new__ns2__ValidateOtp( getSoap() );
    if ( soapBody == NULL ) return soapBody;

    soapBody->code          = in_code;
    soapBody->processId     = in_processId;
    soapBody->applicationId = encode_base64( getApplicationID() );

    _ns2__ValidateOtp *send = soapBody;
    return send;
}/* CMD_client::get_ValidateOtpRequest() */

/*  *********************************************************
    ***          CMD_client::checkCCMovelSignResponse()   ***
    ********************************************************* */
int CMD_client::checkCCMovelSignResponse( _ns2__CCMovelSignResponse *response ){
    if ( response == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Null response"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response == NULL ) */

    if ( response->CCMovelSignResult == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Null CCMovelSignResult"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->CCMovelSignResult == NULL ) */

    if ( response->CCMovelSignResult->X509Certificate == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Null X509Certificate"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->CCMovelSignResult->X509Certificate == NULL ) */

    if ( ( (string)( *response->CCMovelSignResult->X509Certificate ) ).empty() ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Empty certificate"
                , __FUNCTION__ );
        return SOAP_NO_DATA;
    }/* if ( strResp.empty() ) */

    if ( response->CCMovelSignResult->Status == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Null Status"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->CCMovelSignResult->Status == NULL ) */

    if ( response->CCMovelSignResult->Status->ProcessId == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Null ProcessId"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->CCMovelSignResult->Status->ProcessId == NULL ) */

    return SOAP_OK;
}/* checkCCMovelSignResponse() */

/*  *********************************************************
    ***          CMD_client::CCMovelSign()                ***
    ********************************************************* */
int CMD_client::CCMovelSign( string in_hash, string in_pin, string in_userId
                            , string &out_certificate ){

    if ( getSoap() == NULL ){
        MWLOG( LEV_ERROR, MOD_CMD, "%s - Null sp", __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( getSoap() == NULL ) */

    if ( in_hash.empty() ){
        MWLOG( LEV_ERROR, MOD_CMD, "%s - Empty hash", __FUNCTION__ );
        return SOAP_NO_DATA;
    }/* if ( in_hash.empty() ) */

    if ( in_pin.empty() ){
        MWLOG( LEV_ERROR, MOD_CMD, "%s - Empty pin", __FUNCTION__ );
        return SOAP_NO_DATA;
    }/* if ( in_pin.empty() ) */

    if ( in_userId.empty() ){
        MWLOG( LEV_ERROR, MOD_CMD, "%s - Empty userId", __FUNCTION__ );
        return SOAP_NO_DATA;
    }/* if ( in_userId.empty() ) */

    /*
        ProcessID initialization
    */
    setProcessID( "" );

    /*
        Get CCMovelSign request
    */
    string userId = in_userId;
    _ns2__CCMovelSign *send = get_CCMovelSignRequest( in_hash, in_pin
                                                    , &userId );
    if ( send == NULL ){
        MWLOG( LEV_ERROR, MOD_CMD, "NULL send parameters" );
        return SOAP_NULL;
    }/* if ( send == NULL ) */

    /*
        Call CCMovelSign service
    */
    _ns2__CCMovelSignResponse response;
    int ret = getProxy().CCMovelSign( getEndPoint(), "", send, response );

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
            m_fault = getProxy().soap_fault();
            MWLOG( LEV_ERROR
                    , MOD_CMD
                    , "Error in CCMovelSign() - SOAP Fault! %s"
                    , m_fault->faultstring );
        } else{
            MWLOG( LEV_ERROR, MOD_CMD
                , "Error in CCMovelSign(): Error code: %d", ret );
        }/* if ( ret == SOAP_FAULT ) */
        return ret;
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

    out_certificate = *response.CCMovelSignResult->X509Certificate;

    return SOAP_OK;
}/* CMD_client::CCMovelSign(() */

/*  *********************************************************
    ***          CMD_client::checkValidateOtpResponse()   ***
    ********************************************************* */
int CMD_client::checkValidateOtpResponse( _ns2__ValidateOtpResponse *response ){
    if ( response == NULL ){
        MWLOG( LEV_ERROR, MOD_CMD, "%s - Null response", __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response == NULL ) */

    if ( response->ValidateOtpResult == NULL ){
        MWLOG( LEV_ERROR, MOD_CMD
                , "%s - Null ValidateOtpResult"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult == NULL ) */

    if ( response->ValidateOtpResult->Signature == NULL ){
        MWLOG( LEV_ERROR, MOD_CMD
                , "%s - Null Signature"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult->Signature == NULL ) */

    if ( response->ValidateOtpResult->Status == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Null Status"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult->Status == NULL ) */

    if ( response->ValidateOtpResult->Status->Message == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Null Status Message"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult->Status->Message == NULL ) */

    if ( response->ValidateOtpResult->Signature == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - NULL Signature"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult->Signature == NULL ) */

    if ( response->ValidateOtpResult->Signature->__ptr == NULL ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - NULL Signature pointer"
                , __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( response->ValidateOtpResult->Signature->__ptr == NULL ) */

    if ( response->ValidateOtpResult->Signature->__size < 1 ){
        MWLOG( LEV_ERROR
                , MOD_CMD
                , "%s - Invalide Signature pointer size: %d"
                , __FUNCTION__
                , response->ValidateOtpResult->Signature->__size );
        return SOAP_LENGTH;
    }/* if ( response->ValidateOtpResult->Signature->__ptr == NULL ) */

    return SOAP_OK;
}/* CMD_client::checkValidateOtpResponse(() */

/*  *********************************************************
    ***          CMD_client::ValidateOtp()                ***
    ********************************************************* */
int CMD_client::ValidateOtp( string in_code
                            , unsigned char **out_Signature
                            , unsigned int *out_SignatureLen ){

    if ( getSoap() == NULL ){
        MWLOG( LEV_ERROR, MOD_CMD, "Null sp" );
        return SOAP_CLI_FAULT;
    }/* if ( getSoap() == NULL ) */

    if ( in_code.empty() ){
        MWLOG( LEV_ERROR, MOD_CMD, "%s - Empty in_code", __FUNCTION__ );
        return SOAP_NO_DATA;
    }/* if ( in_code.empty() ) */

    string code = in_code;
    string processId = getProcessID();
    /*
        Get ValidateOtp request
    */
    _ns2__ValidateOtp *send = get_ValidateOtpRequest( &code, &processId );
    if ( send == NULL ){
        MWLOG( LEV_ERROR, MOD_CMD, "%s - NULL send parameters", __FUNCTION__ );
        return SOAP_NULL;
    }/* if ( send == NULL ) */

    /*
        Call ValidateOtp service
    */
    _ns2__ValidateOtpResponse response;
    int ret = getProxy().ValidateOtp( getEndPoint(), "", send, response );
    if ( ret != SOAP_OK ){
        if ( ret == SOAP_FAULT ){
            m_fault = getProxy().soap_fault();
            MWLOG( LEV_ERROR, MOD_CMD
                    , "ValidateOtp(): SOAP Fault! %s"
                    , m_fault->faultstring );
        } else{
            MWLOG( LEV_ERROR, MOD_CMD
                , "ValidateOtp(): Error code: %d", ret );
        }/* if ( ret == SOAP_FAULT ) */
        return ret;
    }/* if ( ret != SOAP_OK ) */

    /*
        Validate response
    */
    ret = checkValidateOtpResponse( &response ) ;
    if ( ret != SOAP_OK ) return ret;

    /*
        Set signature
    */
    if ( ( out_Signature != NULL ) && ( out_SignatureLen != NULL ) ){
        *out_Signature = new unsigned char[ response.ValidateOtpResult->Signature->__size ];
        memcpy( *out_Signature
                , response.ValidateOtpResult->Signature->__ptr
                , response.ValidateOtpResult->Signature->__size );

        *out_SignatureLen = response.ValidateOtpResult->Signature->__size;
    }/* if ( ( out_Signature != NULL ) && ( out_SignatureLen != NULL ) ) */

    cout << "Signature Ok" << endl;

    return SOAP_OK;
}/* CMD_client::ValidateOtp() */

}/* namespace */
