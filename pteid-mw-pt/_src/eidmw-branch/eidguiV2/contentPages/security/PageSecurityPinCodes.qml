import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components/" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageSecurityPinCodesForm {

    Connections {
        target: gapi

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
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            propertyButtonModifyAuth.enabled = false
            propertyButtonTestAuth.enabled = false
            propertyButtonModifySign.enabled = false
            propertyButtonTestSign.enabled = false
            propertyButtonModifyAddress.enabled = false
            propertyButtonTestAddress.enabled = false
            
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Security Pin Codes --> Data Changed")
            propertyButtonModifyAuth.enabled = true
            propertyButtonTestAuth.enabled = true
            propertyButtonModifySign.enabled = true
            propertyButtonTestSign.enabled = true
            propertyButtonModifyAddress.enabled = true
            propertyButtonTestAddress.enabled = true
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Security Pin Codes onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                propertyButtonModifyAuth.enabled = false
                propertyButtonTestAuth.enabled = false
                propertyButtonModifySign.enabled = false
                propertyButtonTestSign.enabled = false
                propertyButtonModifyAddress.enabled = false
                propertyButtonTestAddress.enabled = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                propertyButtonModifyAuth.enabled = false
                propertyButtonTestAuth.enabled = false
                propertyButtonModifySign.enabled = false
                propertyButtonTestSign.enabled = false
                propertyButtonModifyAddress.enabled = false
                propertyButtonTestAddress.enabled = false
            }
        }
    }

    Dialog {
        id: dialogPinOK
        width: 400
        height: 200
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogPinOK.width * 0.5
        y: parent.height * 0.5 - dialogPinOK.height * 0.5

        property alias propertyDialogOkLabelText: dialogOkLabelText
        property alias propertyTextOkPin: textOkPin

        header: Label {
            id: dialogOkLabelText
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectOkPin.height

            Item {
                id: rectOkPin
                width: parent.width
                height: 90
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textOkPin
                    text: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: 150
                }
            }
            Button {
                width: Constants.WIDTH_BUTTON
                height:Constants.HEIGHT_BOTTOM_COMPONENT
                text: "OK"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectOkPin.bottom
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: dialogPinOK.close()
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
                width: textTypePin.width + textFieldPin.width
                height: 90

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
            Button {
                width: Constants.WIDTH_BUTTON
                height:Constants.HEIGHT_BOTTOM_COMPONENT
                text: "OK"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectBadPin.bottom
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: dialogBadPin.close()
            }
        }
    }

    Dialog {
        id: dialogTestPin
        width: 400
        height: 200
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogModifyPin.width * 0.5
        y: parent.height * 0.5 - dialogModifyPin.height * 0.5

        property alias propertyLabelTextTitle: labelTextTitle
        property alias propertyTextTypePin: textTypePin
        property alias propertyTextFieldPin: textFieldPin

        header: Label {
            id: labelTextTitle
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
            height: rectPin.height

            Item {
                id: rectPin
                width: parent.width
                height: 50

                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textTypePin
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
                    anchors.left: textTypePin.right
                    anchors.bottom: parent.bottom
                }
            }
        }

        standardButtons: {
            textFieldPin.length >= 4 ? DialogButtonBox.Ok | DialogButtonBox.Cancel : DialogButtonBox.Cancel
        }

        onAccepted: {
            var triesLeft = 0
            propertyBusyIndicator.running = true

            console.log("StackLayout currentIndex = " + protertyStackLayout.currentIndex)
            switch(protertyStackLayout.currentIndex) {
            case 0:
                triesLeft = gapi.verifyAuthPin(textFieldPin.text)
                if (triesLeft === 0) {
                    propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAuthPin.text = ""
                }
                else {
                    propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
                break;
            case 1:
                triesLeft = gapi.verifySignPin(textFieldPin.text)
                if (triesLeft === 0) {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftSignPin.text = ""
                }
                else {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
                break;
            case 2:
                triesLeft = gapi.verifyAddressPin(textFieldPin.text)
                if (triesLeft === 0) {
                    propertyTriesLeftAddressPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAddressPin.text = ""
                }
                else {
                    propertyTriesLeftAddressPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
                break;
            default:
                break
            }

            dialogTestPin.visible = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            propertyBusyIndicator.running = false

            if (triesLeft === 3) {
                dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
                dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                dialogPinOK.open()
            }
            else if (triesLeft === 0) {
                textBadPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                dialogBadPin.open()
            }
            else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                textBadPin.text = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
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
        id: dialogModifyPin
        width: 400
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogModifyPin.width * 0.5
        y: parent.height * 0.5 - dialogModifyPin.height * 0.5

        property alias propertyLabelModifyTextTitle: labelModifyTextTitle
        property alias propertyTextFieldPinCurrent: textFieldPinCurrent
        property alias propertyTextFieldPinNew: textFieldPinNew
        property alias propertyTextFieldPinConfirm: textFieldPinConfirm
        property alias propertyTextPinMsgConfirm: textPinMsgConfirm


        header: Label {
            id: labelModifyTextTitle
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
            height: rectPinCurrent.height + rectPinNew.height + rectPinConfirm.height + rectPinMsgConfirm.height

            Item {
                id: rectPinCurrent
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinCurrent
                    text: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_CURRENT")
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
                    id: textFieldPinCurrent
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPinCurrent.text === "" ? true: false
                    placeholderText: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_CURRENT")
                    echoMode : TextInput.Password
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    maximumLength: 8
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinCurrent.right
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectPinNew
                width: parent.width
                height: 50
                anchors.top: rectPinCurrent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinNew
                    text: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_NEW")
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
                    id: textFieldPinNew
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPinNew.text === "" ? true: false
                    placeholderText: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_NEW") + "?"
                    echoMode : TextInput.Password
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    maximumLength: 8
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinNew.right
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectPinConfirm
                width: parent.width
                height: 50
                anchors.top: rectPinNew.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinConfirm
                    text: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_CONFIRM")
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
                    id: textFieldPinConfirm
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPinConfirm.text === "" ? true: false
                    placeholderText: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_CONFIRM") + "?"
                    echoMode : TextInput.Password
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    maximumLength: 8
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinConfirm.right
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectPinMsgConfirm
                width: parent.width
                height: 50
                anchors.top: rectPinConfirm.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinMsgConfirm
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                }
            }
        }

        standardButtons: {
            textFieldPinCurrent.length >= 4 && textFieldPinNew.length >=4 && textFieldPinConfirm.length >= 4
                    ? DialogButtonBox.Ok | DialogButtonBox.Cancel : DialogButtonBox.Cancel
        }

        onAccepted: {
            var triesLeft = 0
            propertyBusyIndicator.running = true

            console.log("StackLayout currentIndex = " + protertyStackLayout.currentIndex)

            if(textFieldPinNew.text === textFieldPinConfirm.text){

                switch(protertyStackLayout.currentIndex) {
                case 0:
                    triesLeft = gapi.changeAuthPin(textFieldPinCurrent.text,textFieldPinNew.text)
                    if (triesLeft === 0) {
                        propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                    }
                    else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                        propertyTriesLeftAuthPin.text = ""
                    }
                    else {
                        propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                                + " " + triesLeft + " "
                                + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                    }
                    break;
                case 1:
                    triesLeft = gapi.changeSignPin(textFieldPinCurrent.text,textFieldPinNew.text)
                    if (triesLeft === 0) {
                        propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                    }
                    else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                        propertyTriesLeftSignPin.text = ""
                    }
                    else {
                        propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                                + " " + triesLeft + " "
                                + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                    }
                    break;
                case 2:
                    triesLeft = gapi.changeAddressPin(textFieldPinCurrent.text,textFieldPinNew.text)
                    if (triesLeft === 0) {
                        propertyTriesLeftAddressPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                    }
                    else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                        propertyTriesLeftAddressPin.text = ""
                    }
                    else {
                        propertyTriesLeftAddressPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                                                + " " + triesLeft + " "
                                                + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                    }
                    break;
                default:
                    break
                }

                dialogTestPin.visible = false
                mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                propertyBusyIndicator.running = false

                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_PIN_MODIFY")
                    dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_PIN_MODIFY_SUCESS")
                    dialogPinOK.open()
                }
                else if (triesLeft === 0) {
                    textBadPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                    dialogBadPin.open()
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    textBadPin.text = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
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
            }else{
                propertyBusyIndicator.running = false
                textPinMsgConfirm.text = qsTranslate("Popup PIN","STR_PIN_NEW_CONFIRM")
                dialogModifyPin.open()
            }
        }
        onRejected: {
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
    }

    // Buttons actions
    propertyButtonModifyAuth{
        onClicked: {
            if(Constants.USE_SDK_PIN_UI_POPUP){
                var triesLeft = 0
                triesLeft = gapi.changeAuthPin("","")
                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_PIN_MODIFY")
                    dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_PIN_MODIFY_SUCESS")
                    dialogPinOK.open()
                }
                if (triesLeft === 0) {
                    propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAuthPin.text = ""
                }
                else {
                    propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                                            + " " + triesLeft + " "
                                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
            }else{
                mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                dialogModifyPin.propertyLabelModifyTextTitle.text = qsTranslate("Popup PIN","STR_PIN_AUTH_MODIFY")
                dialogModifyPin.propertyTextFieldPinCurrent.text = ""
                dialogModifyPin.propertyTextFieldPinNew.text = ""
                dialogModifyPin.propertyTextFieldPinConfirm.text = ""
                dialogModifyPin.propertyTextPinMsgConfirm.text = ""
                dialogModifyPin.open()
            }
        }
    }
    propertyButtonTestAuth{
        onClicked: {
            if(Constants.USE_SDK_PIN_UI_POPUP){
                var triesLeft = 0
                triesLeft = gapi.verifyAuthPin("")
                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
                    dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                    dialogPinOK.open()
                }
                if (triesLeft === 0) {
                    propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAuthPin.text = ""
                }
                else {
                    propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                                            + " " + triesLeft + " "
                                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
            }else{
                mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                dialogTestPin.propertyLabelTextTitle.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY_AUTH")
                dialogTestPin.propertyTextTypePin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_AUTH")
                dialogTestPin.propertyTextFieldPin.text = ""
                dialogTestPin.open()
            }

        }
    }
    propertyButtonModifySign{
        onClicked: {


            if(Constants.USE_SDK_PIN_UI_POPUP){
                var triesLeft = 0
                triesLeft = gapi.changeSignPin("","")
                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_PIN_MODIFY")
                    dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_PIN_MODIFY_SUCESS")
                    dialogPinOK.open()
                }
                if (triesLeft === 0) {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftSignPin.text = ""
                }
                else {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                                            + " " + triesLeft + " "
                                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
            }else{
                mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                dialogModifyPin.propertyLabelModifyTextTitle.text = qsTranslate("Popup PIN","STR_PIN_SIGN_MODIFY")
                dialogModifyPin.propertyTextFieldPinCurrent.text = ""
                dialogModifyPin.propertyTextFieldPinNew.text = ""
                dialogModifyPin.propertyTextFieldPinConfirm.text = ""
                dialogModifyPin.propertyTextPinMsgConfirm.text = ""
                dialogModifyPin.open()
            }
        }
    }
    propertyButtonTestSign{
        onClicked: {


            if(Constants.USE_SDK_PIN_UI_POPUP){
                var triesLeft = 0
                triesLeft = gapi.verifySignPin("")
                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
                    dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                    dialogPinOK.open()
                }
                if (triesLeft === 0) {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftSignPin.text = ""
                }
                else {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                                            + " " + triesLeft + " "
                                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
            }else{
                mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                dialogTestPin.propertyLabelTextTitle.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY_SIGN")
                dialogTestPin.propertyTextTypePin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SIGN")
                dialogTestPin.propertyTextFieldPin.text = ""
                dialogTestPin.open()
            }
        }
    }
    propertyButtonModifyAddress{
        onClicked: {

            if(Constants.USE_SDK_PIN_UI_POPUP){
                var triesLeft = 0
                triesLeft = gapi.changeAddressPin("","")
                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_PIN_MODIFY")
                    dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_PIN_MODIFY_SUCESS")
                    dialogPinOK.open()
                }
                if (triesLeft === 0) {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftSignPin.text = ""
                }
                else {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
            }else{
                mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                dialogModifyPin.propertyLabelModifyTextTitle.text = qsTranslate("Popup PIN","STR_PIN_ADDRESS_MODIFY")
                dialogModifyPin.propertyTextFieldPinCurrent.text = ""
                dialogModifyPin.propertyTextFieldPinNew.text = ""
                dialogModifyPin.propertyTextFieldPinConfirm.text = ""
                dialogModifyPin.propertyTextPinMsgConfirm.text = ""
                dialogModifyPin.open()
            }
        }
    }
    propertyButtonTestAddress{
        onClicked: {

            if(Constants.USE_SDK_PIN_UI_POPUP){
                var triesLeft = 0
                triesLeft = gapi.verifyAddressPin("")
                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
                    dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                    dialogPinOK.open()
                }
                if (triesLeft === 0) {
                    propertyTriesLeftAddressPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAddressPin.text = ""
                }
                else {
                    propertyTriesLeftAddressPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
            }else{
                mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                dialogTestPin.propertyLabelTextTitle.text =
                        qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY_ADDRESS")
                dialogTestPin.propertyTextTypePin.text =
                        qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_ADDRESS")
                dialogTestPin.propertyTextFieldPin.text = ""
                dialogTestPin.open()
            }
        }
    }
    protertyStackLayout{

        onCurrentIndexChanged: {
            var triesLeft = 0
            console.log("StackLayout currentIndex = " + protertyStackLayout.currentIndex)
            switch(protertyStackLayout.currentIndex) {
            case 0:
                console.log("Auth Pin tries left = " + gapi.getTriesLeftAuthPin())
                triesLeft = gapi.getTriesLeftAuthPin()
                if (triesLeft === 0) {
                    propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAuthPin.text = ""
                }
                else {
                    propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                    propertyButtonModifyAuth.enabled = true
                    propertyButtonTestAuth.enabled = true
                }
                break;
            case 1:
                console.log("Sign Pin tries left = " + gapi.getTriesLeftSignPin())
                triesLeft = gapi.getTriesLeftSignPin()
                if (triesLeft === 0) {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftSignPin.text = ""
                }
                else {
                    propertyTriesLeftSignPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                    propertyButtonModifySign.enabled = true
                    propertyButtonTestSign.enabled = true
                }
                break;
            case 2:
                console.log("Address Pin tries left = " + gapi.getTriesLeftAddressPin())
                triesLeft = gapi.getTriesLeftAddressPin()
                if (triesLeft === 0) {
                    propertyTriesLeftAddressPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAddressPin.text = ""
                    propertyButtonModifyAddress.enabled = true
                    propertyButtonTestAddress.enabled = true
                }
                else {
                    propertyTriesLeftAddressPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                    propertyButtonModifyAddress.enabled = true
                    propertyButtonTestAddress.enabled = true
                }
                break;
            default:
                break
            }
        }
    }
    Component.onCompleted: {
        propertyBusyIndicator.running = true
        console.log("StackLayout currentIndex = " + protertyStackLayout.currentIndex)
        console.log("Auth Pin tries left = " + gapi.getTriesLeftAuthPin())
        var triesLeft = 0
        triesLeft = gapi.getTriesLeftAuthPin()
        if (triesLeft === 0) {
            propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
        }
        else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
            propertyTriesLeftAuthPin.text = ""
            propertyButtonModifyAuth.enabled = true
            propertyButtonTestAuth.enabled = true
        }
        else {
            propertyTriesLeftAuthPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                    + " " + triesLeft + " "
                    + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
            propertyButtonModifyAuth.enabled = true
            propertyButtonTestAuth.enabled = true

        }
        propertyBusyIndicator.running = false
    }
}
