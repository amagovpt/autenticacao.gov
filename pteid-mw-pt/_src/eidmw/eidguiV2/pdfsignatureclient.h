/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

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

int handleError(int status_code, soap *sp, const char *call);

class PDFSignatureInfo{
public:
    PDFSignatureInfo(int _selectedPage, double _x, double _y, bool isLtv, const char *_location, const char *_reason,
            unsigned int _seal_width, unsigned int _seal_height) {
        selectedPage = _selectedPage;
        x = _x;
        y = _y;
        location = _location;
        reason = _reason;
		is_ltv = isLtv;
        seal_width = _seal_width;
        seal_height = _seal_height;
    }

    int getSelectedPage() { return selectedPage; }
    double getX() { return x; }
    double getY() { return y; }
    unsigned int getSealWidth() { return seal_width; }
    unsigned int getSealHeight() { return seal_height; }
	bool isLtv() { return is_ltv; }
    const char * getLocation() { return location; }
    const char * getReason() { return reason; }
    void setReason(const char * _reason) { reason = _reason; }
    void setLocation(const char * _location) { location = _location; }

private:
    int selectedPage;
    double x;
    double y;
    unsigned int seal_width;
    unsigned int seal_height;
	bool is_ltv;
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
    int signPDF(ProxyInfo, QString, QString, QString, QString, bool, bool,
                PDFSignatureInfo, std::vector<ns3__AttributeType *> &,
                bool, QByteArray &m_jpeg_scaled_data);

private:

    QByteArray openSCAPSignature(const char *inputFile, const char *outputPath,
                            std::string certChain, QString citizenName, QString citizenId,
                            QString attributeSupplier, QString attribute,
                            PDFSignatureInfo signatureInfo, bool isVisible, bool isTimestamp, bool isCC,
                            bool useCustomImage, QByteArray &m_jpeg_scaled_data);

    unsigned char * callSCAPSignatureService(soap* sp, QByteArray signatureHash,
                            ns1__TransactionType *transaction, unsigned int &signatureLen,
                            QString citizenId, int &error);

    int closeSCAPSignature(unsigned char * scap_signature, unsigned int len);
    
    eIDMW::PTEID_PDFSignature * local_pdf_handler;
    QString current_output_file;

	bool is_last_signature;
    std::string * processId;
    //This key is used as input for the TOTP generation
    std::string m_secretKey;
    QString m_appID;
};

#endif // PDFSIGNATURECLIENT_H
