import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

import "../../scripts/Constants.js" as Constants
import "../../components" as Components


Item {
    anchors.fill: parent

    property alias propertyBusyIndicator : busyIndication

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
    property alias propertyPhoto: photoImage

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_CARD_IDENTIFY_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_CARD_IDENTIFY_ROW_TOP_INC_RELATIVE
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
                    propertyDateText.text: qsTranslate("GAPI","STR_GIVEN_NAME")
                    propertyRectField.height: parent.height
                    propertyDateField.wrapMode: Text.WordWrap
                }
                KeyNavigation.tab: textBoxSurName
            }
            Item{
                id: surNameTextTextForm
                width: parent.width
                height: rectNamePhotoLeft.height - givenNameTextForm.height
                    - 2 * Constants.SIZE_ROW_V_SPACE
                    - Constants.SIZE_TEXT_LABEL - Constants.SIZE_TEXT_V_SPACE
                anchors.top :givenNameTextForm.bottom
                anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    id: textBoxSurName
                    propertyDateText.text: qsTranslate("GAPI","STR_SURNAME")
                    propertyRectField.height: parent.height
                    propertyDateField.wrapMode: Text.WordWrap
                }
                KeyNavigation.tab: photoImage
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
                border.width: photoImage.focus ? 2 : 0
                border.color: photoImage.focus ? Constants.COLOR_GREY_BUTTON_BACKGROUND
                                                   : Constants.COLOR_MAIN_SOFT_GRAY
                Image {
                    id: photoImage
                    width: parent.width - 2
                    height: parent.height - 2
                    x: 1
                    y: 1
                    fillMode: Image.PreserveAspectFit
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    antialiasing: true
                    cache: false
                    Accessible.role: Accessible.Graphic
                    Accessible.name: qsTranslate("GAPI","STR_PHOTO")
                    KeyNavigation.tab: textBoxSex
                }
            }
        }
    }

    Item {
        id: rowSex
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowNamePhoto.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectSex
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.20
            height: parent.height
            Components.LabelTextBoxForm{
                id: textBoxSex
                propertyDateText.text: qsTranslate("GAPI","STR_GENDER")
            }
            KeyNavigation.tab: textBoxHeight
        }
        Item{
            id: rectHeight
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.20
            height: parent.height
            anchors.left: rectSex.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxHeight
                propertyDateText.text: qsTranslate("GAPI","STR_HEIGHT")
            }
            KeyNavigation.tab: textBoxNacionality
        }
        Item{
            id: rectNacionality
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.20
            height: parent.height
            anchors.left: rectHeight.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxNacionality
                propertyDateText.text: qsTranslate("GAPI","STR_NATIONALITY")
            }
            KeyNavigation.tab: textBoxDateOfBirth
        }
        Item{
            id: rectDateOfBirth
            width: (parent.width - 3 * Constants.SIZE_ROW_H_SPACE ) * 0.40
            height: parent.height
            anchors.left: rectNacionality.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxDateOfBirth
                propertyDateText.text: qsTranslate("GAPI","STR_DATE_OF_BIRTH")
            }
            KeyNavigation.tab: textBoxDocumentNum
        }
    }

    Item {
        id: rowDocumentMum
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowSex.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectDocumentNum
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            height: parent.height
            Components.LabelTextBoxForm{
                id: textBoxDocumentNum
                propertyDateText.text: qsTranslate("GAPI","STR_DOCUMENT_NUMBER")
            }
            KeyNavigation.tab: textBoxExpirydate
        }
        Item{
            id: rectExpiryDate
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            height: parent.height
            anchors.left: rectDocumentNum.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.LabelTextBoxForm{
                id: textBoxExpirydate
                propertyDateText.text: qsTranslate("GAPI","STR_VALIDITY_DATE")
            }
            KeyNavigation.tab: textBoxCountry
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
            height: parent.height
            Components.LabelTextBoxForm{
                id: textBoxCountry
                propertyDateText.text: qsTranslate("GAPI","STR_COUNTRY")
            }
            KeyNavigation.tab: textBoxParentsFather
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
            height: Constants.HEIGHT_TEXT_BOX
            Components.LabelTextBoxForm{
                id: textBoxParentsFather
                propertyDateText.text: qsTranslate("GAPI","STR_AFFILIATION")
            }
            KeyNavigation.tab: textBoxParentsMother
        }
        Item{
            id: rectParentsMother
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top :rectParentsFather.bottom
            Components.LabelTextBoxForm{
                id: textBoxParentsMother
                propertyDateText.text: qsTranslate("GAPI","STR_AFFILIATION")
                propertyDateText.visible: false
                propertyDateText.height: 0
                propertyRectField.anchors.topMargin: 2 * Constants.SIZE_TEXT_V_SPACE
            }
            KeyNavigation.tab: textBoxNotes
        }
    }

    Item {
        id: rowNotes
        width: parent.width
        height: Constants.HEIGHT_TEXT_BOX
        anchors.top: rowParents.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectNotes
            width: parent.width
            height: parent.height
            Components.LabelTextBoxForm{
                id: textBoxNotes
                propertyDateText.text: qsTranslate("GAPI","STR_NOTES")
            }
            KeyNavigation.tab: textBoxName
        }
    }
}
