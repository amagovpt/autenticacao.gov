import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants

Item {
    property alias propertyTextLinkCC: textLinkCC

    anchors.fill: parent
    Item {
        width: parent.width * Constants.HOME_PAGE_RELATIVE_H_SIZE
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            id: text0
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            font.family: lato.name
            text: "Autenticação.gov.pt"
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
        RowLayout {
            id: raw5
            anchors.top: raw4.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width

            Text {
                id: textLinks
                text: "Documentação online:"
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
            }
        }

        RowLayout {
            id: rawLinks
            anchors.top: raw5.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width


            Rectangle {
                width: Constants.SIZE_TEXT_BODY
                height: Constants.SIZE_TEXT_BODY
                color: Constants.COLOR_ITEM_BULLETED
            }
            Rectangle {
                width: Constants.SIZE_TEXT_BODY
                height: Constants.SIZE_TEXT_BODY
            }
            Text {
                id: textLinkCC
                textFormat: Text.RichText
                text: "<a href=\"https://www.autenticacao.gov.pt/documents/10179/11465/\
Manual+de+Utiliza%C3%A7%C3%A3o+da+Aplica%C3%A7%C3%A3o+do+Cart%C3%A3o+de+Cidad%C3%A3o\">
Manual de Utilização da Aplicação do Cartão de Cidadão</a>"
                font.italic: true
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
            }
        }
    }
}
