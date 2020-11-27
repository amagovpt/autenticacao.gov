/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import Qt.labs.platform 1.0
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
    property bool cardLoaded: false
    property bool propertyBusyIndicatorRunning: false

    property alias propertyRectMainRight: rectMainRight
    property alias propertyPDFPreview: pdfPreviewArea
    property alias propertyFileDialog: fileDialog
    property alias propertyFileDialogOutput: fileDialogOutput
    property alias propertyFileDialogBatchOutput: fileDialogBatchOutput
    property alias propertyFileDialogCMDOutput: fileDialogCMDOutput
    property alias propertyFileDialogBatchCMDOutput: fileDialogBatchCMDOutput
    property alias propertyMouseAreaRectMainRigh: mouseAreaRectMainRigh
    property alias propertyMouseAreaItemOptionsFiles: mouseAreaItemOptionsFiles
    property alias propertyTextDragMsgListView: textDragMsgListView
    property alias propertyTextDragMsgImg: textDragMsgImg
    property alias propertyListViewFiles: listViewFiles
    property alias propertyListViewEntities: listViewEntities
    property alias propertyFilesListViewScroll: filesListViewScroll
    property alias propertyButtonAdd: buttonAdd
    property alias propertyButtonRemoveAll: buttonRemoveAll
    property alias propertyButtonSignWithCC: button_signCC
    property alias propertyButtonSignCMD: button_signCMD
    property alias propertyDropArea: dropArea
    property alias propertyDropFileArea: dropFileArea
    property alias propertyTextFieldReason: textFieldReason
    property alias propertyTextFieldLocal: textFieldLocal
    property alias propertySwitchSignTemp: switchSignTemp
    property alias propertyCheckboxLTV : checkboxLTV
    property alias propertyCheckSignShow: checkSignShow
    property alias propertyCheckSignReduced: checkSignReduced
    property alias propertyRectSignPageOptions: rectSignPageOptions
    property alias propertyItemCheckPage: itemCheckPage
    property alias propertyRadioButtonPADES: radioButtonPADES
    property alias propertyRadioButtonXADES: radioButtonXADES
    property alias propertyMouseAreaToolTipPades: mouseAreaToolTipPades
    property alias propertyMouseAreaToolTipXades: mouseAreaToolTipXades
    property alias propertyMouseAreaToolTipLTV: mouseAreaToolTipLTV
    property alias propertySwitchSignAdd: switchSignAdd
    property alias propertyTextAttributesMsg: textAttributesMsg
    property alias propertyMouseAreaTextAttributesMsg: mouseAreaTextAttributesMsg
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
    property int propertyMouseAreaToolTipLTVX: Constants.SIZE_ROW_H_SPACE
                                                 + Constants.SIZE_TEXT_FIELD_H_SPACE
                                                 + checkboxLTV.x
                                                 + 10 + checkboxLTV.width
                                                 + rectToolTipLTV.width * 0.5
    property int propertyMouseAreaToolTipY: rectMainLeftFile.height
    property alias propertyTextSpinBox: textSpinBox
    property alias propertySpinBoxControl: spinBoxControl
    property alias propertyCheckLastPage: checkLastPage
    property alias propertyPageText: pageText
    property alias propertyTitleConf: titleConf


    BusyIndicator {
        id: busyIndicator
        anchors.verticalCenterOffset: -50 // Avoid conflit with mouseAreaToolTipXades
        anchors.horizontalCenterOffset: 0
        running: propertyBusyIndicatorRunning || pdfPreviewArea.propertyBackground.status == Image.Loading
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

        FileDialog {
            id: fileDialog
            title: qsTranslate("Popup File", "STR_POPUP_FILE_INPUT_MULTI")
            folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
            modality: Qt.WindowModal
            Component.onCompleted: visible = false
            fileMode: FileDialog.OpenFiles
        }

        FileDialog {
            id: fileDialogOutput
            title: qsTranslate("Popup File", "STR_POPUP_FILE_OUTPUT")
            nameFilters: (propertyRadioButtonPADES.checked ? ["PDF (*.pdf)", "All files (*)"] :
                                                             ["ASiC container with XAdES (*.asics *.asice)", "All files (*)"])
            fileMode: FileDialog.SaveFile
        }
        FileDialog {
            id: fileDialogCMDOutput
            title: qsTranslate("Popup File", "STR_POPUP_FILE_OUTPUT")
            nameFilters: (propertyRadioButtonPADES.checked ? ["PDF (*.pdf)", "All files (*)"] :
                                                             ["ASiC container with XAdES (*.asics *.asice)", "All files (*)"])
            fileMode: FileDialog.SaveFile
        }
        FolderDialog {
            id: fileDialogBatchOutput
            folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
        }
        FolderDialog {
            id: fileDialogBatchCMDOutput
            folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
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
                text: qsTranslate("Popup File", "STR_POPUP_FILE_INPUT_MULTI")
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
                        text: qsTranslate("PageServicesSign",
                                          "STR_SIGN_ADD_MULTI_BUTTON")
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        KeyNavigation.tab: buttonRemoveAll
                        KeyNavigation.down: buttonRemoveAll
                        KeyNavigation.right: buttonRemoveAll
                        KeyNavigation.backtab: button_signCMD
                        KeyNavigation.up: button_signCMD
                        Keys.onEnterPressed: clicked()
                        Keys.onReturnPressed: clicked()
                    }
                    Button {
                        id: buttonRemoveAll
                        width: Constants.WIDTH_BUTTON
                        height: parent.height
                        anchors.right: itemBottonsFiles.right
                        text: qsTranslate("PageServicesSign",
                                          "STR_SIGN_REMOVE_MULTI_BUTTON")
                        enabled: fileLoaded
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        highlighted: activeFocus ? true : false
                        Accessible.role: Accessible.Button
                        Accessible.name: text
                        KeyNavigation.tab: titleConf
                        KeyNavigation.down: titleConf
                        KeyNavigation.right: titleConf
                        KeyNavigation.backtab: buttonAdd
                        KeyNavigation.up: buttonAdd
                        Keys.onEnterPressed: clicked()
                        Keys.onReturnPressed: clicked()
                    }
                }
            }
            DropArea {
                id: dropFileArea
                anchors.fill: parent
                z: 1
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
                font.pixelSize: activeFocus
                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                : Constants.SIZE_TEXT_LABEL
                font.bold: activeFocus
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
                text: qsTranslate("PageServicesSign", "STR_SIGN_SETTINGS")
                Accessible.role: Accessible.Section
                Accessible.name: text
                KeyNavigation.tab: textFormatSign
                KeyNavigation.down: textFormatSign
                KeyNavigation.right: textFormatSign
                KeyNavigation.backtab: buttonRemoveAll
                KeyNavigation.up: buttonRemoveAll
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
                            + rectangleEntities.height
                            //+ rowPreserv.height
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
                            text: qsTranslate("PageServicesSign",
                                              "STR_SIGN_FORMAT")
                            rightPadding: 0
                            padding: 0
                            verticalAlignment: Text.AlignVCenter
                            color: Constants.COLOR_TEXT_BODY
                            height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.bold: activeFocus
                            Accessible.role: Accessible.Row
                            Accessible.name: text
                            KeyNavigation.tab: radioButtonPADES
                            KeyNavigation.down: radioButtonPADES
                            KeyNavigation.right: radioButtonPADES
                            KeyNavigation.backtab: titleConf
                            KeyNavigation.up: titleConf
                        }
                        RadioButton {
                            id: radioButtonPADES
                            anchors.left: textFormatSign.right
                            height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                            text: qsTranslate("PageServicesSign","STR_SIGN_PDF")
                            checked: true
                            leftPadding: 0
                            rightPadding: 0
                            anchors.leftMargin: 10
                            enabled: true
                            font.capitalization: Font.MixedCase
                            opacity: enabled ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                            Accessible.role: Accessible.RadioButton
                            Accessible.name: text
                            KeyNavigation.tab: radioButtonXADES
                            KeyNavigation.down: radioButtonXADES
                            KeyNavigation.right: radioButtonXADES
                            KeyNavigation.backtab: textFormatSign
                            KeyNavigation.up: textFormatSign
                            Keys.onEnterPressed: toggleRadio(radioButtonPADES)
                            Keys.onReturnPressed: toggleRadio(radioButtonPADES)
                            contentItem: Text {
                                text: radioButtonPADES.text
                                leftPadding: 22
                                font.family: lato.name
                                font.bold: radioButtonPADES.activeFocus
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
                            text: qsTranslate("PageServicesSign",
                                              "STR_SIGN_ANY_FILE")
                            anchors.leftMargin: 10
                            leftPadding: 0
                            rightPadding: 0
                            enabled: true
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            opacity: enabled ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                            Accessible.role: Accessible.RadioButton
                            Accessible.name: text
                            KeyNavigation.tab: textFieldReason
                            KeyNavigation.down: textFieldReason
                            KeyNavigation.right: textFieldReason
                            KeyNavigation.backtab: radioButtonPADES
                            KeyNavigation.up: radioButtonPADES
                            Keys.onEnterPressed: toggleRadio(radioButtonXADES)
                            Keys.onReturnPressed: toggleRadio(radioButtonXADES)
                            contentItem: Text {
                                text: radioButtonXADES.text
                                leftPadding: 22
                                font.family: lato.name
                                font.bold: radioButtonXADES.activeFocus
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
                        placeholderText: qsTranslate("PageServicesSign",
                                                     "STR_SIGN_REASON") + "?"
                        enabled: fileLoaded
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        Accessible.role: Accessible.EditableText
                        Accessible.name: textFieldReason.placeholderText
                        KeyNavigation.tab: textFieldLocal
                        KeyNavigation.down: textFieldLocal
                        KeyNavigation.right: textFieldLocal
                        KeyNavigation.backtab: radioButtonXADES
                        KeyNavigation.up: radioButtonXADES
                    }
                    TextField {
                        id: textFieldLocal
                        width: parent.width
                        font.italic: textFieldLocal.text === "" ? true : false
                        anchors.top: textFieldReason.bottom
                        placeholderText: qsTranslate("PageServicesSign",
                                                     "STR_SIGN_LOCAL") + "?"
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        enabled: fileLoaded
                        font.family: lato.name
                        font.capitalization: Font.MixedCase
                        Accessible.role: Accessible.EditableText
                        Accessible.name: textFieldLocal.placeholderText
                        KeyNavigation.tab: switchSignTemp
                        KeyNavigation.down: switchSignTemp
                        KeyNavigation.right: switchSignTemp
                        KeyNavigation.backtab: textFieldReason
                        KeyNavigation.up: textFieldReason
                    }
                    Switch {
                        id: switchSignTemp
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        //width: parent.width * 0.7
                        anchors.top: textFieldLocal.bottom
                        text: qsTranslate("PageServicesSign",
                                          "STR_SIGN_ADD_TIMESTAMP")
                        enabled: fileLoaded
                        font.family: lato.name
                        font.bold: activeFocus
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        KeyNavigation.tab: (checkboxLTV.enabled ? checkboxLTV : switchSignAdd)
                        KeyNavigation.down: (checkboxLTV.enabled ? checkboxLTV : switchSignAdd)
                        KeyNavigation.right: (checkboxLTV.enabled ? checkboxLTV : switchSignAdd)
                        KeyNavigation.backtab: textFieldLocal
                        KeyNavigation.up: textFieldLocal
                        Keys.onEnterPressed: toggleSwitch(switchSignTemp)
                        Keys.onReturnPressed: toggleSwitch(switchSignTemp)
                    }
                    CheckBox {
                        id: checkboxLTV
                        text: qsTranslate("PageServicesSign",
                                          "STR_SIGN_ADD_LTV")
                        height: 25
                        anchors.top: textFieldLocal.bottom
                        x: 0.6 * parent.width
                        enabled: switchSignTemp.checked         /* timestamp enabled */
                                && !switchSignAdd.checked       /* scap disabled */
                                && radioButtonPADES.checked     /* pdf signature */
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        font.bold: activeFocus
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                    }
                    Item {
                        id: rectToolTipLTV
                        width: Constants.SIZE_IMAGE_TOOLTIP
                        height: Constants.SIZE_IMAGE_TOOLTIP
                        anchors.leftMargin: 0
                        anchors.left: checkboxLTV.right
                        anchors.top: checkboxLTV.top

                        Image {
                            anchors.fill: parent
                            antialiasing: true
                            fillMode: Image.PreserveAspectFit
                            source: "../../images/tooltip_grey.png"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        MouseArea {
                            id: mouseAreaToolTipLTV
                            anchors.fill: parent
                            hoverEnabled: true
                        }
                    }
                    Switch {
                        id: switchSignAdd
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        anchors.top: switchSignTemp.bottom
                        text: qsTranslate("PageServicesSign",
                                          "STR_SIGN_ADD_ATTRIBUTES")
                        enabled: fileLoaded
                        font.family: lato.name
                        font.bold: activeFocus
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        z: 1
                        KeyNavigation.tab: switchSignAdd.checked ? (textAttributesMsg.visible ? textAttributesMsg : listViewEntities) : pdfPreviewArea
                        KeyNavigation.down: switchSignAdd.checked ? (textAttributesMsg.visible ? textAttributesMsg : listViewEntities) : pdfPreviewArea
                        KeyNavigation.right: switchSignAdd.checked ? (textAttributesMsg.visible ? textAttributesMsg : listViewEntities) : pdfPreviewArea
                        KeyNavigation.backtab: (checkboxLTV.enabled ? checkboxLTV : switchSignTemp)
                        KeyNavigation.up: (checkboxLTV.enabled ? checkboxLTV : switchSignTemp)
                        Keys.onEnterPressed: toggleSwitch(switchSignAdd)
                        Keys.onReturnPressed: toggleSwitch(switchSignAdd)
                    }
                    Item {
                        id: rectangleEntities
                        width: parent.width
                        height: mainItem.height * 0.25
                        anchors.top: switchSignAdd.bottom
                        MouseArea {
                            id: mouseAreaTextAttributesMsg
                            anchors.fill: parent
                            enabled: true
                            hoverEnabled: true
                        }

                        Text {
                            id: textAttributesMsg
                            text: qsTranslate("PageServicesSign",
                                              "STR_ATTRIBUTES_EMPTY")
                            verticalAlignment: Text.AlignVCenter
                            color: Constants.COLOR_TEXT_LABEL
                            height: Constants.HEIGHT_RADIO_BOTTOM_COMPONENT
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_FIELD
                            font.capitalization: Font.MixedCase
                            font.underline: mouseAreaTextAttributesMsg.containsMouse
                            font.bold: activeFocus
                            visible: false
                            x: 54
                            KeyNavigation.tab: listViewEntities
                            KeyNavigation.down:listViewEntities
                            KeyNavigation.right: listViewEntities
                            KeyNavigation.backtab: switchSignAdd
                            KeyNavigation.up: switchSignAdd
                        }
                        ListView {
                            id: listViewEntities
                            anchors.fill: parent
                            clip: true
                            model: entityAttributesModel
                            delegate: attributeListDelegate
                            focus: true
                            spacing: 2
                            boundsBehavior: Flickable.StopAtBounds
                            highlightMoveDuration: 1000
                            highlightMoveVelocity: 1000

                            ScrollBar.vertical: ScrollBar {
                                id: attributesListViewScroll
                                active: true
                                visible: true
                                width: Constants.SIZE_TEXT_FIELD_H_SPACE
                            }
                            KeyNavigation.tab: pdfPreviewArea
                            KeyNavigation.down:pdfPreviewArea
                            KeyNavigation.right: pdfPreviewArea
                            KeyNavigation.backtab: textAttributesMsg
                            KeyNavigation.up: textAttributesMsg
                        }
                    }
//                    Row {
//                        id: rowPreserv
//                        anchors.top: rectangleEntities.bottom
//                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
//                        width: parent.width
//                        height: 0
//                        spacing: 5
//                        x: 40
//                        visible: false

//                        Switch {
//                            id: switchPreserv
//                            text: ""
//                            spacing: -10
//                            anchors.verticalCenter: parent.verticalCenter
//                            font.family: lato.name
//                            font.pixelSize: Constants.SIZE_TEXT_FIELD
//                            font.capitalization: Font.MixedCase
//                            height: parent.height
//                        }
//                        Text {
//                            id: textPreserv1
//                            x: 35
//                            text: qsTranslate("PageServicesSign",
//                                              "STR_SIGN_ATTRIBUTES_SAVE")
//                            verticalAlignment: Text.AlignVCenter
//                            anchors.verticalCenter: parent.verticalCenter
//                            font.bold: false
//                            font.family: lato.name
//                            font.pixelSize: Constants.SIZE_TEXT_FIELD
//                            font.capitalization: Font.MixedCase
//                            color: Constants.COLOR_TEXT_BODY
//                            height: parent.height
//                        }
//                        Text {
//                            id: textPreserv2
//                            text: qsTranslate(
//                                      "PageServicesSign",
//                                      "STR_SIGN_ATTRIBUTES_SAVE_HOW_LONG")
//                            verticalAlignment: Text.AlignVCenter
//                            anchors.verticalCenter: parent.verticalCenter
//                            font.bold: false
//                            font.family: lato.name
//                            font.pixelSize: Constants.SIZE_TEXT_FIELD
//                            font.capitalization: Font.MixedCase
//                            color: Constants.COLOR_TEXT_BODY
//                            visible: switchPreserv.checked
//                            height: parent.height
//                        }
//                        ComboBox {
//                            id: comboBoxPreserve
//                            width: 80
//                            anchors.verticalCenter: parent.verticalCenter
//                            height: parent.height
//                            // textPreservAnos.text is related with model values
//                            // if model is changed textPreservAnos.text may be changed
//                            model: ["0", "1", "3", "5", "10"]
//                            font.family: lato.name
//                            font.pixelSize: Constants.SIZE_TEXT_FIELD
//                            font.capitalization: Font.MixedCase
//                            visible: switchPreserv.checked
//                        }
//                        Text {
//                            id: textPreservAnos
//                            text: comboBoxPreserve.currentIndex
//                                  === 1 ? qsTranslate(
//                                              "PageServicesSign",
//                                              "STR_SIGN_ATTRIBUTES_YEAR") : qsTranslate(
//                                              "PageServicesSign",
//                                              "STR_SIGN_ATTRIBUTES_YEARS")
//                            verticalAlignment: Text.AlignVCenter
//                            anchors.verticalCenter: parent.verticalCenter
//                            font.bold: false
//                            font.family: lato.name
//                            font.pixelSize: Constants.SIZE_TEXT_FIELD
//                            font.capitalization: Font.MixedCase
//                            color: Constants.COLOR_TEXT_BODY
//                            visible: switchPreserv.checked
//                            height: parent.height
//                        }
//                    }
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
                text: qsTranslate("PageServicesSign", "STR_SIGN_PREVIEW")
            }

            Rectangle {
                id: rectPre
                width: parent.width
                height: parent.height - titlePre.height - Constants.SIZE_TEXT_V_SPACE
                enabled: fileLoaded
                anchors.top: titlePre.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                Text {
                    id: textDragMsgImg
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: parent.height
                    font.bold: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    color: Constants.COLOR_TEXT_LABEL
                    visible: true
                    font.family: lato.name
                    z: 1
                }
                Components.PDFPreview {
                    id: pdfPreviewArea
                    width: parent.width
                    height: parent.height
                    propertyDragSigRect.visible: checkSignShow.checked
                    propertyReducedChecked: checkSignReduced.checked
                    KeyNavigation.tab: textSpinBox
                    KeyNavigation.down: textSpinBox
                    KeyNavigation.right: textSpinBox
                    KeyNavigation.backtab: switchSignAdd
                    KeyNavigation.up: switchSignAdd
                }
            }

            MouseArea {
                id: mouseAreaRectMainRigh
                anchors.fill: parent
                enabled: !fileLoaded
            }
            DropArea {
                id: dropArea
                anchors.fill: parent
                z: 2
            }
        }
        Item {
            id: rowBottom
            width: rectMainRight.width
            height: rectSignOptions.height + rectSignPageOptions.height + rectSign.height
            anchors.top: rectMainRight.bottom
            anchors.left: rectMainRight.left

            Item {
                id: rectSignPageOptions
                width: parent.width * 0.60
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                visible: rectMainRight.visible
                anchors.horizontalCenter: parent.horizontalCenter

                Item {
                    id: itemCheckPage
                    width: 150
                    height: parent.height
                    anchors.top: parent.top

                    Text {
                        id: pageText
                        x: 11
                        y: 8
                        text: qsTranslate("PageServicesSign",
                                          "STR_SIGN_PAGE") + ":"
                        font.family: lato.name
                        font.pixelSize: Constants.SIZE_TEXT_LABEL
                        color: Constants.COLOR_MAIN_PRETO
                        font.capitalization: Font.MixedCase
                        opacity: fileLoaded && propertyRadioButtonPADES.checked
                                 && !checkLastPage.checked ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
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
                        anchors.leftMargin: 0
                        enabled: fileLoaded && propertyRadioButtonPADES.checked
                                 && !checkLastPage.checked
                        editable: checkSignShow.checked
                                  && fileLoaded ? true : false
                        validator: RegExpValidator { regExp: /[1-9][0-9]*/ }
                        Accessible.ignored: true

                        contentItem: TextInput {
                            id: textSpinBox
                            z: 2
                            font.family: lato.name
                            font.pixelSize: Constants.SIZE_TEXT_LABEL
                            color: Constants.COLOR_MAIN_PRETO
                            opacity: fileLoaded
                                     && propertyRadioButtonPADES.checked
                                     && !checkLastPage.checked ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter

                            readOnly: !spinBoxControl.editable
                            validator: spinBoxControl.validator
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                            maximumLength: 5 //pages go from 1 to 99999
                            Accessible.role: Accessible.EditableText
                            Accessible.name: qsTranslate("PageServicesSign","STR_SIGN_PAGE") + spinBoxControl.value
                            KeyNavigation.tab: checkLastPage
                            KeyNavigation.down: checkLastPage
                            KeyNavigation.right: checkLastPage
                            KeyNavigation.backtab: pdfPreviewArea
                            KeyNavigation.up: pdfPreviewArea
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
                                font.bold: true
                                opacity: fileLoaded
                                         && propertyRadioButtonPADES.checked
                                         && !checkLastPage.checked ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
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
                            implicitHeight: Constants.HEIGHT_BOTTOM_COMPONENT

                            Text {
                                text: "<"
                                font.family: lato.name
                                font.pixelSize: Constants.SIZE_ARROW_INDICATOR
                                color: Constants.COLOR_MAIN_BLUE
                                opacity: fileLoaded
                                         && propertyRadioButtonPADES.checked
                                         && !checkLastPage.checked ? 1.0 : Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                                anchors.fill: parent
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
                        text: qsTranslate("PageServicesSign", "STR_SIGN_LAST")
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        font.family: lato.name
                        font.bold: activeFocus
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        enabled: fileLoaded && propertyRadioButtonPADES.checked
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        KeyNavigation.tab: checkSignShow
                        KeyNavigation.down: checkSignShow
                        KeyNavigation.right: checkSignShow
                        KeyNavigation.backtab: textSpinBox
                        KeyNavigation.up: textSpinBox
                        Keys.onEnterPressed: toggleSwitch(checkLastPage)
                        Keys.onReturnPressed: toggleSwitch(checkLastPage)
                    }
                }
            }

            Item {
                id: rectSignOptions
                width: parent.width * 0.60
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.top: rectSignPageOptions.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                visible: rectMainRight.visible
                Item {
                    id: itemCheckSignShow
                    width: parent.width * 0.5
                    height: parent.height
                    anchors.top: parent.top
                    Switch {
                        id: checkSignShow
                        text: qsTranslate("PageServicesSign",
                                          "STR_SIGN_VISIBLE")
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        font.family: lato.name
                        font.bold: activeFocus
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        enabled: fileLoaded
                        checked: true
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        KeyNavigation.tab: checkSignReduced
                        KeyNavigation.down: checkSignReduced
                        KeyNavigation.right: checkSignReduced
                        KeyNavigation.backtab: checkLastPage
                        KeyNavigation.up: checkLastPage
                        Keys.onEnterPressed: toggleSwitch(checkSignShow)
                        Keys.onReturnPressed: toggleSwitch(checkSignShow)
                    }
                }
                Item {
                    id: itemCheckSignReduced
                    width: parent.width * 0.5
                    height: parent.height
                    anchors.top: parent.top
                    anchors.left: itemCheckSignShow.right

                    Switch {
                        id: checkSignReduced
                        text: qsTranslate("PageServicesSign",
                                          "STR_SIGN_REDUCED")
                        height: Constants.HEIGHT_SWITCH_COMPONENT
                        font.family: lato.name
                        font.bold: activeFocus
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        font.capitalization: Font.MixedCase
                        enabled: fileLoaded
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: text
                        KeyNavigation.tab: button_signCC.enabled ? button_signCC : button_signCMD
                        KeyNavigation.down: button_signCC.enabled ? button_signCC : button_signCMD
                        KeyNavigation.right: button_signCC.enabled ? button_signCC : button_signCMD
                        KeyNavigation.backtab: checkSignShow
                        KeyNavigation.up: checkSignShow
                        Keys.onEnterPressed: toggleSwitch(checkSignReduced)
                        Keys.onReturnPressed: toggleSwitch(checkSignReduced)
                    }
                }
            }

            Item {
                id: rectSign
                width: parent.width
                height: Constants.HEIGHT_SIGN_BOTTOM_COMPONENT
                anchors.left: parent.left
                anchors.top: rectSignOptions.bottom

                Button {
                    id: button_signCC
                    text: qsTranslate(
                              "PageServicesSign",
                              "STR_SIGN_SIGN_BUTTON") + "\n" + qsTranslate(
                              "PageServicesSign", "STR_SIGN_CARD_BUTTON")
                    width: Constants.WIDTH_BUTTON
                    height: parent.height
                    enabled: fileLoaded && cardLoaded
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false
                    anchors.left: parent.left
                    KeyNavigation.tab: button_signCMD.enabled ? button_signCMD : buttonAdd
                    KeyNavigation.down: button_signCMD.enabled ? button_signCMD : buttonAdd
                    KeyNavigation.right: button_signCMD.enabled ? button_signCMD : buttonAdd
                    KeyNavigation.backtab: checkSignReduced
                    KeyNavigation.up: checkSignReduced
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
                Button {
                    id: button_signCMD
                    text: qsTranslate(
                              "PageServicesSign",
                              "STR_SIGN_SIGN_BUTTON") + "\n" + qsTranslate(
                              "PageServicesSign", "STR_SIGN_CMD_BUTTON")
                    width: Constants.WIDTH_BUTTON
                    height: parent.height
                    enabled: fileLoaded
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false
                    anchors.right: parent.right
                    KeyNavigation.tab: buttonAdd
                    KeyNavigation.down: buttonAdd
                    KeyNavigation.right: buttonAdd
                    KeyNavigation.backtab: button_signCC
                    KeyNavigation.up: button_signCC
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
            }
        }
    }
}
