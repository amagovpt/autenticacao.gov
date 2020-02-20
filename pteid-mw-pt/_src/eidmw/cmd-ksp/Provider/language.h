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
DEFINE_LANG_STR(ConnectingWithServer) = { L"Connecting with server.", L"A conectar com o servidor.", L"", L"" };
DEFINE_LANG_STR(SendingOtp) = { L"Sending confirmation code to server.", L"Enviando o código de confirmação para o servidor.", L"", L"" };
}

#endif // __CMDKSP_LANGUAGE_H__
