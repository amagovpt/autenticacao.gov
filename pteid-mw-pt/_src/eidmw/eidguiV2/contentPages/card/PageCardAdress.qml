/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

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

            propertyButtonConfirmOfAddress.enabled = true

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
            textMessageTop.propertyText.forceActiveFocus()
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
            }
            else if (error_code == GAPI.CardUserPinCancel) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
            }
            else if (error_code == GAPI.CardPinTimeout) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
            }
            propertyButtonConfirmOfAddress.enabled = false
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
        }
        onSignalCardChanged: {
            console.log("Card Adress onSignalCardChanged")
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
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                if(Constants.USE_SDK_PIN_UI_POPUP){
                    gapi.verifyAddressPin("")
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
            if (gapi.isAddressLoaded) {
                console.debug("Address is already loaded! Hiding pin dialog...")
                dialogTestPin.visible = false
                propertyBusyIndicator.running = true
                gapi.startReadingAddress()
                return;
            }

            var triesLeft = gapi.doVerifyAddressPin(textFieldPin.text)
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

    Dialog {
        id: dialogConfirmOfAddress
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogConfirmOfAddress.width * 0.5
        y: parent.height * 0.5 - dialogConfirmOfAddress.height * 0.5
        modal: true
        focus: true

        header: Label {
            id: labelConfirmOfAddressTextTitle
            text: qsTr("STR_ADDRESS_CHANGE_CONFIRMATION")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: rectPopUp.activeFocus ? true : false
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            id: rectPopUp
            width: parent.width
            height: rectMessage.height + rectNumProcess.height + rectConfirmAddress.height

            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS")
                             + labelConfirmOfAddressTextTitle.text
            KeyNavigation.tab: textPinMsgConfirm.propertyText
            KeyNavigation.down: textPinMsgConfirm.propertyText
            KeyNavigation.right: textPinMsgConfirm.propertyText
            KeyNavigation.backtab: okButton
            KeyNavigation.up: okButton

            Item {
                id: rectMessage
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Components.Link {
                    id: textPinMsgConfirm
                    propertyText.text: qsTr("STR_ADDRESS_CHANGE_TEXT") + " "
                          + "<a href='https://eportugal.gov.pt/pt/servicos/alterar-a-morada-do-cartao-de-cidadao'>"
                          + qsTr("STR_ADDRESS_CHANGE_TEXT_LINK")
                    propertyText.verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    propertyText.font.pixelSize: Constants.SIZE_TEXT_LABEL
                    anchors.fill: parent 
                    propertyText.anchors.fill: textPinMsgConfirm
                    propertyAccessibleText: qsTr("STR_ADDRESS_CHANGE_TEXT") + 'https://eportugal.gov.pt/pt/servicos/alterar-a-morada-do-cartao-de-cidadao'
                    propertyLinkUrl: 'https://eportugal.gov.pt/pt/servicos/alterar-a-morada-do-cartao-de-cidadao'
                    KeyNavigation.tab: textPinCurrent
                    KeyNavigation.down: textPinCurrent
                    KeyNavigation.right: textPinCurrent
                    KeyNavigation.backtab: rectPopUp
                    KeyNavigation.up: rectPopUp
                }
            }

            Item {
                id: rectNumProcess
                width: parent.width
                height: 50
                anchors.top: rectMessage.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinCurrent
                    text:  qsTr("STR_ADDRESS_CHANGE_NUMBER")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus ? true : false
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom

                    Accessible.role: Accessible.StaticText
                    Accessible.name: text
                    KeyNavigation.tab: textFieldNumProcess
                    KeyNavigation.down: textFieldNumProcess
                    KeyNavigation.right: textFieldNumProcess
                    KeyNavigation.backtab: textPinMsgConfirm.propertyText
                    KeyNavigation.up: textPinMsgConfirm.propertyText
                }
                TextField {
                    id: textFieldNumProcess
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldNumProcess.text === "" ? true: false
                    placeholderText: qsTr("STR_ADDRESS_CHANGE_NUMBER_OP")
                    font.bold: activeFocus ? true : false
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinCurrent.right
                    anchors.bottom: parent.bottom
                    focus: true

                    Accessible.role: Accessible.EditableText
                    Accessible.name: placeholderText
                    KeyNavigation.tab: textPinNew
                    KeyNavigation.down: textPinNew
                    KeyNavigation.right: textPinNew
                    KeyNavigation.backtab: textPinCurrent
                    KeyNavigation.up: textPinCurrent
                }
            }
            Item {
                id: rectConfirmAddress
                width: parent.width
                height: 50
                anchors.top: rectNumProcess.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinNew
                    text: qsTr("STR_ADDRESS_CHANGE_CODE")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: activeFocus ? true : false
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom

                    Accessible.role: Accessible.StaticText
                    Accessible.name: text
                    KeyNavigation.tab: textFieldConfirmAddress
                    KeyNavigation.down: textFieldConfirmAddress
                    KeyNavigation.right: textFieldConfirmAddress
                    KeyNavigation.backtab: textFieldNumProcess
                    KeyNavigation.up: textFieldNumProcess
                }
                TextField {
                    id: textFieldConfirmAddress
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldConfirmAddress.text === "" ? true: false
                    placeholderText: qsTr("STR_ADDRESS_CHANGE_CODE_OP")
                    font.bold: activeFocus ? true : false
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinNew.right
                    anchors.bottom: parent.bottom

                    Accessible.role: Accessible.EditableText
                    Accessible.name: placeholderText
                    KeyNavigation.tab: cancelButton
                    KeyNavigation.down: cancelButton
                    KeyNavigation.right: cancelButton
                    KeyNavigation.backtab: textPinNew
                    KeyNavigation.up: textPinNew
                }
            }
        }

        Item {
            width: dialogConfirmOfAddress.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.horizontalCenter: parent.horizontalCenter
            y: 180
            Button {
                id: cancelButton
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTr("STR_ADDRESS_CHANGE_CANCEL")
                anchors.left: parent.left
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus ? true : false
                onClicked: {
                    dialogConfirmOfAddress.close()
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    mainFormID.propertyPageLoader.forceActiveFocus()
                }
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: okButton
                KeyNavigation.down: okButton
                KeyNavigation.right: okButton
                KeyNavigation.backtab: textFieldConfirmAddress
                KeyNavigation.up: textFieldConfirmAddress
            }
            Button {
                id: okButton
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTr("STR_ADDRESS_CHANGE_CONFIRM")
                anchors.right: parent.right
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                enabled: textFieldNumProcess.length !== 0 && textFieldConfirmAddress.length !== 0 ? true : false
                highlighted: activeFocus ? true : false
                onClicked: {
                    gapi.changeAddress(textFieldNumProcess.text,textFieldConfirmAddress.text)
                    progressBarIndeterminate.visible = true
                    textFieldNumProcess.text = ""
                    textFieldConfirmAddress.text = ""
                    dialogConfirmOfAddress.close()
                    dialogConfirmOfAddressProgress.open()
                }
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: rectPopUp
                KeyNavigation.down: rectPopUp
                KeyNavigation.right: rectPopUp
                KeyNavigation.backtab: cancelButton
                KeyNavigation.up: cancelButton
            }
        }
        onRejected:{
            // Reject address change Popup's only with ESC key
            dialogConfirmOfAddress.open()
        }
        onOpened: {
            rectPopUp.forceActiveFocus()
        }
    }

    Dialog {
        id: dialogConfirmOfAddressProgress
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogConfirmOfAddress.width * 0.5
        y: parent.height * 0.5 - dialogConfirmOfAddress.height * 0.5
        modal: true
        focus: true

        header: Label {
            id: labelConfirmOfAddressProgressTextTitle
            text: qsTr("STR_ADDRESS_CHANGE_CONFIRMATION")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: activeFocus ? true : false
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS")
                             + labelConfirmOfAddressProgressTextTitle.text
            KeyNavigation.tab: textMessageTop.propertyText
            KeyNavigation.down: textMessageTop.propertyText
            KeyNavigation.right: textMessageTop.propertyText
            KeyNavigation.backtab: okButton
            KeyNavigation.up: okButton
        }

        Item {
            width: parent.width
            height: rectMessageTop.height + progressBar.height + progressBarIndeterminate.height

            Item {
                id: rectMessageTop
                width: parent.width
                height: 150
                anchors.horizontalCenter: parent.horizontalCenter
                Components.Link {
                    id: textMessageTop
                    propertyText.text: ""
                    propertyText.verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    propertyText.font.pixelSize: Constants.SIZE_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    propertyText.height: parent.height
                    anchors.bottom: parent.bottom
                    propertyAccessibleText: Functions.filterText(textMessageTop.propertyText.text)
                    KeyNavigation.tab: confirmOkButton
                    KeyNavigation.down: confirmOkButton
                    KeyNavigation.right: confirmOkButton
                    KeyNavigation.backtab: rectPopUp
                    KeyNavigation.up: rectPopUp
                }
            }

            ProgressBar {
                id: progressBar
                width: parent.width * 0.5
                anchors.top: rectMessageTop.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                height: 20
                to: 100
                value: 0
                visible: true
                indeterminate: false
                z:1
            }

            ProgressBar {
                id: progressBarIndeterminate
                width: parent.width * 0.5
                anchors.top: progressBar.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                height: 20
                to: 100
                value: 0
                visible: true
                indeterminate: true
                z:1
            }
        }

        Item {
            width: dialogConfirmOfAddress.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.horizontalCenter: parent.horizontalCenter
            y: 180

            Button {
                id: confirmOkButton
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTr("STR_ADDRESS_CHANGE_OK")
                anchors.right: parent.right
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                visible: progressBarIndeterminate.visible ? false : true
                onClicked: {
                    dialogConfirmOfAddressProgress.close()
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    mainFormID.propertyPageLoader.forceActiveFocus()
                }
                highlighted: activeFocus ? true : false
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: labelConfirmOfAddressProgressTextTitle
                KeyNavigation.down: labelConfirmOfAddressProgressTextTitle
                KeyNavigation.right: textMessageTop.propertyText
                KeyNavigation.backtab: textMessageTop.propertyText
                KeyNavigation.up: textMessageTop.propertyText
            }
        }
        onRejected:{
            // Reject address change Popup's only with ESC key
            dialogConfirmOfAddressProgress.open()
        }
        onOpened: {
            labelConfirmOfAddressProgressTextTitle.forceActiveFocus()
        }
    }

    propertyButtonConfirmOfAddress{
        onClicked: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            dialogConfirmOfAddress.open()
        }

    }

    Component.onCompleted: {
        console.log("Page Card Address Completed")
        if (gapi.isAddressLoaded) {
            console.log("Page Card Address isAddressLoaded")
            propertyBusyIndicator.running = true
            gapi.startReadingAddress()
        }else{
            if(Constants.USE_SDK_PIN_UI_POPUP){
                if (gapi.doGetTriesLeftAddressPin() === 0) {
                    var titlePopup = qsTranslate("Popup PIN","STR_POPUP_ERROR")
                    var bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_ADDRESS_BLOCKED")
                    mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
                }else{
                    gapi.verifyAddressPin("")
                }
            }else{
                dialogTestPin.open()
                textFieldPin.text = ""
            }
        }
    }
}
