import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants

Item {
    property alias propertyTextLinkCC: textLinkCC
    property alias propertyTextVersion: textVersion

    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_HELP_ABOUT_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_HELP_ABOUT_ROW_TOP_INC_RELATIVE
    }

    Text {
        id: textAppName
        text: qsTranslate("PageHelpDocOnline","STR_HELP_DOC_TITLE") + " " + mainWindow.title
        font.pixelSize: Constants.SIZE_TEXT_TITLE
        color: Constants.COLOR_TEXT_TITLE
        font.family: lato.name
        font.bold: true
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
    }

    Text {
        id: textDescription
        text: qsTranslate("PageHelpAbout","STR_HELP_TITLE")
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
        anchors.top: textAppName.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
    }

    Text {
        id: textVersion
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
        anchors.top: textDescription.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
    }

    Text {
        id: textDifficult
        text: qsTranslate("PageHelpAbout","STR_HELP_LINK") + ": "
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
        anchors.top: textVersion.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
    }

    Text {
        id: textLinkCC
        color: Constants.COLOR_MAIN_BLUE
        textFormat: Text.RichText
        text: "<a href=\"https://www.autenticacao.gov.pt/o-cartao-de-cidadao\">https://www.autenticacao.gov.pt/o-cartao-de-cidadao</a>"
        font.italic: true
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
        anchors.top: textDifficult.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

    }

    Text {
        id: textDevBy
        text: qsTranslate("PageHelpAbout","STR_HELP_DEV_BY")
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
        anchors.top: textLinkCC.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
    }

    Text {
        id: textDevByAgency
        text: qsTranslate("PageHelpAbout","STR_HELP_DEV_AGENCY")
        font.pixelSize: Constants.SIZE_TEXT_BODY
        font.family: lato.name
        anchors.top: textDevBy.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        width: parent.width
        wrapMode: Text.Wrap
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
