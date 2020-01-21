/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias propertyDateAppCertificates: dateAppCertificates
    property alias propertyDateAppTimeStamp: dateAppTimeStamp
    property alias propertyRectAppCertificates: rectAppCertificates
    property alias propertyCheckboxRegister: checkboxRegister
    property alias propertyCheckboxRemove: checkboxRemove
    property alias propertyRectAppTimeStamp: rectAppTimeStamp
    property alias propertyCheckboxTimeStamp: checkboxTimeStamp
    property alias propertyTextFieldTimeStamp: textFieldTimeStamp
    property alias propertyCheckboxDisable: checkboxDisable
    property alias propertyRectOffice: rectOffice
    property alias propertyRectLoadCMDCerts: rectLoadCMDCerts
    property alias propertyLoadCMDCertsButton: loadCMDCertsButton

    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_DEFINITIONS_APP_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_DEFINITIONS_APP_ROW_TOP_INC_RELATIVE
    }

    Item {
        id: rowMain
        width: parent.width
        height: parent.height
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item {
            id: rectAppCertificates
            width: parent.width
            height: dateAppCertificates.height + rectAppCertificatesCheckBox.height
                    + 3*Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: dateAppCertificates
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: activeFocus
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp",
                                  "STR_CERTIFICATES_TITLE") + controler.autoTr
                Accessible.role: Accessible.TitleBar
                Accessible.name: text
                KeyNavigation.tab: checkboxRegister
                KeyNavigation.down: checkboxRegister
                KeyNavigation.right: checkboxRegister
                KeyNavigation.backtab: loadCMDCertsButton
                KeyNavigation.up: loadCMDCertsButton
            }
            DropShadow {
                anchors.fill: rectAppCertificatesCheckBox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectAppCertificatesCheckBox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectAppCertificatesCheckBox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectAppCertificatesCheckBox
                width: parent.width
                color: "white"
                height: checkboxRegister.height + checkboxRemove.height + 2
                        * Constants.SIZE_TEXT_V_SPACE
                anchors.top: dateAppCertificates.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                CheckBox {
                    id: checkboxRegister
                    text: qsTranslate(
                              "PageDefinitionsApp",
                              "STR_CERTIFICATES_REGISTER_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    font.bold: activeFocus
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    Accessible.role: Accessible.CheckBox
                    Accessible.name: text
                    KeyNavigation.tab: checkboxRemove
                    KeyNavigation.down: checkboxRemove
                    KeyNavigation.right: checkboxRemove
                    KeyNavigation.backtab: dateAppCertificates
                    KeyNavigation.up: dateAppCertificates
                    Keys.onEnterPressed: toggleSwitch(checkboxRegister)
                    Keys.onReturnPressed: toggleSwitch(checkboxRegister)
                }
                CheckBox {
                    id: checkboxRemove
                    text: qsTranslate(
                              "PageDefinitionsApp",
                              "STR_CERTIFICATES_REMOVE_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    font.bold: activeFocus
                    anchors.top: checkboxRegister.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    Accessible.role: Accessible.CheckBox
                    Accessible.name: text
                    KeyNavigation.tab: dateAppTimeStamp
                    KeyNavigation.down: dateAppTimeStamp
                    KeyNavigation.right: dateAppTimeStamp
                    KeyNavigation.backtab: checkboxRegister
                    KeyNavigation.up: checkboxRegister
                    Keys.onEnterPressed: toggleSwitch(checkboxRemove)
                    Keys.onReturnPressed: toggleSwitch(checkboxRemove)
                }
            }
        }
        Item {
            id: rectAppTimeStamp
            width: parent.width
            height: dateAppTimeStamp.height + rectAppTimeStampCheckBox.height
                    + 3*Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppCertificates.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: dateAppTimeStamp
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: activeFocus
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp",
                                  "STR_TIMESTAMP_TITLE") + controler.autoTr
                Accessible.role: Accessible.TitleBar
                Accessible.name: text
                KeyNavigation.tab: checkboxTimeStamp
                KeyNavigation.down: checkboxTimeStamp
                KeyNavigation.right: checkboxTimeStamp
                KeyNavigation.backtab: checkboxRemove
                KeyNavigation.up: checkboxRemove
            }
            DropShadow {
                anchors.fill: rectAppTimeStampCheckBox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectAppTimeStampCheckBox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectAppTimeStampCheckBox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectAppTimeStampCheckBox
                width: parent.width
                color: "white"
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                height: checkboxTimeStamp.height + boxAppTimeStamp.height
                anchors.top: dateAppTimeStamp.bottom

                CheckBox {
                    id: checkboxTimeStamp
                    text: qsTranslate("PageDefinitionsApp",
                                      "STR_TIMESTAMP_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    font.bold: activeFocus
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: false
                    Accessible.role: Accessible.CheckBox
                    Accessible.name: text
                    KeyNavigation.tab: checkboxTimeStamp.checked ? textFieldTimeStamp : (rectOffice.visible ? textOfficeTitle: dateAppTimeStamp)
                    KeyNavigation.down: checkboxTimeStamp.checked ? textFieldTimeStamp : (rectOffice.visible ? textOfficeTitle: dateAppTimeStamp)
                    KeyNavigation.right: checkboxTimeStamp.checked ? textFieldTimeStamp : (rectOffice.visible ? textOfficeTitle: dateAppTimeStamp)
                    KeyNavigation.backtab: dateAppTimeStamp
                    KeyNavigation.up: dateAppTimeStamp
                    Keys.onEnterPressed: toggleSwitch(checkboxTimeStamp)
                    Keys.onReturnPressed: toggleSwitch(checkboxTimeStamp)
                }
                Item {
                    id: boxAppTimeStamp
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: textFieldTimeStamp.height
                    anchors.top: checkboxTimeStamp.bottom
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE

                    TextField {
                        id: textFieldTimeStamp
                        width: parent.width
                        font.italic: textFieldTimeStamp.text === "" ? true : false
                        placeholderText: qsTranslate(
                                             "PageDefinitionsApp",
                                             "STR_TIMESTAMP_URL_OP") + controler.autoTr
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.bold: activeFocus
                        clip: false
                        enabled: checkboxTimeStamp.checked
                        opacity: checkboxTimeStamp.checked ? 1.0 : Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                        inputMethodHints: Qt.ImhUrlCharactersOnly
                        validator: RegExpValidator {
                            //http/https url validator
                            regExp: /https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*)/
                        }
                        Accessible.role: Accessible.EditableText
                        Accessible.name: text
                        KeyNavigation.tab: rectOffice.visible ? textOfficeTitle: dateAppTimeStamp
                        KeyNavigation.down: rectOffice.visible ? textOfficeTitle: dateAppTimeStamp
                        KeyNavigation.right: rectOffice.visible ? textOfficeTitle: dateAppTimeStamp
                        KeyNavigation.backtab: checkboxTimeStamp
                        KeyNavigation.up: checkboxTimeStamp
                    }
                }
            }
        }

        Item {
            id: rectOffice
            width: parent.width
            height: textOfficeTitle.height + rectOfficeCheckbox.height
                    + 3*Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppTimeStamp.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: textOfficeTitle
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: activeFocus
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsSignSettings",
                                  "STR_OFFICE_TITLE") + controler.autoTr
                Accessible.role: Accessible.TitleBar
                Accessible.name: text
                KeyNavigation.tab: checkboxDisable
                KeyNavigation.down: checkboxDisable
                KeyNavigation.right: checkboxDisable
                KeyNavigation.backtab: textFieldTimeStamp
                KeyNavigation.up: textFieldTimeStamp
            }
            DropShadow {
                anchors.fill: rectOfficeCheckbox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectOfficeCheckbox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectOfficeCheckbox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectOfficeCheckbox
                width: parent.width
                color: "white"
                height: checkboxDisable.height + 2
                        * Constants.SIZE_TEXT_V_SPACE
                anchors.top: textOfficeTitle.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                CheckBox {
                    id: checkboxDisable
                    text: qsTranslate(
                              "PageDefinitionsSignSettings",
                              "STR_DISABLE_EMAIL_MATCH") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    font.bold: activeFocus
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    Accessible.role: Accessible.CheckBox
                    Accessible.name: text
                    KeyNavigation.tab: textLoadCMDCertsTitle
                    KeyNavigation.down: textLoadCMDCertsTitle
                    KeyNavigation.right: textLoadCMDCertsTitle
                    KeyNavigation.backtab: textOfficeTitle
                    KeyNavigation.up: textOfficeTitle
                    Keys.onEnterPressed: toggleSwitch(checkboxDisable)
                    Keys.onReturnPressed: toggleSwitch(checkboxDisable)
                }
            }
        }

        Item {
            id: rectLoadCMDCerts
            width: parent.width
            height: textLoadCMDCertsTitle.height + rectLoadCMDCertsSettings.height
                    + 3*Constants.SIZE_TEXT_V_SPACE

            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectOffice.bottom // in Windows, rectOffice is visible
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: textLoadCMDCertsTitle
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                font.bold: activeFocus
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp","STR_CMD_CERT_TITLE") + controler.autoTr
                Accessible.role: Accessible.TitleBar
                Accessible.name: text
                KeyNavigation.tab: loadCMDCertsText
                KeyNavigation.down: loadCMDCertsText
                KeyNavigation.right: loadCMDCertsText
                KeyNavigation.backtab: checkboxDisable
                KeyNavigation.up: checkboxDisable
            }
            DropShadow {
                anchors.fill: rectLoadCMDCertsSettings
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectLoadCMDCertsSettings
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectLoadCMDCertsSettings
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectLoadCMDCertsSettings
                width: parent.width
                height: rectLoadCMDCertsText.height + rectLoadCMDCertsButton.height + 3
                            * Constants.SIZE_TEXT_V_SPACE
                anchors.top: textLoadCMDCertsTitle.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Rectangle {
                    id: rectLoadCMDCertsText
                    width: (parent.width-2*Constants.SIZE_ROW_H_SPACE)
                    color: "white"
                    height: loadCMDCertsText.height + 2
                            * Constants.SIZE_TEXT_V_SPACE
                    anchors.top: parent.top
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Text {
                        id: loadCMDCertsText
                        x: Constants.SIZE_TEXT_FIELD_H_SPACE
                        font.family: lato.name
                        font.bold: activeFocus
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        width: parent.width- Constants.SIZE_ROW_H_SPACE
                        text: qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_TEXT") + controler.autoTr
                        wrapMode: Text.WordWrap
                        Accessible.role: Accessible.TitleBar
                        Accessible.name: text
                        KeyNavigation.tab: loadCMDCertsButton
                        KeyNavigation.down: loadCMDCertsButton
                        KeyNavigation.right: loadCMDCertsButton
                        KeyNavigation.backtab: textLoadCMDCertsTitle
                        KeyNavigation.up: textLoadCMDCertsTitle
                    }
                }
                Rectangle {
                    id: rectLoadCMDCertsButton
                    width: (parent.width-2*Constants.SIZE_ROW_H_SPACE)
                    color: "white"
                    height: loadCMDCertsButton.height
                    anchors.top: rectLoadCMDCertsText.bottom
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Button {
                        id: loadCMDCertsButton
                        text: qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_BUTTON") + controler.autoTr
                        height: Constants.HEIGHT_BOTTOM_COMPONENT
                        width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE) * 0.50
                        highlighted: activeFocus
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        anchors.left: parent.left
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        KeyNavigation.tab: dateAppCertificates
                        KeyNavigation.down: dateAppCertificates
                        KeyNavigation.right: dateAppCertificates
                        KeyNavigation.backtab: loadCMDCertsText
                        KeyNavigation.up: loadCMDCertsText
                    }
                }
            }
        }
    }
}
