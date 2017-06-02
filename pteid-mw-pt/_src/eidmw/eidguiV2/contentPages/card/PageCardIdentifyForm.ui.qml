import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import "../../scripts/Constants.js" as Constants

Item {
    id: main
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        RowLayout {
            Layout.fillWidth: true
            spacing: Constants.SIZE_ROW_SPACE
            Item{
                width: main.width * Constants.SIZE_FORM_RECT_NAME_RELATIVE
                height: main.height * Constants.SIZE_FORM_RECT_TOP_V_RELATIVE
                Layout.fillWidth: true
            }
            Item{
                width: main.width * Constants.SIZE_FORM_RECT_PHOTO_RELATIVE
                height: main.height * Constants.SIZE_FORM_RECT_TOP_V_RELATIVE
                Layout.fillWidth: true
                Image {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    source: "../../images/logo_cartao_cidadao.png"
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: Constants.SIZE_ROW_SPACE
            Item{
                id: rectLeftName
                width: main.width * Constants.SIZE_FORM_RECT_NAME_RELATIVE
                height: main.height * Constants.SIZE_FORM_RECT_NAME_V_RELATIVE
                Layout.fillWidth: true
                Text {
                    id: givenNameText
                    text: "Nome"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                    height: 1.2 * Constants.SIZE_TEXT_LABEL
                }
                RectangularGlow {
                    id: effect
                    anchors.fill: rectGivenNameTextField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectGivenNameTextField
                    width: rectLeftName.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :givenNameText.bottom
                    Text {
                        id: givenNameTextField
                        width: rectLeftName.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Joana Ovilia"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
                Item{
                    id: rectSpace
                    width: Constants.SIZE_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    anchors.top :rectGivenNameTextField.bottom
                }
                Text {
                    id: surnameText
                    text: "Apelido"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                    anchors.top :rectSpace.bottom
                    height: 1.2 * Constants.SIZE_TEXT_LABEL
                }
                RectangularGlow {
                    id: effectSurname
                    anchors.fill: rectSurnameTextField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectSurnameTextField
                    width: rectLeftName.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 4 * Constants.SIZE_TEXT_FIELD
                    anchors.top :surnameText.bottom
                    Text {
                        id: givenSurnameField
                        width: rectLeftName.width
                        text: "Ribeiro Martins Sousa Costa Mender Cavaco Soares Meireles Dinis Mendes"
                        wrapMode: Text.WordWrap
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
            Item{
                id: rectRightName
                width: main.width * Constants.SIZE_FORM_RECT_PHOTO_RELATIVE
                height: main.height * Constants.SIZE_FORM_RECT_NAME_V_RELATIVE
                Layout.fillWidth: true
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
                    y: 1.2 * Constants.SIZE_TEXT_LABEL
                    anchors.top: rectRightName.Top
                    width: parent.width
                    height: 103
                    color: "white"
                    Layout.fillWidth: true
                    Image {
                        id: photo
                        y: 1.2 * Constants.SIZE_TEXT_LABEL
                        anchors.fill: parent
                        scale: 1
                        fillMode: Image.PreserveAspectFit
                        source: "photo.png"
                    }
                }
            }
        }

        RowLayout {
            spacing: Constants.SIZE_ROW_SPACE
            Layout.fillWidth: true
            Item{
                width: main.width * 0.10
                height: main.height * 0.08
                Layout.fillWidth: true
                Text {
                    id: sexText
                    text: "Sexo"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                }
                RectangularGlow {
                    id: effectSex
                    anchors.fill: rectSexField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectSexField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :sexText.bottom
                    Text {
                        id: sexField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "F"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
            Item{
                width: main.width * 0.10
                height: main.height * 0.08
                Layout.fillWidth: true
                Text {
                    id: heightText
                    text: "Altura"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                }
                RectangularGlow {
                    id: effectHeight
                    anchors.fill: rectHeightField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectHeightField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :heightText.bottom
                    Text {
                        id: heightField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "1.77"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
            Item{
                width: main.width * 0.20
                height: main.height * 0.08
                Layout.fillWidth: true
                Text {
                    id: nacionalityText
                    text: "Nacionalidade"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                }
                RectangularGlow {
                    id: effectNacionality
                    anchors.fill: rectNacionalityField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectNacionalityField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :nacionalityText.bottom
                    Text {
                        id: nacionalityField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "PRT"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
            Item{
                width: main.width * 0.35
                height: main.height * 0.08
                Layout.fillWidth: true
                Text {
                    id: dateOfBirthText
                    text: "Data de Nascimento"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                }
                RectangularGlow {
                    id: effectDateOfBirth
                    anchors.fill: rectDateOfBirthField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectDateOfBirthField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :dateOfBirthText.bottom
                    Text {
                        id: dateOfBirthField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "25 12 1980"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
        }

        RowLayout {
            spacing: Constants.SIZE_ROW_SPACE
            Item{
                height: main.height * 0.08
                Layout.fillWidth: true
                Text {
                    id: documentNumText
                    text: "Nº Documento"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                }
                RectangularGlow {
                    id: effectDocumentNum
                    anchors.fill: rectDocumentNumField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectDocumentNumField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :documentNumText.bottom
                    Text {
                        id: documentNumField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "1223456456"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
            Rectangle{
                height: main.height * 0.08
                Layout.fillWidth: true
                Text {
                    id: expiryDateText
                    text: "Altura"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                }
                RectangularGlow {
                    id: effectExpiryDate
                    anchors.fill: rectExpiryDateField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectExpiryDateField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :expiryDateText.bottom
                    Text {
                        id: expiryDateField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "1.77"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
        }

        RowLayout {
            spacing: Constants.SIZE_ROW_SPACE
            Item{
                height: main.height * 0.08
                Layout.fillWidth: true
                Text {
                    id:  countryText
                    text: "País"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                }
                RectangularGlow {
                    id: effectCountry
                    anchors.fill: rectCountryField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectCountryField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :countryText.bottom
                    Text {
                        id: countryField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "PRT"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
        }

        RowLayout {
            spacing: Constants.SIZE_ROW_SPACE
            Item{
                height: main.height * 0.08
                Layout.fillWidth: true
                Text {
                    id:  parentsText
                    text: "Filiação"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase
                }
                RectangularGlow {
                    id: effectParents
                    anchors.fill: rectParentsField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectParentsField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :parentsText.bottom
                    Text {
                        id: parentsField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Antonio Maria Costa"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
                //                Item{
                //                    id: rectSpaceParents
                //                    width: 5
                //                    height: 5
                //                    anchors.top :rectParentsField.bottom
                //                }
                //                RectangularGlow {
                //                    id: effectParents2
                //                    anchors.fill: rectParents2Field
                //                    glowRadius: Constants.FORM_GROW_RADIUS
                //                    spread: Constants.FORM_GLOW_SPREAD
                //                    color: Constants.COLOR_FORM_GLOW
                //                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                //                }
                //                Rectangle {
                //                    id: rectParents2Field
                //                    width: parent.width
                //                    color: "white"
                //                    Layout.fillWidth: true
                //                    height: 2 * Constants.SIZE_TEXT_FIELD
                //                    anchors.top :rectSpaceParents.bottom
                //                    Text {
                //                        id: parents2Field
                //                        width: parent.width
                //                        anchors.verticalCenter: parent.verticalCenter
                //                        text: "Olivia Maria Costa"
                //                        font.capitalization: Font.AllUppercase
                //                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                //                        font.family: lato.name
                //                    }
                //                }
            }
        }
        RowLayout {
            spacing: Constants.SIZE_ROW_SPACE
            Item{
                height: main.height * 0.08
                Layout.fillWidth: true

                Text {
                    id:  notesText
                    text: "Indicações Eventuais"
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    font.capitalization: Font.AllUppercase

                }
                RectangularGlow {
                    id: effectNotes
                    anchors.fill: rectNotesField
                    glowRadius: Constants.FORM_GROW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                }
                Rectangle {
                    id: rectNotesField
                    width: parent.width
                    color: "white"
                    Layout.fillWidth: true
                    height: 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top :notesText.bottom
                    Text {
                        id: notesField
                        width: parent.width
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Notes"
                        font.capitalization: Font.AllUppercase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                    }
                }
            }
        }
    }
}
