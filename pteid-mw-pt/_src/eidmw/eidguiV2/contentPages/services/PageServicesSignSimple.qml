import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageServicesSignSimpleForm {

    Dialog {
        id: dialog
        title: "Arraste um único ficheiro"
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Label {
            text: "Para assinar múltiplos ficheiros use a opção assinatura avançada"
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
            propertyPDFPreview.propertyDragSigSignedByNameText.text = "Assinado por:"
                    + gapi.getDataCardIdentifyValue(GAPI.Givenname)
                    + " " +  gapi.getDataCardIdentifyValue(GAPI.Surname)

            propertyPDFPreview.propertyDragSigNumIdText.text = "Num. de Identificação Civil:"
                    + gapi.getDataCardIdentifyValue(GAPI.Documentnum)
            propertyBusyIndicator.running = false
        }
    }
    Connections {
        target: image_provider_pdf
        onSignalPdfSourceChanged: {
            console.log("Receive signal onSignalPdfSourceChanged pdfWidth = "+pdfWidth+" pdfHeight = "+pdfHeight);
            propertyPDFPreview.propertyPdfOriginalWidth=pdfWidth
            propertyPDFPreview.propertyPdfOriginalHeight=pdfHeight
            propertyBusyIndicator.running = false
        }
    }
    Dialog {
        id: signsuccess_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - signsuccess_dialog.width * 0.5
        y: parent.height * 0.5 - signsuccess_dialog.height * 0.5

        header: Label {
            text: "Ficheiro(s) assinado(s) com sucesso"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        standardButtons: DialogButtonBox.Ok
    }

    Dialog {
        id: signerror_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - signerror_dialog.width * 0.5
        y: parent.height * 0.5 - signerror_dialog.height * 0.5

        header: Label {
            text: "Erro na assinatura de PDF"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        standardButtons: DialogButtonBox.Ok
    }

    propertyDropArea {

        onEntered: {
            console.log("You chose file(s): " + drag.urls);
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
    propertyFileDialogOutput {
        onAccepted: {

            var loadedFilePath = filesModel.get(0).fileUrl
            var isTimestamp = false
            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }

                //TODO: we need a way to change the page
                var page = 1

                var reason = ""
                var location = ""

                var isSmallSignature = false

                var coord_x = propertyPDFPreview.propertyCoordX

                //coord_y must be the lower left corner of the signature rectangle

                var coord_y = propertyPDFPreview.propertyCoordY

                console.log("Output filename: " + outputFile)
                console.log("Signing in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y)

                gapi.startSigningPDF(loadedFilePath, outputFile, page, coord_x, coord_y,
                                     reason, location, isTimestamp, isSmallSignature)
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

    propertyMouseAreaRectMain {
        onClicked: {
            console.log("propertyMouseAreaRectMainRigh clicked")
            propertyFileDialog.visible = true
        }
    }

    propertyButtonRemove {
        onClicked: {
            fileLoaded = false
            filesModel.clear()
        }
    }
    propertyButtonSignWithCC {
        onClicked: {
            console.log("Sign with CC")
            propertyFileDialogOutput.filename = filesModel.get(0).fileUrl + "_signed.pdf"
            propertyFileDialogOutput.open()
        }
    }

    ListModel {
        id: filesModel

        onCountChanged: {
            console.log("filesModel onCountChanged count:"
                        + filesModel.count)
            if(filesModel.count === 0){
                fileLoaded = false
                propertyPDFPreview.propertyBackground.source = ""
                propertyPDFPreview.propertyDragSigImg.visible = false
                propertyPDFPreview.propertyDragSigSignedByText.visible = false
                propertyPDFPreview.propertyDragSigSignedByNameText.visible = false
                propertyPDFPreview.propertyDragSigNumIdText.visible = false
                propertyPDFPreview.propertyDragSigDateText.visible = false
                propertyPDFPreview.propertyDragSigImg.visible = false
            }else{
                fileLoaded = true
                propertyBusyIndicator.running = true
                var loadedFilePath = filesModel.get(0).fileUrl
                console.log("loadedFilePath: " + loadedFilePath)
                propertyPDFPreview.propertyBackground.cache = false
                propertyPDFPreview.propertyBackground.source = "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=1"
                propertyPDFPreview.propertyDragSigImg.source = "qrc:/images/logo_CC.png"
                propertyPDFPreview.propertyDragSigImg.visible = true
                propertyPDFPreview.propertyDragSigWaterImg.source = "qrc:/images/pteid_signature_watermark.jpg"
                propertyPDFPreview.propertyDragSigWaterImg.visible = true
                propertyPDFPreview.propertyDragSigSignedByText.visible = true
                propertyPDFPreview.propertyDragSigSignedByNameText.visible = true
                propertyPDFPreview.propertyDragSigNumIdText.visible = true
                propertyPDFPreview.propertyDragSigDateText.visible = true
                propertyPDFPreview.propertyDragSigImg.visible = true
            }
        }
    }
    Component.onCompleted: {
        if (gapi.getShortcutFlag() > 0)
            filesModel.append(
                        {
                            "fileUrl": gapi.getShortcutInputPDF()
                        });

        console.log("Page Services Sign Simple mainWindowCompleted")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
