import QtQuick 2.6
import QtQuick.Controls 2.1
import "../../scripts/Constants.js" as Constants
//Import C++ defined enums
import eidguiV2 1.0

PageCardAdressForm {

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
            textMessageTop.text = statusMessage
        }
        onSignalCardAccessError: {
            console.log("Card Adress onSignalCardAccessError"+ error_code)
            
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
            propertyButtonConfirmOfAddress.enabled = false
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            
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
                    var triesLeft = gapi.verifyAddressPin("")
                    if (triesLeft === 3) {
                        propertyBusyIndicator.running = true
                        gapi.startReadingAddress()
                    }
                }else{
                    dialogTestPin.open()
                    textFieldPin.text = ""
                }
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
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

            var triesLeft = gapi.verifyAddressPin(textFieldPin.text)
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

        header: Label {
            id: labelConfirmOfAddressTextTitle
            text: qsTr("STR_ADDRESS_CHANGE_CONFIRMATION")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectMessage.height + rectNumProcess.height + rectConfirmAddress.height

            Item {
                id: rectMessage
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinMsgConfirm
                    text: qsTr("STR_ADDRESS_CHANGE_TEXT") + " "
                          + "<a href=\"https://www.portaldocidadao.pt/web/instituto-dos-registos-e-do-notariado/alterar-a-morada-do-cartao-de-cidadao\">"
                          + qsTr("STR_ADDRESS_CHANGE_TEXT_LINK")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
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
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldNumProcess
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldNumProcess.text === "" ? true: false
                    placeholderText: qsTr("STR_ADDRESS_CHANGE_NUMBER_OP")
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinCurrent.right
                    anchors.bottom: parent.bottom
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
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldConfirmAddress
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldConfirmAddress.text === "" ? true: false
                    placeholderText: qsTr("STR_ADDRESS_CHANGE_CODE_OP")
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinNew.right
                    anchors.bottom: parent.bottom
                }
            }
        }

        Item {
            width: dialogConfirmOfAddress.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.horizontalCenter: parent.horizontalCenter
            y: 180
            Button {
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTr("STR_ADDRESS_CHANGE_CANCEL")
                anchors.left: parent.left
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: {
                    dialogConfirmOfAddress.close()
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                }
            }
            Button {
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTr("STR_ADDRESS_CHANGE_CONFIRM")
                anchors.right: parent.right
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                enabled: textFieldNumProcess.length !== 0 && textFieldConfirmAddress.length !== 0 ? true : false
                onClicked: {
                    gapi.changeAddress(textFieldNumProcess.text,textFieldConfirmAddress.text)
                    progressBarIndeterminate.visible = true
                    textFieldNumProcess.text = ""
                    textFieldConfirmAddress.text = ""
                    dialogConfirmOfAddress.close()
                    dialogConfirmOfAddressProgress.open()
                }
            }
        }
        onRejected:{
            // Reject address change Popup's only with ESC key
            dialogConfirmOfAddress.open()
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

        header: Label {
            id: labelConfirmOfAddressProgressTextTitle
            text: qsTr("STR_ADDRESS_CHANGE_CONFIRMATION")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectMessageTop.height + progressBar.height + progressBarIndeterminate.height

            Item {
                id: rectMessageTop
                width: parent.width
                height: 150
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTop
                    textFormat: Text.RichText
                    text: ""
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
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
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTr("STR_ADDRESS_CHANGE_OK")
                anchors.right: parent.right
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                visible: progressBarIndeterminate.visible ? false : true
                onClicked: {
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    dialogConfirmOfAddressProgress.close()
                }
            }
        }
        onRejected:{
            // Reject address change Popup's only with ESC key
            dialogConfirmOfAddressProgress.open()
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
                if (gapi.getTriesLeftAddressPin() === 0) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup PIN","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_ADDRESS_BLOCKED")
                    mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                    mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
                }else{
                    var triesLeft = gapi.verifyAddressPin("")
                    if (triesLeft === 3) {
                        propertyBusyIndicator.running = true
                        gapi.startReadingAddress()
                    }
                }
            }else{
                dialogTestPin.open()
                textFieldPin.text = ""
            }
        }
    }
}
