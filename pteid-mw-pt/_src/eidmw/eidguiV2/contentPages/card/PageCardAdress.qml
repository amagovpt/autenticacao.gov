import QtQuick 2.6
import QtQuick.Controls 2.1
import "../../scripts/Constants.js" as Constants
//Import C++ defined enums
import eidguiV2 1.0

PageCardAdressForm {

    Connections {
        target: gapi
        onSignalAddressLoaded: {
            console.log("Address --> Data Changed")
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
            propertyBusyIndicator.running = false

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
            text: "Verificar o Pin da Morada"
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
                    id: textPin
                    text: "PIN da Morada"
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
            mainFormID.opacity = 1.0
            if (triesLeft === 3) {
                propertyBusyIndicator.running = true
                gapi.startReadingAddress()
            }
            else if (triesLeft === 0) {
                textBadPin.text = "PIN de morada bloqueado!"
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
            text: "Confirmação de Morada"
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
            height: rectMessage.height + rectNumProcess.height + rectConfirmAddress.height

            Item {
                id: rectMessage
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinMsgConfirm
                    text: "Por favor insira o número de processo e o código secreto recebido pelo correio"
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

            Item {
                id: rectNumProcess
                width: parent.width
                height: 50
                anchors.top: rectMessage.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinCurrent
                    text: "Nº de Processo de alteração de Morada"
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
                    placeholderText: "Número de processo?"
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
                    text: "Código de Confirmação de Morada"
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
                    placeholderText: "Código de confirmação de Morada?"
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinNew.right
                    anchors.bottom: parent.bottom
                }
            }
        }

        standardButtons: {
            textFieldNumProcess.length !== 0 && textFieldConfirmAddress.length !== 0
                    ? DialogButtonBox.Ok | DialogButtonBox.Cancel : DialogButtonBox.Cancel
        }

        onAccepted: {
            gapi.changeAddress(textFieldNumProcess.text,textFieldConfirmAddress.text)
            progressBarIndeterminate.visible = true
            textFieldNumProcess.text = ""
            textFieldConfirmAddress.text = ""
            dialogConfirmOfAddressProgress.open()
        }
        onRejected: {
            mainFormID.opacity = 1.0
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
            text: "Confirmação de Morada"
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
            height: rectMessageTop.height + progressBar.height + progressBarIndeterminate.height

            Item {
                id: rectMessageTop
                width: parent.width
                height: 150
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTop
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

        standardButtons: {
            DialogButtonBox.Ok
        }

        onAccepted: {
            mainFormID.opacity = 1.0
        }
        onRejected: {
            mainFormID.opacity = 1.0
        }
    }


    propertyButtonConfirmOfAddress{
        onClicked: {
            mainFormID.opacity = 0.5
            dialogConfirmOfAddress.open()
        }

    }

    Component.onCompleted: {
        console.log("Page Card Address Completed")
        if (gapi.isAddressLoaded) {
            propertyBusyIndicator.running = true
            gapi.startReadingAddress()
        }else{
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
    }
}
