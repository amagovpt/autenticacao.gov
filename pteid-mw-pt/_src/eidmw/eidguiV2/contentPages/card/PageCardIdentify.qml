import QtQuick 2.6

PageCardIdentifyForm {

    Connections {
        target: gapi
        onSignalCardIdentifyChanged: {
            console.log("Data Card Identify --> Data Changed")
            propertyTextBoxName.propertyDateField.text = gapi.getDataCardIdentifyValue("name")
            propertyTextBoxSurName.propertyDateField.text = gapi.getDataCardIdentifyValue("givenname")
            propertyTextBoxSex.propertyDateField.text = gapi.getDataCardIdentifyValue("sex")
            propertyTextBoxHeight.propertyDateField.text = gapi.getDataCardIdentifyValue("height")
            propertyTextBoxNacionality.propertyDateField.text = gapi.getDataCardIdentifyValue("nationality")
            propertyTextBoxDateOfBirth.propertyDateField.text = gapi.getDataCardIdentifyValue("birthdate")
            propertyTextBoxDocumentNum.propertyDateField.text = gapi.getDataCardIdentifyValue("documentnumber")
            propertyTextBoxExpirydate.propertyDateField.text = gapi.getDataCardIdentifyValue("card_validuntil")
            propertyTextBoxCountry.propertyDateField.text = gapi.getDataCardIdentifyValue("country")
            propertyTextBoxParentsFather.propertyDateField.text = gapi.getDataCardIdentifyValue("father")
            propertyTextBoxParentsMother.propertyDateField.text = gapi.getDataCardIdentifyValue("mother")
            propertyTextBoxNotes.propertyDateField.text = gapi.getDataCardIdentifyValue("ACCIDENTALINDICATIONS")
        }
    }

    propertyTextBoxName{
        propertyDateField.text: gapi.getDataCardIdentifyValue("name")
    }
    propertyTextBoxSurName{
        propertyDateField.text: gapi.getDataCardIdentifyValue("givenname")
    }
    propertyTextBoxSex{
        propertyDateField.text: gapi.getDataCardIdentifyValue("sex")
    }
    propertyTextBoxHeight{
        propertyDateField.text: gapi.getDataCardIdentifyValue("height")
    }
    propertyTextBoxNacionality{
        propertyDateField.text: gapi.getDataCardIdentifyValue("nationality")
    }
    propertyTextBoxDateOfBirth{
        propertyDateField.text: gapi.getDataCardIdentifyValue("birthdate")
    }
    propertyTextBoxDocumentNum{
        propertyDateField.text: gapi.getDataCardIdentifyValue("documentnumber")
    }
    propertyTextBoxExpirydate{
        propertyDateField.text: gapi.getDataCardIdentifyValue("card_validuntil")
    }
    propertyTextBoxCountry{
        propertyDateField.text: gapi.getDataCardIdentifyValue("country")
    }
    propertyTextBoxParentsFather{
        propertyDateField.text: gapi.getDataCardIdentifyValue("father")
    }
    propertyTextBoxParentsMother{
        propertyDateField.text: gapi.getDataCardIdentifyValue("mother")
    }
    propertyTextBoxNotes{
        propertyDateField.text: gapi.getDataCardIdentifyValue("ACCIDENTALINDICATIONS")
    }

    Component.onCompleted: {
        console.log("Page Card Identify mainWindow Completed")
    }
}
