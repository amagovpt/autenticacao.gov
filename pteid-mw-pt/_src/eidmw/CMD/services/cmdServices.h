/*-****************************************************************************

 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2017-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef CMD_CLIENT_H
#define CMD_CLIENT_H

// STD Library
#include <iostream>
#include <string>
#include <openssl/x509.h>
#include <mutex>
#include "soapH.h"
#include "ByteArray.h"
#include "Log.h"
#include "CMDSignature.h"

#ifdef WIN32
#define _LOG_(level, mod, format, ...) MWLOG(level, mod, "%s: " format, __FUNCTION__, __VA_ARGS__);
#define MWLOG_ERR(format, ...) _LOG_(LEV_ERROR, MOD_CMD, format, __VA_ARGS__)
#define MWLOG_WARN(format, ...) _LOG_(LEV_WARN, MOD_CMD, format, __VA_ARGS__)
#define MWLOG_INFO(format, ...) _LOG_(LEV_INFO, MOD_CMD, format, __VA_ARGS__)
#define MWLOG_DEBUG(format, ...) _LOG_(LEV_DEBUG, MOD_CMD, format, __VA_ARGS__)
#else
#define _LOG_(level, mod, format, ...) MWLOG(level, mod, "%s: " format, __FUNCTION__, ##__VA_ARGS__);
#define MWLOG_ERR(format, ...) _LOG_(LEV_ERROR, MOD_CMD, format, ##__VA_ARGS__)
#define MWLOG_WARN(format, ...) _LOG_(LEV_WARN, MOD_CMD, format, ##__VA_ARGS__)
#define MWLOG_INFO(format, ...) _LOG_(LEV_INFO, MOD_CMD, format, ##__VA_ARGS__)
#define MWLOG_DEBUG(format, ...) _LOG_(LEV_DEBUG, MOD_CMD, format, ##__VA_ARGS__)
#endif

namespace eIDMW {

xsd__base64Binary *encode_base64(soap *sp, std::string in_str);

class CMDServices {
public:
	CMDServices(std::string basicAuthUser, std::string basicAuthPassword, std::string applicationId);
	virtual ~CMDServices();

	void cancelRequest();

	// GetCertificate
	int getCertificate(CMDProxyInfo proxyInfo, std::string in_userId, std::vector<CByteArray> &out_certificate);

	// GetCertificateWithPin
	int askForCertificate(CMDProxyInfo proxyInfo, std::string in_userId, std::string in_pin);

	// CCMovelSign
	int ccMovelSign(CMDProxyInfo proxyInfo, unsigned char *in_hash, std::string docName, std::string in_pin,
					bool IsBiometricValidationEnable = true, std::string certificateNumber = "");

	// CCMovelMultipleSign
	int ccMovelMultipleSign(CMDProxyInfo proxyInfo, std::vector<unsigned char *> in_hashs,
							std::vector<std::string> docNames, std::string in_pin,
							bool IsBiometricValidationEnable = true);

	// ForceSMS
	int forceSMS(CMDProxyInfo proxyInfo, std::string in_userId);

	// ValidateOtp and SignatureDocumentPooling
	// This getSignatures is used for the ValidateOTP
	int getSignatures(CMDProxyInfo proxyInfo, std::string in_code, std::vector<CByteArray *> out_signature,
					  bool isBiometric = false);
	// This getSignatures is used for the SignatureDocumentPooling
	int getSignatures(CMDProxyInfo proxyInfo, std::vector<CByteArray *> out_signature);
	int getCMDCertificate(CMDProxyInfo proxyInfo, std::string in_code, std::vector<CByteArray> &out_certificate,
						  bool isBiometric = false);

	static std::string getEndpoint();

protected:
	soap *getSoap();
	void setSoap(soap *);

	void setEndPoint(const char *endpoint);
	const char *getEndPoint();

	std::string getProcessID();
	void setProcessID(std::string processID);

	std::string getApplicationID();
	void setApplicationID(std::string applicationID);

	std::string getUserId();
	void setUserId(std::string in_userId);

	void enableBasicAuthentication();

private:
	soap *m_soap;
	std::string m_applicationID;
	std::string m_processID;
	std::string m_userId; // this is the phone number.
	std::string m_basicAuthUser;
	std::string m_basicAuthPassword;
	std::string m_endpoint;
	std::mutex m_soap_mutex;

	bool init(int recv_timeout, int send_timeout, int connect_timeout, short mustUnderstand);

	// CCMovelSign
	_ns1__CCMovelSign *get_CCMovelSignRequest(soap *sp, std::string in_applicationID, std::string *docName,
											  unsigned char *in_hash, std::string *in_pin, std::string *in_userId,
											  bool *IsBiometricValidationEnable, std::string *certificateNumber);

	int checkCCMovelSignResponse(_ns1__CCMovelSignResponse *response);

	// CCMovelMultipleSign
	_ns1__CCMovelMultipleSign *
	get_CCMovelMultipleSignRequest(soap *sp, std::string in_applicationID, std::vector<std::string *> docNames,
								   std::vector<unsigned char *> in_hashes, std::vector<std::string *> ids,
								   std::string *in_pin, std::string *in_userId, bool *IsBiometricValidationEnable);

	int checkCCMovelMultipleSignResponse(_ns1__CCMovelMultipleSignResponse *response);

	// ValidateOtp
	int ValidateOtp(CMDProxyInfo proxyInfo, std::string in_code, std::vector<unsigned char *> *outSignature,
					std::vector<unsigned int> *outSignatureLen, bool isBiometric);
	int ValidateOtp(CMDProxyInfo proxyInfo, std::string in_code, std::string *outCertificate, bool isBiometric);

	int sendValidateOtp(CMDProxyInfo proxyInfo, std::string in_code, _ns1__ValidateOtpResponse &response,
						bool isBiometric);

	_ns1__ValidateOtp *get_ValidateOtpRequest(soap *sp, std::string in_applicationID, std::string *in_code,
											  std::string *in_processId, bool *isBiometric);

	int checkValidateOtpResponse(_ns1__ValidateOtpResponse *response);

	// GetCertificate
	_ns1__GetCertificate *get_GetCertificateRequest(soap *sp, std::string in_applicationID, std::string *in_userId);

	int checkGetCertificateResponse(_ns1__GetCertificateResponse *response);
	int GetCertificate(CMDProxyInfo proxyInfo, std::string in_userId, char **out_certificate, int *out_certificateLen);

	// GetCertificateWithPin
	_ns1__GetCertificateWithPin *get_GetCertificateWithPinRequest(soap *sp, std::string in_applicationID,
																  std::string *in_userId, std::string *in_pin);

	int checkGetCertificateWithPinResponse(_ns1__GetCertificateWithPinResponse *response);

	// ForceSMS
	int checkForceSmsResponse(_ns1__ForceSMSResponse *response);

	// SignatureDocumentPooling
	_ns1__SignDocumentPooling *get_SignDocumentPoolingRequest(soap *sp, std::string in_applicationID,
															  std::string *in_processId);

	int signatureDocumentPooling(CMDProxyInfo proxyInfo, std::vector<unsigned char *> *outSignature,
								 std::vector<unsigned int> *outSignatureLen);

	int sendSignatureDocumentPooling(CMDProxyInfo proxyInfo, _ns1__SignDocumentPoolingResponse &response);

	int checkSignatureDocumentPoolingResponse(_ns1__SignDocumentPoolingResponse *response);
};

} // namespace eIDMW
#endif // CMD_CLIENT_H
