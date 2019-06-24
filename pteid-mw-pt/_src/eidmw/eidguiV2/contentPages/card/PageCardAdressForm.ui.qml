import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias propertyItemForeignCitizen: itemForeignCitizen
    property alias propertyItemNationalCitizen: itemNationalCitizen

    property alias propertyForeignCountry: textBoxForeignCountry
    property alias propertyForeignAddress: textBoxForeignAddress
    property alias propertyForeignCity: textBoxForeignCity
    property alias propertyForeignRegion: textBoxForeignRegion
    property alias propertyForeignLocality: textBoxForeignLocality
    property alias propertyForeignPostalCode: textBoxForeignPostalCode

    property alias propertyDistrict: textBoxDistrict
    property alias propertyMunicipality: textBoxMunicipality
    property alias propertyParish: textBoxParish
    property alias propertyStreetType: textBoxStreetType
    property alias propertyStreetName: textBoxStreetName
    property alias propertyDoorNo: textBoxDoorNo
    property alias propertyFloor: textBoxFloor
    property alias propertySide: textBoxSide
    property alias propertyPlace: textBoxPlace
    property alias propertyLocality: textBoxLocality
    property alias propertyZip4: textBoxZip4
    property alias propertyZip3: textBoxZip3
    property alias propertyPostalLocality: textBoxPostalLocality

    property alias propertyBusyIndicator: busyIndicator

    property alias propertyButtonConfirmOfAddress : buttonConfirmOfAddress

    anchors.fill: parent
    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_CARD_ADRESS_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_CARD_ADRESS_ROW_TOP_INC_RELATIVE
        Components.CardRowTop {
        }
    }

    BusyIndicator {
        id: busyIndicator
        running: false
        anchors.centerIn: parent
        // BusyIndicator should be on top of all other content
        z: 1
    }
    Item {
        id: itemForeignCitizen
        visible: false
        height: rowNacionalDistrict.height + rowMunicipality.height + rowCivilParish.height
                + rowAbbreviationStreetType.height + rowDoorNo.height + rowPlace.height + rowZip4.height
                + 6 * Constants.SIZE_ROW_V_SPACE
        width: parent.width
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        Item {
            id: rowForeignCountry
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            Item {
                id: rectForeignCountry
                width: parent.width
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxForeignCountry
                    propertyDateText.text: qsTranslate("GAPI","STR_FOREIGN_COUNTRY")
                    propertyDateField.text: ""
                }
            }
        }
        Item {
            id: rowForeignAddress
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowForeignCountry.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectForeignAddress
                width: parent.width
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxForeignAddress
                    propertyDateText.text: qsTranslate("GAPI","STR_FOREIGN_ADDRESS")
                    propertyDateField.text: ""
                }
            }
        }
        Item {
            id: rowForeignCity
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowForeignAddress.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectForeignCity
                width: parent.width
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxForeignCity
                    propertyDateText.text: qsTranslate("GAPI","STR_FOREIGN_CITY")
                    propertyDateField.text: ""
                }
            }
        }
        Item {
            id: rowForeignRegion
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowForeignCity.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectForeignRegion
                width: parent.width
                height: parent.height
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                Components.LabelTextBoxForm {
                    id: textBoxForeignRegion
                    propertyDateText.text: qsTranslate("GAPI","STR_FOREIGN_REGION")
                    propertyDateField.text: ""
                }
            }
        }

        Item {
            id: rowForeignLocality
            width: parent.width
           height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowForeignRegion.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectForeignLocality
                width: parent.width
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxForeignLocality
                    propertyDateText.text: qsTranslate("GAPI","STR_FOREIGN_LOCALITY")
                    propertyDateField.text: ""
                }
            }
        }

        Item {
            id: rowForeignPostalCode
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowForeignLocality.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectForeignPostalCode
                width: parent.width
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxForeignPostalCode
                    propertyDateText.text: qsTranslate("GAPI","STR_FOREIGN_POSTAL_CODE")
                    propertyDateField.text: ""
                }
            }
        }
    }
    Item {
        id: itemNationalCitizen
        visible: true
        height: rowNacionalDistrict.height + rowMunicipality.height + rowCivilParish.height
                + rowAbbreviationStreetType.height + rowDoorNo.height + rowPlace.height + rowZip4.height
                + 6 * Constants.SIZE_ROW_V_SPACE
        width: parent.width
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        Item {
            id: rowNacionalDistrict
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX

            Item {
                id: rectNacionalDistrict
                width: parent.width
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxDistrict
                    propertyDateText.text: qsTranslate("GAPI","STR_DISTRICT")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxMunicipality
            }
        }
        Item {
            id: rowMunicipality
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowNacionalDistrict.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectMunicipality
                width: parent.width
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxMunicipality
                    propertyDateText.text: qsTranslate("GAPI","STR_MUNICIPALITY")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxParish
            }
        }
        Item {
            id: rowCivilParish
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowMunicipality.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectCivilParish
                width: parent.width
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxParish
                    propertyDateText.text: qsTranslate("GAPI","STR_CIVIL_PARISH")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxStreetType
            }
        }
        Item {
            id: rowAbbreviationStreetType
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowCivilParish.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectStreetType
                width: (parent.width - Constants.SIZE_ROW_H_SPACE) * 0.15
                height: parent.height
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                Components.LabelTextBoxForm {
                    id: textBoxStreetType
                    propertyDateText.text: qsTranslate("GAPI","STR_STREET_TYPE")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxStreetName
            }
            Item {
                id: rectStreetName
                width: (parent.width - Constants.SIZE_ROW_H_SPACE) * 0.85
                height: parent.height
                anchors.left: rectStreetType.right
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                Components.LabelTextBoxForm {
                    id: textBoxStreetName
                    propertyDateText.text: qsTranslate("GAPI","STR_STREET_NAME")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxDoorNo
            }
        }

        Item {
            id: rowDoorNo
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowAbbreviationStreetType.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectDoorNo
                width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE) * 0.33
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxDoorNo
                    propertyDateText.text: qsTranslate("GAPI","STR_HOUSE_BUILDING_NUM")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxFloor
            }
            Item {
                id: rectFloor
                width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE) * 0.33
                height: parent.height
                anchors.left: rectDoorNo.right
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                Components.LabelTextBoxForm {
                    id: textBoxFloor
                    propertyDateText.text: qsTranslate("GAPI","STR_FLOOR")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxSide
            }
            Item {
                id: rectSide
                width: (parent.width - 2 * Constants.SIZE_ROW_H_SPACE) * 0.34
                height: parent.height
                anchors.left: rectFloor.right
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                Components.LabelTextBoxForm {
                    id: textBoxSide
                    propertyDateText.text: qsTranslate("GAPI","STR_SIDE")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxPlace
            }
        }
        Item {
            id: rowPlace
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.top: rowDoorNo.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item {
                id: rectPlace
                width: (parent.width - Constants.SIZE_ROW_H_SPACE) * 0.50
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxPlace
                    propertyDateText.text: qsTranslate("GAPI","STR_PLACE")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxLocality
            }
            Item {
                id: rectLocality
                width: (parent.width - Constants.SIZE_ROW_H_SPACE) * 0.50
                height: parent.height
                anchors.left: rectPlace.right
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                Components.LabelTextBoxForm {
                    id: textBoxLocality
                    propertyDateText.text: qsTranslate("GAPI","STR_LOCALITY")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxZip4
            }
        }
        Item {
            id: rowZip4
            width: parent.width
            height: Constants.HEIGHT_TEXT_BOX
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            anchors.top: rowPlace.bottom

            Item {
                id: rectZip4
                width: 50
                height: parent.height
                Components.LabelTextBoxForm {
                    id: textBoxZip4
                    propertyDateText.text: qsTranslate("GAPI","STR_ZIP_CODE")
                    propertyDateField.text: ""
                    text: propertyDateField.text + textBoxZip3.propertyDateField.text
                }
                KeyNavigation.tab: textBoxPostalLocality
            }
            Item {
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
            Item {
                id: rectZip3
                width: 40
                height: parent.height
                anchors.left: textIfen.right
                anchors.leftMargin: 5
                Components.LabelTextBoxForm {
                    id: textBoxZip3
                    propertyDateText.text: ""
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: textBoxPostalLocality
            }
            Item {
                id: rectPostalLocality
                width: parent.width - 2 * Constants.SIZE_ROW_H_SPACE
                       - rectZip4.width - rectZip3.width - textIfen.width + 10
                height: parent.height
                anchors.left: rectZip3.right
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                Components.LabelTextBoxForm {
                    id: textBoxPostalLocality
                    propertyDateText.text: qsTranslate("GAPI","STR_POSTAL_LOCALITY")
                    propertyDateField.text: ""
                }
                KeyNavigation.tab: rectConfirmationOfAddressText
            }
        }
    }
    Item {
        id: rowConfirmationOfAddress
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL + Constants.SIZE_TEXT_V_SPACE + 3
                * Constants.SIZE_TEXT_FIELD
        anchors.top: itemNationalCitizen.bottom
        anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE

        Item {
            id: rectConfirmationOfAddressText
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE) * 0.50
            Text {
                id: dateField
                y: Constants.SIZE_TEXT_FIELD_V_SPACE
                text: qsTranslate("PageCardAdress","STR_ADDRESS_CHANGE_CONFIRMATION_TEXT")
                width: parent.width
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                font.capitalization: Font.MixedCase
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.bold: parent.focus ? true : false
                color: Constants.COLOR_TEXT_LABEL
                wrapMode: Text.WordWrap
            }
            Accessible.role: Accessible.Row
            Accessible.name: dateField.text
            KeyNavigation.tab: buttonConfirmOfAddress
        }
        Item {
            id: rectConfirmationOfAddressButton
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE) * 0.50
            height: parent.height
            anchors.left: rectConfirmationOfAddressText.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Button {
                id: buttonConfirmOfAddress
                text: qsTranslate("PageCardAdress","STR_ADDRESS_CHANGE_BUTTON")
                width: 250
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.right: parent.right
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                enabled: false
                focus: true
                KeyNavigation.tab: textBoxDistrict
            }
        }
    }
}
