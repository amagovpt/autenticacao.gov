/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2021 Miguel Figueira - <miguel.figueira@caixamagica.pt>
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

**************************************************************************** */
#include "eidlib.h"
#include "eidlibException.h"
#include "InternalUtil.h"
#include "eidErrors.h"
#include "cmdSignatureClient.h"
#include "credentials.h"
#include "ByteArray.h"
#include "Log.h"
#include "PDFSignature.h"
#include "Util.h"

namespace eIDMW
{

    PTEID_CMDSignatureClient::PTEID_CMDSignatureClient():PTEID_Object(NULL, new CMDSignatureClient())
    {
        m_delimpl = true;
    }

    PTEID_CMDSignatureClient::~PTEID_CMDSignatureClient()
    {
        if (m_delimpl)
        {
            CMDSignatureClient *pimpl = static_cast<CMDSignatureClient *>(m_impl);
            delete pimpl;
            m_impl = NULL;
        }

        for (PTEID_Certificates *certs : m_certificates)
        {
            delete certs;
        }
    }

    void PTEID_CMDSignatureClient::setMobileNumberCaching(bool enabled)
    {
        CMDSignatureClient *pCmdClient = static_cast<CMDSignatureClient *>(m_impl);
        pCmdClient->setMobileNumberCaching(enabled);
    }

    PTEID_ByteArray PTEID_CMDSignatureClient::Sign(const PTEID_ByteArray& data, bool signatureKey) 
    {
        if (!signatureKey)
        {
            MWLOG(LEV_ERROR, MOD_SDK, L"PTEID_CMDSignatureClient::Sign: only signature key is supported.");
            throw PTEID_Exception(EIDMW_ERR_PARAM_BAD);
        }

        PTEID_ByteArray out;

        BEGIN_TRY_CATCH

        CMDSignatureClient *pCmdClient = static_cast<CMDSignatureClient *>(m_impl);

        CByteArray cData(data.GetBytes(), data.Size());
        CByteArray result = pCmdClient->Sign(cData, signatureKey);
        out.Append(result.GetBytes(), result.Size());

        END_TRY_CATCH

        return out;
    }

    PTEID_ByteArray PTEID_CMDSignatureClient::SignSHA256(const PTEID_ByteArray& data, bool signatureKey) 
    {
        return Sign(data, signatureKey);
    }
    
    PTEID_ByteArray PTEID_CMDSignatureClient::SignXades(const char *output_path, const char * const* paths, unsigned int n_paths, PTEID_SignatureLevel level)
    {
        PTEID_ByteArray out;

        BEGIN_TRY_CATCH

        CMDSignatureClient *pCmdClient = static_cast<CMDSignatureClient *>(m_impl);

        CByteArray &result = pCmdClient->SignXades(output_path, paths, n_paths, ConvertSignatureLevel(level));
        out.Append(result.GetBytes(), result.Size());
        delete &result;

        END_TRY_CATCH

        return out;
    }

    PTEID_ByteArray PTEID_CMDSignatureClient::SignXadesT(const char *output_path, const char * const* paths, unsigned int n_paths)
    {
        return SignXades(output_path, paths, n_paths, PTEID_LEVEL_TIMESTAMP);
    }

    PTEID_ByteArray PTEID_CMDSignatureClient::SignXadesA(const char *output_path, const char * const* paths, unsigned int n_paths)
    {
        return SignXades(output_path, paths, n_paths, PTEID_LEVEL_LTV);
    }

    void PTEID_CMDSignatureClient::SignXadesIndividual(const char *output_path, const char * const* paths, unsigned int n_paths)
    {
        BEGIN_TRY_CATCH

        CMDSignatureClient *pCmdClient = static_cast<CMDSignatureClient *>(m_impl);
        pCmdClient->SignXadesIndividual(output_path, paths, n_paths);

        END_TRY_CATCH
    }

    void PTEID_CMDSignatureClient::SignXadesTIndividual(const char *output_path, const char * const* paths, unsigned int n_paths)
    {
        return SignXadesIndividual(output_path, paths, n_paths);
    }

