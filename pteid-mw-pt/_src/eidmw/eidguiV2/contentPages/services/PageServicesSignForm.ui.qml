/*-****************************************************************************

 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
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
import QtQuick.Layouts 1.3
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
    property bool signCertExpired: false
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
    property alias propertySwitchAddAttributes: switchAddAttributes
    property alias propertyTextAttributesMsg: textAttributesMsg
    property alias propertyMouseAreaTextAttributesMsg: mouseAreaTextAttributesMsg
    property alias propertyTextSpinBox: textSpinBox
    property alias propertySpinBoxControl: spinBoxControl
    property alias propertyCheckLastPage: checkLastPage
    property alias propertyPageText: pageText
    property alias propertyTitleConf: titleConf

    property alias propertyButtonArrowHelp: buttonArrowHelp
    property alias propertyArrowHelpMouseArea: arrowHelpMouseArea

    property alias propertyButtonArrowOptions: buttonArrowOptions
    property alias propertyArrowOptionsMouseArea: arrowOptionsMouseArea
    property alias propertyRectHelp: rectMainLeftHelp

    property bool propertyShowHelp: controler.getShowSignatureHelp()
    property bool propertyShowOptions: controler.getShowSignatureOptions()

    property int propertyOptionsHeight: rectFormatOptions.height
                                        + textFieldReason.height
                                        + textFieldLocal.height
                                        + switchSignTemp.height
                                        + checkboxLTV.height
                                        + switchAddAttributes.height
                                        + rectangleEntities.height
                                        + 4 * Constants.SIZE_TEXT_V_SPACE

    property int propertyListViewHeight:0

    property alias propertyItemOptions: itemOptions
    property alias propertySettingsScroll: settingsScroll
    property alias propertyFlickable: flickable
    property alias propertyTitleHelp: titleHelp

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
            id: rectMainLeft
            width: parent.width * 0.5
            height: parent.height

            Flickable {
                id: flickable
                width: parent.width - Constants.SIZE_ROW_H_SPACE
                height: parent.height
                anchors.top: rectMainLeft.top
                clip:true
                contentHeight: titleHelp.childrenRect.height
                               +rectMainLeftHelp.childrenRect.height
                               + 7 * Constants.SIZE_ROW_V_SPACE
                               + rectMainLeftFile.childrenRect.height
                               + rectMainLeftOptions.childrenRect.height

                ScrollBar.vertical: ScrollBar {
                    id: settingsScroll
                    parent: rectMainLeft
                    visible: true
                    width: Constants.SIZE_TEXT_FIELD_H_SPACE
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    stepSize : 1.0
                }

                Text {
                    id: titleHelp
                    x: 2 * Constants.SIZE_ROW_H_SPACE
                    font.pixelSize: Constants.SIZE_TEXT_BODY
                    font.family: lato.name
                    font.bold: activeFocus
                    color: Constants.COLOR_TEXT_LABEL
                    height: Constants.SIZE_TEXT_BODY
                    text: qsTranslate("PageServicesSign", "STR_SIGN_HELP_TITLE")
                    Accessible.role: Accessible.StaticText
                    Accessible.name: titleHelp.text
                    KeyNavigation.tab: propertyShowHelp == true ? textSubTitle.propertyText : buttonArrowHelp
                    KeyNavigation.down: propertyShowHelp == true ? textSubTitle.propertyText : buttonArrowHelp
                    KeyNavigation.right: propertyShowHelp == true ? textSubTitle.propertyText : buttonArrowHelp
                    Keys.onPressed: {
                        handleKeyPressed(event.key, titleHelp)
                    }
                    KeyNavigation.left: titleHelp
                    KeyNavigation.backtab: titleHelp
                    KeyNavigation.up: titleHelp

                }

                Item {
                    id: rectMainLeftHelp
                    width: parent.width - Constants.SIZE_ROW_H_SPACE
                    height: Constants.HEIGHT_HELP_EXPANDED
                    x: Constants.SIZE_ROW_H_SPACE
                    anchors.top: titleHelp.bottom
                    anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                    clip: true

                    Rectangle {
                        id: rectHelp
                        width: parent.width
                        height: parent.height

                        color: Constants.COLOR_MAIN_MIDDLE_GRAY
                        anchors.horizontalCenter: parent.horizontalCenter

                        Components.Link {
                            id: textSubTitle
                            y: Constants.SIZE_TEXT_FIELD_V_SPACE
                            visible: rectMainLeftHelp.height > Constants.HEIGHT_HELP_COLLAPSED
                            propertyText.text: qsTranslate("PageServicesSign",
                                                           "STR_SIGN_HELP_SUB_TITLE") + " "
                                               + "<a href='https://www.autenticacao.gov.pt/cmd-pedido-chave'>"
                                               + qsTranslate("PageServicesSign",
                                                             "STR_SIGN_HELP_CMD_LINK")
                            propertyText.width: parent.width
                            propertyText.textMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                            propertyText.wrapMode: Text.Wrap
                            propertyText.font.pixelSize: Constants.SIZE_TEXT_LINK_LABEL
                            propertyText.font.bold: activeFocus
                            Layout.fillWidth: true
                            propertyAccessibleText: qsTranslate(
                                                        "PageServicesSign",
                                                        "STR_SIGN_HELP_SUB_TITLE") + qsTranslate(
                                                        "PageServicesSign",
                                                        "STR_SIGN_HELP_CMD_LINK")
                            propertyAccessibleDescription: qsTranslate(
                                                               "PageServicesSign",
                                                               "STR_SIGN_HELP_CMD_SELECT")
                            propertyLinkUrl: 'https://www.autenticacao.gov.pt/cmd-pedido-chave'
                            KeyNavigation.tab: autenticacaoGovLink.propertyText
                            KeyNavigation.down: autenticacaoGovLink.propertyText
                            KeyNavigation.right: autenticacaoGovLink.propertyText
                            Keys.onPressed: {
                                handleKeyPressed(event.key, textSubTitle)
                            }
                            KeyNavigation.left: titleHelp
                            KeyNavigation.backtab: titleHelp
                            KeyNavigation.up: titleHelp
                        }

                        Components.Link {
                            id: autenticacaoGovLink
                            visible: rectMainLeftHelp.height > Constants.HEIGHT_HELP_COLLAPSED
                            anchors.top: textSubTitle.bottom
                            propertyText.text: qsTranslate("PageServicesSign",
                                                           "STR_SIGN_HELP_TOPIC_2")
                                               + "<a href='https://www.autenticacao.gov.pt'>" + " " + qsTranslate(
                                                   "PageServicesSign",
                                                   "STR_SIGN_HELP_AUTENTICACAO.GOV_LINK")
                            propertyText.font.pixelSize: Constants.SIZE_TEXT_LINK_LABEL
                            propertyText.width: parent.width
                            propertyText.textMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                            propertyText.wrapMode: Text.Wrap
                            propertyText.font.bold: activeFocus
                            Layout.fillWidth: true
                            propertyAccessibleText: qsTranslate(
                                                        "PageServicesSign",
                                                        "STR_SIGN_HELP_TOPIC_2") + qsTranslate(
                                                        "PageServicesSign",
                                                        "STR_SIGN_HELP_AUTENTICACAO.GOV_LINK")
                            propertyAccessibleDescription: qsTranslate(
                                                               "PageServicesSign",
                                                               "STR_SIGN_HELP_AUTENTICACAO.GOV_SELECT")
                            propertyLinkUrl: 'https://www.autenticacao.gov.pt'
                            KeyNavigation.tab: buttonArrowHelp
                            KeyNavigation.down: buttonArrowHelp
                            KeyNavigation.right: buttonArrowHelp
                            Keys.onPressed: {
                                handleKeyPressed(event.key, autenticacaoGovLink)
                            }
                            KeyNavigation.left: textSubTitle.propertyText
                            KeyNavigation.backtab: textSubTitle.propertyText
                            KeyNavigation.up: textSubTitle.propertyText
                        }
                        Rectangle {
                            id: arrowHelpRect
                            width: parent.width
                            height: Constants.SIZE_IMAGE_BOTTOM_MENU
                            anchors.bottom: parent.bottom
                            color: Constants.COLOR_MAIN_MIDDLE_GRAY
                            Button {
                                id: buttonArrowHelp
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                width: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                                height: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                                enabled: true
                                background: Rectangle {
                                    anchors.fill: parent
                                    color: "transparent"
                                    Image {
                                        source: arrowHelpMouseArea.containsMouse || buttonArrowHelp.activeFocus
                                                ? "../../images/arrow-down_hover.png"
                                                : "../../images/arrow-down_AMA.png"
                                        anchors.fill: parent
                                        rotation: propertyShowHelp ? 180 : 0
                                    }
                                }
                                Accessible.role: Accessible.Button
                                Accessible.name: qsTranslate("GAPI", "STR_SIGN_OPEN_HELP")
                                KeyNavigation.tab: titleSelectFile
                                KeyNavigation.down: titleSelectFile
                                KeyNavigation.right: titleSelectFile
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, buttonArrowHelp)
                                }
                                KeyNavigation.backtab: propertyShowHelp == true ? autenticacaoGovLink.propertyText : titleHelp
                                KeyNavigation.up: propertyShowHelp == true ? autenticacaoGovLink.propertyText : titleHelp
                                Keys.onEnterPressed: clicked()
                                Keys.onReturnPressed: clicked()
                            }
                            MouseArea {
                                id: arrowHelpMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                z: 0
                            }
                        }
                    }

                }
                Item {
                    id: rectMainLeftFile
                    width: parent.width - Constants.SIZE_ROW_H_SPACE - Constants.FORM_SHADOW_H_OFFSET
                    height: itemOptionsFiles.height + 4 * Constants.SIZE_TEXT_V_SPACE + itemBottonsFiles.height
                    x: Constants.SIZE_ROW_H_SPACE
                    anchors.top: rectMainLeftHelp.bottom
                    anchors.topMargin: 3 * Constants.SIZE_ROW_V_SPACE

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
                        font.pixelSize: activeFocus
                                        ? Constants.SIZE_TEXT_LABEL_FOCUS
                                        : Constants.SIZE_TEXT_LABEL
                        font.bold: activeFocus
                        font.family: lato.name
                        color: Constants.COLOR_TEXT_LABEL
                        height: Constants.SIZE_TEXT_LABEL
                        text: qsTranslate("Popup File", "STR_POPUP_FILE_INPUT_MULTI")

                        Accessible.role: Accessible.Section
                        Accessible.name: text
                        KeyNavigation.tab: listViewFiles.count > 0 ? listViewFiles : buttonAdd
                        KeyNavigation.down: listViewFiles.count > 0 ? listViewFiles : buttonAdd
                        Keys.onPressed: {
                            handleKeyPressed(event.key, titleSelectFile)
                        }
                        KeyNavigation.right: listViewFiles.count > 0 ? listViewFiles : buttonAdd
                        KeyNavigation.backtab: buttonArrowHelp
                        KeyNavigation.up: buttonArrowHelp
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
                            height: !filesModel.count || listViewFiles.contentHeight < Constants.MIN_HEIGHT_FILES_RECT
                                ? Constants.MIN_HEIGHT_FILES_RECT 
                                : listViewFiles.contentHeight
                            anchors.horizontalCenter: parent.horizontalCenter
                            y: 2 * Constants.SIZE_TEXT_V_SPACE

                            ListView {
                                id: listViewFiles
                                width: parent.width
                                height: parent.height
                                clip: true
                                spacing: Constants.SIZE_LISTVIEW_SPACING
                                boundsBehavior: Flickable.StopAtBounds
                                model: filesModel
                                delegate: listViewFilesDelegate
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
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, buttonAdd)
                                }
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
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, buttonRemoveAll)
                                }
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
                    width: parent.width - Constants.SIZE_ROW_H_SPACE - Constants.FORM_SHADOW_H_OFFSET
                    height: titleConf.height + rectOptions.height
                    x: Constants.SIZE_ROW_H_SPACE
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
                        text: qsTranslate("PageServicesSign", "STR_SIGN_ADVANCED_OPTIONS")
                        Accessible.role: Accessible.Section
                        Accessible.name: text
                        KeyNavigation.tab: buttonArrowOptions
                        KeyNavigation.down: buttonArrowOptions
                        Keys.onPressed: {
                            handleKeyPressed(event.key, titleConf)
                        }
                        KeyNavigation.right: buttonArrowOptions
                        KeyNavigation.backtab: buttonRemoveAll
                        KeyNavigation.up: buttonRemoveAll
                    }
                    Rectangle {
                        id: rectOptions
                        width: parent.width
                        height: itemOptions.height + arrowOptionsRect.height
                                + 2 * Constants.SIZE_TEXT_V_SPACE
                        color: "white"
                        anchors.top: titleConf.bottom
                        anchors.topMargin: Constants.SIZE_TEXT_V_SPACE

                        Item {
                            id: arrowOptionsRect
                            width: parent.width
                            height: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                            visible: true
                            y: Constants.SIZE_TEXT_V_SPACE

                            Text {
                                id: titleOptions
                                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                                font.pixelSize: activeFocus
                                                ? Constants.SIZE_TEXT_LABEL_FOCUS
                                                : Constants.SIZE_TEXT_LABEL
                                font.bold: arrowOptionsMouseArea.containsMouse || buttonArrowOptions.activeFocus
                                font.family: lato.name
                                color: Constants.COLOR_TEXT_LABEL
                                height: Constants.SIZE_TEXT_LABEL
                                text: qsTranslate("PageServicesSign", "STR_SIGN_SETTINGS")
                                Accessible.role: Accessible.Section
                                Accessible.name: text
                            }

                            Button {
                                id: buttonArrowOptions
                                anchors.right: parent.right
                                anchors.rightMargin: Constants.SIZE_TEXT_FIELD_H_SPACE
                                anchors.verticalCenter: parent.verticalCenter
                                width: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                                height: Constants.SIZE_IMAGE_ARROW_MAIN_MENU
                                enabled: true
                                background: Rectangle {
                                    anchors.fill: parent
                                    color: "transparent"
                                    Image {
                                        source: arrowOptionsMouseArea.containsMouse || buttonArrowOptions.activeFocus
                                                ? "../../images/arrow-down_hover.png"
                                                : "../../images/arrow-down_AMA.png"
                                        anchors.fill: parent
                                        rotation: propertyShowOptions ? 180 : 0
                                    }
                                }
                                Accessible.role: Accessible.Button
                                Accessible.name: qsTranslate("GAPI", "STR_SIGN_OPEN_OPTIONS")
                                KeyNavigation.tab: propertyShowOptions == true ? textFormatSign : pdfPreviewArea
                                KeyNavigation.down: propertyShowOptions == true ? textFormatSign : pdfPreviewArea
                                KeyNavigation.right: propertyShowOptions == true ? textFormatSign : pdfPreviewArea
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, buttonArrowOptions)
                                }
                                KeyNavigation.backtab: titleConf
                                KeyNavigation.up: titleConf
                                Keys.onEnterPressed: clicked()
                                Keys.onReturnPressed: clicked()
                            }
                            MouseArea {
                                    id: arrowOptionsMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    z: 1
                            }
                        }

                        Item {
                            id: itemOptions
                            width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: arrowOptionsRect.bottom
                            clip: true
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
                                    Keys.onPressed: {
                                        handleKeyPressed(event.key, textFormatSign)
                                    }
                                    KeyNavigation.right: radioButtonPADES
                                    KeyNavigation.backtab: buttonArrowOptions
                                    KeyNavigation.up: buttonArrowOptions
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
                                    Keys.onPressed: {
                                        handleKeyPressed(event.key, radioButtonPADES)
                                    }
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
                                        color: Constants.COLOR_MAIN_BLACK
                                    }
                                }
                                Item {
                                    id: rectToolTipPades
                                    width: Constants.SIZE_IMAGE_TOOLTIP
                                    height: Constants.SIZE_IMAGE_TOOLTIP
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
                                    Keys.onPressed: {
                                        handleKeyPressed(event.key, radioButtonXADES)
                                    }
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
                                        color: Constants.COLOR_MAIN_BLACK
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
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, textFieldReason)
                                }
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
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, textFieldLocal)
                                }
                                KeyNavigation.backtab: textFieldReason
                                KeyNavigation.up: textFieldReason
                            }
                            Switch {
                                id: switchSignTemp
                                height: Constants.HEIGHT_SWITCH_COMPONENT
                                anchors.top: textFieldLocal.bottom
                                x: 5
                                text: qsTranslate("PageServicesSign",
                                                  "STR_SIGN_ADD_TIMESTAMP")
                                enabled: fileLoaded
                                font.family: lato.name
                                font.bold: activeFocus
                                font.pixelSize: Constants.SIZE_TEXT_FIELD
                                font.capitalization: Font.MixedCase
                                Accessible.role: Accessible.CheckBox
                                Accessible.name: text
                                KeyNavigation.tab: (checkboxLTV.enabled ? checkboxLTV : switchAddAttributes)
                                KeyNavigation.down: (checkboxLTV.enabled ? checkboxLTV : switchAddAttributes)
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, switchSignTemp)
                                }
                                KeyNavigation.right: (checkboxLTV.enabled ? checkboxLTV : switchAddAttributes)
                                KeyNavigation.backtab: textFieldLocal
                                KeyNavigation.up: textFieldLocal
                                Keys.onEnterPressed: toggleSwitch(switchSignTemp)
                                Keys.onReturnPressed: toggleSwitch(switchSignTemp)
                            }
                            CheckBox {
                                id: checkboxLTV
                                text: qsTranslate("PageServicesSign",
                                                  "STR_SIGN_ADD_LTV")
                                height: Constants.HEIGHT_SWITCH_COMPONENT
                                anchors.top: switchSignTemp.bottom
                                x: 48
                                enabled: switchSignTemp.checked         /* timestamp enabled */
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
                                anchors.leftMargin: - 5
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
                                id: switchAddAttributes
                                height: Constants.HEIGHT_SWITCH_COMPONENT
                                anchors.top: checkboxLTV.bottom
                                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                                x: 5
                                text: qsTranslate("PageServicesSign",
                                                  "STR_SIGN_ADD_ATTRIBUTES")
                                enabled: fileLoaded
                                font.family: lato.name
                                font.bold: activeFocus
                                font.pixelSize: Constants.SIZE_TEXT_FIELD
                                font.capitalization: Font.MixedCase
                                z: 1
                                KeyNavigation.tab: switchAddAttributes.checked ? (textAttributesMsg.visible ? textAttributesMsg : listViewEntities) : pdfPreviewArea
                                KeyNavigation.down: switchAddAttributes.checked ? (textAttributesMsg.visible ? textAttributesMsg : listViewEntities) : pdfPreviewArea
                                KeyNavigation.right: switchAddAttributes.checked ? (textAttributesMsg.visible ? textAttributesMsg : listViewEntities) : pdfPreviewArea
                                Keys.onPressed: {
                                    handleKeyPressed(event.key, switchAddAttributes)
                                }
                                KeyNavigation.backtab: (checkboxLTV.enabled ? checkboxLTV : switchSignTemp)
                                KeyNavigation.up: (checkboxLTV.enabled ? checkboxLTV : switchSignTemp)
                                Keys.onEnterPressed: toggleSwitch(switchAddAttributes)
                                Keys.onReturnPressed: toggleSwitch(switchAddAttributes)
                            }
                            Item {
                                id: rectangleEntities
                                width: parent.width
                                height: listViewEntities.count > 0
                                    ? (propertyListViewHeight + (listViewEntities.count - 1) * Constants.SIZE_LISTVIEW_SPACING)
                                    : textAttributesMsg.height
                                anchors.top: switchAddAttributes.bottom
                                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
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
                                    height: visible ? Constants.HEIGHT_RADIO_BOTTOM_COMPONENT : 0
                                    font.family: lato.name
                                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                                    font.capitalization: Font.MixedCase
                                    font.underline: mouseAreaTextAttributesMsg.containsMouse
                                    font.bold: activeFocus
                                    visible: false
                                    x: 54
                                    Accessible.role: Accessible.Button
                                    Accessible.name: text
                                    KeyNavigation.tab: pdfPreviewArea
                                    KeyNavigation.down:pdfPreviewArea
                                    KeyNavigation.right: pdfPreviewArea
                                    Keys.onPressed: {
                                        handleKeyPressed(event.key, textAttributesMsg)
                                    }
                                    KeyNavigation.backtab: switchAddAttributes
                                    KeyNavigation.up: switchAddAttributes
                                }
                                ListView {
                                    id: listViewEntities
                                    anchors.fill: parent
                                    model: entityAttributesModel
                                    delegate: attributeListDelegate
                                    focus: true
                                    spacing: Constants.SIZE_LISTVIEW_SPACING
                                    boundsBehavior: Flickable.StopAtBounds
                                    highlightMoveDuration: 1000
                                    highlightMoveVelocity: 1000
                                    cacheBuffer: Constants.SCAP_ATTR_LISTVIEW_CACHEBUFFER
                                    interactive: false //disables scroll/drag for this listview
                                    Keys.forwardTo: attributeListDelegate
                                }
                            }
                        }
                    }
                }
            }
        }

        Item {
            id: rectMainRight
            width: parent.width * 0.5
            height: parent.height - rowBottom.height
            anchors.left: rectMainLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE * 0.5

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
                    height: parent.height
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
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
                    propertyReducedChecked: checkSignReduced.checked
                    KeyNavigation.tab: textSpinBox
                    KeyNavigation.down: textSpinBox
                    KeyNavigation.right: textSpinBox
                    KeyNavigation.backtab: switchAddAttributes
                    KeyNavigation.up: switchAddAttributes
                }
            }

            MouseArea {
                id: mouseAreaRectMainRigh
                anchors.fill: parent
                enabled: !fileLoaded
                visible: !fileLoaded
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
                        color: Constants.COLOR_MAIN_BLACK
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
                            color: Constants.COLOR_MAIN_BLACK
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
                            color: "transparent"

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
                            color: "transparent"

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
                    enabled: fileLoaded && cardLoaded && !signCertExpired
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false
                    anchors.left: parent.left
                    KeyNavigation.tab: button_signCMD.enabled ? button_signCMD : titleHelp
                    KeyNavigation.down: button_signCMD.enabled ? button_signCMD : titleHelp
                    KeyNavigation.right: button_signCMD.enabled ? button_signCMD : titleHelp
                    Keys.onPressed: {
                        handleKeyPressed(event.key, button_signCC)
                    }
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
                    enabled: fileLoaded && radioButtonPADES.checked
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    highlighted: activeFocus ? true : false
                    anchors.right: parent.right
                    KeyNavigation.tab: titleHelp
                    KeyNavigation.down: titleHelp
                    KeyNavigation.right: titleHelp
                    Keys.onPressed: {
                        handleKeyPressed(event.key, button_signCMD)
                    }
                    KeyNavigation.backtab: button_signCC
                    KeyNavigation.up: button_signCC
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
            }
        }
    }
}
