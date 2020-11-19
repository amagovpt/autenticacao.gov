/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 - 2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias propertyBusyIndicator: busyIndicator
    property alias propertyRowMain: rowMain
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
    property alias propertyCheckboxCMDRegRemember: checkboxCMDRegRemember
    property alias propertyRectAddRootCACert: rectAddRootCACert
    property alias propertyButtonAddCACert: buttonAddCACert
    property var propertyIsRootCaCertInstalled: false

    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_DEFINITIONS_APP_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_DEFINITIONS_APP_ROW_TOP_INC_RELATIVE
    }

    Flickable {
        id: rowMain
        width: parent.width + Constants.SIZE_ROW_H_SPACE
        height: parent.height - rowTop.height - Constants.SIZE_ROW_V_SPACE
        anchors.top: rowTop.bottom
        anchors.right: parent.right
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        clip:true
        contentHeight: content.childrenRect.height + 2*Constants.SIZE_ROW_V_SPACE

        ScrollBar.vertical: ScrollBar {
            id: settingsScroll
            parent: rowMain.parent
            visible: true
            active: true // QtQuick.Controls 2.1 does not have AlwaysOn prop
            width: Constants.SIZE_TEXT_FIELD_H_SPACE
            anchors.right: parent.right
            anchors.top: rowTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            anchors.bottom: parent.bottom
            stepSize : 1.0
        }

        BusyIndicator {
            id: busyIndicator
            running: false
            anchors.centerIn: parent
            // BusyIndicator should be on top of all other content
            z: 1
        }

        Item{
            id: content
            anchors.top: parent.top
            width: rowMain.parent.width - Constants.SIZE_ROW_H_SPACE
                x: Constants.SIZE_ROW_H_SPACE
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
                    // If this component is changed, the workaround to scroll the page automatically with
                    // keyboard navigation has to be updated also;
                    // this is located in PageDefinitionsSignSettings.qml - function handleKeyPressed
                    Keys.onPressed: {
                        handleKeyPressed(event.key, dateAppCertificates)
                    }
                    KeyNavigation.tab: checkboxRegister
                    KeyNavigation.down: checkboxRegister
                    KeyNavigation.right: checkboxRegister
                    KeyNavigation.backtab: buttonAddCACert
                    KeyNavigation.up: buttonAddCACert
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
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxRegister)
                        }
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
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxRemove)
                        }
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
                    Keys.onPressed: {
                        handleKeyPressed(event.key, dateAppTimeStamp)
                    }
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
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxTimeStamp)
                        }
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
                            Keys.onPressed: {
                                handleKeyPressed(event.key, textFieldTimeStamp)
                            }
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
                    Keys.onPressed: {
                        handleKeyPressed(event.key, textOfficeTitle)
                    }
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
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxDisable)
                        }
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
                    Keys.onPressed: {
                        handleKeyPressed(event.key, textLoadCMDCertsTitle)
                    }
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
                    Item {
                        id: rectLoadCMDCertsText
                        width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                        x: Constants.SIZE_TEXT_FIELD_H_SPACE
                        height: loadCMDCertsText.height + 2
                                * Constants.SIZE_TEXT_V_SPACE
                        anchors.top: parent.top
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                        Text {
                            id: loadCMDCertsText
                            font.family: lato.name
                            font.bold: activeFocus
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            width: parent.width
                            text: qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_TEXT") + controler.autoTr
                            wrapMode: Text.WordWrap
                            Accessible.role: Accessible.TitleBar
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key, loadCMDCertsText)
                            }
                            KeyNavigation.tab: loadCMDCertsButton
                            KeyNavigation.down: loadCMDCertsButton
                            KeyNavigation.right: loadCMDCertsButton
                            KeyNavigation.backtab: textLoadCMDCertsTitle
                            KeyNavigation.up: textLoadCMDCertsTitle
                        }
                    }
                    Item {
                        id: rectLoadCMDCertsButton
                        width: parent.width * 0.3
                        x: Constants.SIZE_TEXT_FIELD_H_SPACE
                        height: loadCMDCertsButton.height
                        anchors.top: rectLoadCMDCertsText.bottom
                        anchors.leftMargin: 100
                        Button {
                            id: loadCMDCertsButton
                            text: qsTranslate("PageDefinitionsApp","STR_REGISTER_CMD_CERT_BUTTON") + controler.autoTr
                            height: Constants.HEIGHT_BOTTOM_COMPONENT
                            width: Constants.WIDTH_BUTTON
                            highlighted: activeFocus
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.bold: activeFocus
                            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                            Accessible.role: Accessible.CheckBox
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key, loadCMDCertsButton)
                            }
                            KeyNavigation.tab: checkboxCMDRegRemember
                            KeyNavigation.down: checkboxCMDRegRemember
                            KeyNavigation.right: checkboxCMDRegRemember
                            KeyNavigation.backtab: loadCMDCertsText
                            KeyNavigation.up: loadCMDCertsText
                        }
                    }
                    Item {
                        id: rectLoadCMDCertsRemember
                        width: parent.width * 0.7 - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        height: loadCMDCertsButton.height
                        anchors.top: rectLoadCMDCertsText.bottom
                        anchors.right: parent.right

                        CheckBox {
                            id: checkboxCMDRegRemember
                            text: qsTranslate(
                                      "PageDefinitionsSignSettings",
                                      "STR_CMD_REGISTER_REMEMBER") + controler.autoTr
                            height: 25
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.bold: activeFocus
                            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                            Accessible.role: Accessible.CheckBox
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key, checkboxCMDRegRemember)
                            }
                            KeyNavigation.tab: textRootCACertTitle
                            KeyNavigation.down: textRootCACertTitle
                            KeyNavigation.right: textRootCACertTitle
                            KeyNavigation.backtab: loadCMDCertsButton
                            KeyNavigation.up: loadCMDCertsButton
                            Keys.onEnterPressed: toggleSwitch(checkboxDisable)
                            Keys.onReturnPressed: toggleSwitch(checkboxDisable)
                        }
                    }
                }
            }

            Item{
                id: rectAddRootCACert
                width: parent.width
                height: 6*Constants.SIZE_TEXT_FIELD + 4*Constants.SIZE_ROW_V_SPACE
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.rightMargin: Constants.SIZE_ROW_H_SPACE
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP
                anchors.top : rectLoadCMDCerts.bottom

                Text {
                    id: textRootCACertTitle
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: focus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp","STR_ROOT_CA_CERT_INSTALL_TITLE")
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: text
                    Keys.onPressed: {
                        handleKeyPressed(event.key, textRootCACertTitle)
                    }
                    KeyNavigation.tab: textRootCACertDesc
                    KeyNavigation.down: textRootCACertDesc
                    KeyNavigation.right: textRootCACertDesc
                    KeyNavigation.backtab: checkboxCMDRegRemember
                    KeyNavigation.up: checkboxCMDRegRemember
                }
                DropShadow {
                    anchors.fill: rowAddRootCACert
                    horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                    verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                    radius: Constants.FORM_SHADOW_RADIUS
                    samples: Constants.FORM_SHADOW_SAMPLES
                    color: Constants.COLOR_FORM_SHADOW
                    source: rowAddRootCACert
                    spread: Constants.FORM_SHADOW_SPREAD
                    opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
                }
                RectangularGlow {
                    anchors.fill: rowAddRootCACert
                    glowRadius: Constants.FORM_GLOW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                    opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
                }
                Rectangle {
                    id: rowAddRootCACert
                    width: parent.width
                    height: 6*Constants.SIZE_TEXT_FIELD + 3*Constants.SIZE_ROW_V_SPACE
                    anchors.top: textRootCACertTitle.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    Rectangle {
                        id: rectRootCACert
                        height: 4*Constants.SIZE_TEXT_FIELD
                        anchors.top: rowAddRootCACert.top
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                        anchors.left: parent.left
                        anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE 
                        anchors.right: parent.right
                        anchors.rightMargin: Constants.SIZE_TEXT_FIELD_H_SPACE 
                        Text {
                            id: textRootCACertDesc
                            width: parent.width
                            text: qsTranslate("PageDefinitionsApp","STR_ROOT_CA_CERT_INSTALL_DESC")
                            font.capitalization: Font.MixedCase
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.family: lato.name
                            font.bold: focus
                            wrapMode: Text.WordWrap
                            Accessible.role: Accessible.StaticText
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key, textRootCACertDesc)
                            }
                            KeyNavigation.tab: linkManualProblemsNewCertChain
                            KeyNavigation.down: linkManualProblemsNewCertChain
                            KeyNavigation.right: linkManualProblemsNewCertChain
                            KeyNavigation.backtab: textRootCACertTitle
                            KeyNavigation.up: textRootCACertTitle
                        }
                        Components.Link {
                            id: linkManualProblemsNewCertChain
                            anchors.top: textRootCACertDesc.bottom
                            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                            width: parent.width
                            propertyText.text: qsTranslate("PageDefinitionsApp", "STR_MORE_INFO") 
                                + "<a href='https://amagovpt.github.io/autenticacao.gov/user_manual.html#problemas-com-a-nova-cadeia-de-confian%C3%A7a'>" 
                                + qsTranslate("PageDefinitionsApp", "STR_HERE") + "</a>."
                            propertyLinkUrl: 'https://amagovpt.github.io/autenticacao.gov/user_manual.html#problemas-com-a-nova-cadeia-de-confian%C3%A7a'
                            propertyText.font.capitalization: Font.MixedCase
                            propertyText.font.pixelSize: Constants.SIZE_TEXT_LINK_LABEL
                            propertyText.font.bold: activeFocus
                            Keys.onPressed: {
                                handleKeyPressed(event.key, linkManualProblemsNewCertChain)
                            }
                            KeyNavigation.tab: textIsCertInstalled
                            KeyNavigation.down: textIsCertInstalled
                            KeyNavigation.right: textIsCertInstalled
                            KeyNavigation.left: textRootCACertDesc
                            KeyNavigation.backtab: textRootCACertDesc
                            KeyNavigation.up: textRootCACertDesc
                        }
                    }
                
                    Rectangle {
                        id: rectInstallRootCACert
                        height: 2*Constants.SIZE_TEXT_FIELD
                        anchors.top: rectRootCACert.bottom
                        anchors.topMargin: Constants.SIZE_ROW_V_SPACE 
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE 
                        anchors.rightMargin: Constants.SIZE_TEXT_FIELD_H_SPACE 

                        Rectangle {
                            id: rectIsCertInstalled
                            anchors.left: parent.left
                            anchors.right: parent.right
                            Text {
                                id: textIsCertInstalled
                                text: propertyIsRootCaCertInstalled ? qsTranslate("PageDefinitionsApp","STR_CERT_ALREADY_INSTALLED")
                                                                     : qsTranslate("PageDefinitionsApp","STR_CERT_NOT_INSTALLED")
                                anchors.left: parent.left
                                font.capitalization: Font.MixedCase
                                font.pixelSize: Constants.SIZE_TEXT_FIELD
                                font.family: lato.name
                                font.bold: focus
                                wrapMode: Text.WordWrap
                                topPadding: Constants.SIZE_ROW_V_SPACE
                                Accessible.role: Accessible.StaticText
                                Accessible.name: text
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, textIsCertInstalled)
                                }
                                KeyNavigation.tab: buttonAddCACert
                                KeyNavigation.down: buttonAddCACert
                                KeyNavigation.right: buttonAddCACert
                                KeyNavigation.backtab: linkManualProblemsNewCertChain
                                KeyNavigation.up: linkManualProblemsNewCertChain
                            }
                            Button {
                                id: buttonAddCACert
                                enabled: !propertyIsRootCaCertInstalled
                                text: qsTranslate("PageDefinitionsApp","STR_INSTALL_CERT")
                                anchors.right: parent.right
                                width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE) * 0.50
                                height: Constants.HEIGHT_BOTTOM_COMPONENT
                                font.pixelSize: Constants.SIZE_TEXT_FIELD
                                font.family: lato.name
                                font.capitalization: Font.MixedCase
                                highlighted: activeFocus
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, buttonAddCACert)
                                }
                                KeyNavigation.tab: dateAppCertificates
                                KeyNavigation.down: dateAppCertificates
                                KeyNavigation.right: dateAppCertificates
                                KeyNavigation.backtab: textIsCertInstalled
                                KeyNavigation.up: textIsCertInstalled
                            }
                        }
                    }
                }
            }
        }
    }
}
