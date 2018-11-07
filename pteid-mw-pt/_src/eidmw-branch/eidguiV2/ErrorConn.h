#ifndef ERRORCONNECTION_H_
#define ERRORCONNECTION_H_

#include <string>

enum eConnectionError{
      ER_NONE = 0
    , ER_EOF
    , ER_TIMEOUT
    , ER_SSL
    , ER_DATA_ENCODING_UNKNOWN
    , ER_VERSION_MISMATCH
    , ER_MUST_UNDERSTAND
    , ER_CLIENT
    , ER_SERVER
    , ER_TAG_MISMATCH
    , ER_TYPE
    , ER_SYNTAX_ERROR
    , ER_NO_TAG
    , ER_IOB
    , ER_SOAP_NAMESPACE
    , ER_FATAL_ERROR
    , ER_USER_ERROR
    , ER_FAULT
    , ER_NO_METHOD
    , ER_NO_DATA
    , ER_GET_METHOD
    , ER_EOM
    , ER_MOE
    , ER_NULL
    , ER_DUPLICATE_ID
    , ER_MISSING_ID
    , ER_HREF
    , ER_UTF_ERROR
    , ER_UDP_ERROR
    , ER_TCP_ERROR
    , ER_HTTP_ERROR
    , ER_NTLM_ERROR
    , ER_ZLIB_ERROR
    , ER_PLUGIN_ERROR
    , ER_MIME_ERROR
    , ER_MIME_HREF
    , ER_MIME_END
    , ER_DIME_ERROR
    , ER_DIME_END
    , ER_DIME_HREF
    , ER_DIME_MISMATCH
    , ER_REQUIRED
    , ER_PROHIBITED
    , ER_OCCURS
    , ER_LENGTH
    , ER_FD_EXCEEDED
    , ER_UNKNOWN
};

struct ErrConnInfo{
    int              soapErr;
    eConnectionError connErr;
    std::string      description;

    public:
        ErrConnInfo(){};
        ErrConnInfo( int in_soapErr
                    , eConnectionError in_connErr
                    , std::string in_description) :
            soapErr( in_soapErr ), connErr( in_connErr ), description( in_description ){};
        ~ErrConnInfo(){};
};
typedef struct ErrConnInfo tErrConnInfo;

class ErrorConn
{
public:
    #define MAX_NUM_CONN_ERR    ( ER_UNKNOWN - ER_NONE + 1 )
    #define SOAP_UNKNOWN        -100
    #define SOAP_TIMEOUT        -101

    const std::string EMPTY_STR = "";

    public:
        ErrorConn();
        ~ErrorConn();
        void setErrConnInfo();
        void setErr( int soapConnErr, void *in_suppliers_resp );
        std::string getErrStr();
        int getErr();
        int getErrConnInfoSize();
        void setErrConnInfoSize( int Size );

    private:
        eConnectionError m_connErr;
        std::string m_connErrStr;

        ErrConnInfo *errConnInfo;
        int errConnInfoSize;
        tErrConnInfo* curErrConnInfo;
        tErrConnInfo* getEntry( int soapErr );
};

#endif // ERRORCONNECTION_H_
