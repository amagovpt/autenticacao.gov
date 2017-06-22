import QtQuick 2.6
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent
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
            text: "Atualizações Automáticas"
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
            text: "Pressione o botão para verificar se existem atualizações automáticas disponíveis."
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
        Item {
            id: rawButtonSearch
            anchors.top: textDescription.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            Button {
                id: buttonSearch
                text: "Procurar atualizações"
                width: Constants.WIDTH_BUTTON
                height: parent.height
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
