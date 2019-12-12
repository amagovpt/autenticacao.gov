/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

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
        onSignalCardDataChanged: {
            console.log("Definitions Signature --> Data Changed")
            //console.trace();
            propertySigReasonText.text = propertySigReasonTextCustom.text = "{" + qsTr("STR_CUSTOM_SIGN_REASON") + "}"
            propertySigSignedByText.text = propertySigSignedByTextCustom.text =qsTr("STR_CUSTOM_SIGN_BY") + ": "

            propertySigSignedByNameText.text = propertySigSignedByNameTextCustom.text =
                    gapi.getDataCardIdentifyValue(GAPI.Givenname) + " " +  gapi.getDataCardIdentifyValue(GAPI.Surname)


            propertySigNumIdText.text = propertySigNumIdTextCustom.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                    + gapi.getDataCardIdentifyValue(GAPI.Documentnum)
            propertySigLocationText.text = propertySigLocationTextCustom.text ="{" + qsTr("STR_CUSTOM_SIGN_LOCATION") + "}"

            propertySigImg.source = "qrc:/images/logo_CC.png"
            propertySigWaterImg.source = propertySigWaterImgCustom.source = "qrc:/images/pteid_signature_watermark.jpg"

            propertyBusyIndicator.running = false
            mainFormID.propertyPageLoader.propertyGeneralPopUp.close()
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                        propertyRadioButtonDefault.forceActiveFocus()
        }
        onSignalCardAccessError: {
            console.log("Definitions Signature --> onSignalCardAccessError")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
            var bodyPopup = ""
            if (error_code == GAPI.NoReaderFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER") + controler.autoTr
            }
            else if (error_code == GAPI.NoCardFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD") + controler.autoTr
            }
            else if (error_code == GAPI.SodCardReadError) {
                bodyPopup = qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR") + controler.autoTr
            }
            else if (error_code == GAPI.CardUserPinCancel) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED") + controler.autoTr
            }
            else if (error_code == GAPI.CardPinTimeout) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT") + controler.autoTr
            }
            else {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR") + controler.autoTr
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)

            propertySigReasonText.text = propertySigReasonTextCustom.text = "{" + qsTr("STR_CUSTOM_SIGN_REASON") + "}"
            propertySigSignedByText.text = propertySigSignedByTextCustom.text = qsTr("STR_CUSTOM_SIGN_BY") + ": "
            propertySigSignedByNameText.text = propertySigSignedByNameTextCustom.text = ""
            propertySigNumIdText.text = propertySigNumIdTextCustom.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
            propertySigLocationText.text = propertySigLocationTextCustom.text = "{" + qsTr("STR_CUSTOM_SIGN_LOCATION") + "}"
            propertySigImg.source = "qrc:/images/logo_CC.png"
            propertySigWaterImg.source = propertySigWaterImgCustom.source = "qrc:/images/pteid_signature_watermark.jpg"

            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Definitions Signature --> onSignalCardChanged")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ") + controler.autoTr
            var bodyPopup = ""
            var returnSubMenuWhenClosed = false
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED") + controler.autoTr
                returnSubMenuWhenClosed = true;

                propertySigReasonText.text = "{" + qsTr("STR_CUSTOM_SIGN_REASON") + "}"
                propertySigSignedByText.text = qsTr("STR_CUSTOM_SIGN_BY") + ": "
                propertySigSignedByNameText.text = ""
                propertySigNumIdText.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                propertySigLocationText.text = "{" + qsTr("STR_CUSTOM_SIGN_LOCATION") + "}"
                propertySigImg.source = "qrc:/images/logo_CC.png"
                propertySigWaterImg.source = propertySigWaterImgCustom.source = "qrc:/images/pteid_signature_watermark.jpg"
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED") + controler.autoTr
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN") + controler.autoTr
                returnSubMenuWhenClosed = true;
            }

            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, returnSubMenuWhenClosed)
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
                var urlCustomImage = gapi.getCachePath()+"/CustomSignPicture.png"
                propertyImagePreCustom.grabToImage(function(result){
                    if (!result.saveToFile(urlCustomImage)){
                        console.error('Unknown error saving to',urlCustomImage);
                    }
                });
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

            var urlCustomImage = gapi.getCachePath()+"/CustomSignPicture.png"
            propertyImagePreCustom.grabToImage(function(result){
                if (!result.saveToFile(urlCustomImage)){
                    console.error('Unknown error saving to',urlCustomImage);
                }
            });

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
            gapi.customSignRemove()
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

    Component.onCompleted: {

        console.log("Page Definitions Signature mainWindowCompleted")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
        propertySigDateText.text = propertySigDateTextCustom.text =getData()

        if(gapi.getUseCustomSignature()){
            propertyRadioButtonDefault.checked = false
            propertyRadioButtonCustom.checked = true
        }else{
            propertyRadioButtonDefault.checked = true
            propertyRadioButtonCustom.checked = false
        }
        if(gapi.customSignImageExist()){
            var urlCustomImage = gapi.getCachePath()+"/CustomSignPicture.png"
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
