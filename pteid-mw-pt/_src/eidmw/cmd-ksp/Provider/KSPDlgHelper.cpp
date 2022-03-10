/*-****************************************************************************

 * Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "../Inc/KSPDlgHelper.h"
#include "../Inc/log.h"
#include "language.h"
#include "cryptoFramework.h"

using namespace eIDMW;

static char logBuf[512];

/******************************************************************************
*
* DESCRIPTION : Opens a dialog for the user to introduce the mobile number and
*               the PIN for CMD signature
*
* INPUTS :
*               unsigned long userIdLen        Size of userId buffer
*               unsigned long pinLen           Size of pin buffer
*               HWND parentWindow              Handle to parent window
* OUTPUTS :
*               wchar_t *userId    Mobile number introduced
*               wchar_t *pin       PIN introduced
* RETURN :
*               ERROR_SUCCESS          The function was successful.
*/
SECURITY_STATUS
CmdKspOpenDialogSign(
__in    const wchar_t *userId,
__in    unsigned long userIdLen,
__out   wchar_t *pin,
__in    unsigned long pinLen,
__in    const wchar_t *userName,
__in    unsigned long userNameLen,
__in    HWND parentWindow)
{
    if (parentWindow == NULL)
    {
        parentWindow = GetActiveWindow();
    }
    MWLOG_DEBUG(logBuf, "appWindow = [0x%08x]", parentWindow);
    SetApplicationWindow(parentWindow);
    DlgRet ret = DlgAskInputCMD(DlgCmdOperation::DLG_CMD_SIGNATURE, false, pin, pinLen, (wchar_t *)userId, 0, userName, userNameLen);
    if (ret == DLG_OK)
    {
        return ERROR_SUCCESS;
    }
    else if (ret == DLG_CANCEL)
    {
        return NTE_USER_CANCELLED;
    }
    else
    {
        return NTE_INTERNAL_ERROR;
    }
}

/******************************************************************************
*
* DESCRIPTION : Opens a dialog for the user to introduce the OTP
*
* INPUTS :
*               unsigned long otpLen       Size of OTP buffer
*               HWND parentWindow          Handle to parent window
*               LPWSTR pszDocname          Null-terminated string with the document id
* OUTPUTS :
*               wchar_t *otp               OTP introduced by the user
* RETURN :
*               ERROR_SUCCESS          The function was successful.
*/
SECURITY_STATUS
CmdKspOpenDialogOtp(
__out   wchar_t *otp,
__in    unsigned long otpLen,
__in    LPWSTR pszDocname,
__in    HWND parentWindow)
{
    if (parentWindow == NULL)
    {
        parentWindow = GetActiveWindow();
    }
    MWLOG_DEBUG(logBuf, "appWindow = [0x%08x]", parentWindow);
    SetApplicationWindow(parentWindow);
    std::function<void(void)> fSendSmsCallback = sendSmsCallback;
    DlgRet ret = DlgAskInputCMD(DlgCmdOperation::DLG_CMD_SIGNATURE, true, otp, otpLen, (wchar_t *)pszDocname, 0, NULL, 0, &fSendSmsCallback);
    if (ret == DLG_OK)
    {
        return ERROR_SUCCESS;
    }
    else if (ret == DLG_CANCEL)
    {
        return NTE_USER_CANCELLED;
    }
    else
    {
        return NTE_INTERNAL_ERROR;
    }
}

SECURITY_STATUS
CmdKspOpenDialogProgress(
__in    bool isValidateOtp,
__in    HWND parentWindow,
__in    unsigned long *pulHandle)
{
    if (parentWindow == NULL)
    {
        parentWindow = GetActiveWindow();
    }
    MWLOG_DEBUG(logBuf, "appWindow = [0x%08x]", parentWindow);
    SetApplicationWindow(parentWindow);
    DlgRet ret = DlgCMDMessage(DlgCmdOperation::DLG_CMD_SIGNATURE,
        DlgCmdMsgType::DLG_CMD_PROGRESS, 
        (isValidateOtp ? GETSTRING_DLG(SendingOtp) : GETSTRING_DLG(ConnectingWithServer)), pulHandle);
    if (ret == DLG_CANCEL)
    {
        return NTE_USER_CANCELLED;
    }
    else if (ret != DLG_OK)
    {
        return NTE_INTERNAL_ERROR;
    }
    return ERROR_SUCCESS;
}

