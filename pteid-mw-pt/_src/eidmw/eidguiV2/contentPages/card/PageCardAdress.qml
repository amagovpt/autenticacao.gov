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
            dialogTestPin.visible = false
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

    Component.onCompleted: {
        console.log("Page Card Address Completed")
        if (gapi.isAddressLoaded) {
            propertyBusyIndicator.running = true
            gapi.startReadingAddress()
        }else{
            textFieldPin.text = ""
            dialogTestPin.open()
        }
    }
}
