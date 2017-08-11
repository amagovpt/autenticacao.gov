import QtQuick 2.6
import QtQuick.Controls 2.1
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageCardPrintForm {

    Connections {
        target: gapi
        onSignalCardDataChanged: {
            console.log("Data Card Print --> Data Changed")
            propertyBusyIndicator.running = false
        }
        onSignalCardAccessError: {
            console.log("Card Print Page onSignalCardAccessError")
            if (error_code == GAPI.NoReaderFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Error"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  "No card reader found!"
            }
            else if (error_code == GAPI.NoCardFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Error"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "No Card Found!"
            }
            else if (error_code == GAPI.SodCardReadError) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Error"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        "Consistência da informação do cartão está comprometida!"
            }
            else {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Error"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "Card Access Error!"
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;

            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Card Print Page onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  "Cartão do Cidadão removido"
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
        console.log("Page Card Print mainWindow Completed")

        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
