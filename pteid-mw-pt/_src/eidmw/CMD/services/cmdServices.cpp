//STD Library
#include <iostream>
#include <string>

#include <QUuid>
#include <QByteArray>

#include "cmdServices.h"
#include "cmdErrors.h"
#include "BasicHttpBinding_USCORECCMovelSignature.nsmap"
#include "soapBasicHttpBinding_USCORECCMovelSignatureProxy.h"

#include "Util.h"
#include "Config.h"
#include "MiscUtil.h"

#define CC_MOVEL_SERVICE_GET_CERTIFICATE    ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/GetCertificate" )
#define CC_MOVEL_SERVICE_SIGN               ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/CCMovelSign" )
#define CC_MOVEL_SERVICE_VALIDATE_OTP       ( (char *)"http://Ama.Authentication.Service/CCMovelSignature/ValidateOtp" )
#define ENDPOINT_CC_MOVEL_SIGNATURE         ( (const char *)"/Ama.Authentication.Frontend/CCMovelDigitalSignature.svc" )

#define STR_EMPTY                           ""
#define SOAP_MAX_RETRIES                    3

#define SOAP_RECV_TIMEOUT_DEFAULT           20
#define SOAP_SEND_TIMEOUT_DEFAULT           20
#define SOAP_CONNECT_TIMEOUT_DEFAULT        20
#define SOAP_MUST_NO_UNDERSTAND             0
#define SOAP_MUST_UNDERSTAND                1

static char logBuf[512];

namespace eIDMW {

/*  *********************************************************
    ***    CMDServices::CMDServices()       ***
    ********************************************************* */
xsd__base64Binary *encode_base64( soap *sp, std::string in_str ) {

    if ( sp == NULL ) {
        MWLOG_ERR( logBuf, "Null soap" );
        return NULL;
    }

    xsd__base64Binary *encoded = NULL;
    if (in_str.empty()) {
        MWLOG_ERR( logBuf, "Empty in_str" );
        return NULL;
    }

    int len;
    char *c_ptr = getCPtr( in_str, &len );
    encoded = soap_new_set_xsd__base64Binary( sp
                                            , (unsigned char *)c_ptr, len
                                            , NULL, NULL, NULL);

    if ( encoded == NULL ){
        /* this is the same pointer as encoded->__ptr */
        if ( c_ptr != NULL ) free( c_ptr );
    }

    return encoded;
}

class CMDSignatureGsoapProxy: public BasicHttpBinding_USCORECCMovelSignatureProxy {

    public:

        CMDSignatureGsoapProxy(struct soap *sp, CMDProxyInfo p): BasicHttpBinding_USCORECCMovelSignatureProxy(sp) {
            if (p.host.size() > 0)  {

                sp->proxy_host = strdup(p.host.c_str());
                sp->proxy_port = p.port;
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "CMDSignature", "Using proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);

                if (p.user.size() > 0)
                {
                    sp->proxy_userid = strdup(p.user.c_str());
                    sp->proxy_passwd = strdup(p.pwd.c_str());
                }

            }
        }
};

/*  *********************************************************
    ***    handleError()                                  ***
    ********************************************************* */
int handleError( BasicHttpBinding_USCORECCMovelSignatureProxy proxy, int ret ){
    if ( ret != SOAP_OK ) {
        if ( ( proxy.soap_fault() != NULL )
            && ( proxy.soap_fault()->faultstring)) {
            MWLOG_ERR(logBuf, "SOAP Fault! %s", proxy.soap_fault()->faultstring);
        }
        else {
            MWLOG_ERR(logBuf, "Unknown SOAP Fault! - ret: %d", ret );
        }

        return ret;
    }

    MWLOG_INFO( logBuf, "No Error" );
    return SOAP_OK;
}

/*  *******************************************************************************************************************
    ****
    **** CMDServices class - general methods
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::CMDServices()                     ***
    ********************************************************* */
CMDServices::CMDServices() {
    if ( !init( SOAP_RECV_TIMEOUT_DEFAULT, SOAP_SEND_TIMEOUT_DEFAULT, SOAP_CONNECT_TIMEOUT_DEFAULT,
              SOAP_MUST_NO_UNDERSTAND) )
        return;

    const char *new_endpoint = NULL;
    std::string cmd_host = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_HOST));
    std::string cmd_endpoint = "https://" + cmd_host + ENDPOINT_CC_MOVEL_SIGNATURE;
      
    new_endpoint = cmd_endpoint.c_str();

    MWLOG_DEBUG(logBuf, "Using Endpoint: %s", new_endpoint);

    setEndPoint(strdup(new_endpoint));
}

