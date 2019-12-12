/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2018-2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

PageCardPrintForm {

    property string outputFile : ""

    Keys.onPressed: {
        console.log("PageCardPrintForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Dialog {
        id: createsuccess_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - createsuccess_dialog.width * 0.5
        y: parent.height * 0.5 - createsuccess_dialog.height * 0.5

        header: Label {
            id: createdSuccTitle
            text: qsTr("STR_CREATE_SUCESS")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
            KeyNavigation.tab: openFileText
            KeyNavigation.right: openFileText
            KeyNavigation.down: openFileText
        }

        Item {
            width: createsuccess_dialog.availableWidth
            height: 50

            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return)
                {
                    showCreatedFile()
                }
            }

            Item {
                id: rectLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: openFileText
                    text: qsTr("STR_CREATE_OPEN")
                    font.bold: activeFocus
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    wrapMode: Text.Wrap
                    KeyNavigation.tab: cancelButton
                    KeyNavigation.right: cancelButton
                    KeyNavigation.down: cancelButton
                    KeyNavigation.backtab: createdSuccTitle
                    KeyNavigation.up: createdSuccTitle
                }
            }
        }
        Item {
            width: createsuccess_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            y: 80
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: cancelButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_CANCEL")
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    KeyNavigation.tab: openButton
                    KeyNavigation.right: openButton
                    KeyNavigation.backtab: openFileText
                    KeyNavigation.up: openFileText
                    highlighted: activeFocus
                    onClicked: {
                        createsuccess_dialog.close()
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    }
                }
                Button {
                    id: openButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_OPEN")
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    KeyNavigation.tab: createdSuccTitle
                    KeyNavigation.right: createdSuccTitle
                    KeyNavigation.backtab: cancelButton
                    KeyNavigation.up: cancelButton
                    highlighted: activeFocus
                    onClicked: {
                        showCreatedFile()
                    }
                }
            }
        }
        onRejected:{
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
        onClosed: {
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
    }
    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalPdfPrintSignSucess: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            createsuccess_dialog.visible = true
            createdSuccTitle.forceActiveFocus()
            propertyBusyIndicator.running = false
        }
        onSignalPdfPrintSucess: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            createsuccess_dialog.visible = true
            createdSuccTitle.forceActiveFocus()
            propertyBusyIndicator.running = false
        }
        onSignalPdfPrintFail: {
            var titlePopup = qsTr("STR_PRINT_CREATE_PDF")
            var bodyPopup = qsTr("STR_PRINT_CREATE_PDF_FAIL")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalPrinterPrintSucess: {
            var titlePopup = qsTr("STR_PRINT_PRINTER")
            var bodyPopup = qsTr("STR_PRINT_PRINTER_SUCESS")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalPrinterPrintFail: {
            console.log("got error on printer print failed "  + error_code)
            var titlePopup = qsTr("STR_PRINT_PRINTER")
            var bodyPopup = ""
            if (error_code == GAPI.NoPrinterAvailable) {
                bodyPopup = qsTr("STR_PRINT_NO_PRINTER_AVAILABLE")
            } else {
                bodyPopup = qsTr("STR_PRINT_PRINTER_FAIL")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)

        }
        onSignalCardDataChanged: {
            console.log("Data Card Print --> Data Changed")
            propertyBusyIndicator.running = false

            propertySwitchBasic.enabled = true
            propertySwitchAdditional.enabled = true
            propertySwitchAddress.enabled = true
            propertySwitchNotes.enabled = true
            propertySwitchPrintDate.enabled = true
            propertySwitchPdfSign.enabled = true

            mainFormID.propertyPageLoader.propertyGeneralPopUp.close()
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                        propertyTitleSelectData.forceActiveFocus()

        }
        onSignalCardAccessError: {
            console.log("Card Print Page onSignalCardAccessError")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = ""
            if (error_code == GAPI.NoReaderFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
                disableComponents()
            }
            else if (error_code == GAPI.NoCardFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD")
                disableComponents()
            }
            else if (error_code == GAPI.SodCardReadError) {
                bodyPopup = qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
                disableComponents()
            }
            else if (error_code == GAPI.CardUserPinCancel) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
            }
            else if (error_code == GAPI.CardPinTimeout) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Card Print Page onSignalCardChanged")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            var returnSubMenuWhenClosed = false
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                returnSubMenuWhenClosed = true;
                disableComponents()
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                returnSubMenuWhenClosed = true;
                disableComponents()

            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, returnSubMenuWhenClosed)
        }
        onSignalTestPinFinished: {
            if (triesLeft === 3) {
                var titlePopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
                var bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }else {
                propertySwitchAddress.checked = false
            }
        }

    }

    propertyFileDialogOutput {
        onAccepted: {

            outputFile = propertyFileDialogOutput.fileUrl.toString()
            console.log("Output filename on Accepted: " + outputFile)
            
            outputFile = decodeURIComponent(Functions.stripFilePrefix(outputFile))
            /*console.log("Output filename: " + outputFile)*/
            gapi.startPrintPDF(outputFile,
                               propertySwitchBasic.checked,
                               propertySwitchAdditional.checked,
                               propertySwitchAddress.checked,
                               propertySwitchNotes.checked,
                               propertySwitchPrintDate.checked,
                               propertySwitchPdfSign.checked)
        }
    }

    propertyButtonPdf {
        onClicked: {
            propertyFileDialogOutput.filename = propertySwitchPdfSign.checked ? "CartaoCidadao_signed.pdf" : "CartaoCidadao.pdf"
            propertyFileDialogOutput.open()
        }
    }
    propertyButtonPrint {
        onClicked: {
            console.log("Printing file")
            gapi.startPrint("",
                               propertySwitchBasic.checked,
                               propertySwitchAdditional.checked,
                               propertySwitchAddress.checked,
                               propertySwitchNotes.checked,
                               propertySwitchPrintDate.checked,
                               propertySwitchPdfSign.checked)
        }
    }

    propertySwitchAddress{
        onCheckedChanged: {
            if(propertySwitchAddress.checked){
                gapi.verifyAddressPin("")
            }
        }
    }

    Component.onCompleted: {
        console.log("Page Card Print mainWindow Completed")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }

    function disableComponents(){
        propertySwitchBasic.enabled = false
        propertySwitchAdditional.enabled = false
        propertySwitchAddress.enabled = false
        propertySwitchNotes.enabled = false
        propertySwitchPrintDate.enabled = false
        propertySwitchPdfSign.enabled = false

        propertySwitchBasic.checked = false
        propertySwitchAdditional.checked = false
        propertySwitchAddress.checked = false
        propertySwitchNotes.checked = false
        propertySwitchPrintDate.checked = false
        propertySwitchPdfSign.checked = false
    }
    
    function showCreatedFile(){
        if (Qt.platform.os === "windows") {
            if (outputFile.substring(0, 2) === "//") {
                outputFile = "file:" + outputFile
            } else {
                outputFile = "file:///" + outputFile
            }
        }else{
            outputFile = "file://" + outputFile
        }
        /*console.log("Open Url Externally: " + outputFile)*/
        Qt.openUrlExternally(outputFile)
        createsuccess_dialog.close()
        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
    }
}
