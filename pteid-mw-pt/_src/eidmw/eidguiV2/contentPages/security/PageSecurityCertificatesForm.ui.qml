/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

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
    anchors.fill: parent
    visible: true

    property alias propertyAcordion: acordion
    property alias propertyFileDialogOutput: fileDialogOutput
    property alias propertyRectEntity: rectEntity
    property alias propertyRectBottom: rectBottom
    property alias propertyButtonViewCertificate: buttonViewCertificate
    property alias propertyButtonExportCertificate: buttonExportCertificate
    property alias propertyBusyIndicator: busyIndicator
    property int rowVSpace: Constants.SIZE_ROW_V_SPACE * 0.8

    Item {
        id: main
        width: parent.width
        height: parent.height

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
            anchors.top: rowTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        }

        Rectangle{
            id: rectTop
            width: parent.width
            height: parent.height * 0.35 - titleCertificatesTitle.height
            color: "white"
            anchors.top: titleCertificatesTitle.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

            Components.Accordion {
                id: acordion
                anchors.fill: parent
                anchors.margins: 10
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
            anchors.topMargin: 1.95 * Constants.SIZE_ROW_V_SPACE
        }
        Item{
            id: rectBottom
            width: parent.width
            height: parent.height * 0.65 - titleSelectedBox.height 
                - 2 * Constants.SIZE_ROW_V_SPACE 
                - Constants.SIZE_TEXT_V_SPACE
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: titleSelectedBox.bottom
            anchors.topMargin: 0.9 * Constants.SIZE_TEXT_V_SPACE

            Item{
                id: rectEntity
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: parent.top
                anchors.topMargin: rowVSpace
                Components.LabelTextBoxForm{
                    id: textEntity
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_ENTITY")
                    propertyDateField.text: ""
                }
                Accessible.role: Accessible.Row
                Accessible.name: textEntity.accessibleText
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
                anchors.topMargin: rowVSpace
                Components.LabelTextBoxForm{
                    id: textAuth
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_ENTITY_ISSUER")
                    propertyDateField.text: ""
                }
                Accessible.role: Accessible.Row
                Accessible.name: textAuth.accessibleText
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
                anchors.topMargin: rowVSpace
                Components.LabelTextBoxForm{
                    id: textValid
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_VALID_FROM")
                    propertyDateField.text: ""
                }
                Accessible.role: Accessible.Row
                Accessible.name: textValid.accessibleText
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
                anchors.topMargin: rowVSpace
                Components.LabelTextBoxForm{
                    id: textUntil
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_VALID_UNTIL")
                    propertyDateField.text: ""
                }
                Accessible.role: Accessible.Row
                Accessible.name: textUntil.accessibleText
                KeyNavigation.tab: rectKey
                KeyNavigation.down: rectKey
                KeyNavigation.right: rectKey
                KeyNavigation.left: rectValid
                KeyNavigation.backtab: rectValid
                KeyNavigation.up: rectValid
            }
            Item{
                id: rectKey
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectUntil.bottom
                anchors.topMargin: rowVSpace
                Components.LabelTextBoxForm{
                    id: textKey
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_KEY_SIZE")
                    propertyDateField.text: ""
                }
                Accessible.role: Accessible.Row
                Accessible.name: textKey.accessibleText
                KeyNavigation.tab: rectStatus
                KeyNavigation.down: rectStatus
                KeyNavigation.right: rectStatus
                KeyNavigation.left: rectUntil
                KeyNavigation.backtab: rectUntil
                KeyNavigation.up: rectUntil
            }
            Item{
                id: rectStatus
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectKey.bottom
                anchors.topMargin: rowVSpace
                Components.LabelTextBoxForm{
                    id: textStatus
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_STATE")
                    //"Estado do certificado"
                    propertyDateField.text: ""
                }
                Accessible.role: Accessible.Row
                Accessible.name: textStatus.accessibleText
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
                    height:Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageSecurityCertificates","STR_OPEN_CERTIFICATE")
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus
                    Accessible.role: Accessible.Button
                    Accessible.name: text
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
                    anchors.horizontalCenter: (buttonViewCertificate.visible ? undefined : parent.horizontalCenter)
                    anchors.right: (buttonViewCertificate.visible ? parent.right : undefined)
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: rectEntity
                    KeyNavigation.down: rectEntity
                    KeyNavigation.right: rectEntity
                    KeyNavigation.up: buttonViewCertificate
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
            }
        }
    }
}
