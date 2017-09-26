import QtQuick 2.6
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent
    property alias propertyButtonSearch: buttonSearch
    property alias propertyProgressBar: progressBar
    property alias propertyTextDescription: textDescription
    property alias propertyButtonStartUpdate: buttonStartUpdate
    Item {
        width: parent.width
        height: parent.height

        Item {
            id: rowTop
            width: parent.width
            height: parent.height * Constants.HEIGHT_DEFINITIONS_UPDATES_ROW_TOP_V_RELATIVE
                    + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                    * Constants.HEIGHT_DEFINITIONS_UPDATES_ROW_TOP_INC_RELATIVE
        }

        Text {
            id: textTitle
            font.pixelSize: Constants.SIZE_TEXT_LABEL
            font.family: lato.name
            text: qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TITLE")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_LABEL
            Layout.fillWidth: true
            anchors.top: rowTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        }
        Text {
            id: textDescription
            anchors.top: textTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TEXT")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
        Item {
            id: textProgressBar
            anchors.top: textDescription.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width
            height: 50
            ProgressBar {
                id: progressBar
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                height: 20
                to: 100
                value: 0
                visible: false
                indeterminate: false
                z:1
            }
        }
        Item {
            id: rawButtonSearch
            anchors.top: textProgressBar.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            Button {
                id: buttonSearch
                text: qsTranslate("PageDefinitionsUpdates","STR_UPDATE_BUTTON")
                width: Constants.WIDTH_BUTTON
                height: parent.height
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Item {
            id: rawButtonStartUpdate
            anchors.top: rawButtonSearch.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            Button {
                id: buttonStartUpdate
                text: qsTranslate("PageDefinitionsUpdates","STR_UPDATE_BUTTON_START")
                width: Constants.WIDTH_BUTTON
                height: parent.height
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
                visible: false
            }
        }
    }
}
