import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent

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

    Item {
        id: rowTaxNum
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectTaxNum
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm {
                id: textBoxNIF
                propertyDateText.text: "N.º Identificação Fiscal"
                propertyDateField.text: ""
            }
        }
        Item{
            id: rectSocialSecurityNum
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectTaxNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxNISS
                propertyDateText.text: "N.º Segurança Social"
                propertyDateField.text: ""
            }
        }
        Item{
            id: rectHealthNum
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.34
            anchors.left: rectSocialSecurityNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxNSNS
                propertyDateText.text: "N. º Utente de Saúde"
                propertyDateField.text: ""
            }
        }
    }

    Item {
        id: rowCardVersion
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowTaxNum.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectCardVersion
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            Components.LabelTextBoxForm {
                id: textBoxCardVersion
                propertyDateText.text: "Versão do Cartão"
                propertyDateField.text: ""
            }
        }
        Item{
            id: rectIssuingDate
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            anchors.left: rectCardVersion.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm {
                id: textBoxIssueDate
                propertyDateText.text: "Data de Emissão"
                propertyDateField.text: ""
            }
        }
    }
    Item {
        id: rowIssuingEntity
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowCardVersion.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectIssuingEntity
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            Components.LabelTextBoxForm {
                id: textBoxIssuingEntity
                propertyDateText.text: "Entidade Emissora"
                propertyDateField.text: ""
            }
        }
        Item{
            id: rectDocumentType
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            anchors.left: rectIssuingEntity.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxDocumentType
                propertyDateText.text: "Tipo de Documento"
                propertyDateField.text: ""
            }
        }
    }
    Item {
        id: rowLocalOfRequest
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowIssuingEntity.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectLocalOfRequest
            width: (parent.width )
            Components.LabelTextBoxForm {
                id: textBoxPlaceOfRequest
                propertyDateText.text: "Local de Pedido"
                propertyDateField.text: ""
            }
        }
    }

    Item {
        id: rowCardStatus
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowLocalOfRequest.bottom
        anchors.topMargin: 40

        Item{
            id: rectCardStatus
            width: (parent.width )
            Components.LabelTextBoxForm{
                id: textBoxCardState
                propertyDateText.text: "Estado do Cartão"
                propertyDateField.text: ""
            }
        }
    }

    Item {
        id: rowCardStatusText
        width: parent.width
        anchors.top: rowCardStatus.bottom
        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

        Text {
            id: dateField
            text: "Para verificar se o cartão não está suspenso ou revogado, \
deverá validar os certificados em Segurança - Certificados"
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            font.capitalization: Font.MixedCase
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            font.family: lato.name
            color: Constants.COLOR_TEXT_LABEL
            wrapMode: Text.WordWrap
        }
    }
}
