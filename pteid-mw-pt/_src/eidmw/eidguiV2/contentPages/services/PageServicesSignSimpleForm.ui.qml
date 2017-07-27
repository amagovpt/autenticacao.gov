import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.0
import QtGraphicalEffects 1.0
import eidguiV2 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent

    property variant filesArray:[]
    property bool fileLoaded: false

    property alias propertyBusyIndicator: busyIndicator
    property alias propertyPDFPreview: pdfPreviewArea
    property alias propertyFileDialog: fileDialog
    property alias propertyFileDialogOutput: fileDialogOutput
    property alias propertyMouseAreaRectMain: mouseAreaRectMain
    property alias propertyButtonRemove: buttonRemove
    property alias propertyButtonSignWithCC: button_signCC
    property alias propertyDropArea: dropArea

    BusyIndicator {
       id: busyIndicator
       running: false
       anchors.centerIn: parent
       // BusyIndicator should be on top of all other content
       z: 1
    }

    Item {
        id: rowMain
        width: parent.width - Constants.SIZE_ROW_H_SPACE
        height: parent.height - Constants.HEIGHT_BOTTOM_COMPONENT

        // Expanded menu need a Horizontal space to Main Menu
        x: Constants.SIZE_ROW_H_SPACE

        DropArea {
            id: dropArea;
            anchors.fill: parent;
        }

        FileDialog {
            id: fileDialog
            title: "Escolha o ficheiro para assinar"
            folder: shortcuts.home
            modality : Qt.WindowModal
            selectMultiple: false
            nameFilters: [ "PDF document (*.pdf)", "All files (*)" ]
            Component.onCompleted: visible = false
        }
        FileSaveDialog {
            id: fileDialogOutput
            title: "Escolha o ficheiro de destino"
            nameFilters: ["Images (*.pdf)", "All files (*)"]
        }

        Item{
            id: rectMain
            width: parent.width
            height: parent.height

            DropShadow {
                anchors.fill: rectPre
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectPre
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectPre
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            Text {
                id: titlePre
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: fileLoaded ?
                           "Selecione o lugar da assinatura" :
                           "Selecione o ficheiro"
            }

            Rectangle {
                id: rectPre
                width: parent.width
                height: parent.height - titlePre.height - Constants.SIZE_TEXT_V_SPACE
                color: "white"
                anchors.top: titlePre.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Text {
                    id: textDragMsgImg
                    anchors.fill: parent
                    text: "Arraste para esta zona o ficheiro a assinar \nou\n clique para procurar o ficheiro"
                    font.bold: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    color: Constants.COLOR_TEXT_LABEL
                    visible: !fileLoaded
                    font.family: lato.name
                    z: 1
                }
                Components.PDFPreview {
                    anchors.fill: parent
                    id: pdfPreviewArea
                    propertyDragSigRect.visible: true
                    propertyReducedChecked : false
                }
            }
            MouseArea {
                id: mouseAreaRectMain
                anchors.fill: parent
                enabled: !fileLoaded
            }
        }
    }

    Item {
        id: rowBottom
        width: parent.width - Constants.SIZE_ROW_H_SPACE
        height: Constants.HEIGHT_BOTTOM_COMPONENT
        anchors.top: rowMain.bottom
        x: Constants.SIZE_ROW_H_SPACE

        Item{
            id: rectSignLeft
            width: parent.width  * 0.50 - Constants.SIZE_ROW_H_SPACE
            height: parent.height

            Button {
                id: buttonRemove
                x: 140
                text: "Remover ficheiro"
                anchors.rightMargin: 0
                y: 5
                width: Constants.WIDTH_BUTTON
                height: parent.height
                enabled: fileLoaded
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Item{
            id: rectSignRight
            width: parent.width * 0.50
            height: parent.height
            anchors.left: rectSignLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            Button {
                id: button_signCC
                text: "Assinar com CC"
                y: 5
                width: Constants.WIDTH_BUTTON
                height: parent.height
                enabled: fileLoaded
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
            }
            Button {
                id: button_signCMD
                text: "Assinar com CMD"
                y: 5
                width: Constants.WIDTH_BUTTON
                height: parent.height
                enabled: fileLoaded
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.right: parent.right
            }
        }
    }
}
