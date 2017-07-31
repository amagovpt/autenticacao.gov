import QtQuick 2.6

//Import C++ defined enums
import eidguiV2 1.0

PageCardOtherDataForm {

    Connections {
        target: gapi
        onSignalCardDataChanged: {
            console.log("Data Card Identify --> Data Changed")
            propertyTextBoxNIF.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.NIF)
            propertyTextBoxNISS.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.NISS)
            propertyTextBoxNSNS.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.NSNS)
            propertyTextBoxCardVersion.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Documentversion)
            propertyTextBoxIssueDate.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Validitybegindate)
            propertyTextBoxIssuingEntity.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.IssuingEntity)
            propertyTextBoxDocumentType.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.Documenttype)
            propertyTextBoxPlaceOfRequest.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.PlaceOfRequest)
            propertyTextBoxCardState.propertyDateField.text = gapi.getCardActivation()
            propertyBusyIndicator.running = false
        }

        onSignalCardAccessError: {
            propertyBusyIndicator.running = false
            if (error_code == GAPI.NoReaderFound) {
                propertyGeneralTitleText.text =  "Error"
                propertyGeneralPopUpLabelText.text =  "No card reader found!"
            }
            else if (error_code == GAPI.NoCardFound) {
                propertyGeneralTitleText.text =  "Error"
                propertyGeneralPopUpLabelText.text = "No Card Found!"
            }
            propertyGeneralPopUp.visible = true;
        }
    }

    Component.onCompleted: {
        console.log("Page Card Identify mainWindow Completed")

        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
