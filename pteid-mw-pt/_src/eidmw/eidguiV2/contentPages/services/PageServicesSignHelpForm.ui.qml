import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    property alias propertyTextTitle: textTitle
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
            text: qsTranslate("PageServicesSign", "STR_SIGN_HELP_TITLE")
            font.bold: activeFocus ? true : false
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_TITLE
            Layout.fillWidth: true
            Accessible.role: Accessible.TitleBar
            Accessible.name: text
            KeyNavigation.tab: textSubTitle.propertyText
            KeyNavigation.down: textSubTitle.propertyText
            KeyNavigation.right: textSubTitle.propertyText
            KeyNavigation.left: autenticacaoGovLink.propertyText
            KeyNavigation.backtab: autenticacaoGovLink.propertyText
            KeyNavigation.up: autenticacaoGovLink.propertyText
        }
        Components.Link {
            id: textSubTitle
            anchors.top: textTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            propertyText.text: qsTranslate("PageServicesSign",
                                           "STR_SIGN_HELP_SUB_TITLE") + " "
                               + "<a href='https://www.autenticacao.gov.pt/cmd-pedido-chave'>"
                               + qsTranslate("PageServicesSign",
                                             "STR_SIGN_HELP_CMD_LINK")
            propertyText.width: parent.width
            propertyText.wrapMode: Text.Wrap
            Layout.fillWidth: true
            propertyAccessibleText: qsTranslate(
                                        "PageServicesSign",
                                        "STR_SIGN_HELP_SUB_TITLE") + qsTranslate(
                                        "PageServicesSign",
                                        "STR_SIGN_HELP_CMD_LINK")
            propertyAccessibleDescription: qsTranslate(
                                        "PageServicesSign",
                                        "STR_SIGN_HELP_CMD_SELECT")
            propertyLinkUrl: 'https://www.autenticacao.gov.pt/cmd-pedido-chave'
            KeyNavigation.tab: textTopic1
            KeyNavigation.down: textTopic1
            KeyNavigation.right: textTopic1
            KeyNavigation.left: textTitle
            KeyNavigation.backtab: textTitle
            KeyNavigation.up: textTitle
        }
        Text {
            id: textTopic1
            anchors.top: textSubTitle.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: focus ? true : false
            text: qsTranslate("PageServicesSign", "STR_SIGN_HELP_TOPIC_1")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: autenticacaoGovLink.propertyText
            KeyNavigation.down: autenticacaoGovLink.propertyText
            KeyNavigation.right: autenticacaoGovLink.propertyText
            KeyNavigation.left: textSubTitle.propertyText
            KeyNavigation.backtab: textSubTitle.propertyText
            KeyNavigation.up: textSubTitle.propertyText
        }

        Components.Link {
            id: autenticacaoGovLink
            anchors.top: textTopic1.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            propertyText.text: qsTranslate("PageServicesSign",
                                           "STR_SIGN_HELP_TOPIC_2")
                               + "<a href='https://www.autenticacao.gov.pt'>" + " " + qsTranslate(
                                   "PageServicesSign",
                                   "STR_SIGN_HELP_AUTENTICACAO.GOV_LINK")
            propertyText.width: parent.width
            propertyText.wrapMode: Text.Wrap
            Layout.fillWidth: true
            propertyAccessibleText: qsTranslate(
                                        "PageServicesSign",
                                        "STR_SIGN_HELP_TOPIC_2") + qsTranslate(
                                        "PageServicesSign",
                                        "STR_SIGN_HELP_AUTENTICACAO.GOV_LINK")
            propertyAccessibleDescription: qsTranslate(
                                        "PageServicesSign",
                                        "STR_SIGN_HELP_AUTENTICACAO.GOV_SELECT")
            propertyLinkUrl: 'https://www.autenticacao.gov.pt'
            KeyNavigation.tab: textTitle
            KeyNavigation.down: textTitle
            KeyNavigation.right: textTitle
            KeyNavigation.left: textTopic1
            KeyNavigation.backtab: textTopic1
            KeyNavigation.up: textTopic1
        }
    }
}
