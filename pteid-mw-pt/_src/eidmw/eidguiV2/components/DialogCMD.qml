/*-****************************************************************************

 * Copyright (C) 2018-2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2018-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../scripts/Constants.js" as Constants
import "../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

Item {
    property var dialogType;
    property bool signSingleFile;
    
    /* #################################################################### *
     * #                             Signals                              # *
    /* #################################################################### */

    Connections {
        enabled: cmdDialog.visible
        target: gapi

        onSignalValidateOtp: {
            console.log("Signal Validate OTP")
            dialogContent.state = Constants.DLG_STATE.VALIDATE_OTP;
            textFieldReturnCode.forceActiveFocus()
        }
        onSignalShowMessage: {
            showMessage(msg,urlLink)
        }
        onSignalOpenFile: {
            console.log("Signal Open File")
            dialogContent.state = Constants.DLG_STATE.OPEN_FILE;
            dialogContent.forceActiveFocus()
        }
        onSignalUpdateProgressBar: {
            console.log("CMD sign change --> update progress bar with value = " + value)
            progressBar.value = value
        }
        onSignalUpdateProgressStatus: {
            console.log("CMD sign change --> update progress status with text = " + statusMessage)
            textMessageTop.text = statusMessage
            textMessageTop.forceActiveFocus()
        }
        onSignalShowLoadAttrButton: {
            console.log("Signal show load attributes button")
            dialogContent.state = Constants.DLG_STATE.LOAD_ATTRIBUTES;
        }
    }

    /* #################################################################### *
     * #                                UI                                # *
    /* #################################################################### */
    
    id: dialogContainer
    // This dialog can be instantiated anywhere but the parent will always
    // be the mainWindow so that the dialog is centered
    parent: mainWindow.contentItem
    anchors.fill: parent

    Dialog {
        id: cmdDialog
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: parent.width * 0.5 - cmdDialog.width * 0.5
        y: parent.height * 0.5 - cmdDialog.height * 0.5
        modal: true
        z: Constants.DIALOG_CASCATE_MIDDLE

        header: Label {
            id: dialogTitle
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: dialogContent.activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
            wrapMode: Text.WordWrap
            lineHeight: 1.2
        }
        contentItem: Item {
            property var next : linkCMD.visible ? linkCMD : labelCMDText.propertyText

            id: dialogContent
            height: parent.height
            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return)
                {
                    confirmDlg()
                }
            }
            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS")
                             + dialogTitle.text + textMessageTop.text
            KeyNavigation.tab: next
            KeyNavigation.down: next
            KeyNavigation.right: next
            KeyNavigation.backtab: buttonConfirm.enabled ? buttonConfirm : buttonCancel
            KeyNavigation.up: buttonConfirm.enabled ? buttonConfirm : buttonCancel
            KeyNavigation.left: buttonConfirm.enabled ? buttonConfirm : buttonCancel

            Item {
                id: rectMessageTopLogin
                width: parent.width
                height: childrenRect.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                Link {
                    id: linkCMD
                    width: parent.width
                    height: 25
                    visible: false
                    propertyText.text: "<a href='https://www.autenticacao.gov.pt/cmd-pedido-chave'>"
                            + qsTranslate("PageServicesSign","STR_SIGN_CMD_URL") + controler.autoTr
                    propertyText.font.italic: true
                    propertyText.verticalAlignment: Text.AlignVCenter
                    anchors.top: parent.top
                    propertyText.font.pixelSize: Constants.SIZE_TEXT_LINK_LABEL
                    propertyText.font.bold: activeFocus
                    propertyAccessibleText: qsTranslate("PageServicesSign","STR_SIGN_CMD_URL")
                    propertyLinkUrl: 'https://www.autenticacao.gov.pt/cmd-pedido-chave'
                    KeyNavigation.tab: comboBoxMobileNumber.visible ? comboBoxMobileNumber : labelCMDText.propertyText
                    KeyNavigation.down: comboBoxMobileNumber.visible ? comboBoxMobileNumber : labelCMDText.propertyText
                    KeyNavigation.right: comboBoxMobileNumber.visible ? comboBoxMobileNumber : labelCMDText.propertyText
                    KeyNavigation.backtab: dialogContent
                    KeyNavigation.up: dialogContent

                }
                ProgressBar {
                    id: progressBar
                    width: parent.width
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    to: 100
                    value: 0
                    indeterminate: false
                    visible: false
                    z:1
                }
                Text {
                    id: textMessageTop
                    height: 25
                    anchors.top: linkCMD.visible ? linkCMD.bottom : progressBar.bottom
                    anchors.topMargin: linkCMD.visible ? Constants.SIZE_ROW_V_SPACE : 3
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: ( activeFocus || labelCMDText.propertyText.activeFocus) ? true : false
                    color: Constants.COLOR_TEXT_LABEL
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
            }
            Item {
                id: dataFieldsRect
                anchors.top: rectMessageTopLogin.bottom
                height: parent.height
                width: parent.width
                visible: true
                Item {
                    id: rectMobileNumber
                    width: parent.width
                    height: 50
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                    Text {
                        id: textMobileNumber
                        text: qsTranslate("PageServicesSign","STR_SIGN_CMD_MOVEL_NUM") + controler.autoTr
                        verticalAlignment: Text.AlignVCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: Constants.SIZE_TEXT_LABEL
                        font.family: lato.name
                        font.bold: activeFocus
                        color: Constants.COLOR_TEXT_BODY
                        height: parent.height
                        width: parent.width * 0.3
                        anchors.bottom: parent.bottom
                    }
                    PhoneCountriesCodeListModel{id: phoneCountryCodeList}
                    ComboBox {
                        id: comboBoxMobileNumber
                        width: parent.width * 0.4
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        model: phoneCountryCodeList
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        visible: true
                        popup.z: Constants.DIALOG_CASCATE_MIDDLE
                        anchors.left: textMobileNumber.right
                        anchors.bottom: parent.bottom
                        onCurrentIndexChanged: {
                            if(dialogContent.state == Constants.DLG_STATE.SIGN_FORM) {
                                if(comboBoxMobileNumber.currentIndex >= 0){
                                    propertyPageLoader.propertyBackupMobileIndicatorIndex = comboBoxMobileNumber.currentIndex
                                }else{
                                    comboBoxMobileNumber.currentIndex = propertyPageLoader.propertyBackupMobileIndicatorIndex
                                }
                            }
                        }
                        Accessible.role: Accessible.ComboBox
                        Accessible.name: currentText
                        KeyNavigation.tab: textFieldMobileNumber
                        KeyNavigation.right: textFieldMobileNumber
                        KeyNavigation.backtab: linkCMD
                        KeyNavigation.left: linkCMD
                    }
                    TextField {
                        id: textFieldMobileNumber
                        width: parent.width * 0.25
                        anchors.verticalCenter: parent.verticalCenter
                        font.italic: textFieldMobileNumber.text === "" ? true: false
                        placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_MOVEL_NUM_OP") + controler.autoTr + "?"
                        validator: RegExpValidator { regExp: /[0-9]+/ }
                        maximumLength: 15
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: false
                        anchors.left: comboBoxMobileNumber.right
                        anchors.leftMargin:  parent.width * 0.05
                        anchors.bottom: parent.bottom
                        onEditingFinished: {
                            // CMD load backup mobile data
                            propertyPageLoader.propertyBackupMobileNumber = textFieldMobileNumber.text
                        }
                        Accessible.role: Accessible.EditableText
                        Accessible.name: textMessageTop.text + textMobileNumber.text
                        KeyNavigation.tab: textFieldPin
                        KeyNavigation.down: textFieldPin
                        KeyNavigation.right: textFieldPin
                        KeyNavigation.up: comboBoxMobileNumber
                    }
                }
                Item {
                    id: rectPin
                    width: parent.width
                    height: 50
                    anchors.top: rectMobileNumber.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                    Text {
                        id: textPin
                        text: qsTranslate("PageServicesSign","STR_SIGN_CMD_PIN") + controler.autoTr
                        verticalAlignment: Text.AlignVCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: Constants.SIZE_TEXT_LABEL
                        font.family: lato.name
                        font.bold: activeFocus
                        color: Constants.COLOR_TEXT_BODY
                        height: parent.height
                        width: parent.width * 0.3
                        anchors.bottom: parent.bottom
                    }
                    TextField {
                        id: textFieldPin
                        width: parent.width * 0.7
                        anchors.verticalCenter: parent.verticalCenter
                        font.italic: textFieldPin.text === "" ? true: false
                        placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_PIN_OP") + controler.autoTr + "?"
                        validator: RegExpValidator { regExp: /[0-9]{4,8}/ }
                        echoMode : TextInput.Normal
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.bold: activeFocus
                        clip: false
                        anchors.left: textPin.right
                        anchors.bottom: parent.bottom
                        Accessible.role: Accessible.EditableText
                        Accessible.name: textPin.text
                        //KeyNavigation.backtab: textFieldMobileNumber
                        //KeyNavigation.up: textFieldMobileNumber
                        onFocusChanged: {
                            if (activeFocus) {
                                // reset PIN text when focus is gained
                                textFieldPin.text = ""
                                textFieldPin.echoMode = TextInput.Normal
                                accessibilityTimer.start()
                            }
                        }

                        Keys.onEnterPressed: {
                            confirmDlg()
                        }
                        Keys.onReturnPressed: {
                            confirmDlg()
                        }
                    }
                    /*
                      Workaround for the problem of the screen-reader not reading the name of the TextField
                      if its echoMode is set to 'Password':
                        set the initial echoMode to 'Normal'.
                        use a timer(delay) to let the screen-reader start reading the TextField's name,
                        then set the echoMode to 'Password' and force the update of the changed property.
                    */
                    Timer {
                        id: accessibilityTimer
                        interval: 100
                        repeat: false
                        running: false
                        onTriggered: {
                            textFieldPin.echoMode = TextInput.Password
                            controler.forceAccessibilityUpdate(textFieldPin)
                        }
                    }
                }
                Item {
                    id: rectReturnCode
                    width: parent.width
                    height: 50
                    anchors.top: parent.top
                    anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                    Text {
                        id: textReturnCode
                        text: qsTranslate("PageServicesSign","STR_SIGN_CMD_CODE") + controler.autoTr + ":"
                        verticalAlignment: Text.AlignVCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: Constants.SIZE_TEXT_LABEL
                        font.family: lato.name
                        font.bold: activeFocus
                        color: Constants.COLOR_TEXT_BODY
                        height: parent.height
                        width: parent.width * 0.5
                        anchors.bottom: parent.bottom
                    }
                    TextField {
                        id: textFieldReturnCode
                        width: parent.width * 0.5
                        anchors.verticalCenter: parent.verticalCenter
                        font.italic: textFieldReturnCode.text === "" ? true: false
                        placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_CODE_OP") + controler.autoTr + "?"
                        validator: RegExpValidator { regExp: /^[0-9]{6}$/ }
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.bold: activeFocus
                        clip: false
                        anchors.left: textReturnCode.right
                        anchors.bottom: parent.bottom
                        Accessible.role: Accessible.EditableText
                        Accessible.name: textMessageTop.text + textReturnCode.text
                        Keys.onEnterPressed: {
                            confirmDlg()
                        }
                        Keys.onReturnPressed: {
                            confirmDlg()
                        }
                    }
                }
                Rectangle {
                    id: rectSendSMS
                    width: parent.width
                    height: textSendSMS.paintedHeight + cmdDialog.padding
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                    color: 'white'
                    
                    Text {
                        id: textSendSMS
                        width: parent.width
                        font.pixelSize: Constants.SIZE_TEXT_LABEL
                        font.family: lato.name
                        anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                        anchors.rightMargin: Constants.SIZE_ROW_H_SPACE
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTranslate("PageServicesSign","STR_SEND_SMS_DESC") + controler.autoTr
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignJustify
                    }
                }
            }
            ProgressBar {
                id: progressBarIndeterminate
                width: parent.width
                anchors.top: parent.verticalCenter
                anchors.topMargin: 50
                anchors.horizontalCenter: parent.horizontalCenter
                to: 100
                value: 0
                indeterminate: true
                visible: false
                z:1
            }
            Item {
                id: rectLabelCMDText
                height: 50
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                Link {
                        id: labelCMDText
                        visible: false
                        propertyText.verticalAlignment: Text.AlignVCenter
                        anchors.verticalCenter: parent.verticalCenter
                        height: 50
                        width: parent.width
                        propertyText.font.pixelSize: Constants.SIZE_TEXT_LINK_LABEL
                        anchors.fill: parent 
                        propertyText.color: Constants.COLOR_TEXT_LABEL
                        KeyNavigation.tab: checkboxDontAskAgain.visible ? checkboxDontAskAgain : (buttonCancel.visible ? buttonCancel : buttonConfirm)
                        KeyNavigation.down: checkboxDontAskAgain.visible ? checkboxDontAskAgain : (buttonCancel.visible ? buttonCancel : buttonConfirm)
                        KeyNavigation.right: checkboxDontAskAgain.visible ? checkboxDontAskAgain : (buttonCancel.visible ? buttonCancel : buttonConfirm)      
                        Keys.onEnterPressed: {
                            confirmDlg()
                        }
                        Keys.onReturnPressed: {
                            confirmDlg()
                        }
                }
            }
            CheckBox {
                id: checkboxDontAskAgain
                text: qsTranslate("main","STR_DONT_ASK_AGAIN") + controler.autoTr
                height: 25
                visible: false
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
                font.bold: activeFocus
                checked: !controler.getAskToRegisterCmdCertValue()
                anchors.top: rectLabelCMDText.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Accessible.role: Accessible.CheckBox
                Accessible.name: text
                KeyNavigation.left: labelCMDText.propertyText
                KeyNavigation.backtab: labelCMDText.propertyText
                KeyNavigation.up: labelCMDText.propertyText
                onClicked: {
                    controler.setAskToRegisterCmdCertValue(!checkboxDontAskAgain.checked)
                }
            }

            states: [
                State {
                    name: Constants.DLG_STATE.ASK_TO_REGISTER_CERT
                    PropertyChanges {target: dialogContent; next: linkCMD}
                    PropertyChanges {target: linkCMD; visible: true}
                    PropertyChanges {target: checkboxDontAskAgain; visible: true}
                    PropertyChanges {target: buttonCancel; prev: checkboxDontAskAgain}
                    PropertyChanges {
                        target: labelCMDText; visible: true; propertyLinkUrl: ""
                        propertyText.text: qsTr("STR_REGISTER_CMD_CERT_DESC") + controler.autoTr; 
                        propertyAccessibleText: qsTr("STR_REGISTER_CMD_CERT_DESC") + controler.autoTr; 
                    }
                    PropertyChanges {target: buttonConfirm; text: qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_BUTTON") + controler.autoTr}
                    PropertyChanges {target: dialogTitle; restoreEntryValues : false; text: qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_TITLE") + controler.autoTr}
                },
                State {
                    name: Constants.DLG_STATE.REGISTER_FORM
                    PropertyChanges {target: linkCMD; visible: true}
                    PropertyChanges {target: rectMobileNumber; visible: true}
                    PropertyChanges {target: textFieldMobileNumber; visible: true; focus: true}
                    PropertyChanges {target: rectPin; visible: true}
                    PropertyChanges {target: textMessageTop; text: qsTranslate("PageServicesSign","STR_SIGN_INSERT_LOGIN") + controler.autoTr}
                    PropertyChanges {
                        target: dialogTitle
                        restoreEntryValues : false
                        text: qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_TITLE") + controler.autoTr
                    }
                    PropertyChanges {target: buttonCancel; prev: textFieldPin}
                    PropertyChanges {target: buttonConfirm; enabled: textFieldMobileNumber.acceptableInput && textFieldPin.acceptableInput}
                    PropertyChanges {target: dialogContent; next: linkCMD}
                },
                State {
                    name: Constants.DLG_STATE.SIGN_FORM
                    PropertyChanges {target: linkCMD; visible: true}
                    PropertyChanges {target: rectMobileNumber; visible: true}
                    PropertyChanges {target: textFieldMobileNumber; visible: true; focus: true; text: propertyPageLoader.propertyBackupMobileNumber}
                    PropertyChanges {target: rectPin; visible: true}
                    PropertyChanges {target: textMessageTop; text: qsTranslate("PageServicesSign","STR_SIGN_INSERT_LOGIN") + controler.autoTr}
                    PropertyChanges {
                        target: dialogTitle
                        restoreEntryValues : false
                        text: qsTranslate("PageServicesSign","STR_SIGN_CMD") + controler.autoTr
                    }
                    PropertyChanges {target: buttonCancel; prev: textFieldPin}
                    PropertyChanges {target: buttonConfirm; enabled: textFieldMobileNumber.acceptableInput && textFieldPin.acceptableInput}
                    PropertyChanges {target: dialogContent; next: linkCMD}
                },
                State {
                    name: Constants.DLG_STATE.PROGRESS
                    PropertyChanges {target: dataFieldsRect; visible: false}
                    PropertyChanges {target: buttonConfirm; visible: false}
                    PropertyChanges {target: progressBar; visible: true}
                    PropertyChanges {target: progressBarIndeterminate; visible: true}
                },
                State {
                    name: Constants.DLG_STATE.VALIDATE_OTP
                    PropertyChanges {target: progressBar; visible: true}
                    PropertyChanges {target: rectSendSMS; visible: true}
                    PropertyChanges {target: rectReturnCode; visible: true}
                    PropertyChanges {target: buttonCancel; prev: textFieldReturnCode}
                    PropertyChanges {target: dialogContent; next: textFieldReturnCode}
                    PropertyChanges {target: buttonConfirm; enabled: textFieldReturnCode.acceptableInput}
                    PropertyChanges {target: buttonSendSMS; enabled: true}
                },
                State {
                    name: Constants.DLG_STATE.LOAD_ATTRIBUTES
                    PropertyChanges {target: buttonConfirm; text: qsTranslate("PageServicesSign","STR_LOAD_SCAP_ATTRIBUTES") + controler.autoTr}
                    PropertyChanges {target: progressBar; visible: true}
                    PropertyChanges {
                        target: labelCMDText;
                        visible: true;
                        propertyLinkUrl: ""
                        propertyText.text: qsTranslate("Popup File","STR_POPUP_LOAD_SCAP_ATTR") + controler.autoTr;
                        propertyAccessibleText: qsTranslate("Popup File","STR_POPUP_LOAD_SCAP_ATTR") + controler.autoTr;
                    }
                },
                State {
                    name: Constants.DLG_STATE.SHOW_MESSAGE
                    PropertyChanges {
                        target: labelCMDText; visible: true; 
                    }
                    PropertyChanges {target: progressBar; visible: true}
                    PropertyChanges {target: progressBarIndeterminate; visible: false}
                },
                State {
                    name: Constants.DLG_STATE.OPEN_FILE
                    PropertyChanges {target: labelCMDText; visible: true; propertyLinkUrl: ""}
                    PropertyChanges {target: buttonConfirm; text: qsTranslate("Popup File","STR_POPUP_FILE_OPEN") + controler.autoTr}
                    PropertyChanges {target: progressBar; visible: true}
                    PropertyChanges {
                        target: labelCMDText
                        visible: true
                        propertyText.text: signSingleFile ? qsTranslate("PageServicesSign","STR_SIGN_OPEN") + controler.autoTr :
                                               qsTranslate("PageServicesSign","STR_SIGN_OPEN_MULTI") + controler.autoTr
                        propertyAccessibleText: signSingleFile ? qsTranslate("PageServicesSign","STR_SIGN_OPEN") + controler.autoTr :
                                               qsTranslate("PageServicesSign","STR_SIGN_OPEN_MULTI") + controler.autoTr
                    }
                },
                State {
                    name: Constants.DLG_STATE.OPEN_FILE_ERROR
                    PropertyChanges {target: progressBar; visible: true}
                    PropertyChanges {target: progressBarIndeterminate; visible: false}
                    PropertyChanges {target: buttonCancel; visible: false}
                    PropertyChanges {target: buttonConfirm; visible: true}
                    PropertyChanges {target: buttonConfirm; text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM") + controler.autoTr}
                    PropertyChanges {
                        target: dialogTitle
                        restoreEntryValues : false
                        text: signSingleFile ? qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR_TITLE") + controler.autoTr :
                                               qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR_TITLE_MULTI") + controler.autoTr
                    }
                    PropertyChanges {
                        target: labelCMDText;
                        visible: true;
                        propertyLinkUrl: ""
                        propertyText.text: signSingleFile ? qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR") + controler.autoTr :
                                               qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR_MULTI") + controler.autoTr
                        propertyAccessibleText: signSingleFile ? qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR") + controler.autoTr :
                                               qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR_MULTI") + controler.autoTr
                    }
                }
            ]
        }

        footer: Rectangle {
            id: bottomRow
            height: Constants.HEIGHT_BOTTOM_COMPONENT + Constants.SIZE_ROW_V_SPACE
            
            Button {
                property var prev: checkboxDontAskAgain
                id: buttonCancel
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CANCEL") + controler.autoTr
                visible: dialogContent.state != Constants.DLG_STATE.SHOW_MESSAGE
                anchors.left: parent.left
                anchors.leftMargin: cmdDialog.padding
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus
                onClicked: {
                    dialogContainer.close()
                    if(autoUpdateDialog.visible){
                        autoUpdateDialog.close()
                        autoUpdateDialog.open()
                    }
                }
                Keys.onEnterPressed: clicked()
                Keys.onReturnPressed: clicked()
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: buttonSendSMS
                KeyNavigation.down: buttonSendSMS
                KeyNavigation.right: buttonSendSMS
                KeyNavigation.backtab: prev
                KeyNavigation.up: prev
                KeyNavigation.left: prev
            }
            Button {
                id: buttonSendSMS
                visible: rectSendSMS.visible
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTranslate("main","STR_SEND_BY_SMS_BUTTON") + controler.autoTr
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus
                onClicked: {
                    buttonSendSMS.enabled = false
                    textFieldReturnCode.forceActiveFocus()
                    gapi.sendSmsCmd(dialogType)
                }
                Keys.onEnterPressed: clicked()
                Keys.onReturnPressed: clicked()
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: buttonConfirm
                KeyNavigation.down: buttonConfirm
                KeyNavigation.right: buttonConfirm
                KeyNavigation.backtab: buttonCancel
                KeyNavigation.up: buttonCancel
                KeyNavigation.left: buttonCancel
            }
            Button {
                id: buttonConfirm
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM") + controler.autoTr
                anchors.right: parent.right
                anchors.rightMargin: cmdDialog.padding
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus
                onClicked: {
                    confirmDlg()
                }
                Keys.onEnterPressed: clicked()
                Keys.onReturnPressed: clicked()
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.backtab: buttonSendSMS
                KeyNavigation.up: buttonSendSMS
                KeyNavigation.left: buttonSendSMS
            }
        }

        onOpened: {
            if (dialogContent.state == Constants.DLG_STATE.ASK_TO_REGISTER_CERT)
                dialogContent.forceActiveFocus()
            else
                textFieldMobileNumber.forceActiveFocus();
        }
        onRejected:{
            cmdDialog.open()
        }
    }

    /* #################################################################### *
     * #                            Functions                             # *
    /* #################################################################### */

    /* ############## Dialog ############## */
     function isVisible(){
         return cmdDialog.visible
     }

    function open(type) {
        dialogType = type
        mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS

        if (type == GAPI.RegisterCert) {
            dialogContent.state = Constants.DLG_STATE.REGISTER_FORM
        }
        else if (type == GAPI.Sign) {
            dialogContent.state = Constants.DLG_STATE.SIGN_FORM
        }
        else if (type == GAPI.AskToRegisterCert) {
            dialogContent.state = Constants.DLG_STATE.ASK_TO_REGISTER_CERT
        }
        else {
            console.log("Error: invalid cmd dialog type: " + type)
            dialogContent.state = Constants.DLG_STATE.SHOW_MESSAGE
            labelCMDText.propertyText.text = qsTranslate("GAPI", "STR_POPUP_ERROR") + controler.autoTr
        }

        cmdDialog.open()
    }

    function confirmDlg() {
        if (!buttonConfirm.enabled)
            return;
        
        switch(dialogContent.state){
            case Constants.DLG_STATE.REGISTER_FORM:
                registerCMDCertOpen();
                break;
            case Constants.DLG_STATE.SIGN_FORM:
                signCMD()
                break;
            case Constants.DLG_STATE.VALIDATE_OTP:
                if (dialogType == GAPI.RegisterCert)
                    registerCMDCertClose()
                else if (dialogType == GAPI.Sign)
                    signCMDConfirm()
                break;
            case Constants.DLG_STATE.LOAD_ATTRIBUTES:
                loadSCAPAttributes()
                break;
            case Constants.DLG_STATE.SHOW_MESSAGE:
                close()
                break;
            case Constants.DLG_STATE.OPEN_FILE:
                openSignedFiles()
                break;
            case Constants.DLG_STATE.OPEN_FILE_ERROR:
                close()
                break;
            case Constants.DLG_STATE.ASK_TO_REGISTER_CERT:
                close()
                open(GAPI.RegisterCert)
                break;
        }
    }

    function close() {
        clearInputFields()
        cmdDialog.close()
        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        mainFormID.propertyPageLoader.forceActiveFocus()
        if(dialogContent.state == Constants.DLG_STATE.PROGRESS && dialogType == GAPI.Sign) {
            gapi.cancelCMDSign()
        }
        if(dialogContent.state == Constants.DLG_STATE.PROGRESS && dialogType == GAPI.RegisterCert) {
            gapi.cancelCMDRegisterCert()
        }
    }

    function clearInputFields() {
        textFieldPin.text = ""
        textFieldReturnCode.text = ""
    }
    /* ############## Register Certificate ############## */

    function registerCMDCertOpen() {
        dialogContent.state = Constants.DLG_STATE.PROGRESS

        var countryCode = comboBoxMobileNumber.currentText.substring(0, comboBoxMobileNumber.currentText.indexOf(' '));
        var mobileNumber = countryCode + " " + textFieldMobileNumber.text
        var pin = textFieldPin.text
        gapi.registerCMDCertOpen(mobileNumber, pin)
    }

    function registerCMDCertClose() {
        dialogContent.state = Constants.DLG_STATE.PROGRESS

        var otp = textFieldReturnCode.text
        gapi.registerCMDCertClose(otp)
    }

    /* ############## Signature  ############## */

    function signCMD(){
        var loadedFilePaths = []
        for (var fileIndex = 0; fileIndex < filesModel.count; fileIndex++) {
            loadedFilePaths.push(filesModel.get(fileIndex).fileUrl)
        }
        
        var outputFile = ""
        signSingleFile = (filesModel.count == 1)
        if (signSingleFile) {
            outputFile = propertyFileDialogCMDOutput.file.toString()
        }
        else {
            outputFile = propertyFileDialogBatchCMDOutput.folder.toString()
        }
        outputFile = decodeURIComponent(Functions.stripFilePrefix(outputFile))

        var page = 1
        if(propertyCheckLastPage.checked) {
            page = 0 // Sign last page in all documents
        }else{
            page = propertySpinBoxControl.value
        }
        
        var isTimestamp = false
        if (typeof propertySwitchSignTemp !== "undefined")
            isTimestamp = propertySwitchSignTemp.checked

        var isLTV = false
        if (typeof propertyCheckboxLTV !== "undefined")
            isLTV = propertyCheckboxLTV.checked

        var reason = ""
        if (typeof propertyTextFieldReason !== "undefined")
            reason = propertyTextFieldReason.text

        var location = ""
        if (typeof propertyTextFieldLocal !== "undefined")
            location = propertyTextFieldLocal.text

        var isSmallSignature = false
        if (typeof propertyCheckSignReduced !== "undefined")
            isSmallSignature = propertyCheckSignReduced.checked

        propertyPDFPreview.updateSignPreview()
        var coord_x = -1
        var coord_y = -1
        if(typeof propertyCheckSignShow !== "undefined"){
            if(propertyCheckSignShow.checked){
                coord_x = propertyPDFPreview.propertyCoordX
                //coord_y must be the lower left corner of the signature rectangle
                coord_y = propertyPDFPreview.propertyCoordY
            }
        } else {
            coord_x = propertyPDFPreview.propertyCoordX
            //coord_y must be the lower left corner of the signature rectangle
            coord_y = propertyPDFPreview.propertyCoordY
        }

        /*console.log("Output filename: " + outputFile)*/
        console.log("Signing in position coord_x: " + coord_x
                    + " and coord_y: "+coord_y)

        var countryCode = comboBoxMobileNumber.currentText.substring(0, comboBoxMobileNumber.currentText.indexOf(' '));
        var mobileNumber = countryCode + " " + textFieldMobileNumber.text
        propertyOutputSignedFile = outputFile

        dialogContent.state = Constants.DLG_STATE.PROGRESS
        if (typeof propertySwitchSignAdd !== "undefined" && propertySwitchSignAdd.checked) {
            gapi.signOpenScapWithCMD(mobileNumber,textFieldPin.text,
                                     loadedFilePaths,outputFile,page,
                                     coord_x, coord_y,
                                     reason, location, isTimestamp, isLTV)
        } else {
            gapi.signOpenCMD(mobileNumber,textFieldPin.text,
                             loadedFilePaths,outputFile,page,
                             coord_x,coord_y,
                             reason,location,
                             isTimestamp, isLTV, isSmallSignature)
        }
    }

    function signCMDConfirm(){
        dialogContent.state = Constants.DLG_STATE.PROGRESS
        /*console.log("Send sms_token : " + textFieldReturnCode.text)*/
        var attributeList = []
        //CMD with SCAP attributes
        if (typeof propertySwitchSignAdd !== "undefined" && propertySwitchSignAdd.checked) {
            var count = 0
            for (var i = 0; i < entityAttributesModel.count; i++){
                if(entityAttributesModel.get(i).checkBoxAttr == true) {
                    attributeList[count] = i
                    count++
                }
            }
        }
        gapi.signCloseCMD(textFieldReturnCode.text, attributeList)
    }
    function openSignedFiles() {
        if (Functions.openSignedFiles() == true){
                close()
            } else {
                dialogContent.state = Constants.DLG_STATE.OPEN_FILE_ERROR
                labelCMDText.propertyText.forceActiveFocus()
            }
    }
    function loadSCAPAttributes(){
        close()
        propertyPageLoader.attributeListBackup = []
        gapi.startRemovingAttributesFromCache(GAPI.ScapAttrAll)
        jumpToDefinitionsSCAP()
    }
    
    function showMessage(msg,urlLink){
        console.log("Show Message: " + msg)
        dialogContent.state = Constants.DLG_STATE.SHOW_MESSAGE;
        labelCMDText.propertyText.text = msg
        labelCMDText.propertyAccessibleText = textMessageTop.text + Functions.filterText(msg)
        labelCMDText.propertyLinkUrl = urlLink
        labelCMDText.propertyText.forceActiveFocus()
    }
}
