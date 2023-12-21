import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.12

import eidguiV2 1.0
import "../../scripts/Constants.js" as Constants

Dialog {
        id: dialogCAN
        width: 400
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogCAN.width * 0.5
        y: parent.height * 0.5 - dialogCAN.height * 0.5
        onOpened: {
            textFieldCAN.forceActiveFocus()
        }

        Connections {
            target: gapi
            onErrorPace: {
                errorMessageLabel.visible = true
                textFieldCAN.enabled = true
                if(error_code === GAPI.PaceBadToken)
                    errorMessageLabel.text = qsTranslate("Popup PIN","STR_POPUP_PACE_BAD_TOKEN")
                else {
                    errorMessageLabel.text = qsTranslate("Popup PIN","STR_POPUP_PACE_UNKNOWN")
                }
            }
            onPaceSuccess: {
                close()
            }
        }

        property int afterPaceAction: GAPI.IdentityData

        header: Label {
            id: headLabel
            text: qsTranslate("Popup PIN", "STR_POPUP_CAN_TITLE")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        ColumnLayout {
            id: contentItem
            spacing: 10
            width: parent.width
            Text {
                id: textCAN
                Layout.preferredWidth: parent.width
                text: qsTranslate("Popup PIN","STR_POPUP_CAN_TEXT")
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                width: parent.width
                wrapMode: Text.WordWrap
            }

            Item {
                id: rectImageCAN
                width: parent.width
                Layout.preferredHeight: childrenRect.height
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "../../images/CAN_image.png"
                }

            }

            Item {
                id: rectPin
                width: parent.width * 0.5
                Layout.preferredHeight: 30
                Layout.alignment: Qt.AlignCenter
                Rectangle{
                    id: borderRectPin
                    border.color: Constants.COLOR_MAIN_BLUE
                    border.width: 2
                    anchors.fill: parent
                    TextField {
                        id: textFieldCAN
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
                        horizontalAlignment: TextInput.AlignHCenter
                        Keys.onEnterPressed: okButton.clicked()
                        Keys.onReturnPressed: okButton.clicked()
                        onTextChanged: {
                            errorMessageLabel.visible = false
                        }
                    }
                }
            }
            Item {
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                width: errorMessageLabel.width
                Label {
                    id: errorMessageLabel
                    text: qsTranslate("Popup PIN","STR_POPUP_PACE_UNKNOWN")
                    color: "red"
                    visible: false
                }
            }
        }

        footer: DialogButtonBox {
            id: footerDialog
            alignment: Qt.AlignHCenter
                Button {
                    id: rejectButton
                    text: qsTranslate("Popup PIN","STR_POPUP_CAN_CANCEL")
                    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false
                    implicitWidth: Constants.WIDTH_BUTTON
                    implicitHeight: Constants.HEIGHT_BOTTOM_COMPONENT

                    contentItem: Text {
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
                    enabled: textFieldCAN.length == 6 && textFieldCAN.enabled
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false
                    implicitWidth: Constants.WIDTH_BUTTON
                    implicitHeight: Constants.HEIGHT_BOTTOM_COMPONENT
                    Keys.onEnterPressed: okButton.clicked()
                    Keys.onReturnPressed: okButton.clicked()
                    onClicked: {
                        textFieldCAN.forceActiveFocus()
                        startPaceFunction()
                    }

                    contentItem: Text {
                        text : okButton.text
                        font: okButton.font
                        color: okButton.enabled ? Constants.COLOR_MAIN_BLACK : Constants.COLOR_GRAY
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                onApplied: {
                    startPaceFunction()
                }
                onRejected: {
                    mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    propertyBusyIndicator.running = false
                }
            }

        function startPaceFunction() {
            if(textFieldCAN.length != 6)
                return
            textFieldCAN.enabled = false
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            gapi.startPACEAuthentication(textFieldCAN.text, afterPaceAction)
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
    }
