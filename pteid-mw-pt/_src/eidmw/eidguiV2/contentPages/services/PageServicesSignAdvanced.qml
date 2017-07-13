import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

PageServicesSignAdvancedForm {

    ToolTip {
        id: controlToolTip
        timeout: 5000
        contentItem: Text {
            text: controlToolTip.text
            font: controlToolTip.font
            color: Constants.COLOR_MAIN_PRETO
        }
        background: Rectangle {
            border.color: Constants.COLOR_MAIN_DARK_GRAY
            color: Constants.COLOR_MAIN_SOFT_GRAY
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

    /*
    propertyMouseAreaToolTipPades{
        onEntered: {
            controlToolTip.close()
            controlToolTip.text = "Ficheiros PDF"
            controlToolTip.x = propertyMouseAreaToolTipPadesX - controlToolTip.width * 0.5
            controlToolTip.y = propertyMouseAreaToolTipY + 22
            controlToolTip.open()
        }
    }
    propertyMouseAreaToolTipXades{
        onEntered: {
            controlToolTip.close()
            controlToolTip.text = "Pacote XADES / CCSIGN"
            controlToolTip.x = propertyMouseAreaToolTipXadesX - controlToolTip.width * 0.5
            controlToolTip.y = propertyMouseAreaToolTipY + 22
            controlToolTip.open()
        }
    }
    */
    propertyDropArea {

        onEntered: {
            console.log("You chose file(s): " + drag.urls);
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            for(var i = 0; i < filesArray.length; i++){
                console.log("Adding file: " + filesArray[i])
                var path =  filesArray[i]
                //  Get the path itself without a regex
                path = path.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");
                filesModel.append({
                                      "fileUrl": path
                                  })
            }
            // Force scroll and focus to the last item addded
            forceScrollandFocus()
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }

    propertyFileDialogOutput {
        onAccepted: {

            var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
            var isTimestamp = propertySwitchSignTemp.position
            var outputFile = propertyFileDialogOutput.fileUrl

            if (propertyRadioButtonPADES.checked) {
                //TODO: we need a way to change the page
                var page = 1

                var reason = propertyTextFieldReason.text
                var location = propertyTextFieldLocal.text

                //TODO: we need to read this value from settings
                var isSmallSignature = false
                var coord_x = propertyPDFPreview.propertyCoordX / 295.0

                var sig_height = propertyPDFPreview.propertyDragImage.height
                //coord_y must be the lower left corner of the signature rectangle
                var coord_y = (propertyPDFPreview.propertyCoordY + sig_height) / 415.0

                console.log("Output filename: " + outputFile)
                console.log("Signing in position coord_x: " + coord_x  + " and coord_y: "+coord_y)

                gapi.startSigningPDF(loadedFilePath, outputFile, page, coord_x, coord_y,
                                     reason, location, isTimestamp, isSmallSignature)
            }
            else {
                console.debug("XADES signing is not implemented yet...")

                gapi.startSigningXADES(loadedFilePath, outputFile, isTimestamp)
            }

        }
    }

    propertyFileDialog {

        onAccepted: {
            console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            console.log("Num files: " + propertyFileDialog.fileUrls.length)

            for(var i = 0; i < propertyFileDialog.fileUrls.length; i++){
                console.log("Adding file: " + propertyFileDialog.fileUrls[i])
                var path = propertyFileDialog.fileUrls[i];
                //  Get the path itself without a regex
                path = path.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");
                filesModel.append({
                                      "fileUrl": path
                                  })
            }

            // Force scroll and focus to the last item addded
            forceScrollandFocus()
        }
        onRejected: {
            console.log("Canceled")
        }
    }

/*
    propertyMouseAreaRectMainRigh {
        onClicked: {
            console.log("propertyMouseAreaRectMainRigh clicked")
            propertyFileDialog.visible = true
        }
    }
    */
    propertyMouseAreaItemOptionsFiles {
        onClicked: {
            console.log("propertyMouseAreaItemOptionsFiles clicked")
            propertyFileDialog.visible = true
        }
    }
    propertyButtonAdd {
        onClicked: {
            console.log("propertyButtonAdd clicked")
            propertyFileDialog.visible = true
        }
    }
    propertyButtonRemoveAll {
        onClicked: {
            console.log("Removing all files")
            propertyListViewFiles.model.clear()
        }
    }

    propertyButtonSignWithCC {
        onClicked: {
            console.log("Sign with CC")
            if (propertyRadioButtonPADES.checked) {
                propertyFileDialogOutput.filename = propertyListViewFiles.model.get(0).fileUrl + "_signed.pdf"
            }
            else {
                propertyFileDialogOutput.filename = "xadessign.ccsigned"
            }

            propertyFileDialogOutput.open()
        }
    }

    Component {
        id: listViewFilesDelegate
        Rectangle{
            id: rectlistViewFilesDelegate
            width: parent.width - propertyFilesListViewScroll.width
                   - Constants.SIZE_ROW_H_SPACE * 0.5
            color: getColorItem(mouseAreaFileName.containsMouse,
                                mouseAreaIconDelete.containsMouse)
            MouseArea {
                id: mouseAreaFileName
                anchors.fill: parent
                hoverEnabled : true
            }

            Item {
                width: parent.width
                height: parent.height

                Text {
                    id: fileName
                    text: fileUrl
                    width: parent.width - iconRemove.width - Constants.SIZE_LISTVIEW_IMAGE_SPACE
                    x: Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: Constants.COLOR_TEXT_BODY
                    wrapMode: Text.WrapAnywhere
                    Component.onCompleted: {
                        if(fileName.height > iconRemove.height){
                            rectlistViewFilesDelegate.height = fileName.height + 5
                        }else{
                            rectlistViewFilesDelegate.height = iconRemove.height
                        }
                    }
                }
                Image {
                    id: iconRemove
                    x: fileName.width + Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    width: Constants.SIZE_IMAGE_FILE_REMOVE
                    height: Constants.SIZE_IMAGE_FILE_REMOVE
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter
                    source:  mouseAreaIconDelete.containsMouse ?
                                 "../../images/remove_file_hover.png" :
                                 "../../images/remove_file.png"
                    MouseArea {
                        id: mouseAreaIconDelete
                        anchors.fill: parent
                        hoverEnabled : true
                        onClicked: {
                            console.log("Delete file index:" + index);
                            filesModel.remove(index)
                        }
                    }
                }
            }
        }
    }
    ListModel {
        id: filesModel

        onCountChanged: {
            console.log("filesModel onCountChanged count:"
                        + propertyListViewFiles.count)
            if(filesModel.count === 0) {
                fileLoaded = false
                propertyRadioButtonPADES.checked = false
                propertyRadioButtonXADES.checked = false
                propertyPDFPreview.propertyBackground.source = ""
                propertyPDFPreview.propertyDragImage.source = ""
            }
            else {
                fileLoaded = true
                var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
                console.log("loadedFilePath: " + loadedFilePath)
                propertyPDFPreview.propertyBackground.source = "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=1"
                propertyPDFPreview.propertyDragImage.source = "qrc:/images/pteid_signature_small.png"
            }
        }
    }

    Component.onCompleted: {
        if (gapi.getShortcutFlag() > 0)
            filesModel.append(
            {
             "fileUrl": gapi.getShortcutInputPDF()
            });
    }

    function forceScrollandFocus() {
        // Force scroll and focus to the last item added
        propertyListViewFiles.positionViewAtEnd()
        propertyListViewFiles.forceActiveFocus()
        propertyListViewFiles.currentIndex = propertyListViewFiles.count -1
        if(propertyFilesListViewScroll.position > 0)
            propertyFilesListViewScroll.active = true
    }
    function getColorItem(mouseItem, mouseImage){

        var handColor
        if(mouseItem || mouseImage){
            handColor = Constants.COLOR_MAIN_DARK_GRAY
        }else{
            handColor = Constants.COLOR_MAIN_SOFT_GRAY
        }
        return handColor
    }
}
