/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    property alias propertyMainItem: textTitle
    property alias propertyReminderCheckBox: reminderCheckBox
    property alias propertyButtonDownload: buttonDownload

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_DOC_ONLINE_ROW_TOP_V_RELATIVE
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
            id: textTitle
            font.pixelSize: Constants.SIZE_TEXT_TITLE
            font.family: lato.name
            text: qsTranslate("PageHelpDocOnline",
                              "STR_HELP_DOC_TITLE") + " " + mainWindow.title
            font.bold: activeFocus ? true : false
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
            KeyNavigation.down: text1
            KeyNavigation.right: text1
            KeyNavigation.backtab: reminderCheckBox
            KeyNavigation.up: reminderCheckBox
        }
        Text {
            id: text1
            anchors.top: textTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: text1.activeFocus ? true : false
            text: qsTranslate("PageHelpDocOnline", "STR_HELP_DOC_SUB_TITLE")
            wrapMode: Text.Wrap
            width: parent.width
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
            Accessible.role: Accessible.Row
            Accessible.name: text
            KeyNavigation.tab: rectTopics
            KeyNavigation.down: rectTopics
            KeyNavigation.right: rectTopics
            KeyNavigation.backtab: textTitle
            KeyNavigation.up: textTitle
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
            KeyNavigation.down: textLinks
            KeyNavigation.right: textLinks
            KeyNavigation.backtab: text1
            KeyNavigation.up: text1
            Text {
                id: text2
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                font.bold: rectTopics.activeFocus ? true : false
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
                    font.bold: rectTopics.activeFocus ? true : false
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
                    font.bold: rectTopics.activeFocus ? true : false
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
                    font.bold: rectTopics.activeFocus ? true : false
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
                    font.bold: rectTopics.activeFocus ? true : false
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
                font.bold: textLinks.activeFocus ? true : false
                color: Constants.COLOR_TEXT_BODY
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: buttonDownload
                KeyNavigation.down: buttonDownload
                KeyNavigation.right: buttonDownload
                KeyNavigation.backtab: rectTopics
                KeyNavigation.up: rectTopics
            }
            Button {
                id: buttonDownload
                text: qsTranslate("PageHelpDocOnline", "STR_HELP_URL")
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus ? true : false
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: reminderCheckBox
                KeyNavigation.down: reminderCheckBox
                KeyNavigation.right: reminderCheckBox
                KeyNavigation.backtab: textLinks
                KeyNavigation.up: textLinks
                Keys.onEnterPressed: clicked()
                Keys.onReturnPressed: clicked()
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
                KeyNavigation.tab: propertyMainItem
                KeyNavigation.down: propertyMainItem
                KeyNavigation.right: propertyMainItem
                KeyNavigation.backtab: buttonDownload
                KeyNavigation.up: buttonDownload
                Keys.onEnterPressed: reminderCheckBox.checked = !reminderCheckBox.checked
                Keys.onReturnPressed: reminderCheckBox.checked = !reminderCheckBox.checked
            }
        }
    }
}
