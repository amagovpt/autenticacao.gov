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
        id: rowNacionalDistrict
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectNacionalDistrict
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "Distrito Nacional"
                propertyDateField.text: "LISBOA"
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
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "Concelho"
                propertyDateField.text: "LISBOA"
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
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "Freguesia"
                propertyDateField.text: "PARQUE DAS NAÇÔES"
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
            id: rectAbbreviationStreetType
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "Abr. Tipo de Via"
                propertyDateField.text: "R"
            }
        }
        Item{
            id: rectStreetType
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectAbbreviationStreetType.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Tipo de Via"
                propertyDateField.text: "RUA"
            }
        }
        Item{
            id: rectStreetName
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectStreetType.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Designação da Via"
                propertyDateField.text: "Fim do Caminho"
            }
        }
    }
    Item {
        id: rowAbbreviationBuildingType
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowAbbreviationStreetType.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectAbbreviationBuildingType
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "Abr. Tipo de Edifício"
                propertyDateField.text: ""
            }
        }
        Item{
            id: rectBuildingType
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectAbbreviationBuildingType.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Tipo de Edifício"
                propertyDateField.text: ""
            }
        }
    }
    Item {
        id: rowDoorNo
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowAbbreviationBuildingType.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectDoorNo
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "N.º de Porta"
                propertyDateField.text: "Nº 11"
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
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
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
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "Lugar"
                propertyDateField.text: ""
            }
        }
        Item{
            id: rectLocality
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectPlace.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Localidade"
                propertyDateField.text: "Lisboa"
            }
        }
    }
    Item {
        id: rowZip4
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowPlace.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectZip4
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            Components.LabelTextBoxForm{
                propertyDateText.text: "CP4"
                propertyDateField.text: "1990"
            }
        }
        Item{
            id: rectZip3
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectZip4.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "CP3"
                propertyDateField.text: "303"
            }
        }
        Item{
            id: rectPostalLocality
            width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE ) * 0.33
            anchors.left: rectZip3.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Localidade Postal"
                propertyDateField.text: "Lisboa"
            }
        }
    }
    Item {
        id: rowConfirmationOfAddress
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowZip4.bottom
        anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE

        Rectangle{
            id: rectConfirmationOfAddressText
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            Text {
                id: dateField
                y: Constants.SIZE_TEXT_FIELD_V_SPACE
                text: "Para confirmar a alteração de morada, pressione o botão 'alteração de morada'"
                width: parent.width
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                font.capitalization: Font.AllUppercase
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                wrapMode: Text.WordWrap
            }
        }
        Rectangle{
            id: rectConfirmationOfAddressButton
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            height: parent.height
            anchors.left: rectConfirmationOfAddressText.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.Button {
                id: button
                buttonText: "Alteração de morada"
                width: parent.width
                height: parent.height
                color: Constants.COLOR_MAIN_SOFT_GRAY
                hoverColor: Constants.COLOR_MAIN_DARK_GRAY
                pressColor: Constants.COLOR_MAIN_BLUE
            }
        }
    }
}
