/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions
import "../../components/" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageSecurityPinCodesForm {

    Keys.onPressed: {
        var index = protertyStackLayout.currentIndex
        
        var isOnAuthTab = index === 0 && propertyTabAuth.focus === true 
        var isOnSignTab = index === 1 && propertyTabSign.focus === true 
        var isOnAddrTab = index === 2 && propertyTabAddr.focus === true

        var update = true
        if (isOnAuthTab && !wasOnAuthTab) {
            propertyTabAuth.forceActiveFocus()
        } else if (isOnSignTab && !wasOnSignTab) {
            propertyTabSign.forceActiveFocus()
        } else if (isOnAddrTab && !wasOnAddrTab) {
            propertyTabAddr.forceActiveFocus()
        } else {
            Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
            wasOnAuthTab = false
            wasOnSignTab = false
            wasOnAddrTab = false
            update = false
        }

        if (update) {
            wasOnAuthTab = isOnAuthTab
            wasOnSignTab = isOnSignTab
            wasOnAddrTab = isOnAddrTab
        }
    }

    Connections {
        target: gapi

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
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)

            if(error_code !== GAPI.CardUserPinCancel
                    && error_code !== GAPI.CardPinTimeout){
                propertyButtonModifyAuth.enabled = false
                propertyButtonTestAuth.enabled = false
                propertyButtonModifySign.enabled = false
                propertyButtonTestSign.enabled = false
                propertyButtonModifyAddress.enabled = false
                propertyButtonTestAddress.enabled = false
            }
            
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
            propertyBusyIndicator.running = true
            switch(protertyStackLayout.currentIndex) {
            case 0:
                gapi.getTriesLeftAuthPin()
                break;
            case 1:
                gapi.getTriesLeftSignPin()
                break;
            case 2:
                gapi.getTriesLeftAddressPin()
                break;
            default:
                break
            }
        }
        onSignalCardChanged: {
            console.log("Security Pin Codes onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                propertyTriesLeftAuthPin.text = ""
                propertyButtonModifyAuth.enabled = false
                propertyButtonTestAuth.enabled = false
                propertyTriesLeftSignPin.text = ""
                propertyButtonModifySign.enabled = false
                propertyButtonTestSign.enabled = false
                propertyTriesLeftAddressPin.text = ""
                propertyButtonModifyAddress.enabled = false
                propertyButtonTestAddress.enabled = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                propertyTriesLeftAuthPin.text = ""
                propertyButtonModifyAuth.enabled = false
                propertyButtonTestAuth.enabled = false
                propertyTriesLeftSignPin.text = ""
                propertyButtonModifySign.enabled = false
                propertyButtonTestSign.enabled = false
                propertyTriesLeftAddressPin.text = ""
                propertyButtonModifyAddress.enabled = false
                propertyButtonTestAddress.enabled = false
            }
        }
        onSignalTriesLeftPinFinished: {
            updateWithTriesLeft(triesLeft, pin)
        }

        onSignalTestPinFinished: {
            if (triesLeft === 3) {
                var titlePopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
                var bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
            updateWithTriesLeft(triesLeft, pin)
        }

        onSignalModifyPinFinished: {
            if (triesLeft === 3) {
                var titlePopup = qsTranslate("Popup PIN","STR_PIN_MODIFY")
                var bodyPopup = qsTranslate("Popup PIN","STR_PIN_MODIFY_SUCESS")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
            updateWithTriesLeft(triesLeft, pin)
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
                triesLeft = gapi.doVerifyAuthPin(textFieldPin.text)
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
                triesLeft = gapi.doVerifySignPin(textFieldPin.text)
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
                triesLeft = gapi.doVerifyAddressPin(textFieldPin.text)
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

            var titlePopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
            var bodyPopup = ""
            if (triesLeft === 3) {
                bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
            }
            else if (triesLeft === 0) {
                bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
            }
            else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_ACCESS_ERROR")
            }
            else {
                bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_WRONG")
                        + " <br/><br/>"
                        + qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                        + " " + triesLeft + " "
                        + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
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
                    triesLeft = gapi.doChangeAuthPin(textFieldPinCurrent.text,textFieldPinNew.text)
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
                    triesLeft = gapi.doChangeSignPin(textFieldPinCurrent.text,textFieldPinNew.text)
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
                    triesLeft = gapi.doChangeAddressPin(textFieldPinCurrent.text,textFieldPinNew.text)
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

                var titlePopup = qsTranslate("Popup PIN","STR_PIN_MODIFY")
                if (triesLeft === 3) {
                    bodyPopup = qsTranslate("Popup PIN","STR_PIN_MODIFY_SUCESS")
                }
                else if (triesLeft === 0) {
                    bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_ACCESS_ERROR")
                }
                else {
                    bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_WRONG")
                            + " <br/><br/>"
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                            + " " + triesLeft + " "
                            + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
                }
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
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
                gapi.changeAuthPin("","")
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
                gapi.verifyAuthPin("")
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
                gapi.changeSignPin("","")
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
                gapi.verifySignPin("")
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
                gapi.changeAddressPin("","")
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
                gapi.verifyAddressPin("")
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

    propertyBar{
        onCurrentIndexChanged: {
            protertyStackLayout.currentIndex = propertyBar.currentIndex
        }
    }

    protertyStackLayout{
        onCurrentIndexChanged: {
            var triesLeft = 0
            console.log("StackLayout currentIndex = " + protertyStackLayout.currentIndex)
            propertyBusyIndicator.running = true
            switch(protertyStackLayout.currentIndex) {
            case 0:
                gapi.getTriesLeftAuthPin()
                break;
            case 1:
                gapi.getTriesLeftSignPin()
                break;
            case 2:
                gapi.getTriesLeftAddressPin()
                break;
            default:
                break
            }
        }
    }
    Component.onCompleted: {
        propertyBusyIndicator.running = true
        console.log("StackLayout currentIndex = " + protertyStackLayout.currentIndex)

        // guarantee that current index is the first element
        protertyStackLayout.currentIndex = 0

        gapi.getTriesLeftAuthPin()
        propertyTabAuth.forceActiveFocus()
    }

    function updateWithTriesLeft(triesLeft, pin) {
    console.log("update with tries left: tries=" + triesLeft + " pin=" + pin)
        var triesLeftText, modifyButton, testButton
        if (pin == GAPI.AuthPin){
            triesLeftText = propertyTriesLeftAuthPin
            modifyButton  = propertyButtonModifyAuth
            testButton  = propertyButtonTestAuth
        } else if (pin == GAPI.SignPin){
            triesLeftText = propertyTriesLeftSignPin
            modifyButton  = propertyButtonModifySign
            testButton  = propertyButtonTestSign
        } else if (pin == GAPI.AddressPin){
            triesLeftText = propertyTriesLeftAddressPin
            modifyButton  = propertyButtonModifyAddress
            testButton  = propertyButtonTestAddress
        } else {
            console.log("Error: pin can only be authPin, signPin or addressPin when emitting signal")
            propertyBusyIndicator.running = false
            return
        }

        console.log("Pin tries left = " + triesLeft)
        if (triesLeft === 0) {
            triesLeftText.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_BLOCKED")
            modifyButton.enabled = false
            testButton.enabled = false
        }
        else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
            //triesLeftText.text = ""
            modifyButton.enabled = true
            testButton.enabled = true
        }
        else {
            triesLeftText.text = qsTranslate("Popup PIN","STR_POPUP_CARD_REMAIN")
                    + " " + triesLeft + " "
                    + qsTranslate("Popup PIN","STR_POPUP_CARD_TRIES")
            modifyButton.enabled = true
            testButton.enabled = true
        }
        propertyBusyIndicator.running = false
    }
}