/*  *********************************************************
    ***    CMDServices::~CMDServices()                    ***
    ********************************************************* */
CMDServices::~CMDServices() {
    soap *sp = getSoap();
    if ( NULL == sp ) return;

    soap_destroy( sp );
    soap_end( sp );

    setSoap( NULL );
}

/*  *********************************************************
    ***    CMDServices::init()                            ***
    ********************************************************* */
bool CMDServices::init(int recv_timeout, int send_timeout,
                        int connect_timeout, short mustUnderstand) {
    soap *sp = soap_new2( SOAP_C_UTFSTRING, SOAP_C_UTFSTRING );
    if ( sp == NULL ) {
        MWLOG_ERR( logBuf, "Null soap" );
        return false;
    }

    setUserId( STR_EMPTY );

    //Don't change this: it serves as authentication for the service
    setApplicationID(std::string("b826359c-06f8-425e-8ec3-50a97a418916"));

    //Define appropriate network timeouts
    sp->recv_timeout = recv_timeout;
    sp->send_timeout = send_timeout;
    sp->connect_timeout = connect_timeout;

    //Dont output mustUnderstand attributes
    sp->mustUnderstand = mustUnderstand;

    char * ca_path = NULL;
    std::string cacerts_file;

#ifdef __linux__
    ca_path = "/etc/ssl/certs"; 
    //Load CA certificates from file provided with pteid-mw
#elif WIN32
    cacerts_file = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALLDIR))+"/cacerts.pem";
    //TODO
#elif __APPLE__
    cacerts_file = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR))+"/cacerts.pem";
#endif

    int ret = soap_ssl_client_context(sp, SOAP_SSL_DEFAULT,
            NULL,    
            NULL,
            cacerts_file.size() > 0 ? cacerts_file.c_str(): NULL, /* cacert file to store trusted certificates (needed to verify server) */ 
            ca_path,
            NULL);

    if ( ret != SOAP_OK ) {
        soap_destroy( sp );
        setSoap( NULL );

        MWLOG_ERR( logBuf, "soap_ssl_client_context() failed - code: %d", ret );
        return false;
    }

    setSoap( sp );

    return true;
}

/*  *********************************************************
    ***    CMDServices::getSoap()                         ***
    ********************************************************* */
soap *CMDServices::getSoap(){
    return m_soap;
}

/*  *********************************************************
    ***    CMDServices::setSoap()                         ***
    ********************************************************* */
void CMDServices::setSoap( soap *in_soap ){
    m_soap = in_soap;
}

/*  *********************************************************
    ***    CMDServices::setEndPoint()                     ***
    ********************************************************* */
void CMDServices::setEndPoint( const char *endpoint ){
    m_endpoint = endpoint;
}

/*  *********************************************************
    ***    CMDServices::getEndPoint()                     ***
    ********************************************************* */
const char *CMDServices::getEndPoint(){
    return m_endpoint;
}

/*  *********************************************************
    ***    CMDServices::getProcessID()                    ***
    ********************************************************* */
std::string CMDServices::getProcessID(){
    return m_processID;
}

/*  *********************************************************
    ***    CMDServices::setProcessID()                    ***
    ********************************************************* */
void CMDServices::setProcessID( std::string processID ){
    m_processID = processID;
}

/*  *********************************************************
    ***    CMDServices::getApplicationID()                ***
    ********************************************************* */
std::string CMDServices::getApplicationID(){
    return m_applicationID;
}

/*  *********************************************************
    ***    CMDServices::setApplicationID()                ***
    ********************************************************* */
