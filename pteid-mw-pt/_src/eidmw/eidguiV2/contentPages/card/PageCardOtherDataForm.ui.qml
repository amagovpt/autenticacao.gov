/*-****************************************************************************

 * Copyright (C) 2017 - 2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent

    property alias propertyBusyIndicator : busyIndication

    property alias propertyTextBoxNIF: textBoxNIF
    property alias propertyRectTaxNum: rectTaxNum
    property alias propertyTextBoxNISS: textBoxNISS
    property alias propertyTextBoxNSNS: textBoxNSNS
    property alias propertyTextBoxCardVersion: textBoxCardVersion
    property alias propertyTextBoxIssueDate: textBoxIssueDate
    property alias propertyTextBoxIssuingEntity: textBoxIssuingEntity
    property alias propertyTextBoxDocumentType: textBoxDocumentType
    property alias propertyTextBoxPlaceOfRequest: textBoxPlaceOfRequest

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_CARD_OTHER_DATA_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_CARD_OTHER_DATA_ROW_TOP_INC_RELATIVE
        Components.CardRowTop{}
    }

    BusyIndicator {
       id: busyIndication
       running: false
       anchors.centerIn: parent
       // BusyIndicator should be on top of all other content
       z: 1
    }

    Item {
        id: rowTaxNum
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectTaxNum
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            height: parent.height
            Components.LabelTextBoxForm {
                id: textBoxNIF
                propertyDateText.text: qsTranslate("GAPI","STR_VAT_NUM")
                propertyDateField.text: ""
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxNIF.accessibleText
            KeyNavigation.tab: rectSocialSecurityNum
            KeyNavigation.down: rectSocialSecurityNum
            KeyNavigation.right: rectSocialSecurityNum
            KeyNavigation.backtab: rectLocalOfRequest
            KeyNavigation.up: rectLocalOfRequest
        }
        Item{
            id: rectSocialSecurityNum
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            height: parent.height
            anchors.left: rectTaxNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxNISS
                propertyDateText.text: qsTranslate("GAPI","STR_SOCIAL_SECURITY_NUM")
                propertyDateField.text: ""
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxNISS.accessibleText
            KeyNavigation.tab: rectHealthNum
            KeyNavigation.down: rectHealthNum
            KeyNavigation.right: rectHealthNum
            KeyNavigation.backtab: rectTaxNum
            KeyNavigation.up: rectTaxNum
        }
        Item{
            id: rectHealthNum
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.34
            height: parent.height
            anchors.left: rectSocialSecurityNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxNSNS
                propertyDateText.text: qsTranslate("GAPI","STR_NATIONAL_HEALTH_NUM")
                propertyDateField.text: ""
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxNSNS.accessibleText
            KeyNavigation.tab: rectCardVersion
            KeyNavigation.down: rectCardVersion
            KeyNavigation.right: rectCardVersion
            KeyNavigation.backtab: rectSocialSecurityNum
            KeyNavigation.up: rectSocialSecurityNum
        }
    }

    Item {
        id: rowCardVersion
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowTaxNum.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectCardVersion
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            height: parent.height
            Components.LabelTextBoxForm {
                id: textBoxCardVersion
                propertyDateText.text: qsTranslate("GAPI","STR_CARD_VERSION")
                propertyDateField.text: ""
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxCardVersion.accessibleText
            KeyNavigation.tab: rectIssuingDate
            KeyNavigation.down: rectIssuingDate
            KeyNavigation.right: rectIssuingDate
            KeyNavigation.backtab: rectHealthNum
            KeyNavigation.up: rectHealthNum
        }
        Item{
            id: rectIssuingDate
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            height: parent.height
            anchors.left: rectCardVersion.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxIssueDate
                propertyDateText.text: qsTranslate("GAPI","STR_DELIVERY_DATE")
                propertyDateField.text: ""
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxIssueDate.accessibleText
            KeyNavigation.tab: rectIssuingEntity
            KeyNavigation.down: rectIssuingEntity
            KeyNavigation.right: rectIssuingEntity
            KeyNavigation.backtab: rectCardVersion
            KeyNavigation.up: rectCardVersion
        }
    }
    Item {
        id: rowIssuingEntity
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowCardVersion.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectIssuingEntity
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            height: parent.height
            Components.LabelTextBoxForm {
                id: textBoxIssuingEntity
                propertyDateText.text: qsTranslate("GAPI","STR_DELIVERY_ENTITY")
                propertyDateField.text: ""
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxIssuingEntity.accessibleText
            KeyNavigation.tab: rectDocumentType
            KeyNavigation.down: rectDocumentType
            KeyNavigation.right: rectDocumentType
            KeyNavigation.backtab: rectIssuingDate
            KeyNavigation.up: rectIssuingDate
        }
        Item{
            id: rectDocumentType
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            height: parent.height
            anchors.left: rectIssuingEntity.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxDocumentType
                propertyDateText.text: qsTranslate("GAPI","STR_DOCUMENT_TYPE")
                propertyDateField.text: ""
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxDocumentType.accessibleText
            KeyNavigation.tab: rectLocalOfRequest
            KeyNavigation.down: rectLocalOfRequest
            KeyNavigation.right: rectLocalOfRequest
            KeyNavigation.backtab: rectIssuingEntity
            KeyNavigation.up: rectIssuingEntity
        }
    }
    Item {
        id: rowLocalOfRequest
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowIssuingEntity.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectLocalOfRequest
            width: parent.width
            height: parent.height
            Components.LabelTextBoxForm {
                id: textBoxPlaceOfRequest
                propertyDateText.text: qsTranslate("GAPI","STR_DELIVERY_LOCATION")
                propertyDateField.text: ""
            }
            Accessible.role: Accessible.Column
            Accessible.name: textBoxPlaceOfRequest.accessibleText
            KeyNavigation.tab: rectTaxNum
            KeyNavigation.down: rectTaxNum
            KeyNavigation.right: rectTaxNum
            KeyNavigation.backtab: rectLocalOfRequest
            KeyNavigation.up: rectLocalOfRequest
        }
    }
}
