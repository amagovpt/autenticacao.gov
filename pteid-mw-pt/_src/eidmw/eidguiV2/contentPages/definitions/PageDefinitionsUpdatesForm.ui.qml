import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

import QtGraphicalEffects 1.0

import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    property alias propertyButtonSearch: buttonSearch
    property alias propertyProgressBar: progressBar
    property alias propertyTextDescription: textDescription.propertyText
    property alias propertyTextDescriptionText: textDescription
    property alias propertyButtonStartUpdate: buttonStartUpdate
    property alias propertyButtonCancelUpdate: buttonCancelUpdate
    property alias propertyReleaseScrollViewText: releaseNoteScrollViewText
    property alias propertyReleaseNoteScrollView: releaseNoteScrollView
    property alias propertyRemoteVersion: remoteVersion
    property alias propertyInstalledVersion: installedVersion

    property alias propertyMainItem: updateTitle

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_DEFINITIONS_UPDATES_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_DEFINITIONS_UPDATES_ROW_TOP_INC_RELATIVE
    }

    Item {
        id: updateMain
        width: parent.width
        height: parent.height - rowTop.height - Constants.SIZE_ROW_V_SPACE
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item {
            id: rawUpdateTitle
            width: parent.width
            height: Constants.SIZE_TEXT_LABEL
            anchors.top: updateMain.top
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                id: updateTitle
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: activeFocus
                color: Constants.COLOR_TEXT_LABEL
                height: parent.height
                text: qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_TITLE")
                Accessible.role: Accessible.TitleBar
                Accessible.name: text
                KeyNavigation.tab: textDescription
                KeyNavigation.down: textDescription
                KeyNavigation.right: textDescription
                KeyNavigation.left: buttonSearch.visible ? buttonSearch : releaseNoteScrollViewText
                KeyNavigation.backtab: buttonSearch.visible ? buttonSearch : releaseNoteScrollViewText
                KeyNavigation.up: buttonSearch.visible ? buttonSearch : releaseNoteScrollViewText
            }
        }

        Item {
            id: mainItemWithPadding
            width: parent.width
            height: parent.height - rawUpdateTitle.height - 2 * Constants.SIZE_ROW_V_SPACE
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: rawUpdateTitle.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rawTextDescription
                width: parent.width
                height: parent.height * Constants.HEIGHT_DEFINITIONS_UPDATE_LABEL_ROW
                anchors.top: mainItemWithPadding.top
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                Components.Link {
                    id: textDescription
                    propertyText.text: qsTranslate("PageDefinitionsUpdates",
                                                   "STR_UPDATE_TEXT")
                    propertyText.font.bold: activeFocus || textDescription.activeFocus
                    propertyText.font.pixelSize: activeFocus || textDescription.activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    width: parent.width
                    height: parent.height
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: propertyText.text
                    KeyNavigation.tab: buttonSearch.visible ? buttonSearch : installedVersion
                    KeyNavigation.down: buttonSearch.visible ? buttonSearch : installedVersion
                    KeyNavigation.right: buttonSearch.visible ? buttonSearch : installedVersion
                    KeyNavigation.left: updateTitle
                    KeyNavigation.backtab: updateTitle
                    KeyNavigation.up: updateTitle
                }
            }
            Item {
                id: rawRowVersion
                width: parent.width
                height: parent.height * 3 * Constants.HEIGHT_DEFINITIONS_UPDATE_LABEL_ROW
                anchors.top: rawTextDescription.bottom
                anchors.topMargin: 6 * Constants.SIZE_TEXT_V_SPACE
                Item {
                    id: rawInstalledVersion
                    width: parent.width * 0.33 - Constants.SIZE_ROW_H_SPACE
                    height: parent.height
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Components.LabelTextBoxForm {
                        id: installedVersion
                        propertyDateText.text: qsTranslate(
                                                   "PageDefinitionsUpdates",
                                                   "STR_INSTALLED_VERSION")
                        propertyDateField.text: ""
                        visible: false
                        propertyDateField.horizontalAlignment: Text.AlignHCenter
                    }
                    KeyNavigation.tab: remoteVersion
                    KeyNavigation.down: remoteVersion
                    KeyNavigation.right: remoteVersion
                    KeyNavigation.left: textDescription
                    KeyNavigation.backtab: textDescription
                    KeyNavigation.up: textDescription
                }
                Item {
                    id: rawRemoteVersion
                    width: parent.width * 0.33 - Constants.SIZE_ROW_H_SPACE
                    height: parent.height
                    anchors.left: rawInstalledVersion.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Components.LabelTextBoxForm {
                        id: remoteVersion
                        propertyDateText.text: qsTranslate(
                                                   "PageDefinitionsUpdates",
                                                   "STR_REMOTE_VERSION")
                        propertyDateField.text: ""
                        visible: false
                        propertyDateField.horizontalAlignment: Text.AlignHCenter
                    }
                    KeyNavigation.tab: releaseNoteScrollViewText
                    KeyNavigation.down: releaseNoteScrollViewText
                    KeyNavigation.right: releaseNoteScrollViewText
                    KeyNavigation.left: rawInstalledVersion
                    KeyNavigation.backtab: rawInstalledVersion
                    KeyNavigation.up: rawInstalledVersion
                }
                Item {
                    id: rawButtonStartUpdate
                    width: parent.width * 0.33
                    height: parent.height
                    anchors.left: rawRemoteVersion.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Button {
                        id: buttonStartUpdate
                        y: 10
                        text: qsTranslate("PageDefinitionsUpdates",
                                          "STR_UPDATE_BUTTON_START")
                        width: parent.width
                        height: Constants.HEIGHT_BOTTOM_COMPONENT
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        visible: false
                        KeyNavigation.tab: releaseNoteScrollViewText
                        KeyNavigation.down: releaseNoteScrollViewText
                        KeyNavigation.right: releaseNoteScrollViewText
                        KeyNavigation.left: remoteVersion
                        KeyNavigation.backtab: remoteVersion
                        KeyNavigation.up: remoteVersion
                    }
                }
            }

            Item {
                id: textProgressBar
                width: parent.width
                height: Constants.HEIGHT_DEFININTIONS_UPDATE_PROGRESS_BAR
                anchors.top: rawTextDescription.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter

                ProgressBar {
                    id: progressBar
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 20
                    to: 100
                    value: 0
                    visible: false
                    indeterminate: false
                    z: 1
                }
            }
            Item {
                id: rawButtonSearch
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.top: textProgressBar.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter
                Accessible.role: Accessible.Row
                Accessible.name: buttonSearch.text
                KeyNavigation.tab: buttonSearch
                KeyNavigation.down: buttonSearch
                KeyNavigation.right: buttonSearch
                KeyNavigation.left: buttonStartUpdate
                KeyNavigation.backtab: buttonStartUpdate
                KeyNavigation.up: buttonStartUpdate
                Button {
                    id: buttonSearch
                    text: qsTranslate("PageDefinitionsUpdates",
                                      "STR_UPDATE_BUTTON")
                    anchors.horizontalCenterOffset: 0
                    width: 1.4 * Constants.WIDTH_BUTTON
                    height: parent.height
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false
                    anchors.horizontalCenter: parent.horizontalCenter
                    KeyNavigation.tab: propertyMainItem
                    KeyNavigation.down: propertyMainItem
                    KeyNavigation.right: propertyMainItem
                    KeyNavigation.left: textDescription
                    KeyNavigation.backtab: textDescription
                    KeyNavigation.up: textDescription
                }
            }
            Item {
                id: rawButtonCancelUpdate
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.top: textProgressBar.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                Button {
                    id: buttonCancelUpdate
                    text: qsTranslate("PageDefinitionsUpdates",
                                      "STR_UPDATE_BUTTON_CANCEL")
                    anchors.horizontalCenterOffset: 0
                    width: 1.4 * Constants.WIDTH_BUTTON
                    height: parent.height
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                    KeyNavigation.tab: propertyMainItem
                    KeyNavigation.down: propertyMainItem
                    KeyNavigation.right: propertyMainItem
                    KeyNavigation.left: buttonSearch
                    KeyNavigation.backtab: buttonSearch
                    KeyNavigation.up: buttonSearch
                }
            }

            Item {
                id: rawReleaseNoteScrollView
                width: parent.width
                height: (parent.height * Constants.HEIGHT_DEFINITIONS_UPDATE_RELEASE_NOTE)
                        - 12 * Constants.SIZE_TEXT_V_SPACE
                anchors.top: rawRowVersion.bottom
                anchors.topMargin: 2 * Constants.SIZE_TEXT_V_SPACE
                Rectangle {
                    id: releaseNoteScrollView
                    width: parent.width
                    height: parent.height
                    opacity: 1
                    visible: false
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
                        height: parent.height
                        anchors.top: parent.top
                        opacity: 1

                        Flickable {
                            id: flickable
                            x: Constants.SIZE_TEXT_FIELD_H_SPACE
                            y: Constants.SIZE_TEXT_FIELD_V_SPACE
                            width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                            height: parent.height - 2 * Constants.SIZE_TEXT_FIELD_V_SPACE
                            boundsBehavior: Flickable.StopAtBounds
                            maximumFlickVelocity: 2500
                            flickableDirection: Flickable.VerticalFlick
                            contentWidth: releaseNoteScrollViewText.paintedWidth
                            contentHeight: releaseNoteScrollViewText.paintedHeight
                            clip: true

                            Label {
                                id: releaseNoteScrollViewText
                                text: ""
                                width: flickable.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                                height: flickable.height - 2 * Constants.SIZE_TEXT_FIELD_V_SPACE
                                wrapMode: TextEdit.Wrap
                                font.pixelSize: Constants.SIZE_TEXT_FIELD
                                font.bold: activeFocus
                                color: Constants.COLOR_TEXT_BODY
                                visible: false
                                textFormat: "RichText"
                                Accessible.role: Accessible.StaticText
                                KeyNavigation.tab: updateTitle
                                KeyNavigation.down: updateTitle
                                KeyNavigation.right: updateTitle
                                KeyNavigation.left: buttonStartUpdate
                                KeyNavigation.backtab: buttonStartUpdate
                                KeyNavigation.up: buttonStartUpdate
                            }

                            ScrollBar.vertical: ScrollBar {
                                active: true
                                visible: true
                                width: Constants.SIZE_TEXT_FIELD_H_SPACE
                            }
                            MouseArea {
                                id: mouseAreaFlickable
                                anchors.fill: parent
                            }
                        }
                    }
                }
            }
        }
    }
}
