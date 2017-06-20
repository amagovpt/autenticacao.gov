import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

PageDefinitionsSignatureForm {

    Dialog {
        id: dialog
        title: "Arraste um único ficheiro"
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Label {
            text: "Só é possível seleccionar um ficheiro de assinatura personalizada"
        }
    }

    propertyDropArea {

        onEntered: {
            console.log("You chose file(s): " + drag.urls.toString());
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            if(filesArray.length > 1){
                dialog.open()
            }else{
                console.log("Adding file: " + filesArray[0])
                var path =  filesArray[0]
                //  Get the path itself without a regex
                path = path.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");
                filesModel.append({
                                      "fileUrl": path
                                  })
            }
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }

    propertyFileDialog {

        onAccepted: {
            console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            console.log("Adding file: " + propertyFileDialog.fileUrls[0])
            var path = propertyFileDialog.fileUrls[0];
            //  Get the path itself without a regex
            path = path.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");
            filesModel.append({
                                  "fileUrl": path
                              })
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    propertyMouseAreaPreCustom {
        onClicked: {
            console.log("propertyMouseAreaPreCustom clicked")
            propertyFileDialog.visible = true
        }
    }

    propertyButtonAdd {
        onClicked: {
            propertyFileDialog.visible = true
        }
    }

    propertyButtonRemove {
        onClicked: {
            fileLoaded = false
            propertyRadioButtonDefault.checked = true
            propertyRadioButtonCustom.checked = false
        }
    }
    ListModel {
        id: filesModel

        onCountChanged: {
            console.log("filesModel onCountChanged")
            if(filesModel.count === 0){
                fileLoaded = false
            }else{
                fileLoaded = true
                propertyRadioButtonDefault.checked = false
                propertyRadioButtonCustom.checked = true
            }
        }
    }
}
