import QtQuick 2.6
import QtQuick.Controls 2.1

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsDataForm {

    Connections {
        target: gapi

        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalCardAccessError: {
            console.log("Definitions SCAP Signature --> onSignalCardAccessError")
            propertyBusyIndicator.running = false;
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
            if (error_code == GAPI.NoReaderFound) {
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER") + controler.autoTr
            }
            else if (error_code == GAPI.NoCardFound) {
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_NO_CARD") + controler.autoTr
            }
            else if (error_code == GAPI.SodCardReadError) {
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR") + controler.autoTr
            }
            else if (error_code == GAPI.CardUserPinCancel) {
				mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
						qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
			}
			else if (error_code == GAPI.CardPinTimeout) {
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else {
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR") + controler.autoTr
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();

            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Definitions SCAP Signature --> onSignalCardChanged")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ") + controler.autoTr
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED") + controler.autoTr
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED") + controler.autoTr
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN") + controler.autoTr
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalRemoveSCAPAttributesSucess: {
            console.log("Definitions SCAP - Signal SCAP Signal Remove SCAP Attributes Sucess")
            updateCacheSize();
            propertyBusyIndicator.running = false
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE_SUCC")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalRemoveSCAPAttributesFail: {
            console.log("Definitions SCAP - Signal Remove SCAP Attributes Fail")
            updateCacheSize();
            propertyBusyIndicator.running = false
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalCacheNotReadable:{
            updateCacheSize();
            propertyBusyIndicator.running = false;
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CACHE_NOT_READABLE")
        
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
    }
    Connections {
        target: controler
        
        onSignalRemovePteidCacheSuccess:{
            updateCacheSize();
            propertyBusyIndicator.running = false;
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE_SUCC")
        
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            
            
        }
        onSignalRemovePteidCacheFail:{
            updateCacheSize();
            propertyBusyIndicator.running = false;
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE_FAIL")
        
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalCacheNotReadable:{
            updateCacheSize();
            propertyBusyIndicator.running = false;
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CACHE_NOT_READABLE")
        
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalAppCacheSize:{
            propertyCacheAppSizeTextField.text  = qsTranslate("PageDataApp", "STR_SIZE_OF_CACHE") + " " + cacheSize;
        }
        onSignalScapCacheSize:{
            propertyCacheSCAPSizeTextField.text  = qsTranslate("PageDataApp", "STR_SIZE_OF_CACHE") + " " + cacheSize;
        }
    }

    propertyButtonRemoveAppCache {
        onClicked: {
            console.log("propertyButtonRemoveAppCache clicked");
            propertyBusyIndicator.running = true;
            controler.flushCache()
        }
    }
    propertyButtonRemoveSCAPCache {
        onClicked: {
            console.log("propertyButtonRemoveSCAPCache clicked");
            propertyBusyIndicator.running = true;
            gapi.startRemovingAttributesFromCache(0)
        }
    }

    Component.onCompleted: {
        updateCacheSize();
    }

    function updateCacheSize() {
        controler.getPteidCacheSize();
        controler.getScapCacheSize();
    }
}
