/*-****************************************************************************

 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

import "../../components" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageCardAdressForm {

    Keys.onPressed: {
        console.log("PageCardAdressForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
        onSignalAddressLoaded: {
            console.log("Address --> Data Changed")
            if(m_foreign){
                propertyItemForeignCitizen.visible = true
                propertyItemNationalCitizen.visible = false
                propertyForeignCountry.propertyDateField.text = gapi.getAddressField(GAPI.Foreigncountry)
                propertyForeignAddress.propertyDateField.text = gapi.getAddressField(GAPI.Foreignaddress)
                propertyForeignCity.propertyDateField.text = gapi.getAddressField(GAPI.Foreigncity)
                propertyForeignRegion.propertyDateField.text = gapi.getAddressField(GAPI.Foreignregion)
                propertyForeignLocality.propertyDateField.text = gapi.getAddressField(GAPI.Foreignlocality)
                propertyForeignPostalCode.propertyDateField.text = gapi.getAddressField(GAPI.Foreignpostalcode)
            }else{
                propertyItemNationalCitizen.visible = true
                propertyItemForeignCitizen.visible = false
                propertyDistrict.propertyDateField.text = gapi.getAddressField(GAPI.District)
                propertyMunicipality.propertyDateField.text = gapi.getAddressField(GAPI.Municipality)
                propertyParish.propertyDateField.text = gapi.getAddressField(GAPI.Parish)
                propertyStreetType.propertyDateField.text = gapi.getAddressField(GAPI.Streettype)
                propertyStreetName.propertyDateField.text = gapi.getAddressField(GAPI.Streetname)
                propertyDoorNo.propertyDateField.text = gapi.getAddressField(GAPI.Doorno)
                propertyFloor.propertyDateField.text = gapi.getAddressField(GAPI.Floor)
                propertyPlace.propertyDateField.text = gapi.getAddressField(GAPI.Place)
                propertySide.propertyDateField.text = gapi.getAddressField(GAPI.Side)
                propertyLocality.propertyDateField.text = gapi.getAddressField(GAPI.Locality)
                propertyZip4.propertyDateField.text = gapi.getAddressField(GAPI.Zip4)
                propertyZip3.propertyDateField.text = gapi.getAddressField(GAPI.Zip3)
                propertyPostalLocality.propertyDateField.text = gapi.getAddressField(GAPI.PostalLocality)
            }

            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS

            gapi.setAddressLoaded(true)

            if(!Constants.USE_SDK_PIN_UI_POPUP)
                dialogTestPin.visible = false
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                propertyRectNacionalDistrict.forceActiveFocus()
        }
        onSignalUpdateProgressBar: {
            console.log("Address change --> update progress bar with value = " + value)
            progressBar.value = value
            if(value === 100) {
                progressBar.visible = false
                progressBarIndeterminate.visible = false
            }
        }
        onSignalUpdateProgressStatus: {
            console.log("Address change --> update progress status with text = " + statusMessage)            
            textMessageTop.propertyText.text = statusMessage
            textMessageTop.propertyAccessibleText = Functions.filterText(statusMessage)
            textMessageTop.propertyText.forceActiveFocus()
        }

        onSignalRemoteAddressError: {
            console.log("Card Address onSignalRemoteAddressError: "+ error_code)
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = ""

            if (error_code == GAPI.AddressConnectionError) {
                bodyPopup = qsTr("STR_REMOTEADDRESS_NETWORK_ERROR")
                    + "<br/><br/>" + qsTranslate("GAPI","STR_VERIFY_PROXY")
                    + "<br/><br/>" + qsTr("STR_REMOTEADDRESS_GENERIC")
            }
            else if (error_code == GAPI.AddressServerError) {
                bodyPopup = qsTr("STR_REMOTEADDRESS_SERVER_ERROR")
                    + "<br/><br/>" + qsTr("STR_REMOTEADDRESS_GENERIC")
            }
            else if (error_code == GAPI.AddressConnectionTimeout) {
                bodyPopup = qsTr("STR_REMOTEADDRESS_TIMEOUT_ERROR")
                    + "<br/><br/>" + qsTr("STR_REMOTEADDRESS_GENERIC")
            }
            else if (error_code == GAPI.AddressSmartcardError) {
                bodyPopup = qsTr("STR_REMOTEADDRESS_SMARTCARD_ERROR")
                    + "<br/><br/>" + qsTr("STR_REMOTEADDRESS_GENERIC")
            }
            else if (error_code == GAPI.AddressServerCertificateError) {
                bodyPopup = qsTranslate("GAPI", "STR_CERTIFICATE_ERROR")
                    + qsTranslate("GAPI", "STR_CERTIFICATE_ERROR_READ_ADDRESS")
                    + "<br/>" + qsTr("STR_REMOTEADDRESS_GENERIC")
            }
            else if (error_code == GAPI.CardCertificateError) {
                bodyPopup = qsTr("STR_REMOTEADDRESS_CARDCERTIFICATE_ERROR")
            }
            else if (error_code == GAPI.AddressInvalidStateError) {
                bodyPopup = qsTr("STR_REMOTEADDRESS_INVALIDSTATE_ERROR")
            }
            else if (error_code == GAPI.AddressUnknownError) {
                bodyPopup = qsTr("STR_REMOTEADDRESS_UNKNOWN_ERROR")
                    + "<br/><br/>" + qsTr("STR_REMOTEADDRESS_GENERIC")
            }
            else {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)

            propertyDistrict.propertyDateField.text = ""
            propertyMunicipality.propertyDateField.text = ""
            propertyParish.propertyDateField.text = ""
            propertyStreetType.propertyDateField.text = ""
            propertyStreetName.propertyDateField.text = ""
            propertyDoorNo.propertyDateField.text = ""
            propertyFloor.propertyDateField.text = ""
            propertyPlace.propertyDateField.text = ""
            propertySide.propertyDateField.text = ""
            propertyLocality.propertyDateField.text = ""
            propertyZip4.propertyDateField.text = ""
            propertyZip3.propertyDateField.text = ""
            propertyPostalLocality.propertyDateField.text = ""
            propertyForeignCountry.propertyDateField.text = ""
            propertyForeignAddress.propertyDateField.text = ""
            propertyForeignCity.propertyDateField.text = ""
            propertyForeignRegion.propertyDateField.text = ""
            propertyForeignLocality.propertyDateField.text = ""
            propertyForeignPostalCode.propertyDateField.text = ""
            propertyBusyIndicator.running = false

            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }

        onSignalCardAccessError: {
            console.log("Card Adress onSignalCardAccessError"+ error_code)

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

            propertyDistrict.propertyDateField.text = ""
            propertyMunicipality.propertyDateField.text = ""
            propertyParish.propertyDateField.text = ""
            propertyStreetType.propertyDateField.text = ""
            propertyStreetName.propertyDateField.text = ""
            propertyDoorNo.propertyDateField.text = ""
            propertyFloor.propertyDateField.text = ""
            propertyPlace.propertyDateField.text = ""
            propertySide.propertyDateField.text = ""
            propertyLocality.propertyDateField.text = ""
            propertyZip4.propertyDateField.text = ""
            propertyZip3.propertyDateField.text = ""
            propertyPostalLocality.propertyDateField.text = ""
            propertyForeignCountry.propertyDateField.text = ""
            propertyForeignAddress.propertyDateField.text = ""
            propertyForeignCity.propertyDateField.text = ""
            propertyForeignRegion.propertyDateField.text = ""
            propertyForeignLocality.propertyDateField.text = ""
            propertyForeignPostalCode.propertyDateField.text = ""
            propertyBusyIndicator.running = false

            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }

        onSignalCardChanged: {
            console.log("Card Address onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                var bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)

                propertyDistrict.propertyDateField.text = ""
                propertyMunicipality.propertyDateField.text = ""
                propertyParish.propertyDateField.text = ""
                propertyStreetType.propertyDateField.text = ""
                propertyStreetName.propertyDateField.text = ""
                propertyDoorNo.propertyDateField.text = ""
                propertyFloor.propertyDateField.text = ""
                propertyPlace.propertyDateField.text = ""
                propertySide.propertyDateField.text = ""
                propertyLocality.propertyDateField.text = ""
                propertyZip4.propertyDateField.text = ""
                propertyZip3.propertyDateField.text = ""
                propertyPostalLocality.propertyDateField.text = ""
                propertyForeignCountry.propertyDateField.text = ""
                propertyForeignAddress.propertyDateField.text = ""
                propertyForeignCity.propertyDateField.text = ""
                propertyForeignRegion.propertyDateField.text = ""
                propertyForeignLocality.propertyDateField.text = ""
                propertyForeignPostalCode.propertyDateField.text = ""
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                if(Constants.USE_SDK_PIN_UI_POPUP){
                    mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                    gapi.verifyAddressPin("", false)
                }else{
                    dialogTestPin.open()
                    textFieldPin.text = ""
                }
            }
            else{
                var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                var bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
            }
        }
        onSignalTestPinFinished: {
            if (triesLeft === 3) {
                propertyBusyIndicator.running = true
                gapi.startReadingAddress()
            }
        }

        onSignalContactlessCANNeeded: {
            console.log("QML: Contactless CAN is needed!!")
            paceDialogLoader.active = true
        }
    }

    Loader {
        id: paceDialogLoader
        active: false
        anchors.verticalCenter: parent.verticalCenter
        source: "../definitions/DialogRequestPaceSecret.qml"
        onLoaded: {
            paceDialogLoader.item.afterPaceAction = GAPI.DoAddress
            paceDialogLoader.item.open()
        }
    }

    Dialog {
        id: dialogBadPin
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogBadPin.width * 0.5
        y: parent.height * 0.5 - dialogBadPin.height * 0.5

        header: Label {
            text: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectBadPin.height

            Item {
                id: rectBadPin
                width: textPin.width + textFieldPin.width
                height: 50

                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textBadPin
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: 150
                    anchors.bottom: parent.bottom
                }
            }
        }
        standardButtons: DialogButtonBox.Retry | DialogButtonBox.Cancel

        onAccepted: {
            textFieldPin.text = ""
            dialogTestPin.open()
        }
    }

    Dialog {
        id: dialogTestPin
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogTestPin.width * 0.5
        y: parent.height * 0.5 - dialogTestPin.height * 0.5

        header: Label {
            text: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY_ADDRESS")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectPin.height

            Item {
                id: rectPin
                width: parent.width
                height: 50

                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPin
                    text: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_ADDRESS")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldPin
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPin.text === "" ? true: false
                    placeholderText: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_CURRENT")
                    echoMode : TextInput.Password
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    maximumLength: 8
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPin.right
                    anchors.bottom: parent.bottom
                }
            }
        }
        standardButtons: {
            textFieldPin.length >= 4 ? DialogButtonBox.Ok | DialogButtonBox.Cancel : DialogButtonBox.Cancel
        }

        onAccepted: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            var triesLeft = gapi.doVerifyAddressPin(textFieldPin.text, true)
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            if (triesLeft === 3) {
                propertyBusyIndicator.running = true
                gapi.startReadingAddress()
            }
            else if (triesLeft === 0) {
                textBadPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_ADDRESS_BLOCKED")
                dialogBadPin.open()
            }
            else {
                textBadPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_WRONG")
                        + " \n\n"
                        + qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                        + " " + triesLeft + " "
                        + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                dialogBadPin.open()
            }
        }
        onRejected: {
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
    }

    Component.onCompleted: {
        console.log("Page Card Address Completed")
        
        if(Constants.USE_SDK_PIN_UI_POPUP) {
            if (gapi.doGetTriesLeftAddressPin() === 0) {
                var titlePopup = qsTranslate("Popup PIN","STR_POPUP_ERROR")
                var bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_ADDRESS_BLOCKED")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
            }
            else {
                propertyBusyIndicator.running = true
                mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                gapi.verifyAddressPin("", false)
            }
        }
        else 
        {
            dialogTestPin.open()
            textFieldPin.text = ""

        }
    }
}
