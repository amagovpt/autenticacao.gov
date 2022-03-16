/*-****************************************************************************

 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.1
import Qt.labs.platform 1.0
import eidguiV2 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    id: main
    anchors.fill: parent
    visible: true

    property alias propertyAcordion: acordion
    property alias propertyFileDialogOutput: fileDialogOutput
    property alias propertyRectEntity: rectEntity
    property alias propertyRectBottom: rectBottom
    property alias propertyButtonViewCertificate: buttonViewCertificate
    property alias propertyButtonExportCertificate: buttonExportCertificate
    property alias propertyButtonLessCertificateDetails: buttonLessCertificateDetails
    property alias propertyBusyIndicator: busyIndicator
    property alias propertyRowMain: rowMain

    BusyIndicator {
        id: busyIndicator
        running: false
        anchors.centerIn: parent
        // BusyIndicator should be on top of all other content
        z: 1
    }

    FileDialog {
        id: fileDialogOutput
        title: qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
        nameFilters: ["DER encoded binary X.509 (*.DER)", "All files (*)"]
        fileMode: FileDialog.SaveFile
        folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
    }

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_SECURITY_CERTIFICARES_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_SECURITY_CERTIFICARES_ROW_TOP_INC_RELATIVE

    }

    Flickable {
        id: rowMain
        width: parent.width + Constants.SIZE_ROW_H_SPACE
        height: parent.height - rowTop.height
        anchors.top: rowTop.bottom
        anchors.right: parent.right
        clip:true
        contentHeight: content.childrenRect.height

        ScrollBar.vertical: ScrollBar {
            id: settingsScroll
            parent: rowMain.parent
            visible: true
            active: true // QtQuick.Controls 2.1 does not have AlwaysOn prop
            width: Constants.SIZE_TEXT_FIELD_H_SPACE
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            stepSize : 1.0
        }

        Item{
            id: content
            width: rowMain.parent.width - Constants.SIZE_ROW_H_SPACE
            height: parent.height
            x: Constants.SIZE_ROW_H_SPACE

            DropShadow {
                anchors.fill: rectTop
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectTop
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectTop
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Text {
                id: titleCertificatesTitle
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_SELECT")
                anchors.top: content.top
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            }

            Rectangle{
                id: rectTop
                width: parent.width
                height: main.height * 0.34 - titleCertificatesTitle.height
                color: "white"
                anchors.top: titleCertificatesTitle.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                clip: true

                Components.Accordion {
                    id: acordion
                    anchors.fill: parent
                    anchors.margins: 8
                }
            }

            Text {
                id: titleSelectedBox
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_SELECTED")
                anchors.top: rectTop.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            }
            Item{
                id: rectBottom
                width: parent.width
                height: rectBottom.childrenRect.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: titleSelectedBox.bottom

                Item{
                    id: rectEntity
                    width: parent.width
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: parent.top
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    Components.LabelTextBoxForm{
                        id: textEntity
                        propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_ENTITY")
                        propertyDateField.text: ""
                    }
                    Accessible.role: Accessible.Row
                    Accessible.name: textEntity.accessibleText
                    Keys.onPressed: {
                        handleKeyPressed(event.key, rectEntity)
                    }
                    KeyNavigation.tab: rectAuth
                    KeyNavigation.down: rectAuth
                    KeyNavigation.right: rectAuth
                    KeyNavigation.left: buttonExportCertificate
                    KeyNavigation.backtab: buttonExportCertificate
                    KeyNavigation.up: buttonExportCertificate
                }
                Item{
                    id: rectAuth
                    width: parent.width
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: rectEntity.bottom
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    Components.LabelTextBoxForm{
                        id: textAuth
                        propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_ENTITY_ISSUER")
                        propertyDateField.text: ""
                    }
                    Accessible.role: Accessible.Row
                    Accessible.name: textAuth.accessibleText
                    Keys.onPressed: {
                        handleKeyPressed(event.key, rectAuth)
                    }
                    KeyNavigation.tab: rectValid
                    KeyNavigation.down: rectValid
                    KeyNavigation.right: rectValid
                    KeyNavigation.left: rectEntity
                    KeyNavigation.backtab: rectEntity
                    KeyNavigation.up: rectEntity
                }
                Item{
                    id: rectValid
                    width: (parent.width-Constants.SIZE_ROW_H_SPACE) / 2
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: rectAuth.bottom
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    Components.LabelTextBoxForm{
                        id: textValid
                        propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_VALID_FROM")
                        propertyDateField.text: ""
                    }
                    Accessible.role: Accessible.Row
                    Accessible.name: textValid.accessibleText
                    Keys.onPressed: {
                        handleKeyPressed(event.key, rectValid)
                    }
                    KeyNavigation.tab: rectUntil
                    KeyNavigation.down: rectUntil
                    KeyNavigation.right: rectUntil
                    KeyNavigation.left: rectAuth
                    KeyNavigation.backtab: rectAuth
                    KeyNavigation.up: rectAuth
                }
                Item{
                    id: rectUntil
                    width: (parent.width-Constants.SIZE_ROW_H_SPACE) / 2
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: rectAuth.bottom
                    anchors.left: rectValid.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    Components.LabelTextBoxForm{
                        id: textUntil
                        propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_VALID_UNTIL")
                        propertyDateField.text: ""
                    }
                    Accessible.role: Accessible.Row
                    Accessible.name: textUntil.accessibleText
                    Keys.onPressed: {
                        handleKeyPressed(event.key, rectUntil)
                    }
                    KeyNavigation.tab: rectKey
                    KeyNavigation.down: rectKey
                    KeyNavigation.right: rectKey
                    KeyNavigation.left: rectValid
                    KeyNavigation.backtab: rectValid
                    KeyNavigation.up: rectValid
                }
                Item{
                    id: rectKey
                    width: (parent.width-Constants.SIZE_ROW_H_SPACE) / 3
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: rectUntil.bottom
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    Components.LabelTextBoxForm{
                        id: textKey
                        propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_KEY_SIZE")
                        propertyDateField.text: ""
                    }
                    Accessible.role: Accessible.Row
                    Accessible.name: textKey.accessibleText
                    Keys.onPressed: {
                        handleKeyPressed(event.key, rectKey)
                    }
                    KeyNavigation.tab: rectSerialNumber
                    KeyNavigation.down: rectSerialNumber
                    KeyNavigation.right: rectSerialNumber
                    KeyNavigation.left: rectUntil
                    KeyNavigation.backtab: rectUntil
                    KeyNavigation.up: rectUntil
                }
                Item{
                    id: rectSerialNumber
                    width: (parent.width-Constants.SIZE_ROW_H_SPACE) * 2 / 3
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: rectUntil.bottom
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    anchors.left: rectKey.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Components.LabelTextBoxForm{
                        id: textSerialNumber
                        propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_SERIAL_NUMBER")
                        propertyDateField.text: ""
                    }
                    Accessible.role: Accessible.Row
                    Accessible.name: textKey.accessibleText
                    Keys.onPressed: {
                        handleKeyPressed(event.key, rectKey)
                    }
                    KeyNavigation.tab: rectStatus
                    KeyNavigation.down: rectStatus
                    KeyNavigation.right: rectStatus
                    KeyNavigation.left: rectKey
                    KeyNavigation.backtab: rectKey
                    KeyNavigation.up: rectKey
                }
                Item{
                    id: rectStatus
                    width: parent.width
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + textStatus.propertyRectField.height
                    anchors.top: rectKey.bottom
                    anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                    Components.LabelTextBoxForm{
                        id: textStatus
                        propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_STATE")
                        propertyDateField.text: ""
                        propertyRectField.height: propertyDateField.paintedHeight
                                            + Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP
                        propertyDateField.wrapMode: Text.WordWrap
                        propertyDateField.font.capitalization: Font.MixedCase
                    }
                    Accessible.role: Accessible.Row
                    Accessible.name: textStatus.accessibleText
                    Keys.onPressed: {
                        handleKeyPressed(event.key, rectStatus)
                    }
                    KeyNavigation.tab: (buttonViewCertificate.visible ? buttonViewCertificate : buttonExportCertificate)
                    KeyNavigation.down: (buttonViewCertificate.visible ? buttonViewCertificate : buttonExportCertificate)
                    KeyNavigation.right: (buttonViewCertificate.visible ? buttonViewCertificate : buttonExportCertificate)
                    KeyNavigation.left: rectKey
                    KeyNavigation.backtab: rectKey
                    KeyNavigation.up: rectKey
                }
                Item {
                    id: rectViewCertificate
                    width: parent.width
                    height: Constants.HEIGHT_BOTTOM_COMPONENT + Constants.SIZE_TEXT_V_SPACE
                    anchors.top: rectStatus.bottom
                    anchors.topMargin: 0.5 * Constants.SIZE_ROW_V_SPACE

                    Button {
                        id: buttonViewCertificate
                        visible: false
                        enabled: false
                        width: Constants.WIDTH_BUTTON
                        height: Constants.HEIGHT_BOTTOM_COMPONENT
                        text: qsTranslate("PageSecurityCertificates","STR_OPEN_CERTIFICATE")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, buttonViewCertificate)
                        }
                        KeyNavigation.tab: buttonExportCertificate
                        KeyNavigation.down: buttonExportCertificate
                        KeyNavigation.right: buttonExportCertificate
                        KeyNavigation.up: rectStatus
                        Keys.onEnterPressed: clicked()
                        Keys.onReturnPressed: clicked()
                    }

                    Button {
                        id: buttonExportCertificate
                        enabled: false
                        width: Constants.WIDTH_BUTTON
                        height:Constants.HEIGHT_BOTTOM_COMPONENT
                        text: qsTranslate("PageSecurityCertificates","STR_EXPORT_CERTIFICATE")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: (buttonViewCertificate.visible ? buttonViewCertificate.right : parent.left)
                        anchors.leftMargin: (buttonViewCertificate.visible ? (parent.width - Constants.WIDTH_BUTTON * 3) / 2 : 0)
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, buttonExportCertificate)
                        }
                        KeyNavigation.tab: buttonLessCertificateDetails
                        KeyNavigation.down: buttonLessCertificateDetails
                        KeyNavigation.right: buttonLessCertificateDetails
                        KeyNavigation.up: buttonViewCertificate
                        Keys.onEnterPressed: clicked()
                        Keys.onReturnPressed: clicked()
                    }

                    Button {
                        id: buttonLessCertificateDetails
                        enabled: true
                        width: Constants.WIDTH_BUTTON
                        height:Constants.HEIGHT_BOTTOM_COMPONENT
                        text: qsTranslate("PageSecurityCertificates","STR_SEE_LESS_CERTIFICATE_DETAILS")
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus
                        KeyNavigation.tab: rectEntity
                        KeyNavigation.down: rectEntity
                        KeyNavigation.right: rectEntity
                        KeyNavigation.left: buttonExportCertificate
                        KeyNavigation.backtab: buttonExportCertificate
                        KeyNavigation.up: buttonExportCertificate
                        Keys.onEnterPressed: clicked()
                        Keys.onReturnPressed: clicked()
                    }
                }
            }
        }
    }
}