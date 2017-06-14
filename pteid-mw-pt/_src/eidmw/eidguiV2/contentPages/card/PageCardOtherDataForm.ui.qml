import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    Item {
        id: rowTop
        width: parent.width - Constants.WIDTH_PHOTO_IMAGE - 2 * Constants.SIZE_ROW_H_SPACE
        height: parent.height * Constants.SIZE_ROW_TOP_V_RELATIVE

        Item{
            id: rectTopLeft
            width: parent.width
            height: parent.height
        }
        Item{
            id: rectToRight
            width: Constants.WIDTH_PHOTO_IMAGE + Constants.SIZE_ROW_H_SPACE
            height: parent.height
            anchors.left: rectTopLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            Image {
                anchors.bottom: parent.bottom
                width: parent.width
                antialiasing: true
                fillMode: Image.PreserveAspectFit
                source: "../../images/logo_cartao_cidadao.png"
            }
        }
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
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
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
                propertyDateText.text: "Tipo de documento"
                propertyDateField.text: "Cartão de Cidadão"
            }
        }
    }
    Item {
        id: rowCardStatus
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 3 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowIssuingEntity.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectCardStatus
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            Components.LabelTextBoxForm{
                propertyDateText.text: "Estado do Cartão"
                propertyDateField.text: "O Cartão do Cidadão foi ativado"
            }
        }
        Item{
            id: rectCardStatusText
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            anchors.left: rectCardStatus.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Text {
                id: dateField
                y: Constants.SIZE_TEXT_FIELD_V_SPACE
                text: "Para verificar se o cartão não está suspenso ou revogado, deverá validar os certificados no \
menu Segurança - Certificados"
                width: parent.width
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                font.capitalization: Font.MixedCase
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                wrapMode: Text.WordWrap
            }
        }
    }
    Item {
        id: rowLocalOfRequest
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowCardStatus.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectLocalOfRequest
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.5
            Components.LabelTextBoxForm{
                propertyDateText.text: "Local de Pedido"
                propertyDateField.text: "DIC Campus de Justiça Lisboa"
            }
        }
    }
}
