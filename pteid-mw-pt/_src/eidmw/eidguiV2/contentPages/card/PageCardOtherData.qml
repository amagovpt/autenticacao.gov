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
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  "Cartão do Cidadão removido"
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
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "Cartão do Cidadão inserido"
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        "Erro da aplicação! Por favor reinstale a aplicação:"
            }

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
        }
    }

    Component.onCompleted: {
        console.log("Page Card Identify mainWindow Completed")

        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
