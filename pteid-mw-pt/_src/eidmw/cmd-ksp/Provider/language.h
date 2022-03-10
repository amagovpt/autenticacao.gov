/*-****************************************************************************

 * Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef __CMDKSP_LANGUAGE_H__
#define __CMDKSP_LANGUAGE_H__

#include <string>
#include "langUtil.h"

namespace eIDMW
{
DEFINE_LANG_STR(MobileNumberDoesNotMatchCert) = {L"The inserted mobile number does not match the number associated with the selected certificate.", L"O n\u00famero de telem\u00f3vel inserido n\u00e3o corresponde ao n\u00famero associado ao certificado selecionado."};
DEFINE_LANG_STR(ConnectionError) = { L"A connection error has occurred.", L"Ocorreu um erro na ligação" };
DEFINE_LANG_STR(PossibleProxyError) = { L"A connection error has occurred. Check proxy settings.", L"Ocorreu um erro na ligação. Verifique a configuração do proxy." };
DEFINE_LANG_STR(InvalidPinOrOtp) = { L"Invalid PIN or security code.", L"PIN ou código de segurança inválido." };
DEFINE_LANG_STR(OtpValidationFailed) = { L"Validation of security code failed. The request was not found or validation time has expired.", L"Não foi possível validar o código de confirmação. O pedido não foi encontrado ou o tempo de validação expirou." };
DEFINE_LANG_STR(InvalidCertificate) = { L"Invalid certificate.", L"Certificado inválido.", L"", L"" };
DEFINE_LANG_STR(VerifyIfDigitalSignatureIsActive) = { L"Verify that the digital signature subscription of the Chave Móvel Digital is active in https://www.autenticacao.gov.pt.", L"Verifique que a subscrição da assinatura digital da Chave Móvel Digital está ativada em https://www.autenticacao.gov.pt.", L"", L"" };
DEFINE_LANG_STR(RegisterCertificateAgain) = { L"Please try to register your CMD certificate in the application again.", L"Por favor, tente registar novamente o seu certificado da CMD na aplicação.", L"", L"" };
DEFINE_LANG_STR(NoCMDCredentials) = { L"The Citizen Card Middleware installed does not support CMD. An official installer can be obtained at https://www.autenticacao.gov.pt.", L"O Middleware do Cartão de Cidadão instalado não possui suporte para CMD. Obtenha um instalador oficial em https://www.autenticacao.gov.pt.", L"", L"" };
DEFINE_LANG_STR(ValidatingCertificate) = { L"Validating certificate...", L"A validar certificado..." };
}

#endif // __CMDKSP_LANGUAGE_H__
