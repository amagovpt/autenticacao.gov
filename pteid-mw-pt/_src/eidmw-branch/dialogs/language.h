/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

****************************************************************************
**/


#ifndef __DLGS_LANGUAGE_H__
#define __DLGS_LANGUAGE_H__

#include <string>

namespace eIDMW
{

class CLang;

#define DEFINE_LANG_STR(code) static const wchar_t * const LANG_STR_ ## code []

#define LANG_STRCODE_EN L"EN"
#define LANG_STRCODE_NL L"NL"


typedef enum {
  LANG_EN=0,
  LANG_NL=1,
} tLanguage;

DEFINE_LANG_STR(Yes) = {L"&Yes", L"&Sim", L"", L""};
DEFINE_LANG_STR(No) = {L"&No", L"&Nao", L"", L""};
DEFINE_LANG_STR(Ok) = {L"&OK", L"&OK", L"O&k", L"&Ok"};
DEFINE_LANG_STR(Cancel) = {L"&Cancel", L"&Cancelar", L"", L""};
DEFINE_LANG_STR(CancelNo) = {L"&Cancel (No)", L"&Cancelar (Nao)", L"", L""};
DEFINE_LANG_STR(Pin) = {L"PIN", L"PIN", L"", L""};
DEFINE_LANG_STR(Puk) = {L"PUK", L"PUK", L"", L""};
DEFINE_LANG_STR(Retry) = {L"&Retry", L"Repetir", L"", L""};
DEFINE_LANG_STR(Always) = {L"&Always", L"&Sempre", L"", L""};
DEFINE_LANG_STR(Never) = {L"Ne&ver", L"Nunca", L"", L""};
DEFINE_LANG_STR(Application) = {L"Application:", L"Aplicacao:", L"", L""};
DEFINE_LANG_STR(Function) = {L"Function:", L"Funcao:", L"", L""};
DEFINE_LANG_STR(EnterYourPin) = {L"Enter your PIN", L"Introduza o PIN", L"", L""};
DEFINE_LANG_STR(Asking) = {L"Asking", L"Pedir", L"", L""};
DEFINE_LANG_STR(SigningWith) = {L"Signing with", L"Assinar com", L"", L""};
DEFINE_LANG_STR(EnterYour) = {L"Enter your", L"Introduza o seu", L"", L""};
DEFINE_LANG_STR(ForVerification) = {L"for verification", L"voor verificatie", L"", L""};
DEFINE_LANG_STR(Unblock) = {L"Unblock PIN code", L"Desbloquear código PIN", L"", L""};
DEFINE_LANG_STR(ToUnblock) = {L"to unblock", L"para desbloquear", L"", L""};
DEFINE_LANG_STR(UnblockPinHeader) = {L"Unblock PIN code", L"Desbloquear código PIN", L"", L""};
DEFINE_LANG_STR(UnlockDialogHeader) = {L"To unlock your PIN code you must know the matching PUK code", L"Para desbloquear o PIN terá de saber qual o respectivo código de desbloqueio", L"", L""};
DEFINE_LANG_STR(Code) = {L"code", L"código", L"", L""};
DEFINE_LANG_STR(AndSigning) = {L"and signing", L"e assinar", L"", L""};
DEFINE_LANG_STR(AndSign) = {L"and sign", L"e assinar", L"", L""};
DEFINE_LANG_STR(RenewingPinCode) = {L"Renewing PIN code", L"Renovar código PIN", L"", L""};
DEFINE_LANG_STR(VerifyingPinCode) = { L"Verify PIN code", L"Verificar código PIN", L"", L""};
DEFINE_LANG_STR(CurrentPin) = {L"Current PIN", L"PIN Actual", L"", L""};
DEFINE_LANG_STR(NewPin) = {L"New PIN", L"Novo PIN", L"", L""};
DEFINE_LANG_STR(ConfirmNewPin) = {L"Confirm new PIN", L"Confirmar novo PIN", L"", L""};
DEFINE_LANG_STR(EnterYourNewPinCode) = {L"Enter your New PIN code", L"Introduza o novo código PIN", L"", L""};
DEFINE_LANG_STR(EnterYourNewPinCodeAgainToConfirm) = {L"Enter your New PIN code again to confirm", L"Introduza novamente o novo código PIN", L"", L""};
DEFINE_LANG_STR(ErrorTheNewPinCodesAreNotIdentical) = {L"Error: The new PIN codes are not identical", L"Erro: os novos PINs não são iguais", L"", L""};
DEFINE_LANG_STR(RetryEnterYourNewPinCode) = {L"Retry: Enter your New PIN code", L"Repetir: Introduza o novo código PIN", L"", L""};
DEFINE_LANG_STR(Bad) = {L"Bad", L"Erro", L"Erreur", L"Fehler"};
DEFINE_LANG_STR(Notification) = {L"Notification", L"Atenção", L"", L""};
DEFINE_LANG_STR(RemainingAttempts) = {L"remaining attempts", L"tentativas restantes", L"", L""};
DEFINE_LANG_STR(TryAgainOrCancel) = {L"Try again or cancel?", L"Tentar novamente ou cancelar?", L"", L""};
DEFINE_LANG_STR(PinBlocked) = {L"PIN blocked", L"PIN bloqueado", L"", L""};
DEFINE_LANG_STR(PinpadInfo) = {L"Pinpad info", L"Pinpad info", L"", L""};
DEFINE_LANG_STR(Message) = {L"Message", L"Mensagem", L"", L""};
DEFINE_LANG_STR(SignaturePin) = {L"Signature PIN", L"PIN de assinatura", L"", L""};
DEFINE_LANG_STR(ActivationPin) = {L"Activation PIN", L"Código de ativação", L"", L""};
DEFINE_LANG_STR(AuthenticationPin) = {L"Authentication PIN", L"PIN de autenticação", L"", L""};
DEFINE_LANG_STR(AddressPin) = {L"Address PIN", L"PIN de morada", L"", L""};
DEFINE_LANG_STR(UnknownPin) = {L"Unknown PIN", L"PIN desconhecido", L"", L""};
DEFINE_LANG_STR(PleaseEnterTheCorrectEidCard) = {L"Please enter the correct eID card.", L"Introduza um cartão válido.", L"", L""};
DEFINE_LANG_STR(Caution) = {L"Caution:", L"Atenção:", L"Attention:", L"Achtung:"};
DEFINE_LANG_STR(YouAreAboutToMakeALegallyBindingElectronic) = {L"You are about to make a legally binding electronic signature with your identity card.", L"Vai realizar uma assinatura eletrónica válida com o seu Cartão de Cidadão.", L"", L""};
DEFINE_LANG_STR(PleaseEnterYourPin) = {L"Please enter your PIN", L"Introduza o código PIN", L"", L""};
DEFINE_LANG_STR(PleaseEnterYourPinOnThePinpadReader) = {L"Please enter your PIN on the pinpad reader", L"Por favor, introduza o PIN no seu leitor PINPAD.", L"", L""};
DEFINE_LANG_STR(ToContinueOrClickTheCancelButton) = {L"to continue or click the Cancel button.", L"para continuar ou clique em cancelar", L"", L""};
DEFINE_LANG_STR(Warning) = {L"Warning:", L"Aviso:", L"", L""};
DEFINE_LANG_STR(InOrderToAuthenticateYourself) = {L"in order to authenticate yourself.", L"para se autenticar", L"", L""};
DEFINE_LANG_STR(ChangeYourPinOnTheReader) = {L"Change your PIN on the reader", L"Change your PIN on the reader", L"", L""};
DEFINE_LANG_STR(PleaseEnterYourPuk) = {L"Please enter your PUK", L"Introduza o cod. PUK", L"", L""};
DEFINE_LANG_STR(PleaseEnterYourPukOnThePinpadReader) = {L"Please enter your PUK on the pinpad reader", L"Por favor, introduza o código PUK no seu leitor PINPAD.", L"", L""};
DEFINE_LANG_STR(Your) = {L"your", L"seu", L"", L""};
DEFINE_LANG_STR(ChangeYourPin) = {L"Change your PIN", L"Alterar PIN", L"", L""};
DEFINE_LANG_STR(ChangeYourPuk) = {L"Change your PUK", L"Alterar PUK", L"", L""};
DEFINE_LANG_STR(OnTheReader) = {L"on the reader", L"no leitor", L"", L""};
DEFINE_LANG_STR(EnterOldNewVerify) = {L"1) Enter your old PIN and press 'OK'. \n2) Enter your new PIN and press 'OK'.\n3) Enter your new PIN again and press 'OK'.",
L"1) Introduza o seu PIN atual e clique em 'OK'. \n2) Introduza o seu novo PIN e clique em 'OK'.\n3) Introduza novamente o seu novo PIN e clique em 'OK'.", L"", L""};

DEFINE_LANG_STR(UnlockDialogInstructions) = {L"1) Enter your PUK code and press 'OK'. \n2) Enter your new PIN and press 'OK'.3) Enter your new PIN again and press 'OK'.",
                                            L"1) Por favor insira o seu código PUK e pressione 'OK' \n2) Introduza o seu novo PIN e pressione 'OK'. \n3) Introduza novamente o seu novo PIN e pressione 'OK'.", L"", L""};

DEFINE_LANG_STR(UnlockWithoutPUKInstructions) = {L"1) Enter your new PIN and press 'OK'. \n2) Enter your new PIN again and press 'OK'.",
												 L"1) Por favor introduza o seu novo PIN e pressione 'OK'\n2) Introduza novamente o seu novo PIN e pressione 'OK'.", L"", L""};

#define GETSTRING_DLG(code) LANG_STR_ ## code[CLang::GetLang()]
#ifndef WIN32
#define GETQSTRING_DLG(code) QString::fromWCharArray(LANG_STR_ ## code[CLang::GetLang()])
#endif

}

#endif // __DLGS_LANGUAGE_H__
