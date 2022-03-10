/*-****************************************************************************

 * Copyright (C) 2021 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "cmdSignatureClient.h"
#include "cmdServices.h"
#include "cmdCertificates.h"
#include "MWException.h"
#include "eidErrors.h"
#include "credentials.h"
#include "Log.h"
#include "cryptoFramework.h"
#include "Util.h"
#include "MiscUtil.h"
#include "XadesSignature.h"
#include "SigContainer.h"

#include <codecvt>
#include <locale>

namespace eIDMW
{

    std::string CMDSignatureClient::m_basicAuthUser;
    std::string CMDSignatureClient::m_basicAuthPassword;
    std::string CMDSignatureClient::m_applicationId;

    CMDSignatureClient::~CMDSignatureClient()
    {
        for (APL_Certifs *certs : m_certificatesCache.certificates)
        {
            delete certs;
        }
    }

    void CMDSignatureClient::setMobileNumberCaching(bool enabled)
    {
        m_shouldMobileCache = enabled;
    }

    CByteArray CMDSignatureClient::Sign(const CByteArray& data, bool signatureKey, const char * docname, const char *mobileNumber, const char * userName)
    {
        if (!signatureKey)
        {
            MWLOG(LEV_ERROR, MOD_CMD, "CMDSignatureClient::Sign: only signature key is supported.");
            throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
        }

        handleMissingCredentials();

        CMDSignature cmdSignature(m_basicAuthUser,
                                  m_basicAuthPassword,
                                  m_applicationId);

        CMDProxyInfo cmd_proxyinfo = CMDProxyInfo::buildProxyInfo();

        CByteArray cData(data.GetBytes(), data.Size());

        std::string docId;
        if (docname)
        {
            docId = docname;
        }
        else
        {
            docId = getSignatureIdFromHash(cData);
        }

        std::string *mobile = NULL;
        std::string mobileFixed;
        if (mobileNumber)
        {
            // if mobile number is not null this is the number associated with a picked certificate (KSP)
            mobileFixed = mobileNumber;
            mobile = &mobileFixed;
        }
        else if (m_shouldMobileCache)
        {
            // get cache
            mobile = &m_mobileNumberCached;

            MWLOG(LEV_DEBUG, MOD_CMD, "Using cached mobile number...");
        }

        int ret;
        if (!m_mobileNumber.empty() && !m_pin.empty()) {
            ret = cmdSignature.signOpen(cmd_proxyinfo, m_mobileNumber, m_pin, cData, docId);
        } else {
            ret = cmdSignature.signOpen(cmd_proxyinfo, cData, docId.c_str(), mobile, userName);
        }
        handleErrorCode(ret, false);

        ret = cmdSignature.signClose();
        handleErrorCode(ret, true);

        updateCertificateCache(&(cmdSignature.m_certificates));

        return cmdSignature.m_signature;
    }

    CByteArray& CMDSignatureClient::SignXades(const char *output_path, const char * const* paths, unsigned int n_paths, APL_SignatureLevel level)
    {
        const char **input_paths = const_cast<const char **>(paths);
        if (paths == NULL || n_paths < 1 || !CPathUtil::checkExistingFiles(input_paths, n_paths)) {
            throw CMWEXCEPTION(EIDMW_ERR_CHECK);
        }

        if (level == LEVEL_LT) {
            MWLOG(LEV_ERROR, MOD_CMD, "CMDSignatureClient::SignXades(): Signature Level LEVEL_LT is not supported.");
            throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
        }

        DlgRet ret = openAuthenticationDialogPIN(DlgCmdOperation::DLG_CMD_SIGNATURE, &m_pin, &m_mobileNumber);
        handleErrorCode(ret, false);

        getCertificates(m_mobileNumber);

        auto sign_callback = [this](const CByteArray& bytes) { return Sign(bytes, true); };

        XadesSignature sig(m_certificatesCache.certificates.back(), sign_callback);

        if (level == LEVEL_TIMESTAMP) {
            sig.enableTimestamp();
        }
        else if (level == LEVEL_LTV) {
            sig.enableLongTermValidation();
        }

        CByteArray &signature = sig.signXades(input_paths, n_paths);

        if (sig.shouldThrowTimestampException()) {
            throw CMWEXCEPTION(EIDMW_TIMESTAMP_ERROR);
        }

        if (sig.shouldThrowLTVException()) {
            throw CMWEXCEPTION(EIDMW_LTV_ERROR);
        }

        //Zero-out currently stored PIN
        std::fill(m_pin.begin(), m_pin.end(), 0);

        SigContainer::createASiC(signature, input_paths, n_paths, output_path);
        return signature;
    }

    void CMDSignatureClient::SignXadesIndividual(const char *output_path, const char * const* paths, unsigned int n_paths)
    {
        throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED);
    }

    void CMDSignatureClient::SignASiC(const char *path, APL_SignatureLevel level)
    {
        if (path == NULL || !CPathUtil::checkExistingFiles(&path, 1)) {
            throw CMWEXCEPTION(EIDMW_ERR_CHECK);
        }

        DlgRet ret = openAuthenticationDialogPIN(DlgCmdOperation::DLG_CMD_SIGNATURE, &m_pin, &m_mobileNumber);
        handleErrorCode(ret, false);

        getCertificates(m_mobileNumber);

        auto sign_callback = [this](const CByteArray& bytes) { return Sign(bytes, true); };

        XadesSignature sig(m_certificatesCache.certificates.back(), sign_callback);
        if (level == LEVEL_TIMESTAMP) {
            sig.enableTimestamp();
        }
        else if (level == LEVEL_LTV) {
            sig.enableLongTermValidation();
        }
        sig.signASiC(path);

        //Zero-out currently stored PIN
        std::fill(m_pin.begin(), m_pin.end(), 0);
    }


    int CMDSignatureClient::SignPDF(PDFSignature &pdf_sig, const char *location, const char *reason, const char *outfile_path)
    {
        handleMissingCredentials();

        CMDSignature cmdSignature(m_basicAuthUser,
                                  m_basicAuthPassword,
                                  m_applicationId);

        CMDProxyInfo cmd_proxyinfo = CMDProxyInfo::buildProxyInfo();
        std::vector<PDFSignature *> pdfSignatures;
        const size_t batch_size = pdf_sig.getCurrentBatchSize();

        if (batch_size == 0)
        {
            cmdSignature.add_pdf_handler(&pdf_sig);
        }
        else {
            for (size_t i = 0; i < batch_size; i++)
            {

                PDFSignature * sig = pdf_sig.getSpecialCopy(i);
                pdfSignatures.push_back(sig);
                cmdSignature.add_pdf_handler(sig);
            }
        }

        try
        {
            std::string *mobileCache = NULL;
            if (m_shouldMobileCache)
            {
                MWLOG(LEV_DEBUG, MOD_CMD, "CMDSignatureClient::SignPDF using cached mobileNumber");
                mobileCache = &m_mobileNumberCached;
            }

            int ret;
            ret = cmdSignature.signOpen(cmd_proxyinfo, location, reason, outfile_path, mobileCache);
            handleErrorCode(ret, false);

            ret = cmdSignature.signClose();
            handleErrorCode(ret, true);

            updateCertificateCache(&(cmdSignature.m_certificates));

            for (PDFSignature *sig : pdfSignatures)
            {
                delete sig;
            }
            return ret; // FIXME: review codes (map CMD error codes?)

        }
        catch(...)
        {
            for (PDFSignature *sig : pdfSignatures)
            {
                delete sig;
            }
            throw;
        }
    }

    APL_Certifs* CMDSignatureClient::getCertificates()
    {
        handleMissingCredentials();

        if (m_certificatesCache.certificates.empty())
        {
            CMDCertificates cmdCertificates(m_basicAuthUser, m_basicAuthPassword, m_applicationId);
            std::vector<CByteArray> certificates;
            int ret = cmdCertificates.getCertificates(certificates);
            handleErrorCode(ret, false);

            updateCertificateCache(&certificates);
        }
        return m_certificatesCache.certificates.back();
    }

    APL_Certifs* CMDSignatureClient::getCertificates(const std::string& mobile)
    {
        handleMissingCredentials();

        if (m_certificatesCache.mobile.compare(mobile) != 0 || m_certificatesCache.certificates.empty())
        {
            CMDCertificates cmdCertificates(m_basicAuthUser, m_basicAuthPassword, m_applicationId);
            std::vector<CByteArray> certificates;
            int ret = cmdCertificates.getCertificates(certificates, mobile);
            handleErrorCode(ret, false);

            updateCertificateCache(&certificates);
        }

        return m_certificatesCache.certificates.back();
    }

    void CMDSignatureClient::handleErrorCode(int err, bool isOtp)
    {
        if (err != ERR_NONE)
        {
            //Zero-out currently stored PIN
            std::fill(m_pin.begin(), m_pin.end(), 0);

            if (err == ERR_OP_CANCELLED || err == DLG_CANCEL)
            {
                MWLOG(LEV_DEBUG, MOD_CMD, L"CMD dialog was canceled.");
                throw CMWEXCEPTION(EIDMW_ERR_OP_CANCEL);
            }

            // Errors:
            MWLOG(LEV_ERROR, MOD_CMD, L"Error in CMD operation: isOTP=%s", (isOtp ? L"yes" : L"no"));
            switch (err)
            {
            case ERR_GET_CERTIFICATE:
                MWLOG(LEV_ERROR, MOD_CMD, L"CMDSignatureClient::handleErrorCode: Expired account or inactive signature subscription.");
                throw CMWEXCEPTION(EIDMW_ERR_CMD_INACTIVE_ACCOUNT);

            case SOAP_ERR_INVALID_OTP:
                MWLOG(LEV_ERROR, MOD_CMD, L"CMDSignatureClient::handleErrorCode: Invalid PIN or OTP.");
                throw CMWEXCEPTION(EIDMW_ERR_CMD_INVALID_CODE);

            case SOAP_TCP_ERROR:
                MWLOG(LEV_ERROR, MOD_CMD, L"CMDSignatureClient::handleErrorCode: connection error occured.");
                throw CMWEXCEPTION(EIDMW_ERR_CMD_CONNECTION);

            // TODO: Add mapping to missing errors that may be returned from CMD calls. (Internal errors, ...)

            default:
                MWLOG(LEV_ERROR, MOD_CMD, L"CMDSignatureClient::handleErrorCode: Error code 0x%x.", err);
                throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
            }

        }
    }

    std::string CMDSignatureClient::getSignatureIdFromHash(CByteArray hash)
    {
        APL_CryptoFwk *cryptoFwk = (APL_CryptoFwk *)AppLayer.getCryptoFwk();
        CByteArray b64DocIdBA;
        std::string b64DocId;
        if (cryptoFwk->b64Encode(hash, b64DocIdBA) && b64DocIdBA.Size() >= B64_ID_FROM_HASH_SIZE)
        {
            b64DocId.assign((char *)b64DocIdBA.GetBytes(), B64_ID_FROM_HASH_SIZE);
        }
        else
        {
            MWLOG(LEV_ERROR, MOD_CMD, L"CMDSignatureClient::getSignatureIdFromHash: Error computing Base64 encoding of hash.");
        }
        return b64DocId;
    }

    void CMDSignatureClient::updateCertificateCache(std::vector<CByteArray> *certs)
    {
        APL_CryptoFwk *cryptoFwk = (APL_CryptoFwk *)AppLayer.getCryptoFwk();

        APL_Certifs *cmdStore = new APL_Certifs(false);
        m_certificatesCache.certificates.push_back(cmdStore);
        m_certificatesCache.mobile = m_mobileNumber;
        for (size_t i = 0; i < certs->size(); i++)
        {
            CByteArray &certI = certs->at(i);
            tCertifInfo certIInfo;
            cryptoFwk->getCertInfo(certI, certIInfo);
            APL_CertifType type;
            if (cryptoFwk->isSelfIssuer(certI))
                type = APL_CERTIF_TYPE_ROOT;
            else
            {
                type = APL_CERTIF_TYPE_SIGNATURE;

                /* If cert is issuer of another cert in the chain, it is CA. */
                for (size_t j = 0; j < certs->size(); j++)
                {
                    CByteArray &certJ = certs->at(j);
                    tCertifInfo certJInfo;
                    cryptoFwk->getCertInfo(certJ, certJInfo);
                    if (certIInfo.ownerName == certJInfo.issuerName)
                    {
                        type = APL_CERTIF_TYPE_ROOT_SIGN;
                    }
                }
            }
            cmdStore->addCert(certI, type);
        }

        if (cmdStore->countAll() == 0)
            return;

        /* Look for root in eidstore folder: get top cert in cmdStore chain
            and look for issuer in eidstore. */
        APL_Certifs eidstore;
        APL_Certif *topCert = cmdStore->getCert(0);
        while (topCert->getIssuer() != NULL)
        {
            topCert = topCert->getIssuer();
        }

        for (size_t i = 0; i < eidstore.countAll(); i++)
        {
            APL_Certif *caCert = eidstore.getCert(i);
            if (caCert->isRoot() &&
                strcmp(caCert->getOwnerName(), topCert->getIssuerName()) == 0)
            {
                cmdStore->addCert(caCert->getData(), APL_CERTIF_TYPE_ROOT);
                return;
            }
        }

    }

    DlgRet CMDSignatureClient::openAuthenticationDialogPIN(DlgCmdOperation operation, std::string *out_pin, std::string *mobileNumber, const char * userName)
    {
        bool mobileNumberIsAsked = userName == NULL;

        std::wstring pinW;
        const unsigned long pinBufferLength = MAX_PIN_SIZE + 1;
        wchar_t pinBuffer[pinBufferLength];
        wmemset(pinBuffer, L'\0', pinBufferLength);

        std::wstring mobileW = utilStringWiden(*mobileNumber);
        const unsigned long mobileBufferLength = MAX_USER_SIZE + 1;;
        wchar_t mobileBuffer[mobileBufferLength];
        wcsncpy(mobileBuffer, mobileW.c_str(), mobileBufferLength);

        if (mobileW.size() > 0) {
            MWLOG(LEV_DEBUG, MOD_CMD, L"openAuthenticationDialogPIN: mobileNumberIsAsked?: %d cached mobileNumber length: %d", mobileNumberIsAsked ? 1 : 0, mobileW.size());
        }

        //if mobile number is not asked, DlgAskInputCMD expects 0 as the "buffer" length
        unsigned long mobileNumberLength = (mobileNumberIsAsked ? mobileBufferLength : 0);

        std::wstring userNameWstr;
        const wchar_t *userNameW = NULL;
        unsigned long userNameLen = 0;
        if (userName)
        {
            userNameWstr = utilStringWiden(userName);
            userNameW = userNameWstr.c_str();
            userNameLen = userNameWstr.size();
        }

        DlgRet ret = DlgAskInputCMD(operation, false, pinBuffer, pinBufferLength, mobileBuffer, mobileNumberLength, userNameW, userNameLen, NULL);

        out_pin->assign(utilStringNarrow(pinBuffer));

        if (mobileNumberIsAsked)
        {
            mobileNumber->assign(utilStringNarrow(mobileBuffer));
        }

        return ret;
    }


    DlgRet CMDSignatureClient::openAuthenticationDialogOTP(DlgCmdOperation operation, std::string *out_otp, std::string *docId, std::function<void(void)> *fSendSmsCallback)
    {
        std::wstring otpW;
        const unsigned long otpBufferLength = MAX_OTP_SIZE + 1;
        wchar_t otpBuffer[otpBufferLength];
        wmemset(otpBuffer, L'\0', otpBufferLength);

#ifdef _WIN32
        std::wstring docIdW = utilStringWiden(*docId);
#else
        //utilStringWiden doesn't do what we need on Unix: UTF-8 to UTF-32 string conversion
        std::u32string u32_docId = stringWidenUTF32(*docId);
        std::wstring docIdW((wchar_t *) u32_docId.c_str());
#endif

        DlgRet ret = DlgAskInputCMD(operation, true, otpBuffer, otpBufferLength, (wchar_t *)docIdW.c_str(), 0, NULL, 0, fSendSmsCallback);

        out_otp->assign(utilStringNarrow(otpBuffer));
        return ret;
    }

    void CMDSignatureClient::setCredentials(const char* basicAuthUser, const char* basicAuthPassword, const char* applicationId)
    {
        m_basicAuthUser = basicAuthUser;
        m_basicAuthPassword = basicAuthPassword;
        m_applicationId = applicationId;
    }

    void CMDSignatureClient::handleMissingCredentials()
    {
        if (m_basicAuthUser.empty() || m_basicAuthPassword.empty() || m_applicationId.empty())
        {
            throw CMWEXCEPTION(EIDMW_ERR_CMD_BAD_CREDENTIALS);
        }
    }

    CMDProgressDlgThread::CMDProgressDlgThread(DlgCmdOperation operation, bool isOtp, std::function<void(void)> *fCancelCallback)
    {
        m_operation = operation;
        m_isOtp = isOtp;
        m_fCancelCallback = fCancelCallback;
    }

    void CMDProgressDlgThread::Run()
    {
        m_oldDlgHandle = m_dlgHandle;
        DlgRet ret = DlgCMDMessage(m_operation, DlgCmdMsgType::DLG_CMD_PROGRESS, m_isOtp, &m_dlgHandle);

        if (ret == DLG_CANCEL)
        {
            m_wasCancelled = true;
            if (m_fCancelCallback)
            {
                (*m_fCancelCallback)();
            }
        }
    }

    void CMDProgressDlgThread::Stop(unsigned long ulSleepFrequency)
    {
        /* m_dlgHandle and m_oldDlgHandle are equal before showing the dialog.
        the handle is changed after the dialog is shown. If the stored handles are different,
        the dialog is showing. This is important to ensure the correct handle to close is already set! */
        for (size_t i = 0; i < 100 ; i++) // 10 seconds should be enough to show the dialog...
        {
            SleepMillisecs(100);
            if (m_oldDlgHandle != m_dlgHandle)
                break;
        }

        DlgCloseCMDMessage(m_dlgHandle);
        WaitTillStopped();
    }

}