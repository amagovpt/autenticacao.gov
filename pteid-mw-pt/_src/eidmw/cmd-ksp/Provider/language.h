/*-****************************************************************************

 * Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef __CMDKSP_LANGUAGE_H__
#define __CMDKSP_LANGUAGE_H__

#include <string>
#include "langUtil.h"

namespace eIDMW {
DEFINE_LANG_STR(MobileNumberDoesNotMatchCert) = {
	L"The inserted mobile number does not match the number associated with the selected certificate.",
	L"O n\u00famero de telem\u00f3vel inserido n\u00e3o corresponde ao n\u00famero associado ao certificado "
	L"selecionado."};
DEFINE_LANG_STR(ConnectionError) = {L"A connection error has occurred.", L"Ocorreu um erro na liga��o"};
DEFINE_LANG_STR(PossibleProxyError) = {L"A connection error has occurred. Check proxy settings.",
									   L"Ocorreu um erro na liga��o. Verifique a configura��o do proxy."};
DEFINE_LANG_STR(InvalidPinOrOtp) = {L"Invalid PIN or security code.", L"PIN ou c�digo de seguran�a inv�lido."};
DEFINE_LANG_STR(OtpValidationFailed) = {
	L"Validation of security code failed. The request was not found or validation time has expired.",
	L"N�o foi poss�vel validar o c�digo de confirma��o. O pedido n�o foi encontrado ou o tempo de valida��o expirou."};
DEFINE_LANG_STR(InvalidCertificate) = {L"Invalid certificate.", L"Certificado inv�lido.", L"", L""};
DEFINE_LANG_STR(VerifyIfDigitalSignatureIsActive) = {L"Verify that the digital signature subscription of the Chave "
													 L"M�vel Digital is active in https://www.autenticacao.gov.pt.",
													 L"Verifique que a subscri��o da assinatura digital da Chave M�vel "
													 L"Digital est� ativada em https://www.autenticacao.gov.pt.",
													 L"", L""};
DEFINE_LANG_STR(RegisterCertificateAgain) = {
	L"Please try to register your CMD certificate in the application again.",
	L"Por favor, tente registar novamente o seu certificado da CMD na aplica��o.", L"", L""};
DEFINE_LANG_STR(NoCMDCredentials) = {L"The Citizen Card Middleware installed does not support CMD. An official "
									 L"installer can be obtained at https://www.autenticacao.gov.pt.",
									 L"O Middleware do Cart�o de Cidad�o instalado n�o possui suporte para CMD. "
									 L"Obtenha um instalador oficial em https://www.autenticacao.gov.pt.",
									 L"", L""};
DEFINE_LANG_STR(ValidatingCertificate) = {L"Validating certificate...", L"A validar certificado..."};
} // namespace eIDMW

#endif // __CMDKSP_LANGUAGE_H__
