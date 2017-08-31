import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

PageDefinitionsAppForm {
    Connections {
        target: gapi
        onSignalSetReaderComboIndex: {
            console.log("onSignalSetReaderComboIndex index = " + selected_reader)
            propertyComboBoxReader.currentIndex = selected_reader
        }
    }

    propertyComboBoxReader.onActivated:  {
            console.log("propertyComboBoxReader onActivated index = "
                        + propertyComboBoxReader.currentIndex)
            gapi.setReaderByUser(propertyComboBoxReader.currentIndex)
    }

    Component.onCompleted: {
        console.log("Page difinitions onCompleted")

        for ( var i = 0; i < gapi.getRetReaderList().length; ++i ) {
            console.log("Reader List " + "i = " + i +" : "+ gapi.getRetReaderList()[i])
            propertyComboBoxReader.model = gapi.getRetReaderList()
        }

        console.log("Page difinitions onCompleted finished")
    }
}
