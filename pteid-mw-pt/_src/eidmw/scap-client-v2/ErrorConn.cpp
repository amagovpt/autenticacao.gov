#include "ErrorConn.h"
#include "ASService/soapH.h"

void ErrorConn::setErrConnInfo() {
    int index = 0;
    errConnInfo = new ErrConnInfo[MAX_NUM_CONN_ERR];

    // First Error connection
    errConnInfo[index++] = ErrConnInfo( SOAP_OK                 , ER_NONE                   , EMPTY_STR                                                                                             );

    errConnInfo[index++] = ErrConnInfo( SOAP_EOF                , ER_EOF                    , "Unexpected end of file, no input, or timeout receiving data"                                         );
    errConnInfo[index++] = ErrConnInfo( SOAP_TIMEOUT            , ER_TIMEOUT                , "Service unavailable. Please try again later"                                                         );
    errConnInfo[index++] = ErrConnInfo( SOAP_SSL_ERROR          , ER_SSL                    , "An SSL error occured"                                                                                );
    errConnInfo[index++] = ErrConnInfo( SOAP_DATAENCODINGUNKNOWN, ER_DATA_ENCODING_UNKNOWN  , "A data encoding unknown occured"                                                                     );
    errConnInfo[index++] = ErrConnInfo( SOAP_VERSIONMISMATCH    , ER_VERSION_MISMATCH       , "SOAP version mismatch or no SOAP message. Please verify sent message"                                );
    errConnInfo[index++] = ErrConnInfo( SOAP_MUSTUNDERSTAND     , ER_MUST_UNDERSTAND        , "A message element was not understood. Please verify sent message"                                    );
    errConnInfo[index++] = ErrConnInfo( SOAP_CLI_FAULT          , ER_CLIENT                 , "The message was incorrectly formed or contained incorrect information. Please verify sent message"   );
    errConnInfo[index++] = ErrConnInfo( SOAP_SVR_FAULT          , ER_SERVER                 , "There is a problem with the server. The message could not proceed"                                   );
    errConnInfo[index++] = ErrConnInfo( SOAP_TAG_MISMATCH       , ER_TAG_MISMATCH           , "An XML element didn't correspond to anything expected"                                               );
    errConnInfo[index++] = ErrConnInfo( SOAP_TYPE               , ER_TYPE                   , "An XML Schema type mismatch"                                                                         );
    errConnInfo[index++] = ErrConnInfo( SOAP_SYNTAX_ERROR       , ER_SYNTAX_ERROR           , "An XML syntax error occurred on the input"                                                           );
    errConnInfo[index++] = ErrConnInfo( SOAP_NO_TAG             , ER_NO_TAG                 , "Begin of an element expected, but not found"                                                         );
    errConnInfo[index++] = ErrConnInfo( SOAP_IOB                , ER_IOB                    , "Array index out of bounds"                                                                           );
    errConnInfo[index++] = ErrConnInfo( SOAP_NAMESPACE          , ER_SOAP_NAMESPACE         , "Namespace name mismatch (validation error)"                                                          );
    errConnInfo[index++] = ErrConnInfo( SOAP_FATAL_ERROR        , ER_FATAL_ERROR            , "Internal error"                                                                                      );
    errConnInfo[index++] = ErrConnInfo( SOAP_USER_ERROR         , ER_USER_ERROR             , "User error"                                                                                          );
    errConnInfo[index++] = ErrConnInfo( SOAP_FAULT              , ER_FAULT                  , "An exception raised by the service"                                                                  );
    errConnInfo[index++] = ErrConnInfo( SOAP_NO_METHOD          , ER_NO_METHOD              , "The dispatcher did not find a matching operation for a request"                                      );
    errConnInfo[index++] = ErrConnInfo( SOAP_NO_DATA            , ER_NO_DATA                , "No data in HTTP message"                                                                             );
    errConnInfo[index++] = ErrConnInfo( SOAP_GET_METHOD         , ER_GET_METHOD             , "HTTP GET operation not handled"                                                                      );
    errConnInfo[index++] = ErrConnInfo( SOAP_EOM                , ER_EOM                    , "Out of memory"                                                                                       );
    errConnInfo[index++] = ErrConnInfo( SOAP_MOE                , ER_MOE                    , "Memory overflow/corruption error (DEBUG mode)"                                                       );
    errConnInfo[index++] = ErrConnInfo( SOAP_NULL               , ER_NULL                   , "An element was null, while it is not supposed to be null"                                            );
    errConnInfo[index++] = ErrConnInfo( SOAP_DUPLICATE_ID       , ER_DUPLICATE_ID           , "Element's ID duplicated (multi-ref encoding)"                                                        );
    errConnInfo[index++] = ErrConnInfo( SOAP_MISSING_ID         , ER_MISSING_ID             , "Element ID missing for an href/ref (multi-ref encoding)"                                             );
    errConnInfo[index++] = ErrConnInfo( SOAP_HREF               , ER_HREF                   , "Reference to object is incompatible with the object refered to"                                      );
    errConnInfo[index++] = ErrConnInfo( SOAP_UTF_ERROR          , ER_UTF_ERROR              , "An UTF-encoded message decoding error occured"                                                       );
    errConnInfo[index++] = ErrConnInfo( SOAP_UDP_ERROR          , ER_UDP_ERROR              , "Message too large to store in UDP packet"                                                            );
    errConnInfo[index++] = ErrConnInfo( SOAP_TCP_ERROR          , ER_TCP_ERROR              , "A connection error occured. Please make sure you are connected to the Internet"                      );
    errConnInfo[index++] = ErrConnInfo( SOAP_HTTP_ERROR         , ER_HTTP_ERROR             , "An HTTP error occured"                                                                               );
    errConnInfo[index++] = ErrConnInfo( SOAP_NTLM_ERROR         , ER_NTLM_ERROR             , "An NTLM authentication handshake error occured"                                                      );
    errConnInfo[index++] = ErrConnInfo( SOAP_ZLIB_ERROR         , ER_ZLIB_ERROR             , "A Zlib error occured"                                                                                );
    errConnInfo[index++] = ErrConnInfo( SOAP_PLUGIN_ERROR       , ER_PLUGIN_ERROR           , "Failed to register plugin"                                                                           );
    errConnInfo[index++] = ErrConnInfo( SOAP_MIME_ERROR         , ER_MIME_ERROR             , "MIME parsing error"                                                                                  );
    errConnInfo[index++] = ErrConnInfo( SOAP_MIME_HREF          , ER_MIME_HREF              , "MIME attachment has no href from SOAP body error"                                                    );
    errConnInfo[index++] = ErrConnInfo( SOAP_MIME_END           , ER_MIME_END               , "End of MIME attachments protocol error"                                                              );
    errConnInfo[index++] = ErrConnInfo( SOAP_DIME_ERROR         , ER_DIME_ERROR             , "DIME formatting error or DIME size exceeds SOAP_MAXDIMESIZE"                                         );
    errConnInfo[index++] = ErrConnInfo( SOAP_DIME_END           , ER_DIME_END               , "End of DIME attachments protocol error"                                                              );
    errConnInfo[index++] = ErrConnInfo( SOAP_DIME_HREF          , ER_DIME_HREF              , "DIME attachment has no href from SOAP body"                                                          );
    errConnInfo[index++] = ErrConnInfo( SOAP_DIME_MISMATCH      , ER_DIME_MISMATCH          , "DIME version/transmission error"                                                                     );
    errConnInfo[index++] = ErrConnInfo( SOAP_REQUIRED           , ER_REQUIRED               , "Attributed required validation error"                                                                );
    errConnInfo[index++] = ErrConnInfo( SOAP_PROHIBITED         , ER_PROHIBITED             , "Attributed prohibited validation error"                                                              );
    errConnInfo[index++] = ErrConnInfo( SOAP_OCCURS             , ER_OCCURS                 , "Element minOccurs/maxOccurs validation error or SOAP_MAXOCCURS exceeded"                             );
    errConnInfo[index++] = ErrConnInfo( SOAP_LENGTH             , ER_LENGTH                 , "Element length validation error or SOAP_MAXLENGTH exceeded"                                          );
    errConnInfo[index++] = ErrConnInfo( SOAP_FD_EXCEEDED        , ER_FD_EXCEEDED            , "Too many open sockets (for non-win32 systems not supporting poll())"                                 );

    // Last Error connection
    errConnInfo[index++] = ErrConnInfo( SOAP_UNKNOWN            , ER_UNKNOWN                , "Unknown error! Please verify your network connection"                                                );

    // Set ErrConnInfo size
    setErrConnInfoSize( index );

    /*printf( "Init errConnInfo table\n" );
    for(int i = 0; i < index; i++ ){
        printf( "soapErr: %d, connErr: %d, description: %s\n"
                , errConnInfo[i].soapErr
                , errConnInfo[i].connErr
                , errConnInfo[i].description.c_str() );
    }*/
}

