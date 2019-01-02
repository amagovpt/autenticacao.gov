import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../scripts/Constants.js" as Constants

Item {       
    y: parent.height * 0.5 - dialogSignCMD.height * 0.5
    
    Connections {
        target: gapi
        onSignalOpenCMDSucess: {
            console.log("Signal Open CMD Sucess")
            progressBarIndeterminate.visible = false
            rectReturnCode.visible = true
            buttonCMDProgressConfirm.visible = true
        }
        onSignalCloseCMDSucess: {
            console.log("Signal Close CMD Sucess")
            progressBarIndeterminate.visible = false
            rectLabelCMDText.visible = true
            buttonCMDProgressConfirm.visible = true
            buttonCMDProgressConfirm.text = qsTranslate("Popup File","STR_POPUP_FILE_OPEN")
        }
        onSignalUpdateProgressBar: {
            console.log("CMD sign change --> update progress bar with value = " + value)
            progressBar.value = value
            if(value === 100) {
                progressBarIndeterminate.visible = false
            }
        }
        onSignalUpdateProgressStatus: {
            console.log("CMD sign change --> update progress status with text = " + statusMessage)
            textMessageTop.text = statusMessage
        }
    }

    Dialog {
        id: dialogSignCMD
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
            + mainView.width * 0.5 - dialogSignCMD.width * 0.5
        y: parent.height 

        header: Label {
            id: labelTextTitle
            text: qsTranslate("PageServicesSign","STR_SIGN_CMD")
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
            height: rectMessageTopLogin.height + rectMobilNumber.height + rectPin.height

            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return
                        && textFieldMobileNumber.length !== 0 && textFieldPin.length !== 0)
                {
                    signCMD()
                }
            }

            Item {
                id: rectMessageTopLogin
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTopLogin
                    text: qsTranslate("PageServicesSign","STR_SIGN_INSERT_LOGIN")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }
            Item {
                id: rectMobilNumber
                width: parent.width
                height: 50
                anchors.top: rectMessageTopLogin.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinCurrent
                    text: qsTranslate("PageServicesSign","STR_SIGN_CMD_MOVEL_NUM")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width * 0.3
                    anchors.bottom: parent.bottom
                }
                ComboBox {
                    id: comboBoxIndicative
                    width: parent.width * 0.4
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    model: PhoneCountriesCodeListModel{}
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    visible: true
                    anchors.left: textPinCurrent.right
                    anchors.bottom: parent.bottom
                    onCurrentIndexChanged: {
                        if(comboBoxIndicative.currentIndex >= 0){
                            propertyPageLoader.propertyBackupMobileIndicatorIndex = comboBoxIndicative.currentIndex
                        }else{
                            comboBoxIndicative.currentIndex = propertyPageLoader.propertyBackupMobileIndicatorIndex
                        }
                    }
                }
                TextField {
                    id: textFieldMobileNumber
                    width: parent.width * 0.25
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldMobileNumber.text === "" ? true: false
                    placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_MOVEL_NUM_OP") + "?"
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: comboBoxIndicative.right
                    anchors.leftMargin:  parent.width * 0.05
                    anchors.bottom: parent.bottom
                    onEditingFinished: {
                        // CMD load backup mobile data
                        propertyPageLoader.propertyBackupMobileNumber = textFieldMobileNumber.text
                    }
                }
            }
            Item {
                id: rectPin
                width: parent.width
                height: 50
                anchors.top: rectMobilNumber.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinNew
                    text: qsTranslate("PageServicesSign","STR_SIGN_CMD_PIN")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
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
                    placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_PIN_OP") + "?"
                    validator: RegExpValidator { regExp: /[0-9]{4,8}/ }
                    echoMode : TextInput.Password
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinNew.right
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectMessage
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectPin.bottom
                Text {
                    id: textLinkCMD
                    textFormat: Text.RichText
                    text: "<a href=\"https://www.autenticacao.gov.pt/cmd-pedido-chave\">"
                            + qsTranslate("PageServicesSign","STR_SIGN_CMD_URL")
                    font.italic: true
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
                }
            }
            Item {
                width: dialogSignCMD.availableWidth
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                y: 190
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CANCEL")
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        dialogSignCMD.close()
                        textFieldPin.text = ""
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    }
                }
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM")
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    enabled: textFieldMobileNumber.acceptableInput && textFieldPin.acceptableInput
                    onClicked: {
                        signCMD()
                    }
                }
            }
        }
        onRejected:{
            // Reject CMD Popup's only with ESC key
            dialogSignCMD.open()
        }
    }
    Dialog {
        id: dialogCMDProgress
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
            + mainView.width * 0.5 - dialogCMDProgress.width * 0.5
        y: parent.height
        focus: true

        header: Label {
            id: labelConfirmOfAddressProgressTextTitle
            text: qsTranslate("PageServicesSign","STR_SIGN_CMD")
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        ProgressBar {
            id: progressBar
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            height: 20
            to: 100
            value: 0
            visible: true
            indeterminate: false
            z:1
        }

        Item {
            width: parent.width
            height: rectMessageTop.height + rectReturnCode.height + progressBarIndeterminate.height
            anchors.top: progressBar.bottom

            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return && buttonCMDProgressConfirm.visible == true)
                {
                    signCMDConfirm()
                }
            }

            Item {
                id: rectMessageTop
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTop
                    text: ""
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
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
                id: rectLabelCMDText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectMessageTop.bottom
                visible: false
                Text {
                    id: labelCMDText
                    text: qsTranslate("PageServicesSign","STR_SIGN_OPEN")
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    wrapMode: Text.Wrap
                }
            }
            Item {
                id: rectReturnCode
                width: parent.width
                height: 50
                anchors.top: rectMessageTop.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                visible: false
                Text {
                    id: textReturnCode
                    text: qsTranslate("PageServicesSign","STR_SIGN_CMD_CODE") + ":"
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
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
                    placeholderText: qsTranslate("PageServicesSign","STR_SIGN_CMD_CODE_OP") + "?"
                    validator: RegExpValidator { regExp: /^[0-9]{6}$/ }
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textReturnCode.right
                    anchors.bottom: parent.bottom
                }
            }

            ProgressBar {
                id: progressBarIndeterminate
                width: parent.width
                anchors.top: rectReturnCode.bottom
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
            width: dialogCMDProgress.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.horizontalCenter: parent.horizontalCenter
            y: 190
            Button {
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CANCEL")
                anchors.left: parent.left
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: {
                    dialogCMDProgress.close()
                    rectReturnCode.visible = false
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                }
            }
            Button {
                property var isOpenFile: false

                id: buttonCMDProgressConfirm
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM")
                anchors.right: parent.right
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                enabled: textFieldReturnCode.acceptableInput || isOpenFile
                visible: false
                onClicked: {
                    signCMDConfirm()
                }
            }
        }
        onRejected:{
            // Reject CMD Popup's only with ESC key
            dialogCMDProgress.open()
        }
    }

    Component.onCompleted: {
        textFieldMobileNumber.text = propertyPageLoader.propertyBackupMobileNumber
    }

    function open() {
        buttonCMDProgressConfirm.isOpenFile = false
        dialogSignCMD.open()
    }
    function signCMD(){
        var loadedFilePath = filesModel.get(0).fileUrl
        var outputFile = propertyFileDialogCMDOutput.fileUrl.toString()

        outputFile = decodeURIComponent(stripFilePrefix(outputFile))

        var page = 1
        if(propertyCheckLastPage.checked) {
            page = gapi.getPDFpageCount(loadedFilePath)
        }else{
            page = propertySpinBoxControl.value
        }

        var isTimestamp = false
        if (typeof propertySwitchSignTemp !== "undefined")
            isTimestamp = propertySwitchSignTemp.checked

        if (typeof propertyTextFieldReason !== "undefined")
            var reason = propertyTextFieldReason.text

        if (typeof propertyTextFieldLocal !== "undefined")
            var location = propertyTextFieldLocal.text

        if (typeof propertyCheckSignReduced !== "undefined")
            var isSmallSignature = propertyCheckSignReduced.checked

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

        console.log("Output filename: " + outputFile)
        console.log("Signing in position coord_x: " + coord_x
                    + " and coord_y: "+coord_y)

        var countryCode = comboBoxIndicative.currentText.substring(0, comboBoxIndicative.currentText.indexOf(' '));
        var mobileNumber = countryCode + " " + textFieldMobileNumber.text

        propertyOutputSignedFile = outputFile
        rectLabelCMDText.visible = false

        if (typeof propertySwitchSignAdd !== "undefined" && propertySwitchSignAdd.checked) {
            gapi.signOpenScapWithCMD(mobileNumber,textFieldPin.text,
                                     loadedFilePath,outputFile,page,
                                     coord_x, coord_y,
                                     reason,location)
        } else {
            gapi.signOpenCMD(mobileNumber,textFieldPin.text,
                             loadedFilePath,outputFile,page,
                             coord_x,coord_y,
                             reason,location,
                             isTimestamp, isSmallSignature)
        }

        progressBarIndeterminate.visible = true
        progressBar.visible = true
        textFieldPin.text = ""
        textFieldReturnCode.text = ""
        dialogSignCMD.close()
        buttonCMDProgressConfirm.visible = false
        buttonCMDProgressConfirm.text = qsTranslate("PageServicesSign","STR_CMD_POPUP_CONFIRM")
        dialogCMDProgress.open()
        textFieldReturnCode.focus = true
    }
    function signCMDConfirm(){
        console.log("Send sms_token : " + textFieldReturnCode.text)
        buttonCMDProgressConfirm.isOpenFile = true
        if( progressBar.value < 100) {
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
                if(count == 0) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("PageServicesSign","STR_SCAP_ATTRIBUTES_NOT_SELECT")
                    mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
                    return
                }
            }
            gapi.signCloseCMD(textFieldReturnCode.text, attributeList)
            progressBarIndeterminate.visible = true
            rectReturnCode.visible = false
            buttonCMDProgressConfirm.visible = false
            textFieldReturnCode.text = ""
            dialogCMDProgress.open()
        }
        else
        {
            dialogCMDProgress.close()
            if (Qt.platform.os === "windows") {
                propertyOutputSignedFile = "file:///" + propertyOutputSignedFile
            }else{
                propertyOutputSignedFile = "file://" + propertyOutputSignedFile
            }
            console.log("Open Url Externally: " + propertyOutputSignedFile)
            Qt.openUrlExternally(propertyOutputSignedFile)
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
    }
}