void CMDServices::setApplicationID( std::string applicationID ){
    m_applicationID = applicationID;
}

/*  *********************************************************
    ***    CMDServices::getUserId()                       ***
    ********************************************************* */
std::string CMDServices::getUserId(){
    return m_userId;
}

/*  *********************************************************
    ***    CMDServices::setUserId()                       ***
    ********************************************************* */
void CMDServices::setUserId( std::string in_userId ){
    m_userId = in_userId;
}


/*  *******************************************************************************************************************
    ****
    **** GetCertificate
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::get_GetCertificateRequest()       ***
    ********************************************************* */
_ns2__GetCertificate *CMDServices::get_GetCertificateRequest(soap *sp
                                                , char *endpoint
                                                , std::string in_applicationID
                                                , std::string *in_userId) {

    //SOAP_ENV__Header *soapHeader = soap_new_SOAP_ENV__Header( sp );
    //soapHeader->wsa__To = endpoint;

    //soapHeader->wsa__Action = CC_MOVEL_SERVICE_GET_CERTIFICATE;

    //Set the created header in our soap structure
    //sp->header = soapHeader;

    _ns2__GetCertificate *send = soap_new__ns2__GetCertificate( sp );
    if ( NULL == send ) return send;

    send->applicationId = encode_base64( sp, in_applicationID );
    send->userId        = in_userId;

    return send;
}

/*  *********************************************************
    ***    CMDServices::checkGetCertificateResponse()     ***
    ********************************************************* */
int CMDServices::checkGetCertificateResponse(
                                    _ns2__GetCertificateResponse *response ){
    if ( response == NULL ){
        MWLOG_ERR( logBuf, "Null response" );
        return ERR_NULL_HANDLER;
    }

    if ( response->GetCertificateResult == NULL ){
        MWLOG_ERR( logBuf, "Null GetCertificateResult" );
        return ERR_GET_CERTIFICATE;
    }

    return ERR_NONE;
}

/*  *********************************************************
    ***    CMDServices::GetCertificate()                  ***
    ********************************************************* */
int CMDServices::GetCertificate(CMDProxyInfo proxyInfo, std::string in_userId, char **out_certificate,
                                 int *out_certificateLen ) {
    soap *sp = getSoap();
    if ( sp == NULL ) {
        MWLOG_ERR( logBuf, "Null soap" );
        return ERR_NULL_HANDLER;
    }

    if (in_userId.empty()) {
        MWLOG_ERR( logBuf, "Empty userId" );
        return ERR_INV_USERID;
    }

    const char *endPoint = getEndPoint();
    CMDSignatureGsoapProxy proxy(sp, proxyInfo);
                            
    proxy.soap_endpoint = endPoint;

    /*
        Get GetCertificate request
    */
    _ns2__GetCertificate *send = get_GetCertificateRequest( sp
                                                        , (char*)endPoint
                                                        , getApplicationID()
                                                        , &in_userId );
    if ( send == NULL ) {
        MWLOG_ERR( logBuf, "NULL send parameters" );
        return ERR_NULL_HANDLER;
    }

    //QByteArray msg_id = generateMsgID().toUtf8();
    //sp->header->wsa__MessageID = (char *) msg_id.constData();

    /*
        Call GetCertificate service
    */
    _ns2__GetCertificateResponse response;
    int ret;
    ret = proxy.GetCertificate( send, response );

    /* Clean pointers before exit */
    if ( send->applicationId != NULL ) {
        if ( send->applicationId->__ptr != NULL )
            free( send->applicationId->__ptr );
    }

    /* Handling errors */
    if ( handleError( proxy, ret ) != ERR_NONE ) return ret;

    /* Validate response */
    ret = checkGetCertificateResponse( &response );
    if ( ret != ERR_NONE ) return ret;

    /* Process X509Certificate */
    if ( out_certificate != NULL ){
        *out_certificate = getCPtr( *response.GetCertificateResult
                                    , out_certificateLen );
    }

    return ERR_NONE;
}

/*  *******************************************************************************************************************
    ****
    **** CCMovelSign
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::get_CCMovelSignRequest()          ***
    ********************************************************* */
