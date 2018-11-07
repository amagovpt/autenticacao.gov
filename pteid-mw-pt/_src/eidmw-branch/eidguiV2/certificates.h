#ifndef CERTIFICATES_H
#define CERTIFICATES_H

#include <QObject>
#include <QDebug>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Wincrypt.h>
#include <Cryptuiapi.h>
#else
#include "pteidversions.h"
typedef const void *PCX509CERT;
#define PCCERT_CONTEXT PCX509CERT
#endif

//MW libraries
#include "eidlib.h"
#include "eidlibException.h"

using namespace eIDMW;
/*
    CERTIFICATES - Certificates Interface
*/
typedef QMap<QString,QVector<PCCERT_CONTEXT> >	tCertPerReader;

class CERTIFICATES
{
public:
    CERTIFICATES();
    static bool ImportCertificates( const char* readerName );
    static bool ImportCertificates( QString const& readerName );
    static bool RemoveCertificates( const char* readerName );
    static bool RemoveCertificates( QString const& readerName );
    static tCertPerReader			m_certContexts;			//!< certificate contexts of each reader

private:
    static bool StoreUserCerts (PTEID_EIDCard& Card, PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, PTEID_Certificate& cert, const char* readerName);
    static bool StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, const char* readerName);
#ifdef WIN32
    static bool ProviderNameCorrect (PCCERT_CONTEXT pCertContext );
#endif
    static void forgetCertificates( QString const& readerName );
};
#endif // CERTIFICATES_H
