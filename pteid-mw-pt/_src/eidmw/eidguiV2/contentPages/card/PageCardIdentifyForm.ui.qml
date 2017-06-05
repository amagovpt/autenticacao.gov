import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import "../../scripts/Constants.js" as Constants
import "../../components" as Components


Item {
    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width - Constants.WIDTH_PHOTO_IMAGE - 2 * Constants.SIZE_ROW_H_SPACE
        height: parent.height * Constants.SIZE_FORM_RECT_TOP_V_RELATIVE

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
        id: rowNamePhoto
        width: parent.width
        height: Constants.HEIGHT_PHOTO_IMAGE + Constants.SIZE_TEXT_LABEL + Constants.SIZE_TEXT_V_SPACE
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectNamePhotoLeft
            width: parent.width - Constants.WIDTH_PHOTO_IMAGE - Constants.SIZE_ROW_H_SPACE
            height: parent.height

            Item{
                id: givenNameTextForm
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                Components.LabelTextBoxForm{
                    propertyDateText.text: "Nome"
                    propertyDateField.text: "Joana Ovilia"
                }
            }
            Item{
                id: surNameTextTextForm
                width: parent.width
                height: parent.height -
                        (2 * Constants.SIZE_TEXT_LABEL) -
                        (2 * Constants.SIZE_TEXT_V_SPACE) -
                        (2 * Constants.SIZE_TEXT_FIELD) -
                        Constants.SIZE_ROW_V_SPACE
                anchors.top :givenNameTextForm.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    propertyDateText.text: "Apelido"
                    propertyDateField.text: "Ribeiro Martins Sousa Costa Mender Cavaco Soares Meireles Dinis Mendes"
                    propertyRectField.height: parent.height
                    propertyDateField.wrapMode: Text.WordWrap
                }
            }
        }

        Item{
            id: rectNamePhotoRight
            width: Constants.WIDTH_PHOTO_IMAGE
            height: parent.height - Constants.SIZE_TEXT_LABEL - Constants.SIZE_TEXT_V_SPACE
            anchors.left: rectNamePhotoLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.bottom: parent.bottom

            RectangularGlow {
                id: effectPhoto
                anchors.fill: rectPhotoField
                glowRadius: Constants.FORM_GROW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
            }
            Rectangle {
                id: rectPhotoField
                width: parent.width
                height: parent.height
                color: "white"
                Image {
                    anchors.fill: parent
                    antialiasing: true
                    source: "photo.png"
                }
            }
        }
    }

    Item {
        id: rowSex
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowNamePhoto.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectSex
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.20
            Components.LabelTextBoxForm{
                propertyDateText.text: "Sexo"
                propertyDateField.text: "F"
            }
        }
        Item{
            id: rectHeight
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.20
            anchors.left: rectSex.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Altura"
                propertyDateField.text: "1.77"
            }
        }
        Item{
            id: rectNacionality
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.20
            anchors.left: rectHeight.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Nacionalidade"
                propertyDateField.text: "PRT"
            }
        }
        Item{
            id: rectDateOfBirth
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.40
            anchors.left: rectNacionality.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Data de Nascimento"
                propertyDateField.text: "25 12 1980"
            }
        }
    }

    Item {
        id: rowDocumentMum
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowSex.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectDocumentNum
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            Components.LabelTextBoxForm{
                propertyDateText.text: "N.º Documento"
                propertyDateField.text: "1223456456"
            }
        }
        Item{
            id: rectExpiryDate
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            anchors.left: rectDocumentNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                propertyDateText.text: "Data de validade"
                propertyDateField.text: "31 12 2019"
            }
        }
    }

    Item {
        id: rowCountry
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowDocumentMum.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectCountry
            width: parent.width
            Components.LabelTextBoxForm{
                propertyDateText.text: "País"
                propertyDateField.text: "PRT"
            }
        }
    }

    Item {
        id: rowParents
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + 3 * Constants.SIZE_TEXT_V_SPACE
                + 4 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowCountry.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectParentsFather
            width: parent.width
            height: Constants.SIZE_TEXT_LABEL
                    + Constants.SIZE_TEXT_V_SPACE
                    + 2 * Constants.SIZE_TEXT_FIELD
            Components.LabelTextBoxForm{
                propertyDateText.text: "Filiação"
                propertyDateField.text: "Antonio Maria Costa"
            }
        }
        Item{
            id: rectParentsMother
            width: parent.width
            height: + 2 * Constants.SIZE_TEXT_V_SPACE
                    + 2 * Constants.SIZE_TEXT_FIELD
            anchors.top :rectParentsFather.bottom
            Components.LabelTextBoxForm{
                propertyDateText.height: 0
                propertyRectField.anchors.topMargin: 2 * Constants.SIZE_TEXT_V_SPACE
                propertyDateField.text: "Olivia Sousa Costa"
            }
        }
    }

    Item {
        id: rowNotes
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowParents.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectNotes
            width: parent.width
            height: Constants.SIZE_TEXT_LABEL
                    + Constants.SIZE_TEXT_V_SPACE
                    + 2 * Constants.SIZE_TEXT_FIELD
            Components.LabelTextBoxForm{
                propertyDateText.text: "Indicações Eventuais"
                propertyDateField.text: ""
            }
        }
    }
}