    void PTEID_CMDSignatureClient::SignXadesAIndividual(const char *output_path, const char * const* paths, unsigned int n_paths)
    {
        return SignXadesIndividual(output_path, paths, n_paths);
    }

    void PTEID_CMDSignatureClient::SignASiC(const char *path, PTEID_SignatureLevel level)
    {
        BEGIN_TRY_CATCH

        CMDSignatureClient *pCmdClient = static_cast<CMDSignatureClient *>(m_impl);
        pCmdClient->SignASiC(path, ConvertSignatureLevel(level));

        END_TRY_CATCH
    }

    int PTEID_CMDSignatureClient::SignPDF(PTEID_PDFSignature &sig_handler, int page, int page_sector, bool is_landscape, 
        const char *location, const char *reason, const char *outfile_path)
    {

        PDFSignature *pdf_sig = NULL;
        int rc = 0;

        BEGIN_TRY_CATCH

        CMDSignatureClient *pCmdClient = static_cast<CMDSignatureClient *>(m_impl);

        pdf_sig = sig_handler.getPdfSignature();

        if (page_sector != 0 && page != 0)
            pdf_sig->setVisible(page, page_sector);

        rc = pCmdClient->SignPDF(*pdf_sig, location, reason, outfile_path);

        END_TRY_CATCH

        return rc;
    }

    int PTEID_CMDSignatureClient::SignPDF(PTEID_PDFSignature &sig_handler, int page, double coord_x, double coord_y, 
        const char *location, const char *reason, const char *outfile_path)
    {
        PDFSignature *pdf_sig = NULL;
        int rc = 0;

        BEGIN_TRY_CATCH

        CMDSignatureClient *pCmdClient = static_cast<CMDSignatureClient *>(m_impl);

        pdf_sig = sig_handler.getPdfSignature();

        if (coord_x >= 0 && coord_y >= 0) {
            pdf_sig->setVisibleCoordinates(page, coord_x, coord_y);
        }
        rc = pCmdClient->SignPDF(*pdf_sig, location, reason, outfile_path);

        END_TRY_CATCH

        return rc;
    }

    PTEID_Certificates& PTEID_CMDSignatureClient::getCertificates()
    {
        BEGIN_TRY_CATCH

        CMDSignatureClient *pCmdClient = static_cast<CMDSignatureClient *>(m_impl);
        /* Check if the cached APL_Certifs from call to GetCertifica Service matches the
        last PTEID_Certificates in m_certificates. */
        APL_Certifs *cachedCertifs = pCmdClient->getCertificates();
        if (m_certificates.empty())
        {
            m_certificates.push_back(new PTEID_Certificates(NULL, cachedCertifs));
            return *m_certificates.back();
        }
        APL_Certifs *lastWrappedCertifs = (APL_Certifs*)m_certificates.back()->m_impl;
        if (cachedCertifs != lastWrappedCertifs)
        {
            m_certificates.push_back(new PTEID_Certificates(NULL, cachedCertifs));
        }
        return *m_certificates.back();

        END_TRY_CATCH
    }

    PTEID_Certificate &PTEID_CMDSignatureClient::getCert(PTEID_CertifType type)
    {
        return getCertificates().getCert(type);
    }

    PTEID_Certificate &PTEID_CMDSignatureClient::getRoot()
    {
        return getCertificates().getCert(PTEID_CERTIF_TYPE_ROOT);
    }

    PTEID_Certificate &PTEID_CMDSignatureClient::getCA()
    {
        return getCertificates().getCert(PTEID_CERTIF_TYPE_ROOT_SIGN);
    }

    PTEID_Certificate &PTEID_CMDSignatureClient::getAuthentication()
    {
        throw PTEID_Exception(EIDMW_ERR_BAD_USAGE);
    }

    PTEID_Certificate &PTEID_CMDSignatureClient::getSignature()
    {
        return getCertificates().getCert(PTEID_CERTIF_TYPE_SIGNATURE);
    }

    void PTEID_CMDSignatureClient::setCredentials(const char* basicAuthUser, const char* basicAuthPassword, const char* applicationId)
    {
        CMDSignatureClient::setCredentials(basicAuthUser, basicAuthPassword, applicationId);
    }

}