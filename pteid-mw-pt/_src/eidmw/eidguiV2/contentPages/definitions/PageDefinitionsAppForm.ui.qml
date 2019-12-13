import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    property alias propertyComboBoxReader: comboBoxReader
    property alias propertyCheckboxUseSystemScale: checkboxUseSystemScale
    property alias propertyComboBoxScaling: comboBoxScaling
    property alias propertyRectReader: rectReader
    property alias propertyTextReader: textReader
    property alias propertyCheckboxEnablePinpad: checkboxEnablePinpad
    property alias propertyRectAppStart: rectAppStart
    property alias propertyRectAppStartCheckBox: rectAppStartCheckBox
    property alias propertyRectStartAutoupdate: rectStartAutoupdate
    property alias propertyCheckboxStartAutoupdate: checkboxStartAutoupdate
    property alias propertyCheckboxAutoStart: checkboxAutoStart
    property alias propertyRectAppLanguage: rectAppLanguage
    property alias propertyRadioButtonPT: radioButtonPT
    property alias propertyRadioButtonUK: radioButtonUK
    property alias propertyRectAppLook: rectAppLook
    property alias propertyCheckboxShowAnime: checkboxShowAnime
    property alias propertyCheckBoxDebugMode: checkboxDebugMode
    property alias propertyGraphicsTextField: graphicsTextField
    property alias propertyCheckboxAccelGraphics: checkboxAccelGraphics
    property alias propertyRectAppNetworkCheckBox: rectAppNetworkCheckBox
    property alias propertyCheckboxSystemProxy: checkboxSystemProxy
    property alias propertyCheckboxProxy: checkboxProxy
    property alias propertyTextFieldAdress: textFieldAdress
    property alias propertyTextFieldPort: textFieldPort
    property alias propertyCheckboxAutProxy: checkboxAutProxy
    property alias propertyTextFieldAutUser: textFieldAutUser
    property alias propertyTextFieldAutPass: textFieldAutPass
    property alias propertySettingsScroll: settingsScroll
    property alias propertyRowMain: rowMain
    anchors.fill: parent

    Item {
        id: rowTop
        width: parent.width
        height: parent.height * Constants.HEIGHT_DEFINITIONS_APP_ROW_TOP_V_RELATIVE
                + (parent.height + Constants.TITLE_BAR_SIZE - Constants.SCREEN_MINIMUM_HEIGHT)
                * Constants.HEIGHT_DEFINITIONS_APP_ROW_TOP_INC_RELATIVE
    }

    Flickable {
        id: rowMain
        width: parent.width + Constants.SIZE_ROW_H_SPACE
        height: parent.height - rowTop.height - Constants.SIZE_ROW_V_SPACE
        anchors.top: rowTop.bottom
        anchors.right: parent.right
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        clip:true
        contentHeight: content.childrenRect.height + 2*Constants.SIZE_ROW_V_SPACE

        ScrollBar.vertical: ScrollBar {
            id: settingsScroll
            parent: rowMain.parent
            visible: true
            active: true // QtQuick.Controls 2.1 does not have AlwaysOn prop
            width: Constants.SIZE_TEXT_FIELD_H_SPACE
            anchors.right: parent.right
            anchors.top: rowTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE
            anchors.bottom: parent.bottom
            stepSize : 1.0
        }
        Item{
            id: content
            anchors.top: parent.top
            width: rowMain.parent.width - Constants.SIZE_ROW_H_SPACE
            x: Constants.SIZE_ROW_H_SPACE
            Item{
                id: rectReader
                width: parent.width
                height: textReader.height + rectReaderCombo.height + 3*Constants.SIZE_TEXT_V_SPACE
                Text {
                    id: textReader
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp","STR_CARD_READER_TITLE") + controler.autoTr
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: textReader.text
                    KeyNavigation.tab: textSelectReader
                    KeyNavigation.down: textSelectReader
                    KeyNavigation.right: textSelectReader

                    // If this component is changed, the workaround to scroll the page automatically with
                    // keyboard navigation has to be updated also;
                    // this is located in PageDefinitionsApp.qml - function handleKeyPressed
                    Keys.onPressed: {
                        handleKeyPressed(event.key, textReader)
                    }
                    KeyNavigation.backtab: textFieldAutPass
                    KeyNavigation.up: textFieldAutPass
                    KeyNavigation.left: textFieldAutPass
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
                    height: 4 * Constants.SIZE_TEXT_FIELD + checkboxEnablePinpad.height + 4*Constants.SIZE_TEXT_V_SPACE 
                    anchors.top : textReader.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                    Text {
                        id: textSelectReader
                        x: 10
                        y: Constants.SIZE_TEXT_V_SPACE
                        font.capitalization: Font.MixedCase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.bold: activeFocus
                        color: Constants.COLOR_TEXT_BODY
                        wrapMode: Text.WordWrap
                        Accessible.role: Accessible.TitleBar
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key,textSelectReader)
                        }
                        KeyNavigation.tab: comboBoxReader
                        KeyNavigation.down: comboBoxReader
                        KeyNavigation.right: comboBoxReader
                        KeyNavigation.backtab: textReader
                        KeyNavigation.up: textReader
                        KeyNavigation.left: textReader
                        text: qsTranslate("PageDefinitionsApp", "STR_SELECT_READER") + controler.autoTr
                    }

                    ComboBox {
                        id: comboBoxReader
                        width: parent.width - 2 * Constants.SIZE_TEXT_V_SPACE
                        height: 3 * Constants.SIZE_TEXT_FIELD
                        anchors.top: textSelectReader.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        visible: true
                        Accessible.role: Accessible.ComboBox
                        Accessible.name: currentText
                        Keys.onPressed: {
                            if (event.key != Qt.Key_Down)
                                handleKeyPressed(event.key, comboBoxReader)
                        }
                        KeyNavigation.tab: checkboxEnablePinpad
                        KeyNavigation.down: checkboxEnablePinpad
                        KeyNavigation.right: checkboxEnablePinpad
                        KeyNavigation.backtab: textSelectReader
                        KeyNavigation.up: textSelectReader
                        KeyNavigation.left: textSelectReader
                    }

                    CheckBox {
                        id: checkboxEnablePinpad
                        enabled: false
                        text: qsTranslate("PageDefinitionsApp", "STR_USE_PINPAD") + controler.autoTr
                        height: 25
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        anchors.top: comboBoxReader.bottom
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxEnablePinpad)
                        }
                        KeyNavigation.tab: dateAppStart
                        KeyNavigation.down: dateAppStart
                        KeyNavigation.right: dateAppStart
                        KeyNavigation.backtab: comboBoxReader
                        KeyNavigation.up: comboBoxReader
                        KeyNavigation.left: comboBoxReader
                    }
                }
            }

            Item{
                id: rectAppStart
                width: parent.width
                height: dateAppStart.height + rectAppStartCheckBox.height + 3*Constants.SIZE_TEXT_V_SPACE
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.top: rectReader.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP
                Text {
                    id: dateAppStart
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp","STR_START_TITLE") + controler.autoTr
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: text
                    Keys.onPressed: {
                        handleKeyPressed(event.key, dateAppStart)
                    }
                    KeyNavigation.tab: checkboxAutoStart
                    KeyNavigation.down: checkboxAutoStart
                    KeyNavigation.right: checkboxAutoStart
                    KeyNavigation.backtab: checkboxEnablePinpad
                    KeyNavigation.up: checkboxEnablePinpad
                    KeyNavigation.left: checkboxEnablePinpad
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
                        font.bold: activeFocus
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxAutoStart)
                        }
                        KeyNavigation.tab: startAutoupdateText
                        KeyNavigation.down: startAutoupdateText
                        KeyNavigation.right: startAutoupdateText
                        KeyNavigation.backtab: dateAppStart
                        KeyNavigation.up: dateAppStart
                        KeyNavigation.left: dateAppStart
                    }
                }
            }

            Item{
                id: rectStartAutoupdate
                width: parent.width
                height: startAutoupdateText.height + rectStartAutoupdateCheckbox.height + 3*Constants.SIZE_TEXT_V_SPACE
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.top: rectAppStart.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP
                Text {
                    id: startAutoupdateText
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp","STR_START_AUTOUPDATES_TITLE") + controler.autoTr
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: text
                    Keys.onPressed: {
                        handleKeyPressed(event.key, startAutoupdateText)
                    }
                    KeyNavigation.tab: checkboxStartAutoupdate
                    KeyNavigation.down: checkboxStartAutoupdate
                    KeyNavigation.right: checkboxStartAutoupdate
                    KeyNavigation.backtab: checkboxAutoStart
                    KeyNavigation.up: checkboxAutoStart
                    KeyNavigation.left: checkboxAutoStart
                }
                DropShadow {
                    anchors.fill: rectStartAutoupdateCheckbox
                    horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                    verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                    radius: Constants.FORM_SHADOW_RADIUS
                    samples: Constants.FORM_SHADOW_SAMPLES
                    color: Constants.COLOR_FORM_SHADOW
                    source: rectStartAutoupdateCheckbox
                    spread: Constants.FORM_SHADOW_SPREAD
                    opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
                }
                RectangularGlow {
                    anchors.fill: rectStartAutoupdateCheckbox
                    glowRadius: Constants.FORM_GLOW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                    opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
                }
                Rectangle {
                    id: rectStartAutoupdateCheckbox
                    width: parent.width
                    color: "white"
                    height: 25 + Constants.SIZE_TEXT_V_SPACE
                    anchors.top : startAutoupdateText.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                    CheckBox {
                        id: checkboxStartAutoupdate
                        text: qsTranslate("PageDefinitionsApp","STR_CARD_READER_TEXT") + controler.autoTr
                        height: 25
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxStartAutoupdate)
                        }
                        KeyNavigation.tab: dateAppLanguage
                        KeyNavigation.down: dateAppLanguage
                        KeyNavigation.right: dateAppLanguage
                        KeyNavigation.backtab: startAutoupdateText
                        KeyNavigation.up: startAutoupdateText
                        KeyNavigation.left: startAutoupdateText
                    }
                }
            }


            Item{
                id: rectAppLanguage
                width: parent.width
                height: dateAppLanguage.height + rectAppLanguageCheckBox.height + 3*Constants.SIZE_TEXT_V_SPACE
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.top: rectStartAutoupdate.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

                Text {
                    id: dateAppLanguage
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp","STR_LANGUAGE_TITLE") + controler.autoTr
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: text
                    Keys.onPressed: {
                        handleKeyPressed(event.key, dateAppLanguage)
                    }
                    KeyNavigation.tab: radioButtonPT
                    KeyNavigation.down: radioButtonPT
                    KeyNavigation.right: radioButtonPT
                    KeyNavigation.backtab: checkboxStartAutoupdate
                    KeyNavigation.up: checkboxStartAutoupdate
                    KeyNavigation.left: checkboxStartAutoupdate
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
                    anchors.top: dateAppLanguage.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                    Rectangle {
                        id: rectLeft
                        width: parent.width/2
                        height: parent.height
                        RadioButton {
                            id: radioButtonPT
                            height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                            text: qsTranslate("PageDefinitionsApp",
                                              "STR_PT_NAME_OP") + controler.autoTr
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.bold: activeFocus
                            Accessible.role: Accessible.RadioButton
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key, radioButtonPT)
                            }
                            KeyNavigation.tab: radioButtonUK
                            KeyNavigation.down: radioButtonUK
                            KeyNavigation.right: radioButtonUK
                            KeyNavigation.backtab: dateAppLanguage
                            KeyNavigation.up: dateAppLanguage
                            KeyNavigation.left: dateAppLanguage
                        }
                    }
                    Rectangle {
                        id: rectRight
                        width: parent.width/2
                        height: parent.height
                        anchors.left: rectLeft.right
                        RadioButton {
                            id: radioButtonUK
                            height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                            text: qsTranslate("PageDefinitionsApp",
                                              "STR_UK_NAME_OP") + controler.autoTr
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.bold: activeFocus
                            Accessible.role: Accessible.RadioButton
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key, radioButtonUK)
                            }
                            KeyNavigation.tab: dateAppLook
                            KeyNavigation.down: dateAppLook
                            KeyNavigation.right: dateAppLook
                            KeyNavigation.backtab: radioButtonPT
                            KeyNavigation.up: radioButtonPT
                            KeyNavigation.left: radioButtonPT
                        }
                    }
                }
            }
            Item {
                id: rectAppLook
                width: parent.width
                height: dateAppLook.height + rectAppLooks.height + 3*Constants.SIZE_TEXT_V_SPACE
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.top: rectAppLanguage.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

                Text {
                    id: dateAppLook
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp",
                                      "STR_APP_LOOK_TITLE") + controler.autoTr
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: text
                    Keys.onPressed: {
                        handleKeyPressed(event.key, dateAppLook)
                    }
                    KeyNavigation.tab: checkboxShowAnime
                    KeyNavigation.down: checkboxShowAnime
                    KeyNavigation.right: checkboxShowAnime
                    KeyNavigation.backtab: radioButtonUK
                    KeyNavigation.up: radioButtonUK
                    KeyNavigation.left: radioButtonUK
                }
                DropShadow {
                    anchors.fill: rectAppLooks
                    horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                    verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                    radius: Constants.FORM_SHADOW_RADIUS
                    samples: Constants.FORM_SHADOW_SAMPLES
                    color: Constants.COLOR_FORM_SHADOW
                    source: rectAppLooks
                    spread: Constants.FORM_SHADOW_SPREAD
                    opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
                }
                RectangularGlow {
                    anchors.fill: rectAppLooks
                    glowRadius: Constants.FORM_GLOW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                    opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
                }
                Rectangle {
                    id: rectAppLooks
                    width: parent.width
                    height: rectAppAnimationsCheckBox.height + rectAppScaling.height + Constants.SIZE_TEXT_V_SPACE
                    anchors.top: dateAppLook.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE


                    Rectangle {
                        id: rectAppAnimationsCheckBox
                        width: parent.width
                        color: "white"
                        height: 25 + Constants.SIZE_TEXT_V_SPACE
                        anchors.top: rectAppLooks.top

                        CheckBox {
                            id: checkboxShowAnime
                            text: qsTranslate("PageDefinitionsApp",
                                            "STR_SHOW_ANIME_OP") + controler.autoTr
                            height: 25
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.bold: activeFocus
                            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                            Accessible.role: Accessible.CheckBox
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key, checkboxShowAnime)
                            }
                            KeyNavigation.tab: textSelectScaling
                            KeyNavigation.down: textSelectScaling
                            KeyNavigation.right: textSelectScaling
                            KeyNavigation.backtab: dateAppLook
                            KeyNavigation.up: dateAppLook
                            KeyNavigation.left: dateAppLook
                        }
                    }

                    Rectangle {
                        id: rectAppScaling
                        width: parent.width
                        color: "white"
                        height: 4 * Constants.SIZE_TEXT_FIELD + 2*Constants.SIZE_TEXT_V_SPACE
                        anchors.top: rectAppAnimationsCheckBox.bottom
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                        Text {
                            id: textSelectScaling
                            x: 10
                            y: Constants.SIZE_TEXT_V_SPACE
                            font.capitalization: Font.MixedCase
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.family: lato.name
                            font.bold: activeFocus
                            color: Constants.COLOR_TEXT_BODY
                            wrapMode: Text.WordWrap
                            Accessible.role: Accessible.TitleBar
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key,textSelectReader)
                            }
                            KeyNavigation.tab: checkboxUseSystemScale.visible ? checkboxUseSystemScale : comboBoxScaling
                            KeyNavigation.down: checkboxUseSystemScale.visible ? checkboxUseSystemScale : comboBoxScaling
                            KeyNavigation.right: checkboxUseSystemScale.visible ? checkboxUseSystemScale : comboBoxScaling
                            KeyNavigation.backtab: checkboxShowAnime
                            KeyNavigation.up: checkboxShowAnime
                            KeyNavigation.left: checkboxShowAnime
                            text: qsTranslate("PageDefinitionsApp", "STR_SCALE_APPLICATION_TITLE") + controler.autoTr
                        }

                        CheckBox {
                            id: checkboxUseSystemScale
                            enabled: false
                            text: qsTranslate("PageDefinitionsApp", "STR_USE_SYSTEM_SETTING") + controler.autoTr
                            height: 25
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.bold: activeFocus
                            anchors.top: textSelectScaling.bottom
                            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                            Accessible.role: Accessible.CheckBox
                            Accessible.name: text
                            Keys.onPressed: {
                                handleKeyPressed(event.key, checkboxShowAnime)
                            }
                            KeyNavigation.tab: comboBoxScaling
                            KeyNavigation.down: comboBoxScaling
                            KeyNavigation.right: comboBoxScaling
                            KeyNavigation.backtab: textSelectScaling
                            KeyNavigation.up: textSelectScaling
                            KeyNavigation.left: textSelectScaling
                        }

                        ComboBox {
                            id: comboBoxScaling
                            enabled: !checkboxUseSystemScale.checked
                            height: 3 * Constants.SIZE_TEXT_FIELD
                            anchors.left: checkboxUseSystemScale.right
                            anchors.leftMargin: Constants.SIZE_TEXT_V_SPACE
                            anchors.right: rectAppScaling.right
                            anchors.rightMargin: Constants.SIZE_TEXT_V_SPACE
                            anchors.top: textSelectScaling.bottom
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.bold: activeFocus
                            visible: true
                            Accessible.role: Accessible.ComboBox
                            Accessible.name: currentText
                            Keys.onPressed: {
                                if (event.key != Qt.Key_Down)
                                    handleKeyPressed(event.key, comboBoxReader)
                            }
                            KeyNavigation.tab: dateDebugMode
                            KeyNavigation.down: dateDebugMode
                            KeyNavigation.right: dateDebugMode
                            KeyNavigation.backtab: checkboxUseSystemScale.visible ? checkboxUseSystemScale : textSelectScaling
                            KeyNavigation.up: checkboxUseSystemScale.visible ? checkboxUseSystemScale : textSelectScaling
                            KeyNavigation.left: checkboxUseSystemScale.visible ? checkboxUseSystemScale : textSelectScaling
                            model: ["100%", "125%", "150%", "175%"] // each increment must be 25%
                        }
                    }
                }
            }

            Item {
                id: rectDebugMode
                width: parent.width
                height: dateDebugMode.height + rectDebugModeCheckBox.height + 3 * Constants.SIZE_TEXT_V_SPACE
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.top: rectAppLook.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

                Text {
                    id: dateDebugMode
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp", "STR_DEBUG_MODE_TITLE") + controler.autoTr
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: text
                    Keys.onPressed: {
                        handleKeyPressed(event.key, dateDebugMode)
                    }
                    KeyNavigation.tab: debugModeTextField
                    KeyNavigation.down: debugModeTextField
                    KeyNavigation.right: debugModeTextField
                    KeyNavigation.backtab: comboBoxScaling
                    KeyNavigation.up: comboBoxScaling
                    KeyNavigation.left: comboBoxScaling
                }

                DropShadow {
                    anchors.fill: rectDebugModeCheckBox
                    horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                    verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                    radius: Constants.FORM_SHADOW_RADIUS
                    samples: Constants.FORM_SHADOW_SAMPLES
                    color: Constants.COLOR_FORM_SHADOW
                    source: rectDebugModeCheckBox
                    spread: Constants.FORM_SHADOW_SPREAD
                    opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
                }
                RectangularGlow {
                    anchors.fill: rectDebugModeCheckBox
                    glowRadius: Constants.FORM_GLOW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                    opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
                }
                Rectangle {
                    id: rectDebugModeCheckBox
                    width: parent.width
                    color: "white"
                    height: debugModeTextField.height + checkboxDebugMode.height + 3*Constants.SIZE_TEXT_V_SPACE
                    anchors.top: dateDebugMode.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                    Text {
                        id: debugModeTextField
                        width: parent.width - 20
                        x: 10
                        y: Constants.SIZE_TEXT_V_SPACE
                        font.capitalization: Font.MixedCase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.bold: activeFocus
                        wrapMode: Text.WordWrap
                        Accessible.role: Accessible.TitleBar
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, debugModeTextField)
                        }
                        KeyNavigation.tab: checkboxDebugMode
                        KeyNavigation.down: checkboxDebugMode
                        KeyNavigation.right: checkboxDebugMode
                        KeyNavigation.backtab: dateDebugMode
                        KeyNavigation.up: dateDebugMode
                        KeyNavigation.left: dateDebugMode
                        text: qsTranslate("PageDefinitionsApp", "STR_DEBUG_MODE_DESCRIPTION") + controler.autoTr
                    }

                    CheckBox {
                        id: checkboxDebugMode
                        enabled: false
                        text: qsTranslate("PageDefinitionsApp", "STR_DEBUG_MODE_ENABLE") + controler.autoTr
                        height: 25
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        anchors.top: debugModeTextField.bottom
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxDebugMode)
                        }
                        KeyNavigation.tab: dateAppGraphics
                        KeyNavigation.down: dateAppGraphics
                        KeyNavigation.right: dateAppGraphics
                        KeyNavigation.backtab: debugModeTextField
                        KeyNavigation.up: debugModeTextField
                        KeyNavigation.left: debugModeTextField
                    }
                }
            }

            Item {
                id: rectAppGraphics
                width: parent.width
                height: dateAppGraphics.height + rectAppGraphicsCheckBox.height
                        + 3 * Constants.SIZE_TEXT_V_SPACE
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.top: rectDebugMode.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP

                Text {
                    id: dateAppGraphics
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp",
                                      "STR_APP_GRAPHICS_TITLE") + controler.autoTr
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: text
                    Keys.onPressed: {
                        handleKeyPressed(event.key, dateAppGraphics)
                    }
                    KeyNavigation.tab: graphicsTextField
                    KeyNavigation.down: graphicsTextField
                    KeyNavigation.right: graphicsTextField
                    KeyNavigation.backtab: checkboxDebugMode
                    KeyNavigation.up: checkboxDebugMode
                    KeyNavigation.left: checkboxDebugMode
                }
                DropShadow {
                    anchors.fill: rectAppGraphicsCheckBox
                    horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                    verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                    radius: Constants.FORM_SHADOW_RADIUS
                    samples: Constants.FORM_SHADOW_SAMPLES
                    color: Constants.COLOR_FORM_SHADOW
                    source: rectAppLooks
                    spread: Constants.FORM_SHADOW_SPREAD
                    opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
                }
                RectangularGlow {
                    anchors.fill: rectAppGraphicsCheckBox
                    glowRadius: Constants.FORM_GLOW_RADIUS
                    spread: Constants.FORM_GLOW_SPREAD
                    color: Constants.COLOR_FORM_GLOW
                    cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                    opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
                }
                Rectangle {
                    id: rectAppGraphicsCheckBox
                    width: parent.width
                    color: "white"
                    height: graphicsTextField.height + checkboxAccelGraphics.height
                            + 3 * Constants.SIZE_TEXT_V_SPACE
                    anchors.top: dateAppGraphics.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                    Text {
                        id: graphicsTextField
                        width: parent.width - 20
                        x: 10
                        y: Constants.SIZE_TEXT_V_SPACE
                        font.capitalization: Font.MixedCase
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.bold: activeFocus
                        wrapMode: Text.WordWrap
                        Accessible.role: Accessible.TitleBar
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, graphicsTextField)
                        }
                        KeyNavigation.tab: checkboxAccelGraphics
                        KeyNavigation.down: checkboxAccelGraphics
                        KeyNavigation.right: checkboxAccelGraphics
                        KeyNavigation.backtab: dateAppGraphics
                        KeyNavigation.up: dateAppGraphics
                        KeyNavigation.left: dateAppGraphics
                    }
                    CheckBox {
                        id: checkboxAccelGraphics
                        enabled: false
                        text: qsTranslate("PageDefinitionsApp",
                                          "STR_ACCEL_ENABLE")
                              + controler.autoTr
                        height: 25
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        anchors.top: graphicsTextField.bottom
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxAccelGraphics)
                        }
                        KeyNavigation.tab: dateAppNetwork
                        KeyNavigation.down: dateAppNetwork
                        KeyNavigation.right: dateAppNetwork
                        KeyNavigation.backtab: graphicsTextField
                        KeyNavigation.up: graphicsTextField
                        KeyNavigation.left: graphicsTextField
                    }
                }
            }

            Item {
                id: rectAppNetwork
                width: parent.width
                height: dateAppNetwork.height + rectAppNetworkCheckBox.height
                anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE_DEFINITIONS_APP
                anchors.top: rectAppGraphics.bottom

                Text {
                    id: dateAppNetwork
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    font.pixelSize: activeFocus
                                    ? Constants.SIZE_TEXT_LABEL_FOCUS
                                    : Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_LABEL
                    text: qsTranslate("PageDefinitionsApp",
                                      "STR_NETWORK_TITLE") + controler.autoTr
                    Accessible.role: Accessible.TitleBar
                    Accessible.name: text
                    Keys.onPressed: {
                        handleKeyPressed(event.key, dateAppNetwork)
                    }
                    KeyNavigation.tab: checkboxSystemProxy.visible ? checkboxSystemProxy : checkboxProxy
                    KeyNavigation.down: checkboxSystemProxy.visible ? checkboxSystemProxy : checkboxProxy
                    KeyNavigation.right: checkboxSystemProxy.visible ? checkboxSystemProxy : checkboxProxy
                    KeyNavigation.backtab: checkboxAccelGraphics
                    KeyNavigation.up: checkboxAccelGraphics
                    KeyNavigation.left: checkboxAccelGraphics
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
                    height: checkboxProxy.height + checkboxSystemProxy.height
                            + boxAppAdress.height + Constants.SIZE_TEXT_V_SPACE
                    anchors.top: dateAppNetwork.bottom

                    CheckBox {
                        id: checkboxSystemProxy
                        text: qsTranslate(
                                  "PageDefinitionsApp",
                                  "STR_NETWORK_SYSTEM_PROXY_OP") + controler.autoTr
                        height: 25
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        checked: false
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxSystemProxy)
                        }
                        KeyNavigation.tab: checkboxProxy
                        KeyNavigation.down: checkboxProxy
                        KeyNavigation.right: checkboxProxy
                        KeyNavigation.backtab: dateAppNetwork
                        KeyNavigation.up: dateAppNetwork
                        KeyNavigation.left: dateAppNetwork
                    }

                    CheckBox {
                        id: checkboxProxy
                        text: qsTranslate("PageDefinitionsApp",
                                          "STR_NETWORK_PROXY_OP") + controler.autoTr
                        height: 25
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        anchors.top: checkboxSystemProxy.bottom
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        checked: false
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxProxy)
                        }
                        KeyNavigation.tab: checkboxProxy.checked ? textFieldAdress : checkboxAutProxy
                        KeyNavigation.down: checkboxProxy.checked ? textFieldAdress : checkboxAutProxy
                        KeyNavigation.right: checkboxProxy.checked ? textFieldAdress : checkboxAutProxy
                        KeyNavigation.backtab: checkboxSystemProxy
                        KeyNavigation.up: checkboxSystemProxy
                        KeyNavigation.left: checkboxSystemProxy
                    }
                    Item {
                        id: boxAppAdress
                        width: 120
                        height: textFieldAdress.height
                        anchors.top: checkboxProxy.bottom
                        x: Constants.SIZE_TEXT_FIELD_H_SPACE

                        TextField {
                            id: textFieldAdress
                            width: parent.width
                            font.italic: textFieldAdress.text === "" ? true : false
                            font.bold: activeFocus
                            placeholderText: qsTranslate(
                                                 "PageDefinitionsApp",
                                                 "STR_NETWORK_PROXY_ADDRESS_OP") + controler.autoTr
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            enabled: checkboxProxy.checked
                            opacity: checkboxProxy.checked ? 1.0 : Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                            validator: RegExpValidator {
                                //validates IPV4/IPV6 host
                                regExp: /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z]|[a-zA-Z][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\-]*[A-Za-z0-9])$|^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*/
                            }
                            Accessible.role: Accessible.EditableText
                            Accessible.name: text
                            KeyNavigation.tab: textFieldPort
                            KeyNavigation.down: textFieldPort
                            KeyNavigation.right: textFieldPort
                            KeyNavigation.backtab: checkboxProxy
                            KeyNavigation.up: checkboxProxy
                            KeyNavigation.left: checkboxProxy
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
                            font.italic: textFieldPort.text === "" ? true : false
                            font.bold: activeFocus
                            placeholderText: qsTranslate(
                                                 "PageDefinitionsApp",
                                                 "STR_NETWORK_PROXY_PORT_OP") + controler.autoTr
                            validator: RegExpValidator {
                                regExp: /[0-9]+/
                            }
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            clip: true
                            maximumLength: 5
                            enabled: checkboxProxy.checked
                            opacity: checkboxProxy.checked ? 1.0 : Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                            Accessible.role: Accessible.EditableText
                            Accessible.name: text
                            KeyNavigation.tab: checkboxAutProxy
                            KeyNavigation.down: checkboxAutProxy
                            KeyNavigation.right: checkboxAutProxy
                            KeyNavigation.backtab: textFieldAdress
                            KeyNavigation.up: textFieldAdress
                            KeyNavigation.left: textFieldAdress
                        }
                    }
                    CheckBox {
                        id: checkboxAutProxy
                        x: parent.width * 0.40
                        text: qsTranslate("PageDefinitionsApp",
                                          "STR_NETWORK_AUTH_OP") + controler.autoTr
                        height: 25
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        anchors.top: checkboxSystemProxy.bottom
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        checked: false
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        // If this component is changed, the workaround to scroll the page automatically with
                        // keyboard navigation has to be updated also;
                        // this is located in PageDefinitionsApp.qml - function handleKeyPressed
                        Keys.onPressed: {
                            handleKeyPressed(event.key, checkboxAutProxy)
                        }
                        KeyNavigation.tab: checkboxAutProxy.checked? textFieldAutUser: textReader
                        KeyNavigation.down: checkboxAutProxy.checked? textFieldAutUser: textReader
                        KeyNavigation.right: checkboxAutProxy.checked? textFieldAutUser: textReader
                        KeyNavigation.backtab: textFieldPort
                        KeyNavigation.up: textFieldPort
                        KeyNavigation.left: textFieldPort
                    }
                    Item {
                        id: boxAppAutUser
                        width: parent.width * 0.3 - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        height: textFieldAutUser.height
                        anchors.top: checkboxAutProxy.bottom
                        anchors.left: checkboxAutProxy.left
                        anchors.leftMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                        TextField {
                            id: textFieldAutUser
                            width: parent.width
                            font.italic: textFieldAutUser.text === "" ? true : false
                            placeholderText: qsTranslate(
                                                 "PageDefinitionsApp",
                                                 "STR_NETWORK_AUTH_USERNAME_OP") + controler.autoTr
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.bold: activeFocus
                            clip: false
                            enabled: checkboxAutProxy.checked
                            opacity: checkboxAutProxy.checked ? 1.0 : Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                            Accessible.role: Accessible.EditableText
                            Accessible.name: text
                            KeyNavigation.tab: textFieldAutPass
                            KeyNavigation.down: textFieldAutPass
                            KeyNavigation.right: textFieldAutPass
                            KeyNavigation.backtab: checkboxAutProxy
                            KeyNavigation.up: checkboxAutProxy
                            KeyNavigation.left: checkboxAutProxy
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
                            font.italic: textFieldAutPass.text === "" ? true : false
                            font.bold: activeFocus
                            placeholderText: qsTranslate(
                                                 "PageDefinitionsApp",
                                                 "STR_NETWORK_AUTH_PASSWORD_OP") + controler.autoTr
                            echoMode: TextInput.Password
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            clip: false
                            enabled: checkboxAutProxy.checked
                            opacity: checkboxAutProxy.checked ? 1.0 : Constants.OPACITY_DEFINITIONS_APP_OPTION_DISABLED
                            Accessible.role: Accessible.EditableText
                            Accessible.name: text
                            // If this component is changed, the workaround to scroll the page automatically with
                            // keyboard navigation has to be updated also;
                            // this is located in PageDefinitionsApp.qml - function handleKeyPressed
                            Keys.onPressed: {
                                handleKeyPressed(event.key,textFieldAutPass)
                            }
                            KeyNavigation.tab: textReader
                            KeyNavigation.down: textReader
                            KeyNavigation.right: textReader
                            KeyNavigation.backtab: textFieldAutUser
                            KeyNavigation.up: textFieldAutUser
                            KeyNavigation.left: textFieldAutUser
                        }
                    }
                }
            }
        }
    }
}
