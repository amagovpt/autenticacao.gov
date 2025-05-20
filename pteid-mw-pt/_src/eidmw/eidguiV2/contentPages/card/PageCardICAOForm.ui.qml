/*-****************************************************************************

 * Copyright (C) 2017, 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import Qt.labs.platform 1.0
import QtGraphicalEffects 1.0
import eidguiV2 1.0

import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent

    property alias propertyBusyIndicator: busyIndication

    property alias propertyTextBoxSurName: textBoxSurName
    property alias propertyTextBoxSex: textBoxSex
    property alias propertyTextBoxNacionality: textBoxNacionality
    property alias propertyTextBoxDateOfBirth: textBoxDateOfBirth
    property alias propertyTextBoxDocumentNum: textBoxDocumentNum
    property alias propertyTextBoxExpirydate: textBoxExpirydate
    property alias propertyTextBoxCountry: textBoxCountry
    property alias propertyPhoto: photoImage
    property alias propertySavePhotoButton: savePhotoButton
    property alias propertySavePhotoDialogOutput: savePhotoDialogOutput
    property alias propertyExportRect: exportRect
    property alias propertyCancelExportMouseArea: cancelExportMouseArea
    property alias propertyPngButton: pngButton
    property alias propertyJpegButton: jpegButton
    property alias propertyCancelExport: cancelExport
    property bool propertyIsFullNameMrz: false

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_CARD_IDENTIFY_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_CARD_IDENTIFY_ROW_TOP_INC_RELATIVE
        Components.CardRowTop {
        }
    }

    BusyIndicator {
        id: busyIndication
        running: false
        anchors.centerIn: parent
        // BusyIndicator should be on top of all other content
        z: 1
    }

    Item {
        id: rowNamePhoto
        width: parent.width
        height: Constants.HEIGHT_PHOTO_IMAGE + Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE + 2 * Constants.FOCUS_BORDER
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item {
            id: rectNamePhotoLeft
            width: parent.width - Constants.WIDTH_PHOTO_IMAGE
                   - Constants.SIZE_ROW_H_SPACE - 2 * Constants.FOCUS_BORDER
            height: parent.height

            Item {
                id: surNameTextTextForm
                width: parent.width
                height: rectNamePhotoLeft.height - 2
                        * Constants.SIZE_ROW_V_SPACE - Constants.SIZE_TEXT_LABEL
                        - Constants.SIZE_TEXT_V_SPACE
                anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm {
                    id: textBoxSurName
                    propertyDateText.text: propertyIsFullNameMrz ? qsTranslate("GAPI", "STR_FULL_NAME_MRZ_EXTRACT") : qsTranslate("GAPI", "STR_FULL_NAME")
                    propertyRectField.height: parent.height
                    propertyDateField.wrapMode: Text.WordWrap
                }
                Accessible.role: Accessible.Column
                Accessible.name: textBoxSurName.accessibleText
                KeyNavigation.tab: photoImage
                KeyNavigation.down: photoImage
                KeyNavigation.right: photoImage
                KeyNavigation.backtab: rectCountry
                KeyNavigation.up: rectCountry
            }
        }

        Item {
            id: rectNamePhotoRight
            width: Constants.WIDTH_PHOTO_IMAGE + 2 * Constants.FOCUS_BORDER
            height: parent.height - Constants.SIZE_TEXT_LABEL - Constants.SIZE_TEXT_V_SPACE
            anchors.left: rectNamePhotoLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.bottom: parent.bottom
            DropShadow {
                anchors.fill: rectPhotoField
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectPhotoField
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectPhotoField
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectPhotoField
                width: parent.width
                height: parent.height
                border.width: photoImage.focus ? Constants.FOCUS_BORDER : 0
                border.color: photoImage.focus ? Constants.COLOR_MAIN_DARK_GRAY : Constants.COLOR_MAIN_SOFT_GRAY
                clip: true
                Image {
                    id: photoImage
                    width: parent.width - Constants.FOCUS_BORDER
                    height: parent.height - Constants.FOCUS_BORDER
                    x: Constants.FOCUS_BORDER
                    y: Constants.FOCUS_BORDER
                    fillMode: Image.PreserveAspectFit
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    antialiasing: true
                    cache: false
                    Accessible.role: Accessible.Graphic
                    Accessible.name: qsTranslate("GAPI", "STR_PHOTO")
                    KeyNavigation.tab: savePhotoButton.visible ? savePhotoButton : cancelExport
                    KeyNavigation.down: savePhotoButton.visible ? savePhotoButton : cancelExport
                    KeyNavigation.right: savePhotoButton.visible ? savePhotoButton : cancelExport
                    KeyNavigation.backtab: surNameTextTextForm
                    KeyNavigation.up: surNameTextTextForm

                    Button {
                        id: savePhotoButton
                        width: photoImage.width * 0.25
                        height: photoImage.height * 0.18
                        anchors.right: photoImage.right
                        anchors.rightMargin: photoImage.width * 0.05
                        anchors.bottomMargin: photoImage.height * 0.04
                        anchors.bottom: photoImage.bottom
                        enabled: false
                        highlighted: activeFocus ? true : false
                        text: qsTranslate("GAPI","STR_EXPORT_PHOTO")
                        background: Rectangle {
                            anchors.fill: parent
                            color: parent.hovered || parent.activeFocus ? Constants.COLOR_MAIN_BLUE : "white"
                            opacity: parent.enabled  ? (parent.hovered || parent.activeFocus ? 1 : 0.8) : 0.5
                            radius: parent.height * 0.1
                            border.color: parent.hovered
                                          ? Constants.COLOR_MAIN_MIDDLE_GRAY
                                          : Constants.COLOR_MAIN_BLUE
                            border.width: 1
                            Image {
                                source: savePhotoButton.hovered || savePhotoButton.activeFocus
                                        ? "../../images/download_icon_white.png"
                                        : "../../images/download_icon_blue.png"
                                anchors.margins: savePhotoButton.width * 0.15
                                anchors.fill: parent
                            }
                        }
                        contentItem: Text {
                            opacity: 0
                        }
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        KeyNavigation.tab: rectSex
                        KeyNavigation.down: rectSex
                        KeyNavigation.right: rectSex
                        KeyNavigation.backtab: photoImage
                        KeyNavigation.up: photoImage
                        Keys.onEnterPressed: clicked()
                        Keys.onReturnPressed: clicked()
                    }
                    FileDialog {
                        id: savePhotoDialogOutput
                        title: qsTranslate("Popup File",
                                           "STR_POPUP_FILE_OUTPUT")
                        fileMode: FileDialog.SaveFile
                        folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
                    }

                    Rectangle {
                        id: exportRect
                        height: parent.height
                        width: parent.width
                        color: "white"
                        opacity: 0.9
                        visible: true
                        y: parent.height + Constants.FOCUS_BORDER

                        Image {
                            id: cancelExport
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.topMargin: parent.height * 0.05
                            anchors.rightMargin: parent.width * 0.05
                            antialiasing: true
                            width: Constants.SIZE_IMAGE_ARROW_MAIN_MENU_W
                            height: Constants.SIZE_IMAGE_ARROW_MAIN_MENU_H
                            source: cancelExportMouseArea.containsMouse || activeFocus
                                    ? "../../images/arrow-down_hover.png"
                                    : "../../images/arrow-down_AMA.png"
                            MouseArea {
                                id: cancelExportMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                z: 1
                            }
                            Accessible.role: Accessible.Button
                            Accessible.name: qsTranslate("GAPI", "STR_EXPORT_PHOTO_CLOSE")
                            KeyNavigation.tab: pngButton
                            KeyNavigation.down: pngButton
                            KeyNavigation.right: pngButton
                            KeyNavigation.backtab: jpegButton
                            KeyNavigation.up: jpegButton
                        }

                        Button {
                            id: pngButton
                            text: qsTranslate("GAPI","STR_EXPORT_AS") + "\n PNG"
                            width: parent.width * 0.9
                            height: Constants.HEIGHT_SIGN_BOTTOM_COMPONENT
                            anchors.top: cancelExport.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.topMargin: parent.height * 0.05
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.family: lato.name
                            font.capitalization: Font.MixedCase
                            highlighted: activeFocus ? true : false
                            Accessible.role: Accessible.Button
                            Accessible.name: text
                            KeyNavigation.tab: jpegButton
                            KeyNavigation.down: jpegButton
                            KeyNavigation.right: jpegButton
                            KeyNavigation.backtab: cancelExport
                            KeyNavigation.up: cancelExport
                            Keys.onEnterPressed: clicked()
                            Keys.onReturnPressed: clicked()
                        }

                        Button {
                            id: jpegButton
                            text: qsTranslate("GAPI","STR_EXPORT_AS") + "\n JPEG"
                            width: parent.width * 0.9
                            height: Constants.HEIGHT_SIGN_BOTTOM_COMPONENT
                            anchors.top: pngButton.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.topMargin: parent.height * 0.05
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.family: lato.name
                            font.capitalization: Font.MixedCase
                            highlighted: activeFocus ? true : false
                            Accessible.role: Accessible.Button
                            Accessible.name: text
                            KeyNavigation.tab: cancelExport
                            KeyNavigation.down: cancelExport
                            KeyNavigation.right: cancelExport
                            KeyNavigation.backtab: photoImage
                            KeyNavigation.up: photoImage
                            Keys.onEnterPressed: clicked()
                            Keys.onReturnPressed: clicked()
                        }
                    }
                }
            }
        }
    }

    Item {
        id: rowSex
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowNamePhoto.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item {
            id: rectSex
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE) * 0.20
            height: parent.height
            Components.LabelTextBoxForm {
                id: textBoxSex
                propertyDateText.text: qsTranslate("GAPI", "STR_GENDER")
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxSex.accessibleText
            KeyNavigation.tab: rectNacionality
            KeyNavigation.down: rectNacionality
            KeyNavigation.right: rectNacionality
            KeyNavigation.backtab: savePhotoButton
            KeyNavigation.up: savePhotoButton
        }
        Item {
            id: rectNacionality
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.25
            height: parent.height
            anchors.left: rectSex.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxNacionality
                propertyDateText.text: qsTranslate("GAPI", "STR_NATIONALITY")
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxNacionality.accessibleText
            KeyNavigation.tab: rectDateOfBirth
            KeyNavigation.down: rectDateOfBirth
            KeyNavigation.right: rectDateOfBirth
            KeyNavigation.backtab: rectSex
            KeyNavigation.up: rectSex
        }
        Item {
            id: rectDateOfBirth
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.35
            height: parent.height
            anchors.left: rectNacionality.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxDateOfBirth
                propertyDateText.text: qsTranslate("GAPI", "STR_DATE_OF_BIRTH")
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxDateOfBirth.accessibleText
            KeyNavigation.tab: rectDocumentNum
            KeyNavigation.down: rectDocumentNum
            KeyNavigation.right: rectDocumentNum
            KeyNavigation.backtab: rectNacionality
            KeyNavigation.up: rectNacionality
        }
    }

    Item {
        id: rowDocumentMum
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowSex.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item {
            id: rectDocumentNum
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE) * 0.50
            height: parent.height
            Components.LabelTextBoxForm {
                id: textBoxDocumentNum
                propertyDateText.text: qsTranslate("GAPI",
                                                   "STR_DOCUMENT_NUMBER")
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxDocumentNum.accessibleText
            KeyNavigation.tab: rectExpiryDate
            KeyNavigation.down: rectExpiryDate
            KeyNavigation.right: rectExpiryDate
            KeyNavigation.backtab: rectDateOfBirth
            KeyNavigation.up: rectDateOfBirth
        }
        Item {
            id: rectExpiryDate
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE) * 0.50
            height: parent.height
            anchors.left: rectDocumentNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxExpirydate
                propertyDateText.text: qsTranslate("GAPI", "STR_VALIDITY_DATE")
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxExpirydate.accessibleText
            KeyNavigation.tab: rectCountry
            KeyNavigation.down: rectCountry
            KeyNavigation.right: rectCountry
            KeyNavigation.backtab: rectDocumentNum
            KeyNavigation.up: rectDocumentNum
        }
    }

    Item {
        id: rowCountry
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL + Constants.SIZE_TEXT_V_SPACE + 2
                * Constants.SIZE_TEXT_FIELD
        anchors.top: rowDocumentMum.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item {
            id: rectCountry
            width: parent.width
            height: parent.height
            Components.LabelTextBoxForm {
                id: textBoxCountry
                propertyDateText.text: qsTranslate("GAPI", "STR_COUNTRY_OF_EMISSION")
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxCountry.accessibleText
            KeyNavigation.tab: surNameTextTextForm
            KeyNavigation.down: surNameTextTextForm
            KeyNavigation.right: surNameTextTextForm
            KeyNavigation.backtab: rectExpiryDate
            KeyNavigation.up: rectExpiryDate
        }
    }
}
