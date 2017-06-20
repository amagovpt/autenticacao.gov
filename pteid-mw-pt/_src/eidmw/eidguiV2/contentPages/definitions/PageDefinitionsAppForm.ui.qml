import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {

    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_DIFINITIONS_APP_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_DIFINITIONS_APP_ROW_TOP_INC_RELATIVE
    }

    Item {
        id: rowMain
        width: parent.width
        height: parent.height
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectReader
            width: parent.width
            height: textReader.height + rectReaderCombo.height + 2 * Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            Text {
                id: textReader
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Leitor de Cartões"
            }
            DropShadow {
                anchors.fill: rectReaderCombo
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectReaderCombo
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectReaderCombo
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectReaderCombo
                width: parent.width
                color: "white"
                height: 3 * Constants.SIZE_TEXT_FIELD
                anchors.top : textReader.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                ComboBox {
                    id: comboBoxReader
                    width: parent.width - 2 * Constants.SIZE_TEXT_V_SPACE
                    height: 3 * Constants.SIZE_TEXT_FIELD
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: [ "Bit4id minilector 00 00", "Bit4id minilector 00 02", "Bit4id minilector 00 03"]
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    visible: true
                }
            }
        }
        Item{
            id: rectAppStart
            width: parent.width
            height: dateAppStart.height + rectAppStartCheckBox.height + 2 * Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectReader.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Text {
                id: dateAppStart
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Início"
            }
            DropShadow {
                anchors.fill: rectAppStartCheckBox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectAppStartCheckBox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectAppStartCheckBox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectAppStartCheckBox
                width: parent.width
                color: "white"
                height: checkboxAutoRead.height + checkboxStart.height + 3 * Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateAppStart.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                CheckBox {
                    id: checkboxAutoRead
                    text: "Leitura automática do cartão"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: true
                }
                CheckBox {
                    id: checkboxStart
                    text: "Iniciar minimizada"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.top: checkboxAutoRead.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                }
            }
        }
        Item{
            id: rectAppLook
            width: parent.width
            height: dateAppStart.height + rectAppLookCheckBox.height + 2 * Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppStart.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Text {
                id: dateAppLook
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Aparência"
            }
            DropShadow {
                anchors.fill: rectAppLookCheckBox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectAppLookCheckBox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectAppLookCheckBox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectAppLookCheckBox
                width: parent.width
                color: "white"
                height: checkboxShowNot.height
                        + checkboxShowPhoto.height
                        + checkboxShowAnime.height
                        + 3 * Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateAppLook.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                CheckBox {
                    id: checkboxShowNot
                    text: "Apresentar notificações"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: true
                }
                CheckBox {
                    id: checkboxShowPhoto
                    text: "Mostrar fotografia ao iniciar"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.top: checkboxShowNot.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                }
                CheckBox {
                    id: checkboxShowAnime
                    text: "Activar menu animações"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.top: checkboxShowPhoto.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: true
                }
            }
        }
        Item{
            id: rectAppNetwork
            width: parent.width
            height: dateAppNetwork.height + rectAppNetworkCheckBox.height + 2 * Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppLook.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Text {
                id: dateAppNetwork
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Configurações de rede"
            }
            DropShadow {
                anchors.fill: rectAppNetworkCheckBox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectAppNetworkCheckBox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectAppNetworkCheckBox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectAppNetworkCheckBox
                width: parent.width
                color: "white"
                anchors.topMargin: 6
                height: checkboxProxy.height
                        + boxAppAdress.height
                        + checkboxAutProxy.height
                        + boxAppAutAdress.height
                        + 3 * Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateAppNetwork.bottom

                CheckBox {
                    id: checkboxProxy
                    text: "Utilizar servidor Proxy"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: true
                }
                Item{
                    id: boxAppAdress
                    width: 150
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: checkboxProxy.bottom
                    x: 6 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    Components.LabelTextBoxForm{
                        propertyDateText.text: "Endereço"
                        propertyDateField.text: ""
                        propertyDateField.readOnly: false
                    }
                }
                Item {
                    id: boxAppPort
                    width: 150
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                    anchors.topMargin: 0
                    anchors.top: checkboxProxy.bottom
                    anchors.left: boxAppAdress.right
                    Components.LabelTextBoxForm {
                        propertyDateText.text: "Porto"
                        propertyDateField.text: "8080"
                        propertyDateField.readOnly: false
                    }
                }
                CheckBox {
                    id: checkboxAutProxy
                    text: "Utilizar autenticação de Proxy"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.top: boxAppPort.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: true
                }
                Item{
                    id: boxAppAutAdress
                    width: 150
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: checkboxAutProxy.bottom
                    x: 6 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    Components.LabelTextBoxForm{
                        propertyDateText.text: "Utilizador"
                        propertyDateField.text: ""
                        propertyDateField.readOnly: false
                    }
                }
                Item {
                    id: boxAppAutPort
                    width: 150
                    height: Constants.SIZE_TEXT_LABEL
                            + Constants.SIZE_TEXT_V_SPACE
                            + 2 * Constants.SIZE_TEXT_FIELD
                    anchors.top: checkboxAutProxy.bottom
                    anchors.left: boxAppAutAdress.right
                    anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                    Components.LabelTextBoxForm {
                        propertyDateText.text: "Palavra-passe"
                        propertyDateField.text: ""
                        propertyDateField.readOnly: false
                    }
                }
            }
        }
    }
}
