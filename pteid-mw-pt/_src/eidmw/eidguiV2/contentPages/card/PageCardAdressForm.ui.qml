import QtQuick 2.6
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_CARD_ADRESS_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_CARD_ADRESS_ROW_TOP_INC_RELATIVE
        Components.CardRowTop{}
    }

    Item {
        id: rowNacionalDistrict
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectNacionalDistrict
            width: parent.width
            Components.LabelTextBoxForm{
                propertyDateText.text: "Distrito"
                propertyDateField.text: "ANGRA DO HEROISMO"
            }
        }
    }
    Item {
        id: rowMunicipality
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowNacionalDistrict.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectMunicipality
            width: parent.width
            Components.LabelTextBoxForm{
                propertyDateText.text: "Concelho"
                propertyDateField.text: "FIGUEIRA DE CASTELO RODRIGO"
            }
        }
    }
    Item {
        id: rowCivilParish
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowMunicipality.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectCivilParish
            width: parent.width
            Components.LabelTextBoxForm{
                propertyDateText.text: "Freguesia"
                propertyDateField.text: "Belazaima do Chão, Castanheira do Vouga e Agadão"
            }
        }
    }
    Item {
        id: rowAbbreviationStreetType
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowCivilParish.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectStreetType
            width: (parent.width - Constants.SIZE_ROW_H_SPACE ) * 0.15
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Tipo de Via"
                propertyDateField.text: "Avenida"
            }
        }
        Item{
            id: rectStreetName
            width: (parent.width - Constants.SIZE_ROW_H_SPACE ) * 0.85
            anchors.left: rectStreetType.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Via"
                propertyDateField.text: "Professor Doutor António Augusto Gonçalves Rodrigues"
            }
        }
    }

    Item {
        id: rowDoorNo
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowAbbreviationStreetType.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectDoorNo
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "N.º"
                propertyDateField.text: "11"
            }
        }
        Item{
            id: rectFloor
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectDoorNo.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Andar"
                propertyDateField.text: "2º"
            }
        }
        Item{
            id: rectSide
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.34
            anchors.left: rectFloor.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Lado"
                propertyDateField.text: "Frente"
            }
        }
    }
    Item {
        id: rowPlace
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowDoorNo.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectPlace
            width: (parent.width - Constants.SIZE_ROW_H_SPACE ) * 0.50
            Components.LabelTextBoxForm{
                propertyDateText.text: "Lugar"
                propertyDateField.text: "Urbanização casas do lago"
            }
        }
        Item{
            id: rectLocality
            width: (parent.width - Constants.SIZE_ROW_H_SPACE ) * 0.50
            anchors.left: rectPlace.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Localidade"
                propertyDateField.text: "FIGUEIRA DE CASTELO RODRIGO"
            }
        }
    }
    Item {
        id: rowZip4
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        anchors.top: rowPlace.bottom

        Item{
            id: rectZip4
            width: 50
            Components.LabelTextBoxForm{
                propertyDateText.text: "Código Postal"
                propertyDateField.text: "1990"
            }
        }
        Item{
            id: textIfen
            width: 5
            height: parent.height
            anchors.left: rectZip4.right
            anchors.leftMargin: 5
            Text {
                text: "-"
                y: 20
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                color: Constants.COLOR_TEXT_BODY
            }
        }
        Item{
            id: rectZip3
            width: 40
            anchors.left: textIfen.right
            anchors.leftMargin: 5
            Components.LabelTextBoxForm{
                propertyDateText.text: ""
                propertyDateField.text: "303"
            }
        }
        Item{
            id: rectPostalLocality
            width: parent.width - 2 * Constants.SIZE_ROW_H_SPACE
                   - rectZip4.width - rectZip3.width - textIfen.width + 10
            anchors.left: rectZip3.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Localidade Postal"
                propertyDateField.text: "FIGUEIRA DE CASTELO RODRIGO"
            }
        }
    }
    Item {
        id: rowConfirmationOfAddress
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 3 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowZip4.bottom
        anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectConfirmationOfAddressText
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            Text {
                id: dateField
                y: Constants.SIZE_TEXT_FIELD_V_SPACE
                text: "Para confirmar a alteração de morada, pressione o botão alteração de morada"
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
        Item{
            id: rectConfirmationOfAddressButton
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            height: parent.height
            anchors.left: rectConfirmationOfAddressText.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Button {
                text: "Alteração de morada"
                width: Constants.WIDTH_BUTTON
                height:Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.right: parent.right
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
            }
        }
    }
}
