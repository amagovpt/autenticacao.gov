import QtQuick 2.6
//Import C++ defined enums
import eidguiV2 1.0

PageCardIdentifyForm {

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

    }

    Component.onCompleted: {
        console.log("Page Card Identify mainWindow Completed")

        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
