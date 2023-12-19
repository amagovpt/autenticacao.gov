import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQml 2.15

Component {
    Dialog {
        id: dialogCAN
        width: 400
        height: 250
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogCAN.width * 0.5
        y: parent.height * 0.5 - dialogCAN.height * 0.5

        onOpened: {
            textFieldCAN.forceActiveFocus()
        }

        header: Label {
            text: qsTranslate("Popup up", "Dialog request pace secret")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectPin.height + rectTextCAN.height

            Item {
                id: rectTextCAN
                width: parent.width
                height: 100
                y : parent.y - dialogCAN.height * 0.15
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: textCAN
                    text: qsTranslate("Popup PIN","STR_POPUP_CAN_TEXT")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
            }

            Item {
                id: rectImageCAN
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                y : parent.y + dialogCAN.height * 0.2

                Image{
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "../../images/CAN_image.png"
                }

            }

            Item {
                id: rectPin
                width: parent.width * 0.5
                height: 30
                y : parent.y + dialogCAN.height * 0.4
                anchors.horizontalCenter: parent.horizontalCenter
                Rectangle{
                    id: borderRectPin
                    border.color: Constants.COLOR_MAIN_BLUE
                    border.width: 2
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.fill: parent
                    TextField {
                        id: textFieldCAN
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width / 3
                        anchors.topMargin: 10
                        width: parent.width
                        font.italic: textFieldCAN.text === "" ? true: false
                        placeholderText: ""
                        echoMode : TextInput.Password
                        background:null
                        validator: RegExpValidator { regExp: /[0-9]+/ }
                        maximumLength: 6
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: false
                        anchors.horizontalCenter: parent.horizontalCenter
                        //anchors.bottom: parent.bottom
                        horizontalAlignment: TextField.horizontalAlignment
                    }

                }
            }
        }

        footer: DialogButtonBox {
            alignment: Qt.AlignHCenter
                Button{
                    id: rejectButton
                    text: qsTranslate("Popup PIN","STR_POPUP_CAN_CANCEL")
                    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false

                    contentItem: Text{
                        text : rejectButton.text
                        font: rejectButton.font
                        color: Constants.COLOR_MAIN_BLACK
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Keys.onEscapePressed: clicked()
                }
                Button {
                    id: okButton
                    text: qsTranslate("Popup PIN", "STR_POPUP_CAN_OK")
                    enabled: textFieldCAN.length == 6
                    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false

                    contentItem: Text {
                        text : okButton.text
                        font: okButton.font
                        color: okButton.enabled ? Constants.COLOR_MAIN_BLACK : Constants.COLOR_GRAY
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Keys.onEnterPressed: clicked();
                }
                onAccepted: {
                    mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
                    gapi.startPACEAuthentication(textFieldCAN.text, GAPI.IdentityData)
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS

                }
                onRejected: {
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    console.log("Pressed cancel")
                    propertyBusyIndicator.running = false
                }
            }
    }
}
