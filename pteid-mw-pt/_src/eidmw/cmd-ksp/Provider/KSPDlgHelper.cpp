#include "../Inc/KSPDlgHelper.h"
#include "../Inc/log.h"
#include "language.h"

using namespace eIDMW;

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
    LogTrace(LOGTYPE_INFO, "CmdKspOpenDialogSign", "appWindow = [0x%08x]", parentWindow);
    SetApplicationWindow(parentWindow);
    DlgRet ret = DlgAskInputCMD(false, pin, pinLen, (wchar_t *)userId, userName, userNameLen);
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
    LogTrace(LOGTYPE_INFO, "CmdKspOpenDialogValidateOtp", "appWindow = [0x%08x]", parentWindow);
    SetApplicationWindow(parentWindow);
    DlgRet ret = DlgAskInputCMD(true, otp, otpLen, (wchar_t *)pszDocname);
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
__in    HWND parentWindow)
{
    if (parentWindow == NULL)
    {
        parentWindow = GetActiveWindow();
    }
    LogTrace(LOGTYPE_INFO, "CmdKspOpenDialogProgress", "appWindow = [0x%08x]", parentWindow);
    SetApplicationWindow(parentWindow);
    DlgRet ret = DlgCMDMessage(
        DlgCmdMsgType::DLG_CMD_PROGRESS, 
        (isValidateOtp ? GETSTRING_DLG(SendingOtp) : GETSTRING_DLG(ConnectingWithServer)));
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
__in    HWND parentWindow)
{
    if (parentWindow == NULL)
    {
        parentWindow = GetActiveWindow();
    }
    LogTrace(LOGTYPE_INFO, "CmdKspOpenDialogError", "appWindow = [0x%08x]", parentWindow);
    SetApplicationWindow(parentWindow);
    DlgRet ret = DlgCMDMessage(msgType, message);
    return;
}

CmdSignThread::CmdSignThread(CMDProxyInfo *cmd_proxyinfo, CMDSignature *cmdSignature, std::string mobileNumber, std::string pin, CByteArray hash, char* docname)
{
    m_cmd_proxyinfo = cmd_proxyinfo;
    m_cmdSignature = cmdSignature;
    m_mobileNumber = mobileNumber;
    m_pin = pin;
    m_hash = hash;
    m_docname = docname;
    m_isValidateOtp = false;
}

CmdSignThread::CmdSignThread(CMDSignature *cmdSignature, std::string otp)
{
    m_cmdSignature = cmdSignature;
    m_otp = otp;
    m_isValidateOtp = true;
}

void CmdSignThread::Run()
{
    CMDProxyInfo cmd_proxyinfo = CMDProxyInfo::buildProxyInfo();
    if (m_isValidateOtp)
    {
        m_signResult = m_cmdSignature->signClose(m_otp);
    }
    else
    {
        m_signResult = m_cmdSignature->signOpen(cmd_proxyinfo, m_mobileNumber, m_pin, m_hash, m_docname);
    }
    DlgCloseCMDMessage();
}

void CmdSignThread::Stop(unsigned long ulSleepFrequency)
{
    LogTrace(LOGTYPE_INFO, "CmdSignThread::Stop()", "Stop() called");
    if (m_cmdSignature)
    {
        m_cmdSignature->cancelRequest();
    }
    WaitTillStopped(ulSleepFrequency);
}

int CmdSignThread::GetSignResult()
{
    return m_signResult;
}