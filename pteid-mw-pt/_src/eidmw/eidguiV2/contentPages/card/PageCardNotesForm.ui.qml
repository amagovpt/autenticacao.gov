/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions
import "../../components" as Components

Item {
    anchors.fill: parent

    property alias propertyRowText: rowText
    property alias propertyWarningTextItem: warningTextItem
    property alias propertyRowNotesCount: rowNotesCount
    property alias propertyFlickNotes: flickable
    property alias propertyEditNotes: edit
    property alias propertyBusyIndicator: busyIndicator
    property alias propertySaveNotes: saveNotes
    property alias propertyProgressBar: progressBar
    property alias propertyNotesText: notesText
    property string progressBarText: ""

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_CARD_NOTES_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_CARD_NOTES_ROW_TOP_INC_RELATIVE
        Components.CardRowTop{}
        }

    BusyIndicator {
        id: busyIndicator
        running: false
        anchors.centerIn: parent
        // BusyIndicator should be on top of all other content
        z: 1
    }

    Item {
        id: warningTextItem
        visible: false
        width: parent.width
        height: parent.height * Constants.PAGE_NOTES_TEXT_V_RELATIVE
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Rectangle {
            id: warningRectText
            width: parent.width
            height: parent.height
            opacity: 1

            Text {
                id: warningNotesText
                text: qsTranslate("PageCardNotes", "STR_NOTES_PAGE_NOT_SUPPORTED")
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: warningRectText.activeFocus ? true : false
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                Accessible.role: Accessible.Column
                Accessible.name: text
                KeyNavigation.tab: warningNotesTextDescription
                KeyNavigation.down: warningNotesTextDescription
                KeyNavigation.right: warningNotesTextDescription
                KeyNavigation.backtab: warningNotesTextDescription
                KeyNavigation.up: warningNotesTextDescription
            }
            Text {
                id: warningNotesTextDescription
                text: qsTranslate("PageCardNotes", "STR_NOTES_NOT_SUPPORTED_DESCRIPTION_PAGE")
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: notesTextDescription.activeFocus ? true : false
                color: Constants.COLOR_MAIN_BLACK
                anchors.top: warningNotesText.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                wrapMode: Text.Wrap
                Accessible.multiLine: true
                Accessible.role: Accessible.StaticText
                Accessible.name: text
                KeyNavigation.tab: warningNotesText
                KeyNavigation.down: warningNotesText
                KeyNavigation.right: warningNotesText
                KeyNavigation.backtab: warningNotesText
                KeyNavigation.up: warningNotesText
            }
        }
    }

    Item {
        id: rowText
        width: parent.width
        height: parent.height * Constants.PAGE_NOTES_TEXT_V_RELATIVE
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Rectangle {
            id: rectText
            width: parent.width
            height: parent.height
            opacity: 1

            Text {
                id: notesText
                text: qsTranslate("PageCardNotes", "STR_NOTES_PAGE")
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: notesText.activeFocus ? true : false
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                Accessible.role: Accessible.Column
                Accessible.name: text
                KeyNavigation.tab: notesTextDescription
                KeyNavigation.down: notesTextDescription
                KeyNavigation.right: notesTextDescription
                KeyNavigation.backtab: saveNotes
                KeyNavigation.up: saveNotes
            }
            Text {
                id: notesTextDescription
                text: qsTranslate("PageCardNotes", "STR_NOTES_DESCRIPTION_PAGE")
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: notesTextDescription.activeFocus ? true : false
                color: Constants.COLOR_MAIN_BLACK
                anchors.top: notesText.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                wrapMode: Text.Wrap
                Accessible.multiLine: true
                Accessible.role: Accessible.StaticText
                Accessible.name: text
                KeyNavigation.tab: edit
                KeyNavigation.down: edit
                KeyNavigation.right: edit
                KeyNavigation.backtab: notesText
                KeyNavigation.up: notesText
            }
            DropShadow {
                anchors.fill: rectFieldFlick
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectFieldFlick
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectFieldFlick
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectFieldFlick
                width: parent.width
                height: parent.height - notesText.height - notesTextDescription.height
                        - 2 * Constants.SIZE_ROW_V_SPACE
                color: "white"
                anchors.top: notesTextDescription.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                opacity: 1

                Keys.enabled: true

                Flickable {
                    id: flickable
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    y: Constants.SIZE_TEXT_FIELD_V_SPACE
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: parent.height - 2 * Constants.SIZE_TEXT_FIELD_V_SPACE
                    boundsBehavior: Flickable.StopAtBounds
                    maximumFlickVelocity: 2500
                    flickableDirection: Flickable.VerticalFlick
                    contentWidth: edit.paintedWidth
                    contentHeight: edit.paintedHeight
                    clip: true

                    TextEdit {
                        id: edit
                        property string previousText: text
                        text: ""
                        width: flickable.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        height: Math.max(paintedHeight, flickable.height);
                        focus: true
                        wrapMode: TextEdit.Wrap
                        selectByMouse: true
                        selectionColor: Constants.COLOR_MAIN_BLUE
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        color: Constants.COLOR_TEXT_BODY
                        Accessible.editable: true
                        Accessible.focusable: false
                        Accessible.focused: false

                        Accessible.role: Accessible.EditableText
                        Accessible.name: text
                        KeyNavigation.tab: progressBar
                        KeyNavigation.down: progressBar
                        KeyNavigation.right: progressBar
                        KeyNavigation.backtab: notesTextDescription
                        KeyNavigation.up: notesTextDescription
                    }

                    ScrollBar.vertical: ScrollBar {
                        active: true
                        visible: true
                        width: Constants.SIZE_TEXT_FIELD_H_SPACE

                        onActiveChanged: {
                            if (!active)
                                active = true
                        }
                    }
                }
            }
        }
    }

    Item {
        id: rowNotesCount
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL + 2 * Constants.SIZE_TEXT_V_SPACE + 2
                * Constants.SIZE_TEXT_FIELD
        anchors.top: rowText.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item {
            id: rectNotesCount
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE) * 0.50
            height: parent.height
            Text {
                id: dateText
                text: qsTranslate("PageCardNotes", "STR_NOTES_PAGE_SIZE")
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: progressBar.activeFocus ? true : false
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
            }
            Rectangle {
                id: rectField
                width: parent.width
                color: "white"
                height: 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: dateText.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                ProgressBar {
                    id: progressBar
                    width: rectField.width
                    height: rectField.height
                    Accessible.role: Accessible.ProgressBar
                    Accessible.name: qsTranslate("PageCardNotes",
                                                 "STR_NOTES_PAGE_SIZE") + progressBarText
                    KeyNavigation.tab: saveNotes
                    KeyNavigation.down: saveNotes
                    KeyNavigation.right: saveNotes
                    KeyNavigation.backtab: edit
                    KeyNavigation.up: edit
                }
            }
        }
        Item {
            id: rectNotesComfirm
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE) * 0.50
            height: parent.height
            anchors.left: rectNotesCount.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            Button {
                id: saveNotes
                text: qsTranslate("PageCardNotes", "STR_NOTES_PAGE_SAVE")
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus ? true : false
                Accessible.role: Accessible.Button
                Accessible.name: text
                KeyNavigation.tab: notesText
                KeyNavigation.down: notesText
                KeyNavigation.right: notesText
                KeyNavigation.backtab: progressBar
                KeyNavigation.up: progressBar
                Keys.onEnterPressed: {
                    startWritingNotes();
                }
                Keys.onReturnPressed: {
                    startWritingNotes();
                }
                onClicked:{
                    startWritingNotes();
                }
            }
        }
    }
}
