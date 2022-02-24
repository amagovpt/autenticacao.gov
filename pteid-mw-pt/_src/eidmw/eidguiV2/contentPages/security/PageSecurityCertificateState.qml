/*-****************************************************************************

 * Copyright (C) 2021 Tiago Barroso - <tiago.barroso@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions
import "../../components/" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageSecurityCertificateStateForm {

    Connections {
        target: gapi

        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }

        onSignalCardAccessError: {
            console.log("Security Pin Codes onSignalCardAccessError")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = ""
            if (error_code == GAPI.NoReaderFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
                propertyButtonValidateCertificates.enabled = false
                clearFields()
            }
            else if (error_code == GAPI.NoCardFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD")
                propertyButtonValidateCertificates.enabled = false
                clearFields()
            }
            else if (error_code == GAPI.SodCardReadError) {
                bodyPopup = qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
                    + "<br/><br/>" + qsTranslate("Popup Card","STR_GENERIC_ERROR_MSG")
            }
            else if (error_code == GAPI.CardUserPinCancel) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
            }
            else if (error_code == GAPI.CardPinTimeout) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else if (error_code == GAPI.IncompatibleReader) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_INCOMPATIBLE_READER")
            }
            else {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Security Certificates onSignalCardChanged")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyButtonValidateCertificates.enabled = false
                clearFields()
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyButtonValidateCertificates.enabled = true
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                clearFields()
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
        }
        onSignalCertificatesFail: {
            propertyBusyIndicator.running = false
            propertyButtonValidateCertificates.enabled = true

            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = qsTranslate("PageSecurityCertificates","STR_CERT_CHAIN_ERROR")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
        }

        onSignalCertificatesChanged: {
            propertyTextAuthIssuer.propertyDateField.text = certificatesMap.level0.IssuerName
            propertyTextAuthValidFrom.propertyDateField.text =  certificatesMap.level0.ValidityBegin
            propertyTextAuthValidUntil.propertyDateField.text =  certificatesMap.level0.ValidityEnd
            propertyTextAuthSerialNumber.propertyDateField.text =  certificatesMap.level0.SerialNumber
            propertyTextAuthStatus.propertyDateField.text =  getCertStatus(certificatesMap.level0.Status)
            if (certificatesMap.level0.Status === Constants.PTEID_CERTIF_STATUS_VALID)
            {
                propertyTextAuthStatus.propertyDateField.color = "green"
            }
            else 
            {
                propertyTextAuthStatus.propertyDateField.color = "red"
            }
            propertyTextSignIssuer.propertyDateField.text =  certificatesMap.levelB0.IssuerName
            propertyTextSignValidFrom.propertyDateField.text =  certificatesMap.levelB0.ValidityBegin
            propertyTextSignValidUntil.propertyDateField.text =  certificatesMap.levelB0.ValidityEnd
            propertyTextSignSerialNumber.propertyDateField.text =  certificatesMap.levelB0.SerialNumber
            propertyTextSignStatus.propertyDateField.text =  getCertStatus(certificatesMap.levelB0.Status)
            if (certificatesMap.levelB0.Status === Constants.PTEID_CERTIF_STATUS_VALID)
            {
                propertyTextSignStatus.propertyDateField.color = "green"
            }
            else 
            {
                propertyTextSignStatus.propertyDateField.color = "red"
            }

            propertyButtonValidateCertificates.enabled = true
            propertyBusyIndicator.running = false
            console.log("Certificates validation completed");
        }
    }

    propertyButtonValidateCertificates {
        onClicked: {
            console.log("Validate certificates button clicked")
            clearFields()
            propertyBusyIndicator.running = true
            propertyButtonValidateCertificates.enabled = false
            gapi.validateCertificates()
        }
    }

    propertyButtonCertificateDetails {
        onClicked: {
            mainFormID.propertyPageLoader.propertyForceFocus = true
            mainFormID.propertyPageLoader.source = "../../contentPages/security/PageSecurityCertificates.qml"
        }
    }

    Component.onCompleted: {
        console.log("Page Security Certificate State Completed")
        propertyButtonValidateCertificates.enabled = true
        propertyButtonValidateCertificates.forceActiveFocus()
    }

    function getCertStatus(certStatus){

        var strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_UNKNOWN");

        switch(certStatus)
        {
        case Constants.PTEID_CERTIF_STATUS_UNKNOWN:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_UNKNOWN")
            break;
        case Constants.PTEID_CERTIF_STATUS_REVOKED:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_REVOKED")
            break;
        case Constants.PTEID_CERTIF_STATUS_SUSPENDED:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_SUSPENDED")
            break;
        case Constants.PTEID_CERTIF_STATUS_CONNECT:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_NETWORK_ERROR")
            break;
        case Constants.PTEID_CERTIF_STATUS_ISSUER:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_ISSUER")
            break;
        case Constants.PTEID_CERTIF_STATUS_ERROR:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_ERROR")
            break;
        case Constants.PTEID_CERTIF_STATUS_VALID:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_VALID")
            break;
        case Constants.PTEID_CERTIF_STATUS_EXPIRED:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_EXPIRED")
            break;
        default:
            strCertStatus = qsTranslate("PageSecurityCertificates", "STR_STATUS_UNKNOWN");
            break;
        }

        return strCertStatus
    }

    function clearFields(){
        propertyTextAuthIssuer.propertyDateField.text = ""
        propertyTextAuthStatus.propertyDateField.text =  ""
        propertyTextAuthValidFrom.propertyDateField.text =  ""
        propertyTextAuthValidUntil.propertyDateField.text =  ""
        propertyTextAuthSerialNumber.propertyDateField.text =  ""
        propertyTextAuthStatus.propertyDateField.text = ""
        propertyTextAuthStatus.propertyDateField.color = "black"
        propertyTextSignIssuer.propertyDateField.text =  ""
        propertyTextSignStatus.propertyDateField.text =  ""
        propertyTextSignValidFrom.propertyDateField.text =  ""
        propertyTextSignValidUntil.propertyDateField.text =  ""
        propertyTextSignSerialNumber.propertyDateField.text =  ""
        propertyTextSignStatus.propertyDateField.color = "black"
        propertyTextSignStatus.propertyDateField.text = ""
    }
}

