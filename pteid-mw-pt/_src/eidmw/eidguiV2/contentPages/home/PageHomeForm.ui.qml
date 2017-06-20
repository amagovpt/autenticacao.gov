import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_HOME_PAGE_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_HOME_PAGE_ROW_TOP_INC_RELATIVE
    }
    Item {
        width: parent.width - 2 * Constants.SIZE_ROW_H_SPACE
        height: parent.height - 2 * Constants.SIZE_ROW_V_SPACE
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        Text {
            id: text0
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            font.family: lato.name
            text: "Aplicação " + mainWindow.title
            font.bold: true
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_TITLE
            Layout.fillWidth: true
        }
        Text {
            id: text1
            anchors.top: text0.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: "A Aplicação " + mainWindow.title + " permite ao cidadão tirar partido das funcionalidades eletrónicas \
do seu Cartão de Cidadão e da sua Chave Móvel Digital."
            wrapMode: Text.Wrap
            width: parent.width
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
        Text {
            id: text2
            anchors.top: text1.bottom
            anchors.topMargin: 3 * Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: "Aqui poderá, entre outras funcionalidades:"
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
        RowLayout {
            id: raw1
            anchors.top: text2.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width

            Components.TextBullet{}
            Text {
                id: text3
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                text: "Ler os dados residentes no chip do seu Cartão de Cidadão"
                wrapMode: Text.Wrap
                width: parent.width
                horizontalAlignment: Text.left
                color: Constants.COLOR_TEXT_BODY
                Layout.fillWidth: true
            }

        }
        RowLayout {
            id: raw2
            anchors.top: raw1.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width

            Components.TextBullet{}
            Text {
                id: text4

                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                text: "Alterar os códigos PIN associados ao seu Cartão de Cidadão"
                wrapMode: Text.Wrap
                width: parent.width
                horizontalAlignment: Text.left
                color: Constants.COLOR_TEXT_BODY
                Layout.fillWidth: true
            }
        }
        RowLayout {
            id: raw3
            anchors.top: raw2.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width

            Components.TextBullet{}
            Text {
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                text: "Confirmar e atualizar a morada no chip do seu Cartão de Cidadão"
                wrapMode: Text.Wrap
                width: parent.width
                horizontalAlignment: Text.left
                color: Constants.COLOR_TEXT_BODY
                Layout.fillWidth: true
            }
        }
        RowLayout {
            id: raw4
            anchors.top: raw3.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width

            Components.TextBullet{}
            Text {
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                text: "Assinar documentos, enquanto cidadão, profissional ou empresário, recorrendo ao seu Cartão de \
Cidadão ou à sua Chave Móvel Digital."
                width: parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.left
                color: Constants.COLOR_TEXT_BODY
                Layout.fillWidth: true
            }
        }
        RowLayout {
            id: rawCheckBox
            anchors.top: raw4.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            width: parent.width
            CheckBox {
                text: "Não voltar a mostrar"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                checked: false
            }
        }
    }
}
