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
            propertyBusyIndicator.running = false
            if (error_code === GAPI.NoReaderFound) {
                console.log("Error: No card reader found!")
                dialogError.propertyDialogErrorLabelText.text = "Error: No card reader found!"
            }
            else if (error_code === GAPI.NoCardFound) {
                console.log("Error: No Card Found!")
                dialogError.propertyDialogErrorLabelText.text = "Error: No Card Found!"
            }else {
                console.log("Error: Reading Card Error")
                dialogError.propertyDialogErrorLabelText.text = "Error: Reading Card Error!"
            }

            propertyTriesLeftAuthPin.text = ""
            propertyTriesLeftSignPin.text = ""
            propertyTriesLeftAddressPin.text = ""
            propertyButtonModifyAuth.enabled = false
            propertyButtonModifySign.enabled = false
            propertyButtonModifyAddress.enabled = false
            propertyButtonTestAuth.enabled = false
            propertyButtonTestSign.enabled = false
            propertyButtonTestAddress.enabled = false

            dialogError.open()
        }
    }

    Dialog {
        id: dialogError
        width: 400
        height: 200
        visible: false

        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogError.width * 0.5
        y: parent.height * 0.5 - dialogError.height * 0.5

        property alias propertyDialogErrorLabelText: dialogErrorLabelText

        header: Label {
            id: dialogErrorLabelText
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }
        standardButtons: DialogButtonBox.Ok
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
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectOkPin.height

            Item {
                id: rectOkPin
                width: textTypePin.width + textFieldPin.width
                height: 50

                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textOkPin
                    text: "PIN introduzido correctamente."
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
        standardButtons: DialogButtonBox.Ok
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
            text: "Verificação de PIN"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectBadPin.height

            Item {
                id: rectBadPin
                width: textTypePin.width + textFieldPin.width
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
        standardButtons: DialogButtonBox.Ok
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
            font.pixelSize: 16
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
                    placeholderText: "PIN Atual?"
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
                    propertyTriesLeftAuthPin.text = "PIN bloqueado!"
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAuthPin.text = ""
                }
                else {
                    propertyTriesLeftAuthPin.text = "Restam "+ triesLeft + " tentativas."
                }
                break;
            case 1:
                triesLeft = gapi.verifySignPin(textFieldPin.text)
                if (triesLeft === 0) {
                    propertyTriesLeftSignPin.text = "PIN bloqueado!"
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftSignPin.text = ""
                }
                else {
                    propertyTriesLeftSignPin.text = "Restam "+ triesLeft + " tentativas."
                }
                break;
            case 2:
                triesLeft = gapi.verifyAddressPin(textFieldPin.text)
                if (triesLeft === 0) {
                    propertyTriesLeftAddressPin.text = "PIN bloqueado!"
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAddressPin.text = ""
                }
                else {
                    propertyTriesLeftAddressPin.text = "Restam "+ triesLeft + " tentativas."
                }
                break;
            default:
                break
            }

            dialogTestPin.visible = false
            mainFormID.opacity = 1.0
            propertyBusyIndicator.running = false

            if (triesLeft === 3) {
                dialogPinOK.propertyDialogOkLabelText.text = "Verificação de PIN"
                dialogPinOK.propertyTextOkPin.text = "PIN introduzido correctamente"
                dialogPinOK.open()
            }
            else if (triesLeft === 0) {
                textBadPin.text = "PIN bloqueado!"
                dialogBadPin.open()
            }
            else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                textBadPin.text = "Erro na leitura do cartão!"
                dialogBadPin.open()
            }
            else {
                textBadPin.text = "O PIN introduzido está errado! \n\n" + "Restam "+ triesLeft + " tentativas."
                dialogBadPin.open()
            }
        }
        onRejected: {
            mainFormID.opacity = 1.0
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
            font.pixelSize: 16
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
                    text: "PIN Atual"
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
                    placeholderText: "PIN Atual?"
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
                    text: "Novo PIN"
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
                    placeholderText: "Novo PIN?"
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
                    text: "Confirmar novo PIN"
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
                    placeholderText: "Confirmar novo PIN?"
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
                        propertyTriesLeftAuthPin.text = "PIN bloqueado!"
                    }
                    else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                        propertyTriesLeftAuthPin.text = ""
                    }
                    else {
                        propertyTriesLeftAuthPin.text = "Restam "+ triesLeft + " tentativas."
                    }
                    break;
                case 1:
                    triesLeft = gapi.changeSignPin(textFieldPinCurrent.text,textFieldPinNew.text)
                    if (triesLeft === 0) {
                        propertyTriesLeftSignPin.text = "PIN bloqueado!"
                    }
                    else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                        propertyTriesLeftSignPin.text = ""
                    }
                    else {
                        propertyTriesLeftSignPin.text = "Restam "+ triesLeft + " tentativas."
                    }
                    break;
                case 2:
                    triesLeft = gapi.changeAddressPin(textFieldPinCurrent.text,textFieldPinNew.text)
                    if (triesLeft === 0) {
                        propertyTriesLeftAddressPin.text = "PIN bloqueado!"
                    }
                    else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                        propertyTriesLeftAddressPin.text = ""
                    }
                    else {
                        propertyTriesLeftAddressPin.text = "Restam "+ triesLeft + " tentativas."
                    }
                    break;
                default:
                    break
                }

                dialogTestPin.visible = false
                mainFormID.opacity = 1.0
                propertyBusyIndicator.running = false

                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = "Modificação de PIN"
                    dialogPinOK.propertyTextOkPin.text = "Alteração de PIN efectuada"
                    dialogPinOK.open()
                }
                else if (triesLeft === 0) {
                    textBadPin.text = "PIN bloqueado!"
                    dialogBadPin.open()
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    textBadPin.text = "Erro na leitura do cartão!"
                    dialogBadPin.open()
                }
                else {
                    textBadPin.text = "O PIN introduzido está errado! \n\n" + "Restam "+ triesLeft + " tentativas."

                    dialogBadPin.open()
                }
            }else{
                propertyBusyIndicator.running = false
                textPinMsgConfirm.text = "Novo PIN e PIN de confirmação têm de ser iguais"
                dialogModifyPin.open()
            }
        }
        onRejected: {
            mainFormID.opacity = 1.0
        }
    }

    // Buttons actions
    propertyButtonModifyAuth{
        onClicked: {
            mainFormID.opacity = 0.5
            dialogModifyPin.propertyLabelModifyTextTitle.text = "Modificar o Pin de Autenticação"
            dialogModifyPin.propertyTextFieldPinCurrent.text = ""
            dialogModifyPin.propertyTextFieldPinNew.text = ""
            dialogModifyPin.propertyTextFieldPinConfirm.text = ""
            dialogModifyPin.propertyTextPinMsgConfirm.text = ""
            dialogModifyPin.open()
        }
    }
    propertyButtonTestAuth{
        onClicked: {
            mainFormID.opacity = 0.5
            dialogTestPin.propertyLabelTextTitle.text = "Verificar o Pin da Autenticação"
            dialogTestPin.propertyTextTypePin.text = "PIN da Autenticação"
            dialogTestPin.propertyTextFieldPin.text = ""
            dialogTestPin.open()
        }
    }
    propertyButtonModifySign{
        onClicked: {
            mainFormID.opacity = 0.5
            dialogModifyPin.propertyLabelModifyTextTitle.text = "Modificar o Pin da Assinatura"
            dialogModifyPin.propertyTextFieldPinCurrent.text = ""
            dialogModifyPin.propertyTextFieldPinNew.text = ""
            dialogModifyPin.propertyTextFieldPinConfirm.text = ""
            dialogModifyPin.propertyTextPinMsgConfirm.text = ""
            dialogModifyPin.open()
        }
    }
    propertyButtonTestSign{
        onClicked: {
            mainFormID.opacity = 0.5
            dialogTestPin.propertyLabelTextTitle.text = "Verificar o Pin da Assinatura"
            dialogTestPin.propertyTextTypePin.text = "PIN da Assinatura"
            dialogTestPin.propertyTextFieldPin.text = ""
            dialogTestPin.open()
        }
    }
    propertyButtonModifyAddress{
        onClicked: {
            mainFormID.opacity = 0.5
            dialogModifyPin.propertyLabelModifyTextTitle.text = "Modificar o Pin da Morada"
            dialogModifyPin.propertyTextFieldPinCurrent.text = ""
            dialogModifyPin.propertyTextFieldPinNew.text = ""
            dialogModifyPin.propertyTextFieldPinConfirm.text = ""
            dialogModifyPin.propertyTextPinMsgConfirm.text = ""
            dialogModifyPin.open()
        }
    }
    propertyButtonTestAddress{
        onClicked: {
            mainFormID.opacity = 0.5
            dialogTestPin.propertyLabelTextTitle.text = "Verificar o Pin da Morada"
            dialogTestPin.propertyTextTypePin.text = "PIN da Morada"
            dialogTestPin.propertyTextFieldPin.text = ""
            dialogTestPin.open()
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
                    propertyTriesLeftAuthPin.text = "PIN bloqueado!"
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAuthPin.text = ""
                }
                else {
                    propertyTriesLeftAuthPin.text = "Restam "+ triesLeft + " tentativas."
                    propertyButtonModifyAuth.enabled = true
                    propertyButtonTestAuth.enabled = true
                }
                break;
            case 1:
                console.log("Sign Pin tries left = " + gapi.getTriesLeftSignPin())
                triesLeft = gapi.getTriesLeftSignPin()
                if (triesLeft === 0) {
                    propertyTriesLeftSignPin.text = "PIN bloqueado!"
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftSignPin.text = ""
                }
                else {
                    propertyTriesLeftSignPin.text = "Restam "+ triesLeft + " tentativas."
                    propertyButtonModifySign.enabled = true
                    propertyButtonTestSign.enabled = true
                }
                break;
            case 2:
                console.log("Address Pin tries left = " + gapi.getTriesLeftAddressPin())
                triesLeft = gapi.getTriesLeftAddressPin()
                if (triesLeft === 0) {
                    propertyTriesLeftAddressPin.text = "PIN bloqueado!"
                }
                else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
                    propertyTriesLeftAddressPin.text = ""
                }
                else {
                    propertyTriesLeftAddressPin.text = "Restam "+ triesLeft + " tentativas."
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
            propertyTriesLeftAuthPin.text = "PIN bloqueado!"
        }
        else if (triesLeft === Constants.TRIES_LEFT_ERROR) {
            propertyTriesLeftAuthPin.text = ""
        }
        else {
            propertyTriesLeftAuthPin.text = "Restam "+ triesLeft + " tentativas."
            propertyButtonModifyAuth.enabled = true
            propertyButtonTestAuth.enabled = true

        }
        propertyBusyIndicator.running = false
    }
}
