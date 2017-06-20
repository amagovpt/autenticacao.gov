import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent
    Item {
        width: parent.width * Constants.HOME_PAGE_RELATIVE_H_SIZE
        height: parent.height * Constants.HOME_PAGE_RELATIVE_V_SIZE
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        Text {
            id: textTitle
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            font.family: lato.name
            text: mainWindow.title
            font.bold: true
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_TITLE
            Layout.fillWidth: true
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
        RowLayout {
            id: rawButtonSearch
            anchors.top: textDescription.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width

            Button {
                id: buttonSearch
                text: "Procurar atualizações"
                width: 150
                height:Constants.SIZE_V_BOTTOM_COMPONENT
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