void
CmdKspOpenDialogError(
__in    const wchar_t *message,
__in    DlgCmdMsgType msgType,
__in    HWND parentWindow,
__in    unsigned long *pulHandle)
{
    if (parentWindow == NULL)
    {
        parentWindow = GetActiveWindow();
    }
    MWLOG_DEBUG(logBuf, "appWindow = [0x%08x]", parentWindow);
    SetApplicationWindow(parentWindow);
    DlgRet ret = DlgCMDMessage(DlgCmdOperation::DLG_CMD_SIGNATURE, msgType, message, pulHandle);
    return;
}

void sendSmsCallback()
{
    if (CmdSignThread::cmdSignature)
    {
        CmdSignThread::cmdSignature->sendSms();
    }
}

CMDSignature *CmdSignThread::cmdSignature = NULL;
CmdSignThread::CmdSignThread(CMDProxyInfo *cmd_proxyinfo, CMDSignature *cmdSignature,
    std::string mobileNumber, std::string pin, CByteArray hash, char* docname, PCCERT_CONTEXT pCert, unsigned long *pulHandle)
{
    m_cmd_proxyinfo = cmd_proxyinfo;
    CmdSignThread::cmdSignature = cmdSignature;
    m_mobileNumber = mobileNumber;
    m_pin = pin;
    m_hash = hash;
    m_docname = docname;
    m_pCert = pCert;
    m_isValidateOtp = false;
    m_pulHandle = pulHandle;
}

CmdSignThread::CmdSignThread(CMDSignature *cmdSignature, std::string otp, unsigned long *pulHandle)
{
    CmdSignThread::cmdSignature = cmdSignature;
    m_otp = otp;
    m_isValidateOtp = true;
    m_pulHandle = pulHandle;
}

void CmdSignThread::Run()
{
    CMDProxyInfo cmd_proxyinfo = CMDProxyInfo::buildProxyInfo();
    if (m_isValidateOtp)
    {
        m_signResult = cmdSignature->signClose(m_otp);
    }
    else
    {
        m_signResult = cmdSignature->signOpen(cmd_proxyinfo, m_mobileNumber, m_pin, m_hash, m_docname);

        if (m_signResult != ERR_NONE)
        {
            goto end;
        }
        
        // The state of the certificate is checked after signOpen because the certificate is revoked when
        // the CMD subscription is canceled but if it was not activated again, the CMD service will return an error
        // (ERR_GET_CERTIFICATE). This allows to show a more helpful message to the user.
        std::string url;
        APL_CryptoFwk *cryptoFwk = (APL_CryptoFwk *)AppLayer.getCryptoFwk();

        HCERTSTORE hCAStore = NULL;
        hCAStore = CertOpenSystemStore((HCRYPTPROV_LEGACY)NULL, "CA");

        PCCERT_CONTEXT pIssuerCert = NULL;
        DWORD dwFlags = 0;
        pIssuerCert = CertGetIssuerCertificateFromStore(
            hCAStore, m_pCert, pIssuerCert, &dwFlags);
        // TODO: It should be checked if this is the correct issuer but there should be only one
        if (!pIssuerCert)
        {
            MWLOG_WARN(logBuf, "Unable to find issuer certificate in store.");
            goto end;
        }
        
        CByteArray certBytes(m_pCert->pbCertEncoded, m_pCert->cbCertEncoded);
        CByteArray issuerCertBytes(pIssuerCert->pbCertEncoded, pIssuerCert->cbCertEncoded);
        FWK_CertifStatus status = cryptoFwk->OCSPValidation(certBytes, issuerCertBytes);
        MWLOG_INFO(logBuf, "OCSP verification status for CMD cert: %d", status);

        if (status == FWK_CertifStatus::FWK_CERTIF_STATUS_REVOKED)
        {
            m_signResult = ERR_INV_CERTIFICATE;
        }

        CertFreeCertificateContext(pIssuerCert);        
    }

end:
    DlgCloseCMDMessage(*m_pulHandle);
}

void CmdSignThread::Stop(unsigned long ulSleepFrequency)
{
    MWLOG_DEBUG(logBuf, "Stop() called");
    if (cmdSignature)
    {
        cmdSignature->cancelRequest();
    }
    WaitTillStopped(ulSleepFrequency);
}

int CmdSignThread::GetSignResult()
{
    return m_signResult;
}