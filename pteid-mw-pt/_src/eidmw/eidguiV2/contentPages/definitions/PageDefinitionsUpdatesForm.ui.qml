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
    property alias propertyTextDescription: textDescription
    property alias propertyButtonStartUpdate: buttonStartUpdate
    property alias propertyButtonCancelUpdate: buttonCancelUpdate
    property alias propertyReleaseScrollViewText: releaseNoteScrollViewText
    property alias propertyReleaseNoteScrollView: releaseNoteScrollView

    Item {
        id: item1
        width: parent.width
        height: parent.height

        Item {
            id: rowTop
            width: parent.width
            height: parent.height * Constants.HEIGHT_DEFINITIONS_UPDATES_ROW_TOP_V_RELATIVE
                    + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                    * Constants.HEIGHT_DEFINITIONS_UPDATES_ROW_TOP_INC_RELATIVE
        }

        Text {
            id: textTitle
            font.pixelSize: Constants.SIZE_TEXT_LABEL
            font.family: lato.name
            text: qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_TITLE")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            color: Constants.COLOR_TEXT_LABEL
            Layout.fillWidth: true
            anchors.top: rowTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        }
        Text {
            id: textDescription
            anchors.top: textTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            text: qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_TEXT")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.left
            Layout.fillWidth: true
        }
        Item {
            id: textProgressBar
            anchors.top: textDescription.bottom
            anchors.topMargin: Constants.SIZE_TEXT_BODY
            width: parent.width
            height: 50
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
            anchors.top: textDescription.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            Button {
                id: buttonSearch
                y: 53
                text: qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_BUTTON")
                anchors.horizontalCenterOffset: 0
                width: 1.4 * Constants.WIDTH_BUTTON
                height: parent.height
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Item {
            id: rawButtonStartUpdate
            anchors.top: rawReleaseNoteScrollView.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            Button {
                id: buttonStartUpdate
                text: qsTranslate("PageDefinitionsUpdates",
                                  "STR_UPDATE_BUTTON_START")
                width: Constants.WIDTH_BUTTON
                height: parent.height
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
                visible: false
            }
        }
        Item {
            id: rawButtonCancelUpdate
            anchors.top: textProgressBar.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            width: parent.width
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            Button {
                id: buttonCancelUpdate
                y: 21
                text: qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_BUTTON_CANCEL")
                anchors.horizontalCenterOffset: 0
                width: Constants.WIDTH_BUTTON
                height: parent.height
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
                visible: false
            }
        }

        Item {
            id: rawReleaseNoteScrollView
            width: parent.width
            height: parent.height * Constants.HEIGHT_SETTINGS_UPDATE_RELEASE_NOTE
            anchors.top: textDescription.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Rectangle{
                id: releaseNoteScrollView
                width: parent.width
                height: parent.height
                opacity: 1
                visible:false
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
                    height: parent.height - 2 * Constants.SIZE_ROW_V_SPACE
                    anchors.top: textDescription.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
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
                            color: Constants.COLOR_TEXT_BODY
                            visible: false
                            textFormat: "RichText"
                        }

                        ScrollBar.vertical: ScrollBar {
                            active : true
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
