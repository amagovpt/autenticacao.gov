/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

PageCardOtherDataForm {

    Keys.onPressed: {
        console.log("PageCardOtherDataForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

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
            mainFormID.propertyPageLoader.propertyGeneralPopUp.close()
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                propertyRectTaxNum.forceActiveFocus()
        }
        onSignalShowCardActivation: {
            propertyBusyIndicator.running = false
            propertyTextBoxCardState.propertyDateField.text = statusMessage
        }

        onSignalCardAccessError: {
            console.log("Card Other Data onSignalCardAccessError")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = ""
            if (error_code == GAPI.NoReaderFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
            }
            else if (error_code == GAPI.NoCardFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD")
            }
            else if (error_code == GAPI.SodCardReadError) {
                bodyPopup = qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
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
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            var returnSubMenuWhenClosed = false
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                returnSubMenuWhenClosed = true;
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
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                returnSubMenuWhenClosed = true;
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, returnSubMenuWhenClosed)
        }
    }

    Component.onCompleted: {
        console.log("Page Card Identify mainWindow Completed")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