_ns2__CCMovelSign *CMDServices::get_CCMovelSignRequest(soap *sp , char *endpoint, std::string in_applicationID, std::string *docName,
                                              unsigned char *in_hash, std::string *in_pin, std::string *in_userId) {
    //SOAP_ENV__Header *soapHeader = soap_new_SOAP_ENV__Header( sp );
    //soapHeader->wsa__To = endpoint;

    //soapHeader->wsa__Action = CC_MOVEL_SERVICE_SIGN;

    //Set the created header in our soap structure
    //sp->header = soapHeader;

    _ns3__SignRequest *soapBody = soap_new_ns3__SignRequest(sp);

    soapBody->ApplicationId = encode_base64(sp, in_applicationID );
    int hash_len = 51;
    soapBody->Hash          = soap_new_set_xsd__base64Binary(sp, in_hash, hash_len,
                                             NULL, NULL, NULL);  //encode_base64( sp, in_hash );
    soapBody->Pin           = in_pin;
    soapBody->UserId        = in_userId;
	soapBody->DocName = docName;

    _ns2__CCMovelSign *send = soap_new_set__ns2__CCMovelSign( sp, soapBody );
    return send;
}

/*  *********************************************************
    ***    CMDServices::checkCCMovelSignResponse()        ***
    ********************************************************* */
int CMDServices::checkCCMovelSignResponse( _ns2__CCMovelSignResponse *response ){
    if ( response == NULL ){
        MWLOG_ERR( logBuf, "Null response" );
        return ERR_NULL_HANDLER;
    }

    if ( response->CCMovelSignResult == NULL ){
        MWLOG_ERR( logBuf, "Null CCMovelSignResult" );
        return ERR_NULL_DATA;
    }

    return ERR_NONE;
}

/*  *********************************************************
    ***    CMDServices::CCMovelSign()                     ***
    ********************************************************* */
int CMDServices::ccMovelSign(CMDProxyInfo proxyInfo, unsigned char * in_hash, std::string docName, std::string in_pin) {

    soap *sp = getSoap();
    if (sp == NULL) {
        MWLOG_ERR( logBuf, "Null soap" );
        return ERR_NULL_HANDLER;
    }

    if (in_hash == NULL) {
        MWLOG_ERR( logBuf, "NULL hash" );
        return ERR_INV_HASH;
    }

    if (in_pin.empty()) {
        MWLOG_ERR( logBuf, "Empty pin" );
        return ERR_INV_USERPIN;
    }

    std::string in_userId = getUserId();
    if (in_userId.empty()) {
        MWLOG_ERR( logBuf, "Empty userId" );
        return ERR_INV_USERID;
    }

    /*
        ProcessID initialization
    */
    setProcessID( STR_EMPTY );

    const char *endPoint = getEndPoint();
    CMDSignatureGsoapProxy proxy(sp, proxyInfo);
    proxy.soap_endpoint = endPoint;

    /*
        Get CCMovelSign request
    */
    _ns2__CCMovelSign *send = get_CCMovelSignRequest( sp, (char*)endPoint, getApplicationID(), &docName, in_hash, &in_pin, &in_userId );
    if ( send == NULL ){
        MWLOG_ERR( logBuf, "NULL send parameters" );
        return ERR_NULL_HANDLER;
    }

    /*
        Call CCMovelSign service
    */
    _ns2__CCMovelSignResponse response;
    int ret;
    ret = proxy.CCMovelSign( NULL, NULL, send, response );

    /*
    /* Clean pointers before exit
    if ( send->request != NULL ) {
        // Clean pointers before leaving function
        if ( send->request->Pin != NULL ) {
            if ( send->request->Pin->__ptr != NULL )
                free( send->request->Pin->__ptr );
        }

        if ( send->request->ApplicationId != NULL ) {
            if ( send->request->ApplicationId->__ptr != NULL )
                free( send->request->ApplicationId->__ptr );
        }
    }
    */

    /* Handling errors */
    if ( handleError( proxy, ret ) != ERR_NONE ) return ret;

    /* Validate response */
    ret = checkCCMovelSignResponse( &response );
    if ( ret != ERR_NONE ) return ret;

    /* Save ProcessId */
    //std::cerr << "ProcessId: " << *response.CCMovelSignResult->ProcessId << endl;

    setProcessID( *response.CCMovelSignResult->ProcessId );

    return ERR_NONE;
}

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
    //SOAP_ENV__Header *soapHeader = soap_new_SOAP_ENV__Header( sp );
    //soapHeader->wsa__To = endpoint;

    //soapHeader->wsa__Action = CC_MOVEL_SERVICE_VALIDATE_OTP;

    //Set the created header in our soap structure
    //sp->header = soapHeader;

    _ns2__ValidateOtp *send = soap_new__ns2__ValidateOtp( sp );
    if ( send == NULL ) return NULL;

    send->code          = in_code;
    send->processId     = in_processId;
    send->applicationId = encode_base64( sp, in_applicationID );

    return send;
}

