import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias propertyTextLinkCC: textLinkCC

    anchors.fill: parent
    Item {
        width: parent.width
        height: parent.height
        Item {
            id: rowTop
            width: parent.width
            height: parent.height * Constants.HEIGHT_DOC_ONLINE_ROW_TOP_V_RELATIVE
                    + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                    * Constants.HEIGHT_DOC_ONLINE_ROW_TOP_INC_RELATIVE
        }
        Text {
            id: text0
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            font.family: lato.name
            text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_TITLE") + " " + mainWindow.title
            font.bold: true
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_TITLE
            Layout.fillWidth: true
            anchors.top: rowTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        }
        Text {
            id: text1
            anchors.top: text0.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text:  qsTranslate("PageHelpDocOnline","STR_HELP_DOC_SUB_TITLE")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
        Text {
            id: text2
            anchors.top: text1.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_TOPIC") + ":"
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
                text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_TOPIC_1")
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
                text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_TOPIC_2")
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
                text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_TOPIC_3")
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
                text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_TOPIC_4")
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
                text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_ONLINE") + ":"
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
            }
        }

        RowLayout {
            id: rawLinks
            anchors.top: raw5.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width


            Components.TextBullet{}
            Text {
                id: textLinkCC
                textFormat: Text.RichText
                text: "<a href=\"https://www.autenticacao.gov.pt/documents/10179/11465/\
Manual+de+Utiliza%C3%A7%C3%A3o+da+Aplica%C3%A7%C3%A3o+do+Cart%C3%A3o+de+Cidad%C3%A3o\">
Manual de utilização da aplicação do cartão de cidadão</a>"
                font.italic: true
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
            }
        }
    }
}
