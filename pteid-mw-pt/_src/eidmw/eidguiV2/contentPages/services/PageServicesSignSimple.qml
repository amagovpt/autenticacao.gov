import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageServicesSignSimpleForm {

    property string propertyOutputSignedFile : ""

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        
        onSignalPdfSignSucess: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            signsuccess_dialog.visible = true
            propertyBusyIndicator.running = false
        }
        onSignalPdfSignFail: {
            console.log("Sign failed with error code: " + error_code)

            if (error_code == GAPI.SignFilePermissionFailed) {
                signerror_dialog.propertySignFailDialogText.text = qsTranslate("PageServicesSign","STR_SIGN_FILE_PERMISSION_FAIL")
            } else if (error_code == GAPI.PDFFileUnsupported) {
                signerror_dialog.propertySignFailDialogText.text = qsTranslate("PageServicesSign","STR_SIGN_PDF_FILE_UNSUPPORTED")
            } else {
                signerror_dialog.propertySignFailDialogText.text = qsTranslate("PageServicesSign","STR_SIGN_GENERIC_ERROR") + " " + error_code
            }
            signerror_dialog.visible = true
            propertyBusyIndicator.running = false
            propertyOutputSignedFile = ""
        }
        onSignalCardAccessError: {
            console.log("Sign simple Page onSignalCardAccessError")
            if(cardLoaded){
                if (error_code == GAPI.NoReaderFound) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
                }
                else if (error_code == GAPI.NoCardFound) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_NO_CARD")
                }
                else if (error_code == GAPI.SodCardReadError) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
                }
                else if (error_code == GAPI.CardUserPinCancel) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
                }
                else if(error_code == GAPI.PinBlocked) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (error_code == GAPI.CardPinTimeout) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
                }
                else {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
                }
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
                if(error_code != GAPI.CardUserPinCancel && error_code != GAPI.CardPinTimeout)
                    cardLoaded = false
            }
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Services Sign Simple --> Data Changed")
            //console.trace();
            propertyPDFPreview.propertyDragSigSignedByNameText.text =
                    qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
                    + gapi.getDataCardIdentifyValue(GAPI.Givenname)
                    + " " +  gapi.getDataCardIdentifyValue(GAPI.Surname)

            propertyPDFPreview.propertyDragSigNumIdText.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                    + gapi.getDataCardIdentifyValue(GAPI.Documentnum)
            propertyBusyIndicator.running = false
            cardLoaded = true
        }
        onSignalCardChanged: {
            console.log("Services Sign Simple onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyPDFPreview.propertyDragSigSignedByNameText.text =
                        qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
                propertyPDFPreview.propertyDragSigNumIdText.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                cardLoaded = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                cardLoaded = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                cardLoaded = false
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
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
        id: dialog
        title: qsTranslate("Popup File","STR_POPUP_FILE_UNIQUE")
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Label {
            text: qsTranslate("Popup File","STR_POPUP_FILE_UNIQUE_MULTI")
        }
    }
    Components.DialogCMD{
        id: dialogSignCMD
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
            text: qsTranslate("PageServicesSign","STR_SIGN_SUCESS")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: signsuccess_dialog.availableWidth
            height: 50

            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return)
                {
                    signSuccessShowSignedFile()
                }
            }

            Item {
                id: rectLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    text: qsTranslate("PageServicesSign","STR_SIGN_OPEN")
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    wrapMode: Text.Wrap
                }
            }
        }
        Item {
            width: signsuccess_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            y: 80
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageServicesSign","STR_POPUP_FILE_CANCEL")
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        signsuccess_dialog.close()
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    }
                }
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_OPEN")
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        signSuccessShowSignedFile()
                    }
                }
            }
        }
        onRejected:{
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
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

        property alias propertySignFailDialogText: text_sign_error

        header: Label {
            text: qsTranslate("PageServicesSign","STR_SIGN_FAIL")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            width: signerror_dialog.availableWidth
            height: 50
            Text {
                id: text_sign_error
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: parent.height
                width: parent.width - 48
                wrapMode: Text.Wrap
            }
        }

        standardButtons: DialogButtonBox.Ok
    }

    propertyDropArea {

        onEntered: {
            console.log("Signature simple ! You chose file(s): " + drag.urls);

            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            // Update sign preview position variables to be used to send to sdk
            propertyPDFPreview.updateSignPreview(drop.x,drop.y)
            //TODO: Validate files type
            if(filesArray.length > 1){
                dialog.open()
            }else if(filesArray.length == 1){
                // Needed because the PDFPreview
                var path =  filesArray[0]
                //  Get the path itself without a regex
                path = decodeURIComponent(decodeURIComponent(stripFilePrefix(path)))
                console.log("Adding file: " + path)
                var newFileUrl = {"fileUrl": path}
                if (!containsFile(newFileUrl, filesModel)){
                    //guarantees only a single file is used on simple signature
                    filesModel.clear()
                    filesModel.insert(0, newFileUrl);
                }
            }
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }
    propertyFileDialogOutput {
        onAccepted: {
            propertyBusyIndicator.running = true
            var loadedFilePath = filesModel.get(0).fileUrl
            var isTimestamp = false
            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            outputFile = decodeURIComponent(stripFilePrefix(outputFile))

            var page = 1
            propertyCheckLastPage.checked ? page = gapi.getPDFpageCount(loadedFilePath) :
                                            page = propertySpinBoxControl.value
            var reason = ""
            var location = ""

            var isSmallSignature = false

            var coord_x = propertyPDFPreview.propertyCoordX

            //coord_y must be the lower left corner of the signature rectangle

            var coord_y = propertyPDFPreview.propertyCoordY

            console.log("Output filename: " + outputFile)
            console.log("Signing in position coord_x: " + coord_x
                        + " and coord_y: "+coord_y)

            propertyOutputSignedFile = outputFile;

            gapi.startSigningPDF(loadedFilePath, outputFile, page, coord_x, coord_y,
                                 reason, location, isTimestamp, isSmallSignature)
        }
    }
    propertyFileDialogCMDOutput {
        onAccepted: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            dialogSignCMD.open()
        }
    }

    propertyFileDialog {

        onAccepted: {
            console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            var path = propertyFileDialog.fileUrls[0];
            path = decodeURIComponent(stripFilePrefix(path))
            console.log("Adding file: " + path)
            var newFileUrl = {"fileUrl": path}
            if (!containsFile(newFileUrl, filesModel)){
                //guarantees only a single file is used on simple signature
                filesModel.clear()
                filesModel.insert(0, newFileUrl);
            }
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
            if (gapi.getTriesLeftSignPin() === 0) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup PIN","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SIGN_BLOCKED")
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            }else{
                var outputFile = filesModel.get(0).fileUrl
                //Check if filename has extension and remove it.
                if( outputFile.lastIndexOf('.') > 0)
                    var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'));
                propertyFileDialogOutput.filename = outputFile + "_signed.pdf"
                propertyFileDialogOutput.open()
            }
        }
    }
    propertyButtonSignCMD {
        onClicked: {
            console.log("Sign with CMD")
            var outputFile = filesModel.get(0).fileUrl
            //Check if filename has extension and remove it.
            if( outputFile.lastIndexOf('.') > 0)
                var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'));
            propertyFileDialogCMDOutput.filename = outputFile + "_signed.pdf";
            propertyFileDialogCMDOutput.open()
        }
    }

    propertyTextSpinBox{
        text: propertySpinBoxControl.textFromValue(propertySpinBoxControl.value, propertySpinBoxControl.locale)
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
                propertyBusyIndicator.running = true
                var loadedFilePath = filesModel.get(0).fileUrl
                var pageCount = gapi.getPDFpageCount(loadedFilePath)

                if(pageCount > 0){
                    propertyTextSpinBox.maximumLength = maxTextInputLength(pageCount)

                    console.log("loadedFilePath: " + loadedFilePath + " page count: " + pageCount)
                    fileLoaded = true
                    propertyPDFPreview.propertyBackground.cache = false

                    propertySpinBoxControl.to = pageCount

                    if(propertyCheckLastPage.checked == true){
                        propertySpinBoxControl.value = pageCount
                        propertyPDFPreview.propertyBackground.source =
                                "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + pageCount
                    }else{
                        if(propertySpinBoxControl.value > pageCount){
                            propertySpinBoxControl.value = 1
                            propertyPDFPreview.propertyBackground.source
                                    = "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=1"
                        }else{
                            propertyPDFPreview.propertyBackground.source
                                    = "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
                        }
                    }
                    updateIndicators(pageCount)

                    var urlCustomImage = gapi.getCachePath()+"/CustomSignPicture.png"
                    if(gapi.getUseCustomSignature() && gapi.customSignImageExist()){
                        if (Qt.platform.os === "windows") {
                            urlCustomImage = "file:///"+urlCustomImage
                        }else{
                            urlCustomImage = "file://"+urlCustomImage
                        }
                        propertyPDFPreview.propertyDragSigImg.source = urlCustomImage
                    }else{
                        propertyPDFPreview.propertyDragSigImg.source = "qrc:/images/logo_CC.png"
                    }
                    propertyPDFPreview.propertyDragSigImg.visible = true
                    propertyPDFPreview.propertyDragSigWaterImg.source = "qrc:/images/pteid_signature_watermark.jpg"
                    propertyPDFPreview.propertyDragSigWaterImg.visible = true
                    propertyPDFPreview.propertyDragSigSignedByText.visible = true
                    propertyPDFPreview.propertyDragSigSignedByNameText.visible = true
                    propertyPDFPreview.propertyDragSigNumIdText.visible = true
                    propertyPDFPreview.propertyDragSigDateText.visible = true
                    propertyPDFPreview.propertyDragSigImg.visible = true
                }else{
                    propertyBusyIndicator.running = false
                    filesModel.remove(filesModel.count-1)
                    fileLoaded = false
                    propertyPDFPreview.propertyBackground.source = ""
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR")
                    if(pageCount === -1){
                        console.log("Error loading pdf file")
                        mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR_MSG")

                    }else if(pageCount === -2){
                        console.log("Error loading pdf encrypted file")
                        mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                qsTranslate("PageServicesSign","STR_LOAD_ENCRYPTED_PDF_ERROR_MSG")
                    }else{
                        console.log("Generic Error loading pdf file")
                        mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR_MSG")
                    }

                    mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                    mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
                }
            }
        }
    }

    propertySpinBoxControl {
        onValueChanged: {
            var loadedFilePath = filesModel.get(0).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            if(propertySpinBoxControl.value > pageCount){
                propertySpinBoxControl.value = 1
            }
            propertyPDFPreview.propertyBackground.source =
                    "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
            updateIndicators(pageCount)
        }
    }
    propertyCheckLastPage {
        onCheckedChanged: {
            var loadedFilePath = filesModel.get(0).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            propertySpinBoxControl.value = pageCount
            if(propertyCheckLastPage.checked){
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + pageCount
            }else{
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
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

        propertyPDFPreview.propertyDragSigSignedByNameText.text =
                qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
        propertyPDFPreview.propertyDragSigNumIdText.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "

        //  CMD load backup mobile data
        //textFieldMobileNumber.text = propertyPageLoader.propertyBackupMobileNumber

        gapi.startCardReading()
    }
    function updateIndicators(pageCount){
        propertySpinBoxControl.up.indicator.visible = true
        propertySpinBoxControl.down.indicator.visible = true
        propertySpinBoxControl.up.indicator.enabled = true
        propertySpinBoxControl.down.indicator.enabled = true
        if(propertySpinBoxControl.value >= pageCount){
            propertySpinBoxControl.up.indicator.visible = false
            propertySpinBoxControl.up.indicator.enabled = false
        }
        if (propertySpinBoxControl.value === 1){
            propertySpinBoxControl.down.indicator.visible = false
            propertySpinBoxControl.down.indicator.enabled = false
        }
    }
	function openSignedFile(){
        if (Qt.platform.os === "windows") {
			if (propertyOutputSignedFile.substring(0, 2) == "//" ){
				propertyOutputSignedFile = "file:" + propertyOutputSignedFile
			}else{
				propertyOutputSignedFile = "file:///" + propertyOutputSignedFile
			}
        }else{
            propertyOutputSignedFile = "file://" + propertyOutputSignedFile
        }
        console.log("Open Url Externally: " + propertyOutputSignedFile)
        Qt.openUrlExternally(propertyOutputSignedFile)
    }
    function signSuccessShowSignedFile(){
		openSignedFile()
        signsuccess_dialog.close()
        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
    }
    function stripFilePrefix(filePath) {
        if (Qt.platform.os === "windows") {
            return filePath.replace(/^(file:\/{3})|(file:)|(qrc:\/{3})|(http:\/{3})/,"")
        }
        else {
            return filePath.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
        }
    }
    function containsFile(obj, list) {
        var i;
        for (i = 0; i < list.count; i++) {
            if (list.get(i).fileUrl.toString() === obj.fileUrl) {
                console.log("File already uploaded");
                return true;
            }
        }
        return false;
    }
    function maxTextInputLength(num){
        //given number of pages returns maximum length that TextInput should accept
        return Math.ceil(Math.log(num + 1) / Math.LN10);
    }
}