/*  *********************************************************
    ***    CMDServices::checkValidateOtpResponse()        ***
    ********************************************************* */
int CMDServices::checkValidateOtpResponse( _ns2__ValidateOtpResponse *response ){
    if ( response == NULL ) {
        MWLOG_ERR( logBuf, "Null response" );
        return ERR_NULL_HANDLER;
    }

    if ( response->ValidateOtpResult == NULL ) {
        MWLOG_ERR( logBuf, "Null ValidateOtpResult" );
        return ERR_NULL_HANDLER;
    }

    if (response->ValidateOtpResult->Status == NULL) {
        MWLOG_ERR( logBuf, "Null Status" );
        return ERR_NULL_HANDLER;
    }

    if (response->ValidateOtpResult->Status->Code == NULL) {
        MWLOG_ERR( logBuf, "Null Status Code" );
        return ERR_NULL_DATA;
    }

    int statusCode = atoi( response->ValidateOtpResult->Status->Code->c_str() );    
    if (statusCode == 0) {
        MWLOG_ERR( logBuf, "Status Code is not a valid code" );
        return ERR_INV_DATA;
    }

    if (IS_SOAP_ERROR(statusCode)) {
        MWLOG_ERR( logBuf, "Error Status Code");
        return statusCode;
    }

    if ( response->ValidateOtpResult->Signature == NULL ) {
        MWLOG_ERR( logBuf, "Null Signature" );
        return ERR_NULL_HANDLER;
    }

    if ( response->ValidateOtpResult->Signature->__ptr == NULL ) {
        MWLOG_ERR( logBuf, "Null Signature pointer" );
        return ERR_NULL_DATA;
    }

    if ( response->ValidateOtpResult->Signature->__size < 1 ) {
        MWLOG_ERR( logBuf, "Invalide Signature pointer size: %d",
                  response->ValidateOtpResult->Signature->__size);
        return ERR_SIZE;
    }

    return ERR_NONE;
}

/*  *********************************************************
    ***    CMDServices::ValidateOtp()                     ***
    ********************************************************* */
