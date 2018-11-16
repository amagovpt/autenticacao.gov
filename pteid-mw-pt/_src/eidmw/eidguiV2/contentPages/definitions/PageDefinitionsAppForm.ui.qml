import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias propertyComboBoxReader: comboBoxReader
    property alias propertyRectReader: rectReader
    property alias propertyRectAppStart: rectAppStart
    property alias propertyRectAppStartCheckBox: rectAppStartCheckBox
    property alias propertyCheckboxAutoStart: checkboxAutoStart
    property alias propertyRectAppLanguage: rectAppLanguage
    property alias propertyRadioButtonPT: radioButtonPT
    property alias propertyRadioButtonUK: radioButtonUK
    property alias propertyRectAppLook: rectAppLook
    property alias propertyCheckboxShowAnime: checkboxShowAnime
    property alias propertyRectAppCertificates: rectAppCertificates
    property alias propertyCheckboxRegister: checkboxRegister
    property alias propertyCheckboxRemove: checkboxRemove
    property alias propertyRectAppTimeStamp: rectAppTimeStamp
    property alias propertyCheckboxTimeStamp: checkboxTimeStamp
    property alias propertyTextFieldTimeStamp: textFieldTimeStamp
    property alias propertyRectAppNetworkCheckBox: rectAppNetworkCheckBox
    property alias propertyCheckboxSystemProxy: checkboxSystemProxy
    property alias propertyCheckboxProxy: checkboxProxy
    property alias propertyTextFieldAdress: textFieldAdress
    property alias propertyTextFieldPort: textFieldPort
    property alias propertyCheckboxAutProxy: checkboxAutProxy
    property alias propertyTextFieldAutUser: textFieldAutUser
    property alias propertyTextFieldAutPass: textFieldAutPass

    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_DEFINITIONS_APP_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_DEFINITIONS_APP_ROW_TOP_INC_RELATIVE
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
            height: textReader.height + rectReaderCombo.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            Text {
                id: textReader
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp","STR_CARD_READER_TITLE") + controler.autoTr
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
            height: dateAppStart.height + rectAppStartCheckBox.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectReader.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: dateAppStart
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp","STR_START_TITLE") + controler.autoTr
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
                height: 25 + Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateAppStart.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                CheckBox {
                    id: checkboxAutoStart
                    text: qsTranslate("PageDefinitionsApp","STR_AUTO_START_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
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
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: dateAppLanguage
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp","STR_LANGUAGE_TITLE") + controler.autoTr
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
                    text: qsTranslate("PageDefinitionsApp","STR_PT_NAME_OP") + controler.autoTr
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                }
                RadioButton {
                    id: radioButtonUK
                    height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                    x: rectAppLanguageCheckBox.width * 0.5
                    text: qsTranslate("PageDefinitionsApp","STR_UK_NAME_OP") + controler.autoTr
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                }
            }
        }
        Item{
            id: rectAppLook
            width: parent.width
            height: dateAppLook.height + rectAppLookCheckBox.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppLanguage.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: dateAppLook
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp","STR_APP_LOOK_TITLE") + controler.autoTr
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
                height: 25 + Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateAppLook.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                CheckBox {
                    id: checkboxShowAnime
                    text: qsTranslate("PageDefinitionsApp","STR_SHOW_ANIME_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                }
            }
        }
        Item{
            id: rectAppCertificates
            width: parent.width
            height: dateAppCertificates.height + rectAppCertificatesCheckBox.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppLook.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: dateAppCertificates
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp","STR_CERTIFICATES_TITLE") + controler.autoTr
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
                    text: qsTranslate("PageDefinitionsApp","STR_CERTIFICATES_REGISTER_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                }
                CheckBox {
                    id: checkboxRemove
                    text: qsTranslate("PageDefinitionsApp","STR_CERTIFICATES_REMOVE_OP") + controler.autoTr
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
            height: dateAppTimeStamp.height + rectAppTimeStampCheckBox.height + Constants.SIZE_TEXT_V_SPACE
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.top: rectAppCertificates.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: dateAppTimeStamp
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp","STR_TIMESTAMP_TITLE") + controler.autoTr
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
                    text: qsTranslate("PageDefinitionsApp","STR_TIMESTAMP_OP") + controler.autoTr
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
                        placeholderText: qsTranslate("PageDefinitionsApp","STR_TIMESTAMP_URL_OP") + controler.autoTr
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: false
                        enabled: checkboxTimeStamp.checked
                        opacity: checkboxTimeStamp.checked ?
                                     1.0 :
                                     Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                        inputMethodHints: Qt.ImhUrlCharactersOnly
                        validator: RegExpValidator{
                            //http/https url validator
                            regExp: /https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%_\+.~#?&//=]*)/
                        }
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
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

            Text {
                id: dateAppNetwork
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageDefinitionsApp","STR_NETWORK_TITLE") + controler.autoTr
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
                        + checkboxSystemProxy.height
                        + boxAppAdress.height
                        + Constants.SIZE_TEXT_V_SPACE
                anchors.top : dateAppNetwork.bottom

                CheckBox {
                    id: checkboxSystemProxy
                    text: qsTranslate("PageDefinitionsApp","STR_NETWORK_SYSTEM_PROXY_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    checked: false
                }

                CheckBox {
                    id: checkboxProxy
                    text: qsTranslate("PageDefinitionsApp","STR_NETWORK_PROXY_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.top: checkboxSystemProxy.bottom
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
                        placeholderText: qsTranslate("PageDefinitionsApp","STR_NETWORK_PROXY_ADDRESS_OP") + controler.autoTr
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        enabled: checkboxProxy.checked
                        opacity: checkboxProxy.checked ?
                                     1.0 :
                                     Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                        validator: RegExpValidator{
                            //validates IPV4/IPV6 host
                            regExp: /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z]|[a-zA-Z][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\-]*[A-Za-z0-9])$|^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*/
                        }
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
                        placeholderText: qsTranslate("PageDefinitionsApp","STR_NETWORK_PROXY_PORT_OP") + controler.autoTr
                        validator: RegExpValidator { regExp: /[0-9]+/ }
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: true
                        maximumLength: 5
                        enabled: checkboxProxy.checked
                        opacity: checkboxProxy.checked ?
                                     1.0 :
                                     Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                    }
                }
                CheckBox {
                    id: checkboxAutProxy
                    x: parent.width * 0.40
                    text: qsTranslate("PageDefinitionsApp","STR_NETWORK_AUTH_OP") + controler.autoTr
                    height: 25
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    anchors.top: checkboxSystemProxy.bottom
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
                        placeholderText: qsTranslate("PageDefinitionsApp","STR_NETWORK_AUTH_USERNAME_OP") + controler.autoTr
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: false
                        enabled: checkboxAutProxy.checked
                        opacity: checkboxAutProxy.checked ?
                                     1.0 :
                                     Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
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
                        placeholderText: qsTranslate("PageDefinitionsApp","STR_NETWORK_AUTH_PASSWORD_OP") + controler.autoTr
                        echoMode : TextInput.Password
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        clip: false
                        enabled: checkboxAutProxy.checked
                        opacity: checkboxAutProxy.checked ?
                                     1.0 :
                                     Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                    }
                }
            }
        }
    }
}
