#ifndef PDFSIGNATURECLIENT_H
#define PDFSIGNATURECLIENT_H

#include <QString>
#include <QTreeWidget>
#include <string>

#include "eidlib.h"
#include "cstdio"

class SelectedTreeInfo;
class soap;
class ProxyInfo;

class ns2__AttributesType;
class ns3__AttributeType;

//PDFSignature version of Attribute type
class ns4__AttributeType;
class ns1__MainAttributeType;
class ns1__AttributeSupplierType;
class ns1__TransactionType;

class PDFSignatureInfo{
public:
    PDFSignatureInfo(int _selectedPage, double _x, double _y, bool _isPortrait, const char *_location, const char *_reason){
        selectedPage = _selectedPage;
        x = _x;
        y = _y;
        portrait = _isPortrait;
        location = _location;
        reason = _reason;
    }

    int getSelectedPage() { return selectedPage; }
    double getX() { return x; }
    double getY() { return y; }
    bool isPortrait() { return portrait; }
    const char * getLocation(){ return location; }
    const char * getReason(){ return reason; }
    void setReason(const char * _reason){reason = _reason;}
    void setLocation(const char * _location){location = _location;}

private:
    int selectedPage;
    double x;
    double y;
    bool portrait;
    const char *location;
    const char *reason;
};

class SignatureDetails {
public: 
    QByteArray signature;
    QByteArray document_hash;
    QByteArray signing_certificate;
};

class PDFSignatureClient
{

public:

    PDFSignatureClient();
    int signPDF(ProxyInfo, QString, QString, QString, QString, int, bool,
                PDFSignatureInfo, std::vector<ns3__AttributeType *> &);

private:

    QByteArray openSCAPSignature(const char *inputFile, const char *outputPath,
                            std::string certChain, QString citizenName, QString citizenId,
                            QString attributeSupplier, QString attribute,
                            PDFSignatureInfo signatureInfo, bool isVisible, bool isCC);

    unsigned char * callSCAPSignatureService(soap* sp, QByteArray signatureHash,
                            ns1__TransactionType *transaction, unsigned int &signatureLen, QString citizenId);

    int closeSCAPSignature(unsigned char * scap_signature, unsigned int len);
    
    eIDMW::PTEID_PDFSignature * local_pdf_handler;
    QString current_output_file;

    std::string * processId;
    //This key is used as input for the TOTP generation
    std::string m_secretKey;
    QString m_appID;
};

#endif // PDFSIGNATURECLIENT_H
