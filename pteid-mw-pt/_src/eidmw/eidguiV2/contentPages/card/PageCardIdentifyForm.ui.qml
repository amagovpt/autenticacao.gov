import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import "../../scripts/Constants.js" as Constants

Item {
    anchors.fill: parent
    Layout.alignment: Qt.AlignHCenter
    ColumnLayout {
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Image {
            id: photo
            width: parent.width * 0.1
            Layout.fillWidth: false
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            fillMode: Image.PreserveAspectFit
            source: "photo.png"
        }
        Text {
            id: givenNameText
            text: "Nome[s]"
            font.pointSize: Constants.SIZE_TEXT_LABEL
            font.family: lato.name
            color: Constants.COLOR_TEXT_LABEL
        }

        TextField {
            id: givenNameTextField
            width: parent.width
            text: "Joana Ovilia"
            readOnly: true
            font.capitalization: Font.AllUppercase
            Layout.fillWidth: true
            font.pointSize: Constants.SIZE_TEXT_FIELD
            font.family: lato.name
            style: TextFieldStyle {
                textColor: Constants.COLOR_TEXT_FIELD_TEXT
                background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
            }
        }
        Text {
            id: surnameText
            text: "Apelido[s]"
            font.pointSize: Constants.SIZE_TEXT_LABEL
            font.family: lato.name
            color: Constants.COLOR_TEXT_LABEL
        }

        TextField {
            id: surnameTextField
            width: parent.width
            text: "Martins Azevedo Costa"
            readOnly: true
            font.capitalization: Font.AllUppercase
            Layout.fillWidth: true
            font.pointSize: Constants.SIZE_TEXT_FIELD
            font.family: lato.name
            style: TextFieldStyle {
                textColor: Constants.COLOR_TEXT_FIELD_TEXT
                background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
            }
        }

        RowLayout {
            ColumnLayout {
                Text {
                    id: sexText
                    text: "Sexo"
                    font.pointSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                }
                TextField {
                    id: sexTextField
                    width: parent.width
                    text: "F"
                    readOnly: true
                    font.capitalization: Font.AllUppercase
                    Layout.fillWidth: true
                    font.pointSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    style: TextFieldStyle {
                        textColor: Constants.COLOR_TEXT_FIELD_TEXT
                        background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                    }
                }
            }

            ColumnLayout {
                Text {
                    id: heightText
                    text: "Altura"
                    font.pointSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                }
                TextField {
                    id: heightTextField
                    width: parent.width
                    text: "1,93"
                    readOnly: true
                    font.capitalization: Font.AllUppercase
                    Layout.fillWidth: true
                    font.pointSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    style: TextFieldStyle {
                        textColor: Constants.COLOR_TEXT_FIELD_TEXT
                        background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                    }
                }
            }

            ColumnLayout {
                Text {
                    id: nacionalityText
                    text: "Nacionalidade"
                    font.pointSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                }
                TextField {
                    id: nacionalityTextField
                    width: parent.width
                    text: "PRT"
                    readOnly: true
                    font.capitalization: Font.AllUppercase
                    Layout.fillWidth: true
                    font.pointSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    style: TextFieldStyle {
                        textColor: Constants.COLOR_TEXT_FIELD_TEXT
                        background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                    }
                }
            }

            ColumnLayout {
                Text {
                    id: dateOfBirthText
                    text: "Data de Nascimento"
                    font.pointSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                }
                TextField {
                    id: dateOfBirthTextField
                    width: parent.width
                    text: "25 12 1980"
                    readOnly: true
                    font.capitalization: Font.AllUppercase
                    Layout.fillWidth: true
                    font.pointSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    style: TextFieldStyle {
                        textColor: Constants.COLOR_TEXT_FIELD_TEXT
                        background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                    }
                }
            }
        }

        RowLayout {
            ColumnLayout {
                Text {
                    id: documentNumText
                    text: "Nº Documento"
                    font.pointSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                }
                TextField {
                    id: documentNumTextField
                    width: parent.width
                    text: "1223456456"
                    readOnly: true
                    font.capitalization: Font.AllUppercase
                    Layout.fillWidth: true
                    font.pointSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    style: TextFieldStyle {
                        textColor: Constants.COLOR_TEXT_FIELD_TEXT
                        background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                    }
                }
            }

            ColumnLayout {
                Text {
                    id: expiryDateText
                    text: "Data de Validade"
                    font.pointSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                }

                TextField {
                    id: expiryDateTextField
                    width: parent.width
                    text: "12 10 2017"
                    readOnly: true
                    font.capitalization: Font.AllUppercase
                    Layout.fillWidth: true
                    font.pointSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    style: TextFieldStyle {
                        textColor: Constants.COLOR_TEXT_FIELD_TEXT
                        background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                    }
                }
            }
        }

        ColumnLayout {
            Text {
                id: countryText
                text: "País"
                font.pointSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
            }

            TextField {
                id: countryTextield
                width: parent.width
                text: "PRT"
                readOnly: true
                font.capitalization: Font.AllUppercase
                Layout.fillWidth: true
                font.pointSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                style: TextFieldStyle {
                    textColor: Constants.COLOR_TEXT_FIELD_TEXT
                    background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                }
            }
        }

        ColumnLayout {
            Text {
                id: parentsText
                text: "Filiação"
                font.pointSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
            }

            TextField {
                id: parentsFatherTextField
                width: parent.width
                text: "Antonio Maria Costa"
                readOnly: true
                font.capitalization: Font.AllUppercase
                Layout.fillWidth: true
                font.pointSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                style: TextFieldStyle {
                    textColor: Constants.COLOR_TEXT_FIELD_TEXT
                    background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                }
            }
            TextField {
                id: parentsMotherTextField
                width: parent.width
                text: "Olivia Maria Costa"
                readOnly: true
                font.capitalization: Font.AllUppercase
                Layout.fillWidth: true
                font.pointSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                style: TextFieldStyle {
                    textColor: Constants.COLOR_TEXT_FIELD_TEXT
                    background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                }
            }
        }

        ColumnLayout {
            Text {
                id: notesText
                text: "Indicações Eventuais"
                font.pointSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
            }

            TextField {
                id: notesTextField
                width: parent.width
                text: "Notes"
                readOnly: true
                font.capitalization: Font.AllUppercase
                Layout.fillWidth: true
                font.pointSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                style: TextFieldStyle {
                    textColor: Constants.COLOR_TEXT_FIELD_TEXT
                    background: Rectangle { color: Constants.COLOR_TEXT_FIELD_BACKGROUD }
                }
            }
        }
    }
}
