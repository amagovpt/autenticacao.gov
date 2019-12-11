/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions
import "../../components/" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageSecurityCertificatesForm {

    Keys.onPressed: {
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalCardAccessError: {
            console.log("Security Pin Codes onSignalCardAccessError")
            if (error_code == GAPI.NoReaderFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
            }
            else if (error_code == GAPI.NoCardFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_NO_CARD")
            }
            else if (error_code == GAPI.SodCardReadError) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
            }
            else if (error_code == GAPI.CardUserPinCancel) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
            }
			else if (error_code == GAPI.CardPinTimeout) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyGeneralPopUpRetSubMenu = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Security Certificates onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                clearFields()
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startfillCertificateList()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                clearFields()
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyGeneralPopUpRetSubMenu = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalCertificatesFail: {
            propertyBusyIndicator.running = false
            propertyButtonViewCertificate.enabled = false
            propertyButtonExportCertificate.enabled = false

            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("Popup Card","STR_POPUP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageSecurityCertificates","STR_CERT_CHAIN_ERROR")

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyGeneralPopUpRetSubMenu = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }

        onSignalCertificatesChanged: {

            console.log("Certificates Added: "+ certificatesMap.levelCount)
            var initialOption
            if(certificatesMap.levelCount === 5){ // Production Card
                console.log("Detected Production Card")
                propertyAcordion.model = [
                            {
                                'entity': certificatesMap.level4.OwnerName,
                                'auth': certificatesMap.level4.IssuerName,
                                'valid': certificatesMap.level4.ValidityBegin,
                                'until':certificatesMap.level4.ValidityEnd,
                                'key': certificatesMap.level4.KeyLength,
                                'status': getCertStatus(certificatesMap.level4.Status),
                                'children': [
                                    {
                                        'entity': certificatesMap.level3.OwnerName,
                                        'auth': certificatesMap.level3.IssuerName,
                                        'valid': certificatesMap.level3.ValidityBegin,
                                        'until':certificatesMap.level3.ValidityEnd,
                                        'key': certificatesMap.level3.KeyLength,
                                        'status': getCertStatus(certificatesMap.level3.Status),
                                        'children': [
                                            {
                                                'entity': certificatesMap.level2.OwnerName,
                                                'auth': certificatesMap.level2.IssuerName,
                                                'valid': certificatesMap.level2.ValidityBegin,
                                                'until':certificatesMap.level2.ValidityEnd,
                                                'key': certificatesMap.level2.KeyLength,
                                                'status': getCertStatus(certificatesMap.level2.Status),
                                                'children': [
                                                    {
                                                        'entity': certificatesMap.level1.OwnerName,
                                                        'auth': certificatesMap.level1.IssuerName,
                                                        'valid': certificatesMap.level1.ValidityBegin,
                                                        'until':certificatesMap.level1.ValidityEnd,
                                                        'key': certificatesMap.level1.KeyLength,
                                                        'status': getCertStatus(certificatesMap.level1.Status),
                                                        'children': [
                                                            {
                                                                'entity': certificatesMap.level0.OwnerName,
                                                                'auth': certificatesMap.level0.IssuerName,
                                                                'valid': certificatesMap.level0.ValidityBegin,
                                                                'until':certificatesMap.level0.ValidityEnd,
                                                                'key': certificatesMap.level0.KeyLength,
                                                                'status': getCertStatus(certificatesMap.level0.Status),
                                                            }
                                                        ]
                                                    },
                                                    {
                                                        'entity': certificatesMap.levelB1.OwnerName,
                                                        'auth': certificatesMap.levelB1.IssuerName,
                                                        'valid': certificatesMap.levelB1.ValidityBegin,
                                                        'until':certificatesMap.levelB1.ValidityEnd,
                                                        'key': certificatesMap.levelB1.KeyLength,
                                                        'status': getCertStatus(certificatesMap.levelB1.Status),
                                                        'children': [
                                                            {
                                                                'entity': certificatesMap.levelB0.OwnerName,
                                                                'auth': certificatesMap.levelB0.IssuerName,
                                                                'valid': certificatesMap.levelB0.ValidityBegin,
                                                                'until':certificatesMap.levelB0.ValidityEnd,
                                                                'key': certificatesMap.levelB0.KeyLength,
                                                                'status': getCertStatus(certificatesMap.levelB0.Status),
                                                            }
                                                        ]
                                                    }
                                                ]
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                // Init Date Field with the signature certificate 
                initialOption = propertyAcordion.model[0].children[0].children[0].children[1].children[0]
                propertyAcordion.selectOption(initialOption)
            }else{
                console.log("test Production Card")
                propertyAcordion.model = [
                            {
                                'entity': certificatesMap.level2.OwnerName,
                                'auth': certificatesMap.level2.IssuerName,
                                'valid': certificatesMap.level2.ValidityBegin,
                                'until':certificatesMap.level2.ValidityEnd,
                                'key': certificatesMap.level2.KeyLength,
                                'status': getCertStatus(certificatesMap.level2.Status),
                                'children': [
                                    {
                                        'entity': certificatesMap.level1.OwnerName,
                                        'auth': certificatesMap.level1.IssuerName,
                                        'valid': certificatesMap.level1.ValidityBegin,
                                        'until':certificatesMap.level1.ValidityEnd,
                                        'key': certificatesMap.level1.KeyLength,
                                        'status': getCertStatus(certificatesMap.level1.Status),
                                        'children': [
                                            {
                                                'entity': certificatesMap.level0.OwnerName,
                                                'auth': certificatesMap.level0.IssuerName,
                                                'valid': certificatesMap.level0.ValidityBegin,
                                                'until':certificatesMap.level0.ValidityEnd,
                                                'key': certificatesMap.level0.KeyLength,
                                                'status': getCertStatus(certificatesMap.level0.Status),
                                            }
                                        ]
                                    },
                                    {
                                        'entity': certificatesMap.levelB1.OwnerName,
                                        'auth': certificatesMap.levelB1.IssuerName,
                                        'valid': certificatesMap.levelB1.ValidityBegin,
                                        'until':certificatesMap.levelB1.ValidityEnd,
                                        'key': certificatesMap.levelB1.KeyLength,
                                        'status': getCertStatus(certificatesMap.levelB1.Status),
                                        'children': [
                                            {
                                                'entity': certificatesMap.levelB0.OwnerName,
                                                'auth': certificatesMap.levelB0.IssuerName,
                                                'valid': certificatesMap.levelB0.ValidityBegin,
                                                'until':certificatesMap.levelB0.ValidityEnd,
                                                'key': certificatesMap.levelB0.KeyLength,
                                                'status': getCertStatus(certificatesMap.levelB0.Status),
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                // Init Date Field with the signature certificate 
                initialOption = propertyAcordion.model[0].children[1].children[0]
                propertyAcordion.selectOption(initialOption)
            }
            propertyBusyIndicator.running = false
            propertyButtonViewCertificate.enabled = true
            propertyButtonExportCertificate.enabled = true
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                propertyRectEntity.forceActiveFocus()
        }
        onSignalExportCertificates: {
            if (success) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("GAPI","STR_POPUP_SUCESS")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("PageSecurityCertificates","STR_EXPORT_CERTIFICATE_SUCCESS")
            }
            else {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("PageSecurityCertificates","STR_EXPORT_CERTIFICATE_FAILED")
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
        }
    }


    propertyButtonViewCertificate {
        onClicked: {
            console.log("View certificate button clicked")
            gapi.viewCardCertificate(propertyAcordion.dataModel.auth, propertyAcordion.dataModel.entity)
        }
    }
    propertyButtonExportCertificate {
        onClicked: {
            console.log("Export certificate button clicked")
            propertyFileDialogOutput.filename = "cert.der"
            propertyFileDialogOutput.open()
        }
    }
    propertyFileDialogOutput {
        onAccepted: {
            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            outputFile = decodeURIComponent(Functions.stripFilePrefix(outputFile))
            gapi.exportCardCertificate(propertyAcordion.dataModel.auth, propertyAcordion.dataModel.entity, outputFile)
        }
    }
    Component.onCompleted: {
        console.log("Page Security Certificates Completed")
        propertyBusyIndicator.running = true

        if (Qt.platform.os === "windows") {
            propertyButtonViewCertificate.visible = true
        }

        gapi.startfillCertificateList()
    }
    function getCertStatus(certStatus){

        var strCertStatus = qsTr("STR_STATUS_UNKNOWN");

        switch(certStatus)
        {
        case Constants.PTEID_CERTIF_STATUS_UNKNOWN:
            strCertStatus = qsTr("STR_STATUS_UNKNOWN")
            break;
        case Constants.PTEID_CERTIF_STATUS_REVOKED:
            strCertStatus = qsTr("STR_STATUS_REVOKED")
            break;
        case Constants.PTEID_CERTIF_STATUS_SUSPENDED:
            strCertStatus = qsTr("STR_STATUS_SUSPENDED")
            break;
        case Constants.PTEID_CERTIF_STATUS_CONNECT:
            strCertStatus = qsTr("STR_STATUS_NETWORK_ERROR")
            break;
        case Constants.PTEID_CERTIF_STATUS_ISSUER:
            strCertStatus = qsTr("STR_STATUS_ISSUER")
            break;
        case Constants.PTEID_CERTIF_STATUS_ERROR:
            strCertStatus = qsTr("STR_STATUS_ERROR")
            break;
        case Constants.PTEID_CERTIF_STATUS_VALID:
            strCertStatus = qsTr("STR_STATUS_VALID")
            break;
        case Constants.PTEID_CERTIF_STATUS_EXPIRED:
            strCertStatus = qsTr("STR_STATUS_EXPIRED")
            break;
        default:
            strCertStatus = qsTr("STR_STATUS_UNKNOWN");
            break;
        }

        return strCertStatus
    }
    function clearFields(){
        propertyButtonViewCertificate.enabled = false
        propertyButtonExportCertificate.enabled = false
        for (var i = 0; i < propertyRectBottom.children.length; i++){
            propertyAcordion.model = []
            propertyRectBottom.children[i].children[0].propertyDateField.text = ""
        }
    }
}
