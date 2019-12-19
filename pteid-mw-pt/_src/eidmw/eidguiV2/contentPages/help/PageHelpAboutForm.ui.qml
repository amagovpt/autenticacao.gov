/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias propertyTextLinkCC: textLinkCC
    property alias propertyTextVersion: textVersion
    property alias propertyTextRevision: textRevision
    property alias propertyTextCopyright: textCopyright
    property alias propertyMainItem: textAppName

    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_HELP_ABOUT_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_HELP_ABOUT_ROW_TOP_INC_RELATIVE
    }
    Item {
        width: parent.width - 2 * Constants.SIZE_ROW_H_SPACE
        height: parent.height - 2 * Constants.SIZE_ROW_V_SPACE
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        Text {
            id: textAppName
            text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_TITLE") + " " + mainWindow.title
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            color: Constants.COLOR_TEXT_TITLE
            font.family: lato.name
            font.bold: activeFocus
            Accessible.role: Accessible.TitleBar
            Accessible.name: text
            KeyNavigation.tab: textDescription
            KeyNavigation.down: textDescription
            KeyNavigation.right: textDescription
            KeyNavigation.backtab: textCopyright
            KeyNavigation.up: textCopyright
        }
        Text {
            id: textDescription
            text: qsTranslate("PageHelpAbout","STR_HELP_TITLE")
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: activeFocus
            anchors.top: textAppName.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textVersion
            KeyNavigation.down: textVersion
            KeyNavigation.right: textVersion
            KeyNavigation.backtab: textAppName
            KeyNavigation.up: textAppName
        }
        Text {
            id: textVersion
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: activeFocus
            anchors.top: textDescription.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textRevision
            KeyNavigation.down: textRevision
            KeyNavigation.right: textRevision
            KeyNavigation.backtab: textDescription
            KeyNavigation.up: textDescription
        }
        Text {
            id: textRevision
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: activeFocus
            anchors.top: textVersion.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textDifficult
            KeyNavigation.down: textDifficult
            KeyNavigation.right: textDifficult
            KeyNavigation.backtab: textVersion
            KeyNavigation.up: textVersion
        }
        Text {
            id: textDifficult
            text: qsTranslate("PageHelpAbout","STR_HELP_LINK") + ": "
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: activeFocus
            anchors.top: textRevision.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textLinkCC.propertyText
            KeyNavigation.down: textLinkCC.propertyText
            KeyNavigation.right: textLinkCC.propertyText
            KeyNavigation.backtab: textRevision
            KeyNavigation.up: textRevision
        }
        Components.Link {
            id: textLinkCC
            width: parent.width
            anchors.top: textDifficult.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            propertyText.text: "<a href='https://www.autenticacao.gov.pt/o-cartao-de-cidadao'>" +
                               "https://www.autenticacao.gov.pt/o-cartao-de-cidadao" + "</a>"
            propertyAccessibleText: qsTranslate("PageHelpAbout","STR_HELP_CC_SELECT")
            propertyAccessibleDescription:  qsTranslate("PageHelpAbout","STR_HELP_CC_SELECT")
            propertyLinkUrl: 'https://www.autenticacao.gov.pt/o-cartao-de-cidadao'
            KeyNavigation.tab: textDevBy
            KeyNavigation.down: textDevBy
            KeyNavigation.right: textDevBy
            KeyNavigation.left: textDifficult
            KeyNavigation.backtab: textDifficult
            KeyNavigation.up: textDifficult
        }

        Text {
            id: textDevBy
            text: qsTranslate("PageHelpAbout","STR_HELP_DEV_BY")
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: activeFocus
            anchors.top: textLinkCC.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textDevByAgency
            KeyNavigation.down: textDevByAgency
            KeyNavigation.right: textDevByAgency
            KeyNavigation.left: textLinkCC.propertyText
            KeyNavigation.backtab: textLinkCC.propertyText
            KeyNavigation.up: textLinkCC.propertyText
        }

        Text {
            id: textDevByAgency
            text: qsTranslate("PageHelpAbout","STR_HELP_DEV_AGENCY")
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: activeFocus
            anchors.top: textDevBy.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            width: parent.width
            wrapMode: Text.Wrap
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textCopyright
            KeyNavigation.down: textCopyright
            KeyNavigation.right: textCopyright
            KeyNavigation.backtab: textDevBy
            KeyNavigation.up: textDevBy
        }
        Text {
            id: textCopyright
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: activeFocus
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: textDevByAgency.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            wrapMode: Text.Wrap
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: propertyMainItem
            KeyNavigation.down: propertyMainItem
            KeyNavigation.right: propertyMainItem
            KeyNavigation.backtab: textDevByAgency
            KeyNavigation.up: textDevByAgency
        }

        Image {
            id: imageCC
            y: parent.height * 0.1
            width: Constants.SIZE_IMAGE_LOGO_CC_WIDTH
            height: Constants.SIZE_IMAGE_LOGO_CC_HEIGHT
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            source: "../../images/logo_CC.png"
            anchors.top: textCopyright.bottom
            anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE
        }
        Image {
            id: imageSupported
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            source: "../../images/logo_compete2020.png"
            anchors.top: imageCC.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        }
    }
}
