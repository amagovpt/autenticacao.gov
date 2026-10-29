/*-****************************************************************************

 * Copyright (C) 2018 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick
import QtQuick.Controls

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsDataForm {

    Keys.onPressed: function(event) {
        console.log("PageDefinitionsDataForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
    }

    Connections {
        target: gapi

        function onSignalGenericError(error_code) {
            propertyBusyIndicator.running = false
        }
        function onSignalCardDataChanged() {
            propertyBusyIndicator.running = false
            updateCacheSize();
        }
        function onSignalCardAccessError(error_code) {
            console.log("Definitions SCAP Signature --> onSignalCardAccessError")
            propertyBusyIndicator.running = false;
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = ""
            if (error_code == GAPI.NoReaderFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
            }
            else if (error_code == GAPI.NoCardFound) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD")
            }
            else if (error_code == GAPI.SodCardReadError) {
                bodyPopup = qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR") 
                    + "<br/><br/>" + qsTranslate("Popup Card","STR_GENERIC_CARD_ERROR_MSG") 
                   
            }
            else if (error_code == GAPI.CardUserPinCancel) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
            }
            else if (error_code == GAPI.CardPinTimeout) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)

            propertyBusyIndicator.running = false
        }
        function onSignalCardChanged(error_code) {
            console.log("Definitions SCAP Signature --> onSignalCardChanged")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        function onSignalRemoveSCAPAttributesSucess() {
            console.log("Definitions SCAP - Signal SCAP Signal Remove SCAP Attributes Sucess")
            updateCacheSize();
            propertyBusyIndicator.running = false
            var titlePopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            var bodyPopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE_SUCC")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        function onSignalRemoveSCAPAttributesFail() {
            console.log("Definitions SCAP - Signal Remove SCAP Attributes Fail")
            updateCacheSize();
            propertyBusyIndicator.running = false
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE_ERROR")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        function onSignalCacheNotReadable() {
            updateCacheSize();
            propertyBusyIndicator.running = false;
            var titlePopup = qsTranslate("PageDataApp","STR_PERMISSIONS_CACHE")
            var bodyPopup = qsTranslate("PageDataApp","STR_CACHE_NOT_READABLE")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }

		function onSignalRemoveCANCacheSucess() {
			console.log("Definitions SCAP - Signal CAN Signal Remove CAN cache Sucess")
			propertyBusyIndicator.running = false;
			var titlePopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            var bodyPopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE_SUCC")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
		}
    }
    Connections {
        target: controler

        function onSignalRemovePteidCacheSuccess() {
            updateCacheSize();
            if (propertyCheckboxEnableCache.checked)
            {
                propertyBusyIndicator.running = false;
                var titlePopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE")
                var bodyPopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE_SUCC")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
        }
        function onSignalRemovePteidCacheFail() {
            updateCacheSize();
            if (propertyCheckboxEnableCache.checked)
            {
                propertyBusyIndicator.running = false;
                var titlePopup = qsTranslate("PageDataApp","STR_PERMISSIONS_CACHE")
                var bodyPopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE_FAIL")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
        }
        function onSignalCacheNotReadable() {
            updateCacheSize();
            if (propertyCheckboxEnableCache.checked)
            {
                propertyBusyIndicator.running = false;
                var titlePopup = qsTranslate("PageDataApp","STR_PERMISSIONS_CACHE")
                var bodyPopup = qsTranslate("PageDataApp","STR_CACHE_NOT_READABLE")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
        }
        function onSignalAppCacheSize(cacheSize) {
            propertyCacheAppSizeTextField.text  = qsTranslate("PageDataApp", "STR_SIZE_OF_CACHE") + " " + cacheSize;
        }
        function onSignalScapCacheSize(cacheSize) {
            propertyCacheSCAPSizeTextField.text  = qsTranslate("PageDataApp", "STR_SIZE_OF_CACHE") + " " + cacheSize;
        }
    }

    propertyCheckboxEnableCache{
        onCheckedChanged: {
            var bEnabled = propertyCheckboxEnableCache.checked;
            controler.setEnablePteidCache(bEnabled);
            //clear cache when it is disabled
            if(!bEnabled)
            {
                controler.flushCache();
            }
            propertyButtonRemoveAppCache.enabled = bEnabled;
        }
    }

    propertyButtonRemoveAppCache {
        onClicked: {
            console.log("propertyButtonRemoveAppCache clicked" + propertyButtonRemoveAppCache.enabled);
            if(propertyButtonRemoveAppCache.enabled)
            {
                propertyBusyIndicator.running = true;
                controler.flushCache()
            }

        }
    }
    propertyButtonRemoveSCAPCache {
        onClicked: {
            console.log("propertyButtonRemoveSCAPCache clicked");
            propertyBusyIndicator.running = true;
            gapi.startRemovingAttributesFromCache(GAPI.ScapAttrAll);
        }
    }

    propertyCheckboxEnableCANCache {
        onCheckedChanged: {
            var bEnabled = propertyCheckboxEnableCANCache.checked;
            controler.setEnablePteidCANCache(bEnabled);
			if(!bEnabled){
				gapi.startRemoveCANCache();
			}
			propertyButtonRemoveCANCache.enabled = bEnabled;
        }
    }
    
	propertyButtonRemoveCANCache{
		onClicked: {
            console.log("propertyButtonRemoveCANCache clicked");
            propertyBusyIndicator.running = true;
            gapi.startRemoveCANCache();
		    propertyBusyIndicator.running = false
		}
	}


    Component.onCompleted: {
        propertyMainItem.forceActiveFocus()
        updateCacheSize();

    }

    function updateCacheSize() {
        controler.getPteidCacheSize();
        controler.getScapCacheSize();
    }
}
