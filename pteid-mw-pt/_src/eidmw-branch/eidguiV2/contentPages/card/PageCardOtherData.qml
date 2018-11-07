import QtQuick 2.6

//Import C++ defined enums
import eidguiV2 1.0

PageCardOtherDataForm {

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Data Card Identify --> Data Changed")
            gapi.startGetCardActivation()
            propertyTextBoxNIF.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.NIF)
            propertyTextBoxNISS.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.NISS)
            propertyTextBoxNSNS.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.NSNS)
            propertyTextBoxCardVersion.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Documentversion)
            propertyTextBoxIssueDate.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Validitybegindate)
            propertyTextBoxIssuingEntity.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.IssuingEntity)
            propertyTextBoxDocumentType.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Documenttype)
            propertyTextBoxPlaceOfRequest.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.PlaceOfRequest)
            propertyTextBoxCardState.propertyDateField.text = ""

        }
        onSignalShowCardActivation: {
            propertyBusyIndicator.running = false
            propertyTextBoxCardState.propertyDateField.text = statusMessage
        }

        onSignalCardAccessError: {
            console.log("Card Other Data onSignalCardAccessError")
            if (error_code == GAPI.NoReaderFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
            }
            else if (error_code == GAPI.NoCardFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_NO_CARD")
            }
            else if (error_code == GAPI.SodCardReadError) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
            }
            else if (error_code == GAPI.CardUserPinCancel) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
            }
			else if (error_code == GAPI.CardPinTimeout) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            propertyTextBoxNIF.propertyDateField.text = ""
            propertyTextBoxNISS.propertyDateField.text = ""
            propertyTextBoxNSNS.propertyDateField.text = ""
            propertyTextBoxCardVersion.propertyDateField.text = ""
            propertyTextBoxIssueDate.propertyDateField.text = ""
            propertyTextBoxIssuingEntity.propertyDateField.text = ""
            propertyTextBoxDocumentType.propertyDateField.text = ""
            propertyTextBoxPlaceOfRequest.propertyDateField.text = ""
            propertyTextBoxCardState.propertyDateField.text = ""
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Card Other Data Page onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyTextBoxNIF.propertyDateField.text = ""
                propertyTextBoxNISS.propertyDateField.text = ""
                propertyTextBoxNSNS.propertyDateField.text = ""
                propertyTextBoxCardVersion.propertyDateField.text = ""
                propertyTextBoxIssueDate.propertyDateField.text = ""
                propertyTextBoxIssuingEntity.propertyDateField.text = ""
                propertyTextBoxDocumentType.propertyDateField.text = ""
                propertyTextBoxPlaceOfRequest.propertyDateField.text = ""
                propertyTextBoxCardState.propertyDateField.text = ""
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

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
    }

    Component.onCompleted: {
        console.log("Page Card Identify mainWindow Completed")

        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
