import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
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
            Components.LabelTextBoxForm{
                propertyDateText.text: "N.º Identificação Fiscal"
                propertyDateField.text: "21342342342342"
            }
        }
        Item{
            id: rectSocialSecurityNum
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectTaxNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "N.º Segurança Social"
                propertyDateField.text: "3453452452345"
            }
        }
        Item{
            id: rectHealthNum
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.34
            anchors.left: rectSocialSecurityNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "N. º Utente de Saúde"
                propertyDateField.text: "3453452452345234"
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
            Components.LabelTextBoxForm{
                propertyDateText.text: "Versão do Cartão"
                propertyDateField.text: "004.004.21"
            }
        }
        Item{
            id: rectIssuingDate
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            anchors.left: rectCardVersion.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Data de Emissão"
                propertyDateField.text: "23 10 2014"
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
            Components.LabelTextBoxForm{
                propertyDateText.text: "Entidade Emissora"
                propertyDateField.text: "República Portuguesa"
            }
        }
        Item{
            id: rectDocumentType
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            anchors.left: rectIssuingEntity.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Tipo de Documento"
                propertyDateField.text: "Cartão de Cidadão"
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
            Components.LabelTextBoxForm{
                propertyDateText.text: "Local de Pedido"
                propertyDateField.text: "DIC Campus de Justiça Lisboa"
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
                propertyDateText.text: "Estado do Cartão"
                propertyDateField.text: "Ativado"
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
