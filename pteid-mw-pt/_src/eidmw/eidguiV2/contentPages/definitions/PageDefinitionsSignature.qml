/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsSignatureForm {

    Keys.onPressed: {
        console.log("PageDefinitionsSignatureForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }
    Connections {
        target: controler
    }
    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalSignCertDataChanged: {
            console.log("Definitions Signature --> Certificate Data Changed")

            propertySigSignedByNameText.text = propertySigSignedByNameTextCustom.text = ownerName
            propertySigNumIdText.text = propertySigNumIdTextCustom.text =
                qsTranslate("GAPI","STR_NIC") + ": " + NIC
            propertySigLocationText.text = propertySigLocationTextCustom.text ="{" + qsTr("STR_CUSTOM_SIGN_LOCATION") + "}"

            propertyBusyIndicator.running = false
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                        propertyRadioButtonDefault.forceActiveFocus()
        }
        onSignalCardAccessError: {
            console.log("Definitions Signature --> onSignalCardAccessError")
            clearFields()
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Definitions Signature --> onSignalCardChanged")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            var returnSubMenuWhenClosed = false
            var hasOnlyIcao = gapi.hasOnlyICAO()
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                returnSubMenuWhenClosed = true;
                clearFields()
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = !hasOnlyIcao
                if (!hasOnlyIcao)
                    gapi.startGettingInfoFromSignCert()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                returnSubMenuWhenClosed = true;
            }

            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, returnSubMenuWhenClosed)
        }
        onSignalCustomSignImageRemoved: {
            console.log("Definitions Signature onSignalCustomSignImageRemoved")
            var titlePopup = qsTranslate("PageServicesSign", "STR_WARNING")
            var bodyPopup = qsTranslate("PageServicesSign","STR_CUSTOM_IMAGE_REMOVED")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
    }

    propertyDropArea {

        onEntered: {
            /*console.log("You chose file(s): " + drag.urls.toString());*/
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            if(filesArray.length > 1){
                var titlePopup = qsTranslate("Popup File","STR_POPUP_FILE_UNIQUE")
                var bodyPopup = qsTranslate("Popup File","STR_POPUP_FILE_UNIQUE_MULTI")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }else{
                /*console.log("Adding file: " + filesArray[0])*/
                var path =  filesArray[0]

                filesModel.clear()
                filesModel.append({
                                      "fileUrl": path
                                  })

                if(!gapi.saveCustomImageToCache(path,
                                Qt.size(propertyImagePreCustom.sourceSize.width,
                                        propertyImagePreCustom.sourceSize.height))){
                    console.error("Error saving img: ", path);
                }
            }
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }

    propertyFileDialog {
        onAccepted: {
            /*console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            console.log("Adding file: " + propertyFileDialog.fileUrls[0])*/
            var path = propertyFileDialog.fileUrls[0];

            filesModel.clear()
            filesModel.append({
                                  "fileUrl": path
                              })

            if(!gapi.saveCustomImageToCache(path,
                            Qt.size(propertyImagePreCustom.sourceSize.width,
                                    propertyImagePreCustom.sourceSize.height))){
                console.error("Error saving img: ", path);
            }
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
            gapi.customSignImageRemove()
            propertyRadioButtonDefault.forceActiveFocus()
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
                var loadedFilePath = filesModel.get(0).fileUrl
                propertyImagePreCustom.source = loadedFilePath
                propertyRadioButtonCustom.forceActiveFocus()
            }
        }
    }

    propertyRadioButtonDefault {
        onCheckedChanged: {
            if (propertyRadioButtonDefault.checked){
                gapi.setUseCustomSignature(false)
            }else{
                gapi.setUseCustomSignature(true)
            }
        }
    }
    
    propertyCheckBoxNumId {
        onCheckedChanged: {
            gapi.setUseNumId(propertyCheckBoxNumId.checked)
            propertyCheckBoxNumId2.checked = propertyCheckBoxNumId.checked
        }
    }

    propertyCheckBoxDate {
        onCheckedChanged: {
            gapi.setUseDate(propertyCheckBoxDate.checked)
            propertyCheckBoxDate2.checked = propertyCheckBoxDate.checked
        }
    }

    propertyCheckBoxNumId2 {
        onCheckedChanged: {
            propertyCheckBoxNumId.checked = propertyCheckBoxNumId2.checked
        }
    }

    propertyCheckBoxDate2 {
        onCheckedChanged: {
            propertyCheckBoxDate.checked = propertyCheckBoxDate2.checked
        }
    }

    Component.onCompleted: {

        console.log("Page Definitions Signature mainWindowCompleted")
        var hasOnlyIcao = gapi.hasOnlyICAO()
        propertyBusyIndicator.running = !hasOnlyIcao
        if (!hasOnlyIcao)
            gapi.startGettingInfoFromSignCert()

        propertySigDateText.text = propertySigDateTextCustom.text =
                qsTranslate("PageServicesSign", "STR_SIGN_DATE") + ": " + getDate()
        clearFields()
        if(gapi.getUseCustomSignature() && gapi.customSignImageExist()){
            propertyRadioButtonDefault.checked = false
            propertyRadioButtonCustom.checked = true
        }else{
            propertyRadioButtonDefault.checked = true
            propertyRadioButtonCustom.checked = false
        }
        if(gapi.customSignImageExist()){
            var urlCustomImage = gapi.getCachePath()+"/CustomSignPicture.jpg"
            if (Qt.platform.os === "windows") {
                urlCustomImage = "file:///"+urlCustomImage
            }else{
                urlCustomImage = "file://"+urlCustomImage
            }
            fileLoaded = true
            propertyImagePreCustom.source = urlCustomImage
        }
        if(mainFormID.propertyPageLoader.propertyForceFocus)
            propertyRadioButtonDefault.forceActiveFocus()
        
        propertyCheckBoxNumId.checked = gapi.getUseNumId()
        propertyCheckBoxDate.checked = gapi.getUseDate()
        
    }
    function getDate(){
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
    function toggleRadio(element){
        if(!element.checked)
            element.checked = true
    }
    function clearFields(){
        propertySigReasonText.text = propertySigReasonTextCustom.text = "{" + qsTr("STR_CUSTOM_SIGN_REASON") + "}"
        propertySigSignedByText.text = propertySigSignedByTextCustom.text = qsTr("STR_CUSTOM_SIGN_BY")
        propertySigSignedByNameText.text = propertySigSignedByNameTextCustom.text = ""
        propertySigNumIdText.text = propertySigNumIdTextCustom.text = qsTranslate("GAPI","STR_NIC") + ": "
        propertySigLocationText.text = propertySigLocationTextCustom.text = "{" + qsTr("STR_CUSTOM_SIGN_LOCATION") + "}"
        propertySigImg.source = "qrc:/images/pdf_document_header.png"
        propertySigWaterImg.source = propertySigWaterImgCustom.source = "qrc:/images/pteid_signature_watermark.jpg"
    }
}
