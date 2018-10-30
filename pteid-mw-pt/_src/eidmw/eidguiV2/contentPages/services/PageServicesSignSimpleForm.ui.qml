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
    property bool cardLoaded: false

    property alias propertyBusyIndicator: busyIndicator
    property alias propertyPDFPreview: pdfPreviewArea
    property alias propertyFileDialog: fileDialog
    property alias propertyFileDialogOutput: fileDialogOutput
    property alias propertyFileDialogCMDOutput: fileDialogCMDOutput
    property alias propertyMouseAreaRectMain: mouseAreaRectMain
    property alias propertyButtonRemove: buttonRemove
    property alias propertyButtonSignWithCC: button_signCC
    property alias propertyButtonSignCMD: button_signCMD
    property alias propertyDropArea: dropArea

    property alias propertyTextSpinBox: textSpinBox
    property alias propertySpinBoxControl: spinBoxControl
    property alias propertyCheckLastPage: checkLastPage

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
        height: parent.height - rowBottom.height - rectSignPageOptions.height - 2 * Constants.SIZE_ROW_V_SPACE

        // Expanded menu need a Horizontal space to Main Menu
        x: Constants.SIZE_ROW_H_SPACE

        FileDialog {
            id: fileDialog
            title: qsTranslate("Popup File","STR_POPUP_FILE_INPUT")
            folder: shortcuts.home
            modality : Qt.WindowModal
            selectMultiple: false
            nameFilters: [ "PDF document (*.pdf)", "All files (*)" ]
            Component.onCompleted: visible = false
        }
        FileSaveDialog {
            id: fileDialogOutput
            title: qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
            nameFilters: ["Images (*.pdf)", "All files (*)"]
        }
        FileSaveDialog {
            id: fileDialogCMDOutput
            title: qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
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
                           qsTranslate("PageServicesSign","STR_SIGN_TITLE_SIGN") :
                           qsTranslate("PageServicesSign","STR_SIGN_TITLE_FILE")
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
                    text: qsTranslate("PageServicesSign","STR_SIGN_DROP")
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
                DropArea {
                    id: dropArea;
                    anchors.fill: parent;
                    z: 2
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
        id: rectSignPageOptions
        width: parent.width - Constants.SIZE_ROW_H_SPACE
        height: Constants.HEIGHT_BOTTOM_COMPONENT
        anchors.top: rowMain.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        // Expanded menu need a Horizontal space to Main Menu
        x: Constants.SIZE_ROW_H_SPACE

        Item {
            id: itemCheckPage
            width: parent.width * 0.30
            height: parent.height
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            Text{
                id: pageText
                x: 11
                y: 8
                text: qsTranslate("PageServicesSign","STR_SIGN_PAGE") + ":"
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                color: Constants.COLOR_MAIN_PRETO
                font.capitalization: Font.MixedCase
                opacity: fileLoaded && !checkLastPage.checked
                         ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
            }

            SpinBox {
                id: spinBoxControl
                y: 0
                from: 1
                to: 10000
                stepSize: 1
                value: 1
                anchors.left: pageText.right
                width: 100
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: fileLoaded && !checkLastPage.checked
                editable:  fileLoaded ? true : false
                validator: RegExpValidator { regExp: /[1-9][0-9]*/ }
                contentItem: TextInput {
                    id: textSpinBox
                    z: 2
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    color: Constants.COLOR_MAIN_PRETO
                    opacity: fileLoaded && !checkLastPage.checked
                             ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter

                    readOnly: !spinBoxControl.editable
                    validator: spinBoxControl.validator
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    maximumLength: 5 //pages go from 1 to 99999
                }

                up.indicator: Rectangle {
                    x: spinBoxControl.mirrored ? 0 : parent.width - width
                    y: Constants.SIZE_ARROW_OFFSET
                    height: parent.height
                    implicitWidth: 20
                    implicitHeight: parent.height

                    Text {
                        text: ">"
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_ARROW_INDICATOR
                        color: Constants.COLOR_MAIN_BLUE
                        font.bold : true
                        opacity: fileLoaded && !checkLastPage.checked
                                 ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                down.indicator: Rectangle {
                    visible: false
                    x: spinBoxControl.mirrored ? parent.width - width : 0
                    y: Constants.SIZE_ARROW_OFFSET
                    height: parent.height
                    implicitWidth: 20
                    implicitHeight: parent.height

                    Text {
                        text: "<"
                        font.family: lato.name
                        font.pixelSize:  Constants.SIZE_ARROW_INDICATOR
                        color: Constants.COLOR_MAIN_BLUE
                        font.bold : true
                        opacity: fileLoaded && !checkLastPage.checked
                                 ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
        Item {
            id: itemLastPage
            width: parent.width * 0.4
            height: parent.height
            anchors.left: itemCheckPage.right
            anchors.top: parent.top
            Switch {
                id: checkLastPage
                text: qsTranslate("PageServicesSign","STR_SIGN_LAST")
                height: Constants.HEIGHT_SWITCH_COMPONENT
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
                enabled: fileLoaded
            }
        }
    }

    Item {
        id: rowBottom
        width: parent.width - Constants.SIZE_ROW_H_SPACE
        height: Constants.HEIGHT_SIGN_BOTTOM_COMPONENT
        anchors.top: rectSignPageOptions.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE
        x: Constants.SIZE_ROW_H_SPACE

        Item{
            id: rectSignLeft
            width: parent.width  * 0.50 - Constants.SIZE_ROW_H_SPACE
            height: parent.height

            Button {
                id: buttonRemove
                x: 140
                text: qsTranslate("PageServicesSign","STR_SIGN_REMOVE_BUTTON")
                width: Constants.WIDTH_BUTTON
                height: parent.height
                enabled: fileLoaded
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                anchors.left: parent.left
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
                text: qsTranslate("PageServicesSign","STR_SIGN_SIGN_BUTTON") + "\n"
                      + qsTranslate("PageServicesSign","STR_SIGN_CARD_BUTTON")
                width: Constants.WIDTH_BUTTON
                height: parent.height
                enabled: fileLoaded && cardLoaded
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
            }
            Button {
                id: button_signCMD
                text: qsTranslate("PageServicesSign","STR_SIGN_SIGN_BUTTON") + "\n"
                      + qsTranslate("PageServicesSign","STR_SIGN_CMD_BUTTON")
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