ErrorConn::ErrorConn(){
    m_connErr = ER_UNKNOWN;
    m_connErrStr = EMPTY_STR;

    setErrConnInfo();

    // No selected error yet
    curErrConnInfo = NULL;
}

ErrorConn::~ErrorConn(){
    delete[] errConnInfo;
}

void ErrorConn::setErrConnInfoSize( int Size ) {
    errConnInfoSize = Size;
}

int ErrorConn::getErrConnInfoSize(){
    printf( "getErrConnInfoSize() - errConnInfoSize: %d\n", errConnInfoSize );

    return errConnInfoSize;
}

tErrConnInfo* ErrorConn::getEntry( int soapErr ) {

    tErrConnInfo* p, *cur = NULL;
    int infoSize = getErrConnInfoSize();
    int index;
    for( index = 0, p = errConnInfo ; ( p != NULL ) && ( index < infoSize )
        ; index++, p++ ) {

        if ( soapErr == p->soapErr ) {
            cur = p;
            break;
        }
    }

    if ( NULL == cur ) {
        if ( p != NULL ) {
            //Assign to the last element of the errConnInfo array
            p--;
            cur = p;
        }
    }

    return cur;
}

void ErrorConn::setErr( int soapErr, void *in_suppliers_resp ) {

    ns2__AttributeSupplierResponseType *suppliers_resp =
        (ns2__AttributeSupplierResponseType*)in_suppliers_resp;

    if ( ( SOAP_EOF == soapErr )
        && ( suppliers_resp->soap != NULL )
        && ( 0 == suppliers_resp->soap->errnum ) ){
        soapErr = SOAP_TIMEOUT;
    }

    printf( "setErr() - soapErr: %d\n", soapErr );
    curErrConnInfo = getEntry( soapErr );
}

int ErrorConn::getErr(){
    m_connErr = ( curErrConnInfo != NULL ) ? curErrConnInfo->connErr : ER_UNKNOWN;

    //printf( "getErr() - m_connErr: %d\n", ((int)m_connErr) );

    return ((int)m_connErr);
}

std::string ErrorConn::getErrStr() {

    if ( curErrConnInfo != NULL ){
        m_connErrStr =  curErrConnInfo->description;
    } else {
        curErrConnInfo = getEntry( SOAP_UNKNOWN );
        if ( curErrConnInfo != NULL ) {
            m_connErrStr = curErrConnInfo->description;
        } else {
            printf( "getErrStr() - NULL curErrConnInfo\n" );
            m_connErrStr = EMPTY_STR;
        }
    }

    printf( "getErrStr() - m_connErrStr: %s\n", m_connErrStr.c_str() );
    return m_connErrStr;
}
