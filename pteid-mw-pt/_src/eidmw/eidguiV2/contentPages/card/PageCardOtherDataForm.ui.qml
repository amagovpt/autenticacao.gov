import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent

    property alias propertyBusyIndicator : busyIndication

    property alias propertyTextBoxNIF: textBoxNIF
    property alias propertyTextBoxNISS: textBoxNISS
    property alias propertyTextBoxNSNS: textBoxNSNS
    property alias propertyTextBoxCardVersion: textBoxCardVersion
    property alias propertyTextBoxIssueDate: textBoxIssueDate
    property alias propertyTextBoxIssuingEntity: textBoxIssuingEntity
    property alias propertyTextBoxDocumentType: textBoxDocumentType
    property alias propertyTextBoxPlaceOfRequest: textBoxPlaceOfRequest
    property alias propertyTextBoxCardState: textBoxCardState

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
            KeyNavigation.tab: textBoxNISS
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
            KeyNavigation.tab: textBoxNSNS
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
            KeyNavigation.tab: textBoxCardVersion
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
            KeyNavigation.tab: textBoxIssueDate
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
            KeyNavigation.tab: textBoxIssuingEntity
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
            KeyNavigation.tab: textBoxDocumentType
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
            KeyNavigation.tab: textBoxPlaceOfRequest
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
            KeyNavigation.tab: textBoxCardState
        }
    }

    Rectangle {
        id: rowCardStatus
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX + 20
        anchors.top: rowLocalOfRequest.bottom
        anchors.topMargin: 40
        Item{
            id: rectCardStatus
            width: parent.width
            height: parent.height
            Components.LabelTextBoxForm{
                id: textBoxCardState
                propertyDateText.text: qsTranslate("GAPI","STR_CARD_STATE")
                propertyRectField.height: Constants.HEIGHT_TEXT_BOX
                propertyDateField.wrapMode: Text.WordWrap
            }
            KeyNavigation.tab: textBoxNIF
        }
    }
}
