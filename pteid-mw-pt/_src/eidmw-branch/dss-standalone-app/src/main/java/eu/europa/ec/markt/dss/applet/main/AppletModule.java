/*
 * DSS - Digital Signature Services
 *
 * Copyright (C) 2013 European Commission, Directorate-General Internal Market and Services (DG MARKT), B-1049 Bruxelles/Brussel
 *
 * Developed by: 2013 ARHS Developments S.A. (rue Nicolas Bové 2B, L-1253 Luxembourg) http://www.arhs-developments.com
 *
 * This file is part of the "DSS - Digital Signature Services" project.
 *
 * "DSS - Digital Signature Services" is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * DSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * "DSS - Digital Signature Services".  If not, see <http://www.gnu.org/licenses/>.
 */

package eu.europa.ec.markt.dss.applet.main;

import eu.europa.ec.markt.dss.applet.io.NativeHTTPDataLoader;
import eu.europa.ec.markt.dss.applet.io.RemoteAppletTSLCertificateSource;
import eu.europa.ec.markt.dss.applet.io.RemoteCRLSource;
import eu.europa.ec.markt.dss.applet.io.RemoteOCSPSource;
import eu.europa.ec.markt.dss.applet.io.RemoteTSPSource;
import eu.europa.ec.markt.dss.validation.TrustedListCertificateVerifier;
import eu.europa.ec.markt.dss.validation.certificate.CertificateSource;
import eu.europa.ec.markt.dss.validation.certificate.RemoteCertificateSource;
import eu.europa.ec.markt.dss.validation.crl.CRLSource;
import eu.europa.ec.markt.dss.validation.ocsp.OCSPSource;
import eu.europa.ec.markt.dss.validation.tsp.TSPSource;
import eu.europa.ec.markt.dss.validation102853.CommonCertificateVerifier;
import eu.europa.ec.markt.dss.validation102853.TrustedCertificateSource;

public class AppletModule {

    private final DSSAppletCore dssAppletCore;

    private static final String TSP_CONTEXT = "/tsp";
    private static final String OCSP_CONTEXT = "/ocsp";
    private static final String CRL_CONTEXT = "/crl";
    private static final String CERTIFICATE_CONTEXT = "/certificate";

    public AppletModule(DSSAppletCore dssAppletCore) {
        this.dssAppletCore = dssAppletCore;
    }


    CRLSource getCRLSource() {
        RemoteCRLSource crlSource = new RemoteCRLSource();
        crlSource.setDataLoader(new NativeHTTPDataLoader());
        crlSource.setUrl(getServiceURL() + CRL_CONTEXT);
        return crlSource;
    }

    OCSPSource getOSCPSource() {
        RemoteOCSPSource ocspSource = new RemoteOCSPSource();
        ocspSource.setUrl(getServiceURL() + OCSP_CONTEXT);
        ocspSource.setDataLoader(new NativeHTTPDataLoader());
        return ocspSource;
    }

    eu.europa.ec.markt.dss.validation.certificate.CertificateSource getCertificateSource() {

        final RemoteCertificateSource tslCertSource = new RemoteCertificateSource();
        tslCertSource.setDelegate(getCertificateSource102853());
        return tslCertSource;
    }

    eu.europa.ec.markt.dss.validation.CertificateVerifier getCertificateVerifier(CRLSource crlSource, OCSPSource ocspSource,
                                                                                 CertificateSource certificatesSource) {
        eu.europa.ec.markt.dss.validation.TrustedListCertificateVerifier certificateVerifier = new TrustedListCertificateVerifier();
        certificateVerifier.setCrlSource(crlSource);
        certificateVerifier.setOcspSource(ocspSource);
        certificateVerifier.setTrustedListCertificatesSource(certificatesSource);
        return certificateVerifier;
    }

    TSPSource getTSPSource() {
        final RemoteTSPSource remoteTSPSource = new RemoteTSPSource();
        remoteTSPSource.setUrl(getServiceURL() + TSP_CONTEXT);
        remoteTSPSource.setDataLoader(new NativeHTTPDataLoader());
        return remoteTSPSource;
    }

    TrustedCertificateSource getCertificateSource102853() {

        final RemoteAppletTSLCertificateSource trustedListsCertificateSource = new RemoteAppletTSLCertificateSource();
        trustedListsCertificateSource.setDataLoader(new NativeHTTPDataLoader());
        trustedListsCertificateSource.setServiceUrl(getServiceURL() + CERTIFICATE_CONTEXT);
        return trustedListsCertificateSource;
    }

    CommonCertificateVerifier getTrustedListCertificateVerifier102853(CRLSource crlSource, OCSPSource ocspSource,
                                                                      TrustedCertificateSource certificateSource) {
        final CommonCertificateVerifier trustedListCertificateVerifier = new CommonCertificateVerifier();
        trustedListCertificateVerifier.setCrlSource(crlSource);
        trustedListCertificateVerifier.setOcspSource(ocspSource);
        trustedListCertificateVerifier.setTrustedCertSource(certificateSource);
        return trustedListCertificateVerifier;
    }

    private String getServiceURL() {
        return dssAppletCore.getParameters().getServiceURL();
    }
}
