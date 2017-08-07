import QtQuick 2.6
import QtQuick.Controls 2.1

//Import C++ defined enums
import eidguiV2 1.0

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

    Connections {
        target: gapi
        onSignalPdfSignSucess: {
            signsuccess_dialog.visible = true
        }

        onSignalPdfSignError: {
            signerror_dialog.visible = true
        }
        onSignalCardDataChanged: {
            console.log("Services Sign Advanced --> Data Changed")
            //console.trace();
            propertySigReasonText.text = "{ Motivo da assinatura }"
            propertySigSignedByText.text = "Assinado por: "

            propertySigSignedByNameText.text = gapi.getDataCardIdentifyValue(GAPI.Givenname)
                  + " " +  gapi.getDataCardIdentifyValue(GAPI.Surname)

            propertySigNumIdText.text = "Num. de Identificação Civil: "
                    + gapi.getDataCardIdentifyValue(GAPI.Documentnum)
            propertySigLocationText.text = "{ Localização da assinatura }"

            propertySigImg.source = "qrc:/images/logo_CC.png"
            propertySigWaterImg.source = "qrc:/images/pteid_signature_watermark.jpg"

            propertyBusyIndicator.running = false
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
                if (Qt.platform.os === "windows") {
                    path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
                }else{
                    path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
                }
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
            if (Qt.platform.os === "windows") {
                path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }
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


    Component.onCompleted: {
        if (gapi.getShortcutFlag() > 0)
            filesModel.append(
                        {
                            "fileUrl": gapi.getShortcutInputPDF()
                        });

        console.log("Page Difinitions Signature mainWindowCompleted")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
        propertySigDateText.text = getData()
    }
    function getData(){
        var time = Qt.formatDateTime(new Date(), "yy.MM.dd hh:mm:ss")

        function pad(number, length){
            var str = "" + number
            while (str.length < length) {
                str = '0'+str
            }
            return str
        }

        var offset = new Date().getTimezoneOffset()
        offset = ((offset<0? '+':'-')+ // Note the reversed sign!
                  pad(parseInt(Math.abs(offset/60)), 2)+
                  pad(Math.abs(offset%60), 2))

        time += " " + offset

        return time
    }
}
