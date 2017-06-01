import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import "../../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent
    Item {
        width: parent.width * Constants.HOME_PAGE_RELATIVE_H_SIZE
        height: parent.height * Constants.HOME_PAGE_RELATIVE_V_SIZE
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        Text {
            id: text0
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            font.family: lato.name
            text: "Autenticação.gov.pt"
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
        Text {
            id: text1
            anchors.top: text0.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: "A Aplicação Autenticação.gov.pt permite ao cidadão tirar partido das funcionalidades eletrónicas do \
seu Cartão de Cidadão e da sua Chave Móvel Digital."
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
        Text {
            id: text2
            anchors.top: text1.bottom
            anchors.topMargin: 3 * Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_TITLE
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
            Rectangle {
                height: Constants.SIZE_TEXT_BODY
                width: Constants.SIZE_TEXT_BODY
                color: Constants.COLOR_ITEM_BULLETED
            }
            Rectangle {
                height: Constants.SIZE_TEXT_BODY
                width: Constants.SIZE_TEXT_BODY
            }
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
            Rectangle {
                height: Constants.SIZE_TEXT_BODY
                width: Constants.SIZE_TEXT_BODY
                color: Constants.COLOR_ITEM_BULLETED
            }
            Rectangle {
                height: Constants.SIZE_TEXT_BODY
                width: Constants.SIZE_TEXT_BODY
            }
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
            Rectangle {
                height: Constants.SIZE_TEXT_BODY
                width: Constants.SIZE_TEXT_BODY
                color: Constants.COLOR_ITEM_BULLETED
            }
            Rectangle {
                height: Constants.SIZE_TEXT_BODY
                width: Constants.SIZE_TEXT_BODY
            }
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
            Rectangle {
                height: Constants.SIZE_TEXT_BODY
                width: Constants.SIZE_TEXT_BODY
                color: Constants.COLOR_ITEM_BULLETED
            }
            Rectangle {
                height: Constants.SIZE_TEXT_BODY
                width: Constants.SIZE_TEXT_BODY
            }
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
    }
}
