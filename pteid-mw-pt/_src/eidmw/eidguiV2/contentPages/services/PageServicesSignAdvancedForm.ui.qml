import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.0
import QtGraphicalEffects 1.0
import eidguiV2 1.0


/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    id: mainItem
    anchors.fill: parent

    property variant filesArray: []
    property bool fileLoaded: false

    property alias propertyBusyIndicator: busyIndicator
    property alias propertyPDFPreview: pdfPreviewArea
    property alias propertyFileDialog: fileDialog
    property alias propertyFileDialogOutput: fileDialogOutput
    property alias propertyMouseAreaRectMainRigh: mouseAreaRectMainRigh
    property alias propertyMouseAreaItemOptionsFiles: mouseAreaItemOptionsFiles
    property alias propertyTextDragMsgListView: textDragMsgListView
    property alias propertyListViewFiles: listViewFiles
    property alias propertyFilesListViewScroll: filesListViewScroll
    property alias propertyButtonAdd: buttonAdd
    property alias propertyButtonRemoveAll: buttonRemoveAll
    property alias propertyButtonSignWithCC: button_signCC
    property alias propertyDropArea: dropArea
    property alias propertyTextFieldReason: textFieldReason
    property alias propertyTextFieldLocal: textFieldLocal
    property alias propertySwitchSignTemp: switchSignTemp
    property alias propertyCheckSignReduced: checkSignReduced
    property alias propertyRadioButtonPADES: radioButtonPADES
    property alias propertyRadioButtonXADES: radioButtonXADES
    property alias propertyMouseAreaToolTipPades: mouseAreaToolTipPades
    property alias propertyMouseAreaToolTipXades: mouseAreaToolTipXades
    // Calculate ToolTip Position
    property int propertyMouseAreaToolTipPadesX: Constants.SIZE_ROW_H_SPACE
                                                 + Constants.SIZE_TEXT_FIELD_H_SPACE
                                                 + textFormatSign.width + 10
                                                 + radioButtonPADES.width
                                                 + rectToolTipPades.width * 0.5
    property int propertyMouseAreaToolTipXadesX: Constants.SIZE_ROW_H_SPACE
                                                 + Constants.SIZE_TEXT_FIELD_H_SPACE
                                                 + textFormatSign.width + 10
                                                 + radioButtonPADES.width + rectToolTipPades.width
                                                 + 10 + radioButtonXADES.width
                                                 + rectToolTipXades.width * 0.5
    property int propertyMouseAreaToolTipY: rectMainLeftFile.height
    property alias propertyTextSpinBox: textSpinBox
    property alias propertySpinBoxControl: spinBoxControl

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
        height: parent.height

        // Expanded menu need a Horizontal space to Main Menu
        x: Constants.SIZE_ROW_H_SPACE

        DropArea {
            id: dropArea
            anchors.fill: parent
        }

        FileDialog {
            id: fileDialog
            title: "Escolha o ficheiro para assinar"
            folder: shortcuts.home
            modality: Qt.WindowModal
            selectMultiple: true
            nameFilters: ["PDF document (*.pdf)", "All files (*)"]
            Component.onCompleted: visible = false
        }

