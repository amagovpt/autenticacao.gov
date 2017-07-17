import QtQuick 2.6
import QtQuick.Controls 2.1
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageCardIdentifyForm {
    property alias propertyLabelText: labelText
    Dialog {
        id: errorDialog
        width: 400
        height: 200
        visible: false

        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - errorDialog.width * 0.5
        y: parent.height * 0.5 - errorDialog.height * 0.5

        header: Label {
              id: labelText
              elide: Label.ElideRight
              padding: 24
              bottomPadding: 0
              font.bold: true
              font.pixelSize: 16
              color: Constants.COLOR_MAIN_BLUE
        }
        standardButtons: DialogButtonBox.Ok
    }

    Connections {
        target: gapi
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
            propertyPhoto.source = "image://myimageprovider/photo.png"
            propertyBusyIndicator.running = false
        }

        onSignalCardAccessError: {
            propertyBusyIndicator.running = false
            if (error_code == GAPI.NoReaderFound) {
                propertyLabelText.text = "Error: No card reader found!"
            }
            else if (error_code == GAPI.NoCardFound) {
                propertyLabelText.text = "Error: No Card Found!"
            }
            errorDialog.visible = true
        }

    }

    Component.onCompleted: {
        console.log("Page Card Identify mainWindow Completed")

        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
