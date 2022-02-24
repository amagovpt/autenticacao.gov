/*-****************************************************************************

 * Copyright (C) 2021 Tiago Barroso - <tiago.barroso@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import Qt.labs.platform 1.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import eidguiV2 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions
import "../../components/" as Components

Item{

    property alias propertyTextAuthIssuer: textAuthIssuer
    property alias propertyTextAuthStatus: textAuthStatus
    property alias propertyTextAuthValidFrom: textAuthValidFrom
    property alias propertyTextAuthValidUntil: textAuthValidUntil
    property alias propertyTextAuthSerialNumber: textAuthSerialNumber
    
    property alias propertyTextSignIssuer: textSignIssuer
    property alias propertyTextSignStatus: textSignStatus
    property alias propertyTextSignValidFrom: textSignValidFrom
    property alias propertyTextSignValidUntil: textSignValidUntil
    property alias propertyTextSignSerialNumber: textSignSerialNumber
    
    property alias propertyButtonValidateCertificates: buttonValidateCertificates
    property alias propertyButtonCertificateDetails: buttonCertificateDetails
    property alias propertyBusyIndicator: busyIndicator

    id: main
    anchors.fill: parent
    visible: true

    BusyIndicator {
        id: busyIndicator
        running: false
        anchors.centerIn: parent
        z: 1
    }

    Rectangle {
        id: certificatesArea
        width: parent.width
        height: childrenRect.height
        anchors.verticalCenter: parent.verticalCenter

        Text {
            id: titleAuthCertificate
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            font.family: lato.name
            font.bold: true
            color: Constants.COLOR_TEXT_LABEL
            height: Constants.SIZE_TEXT_LABEL
            text: qsTranslate("PageSecurityCertificates","STR_AUTH_CERTIFICATE")
            anchors.top: parent.top
            KeyNavigation.tab: rectAuthIssuer
            KeyNavigation.down: rectAuthIssuer
            KeyNavigation.right: rectAuthIssuer
            KeyNavigation.left: buttonValidateCertificates
            KeyNavigation.backtab: buttonValidateCertificates
            KeyNavigation.up: buttonValidateCertificates
        }

        DropShadow {
            anchors.fill: authCertificateArea
            horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
            verticalOffset: Constants.FORM_SHADOW_V_OFFSET
            radius: Constants.FORM_SHADOW_RADIUS
            samples: Constants.FORM_SHADOW_SAMPLES
            color: Constants.COLOR_FORM_SHADOW
            source: authCertificateArea
            spread: Constants.FORM_SHADOW_SPREAD
            opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
        }

        RectangularGlow {
            anchors.fill: authCertificateArea
            glowRadius: Constants.FORM_GLOW_RADIUS
            spread: Constants.FORM_GLOW_SPREAD
            color: Constants.COLOR_FORM_GLOW
            cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
            opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
        }
        
        Rectangle {
            id: authCertificateArea
            width: parent.width
            height: childrenRect.height + Constants.SIZE_ROW_V_SPACE 
            anchors.top: titleAuthCertificate.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE / 2

            Item{
                id: rectAuthIssuer
                width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: authCertificateArea.top
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE / 2
                anchors.horizontalCenter: parent.horizontalCenter
                Components.LabelTextBoxForm{
                    id: textAuthIssuer
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_ENTITY_ISSUER")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectAuthValidFrom
                KeyNavigation.down: rectAuthValidFrom
                KeyNavigation.right: rectAuthValidFrom
                KeyNavigation.left: titleAuthCertificate
                KeyNavigation.backtab: titleAuthCertificate
                KeyNavigation.up: titleAuthCertificate
            }

            Item{
                id: rectAuthValidFrom
                width: (parent.width - Constants.SIZE_ROW_H_SPACE - Constants.SIZE_TEXT_FIELD_H_SPACE) / 2
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectAuthIssuer.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                anchors.left: parent.left
                anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE / 2
                Components.LabelTextBoxForm{
                    id: textAuthValidFrom
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_VALID_FROM")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectAuthValidUntil
                KeyNavigation.down: rectAuthValidUntil
                KeyNavigation.right: rectAuthValidUntil
                KeyNavigation.left: rectAuthIssuer
                KeyNavigation.backtab: rectAuthIssuer
                KeyNavigation.up: rectAuthIssuer
            }

            Item{
                id: rectAuthValidUntil
                width: (parent.width - Constants.SIZE_ROW_H_SPACE - Constants.SIZE_TEXT_FIELD_H_SPACE) / 2
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectAuthIssuer.bottom
                anchors.left: rectAuthValidFrom.right
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    id: textAuthValidUntil
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_VALID_UNTIL")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectAuthSerialNumber
                KeyNavigation.down: rectAuthSerialNumber
                KeyNavigation.right: rectAuthSerialNumber
                KeyNavigation.left: rectAuthValidFrom
                KeyNavigation.backtab: rectAuthValidFrom
                KeyNavigation.up: rectAuthValidFrom
            }

            Item{
                id: rectAuthSerialNumber
                width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectAuthValidUntil.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter
                Components.LabelTextBoxForm{
                    id: textAuthSerialNumber
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_SERIAL_NUMBER")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectAuthStatus
                KeyNavigation.down: rectAuthStatus
                KeyNavigation.right: rectAuthStatus
                KeyNavigation.left: rectAuthValidUntil
                KeyNavigation.backtab: rectAuthValidUntil
                KeyNavigation.up: rectAuthValidUntil
            }

            Item{
                id: rectAuthStatus
                width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + textAuthStatus.propertyRectField.height
                anchors.top: rectAuthSerialNumber.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter
                Components.LabelTextBoxForm{
                    id: textAuthStatus
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_STATE")
                    propertyDateField.text: ""
                    propertyDateField.font.bold: true
                    propertyRectField.height: propertyDateField.paintedHeight
                                        + Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP
                    propertyDateField.wrapMode: Text.WordWrap
                    propertyDateField.font.capitalization: Font.MixedCase
                }
                KeyNavigation.tab: titleSignCertificate
                KeyNavigation.down: titleSignCertificate
                KeyNavigation.right: titleSignCertificate
                KeyNavigation.left: rectAuthSerialNumber
                KeyNavigation.backtab: rectAuthSerialNumber
                KeyNavigation.up: rectAuthSerialNumber
            }
        }

        Text {
            id: titleSignCertificate
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            font.family: lato.name
            font.bold: true
            color: Constants.COLOR_TEXT_LABEL
            height: Constants.SIZE_TEXT_LABEL
            text: qsTranslate("PageSecurityCertificates","STR_SIGN_CERTIFICATE")
            anchors.top: authCertificateArea.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE * 2
            KeyNavigation.tab: rectSignIssuer
            KeyNavigation.down: rectSignIssuer
            KeyNavigation.right: rectSignIssuer
            KeyNavigation.left: rectAuthStatus
            KeyNavigation.backtab: rectAuthStatus
            KeyNavigation.up: rectAuthStatus
        }

        DropShadow {
            anchors.fill: signCertificateArea
            horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
            verticalOffset: Constants.FORM_SHADOW_V_OFFSET
            radius: Constants.FORM_SHADOW_RADIUS
            samples: Constants.FORM_SHADOW_SAMPLES
            color: Constants.COLOR_FORM_SHADOW
            source: signCertificateArea
            spread: Constants.FORM_SHADOW_SPREAD
            opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
        }

        RectangularGlow {
            anchors.fill: signCertificateArea
            glowRadius: Constants.FORM_GLOW_RADIUS
            spread: Constants.FORM_GLOW_SPREAD
            color: Constants.COLOR_FORM_GLOW
            cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
            opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
        }

        Rectangle {
            id: signCertificateArea
            width: parent.width
            height: childrenRect.height + Constants.SIZE_ROW_V_SPACE
            anchors.top: titleSignCertificate.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE / 2

            Item{
                id: rectSignIssuer
                width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: signCertificateArea.top
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE / 2
                anchors.left: parent.left
                anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE / 2
                Components.LabelTextBoxForm{
                    id: textSignIssuer
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_ENTITY_ISSUER")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectSignValidFrom
                KeyNavigation.down: rectSignValidFrom
                KeyNavigation.right: rectSignValidFrom
                KeyNavigation.left: titleSignCertificate
                KeyNavigation.backtab: titleSignCertificate
                KeyNavigation.up: titleSignCertificate
            }

            Item{
                id: rectSignValidFrom
                width: (parent.width - Constants.SIZE_ROW_H_SPACE - Constants.SIZE_TEXT_FIELD_H_SPACE) / 2
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectSignIssuer.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                anchors.left: parent.left
                anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE / 2
                Components.LabelTextBoxForm{
                    id: textSignValidFrom
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_VALID_FROM")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectSignValidUntil
                KeyNavigation.down: rectSignValidUntil
                KeyNavigation.right: rectSignValidUntil
                KeyNavigation.left: rectSignIssuer
                KeyNavigation.backtab: rectSignIssuer
                KeyNavigation.up: rectSignIssuer
            }

            Item{
                id: rectSignValidUntil
                width: (parent.width - Constants.SIZE_ROW_H_SPACE - Constants.SIZE_TEXT_FIELD_H_SPACE) / 2
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectSignIssuer.bottom
                anchors.left: rectSignValidFrom.right
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    id: textSignValidUntil
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_VALID_UNTIL")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectSignSerialNumber
                KeyNavigation.down: rectSignSerialNumber
                KeyNavigation.right: rectSignSerialNumber
                KeyNavigation.left: rectSignValidFrom
                KeyNavigation.backtab: rectSignValidFrom
                KeyNavigation.up: rectSignValidFrom
            }

            Item{
                id: rectSignSerialNumber
                width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectSignValidUntil.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                anchors.left: parent.left
                anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE / 2
                Components.LabelTextBoxForm{
                    id: textSignSerialNumber
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_SERIAL_NUMBER")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectSignStatus
                KeyNavigation.down: rectSignStatus
                KeyNavigation.right: rectSignStatus
                KeyNavigation.left: rectSignValidUntil
                KeyNavigation.backtab: rectSignValidUntil
                KeyNavigation.up: rectSignValidUntil
            }

            Item{
                id: rectSignStatus
                width: parent.width - Constants.SIZE_TEXT_FIELD_H_SPACE
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + textSignStatus.propertyRectField.height
                anchors.top: rectSignSerialNumber.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                anchors.left: parent.left
                anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE / 2
                Components.LabelTextBoxForm{
                    id: textSignStatus
                    propertyDateText.text: qsTranslate("PageSecurityCertificates","STR_CERTIFICATES_STATE")
                    propertyDateField.text: ""
                    propertyDateField.font.bold: true
                    propertyRectField.height: propertyDateField.paintedHeight
                                        + Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP
                    propertyDateField.wrapMode: Text.WordWrap
                    propertyDateField.font.capitalization: Font.MixedCase
                }
                KeyNavigation.tab: buttonValidateCertificates
                KeyNavigation.down: buttonValidateCertificates
                KeyNavigation.right: buttonValidateCertificates
                KeyNavigation.left: rectSignSerialNumber
                KeyNavigation.backtab: rectSignSerialNumber
                KeyNavigation.up: rectSignSerialNumber
            }
        }

        Button {
            id: buttonValidateCertificates
            enabled: false
            width: Constants.WIDTH_BUTTON
            height:Constants.HEIGHT_BOTTOM_COMPONENT
            text: qsTranslate("PageSecurityCertificates","STR_VALIDATE_CERTIFICATES")
            anchors.top: signCertificateArea.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            anchors.left: parent.left  
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            font.family: lato.name
            font.capitalization: Font.MixedCase
            highlighted: activeFocus
            KeyNavigation.tab: buttonCertificateDetails
            KeyNavigation.down: buttonCertificateDetails
            KeyNavigation.right: buttonCertificateDetails
            KeyNavigation.left: rectSignStatus
            KeyNavigation.backtab: rectSignStatus
            KeyNavigation.up: rectSignStatus
            Keys.onEnterPressed: clicked()
            Keys.onReturnPressed: clicked()
        }

        Button {
            id: buttonCertificateDetails
            enabled: true
            width: Constants.WIDTH_BUTTON
            height:Constants.HEIGHT_BOTTOM_COMPONENT
            text: qsTranslate("PageSecurityCertificates","STR_SEE_CERTIFICATE_DETAILS")
            anchors.top: signCertificateArea.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            anchors.right: parent.right
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            font.family: lato.name
            font.capitalization: Font.MixedCase
            highlighted: activeFocus
            KeyNavigation.tab: titleAuthCertificate
            KeyNavigation.down: titleAuthCertificate
            KeyNavigation.right: titleAuthCertificate
            KeyNavigation.left: buttonValidateCertificates
            KeyNavigation.backtab: buttonValidateCertificates
            KeyNavigation.up: buttonValidateCertificates
            Keys.onEnterPressed: clicked()
            Keys.onReturnPressed: clicked()
        }
    }
}