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
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP

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
                height: checkboxAutoRead.height + Constants.SIZE_TEXT_V_SPACE
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
                    x: rectAppStartCheckBox.width * 0.5
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                }
            }
        }

        Item{
            id: rectAppLanguage
            width: parent.width
            height: dateAppLanguage.height + rectAppLanguageCheckBox.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppStart.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP

            Text {
                id: dateAppLanguage
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Idioma"
            }
            DropShadow {
                anchors.fill: rectAppLanguageCheckBox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectAppLanguageCheckBox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectAppLanguageCheckBox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectAppLanguageCheckBox
                width: parent.width
                color: "white"
                height: radioButtonPT.height
                anchors.top : dateAppLanguage.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                RadioButton {
                    id: radioButtonPT
                    height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                    text: "Português"
                    checked: true
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                }
                RadioButton {
                    id: radioButtonUK
                    height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                    x: rectAppLanguageCheckBox.width * 0.5
                    text: "English"
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                }
            }
        }

        Item{
            id: rectAppLook
            width: parent.width
            height: dateAppStart.height + rectAppLookCheckBox.height + 2 * Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppLanguage.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP

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
                        + 2 * Constants.SIZE_TEXT_V_SPACE
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
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    x: rectAppLookCheckBox.width * 0.5
                    checked: true
                }
            }
        }
        Item{
            id: rectAppCertificates
            width: parent.width
            height: dateAppStart.height + rectAppCertificatesCheckBox.height
                    + 2 * Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppLook.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP

            Text {
                id: dateAppCertificates
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Certificados"
            }
            DropShadow {
                anchors.fill: rectAppCertificatesCheckBox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectAppCertificatesCheckBox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectAppCertificatesCheckBox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectAppCertificatesCheckBox
                width: parent.width
                color: "white"
                height: checkboxRegister.height
                        + checkboxRemove.height
                        + 2 * Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateAppCertificates.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                CheckBox {
                    id: checkboxRegister
                    text: "Registar certificados com a inserção do cartão"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: true
                }
                CheckBox {
                    id: checkboxRemove
                    text: "Remover certificados com a remoção do cartão"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.top: checkboxRegister.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                }
            }
        }

        Item{
            id: rectAppTimeStamp
            width: parent.width
            height: dateAppTimeStamp.height + rectAppTimeStampCheckBox.height + 2 * Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppCertificates.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP

            Text {
                id: dateAppTimeStamp
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Selos Temporais"
            }
            DropShadow {
                anchors.fill: rectAppTimeStampCheckBox
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectAppTimeStampCheckBox
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectAppTimeStampCheckBox
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }
            Rectangle {
                id: rectAppTimeStampCheckBox
                width: parent.width
                color: "white"
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                height: checkboxTimeStamp.height
                        + boxAppTimeStamp.height
                anchors.top : dateAppTimeStamp.bottom

                CheckBox {
                    id: checkboxTimeStamp
                    text: "Utilizar serviço temporal personalizado"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: false
                }
                Item{
                    id: boxAppTimeStamp
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: textFieldTimeStamp.height
                    anchors.top: checkboxTimeStamp.bottom
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE

                    TextField {
                        id: textFieldTimeStamp
                        width: parent.width
                        font.italic: textFieldTimeStamp.text === "" ? true: false
                        placeholderText: "URL do servidor de selos temporais?"
                        inputMethodHints: Qt.ImhDigitsOnly
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: false
                        enabled: checkboxTimeStamp.checked
                        opacity: checkboxTimeStamp.checked ?
                                     1.0 :
                                     Constants.OPACITY_DIFINITIONS_APP_OPTION_DISABLED
                    }
                }
            }
        }

        Item{
            id: rectAppNetwork
            width: parent.width
            height: dateAppNetwork.height + rectAppNetworkCheckBox.height
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppTimeStamp.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DIFINITIONS_APP

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
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                height: checkboxProxy.height
                        + boxAppAdress.height
                anchors.top : dateAppNetwork.bottom

                CheckBox {
                    id: checkboxProxy
                    text: "Utilizar servidor Proxy"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: false
                }
                Item{
                    id: boxAppAdress
                    width: 120
                    height: textFieldAdress.height
                    anchors.top: checkboxProxy.bottom
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE

                    TextField {
                        id: textFieldAdress
                        width: parent.width
                        font.italic: textFieldAdress.text === "" ? true: false
                        placeholderText: "Endereço?"
                        validator: RegExpValidator { regExp: /[0-9A-Fa-f.:]+/ }
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        enabled: checkboxProxy.checked
                        opacity: checkboxProxy.checked ?
                                     1.0 :
                                     Constants.OPACITY_DIFINITIONS_APP_OPTION_DISABLED
                    }
                }
                Item {
                    id: boxAppPort
                    width: 37
                    height: textFieldPort.height
                    anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                    anchors.top: checkboxProxy.bottom
                    anchors.left: boxAppAdress.right
                    TextField {
                        id: textFieldPort
                        width: parent.width
                        font.italic: textFieldPort.text === "" ? true: false
                        placeholderText: "Porto?"
                        validator: RegExpValidator { regExp: /[0-9]+/ }
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: true
                        maximumLength: 5
                        enabled: checkboxProxy.checked
                        opacity: checkboxProxy.checked ?
                                     1.0 :
                                     Constants.OPACITY_DIFINITIONS_APP_OPTION_DISABLED
                    }
                }
                CheckBox {
                    id: checkboxAutProxy
                    x: parent.width * 0.40
                    text: "Utilizar autenticação de Proxy"
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: false
                }
                Item{
                    id: boxAppAutUser
                    width: parent.width * 0.3 - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: textFieldAutUser.height
                    anchors.top: checkboxAutProxy.bottom
                    anchors.left: checkboxAutProxy.left
                    anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                    TextField {
                        id: textFieldAutUser
                        width: parent.width
                        font.italic: textFieldAutUser.text === "" ? true: false
                        placeholderText: "Utilizador?"
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: false
                        enabled: checkboxAutProxy.checked
                        opacity: checkboxAutProxy.checked ?
                                     1.0 :
                                     Constants.OPACITY_DIFINITIONS_APP_OPTION_DISABLED
                    }
                }
                Item {
                    id: boxAppAutPass
                    width: parent.width * 0.3 - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: textFieldAutPass.height
                    anchors.top: checkboxAutProxy.bottom
                    anchors.left: boxAppAutUser.right
                    anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                    TextField {
                        id: textFieldAutPass
                        width: parent.width
                        font.italic: textFieldAutPass.text === "" ? true: false
                        placeholderText: "Palavra-passe?"
                        echoMode : TextInput.Password
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: false
                        enabled: checkboxAutProxy.checked
                        opacity: checkboxAutProxy.checked ?
                                     1.0 :
                                     Constants.OPACITY_DIFINITIONS_APP_OPTION_DISABLED
                    }
                }
            }
        }
    }
}