int CMDServices::ValidateOtp(CMDProxyInfo proxyInfo, std::string in_code
                            , unsigned char **outSignature
                            , unsigned int *outSignatureLen) {
    soap *sp = getSoap();
    if ( sp == NULL ) {
        MWLOG_ERR( logBuf, "Null soap" );
        return ERR_NULL_HANDLER;
    }

    if ( in_code.empty() ) {
        MWLOG_ERR( logBuf, "Empty code" );
        return ERR_INV_CODE;
    }

    std::string code = in_code;
    std::string processId = getProcessID();
    const char *endPoint = getEndPoint();

    CMDSignatureGsoapProxy proxy( sp, proxyInfo );
    proxy.soap_endpoint = endPoint;

    /*
        Get ValidateOtp request
    */
    _ns2__ValidateOtp *send = get_ValidateOtpRequest(sp, (char*)endPoint,
                                  getApplicationID(), &code, &processId);

    if ( send == NULL ) {
        MWLOG_ERR( logBuf, "Null send parameters" );
        return ERR_NULL_HANDLER;
    }

    /*
        Call ValidateOtp service
    */
    _ns2__ValidateOtpResponse response;
    int ret;
    ret = proxy.ValidateOtp( NULL, NULL, send, response );


    /* Clean pointers before exit */
    if ( send->applicationId != NULL ){
        if ( send->applicationId->__ptr != NULL )
            free( send->applicationId->__ptr );
    }

    /* Handling errors */
    if ( handleError( proxy, ret ) != ERR_NONE ) return ret;

    /* Validate response */
    ret = checkValidateOtpResponse( &response );
    if ( ret != ERR_NONE ) return ret;

    /* Set signature */
    if ( ( outSignature != NULL ) && ( outSignatureLen != NULL ) ) {
        *outSignature =
            (unsigned char*)malloc(
                            response.ValidateOtpResult->Signature->__size );

        if ( outSignature == NULL ) {
            MWLOG_ERR( logBuf, "Malloc fail!" );
            return ERR_NULL_HANDLER;
        }

        memcpy( *outSignature
                , response.ValidateOtpResult->Signature->__ptr
                , response.ValidateOtpResult->Signature->__size );

        *outSignatureLen = response.ValidateOtpResult->Signature->__size;
    }

    return ERR_NONE;
}

/*  *******************************************************************************************************************
    ****
    **** Public methods
    ****
    ******************************************************************************************************************* */

/*  *********************************************************
    ***    CMDServices::getCertificate()                  ***
    ********************************************************* */
int CMDServices::getCertificate(CMDProxyInfo proxyInfo, std::string in_userId,
                                 std::vector<CByteArray> &out_cb)   {
    CByteArray empty_certificate;

    if ( in_userId.empty() ) {
        MWLOG_ERR( logBuf, "Empty userId" );
        return ERR_INV_USERID;
    }

    char *p_certificate = NULL;
    int certificateLen = 0;

    int ret = GetCertificate(proxyInfo, in_userId, &p_certificate, &certificateLen );
    if (ret != ERR_NONE)
        return ret;

    std::vector<std::string> certs = toPEM(p_certificate, certificateLen );
    free(p_certificate);

    for (size_t i=0; i != certs.size(); i++)
    {
        CByteArray ba;
        unsigned char *der = NULL;
        int derLen = PEM_to_DER((char *)certs.at(i).c_str(), &der);

       if ( derLen < 0 ) {
         MWLOG_ERR( logBuf, "PEM -> DER conversion failed - len: %d", derLen );
         return ERR_INV_CERTIFICATE;

       }
       ba.Append( (const unsigned char *)der, (unsigned long)derLen );
       out_cb.push_back(ba);
    }

    
    /* Set variables */
    setUserId( in_userId );

    return ERR_NONE;
}

/*  *********************************************************
    ***    CMDServices::sendDataToSign()                  ***
    ********************************************************* */
/*
int CMDServices::sendDataToSign(unsigned char * in_hash, std::string docName, std::string in_pin ){
    return CCMovelSign( in_hash, docName, in_pin );
}
*/

/*  *********************************************************
    ***    CMDServices::getSignature()                    ***
    ********************************************************* */
int CMDServices::getSignature(CMDProxyInfo proxyInfo, std::string in_code, CByteArray& out_cb ){
    unsigned int signLen = 0;
    unsigned char *sign = NULL;

    int ret = ValidateOtp(proxyInfo, in_code, &sign, &signLen );
    if ( ret != ERR_NONE ){
        MWLOG_ERR( logBuf, "ValidateOtp failed" );
        return ret;
    }

    if ( NULL == sign ){
        MWLOG_ERR( logBuf, "Null signature" );
        return ERR_NULL_HANDLER;
    }

    if ( signLen <= 0 ){
        free( sign );

        MWLOG_ERR( logBuf, "Invalid signature length: %d", signLen );
        return ERR_SIZE;
    }

    out_cb.ClearContents();
    out_cb.Append( (const unsigned char *)sign, (unsigned long)signLen );

    free( sign );

    return ERR_NONE;
}


}
