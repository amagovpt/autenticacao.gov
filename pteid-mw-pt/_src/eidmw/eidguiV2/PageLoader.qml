import QtQuick 2.6
import QtQuick.Controls 2.1

import "scripts/Constants.js" as Constants

Loader{
    id: theloader
    opacity:1
    width: parent.width
    height: parent.height

    property alias propertyGeneralPopUp: generalPopUp
    property alias propertyGeneralTitleText: titleText
    property alias propertyGeneralPopUpLabelText: labelText

    Dialog {
        id: generalPopUp
        width: 400
        height: 200
        visible: false

        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - generalPopUp.width * 0.5
        y: parent.height * 0.5 - generalPopUp.height * 0.5

        header: Label {
            id: titleText
            text: "Error"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            width: generalPopUp.width
            height: generalPopUp.height

            Item {
                id: rectLabelText
                width: parent.width
                height: 90

                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: labelText
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    anchors.bottom: parent.bottom
                    wrapMode: Text.Wrap
                }
            }
            Button {
                width: Constants.WIDTH_BUTTON
                height:Constants.HEIGHT_BOTTOM_COMPONENT
                text: "OK"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectLabelText.bottom
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: generalPopUp.close()
            }
        }
    }

    focus: true
    anchors{
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
    }
}
