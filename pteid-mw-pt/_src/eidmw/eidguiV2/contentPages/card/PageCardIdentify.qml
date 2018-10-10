import QtQuick 2.6
import QtQuick.Controls 2.1
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageCardIdentifyForm {

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalReaderContext: {
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Data Card Identify --> Data Changed")
            //console.trace();
            propertyTextBoxName.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Givenname)
            propertyTextBoxSurName.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Surname)
            propertyTextBoxSex.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Sex)
            propertyTextBoxHeight.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Height)

                     
            propertyTextBoxNacionality.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Nationality)
            propertyTextBoxDateOfBirth.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Birthdate)
            propertyTextBoxDocumentNum.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Documentnum)
            propertyTextBoxExpirydate.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Validityenddate)
            propertyTextBoxCountry.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Country)
            propertyTextBoxParentsFather.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Father)
            propertyTextBoxParentsMother.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Mother)
            propertyTextBoxNotes.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.AccidentalIndications)
            propertyPhoto.source = ""
            propertyPhoto.cache = false
            propertyPhoto.source = "image://myimageprovider/photo.png"
            propertyBusyIndicator.running = false
        }
        onSignalCardAccessError: {
            console.log("Card Identify Page onSignalCardAccessError")
           
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
            

            propertyTextBoxName.propertyDateField.text = ""
            propertyTextBoxSurName.propertyDateField.text = ""
            propertyTextBoxSex.propertyDateField.text = ""
            propertyTextBoxHeight.propertyDateField.text = ""
            propertyTextBoxNacionality.propertyDateField.text = ""
            propertyTextBoxDateOfBirth.propertyDateField.text = ""
            propertyTextBoxDocumentNum.propertyDateField.text = ""
            propertyTextBoxExpirydate.propertyDateField.text = ""
            propertyTextBoxCountry.propertyDateField.text = ""
            propertyTextBoxParentsFather.propertyDateField.text = ""
            propertyTextBoxParentsMother.propertyDateField.text = ""
            propertyTextBoxNotes.propertyDateField.text = ""
            propertyPhoto.source = ""
            propertyPhoto.cache = false
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Card Identify Page onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyTextBoxName.propertyDateField.text = ""
                propertyTextBoxSurName.propertyDateField.text = ""
                propertyTextBoxSex.propertyDateField.text = ""
                propertyTextBoxHeight.propertyDateField.text = ""
                propertyTextBoxNacionality.propertyDateField.text = ""
                propertyTextBoxDateOfBirth.propertyDateField.text = ""
                propertyTextBoxDocumentNum.propertyDateField.text = ""
                propertyTextBoxExpirydate.propertyDateField.text = ""
                propertyTextBoxCountry.propertyDateField.text = ""
                propertyTextBoxParentsFather.propertyDateField.text = ""
                propertyTextBoxParentsMother.propertyDateField.text = ""
                propertyTextBoxNotes.propertyDateField.text = ""
                propertyPhoto.source = ""
                propertyPhoto.cache = false
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
