import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import "../../scripts/Constants.js" as Constants
import "../../components" as Components


Item {
    anchors.fill: parent

    property alias propertyTextBoxName: textBoxName
    property alias propertyTextBoxSurName: textBoxSurName
    property alias propertyTextBoxSex: textBoxSex
    property alias propertyTextBoxHeight: textBoxHeight
    property alias propertyTextBoxNacionality: textBoxNacionality
    property alias propertyTextBoxDateOfBirth: textBoxDateOfBirth
    property alias propertyTextBoxDocumentNum: textBoxDocumentNum
    property alias propertyTextBoxExpirydate: textBoxExpirydate
    property alias propertyTextBoxCountry: textBoxCountry
    property alias propertyTextBoxParentsFather: textBoxParentsFather
    property alias propertyTextBoxParentsMother: textBoxParentsMother
    property alias propertyTextBoxNotes: textBoxNotes

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_CARD_IDENTIFY_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_CARD_IDENTIFY_ROW_TOP_INC_RELATIVE
        Components.CardRowTop{}
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
                    id: textBoxName
                    propertyDateText.text: "Nome"
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
                    id: textBoxSurName
                    propertyDateText.text: "Apelido"
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

            DropShadow {
                anchors.fill: rectPhotoField
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectPhotoField
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectPhotoField
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectPhotoField
                width: parent.width
                height: parent.height
                color: "white"
                Image {
                    width: parent.width - 1
                    height: parent.height - 1
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    antialiasing: true
                    source: "../../images/dummy/photo.png"
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
                id: textBoxSex
                propertyDateText.text: "Sexo"
            }
        }
        Item{
            id: rectHeight
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.20
            anchors.left: rectSex.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxHeight
                propertyDateText.text: "Altura"
            }
        }
        Item{
            id: rectNacionality
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.20
            anchors.left: rectHeight.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxNacionality
                propertyDateText.text: "Nacionalidade"
            }
        }
        Item{
            id: rectDateOfBirth
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.40
            anchors.left: rectNacionality.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxDateOfBirth
                propertyDateText.text: "Data de Nascimento"
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
                id: textBoxDocumentNum
                propertyDateText.text: "N.º Documento"
            }
        }
        Item{
            id: rectExpiryDate
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            anchors.left: rectDocumentNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxExpirydate
                propertyDateText.text: "Data de validade"
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
                id: textBoxCountry
                propertyDateText.text: "País"
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
                id: textBoxParentsFather
                propertyDateText.text: "Filiação"
            }
        }
        Item{
            id: rectParentsMother
            width: parent.width
            height: + 2 * Constants.SIZE_TEXT_V_SPACE
                    + 2 * Constants.SIZE_TEXT_FIELD
            anchors.top :rectParentsFather.bottom
            Components.LabelTextBoxForm{
                id: textBoxParentsMother
                propertyDateText.height: 0
                propertyRectField.anchors.topMargin: 2 * Constants.SIZE_TEXT_V_SPACE
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
                id: textBoxNotes
                propertyDateText.text: "Indicações Eventuais"
            }
        }
    }
}
