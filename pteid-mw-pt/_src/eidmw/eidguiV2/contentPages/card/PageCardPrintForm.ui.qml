import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0
import eidguiV2 1.0

import "../../scripts/Constants.js" as Constants
import "../../components" as Components


Item {
    anchors.fill: parent

    property alias propertyBusyIndicator : busyIndication
    property alias propertyFileDialogOutput: fileDialogOutput
    property alias propertyButtonPrint: buttonPrint
    property alias propertyButtonPdf: buttonPdf
    property alias propertySwitchBasic: switchBasic
    property alias propertySwitchAdditional: switchAdditional
    property alias propertySwitchAddress: switchAddress
    property alias propertySwitchNotes: switchNotes
    property alias propertySwitchPrintDate: switchPrintDate
    property alias propertySwitchPdfSign: switchPdfSign
    property alias propertyMainItem: titleSelectData

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

    FileSaveDialog {
        id: fileDialogOutput
        title: qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
        nameFilters: ["PDF (*.pdf)", "All files (*)"]
    }

    Item {
        id: rowSelectData
        width: parent.width
        height: 5 * Constants.HEIGHT_SWITCH_COMPONENT
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        DropShadow {
            anchors.fill: rectselectData
            horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
            verticalOffset: Constants.FORM_SHADOW_V_OFFSET
            radius: Constants.FORM_SHADOW_RADIUS
            samples: Constants.FORM_SHADOW_SAMPLES
            color: Constants.COLOR_FORM_SHADOW
            source: rectselectData
            spread: Constants.FORM_SHADOW_SPREAD
            opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
        }
        RectangularGlow {
            anchors.fill: rectselectData
            glowRadius: Constants.FORM_GLOW_RADIUS
            spread: Constants.FORM_GLOW_SPREAD
            color: Constants.COLOR_FORM_GLOW
            cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
            opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
        }

        Text {
            id: titleSelectData
            x: Constants.SIZE_TEXT_FIELD_H_SPACE
            font.pixelSize: Constants.SIZE_TEXT_LABEL
            font.family: lato.name
            font.bold: titleSelectData.focus ? true : false
            color: Constants.COLOR_TEXT_LABEL
            height: Constants.SIZE_TEXT_LABEL
            text: qsTranslate("PageCardPrint","STR_PRINT_SETTINGS")
            Accessible.role: Accessible.Grouping
            Accessible.name: text
            KeyNavigation.tab: switchBasic
        }

        Rectangle {
            id: rectselectData
            width: parent.width
            height: parent.height
            color: "white"
            anchors.top: titleSelectData.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            Switch {
                id: switchBasic
                height: Constants.HEIGHT_SWITCH_COMPONENT
                text: qsTranslate("GAPI","STR_BASIC_INFORMATION_UPPERCASE")
                font.bold: focus ? true : false
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                Accessible.role: Accessible.CheckBox
            }
            Switch {
                id: switchAdditional
                height: Constants.HEIGHT_SWITCH_COMPONENT
                anchors.top: switchBasic.bottom
                text: qsTranslate("GAPI","STR_ADDITIONAL_INFORMATION_UPPERCASE")
                font.bold: focus ? true : false
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                Accessible.role: Accessible.CheckBox
            }
            Switch {
                id: switchAddress
                height: Constants.HEIGHT_SWITCH_COMPONENT
                anchors.top: switchAdditional.bottom
                text: qsTranslate("GAPI","STR_ADDRESS_UPPERCASE")
                font.bold: focus ? true : false
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                Accessible.role: Accessible.CheckBox
            }
            Switch {
                id: switchNotes
                height: Constants.HEIGHT_SWITCH_COMPONENT
                anchors.top: switchAddress.bottom
                text: qsTranslate("GAPI","STR_PERSONAL_NOTES_UPPERCASE")
                font.bold: focus ? true : false
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                Accessible.role: Accessible.CheckBox
            }
            Switch {
                id: switchPrintDate
                height: Constants.HEIGHT_SWITCH_COMPONENT
                anchors.top: switchNotes.bottom
                text: qsTranslate("GAPI","STR_PRINT_DATE")
                font.bold: focus ? true : false
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                KeyNavigation.tab: titleOptions
                Accessible.role: Accessible.CheckBox
            }
        }
    }

    Item {
        id: rowSelectOptions
        width: parent.width
        height: Constants.HEIGHT_SWITCH_COMPONENT
        anchors.top: rowSelectData.bottom
        anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE

        DropShadow {
            anchors.fill: rectOptions
            horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
            verticalOffset: Constants.FORM_SHADOW_V_OFFSET
            radius: Constants.FORM_SHADOW_RADIUS
            samples: Constants.FORM_SHADOW_SAMPLES
            color: Constants.COLOR_FORM_SHADOW
            source: rectOptions
            spread: Constants.FORM_SHADOW_SPREAD
            opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
        }
        RectangularGlow {
            anchors.fill: rectOptions
            glowRadius: Constants.FORM_GLOW_RADIUS
            spread: Constants.FORM_GLOW_SPREAD
            color: Constants.COLOR_FORM_GLOW
            cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
            opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
        }

        Text {
            id: titleOptions
            x: Constants.SIZE_TEXT_FIELD_H_SPACE
            font.pixelSize: Constants.SIZE_TEXT_LABEL
            font.family: lato.name
            font.bold: focus ? true : false
            color: Constants.COLOR_TEXT_LABEL
            height: Constants.SIZE_TEXT_LABEL
            text: qsTranslate("PageCardPrint","STR_PRINT_SIGN_SETTINGS")
            Accessible.role: Accessible.Grouping
            Accessible.name: text
            KeyNavigation.tab: switchPdfSign
        }

        Rectangle {
            id: rectOptions
            width: parent.width
            height: parent.height
            color: "white"
            anchors.top: titleOptions.bottom
            anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
            Switch {
                id: switchPdfSign
                height: Constants.HEIGHT_SWITCH_COMPONENT
                text: qsTranslate("PageCardPrint","STR_PRINT_SIGN_PDF")
                font.bold: focus ? true : false
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
                Accessible.role: Accessible.CheckBox
                KeyNavigation.tab: buttonPrint.enabled ? (buttonPrint) : (buttonPdf.enabled ? buttonPdf : propertyMainItem)
            }
        }
    }
    Item {
        id: rowButtons
        width: parent.width
        height: Constants.HEIGHT_BOTTOM_COMPONENT
        anchors.top: rowSelectOptions.bottom
        anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE

        Item {
            id: rectPrint
            width: parent.width / 2
            height: parent.height
            Button {
                id: buttonPrint
                text: qsTranslate("PageCardPrint","STR_PRINT_BUTTON")
                enabled: {
                    (switchBasic.checked || switchAdditional.checked || switchAddress.checked 
                              || switchNotes.checked) && !switchPdfSign.checked
                }
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus ? true : false
                anchors.horizontalCenter: parent.horizontalCenter
                KeyNavigation.tab: buttonPdf.enabled ? buttonPdf : titleSelectData

            }
        }
        Item {
            id: rectPdf
            width: parent.width / 2
            height: parent.height
            anchors.left: rectPrint.right
            Button {
                id: buttonPdf
                text: qsTranslate("PageCardPrint","STR_PRINT_PDF_BUTTON")
                enabled: {
                    switchBasic.checked || switchAdditional.checked 
                         || switchAddress.checked || switchNotes.checked
                }
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                highlighted: activeFocus ? true : false
                anchors.horizontalCenter: parent.horizontalCenter
                KeyNavigation.tab: titleSelectData

            }
        }

    }
}
