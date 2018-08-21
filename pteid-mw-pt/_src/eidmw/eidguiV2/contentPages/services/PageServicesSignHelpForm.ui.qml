import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    property alias propertyTextSubTitle: textSubTitle
    property alias propertyTextAutenticacaoGovLink: autenticacaoGovLink

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_SERVICES_SIGN_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_SERVICES_SIGN_ROW_TOP_INC_RELATIVE
    }
    Item {
        width: parent.width - 2 * Constants.SIZE_ROW_H_SPACE
        height: parent.height - 2 * Constants.SIZE_ROW_V_SPACE
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        Text {
            id: textTitle
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            font.family: lato.name
            text: qsTranslate("PageServicesSign","STR_SIGN_HELP_TITLE")
            font.bold: true
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_TITLE
            Layout.fillWidth: true
        }
        Text {
            id: textSubTitle
            anchors.top: textTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: qsTranslate("PageServicesSign","STR_SIGN_HELP_SUB_TITLE")
                  + "<a href=\"https://www.autenticacao.gov.pt/cmd-pedido-chave\">"
                  + " " + qsTranslate("PageServicesSign","STR_SIGN_HELP_CMD_LINK")
            wrapMode: Text.Wrap
            width: parent.width
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
        Text {
            id: textTopic1
            anchors.top: textSubTitle.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: qsTranslate("PageServicesSign","STR_SIGN_HELP_TOPIC_1")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }

        Text {
            id: autenticacaoGovLink
            anchors.top: textTopic1.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: qsTranslate("PageServicesSign","STR_SIGN_HELP_TOPIC_2")
                  + "<a href=\"https://www.autenticacao.gov.pt\">"
                  + " " + qsTranslate("PageServicesSign","STR_SIGN_HELP_AUTENTICACAO.GOV_LINK")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
        }
    }
}
