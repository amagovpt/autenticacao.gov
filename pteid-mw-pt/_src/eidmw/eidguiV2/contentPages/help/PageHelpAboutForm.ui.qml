import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias propertyTextLinkCC: textLinkCC
    property alias propertyTextVersion: textVersion
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
            font.bold: activeFocus ? true : false
            Accessible.role: Accessible.TitleBar
            Accessible.name: text
            KeyNavigation.tab: textDescription
        }
        Text {
            id: textDescription
            text: qsTranslate("PageHelpAbout","STR_HELP_TITLE")
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: textDescription.focus ? true : false
            anchors.top: textAppName.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textVersion
        }
        Text {
            id: textVersion
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: textVersion.focus ? true : false
            anchors.top: textDescription.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textDifficult
        }
        Text {
            id: textDifficult
            text: qsTranslate("PageHelpAbout","STR_HELP_LINK") + ": "
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: textDifficult.focus ? true : false
            anchors.top: textVersion.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textLinkCC.propertyText
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
    }

    Text {
        id: textDevBy
        text: qsTranslate("PageHelpAbout","STR_HELP_DEV_BY")
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
            font.bold: textDevBy.focus ? true : false
        anchors.top: textLinkCC.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: textDevByAgency
    }

    Text {
        id: textDevByAgency
        text: qsTranslate("PageHelpAbout","STR_HELP_DEV_AGENCY")
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
            font.bold: textDevByAgency.focus ? true : false
        anchors.top: textDevBy.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        width: parent.width
        wrapMode: Text.Wrap
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: propertyMainItem
    }

    Image {
        id: imageCC
        y: parent.height * 0.1
        width: Constants.SIZE_IMAGE_LOGO_CC_WIDTH
        height: Constants.SIZE_IMAGE_LOGO_CC_HEIGHT
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        source: "../../images/logo_CC.png"
        anchors.top: textDevByAgency.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
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
