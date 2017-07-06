import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components/" as Components

PageSecurityPinCodesForm {


    Dialog {
        id: verifypin_dialog
        width: 400
        height: 200
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - changepin_dialog.width * 0.5
        y: parent.height * 0.5 - changepin_dialog.height * 0.5

        header: Label {
              text: "Verificar o Pin da Morada"
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
                width: textPin.width + textFieldPin.width
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
                    width: 150
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldPin
                    width: 150
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPin.text === "" ? true: false
                    placeholderText: "PIN Atual?"
                    echoMode : TextInput.Password
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPin.right
                    anchors.leftMargin: 20
                    anchors.bottom: parent.bottom
                }
            }
       }

        /*
        footer: DialogButtonBox {
            Button {
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

                contentItem: Text {
                    text: qsTr("OK")
                    font: lato.name
                    color: Constants.COLOR_MAIN_PRETO
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    width: 100
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    color: Constants.COLOR_GREY_BUTTON_BACKGROUND
                    radius: 2
                }
            }
        }
        */
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

        onAccepted: {
            mainFormID.opacity = 1.0
            if(gapi.verifyAddressPin(textFieldPin.text))
                pinOK_dialog.open()
        }
        onRejected: {
            mainFormID.opacity = 1.0
        }
    }

    Dialog {
            id: pinOK_dialog
            width: 400
            height: 200
            // Center dialog in the main view
            x: - mainMenuView.width - subMenuView.width
               + mainView.width * 0.5 - pinOK_dialog.width * 0.5
            y: parent.height * 0.5 - pinOK_dialog.height * 0.5
            font.family: lato.name
            font.pixelSize: 16
            title: "PIN OK!"
            standardButtons: Dialog.Ok
            onAccepted: console.log("Ok clicked")
    }

    Dialog {
        id: changepin_dialog
        width: 400
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - changepin_dialog.width * 0.5
        y: parent.height * 0.5 - changepin_dialog.height * 0.5

        header: Label {
              text: "Modificar o Pin da Morada"
              visible: true
              elide: Label.ElideRight
              padding: 24
              bottomPadding: 0
              font.bold: true
              font.pixelSize: 16
              color: Constants.COLOR_MAIN_BLUE
       }

        Item{
            width: parent.width
            height: rectPinCurrent.height + rectPinNew.height + rectPinConfirm.height

            Item {
                id: rectPinCurrent
                width: textPinCurrent.width + textFieldPinCurrent.width
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
                    width: 150
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldPinCurrent
                    width: 150
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPinCurrent.text === "" ? true: false
                    placeholderText: "PIN Atual?"
                    echoMode : TextInput.Password
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinCurrent.right
                    anchors.leftMargin: 20
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectPinNew
                width: textPinNew.width + textFieldPinNew.width
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
                    width: 150
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldPinNew
                    width: 150
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPinNew.text === "" ? true: false
                    placeholderText: "Novo PIN?"
                    echoMode : TextInput.Password
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinNew.right
                    anchors.leftMargin: 20
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectPinConfirm
                width: textPinConfirm.width + textFieldPinConfirm.width
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
                    width: 150
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldPinConfirm
                    width: 150
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPinConfirm.text === "" ? true: false
                    placeholderText: "Confirmar novo PIN?"
                    echoMode : TextInput.Password
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinConfirm.right
                    anchors.leftMargin: 20
                    anchors.bottom: parent.bottom
                }
            }
        }
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            mainFormID.opacity = 1.0
        }
        onRejected: {
            mainFormID.opacity = 1.0
        }
    }
    propertyButtonModify{
        onClicked: {
            mainFormID.opacity = 0.5
            changepin_dialog.open()
        }
    }
    propertyButtonTest{
        onClicked: {
            mainFormID.opacity = 0.5
            verifypin_dialog.open()
        }
    }
    Component {
        id: pinCodesMenuDelegate
        Item {
            width: propertyPinCodesListView.width
            height: propertyPinCodesViewLeft.height * Constants.SUB_MENU_RELATIVE_V_ITEM_SIZE
            MouseArea {
                id: mouseAreaPinCodes
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    propertyPinCodesListView.currentIndex = index
                    console.log("Pin Codes index = " + index);
                }
            }
            Text {
                text: name
                color: getPinCodesColor(index, mouseAreaPinCodes.containsMouse)
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.weight: mouseAreaPinCodes.containsMouse ?
                                 Font.Bold :
                                 Font.Normal
                font.pixelSize: Constants.SIZE_TEXT_SUB_MENU
                wrapMode: Text.Wrap
                width: parent.width - imageArrowPinCodes.width
                horizontalAlignment: Text.AlignHCenter

            }
            Image {
                id: imageArrowPinCodes
                width: Constants.SIZE_IMAGE_ARROW_SUB_MENU
                height: Constants.SIZE_IMAGE_ARROW_SUB_MENU
                fillMode: Image.PreserveAspectFit
                x: parent.width * Constants.IMAGE_ARROW_SUB_MENU_RELATIVE
                anchors.verticalCenter: parent.verticalCenter
                source: getPinCodesArrowSource(index, mouseAreaPinCodes.containsMouse)
            }
        }
    }

    function getPinCodesColor(index, containsMouse)
    {
        var handColor
        if(propertyPinCodesListView.currentIndex === index)
        {
            handColor =  Constants.COLOR_TEXT_SUB_MENU_SELECTED
        }else{
            if(containsMouse === true)
            {
                handColor = Constants.COLOR_TEXT_SUB_MENU_MOUSE_OVER
            }else{
                handColor = Constants.COLOR_TEXT_SUB_MENU_DEFAULT
            }
        }
        return handColor
    }

    function getPinCodesArrowSource(index, containsMouse)
    {
        var handSource
        if(propertyPinCodesListView.currentIndex === index)
        {
            handSource =  "../../images/arrow-right_white_AMA.png"
        }else{
            if(containsMouse === true)
            {
                handSource = "../../images/arrow-right_hover.png"
            }else{
                handSource = ""
            }
        }
        return handSource
    }
}
