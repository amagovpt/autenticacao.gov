import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.0
import QtGraphicalEffects 1.0


/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

// Work around to QTCREATORBUG-18321
import QtQuick.Templates 2.0 as T

Item {
    anchors.fill: parent

    property variant filesArray:[]
    property bool fileLoaded: false
    property alias propertyFileDialog: fileDialog
    property alias propertyMouseAreaPreCustom: mouseAreaPreCustom
    property alias propertyButtonAdd: buttonAdd
    property alias propertyButtonRemove: buttonRemove
    property alias propertyDropArea: dropArea
    property alias propertyRadioButtonDefault: radioButtonDefault
    property alias propertyRadioButtonCustom: radioButtonCustom

    Item {
        id: rowMain
        width: parent.width
        height: parent.height

        T.ButtonGroup {
            id: radioGroup
        }

        Item{
            id: rectTop
            width: parent.width
            height: parent.height * 0.5
                    - ((Constants.HEIGHT_BOTTOM_COMPONENT + Constants.SIZE_ROW_V_SPACE) * 0.5)
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            DropShadow {
                anchors.fill: rectPreDefault
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectPreDefault
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectPreDefault
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            RadioButton {
                id: radioButtonDefault
                text: "Usar assinatura padrão"
                T.ButtonGroup.group: radioGroup
                checked: true
                opacity: enabled ? 1.0 : Constants.OPACITY_SIGNATURE_TEXT_DISABLED

                contentItem: Text {
                    text: radioButtonDefault.text
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    color: Constants.COLOR_TEXT_LABEL
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: radioButtonDefault.indicator.width + radioButtonDefault.spacing
                }
            }

            Rectangle {
                id: rectPreDefault
                width: parent.width
                height: parent.height - radioButtonDefault.height
                color: "white"
                anchors.top: radioButtonDefault.bottom

                Image {
                    id: imagePreDefault
                    width: parent.width
                    height: parent.height
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    source: "../../images/CCdemo.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: true
                    opacity: radioButtonDefault.checked ? 1 : Constants.OPACITY_SIGNATURE_IMAGE_DISABLED
                }

            }
        }

        Item{
            id: rectBottom
            width: parent.width
            height: parent.height * 0.5
                    - ((Constants.HEIGHT_BOTTOM_COMPONENT + Constants.SIZE_ROW_V_SPACE) * 0.5)
                    + Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.top: rectTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE


            FileDialog {
                id: fileDialog
                title: "Escolha o ficheiro da assinatura personalizada"
                folder: shortcuts.home
                modality : Qt.WindowModal
                selectMultiple: false
                nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]
                Component.onCompleted: visible = false
            }
            DropShadow {
                anchors.fill: rectPreCustom
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectPreCustom
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectPreCustom
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            RadioButton {
                id: radioButtonCustom
                text: "Usar assinatura personalizada"
                T.ButtonGroup.group: radioGroup
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                enabled: fileLoaded
                opacity: enabled ? 1.0 : Constants.OPACITY_SIGNATURE_TEXT_DISABLED

                contentItem: Text {
                    text: radioButtonCustom.text
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    color: Constants.COLOR_TEXT_LABEL
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: radioButtonCustom.indicator.width + radioButtonCustom.spacing
                }
            }
            Rectangle {
                id: rectPreCustom
                width: parent.width
                height: parent.height - radioButtonCustom.height
                color: "white"
                anchors.top: radioButtonCustom.bottom

                Image {
                    id: imagePreCustom
                    width: parent.width
                    height: parent.height - Constants.HEIGHT_BOTTOM_COMPONENT
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    source: "../../images/CCdemo_custom.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: fileLoaded
                    opacity: radioButtonCustom.checked ? 1 : Constants.OPACITY_SIGNATURE_IMAGE_DISABLED
                }
                Text {
                    id: textDragMsgImg
                    width: parent.width
                    height: parent.height - Constants.HEIGHT_BOTTOM_COMPONENT
                    text:
                        "Arraste para esta zona o ficheiro da assinatura personalizada
ou
clique para procurar o ficheiro
( Dimensão máxima da imagem: 185 x 41 px)"
                    font.bold: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    color: Constants.COLOR_TEXT_LABEL
                    visible: !fileLoaded
                    font.family: lato.name
                    DropArea {
                        id: dropArea;
                        anchors.fill: parent;
                    }
                }
                MouseArea {
                    id: mouseAreaPreCustom
                    width: parent.width
                    height: parent.height - Constants.HEIGHT_BOTTOM_COMPONENT
                }
                Item{
                    id: rectSignLeft
                    width: parent.width  * 0.50 - Constants.SIZE_ROW_H_SPACE
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    anchors.top: imagePreCustom.bottom
                    x: Constants.SIZE_ROW_H_SPACE / 2

                    Button {
                        id: buttonRemove
                        text: "Remover assinatura"
                        width: Constants.WIDTH_BUTTON
                        height:Constants.HEIGHT_BOTTOM_COMPONENT
                        anchors.right: parent.right
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Item{
                    id: rectSignRight
                    width: parent.width * 0.50 - Constants.SIZE_ROW_H_SPACE
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    anchors.top: imagePreCustom.bottom
                    anchors.left: rectSignLeft.right
                    anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

                    Button {
                        id: buttonAdd
                        text: "Adicionar assinatura"
                        width: Constants.WIDTH_BUTTON
                        height:Constants.HEIGHT_BOTTOM_COMPONENT
                        anchors.right: parent.right
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }
    }
}