/*        FileDialog {
            id: fileDialogOutput
            title: "Escolha o ficheiro de destino"
            folder: shortcuts.home
            modality: Qt.WindowModal
            selectExisting: false
            nameFilters: ["PDF document (*.pdf)", "All files (*)"]
            Component.onCompleted: visible = false
        }
        */

        FileSaveDialog {
            id: fileDialogOutput
            title: "Escolha o ficheiro de destino"
            nameFilters: ["Images (*.pdf)", "All files (*)"]

        }

        Item {
            id: rectMainLeftFile
            width: parent.width * 0.5 - Constants.SIZE_ROW_H_SPACE
            height: mainItem.height - rectMainLeftOptions.height - 4 * Constants.SIZE_ROW_V_SPACE

            DropShadow {
                anchors.fill: rectFile
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectFile
                spread: Constants.FORM_SHADOW_SPREAD
                opacity: Constants.FORM_SHADOW_OPACITY_FORM_EFFECT
            }
            RectangularGlow {
                anchors.fill: rectFile
                glowRadius: Constants.FORM_GLOW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
                opacity: Constants.FORM_GLOW_OPACITY_FORM_EFFECT
            }

            Text {
                id: titleSelectFile
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Selecione os ficheiros a assinar"
            }
            Rectangle {
                id: rectFile
                width: parent.width
                height: parent.height

                color: "white"
                anchors.top: titleSelectFile.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter

                Item {
                    id: itemOptionsFiles
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: parent.height - itemBottonsFiles.height - Constants.SIZE_ROW_V_SPACE
                    anchors.horizontalCenter: parent.horizontalCenter

                    ListView {
                        id: listViewFiles
                        y: Constants.SIZE_TEXT_V_SPACE
                        width: parent.width
                        height: parent.height
                        clip: true
                        spacing: Constants.SIZE_LISTVIEW_SPACING
                        boundsBehavior: Flickable.StopAtBounds
                        model: filesModel
                        delegate: listViewFilesDelegate

                        ScrollBar.vertical: ScrollBar {
                            id: filesListViewScroll
                            hoverEnabled: true
                            active: hovered || pressed
                        }
                        Text {
                            id: textDragMsgListView
                            anchors.fill: parent
                            text: "Arraste para esta zona o ficheiro a assinar \nou\n clique para procurar o ficheiro"
                            font.bold: true
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: Constants.SIZE_TEXT_BODY
                            font.family: lato.name
                            color: Constants.COLOR_TEXT_LABEL
                            visible: !fileLoaded
                        }
                    }
                    MouseArea {
                        id: mouseAreaItemOptionsFiles
                        width: parent.width
                        height: parent.height
                        enabled: !fileLoaded
                    }
                }
                Item {
                    id: itemBottonsFiles
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: itemOptionsFiles.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                    Button {
                        id: buttonAdd
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        text: "Adicionar ficheiro"
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }
                    Button {
                        id: buttonRemoveAll
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        anchors.right: itemBottonsFiles.right
                        text: "Remover todos"
                        enabled: fileLoaded
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }
                }
            }
        }
        Item {
            id: rectMainLeftOptions
            width: parent.width * 0.5 - Constants.SIZE_ROW_H_SPACE

            height: titleConf.height + rectOptions.height

            anchors.top: rectMainLeftFile.bottom
            anchors.topMargin: 3 * Constants.SIZE_ROW_V_SPACE

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
                id: titleConf
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: "Configurações"
            }

            Rectangle {
                id: rectOptions
                width: parent.width
                height: itemOptions.height
                color: "white"
                anchors.top: titleConf.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Item {
                    id: itemOptions
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: Constants.SIZE_TEXT_V_SPACE + rectFormatOptions.height
                            + textFieldReason.height + textFieldLocal.height
                            + switchSignTemp.height + switchSignAdd.height
                            + columAttributes.height + rowPreserv.height
                            + Constants.SIZE_TEXT_V_SPACE + Constants.SIZE_TEXT_V_SPACE

                    anchors.horizontalCenter: parent.horizontalCenter

                    Item {
                        id: rectFormatOptions
                        width: parent.width
                        height: radioButtonPADES.height
                        anchors.top: itemOptions.top
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        Text {
                            id: textFormatSign
                            text: "Formato"
                            rightPadding: 0
                            padding: 0
                            verticalAlignment: Text.AlignVCenter
                            color: Constants.COLOR_TEXT_BODY
                            height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                        }
                        RadioButton {
                            id: radioButtonPADES
                            anchors.left: textFormatSign.right
                            height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                            text: "No documento"
                            leftPadding: 0
                            rightPadding: 0
                            anchors.leftMargin: 10
                            enabled: fileLoaded
                            font.capitalization: Font.MixedCase
                            opacity: enabled ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                            contentItem: Text {
                                text: radioButtonPADES.text
                                leftPadding: 22
                                font.family: lato.name
                                font.pixelSize: Constants.SIZE_TEXT_LABEL
                                horizontalAlignment: Text.AlignRight
                                verticalAlignment: Text.AlignVCenter
                                color: Constants.COLOR_MAIN_PRETO
                            }
                        }
                        Item {
                            id: rectToolTipPades
                            width: Constants.SIZE_IMAGE_TOOLTIP
                            height: Constants.SIZE_IMAGE_TOOLTIP
                            anchors.leftMargin: 0
                            anchors.left: radioButtonPADES.right

                            Image {
                                anchors.fill: parent
                                antialiasing: true
                                fillMode: Image.PreserveAspectFit
                                source: "../../images/tooltip_grey.png"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            MouseArea {
                                id: mouseAreaToolTipPades
                                anchors.fill: parent
                                hoverEnabled: true
                            }
                        }
                        RadioButton {
                            id: radioButtonXADES
                            anchors.left: rectToolTipPades.right
                            height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                            text: "Em qualquer ficheiro"
                            anchors.leftMargin: 10
                            leftPadding: 0
                            rightPadding: 0
                            enabled: fileLoaded
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            opacity: enabled ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                            contentItem: Text {
                                text: radioButtonXADES.text
                                leftPadding: 22
                                font.family: lato.name
                                font.pixelSize: Constants.SIZE_TEXT_LABEL
                                horizontalAlignment: Text.AlignRight
                                verticalAlignment: Text.AlignVCenter
                                color: Constants.COLOR_MAIN_PRETO
                            }
                        }
                        Item {
                            id: rectToolTipXades
                            width: Constants.SIZE_IMAGE_TOOLTIP
                            height: Constants.SIZE_IMAGE_TOOLTIP
                            anchors.left: radioButtonXADES.right

                            Image {
                                anchors.fill: parent
                                antialiasing: true
                                fillMode: Image.PreserveAspectFit
                                source: "../../images/tooltip_grey.png"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            MouseArea {
                                id: mouseAreaToolTipXades
                                width: parent.width
                                height: parent.height
                                hoverEnabled: true
                            }
                        }
                    }

                    TextField {
                        id: textFieldReason
                        width: parent.width
                        font.italic: textFieldReason.text === "" ? true : false
                        anchors.top: rectFormatOptions.bottom
                        placeholderText: "Motivo?"
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    TextField {
                        id: textFieldLocal
                        width: parent.width
                        font.italic: textFieldLocal.text === "" ? true : false
                        anchors.top: textFieldReason.bottom
                        placeholderText: "Localidade?"
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        enabled: fileLoaded
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                    }
                    Switch {
                        id: switchSignTemp
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        anchors.top: textFieldLocal.bottom
                        text: "Adicionar selo temporal"
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    Switch {
                        id: switchSignAdd
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        anchors.top: switchSignTemp.bottom
                        text: "Adicionar atributos profissionais"
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                    }
                    Column {
                        id: columAttributes
                        anchors.top: switchSignAdd.bottom
                        width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        height: checkBox1.height + checkBox2.height
                        x: 6 * Constants.SIZE_TEXT_FIELD_H_SPACE
                        visible: switchSignAdd.checked

                        CheckBox {
                            id: checkBox1
                            text: "Engenheiro Civil"
                            height: 25
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                        }
                        CheckBox {
                            id: checkBox2
                            text: "Socio da Empresa Obras Prontas"
                            height: 25
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                        }
                    }
                    Row {
                        id: rowPreserv
                        anchors.top: columAttributes.bottom
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                        width: parent.width
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        spacing: 5
                        x: 40
                        visible: switchSignAdd.checked

                        Switch {
                            id: switchPreserv
                            text: ""
                            spacing: -10
                            anchors.verticalCenter: parent.verticalCenter
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            height: parent.height
                        }
                        Text {
                            id: textPreserv1
                            x: 35
                            text: "Preservar"
                            verticalAlignment: Text.AlignVCenter
                            anchors.verticalCenter: parent.verticalCenter
                            font.bold: false
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            color: Constants.COLOR_TEXT_BODY
                            height: parent.height
                        }
                        Text {
                            id: textPreserv2
                            text: "durante"
                            verticalAlignment: Text.AlignVCenter
                            anchors.verticalCenter: parent.verticalCenter
                            font.bold: false
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            color: Constants.COLOR_TEXT_BODY
                            visible: switchPreserv.checked
                            height: parent.height
                        }
                        ComboBox {
                            id: comboBoxPreserve
                            width: 80
                            anchors.verticalCenter: parent.verticalCenter
                            height: parent.height
                            // textPreservAnos.text is related with model values
                            // if model is changed textPreservAnos.text may be changed
                            model: ["0", "1", "3", "5", "10"]
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            visible: switchPreserv.checked
                        }
                        Text {
                            id: textPreservAnos
                            text: comboBoxPreserve.currentIndex === 1 ? "ano" : "anos"
                            verticalAlignment: Text.AlignVCenter
                            anchors.verticalCenter: parent.verticalCenter
                            font.bold: false
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            color: Constants.COLOR_TEXT_BODY
                            visible: switchPreserv.checked
                            height: parent.height
                        }
                    }
                }
            }
        }

        Item {
            id: rectMainRight
            width: parent.width * 0.5
            height: parent.height - rowBottom.height
            anchors.left: rectMainLeftFile.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

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
                text: "Pré-Visualização"
            }

            Rectangle {
                id: rectPre
                width: parent.width
                height: parent.height - titlePre.height - Constants.SIZE_TEXT_V_SPACE
                enabled: fileLoaded
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
                    propertyDragSigRect.visible: checkSignShow.checked
                    propertyReducedChecked : checkSignReduced.checked
                }
            }

            MouseArea {
                id: mouseAreaRectMainRigh
                anchors.fill: parent
                enabled: !fileLoaded
            }

        }
        Item {
            id: rowBottom
            width: rectMainRight.width
            height: 3 * Constants.HEIGHT_BOTTOM_COMPONENT
            anchors.top: rectMainRight.bottom
            anchors.left: rectMainRight.left

            Item {
                id: rectSignOptions
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.left: parent.left
                Item {
                    id: itemCheckSignShow
                    width: parent.width * 0.6
                    height: parent.height
                    anchors.top: parent.top
                    Switch {
                        id: checkSignShow
                        text: "Assinatura visível"
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        enabled: fileLoaded
                        checked: true
                    }
                }

            }

            Item {
                id: rectSignPageOptions
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.left: parent.left
                anchors.top: rectSignOptions.bottom

                Item {
                    id: itemCheckSignReduced
                    width: parent.width * 0.3
                    height: parent.height
                    anchors.top: parent.top

                    Switch {
                        id: checkSignReduced
                        text: "Reduzida"
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        enabled: checkSignShow.checked && fileLoaded
                    }
                }

                Item {
                    id: itemCheckPage
                    width: parent.width * 0.4
                    height: parent.height
                    anchors.top: parent.top
                    anchors.left: itemCheckSignReduced.right
                    Text{
                        id: pageText
                        x: 11
                        y: 8
                        text: "Página:"
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_LABEL
                        color: Constants.COLOR_MAIN_PRETO
                        font.capitalization: Font.MixedCase
                        opacity: checkSignShow.checked && fileLoaded && !checkLastPage.checked
                                 ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                    }

                    SpinBox {
                        id: spinBoxControl
                        y: 0
                        from: 1
                        to: 10000
                        value: 1
                        anchors.left: pageText.right
                        width: parent.width - pageText.width - pageText.x
                        height: parent.height
                        anchors.leftMargin: 0
                        enabled: checkSignShow.checked && fileLoaded && !checkLastPage.checked
                        editable:  checkSignShow.checked && fileLoaded ? true : false

                        contentItem: TextInput {
                            id: textSpinBox
                            z: 2
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_LABEL
                            color: Constants.COLOR_MAIN_PRETO
                            opacity: checkSignShow.checked && fileLoaded && !checkLastPage.checked
                                     ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter

                            readOnly: !spinBoxControl.editable
                            validator: spinBoxControl.validator
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                        }


                        up.indicator: Rectangle {
                            x: spinBoxControl.mirrored ? 0 : parent.width - width
                            height: parent.height
                            implicitWidth: 20
                            implicitHeight: Constants.HEIGHT_BOTTOM_COMPONENT

                            Text {
                                text: "+"
                                font.family: lato.name
                                font.pixelSize: Constants.SIZE_TEXT_LABEL
                                color: Constants.COLOR_MAIN_PRETO
                                opacity: checkSignShow.checked && fileLoaded && !checkLastPage.checked
                                         ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                                anchors.fill: parent
                                fontSizeMode: Text.Fit
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }

                        down.indicator: Rectangle {
                            x: spinBoxControl.mirrored ? parent.width - width : 0
                            height: parent.height
                            implicitWidth: 20
                            implicitHeight: Constants.HEIGHT_BOTTOM_COMPONENT

                            Text {
                                text: "-"
                                font.family: lato.name
                                font.pixelSize:  Constants.SIZE_TEXT_LABEL
                                color: Constants.COLOR_MAIN_PRETO
                                opacity: checkSignShow.checked && fileLoaded && !checkLastPage.checked
                                         ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                                anchors.fill: parent
                                fontSizeMode: Text.Fit
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
                Item {
                    id: itemLastPage
                    width: parent.width * 0.3
                    height: parent.height
                    anchors.left: itemCheckPage.right
                    anchors.top: parent.top
                    Switch {
                        id: checkLastPage
                        text: "Ultima"
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        enabled: checkSignShow.checked && fileLoaded
                    }
                }
            }

            Item {
                id: rectSign
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.left: parent.left
                anchors.top: rectSignPageOptions.bottom

                Button {
                    id: button_signCC
                    text: "Assinar com CC"
                    width: Constants.WIDTH_BUTTON
                    height: parent.height
                    enabled: fileLoaded
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    anchors.left: parent.left
                }
                Button {
                    id: button_signCMD
                    text: "Assinar com CMD"
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
}
