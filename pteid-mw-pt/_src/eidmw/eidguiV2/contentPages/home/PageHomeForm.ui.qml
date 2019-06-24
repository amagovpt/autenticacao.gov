import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    property alias propertyReminderCheckBox: reminderCheckBox
    property alias propertyButtonDownload: buttonDownload
    property alias propertyMainItem: text0

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
            text: qsTranslate("PageHelpDocOnline",
                              "STR_HELP_DOC_TITLE") + " " + mainWindow.title
            font.bold: text0.focus ? true : false
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_TITLE
            Layout.fillWidth: true
            Accessible.role: Accessible.TitleBar
            Accessible.name: qsTranslate("PageHelpDocOnline",
                                         "STR_HELP_DOC_TITLE") + " " + mainWindow.title
                             + qsTranslate("PageHelpDocOnline", "STR_HELP_DOC_HOME_PAGE")
            KeyNavigation.tab: text1
        }
        Text {
            id: text1
            anchors.top: text0.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: text1.focus ? true : false
            text: qsTranslate("PageHelpDocOnline", "STR_HELP_DOC_SUB_TITLE")
            wrapMode: Text.Wrap
            width: parent.width
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: rectTopics
        }
        Item{
            id: rectTopics
            anchors.top: text1.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            height: text2.height + raw1.height + raw2.height + raw3.height + raw4.height
                    + 4 * Constants.SIZE_TEXT_BODY
            width: parent.width
            Accessible.role: Accessible.Row
            Accessible.name: text2.text + text3.text + text4.text + text5.text + text6.text
            KeyNavigation.tab: textLinks
            Text {
                id: text2
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                font.bold: rectTopics.focus ? true : false
                text: qsTranslate("PageHelpDocOnline", "STR_HELP_DOC_TOPIC") + ":"
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

                Components.TextBullet {
                }
                Text {
                    id: text3
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    font.family: lato.name
                    font.bold: rectTopics.focus ? true : false
                    text: qsTranslate("PageHelpDocOnline", "STR_HELP_DOC_TOPIC_1")
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

                Components.TextBullet {
                }
                Text {
                    id: text4
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    font.family: lato.name
                    font.bold: rectTopics.focus ? true : false
                    text: qsTranslate("PageHelpDocOnline", "STR_HELP_DOC_TOPIC_2")
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

                Components.TextBullet {
                }
                Text {
                    id: text5
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    font.family: lato.name
                    font.bold: rectTopics.focus ? true : false
                    text: qsTranslate("PageHelpDocOnline", "STR_HELP_DOC_TOPIC_3")
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

                Components.TextBullet {
                }
                Text {
                    id: text6
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    font.family: lato.name
                    font.bold: rectTopics.focus ? true : false
                    text: qsTranslate("PageHelpDocOnline", "STR_HELP_DOC_TOPIC_4")
                    width: parent.width
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.left
                    color: Constants.COLOR_TEXT_BODY
                    Layout.fillWidth: true
                }
            }
        }


        RowLayout {
            id: raw5
            anchors.top: rectTopics.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width

            Text {
                id: textLinks
                text: qsTranslate("PageHelpDocOnline",
                                  "STR_HELP_DOC_ONLINE") + ":"
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                font.bold: textLinks.focus ? true : false
                color: Constants.COLOR_TEXT_BODY
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: buttonDownload
            }
            Button {
                id: buttonDownload
                text: qsTranslate("PageHelpDocOnline", "STR_HELP_URL")
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: reminderCheckBox
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus ? true : false
            }
        }

        RowLayout {
            id: rawCheckBox
            anchors.top: raw5.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width
            CheckBox {
                id: reminderCheckBox
                text: qsTranslate("PageHelpDocOnline", "STR_HOME_REMINDER")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                enabled: true
                Accessible.role: Accessible.CheckBox
                Accessible.name: text
            }
        }
    }
}
