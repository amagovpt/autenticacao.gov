/*-****************************************************************************

 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
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
    property alias propertyTextTitle: textTitle

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_HELP_ACCESSIBILITY_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_HELP_ACCESSIBILITY_ROW_TOP_INC_RELATIVE
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
            text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_TITLE")
            font.bold: activeFocus
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_TITLE
            Layout.fillWidth: true
            Accessible.role: Accessible.TitleBar
            Accessible.name: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_TITLE")
            KeyNavigation.tab: text1
            KeyNavigation.down: text1
            KeyNavigation.right: text1
            KeyNavigation.backtab: rectPageKeys
            KeyNavigation.up: rectPageKeys
        }
        Text {
            id: text1
            anchors.top: textTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            font.bold: activeFocus
            text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_SUB_TITLE")
            wrapMode: Text.Wrap
            width: parent.width
            color: Constants.COLOR_TEXT_BODY
            Layout.fillWidth: true
            Accessible.role: Accessible.Row
            Accessible.name: text1.text
            KeyNavigation.tab: rectMenuKeys
            KeyNavigation.down: rectMenuKeys
            KeyNavigation.right: rectMenuKeys
            KeyNavigation.backtab: textTitle
            KeyNavigation.up: textTitle
        }
        Item{
            id: rectMenuKeys
            anchors.top: text1.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            width: parent.width
            height: text2.height + row1.propertyRowText.height + row2.propertyRowText.height
                            + row3.propertyRowText.height + Constants.SIZE_TEXT_BODY
            Accessible.role: Accessible.Row
            Accessible.name: text2.text + row1.propertyRowText.text + row2.propertyRowText.text
                                    + row3.propertyRowText.text
            KeyNavigation.tab: rectPageKeys
            KeyNavigation.down: rectPageKeys
            KeyNavigation.right: rectPageKeys
            KeyNavigation.backtab: text1
            KeyNavigation.up: text1

            Text {
                id: text2
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                font.bold: rectMenuKeys.activeFocus
                text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_MENUS")
                wrapMode: Text.Wrap
                width: parent.width
                horizontalAlignment: Text.left
                color: Constants.COLOR_TEXT_BODY
                Layout.fillWidth: true
            }
            Components.TextRow {
                id: row1
                anchors.top: text2.bottom
                propertyRowText.text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_MENUS_1")
            }
            Components.TextRow {
                id: row2
                anchors.top: row1.bottom
                propertyRowText.text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_MENUS_2")
            }
            Components.TextRow {
                id: row3
                anchors.top: row2.bottom
                propertyRowText.text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_MENUS_3")
            }
        }
        Item{
            id: rectPageKeys
            anchors.top: rectMenuKeys.bottom
            anchors.topMargin: 2 * Constants.SIZE_TEXT_BODY
            width: parent.width
            height: text3.height + row4.propertyRowText.height + row5.propertyRowText.height
                            + row6.propertyRowText.height + text4.height
            Accessible.role: Accessible.Row
            Accessible.name: text3.text + row4.propertyRowText.text + row5.propertyRowText.text
                                    + row6.propertyRowText.text + text4.text
            KeyNavigation.tab: textTitle
            KeyNavigation.down: textTitle
            KeyNavigation.right: textTitle
            KeyNavigation.backtab: rectMenuKeys
            KeyNavigation.up: rectMenuKeys

            Text {
                id: text3
                font.pixelSize: Constants.SIZE_TEXT_BODY
                font.family: lato.name
                font.bold: rectPageKeys.activeFocus
                text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_PAGES")
                wrapMode: Text.Wrap
                width: parent.width
                horizontalAlignment: Text.left
                color: Constants.COLOR_TEXT_BODY
                Layout.fillWidth: true
            }
            Components.TextRow {
                id: row4
                anchors.top: text3.bottom
                propertyRowText.text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_PAGES_2")
            }
            Components.TextRow {
                id: row5
                anchors.top: row4.bottom
                propertyRowText.text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_PAGES_3")
            }
            Components.TextRow {
                id: row6
                anchors.top: row5.bottom
                propertyRowText.text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_PAGES_1")
            }
            Components.TextRow {
                id: row7
                anchors.top: row6.bottom
                propertyRowText.text: qsTranslate("PageHelpAccessibility", "STR_HELP_ACCESSIBILITY_PAGES_4")
            }
        }
    }
}
