import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias buttonTest: buttonTest
    property alias buttonModify: buttonModify
    anchors.fill: parent

    TabBar {
        id: bar
        x: 0
        y: 0
        width: parent.width
        currentIndex: 0
        TabButton {
            text: "Pin de Autenticação"
            rightPadding: 2
            leftPadding: 2
        }
        TabButton {
            text: "Pin de Assinatura"
            rightPadding: 2
            leftPadding: 2
        }
        TabButton {
            text: "Pin de Morada"
            rightPadding: 2
            leftPadding: 2
        }
    }

    StackLayout {
        width: parent.width
        currentIndex: bar.currentIndex
        Item {
            id: tabAuthentication
            Item {
                id: rowAuth
                y: bar.height + 3 * Constants.SIZE_ROW_V_SPACE
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD

                Item{
                    id: rectAuthLeft
                    width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
                    Components.LabelTextBoxForm{
                        propertyDateText.text: "Tipo de Código PIN"
                        propertyDateField.text: "PIN Autenticação"
                    }
                }
                Item{
                    id: rectAuthRight
                    width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
                    anchors.left: rectAuthLeft.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Components.LabelTextBoxForm{
                        propertyDateText.text: "Identificação do código PIN"
                        propertyDateField.text: "1"
                    }
                }
            }

            Item {
                id: rowStateAuth
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rowAuth.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Item {
                    id: rectStateAuth
                    width: parent.width
                    Components.LabelTextBoxForm {
                        propertyDateText.text: "Estado do Código PIN"
                        propertyDateField.text: "Restão 3 tentativas"
                    }
                }
            }
            Item {
                id: rowBottomAuth
                width: parent.width
                height: Constants.SIZE_V_BOTTOM_COMPONENT
                anchors.top: rowStateAuth.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            }
        }
        Item {
            id: tabSign

            Item {
                id: rowSign
                y: bar.height + 3 * Constants.SIZE_ROW_V_SPACE
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD

                Item{
                    id: rectSignLeft
                    width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
                    Components.LabelTextBoxForm{
                        propertyDateText.text: "Tipo de Código PIN"
                        propertyDateField.text: "PIN Assinatura"
                    }
                }
                Item{
                    id: rectSignRight
                    width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
                    anchors.left: rectSignLeft.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Components.LabelTextBoxForm{
                        propertyDateText.text: "Identificação do código PIN"
                        propertyDateField.text: "2"
                    }
                }
            }

            Item {
                id: rowStateSign
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rowSign.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Item {
                    id: rectStateSign
                    width: parent.width
                    Components.LabelTextBoxForm {
                        propertyDateText.text: "Estado do Código PIN"
                        propertyDateField.text: "Restão 3 tentativas"
                    }
                }
            }

            Item {
                id: rowBottomSign
                width: parent.width
                height: Constants.SIZE_V_BOTTOM_COMPONENT
                anchors.top: rowStateSign.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            }
        }
        Item {
            id: tabAddress

            Item {
                id: rowAddress
                y: bar.height + 3 * Constants.SIZE_ROW_V_SPACE
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD

                Item{
                    id: rectAddressLeft
                    width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
                    Components.LabelTextBoxForm{
                        propertyDateText.text: "Tipo de Código PIN"
                        propertyDateField.text: "PIN Morada"
                    }
                }
                Item{
                    id: rectAddressRight
                    width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
                    anchors.left: rectAddressLeft.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                    Components.LabelTextBoxForm{
                        propertyDateText.text: "Identificação do código PIN"
                        propertyDateField.text: "3"
                    }
                }
            }

            Item {
                id: rowStateAddress
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rowAddress.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Item {
                    id: rectStateAddress
                    width: parent.width
                    Components.LabelTextBoxForm {
                        propertyDateText.text: "Estado do Código PIN"
                        propertyDateField.text: "Restão 3 tentativas"
                    }
                }
            }

            Item {
                id: rowBottomAddress
                width: parent.width
                height: Constants.SIZE_V_BOTTOM_COMPONENT
                anchors.top: rowStateAddress.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            }
        }
    }

    Frame {
        id: frameTest
        x: 0
        y: 207
        width: parent.width
        height: 76
    }

    Frame {
        id: frameModify
        x: 0
        y: 289
        width: parent.width
        height: 195
    }

    TextField {
        id: textFieldTest
        x: 156
        y: 225
        width: 100
        height: 40
        color: Constants.COLOR_TEXT_BODY
        font.capitalization: Font.AllUppercase
        echoMode: TextInput.Password
        font.family: lato.name
    }

    TextField {
        id: textFieldNew
        x: 156
        y: 363
        width: 100
        height: 40
        echoMode:TextInput.Password
    }

    TextField {
        id: textFieldCurrent
        x: 156
        y: 309
        width: 100
        height: 40
        echoMode:TextInput.Password
        color: Constants.COLOR_TEXT_BODY
        font.capitalization: Font.AllUppercase
        font.family: lato.name
    }

    Text {
        id: textConfimr
        x: 14
        y: 431
        color: Constants.COLOR_TEXT_BODY
        text: "Confirmar novo PIN"
        font.capitalization: Font.AllUppercase
        font.family: lato.name
        font.pixelSize: Constants.SIZE_TEXT_LABEL
    }

    Text {
        id: textNew
        x: 85
        y: 377
        color: Constants.COLOR_TEXT_BODY
        text: "Novo PIN"
        font.capitalization: Font.AllUppercase
        font.family: lato.name
        font.pixelSize: Constants.SIZE_TEXT_LABEL
    }

    Text {
        id: textInput
        x: 83
        y: 239
        color: Constants.COLOR_TEXT_BODY
        text: "PIN Atual"
        font.capitalization: Font.AllUppercase
        font.family: lato.name
        font.pixelSize: Constants.SIZE_TEXT_LABEL
    }

    Button {
        id: buttonModify
        x: 300
        y: 417
        width: 150
        height: 40
        text: "Modificar PIN"
    }

    Text {
        id: textCurrent
        x: 83
        y: 323
        text: "PIN Atual"
        font.capitalization: Font.AllUppercase
        font.family: lato.name
        color: Constants.COLOR_TEXT_BODY
        font.pixelSize: Constants.SIZE_TEXT_LABEL
    }

    Button {
        id: buttonTest
        x: 300
        y: 225
        width: 150
        height: 40
        text: "Testar PIN"
    }

    TextField {
        id: textFieldConfimr
        x: 156
        y: 417
        width: 100
        height: 40
        echoMode:TextInput.Password
    }
}
