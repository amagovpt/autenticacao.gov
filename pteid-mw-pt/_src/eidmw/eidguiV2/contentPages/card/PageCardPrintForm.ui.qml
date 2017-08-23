import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.0
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
    property alias propertySwitchPdfSign: switchPdfSign

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
        title: "Escolha o ficheiro de destino"
        nameFilters: ["Images (*.pdf)", "All files (*)"]
    }

    Item {
        id: rowSelectData
        width: parent.width
        height: 4 * Constants.HEIGHT_SWITCH_COMPONENT
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
            color: Constants.COLOR_TEXT_LABEL
            height: Constants.SIZE_TEXT_LABEL
            text: "Configurações"
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
                text: "Informações Básicas"
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
            }
            Switch {
                id: switchAdditional
                height: Constants.HEIGHT_SWITCH_COMPONENT
                anchors.top: switchBasic.bottom
                text: "Informações Adicionais"
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
            }
            Switch {
                id: switchAddress
                height: Constants.HEIGHT_SWITCH_COMPONENT
                anchors.top: switchAdditional.bottom
                text: "Morada"
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
            }
            Switch {
                id: switchNotes
                height: Constants.HEIGHT_SWITCH_COMPONENT
                anchors.top: switchAddress.bottom
                text: "Notas Pessoais"
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
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
            color: Constants.COLOR_TEXT_LABEL
            height: Constants.SIZE_TEXT_LABEL
            text: "Opções"
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
                text: "Documento PDF assinado"
                enabled: true
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
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
                text: "Imprimir"
                enabled: false
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter

            }
        }
        Item {
            id: rectPdf
            width: parent.width / 2
            height: parent.height
            anchors.left: rectPrint.right
            Button {
                id: buttonPdf
                text: "Gerar PDF"
                enabled: false
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter

            }
        }

    }
}
