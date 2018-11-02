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
    property alias propertyRemoteVersion: remoteVersion
    property alias propertyInstalledVersion: installedVersion

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
            anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                id: updateTitle
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: parent.height
                text: qsTranslate("PageDefinitionsUpdates", "STR_UPDATE_TITLE")
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
                Text {
                    id: textDescription
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    font.family: lato.name
                    text: qsTranslate("PageDefinitionsUpdates",
                                      "STR_UPDATE_TEXT")
                    wrapMode: Text.Wrap
                    width: parent.width
                    height: parent.height
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
                        visible: false
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
                    anchors.horizontalCenter: parent.horizontalCenter
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
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                }
            }

            Item {
                id: rawReleaseNoteScrollView
                width: parent.width
                height: (parent.height * Constants.HEIGHT_DEFINITIONS_UPDATE_RELEASE_NOTE) - 12 * Constants.SIZE_TEXT_V_SPACE
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
                                color: Constants.COLOR_TEXT_BODY
                                visible: false
                                textFormat: "RichText"
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
