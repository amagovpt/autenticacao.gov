/*-****************************************************************************

 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

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
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = ""
            if (error_code == GAPI.NoReaderFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
            }
            else if (error_code == GAPI.NoCardFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD")
            }
            else if (error_code == GAPI.SodCardReadError) {
                bodyPopup = qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
                    + "<br/><br/>" + qsTranslate("Popup Card","STR_GENERIC_CARD_ERROR_MSG")
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
                clearFields()
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startfillCertificateList()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                clearFields()
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
        }
        onSignalCertificatesFail: {
            propertyBusyIndicator.running = false
            propertyButtonViewCertificate.enabled = false
            propertyButtonExportCertificate.enabled = false

            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = qsTranslate("PageSecurityCertificates","STR_CERT_CHAIN_ERROR")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
        }

        onSignalContactlessCANNeeded: {
            console.log("QML: Contactless CAN is needed!!")
            paceDialogLoader.active = true
        }

        onSignalCertificatesChanged: {
            console.log("Certificate chain levels: "+ certificatesMap.levelCount)
            var initialOption
            if(certificatesMap.levelCount === 5){
                propertyAcordion.model = [
                            {
                                'entity': certificatesMap.level4.OwnerName,
                                'auth': certificatesMap.level4.IssuerName,
                                'valid': certificatesMap.level4.ValidityBegin,
                                'until':certificatesMap.level4.ValidityEnd,
                                'key': certificatesMap.level4.KeyLength,
                                'serial': certificatesMap.level4.SerialNumber,
                                'status': getCertStatus(certificatesMap.level4.Status),
                                'children': [
                                    {
                                        'entity': certificatesMap.level3.OwnerName,
                                        'auth': certificatesMap.level3.IssuerName,
                                        'valid': certificatesMap.level3.ValidityBegin,
                                        'until':certificatesMap.level3.ValidityEnd,
                                        'key': certificatesMap.level3.KeyLength,
                                        'serial': certificatesMap.level3.SerialNumber,
                                        'status': getCertStatus(certificatesMap.level3.Status),
                                        'children': [
                                            {
                                                'entity': certificatesMap.level2.OwnerName,
                                                'auth': certificatesMap.level2.IssuerName,
                                                'valid': certificatesMap.level2.ValidityBegin,
                                                'until':certificatesMap.level2.ValidityEnd,
                                                'key': certificatesMap.level2.KeyLength,
                                                'serial': certificatesMap.level2.SerialNumber,
                                                'status': getCertStatus(certificatesMap.level2.Status),
                                                'children': [
                                                    {
                                                        'entity': certificatesMap.level1.OwnerName,
                                                        'auth': certificatesMap.level1.IssuerName,
                                                        'valid': certificatesMap.level1.ValidityBegin,
                                                        'until':certificatesMap.level1.ValidityEnd,
                                                        'key': certificatesMap.level1.KeyLength,
                                                        'serial': certificatesMap.level1.SerialNumber,
                                                        'status': getCertStatus(certificatesMap.level1.Status),
                                                        'children': [
                                                            {
                                                                'entity': certificatesMap.level0.OwnerName,
                                                                'auth': certificatesMap.level0.IssuerName,
                                                                'valid': certificatesMap.level0.ValidityBegin,
                                                                'until':certificatesMap.level0.ValidityEnd,
                                                                'key': certificatesMap.level0.KeyLength,
                                                                'serial': certificatesMap.level0.SerialNumber,
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
                                                        'serial': certificatesMap.levelB1.SerialNumber,
                                                        'status': getCertStatus(certificatesMap.levelB1.Status),
                                                        'children': [
                                                            {
                                                                'entity': certificatesMap.levelB0.OwnerName,
                                                                'auth': certificatesMap.levelB0.IssuerName,
                                                                'valid': certificatesMap.levelB0.ValidityBegin,
                                                                'until':certificatesMap.levelB0.ValidityEnd,
                                                                'key': certificatesMap.levelB0.KeyLength,
                                                                'serial': certificatesMap.levelB0.SerialNumber,
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
            } else if(certificatesMap.levelCount === 4){
                propertyAcordion.model = [
                            {
                                'entity': certificatesMap.level3.OwnerName,
                                'auth': certificatesMap.level3.IssuerName,
                                'valid': certificatesMap.level3.ValidityBegin,
                                'until':certificatesMap.level3.ValidityEnd,
                                'key': certificatesMap.level3.KeyLength,
                                'serial': certificatesMap.level3.SerialNumber,
                                'status': getCertStatus(certificatesMap.level3.Status),
                                'children': [
                                    {
                                        'entity': certificatesMap.level2.OwnerName,
                                        'auth': certificatesMap.level2.IssuerName,
                                        'valid': certificatesMap.level2.ValidityBegin,
                                        'until':certificatesMap.level2.ValidityEnd,
                                        'key': certificatesMap.level2.KeyLength,
                                        'serial': certificatesMap.level2.SerialNumber,
                                        'status': getCertStatus(certificatesMap.level2.Status),
                                        'children': [
                                            {
                                                'entity': certificatesMap.level1.OwnerName,
                                                'auth': certificatesMap.level1.IssuerName,
                                                'valid': certificatesMap.level1.ValidityBegin,
                                                'until':certificatesMap.level1.ValidityEnd,
                                                'key': certificatesMap.level1.KeyLength,
                                                'serial': certificatesMap.level1.SerialNumber,
                                                'status': getCertStatus(certificatesMap.level1.Status),
                                                'children': [
                                                    {
                                                        'entity': certificatesMap.level0.OwnerName,
                                                        'auth': certificatesMap.level0.IssuerName,
                                                        'valid': certificatesMap.level0.ValidityBegin,
                                                        'until':certificatesMap.level0.ValidityEnd,
                                                        'key': certificatesMap.level0.KeyLength,
                                                        'serial': certificatesMap.level0.SerialNumber,
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
                                                'serial': certificatesMap.levelB1.SerialNumber,
                                                'status': getCertStatus(certificatesMap.levelB1.Status),
                                                'children': [
                                                    {
                                                        'entity': certificatesMap.levelB0.OwnerName,
                                                        'auth': certificatesMap.levelB0.IssuerName,
                                                        'valid': certificatesMap.levelB0.ValidityBegin,
                                                        'until':certificatesMap.levelB0.ValidityEnd,
                                                        'key': certificatesMap.levelB0.KeyLength,
                                                        'serial': certificatesMap.levelB0.SerialNumber,
                                                        'status': getCertStatus(certificatesMap.levelB0.Status),
                                                    }
                                                ]
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                // Init Date Field with the signature certificate
                initialOption = propertyAcordion.model[0].children[0].children[1].children[0]
            } else if(certificatesMap.levelCount === 3){
                propertyAcordion.model = [
                            {
                                'entity': certificatesMap.level2.OwnerName,
                                'auth': certificatesMap.level2.IssuerName,
                                'valid': certificatesMap.level2.ValidityBegin,
                                'until':certificatesMap.level2.ValidityEnd,
                                'key': certificatesMap.level2.KeyLength,
                                'serial': certificatesMap.level2.SerialNumber,
                                'status': getCertStatus(certificatesMap.level2.Status),
                                'children': [
                                    {
                                        'entity': certificatesMap.level1.OwnerName,
                                        'auth': certificatesMap.level1.IssuerName,
                                        'valid': certificatesMap.level1.ValidityBegin,
                                        'until':certificatesMap.level1.ValidityEnd,
                                        'key': certificatesMap.level1.KeyLength,
                                        'serial': certificatesMap.level1.SerialNumber,
                                        'status': getCertStatus(certificatesMap.level1.Status),
                                        'children': [
                                            {
                                                'entity': certificatesMap.level0.OwnerName,
                                                'auth': certificatesMap.level0.IssuerName,
                                                'valid': certificatesMap.level0.ValidityBegin,
                                                'until':certificatesMap.level0.ValidityEnd,
                                                'key': certificatesMap.level0.KeyLength,
                                                'serial': certificatesMap.level0.SerialNumber,
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
                                        'serial': certificatesMap.levelB1.SerialNumber,
                                        'status': getCertStatus(certificatesMap.levelB1.Status),
                                        'children': [
                                            {
                                                'entity': certificatesMap.levelB0.OwnerName,
                                                'auth': certificatesMap.levelB0.IssuerName,
                                                'valid': certificatesMap.levelB0.ValidityBegin,
                                                'until':certificatesMap.levelB0.ValidityEnd,
                                                'key': certificatesMap.levelB0.KeyLength,
                                                'serial': certificatesMap.levelB0.SerialNumber,
                                                'status': getCertStatus(certificatesMap.levelB0.Status),
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                // Init Date Field with the signature certificate
                initialOption = propertyAcordion.model[0].children[1].children[0]
            } else {
                propertyBusyIndicator.running = false
                propertyButtonViewCertificate.enabled = false
                propertyButtonExportCertificate.enabled = false
                var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                var bodyPopup = qsTranslate("PageSecurityCertificates","STR_CERT_CHAIN_ERROR")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
                return
            }

            propertyAcordion.selectOption(initialOption)
            propertyBusyIndicator.running = false
            propertyButtonViewCertificate.enabled = true
            propertyButtonExportCertificate.enabled = true
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                propertyRectEntity.forceActiveFocus()
        }
        onSignalExportCertificates: {
            if (success) {
                var titlePopup = qsTranslate("GAPI","STR_POPUP_SUCESS")
                var bodyPopup = qsTranslate("PageSecurityCertificates","STR_EXPORT_CERTIFICATE_SUCCESS")
            }
            else {
                var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                var bodyPopup = qsTranslate("PageSecurityCertificates","STR_EXPORT_CERTIFICATE_FAILED")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }

        onSignalErrorPace: {
            if(error_code === GAPI.PaceBadToken){
                console.log("QML: The stored CAN is wrong!! Will ask again!!")
                paceDialogLoader.active = true
            }
        }
    }

    Connections {
        target: paceDialogLoader.status === Loader.Ready ? paceDialogLoader.item : null
        onClosed: {
            paceDialogLoader.active = false
        }
    }

    Loader {
        id: paceDialogLoader
        active: false
        anchors.verticalCenter: parent.verticalCenter
        source: "../definitions/DialogRequestPaceSecret.qml"
        onLoaded: {
            paceDialogLoader.item.afterPaceAction = GAPI.ReadCertDetails
            paceDialogLoader.item.open()
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
            propertyFileDialogOutput.currentFile = propertyFileDialogOutput.folder + "/cert.der"
            propertyFileDialogOutput.open()
        }
    }

    propertyButtonLessCertificateDetails {
        onClicked: {
            mainFormID.propertyPageLoader.propertyForceFocus = false
            mainFormID.propertyPageLoader.source = "../../contentPages/security/PageSecurityCertificateState.qml"
        }
    }

    propertyFileDialogOutput {
        onAccepted: {
            var outputFile = propertyFileDialogOutput.file.toString()
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
        propertyAcordion.model = []
        for (var i = 0; i < propertyRectBottom.children.length; i++){
            var child = propertyRectBottom.children[i].children[0]
            if (child.type == "LabelTextBox"){
                child.propertyDateField.text = ""
            }
        }
    }
    function handleKeyPressed(key, callingObject){
        var direction = getDirection(key)
        switch(direction){
            case Constants.DIRECTION_UP:
                if(callingObject === propertyRectEntity && !propertyRowMain.atYEnd){
                    propertyRowMain.flick(0, - getMaxFlickVelocity())
                }
                else if(!propertyRowMain.atYBeginning)
                    propertyRowMain.flick(0, Constants.FLICK_Y_VELOCITY)
                break;

            case Constants.DIRECTION_DOWN:
                if(callingObject === propertyButtonExportCertificate && !propertyRowMain.atYBeginning){
                    propertyRowMain.flick(0, getMaxFlickVelocity());
                }
                else if(!propertyRowMain.atYEnd)
                    propertyRowMain.flick(0, - Constants.FLICK_Y_VELOCITY)
                break;
        }
    }
    function getDirection(key){
        var direction = Constants.NO_DIRECTION;
        if (key == Qt.Key_Backtab || key == Qt.Key_Up || key == Qt.Key_Left){
            direction = Constants.DIRECTION_UP;
        }
        else if (key == Qt.Key_Tab || key == Qt.Key_Down || key == Qt.Key_Right){
            direction = Constants.DIRECTION_DOWN;
        }
        return direction;
    }
    function getMaxFlickVelocity(){
        // use visible area of flickable object to calculate
        // a smooth flick velocity
        return 200 + Constants.FLICK_Y_VELOCITY_MAX * (1 - propertyRowMain.visibleArea.heightRatio)
    }
}
