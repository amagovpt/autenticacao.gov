import QtQuick 2.6
import QtQuick.Controls 2.1
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageCardPrintForm {

    Dialog {
        id: dialogPinOK
        width: 400
        height: 200
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogPinOK.width * 0.5
        y: parent.height * 0.5 - dialogPinOK.height * 0.5

        property alias propertyDialogOkLabelText: dialogOkLabelText
        property alias propertyTextOkPin: textOkPin

        header: Label {
            id: dialogOkLabelText
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectOkPin.height

            Item {
                id: rectOkPin
                width: parent.width
                height: 90
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textOkPin
                    text: qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: 150
                }
            }
            Button {
                width: Constants.WIDTH_BUTTON
                height:Constants.HEIGHT_BOTTOM_COMPONENT
                text: "OK"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectOkPin.bottom
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: dialogPinOK.close()
            }
        }
    }
    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalPdfPrintSignSucess: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text = qsTr("STR_PRINT_CREATE_PDF")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = qsTr("STR_PRINT_CREATE_PDF_SUCESS")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalPdfPrintSucess: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text = qsTr("STR_PRINT_CREATE_PDF")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = qsTr("STR_PRINT_CREATE_PDF_SUCESS")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalPdfPrintFail: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text = qsTr("STR_PRINT_CREATE_PDF")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  qsTr("STR_PRINT_CREATE_PDF_FAIL")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalPrinterPrintSucess: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text = qsTr("STR_PRINT_PRINTER")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = qsTr("STR_PRINT_PRINTER_SUCESS")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalPrinterPrintFail: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text = qsTr("STR_PRINT_PRINTER")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  qsTr("STR_PRINT_PRINTER_FAIL")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalCardDataChanged: {
            console.log("Data Card Print --> Data Changed")
            propertyBusyIndicator.running = false
            propertyButtonPrint.enabled = true
            propertyButtonPdf.enabled = true
            propertySwitchBasic.enabled = true
            propertySwitchAdditional.enabled = true
            propertySwitchAddress.enabled = true
            propertySwitchNotes.enabled = true
            propertySwitchPdfSign.enabled = true
            propertySwitchBasic.checked = false
            propertySwitchAdditional.checked = false
            propertySwitchAddress.checked = false
            propertySwitchNotes.checked = false
            propertySwitchPdfSign.checked = false
        }
        onSignalCardAccessError: {
            console.log("Card Print Page onSignalCardAccessError")
            if (error_code != GAPI.CardUserPinCancel){
                if (error_code == GAPI.NoReaderFound) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
                    disableComponents()
                }
                else if (error_code == GAPI.NoCardFound) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_NO_CARD")
                    disableComponents()
                }
                else if (error_code == GAPI.SodCardReadError) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
                    disableComponents()
                }
                else {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
                }
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            }
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Card Print Page onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                         qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")

            }
            disableComponents()
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }

    }

    propertyFileDialogOutput {
        onAccepted: {

            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }

            console.log("Output filename: " + outputFile)
            gapi.startPrintPDF(outputFile,
                               propertySwitchBasic.checked,
                               propertySwitchAdditional.checked,
                               propertySwitchAddress.checked,
                               propertySwitchNotes.checked,
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
                               propertySwitchPdfSign.checked)
        }
    }

    propertySwitchAddress{
        onCheckedChanged: {
            if(propertySwitchAddress.checked){
                var triesLeft = 0
                triesLeft = gapi.verifyAddressPin("")
                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_VERIFY")
                    dialogPinOK.propertyTextOkPin.text = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SUCESS")
                    dialogPinOK.open()
                }else {
                    propertySwitchAddress.checked = false
                }
            }
        }
    }
    propertySwitchPdfSign {
        onCheckedChanged: {
            if(propertySwitchPdfSign.checked){
                propertyButtonPrint.enabled = false
                propertyButtonPrint.checked = false
            } else {
                propertyButtonPrint.enabled = true
                propertyButtonPrint.checked = false
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
        propertySwitchPdfSign.enabled = false
        propertySwitchBasic.checked = false
        propertySwitchAdditional.checked = false
        propertySwitchAddress.checked = false
        propertySwitchNotes.checked = false
        propertySwitchPdfSign.checked = false
        propertyButtonPrint.enabled = false
        propertyButtonPdf.enabled = false
    }
}
