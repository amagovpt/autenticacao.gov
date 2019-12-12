/*-****************************************************************************

 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

//Import C++ defined enums
import eidguiV2 1.0

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

PageDefinitionsSCAPForm {

    property string popupMsg: ""
    property bool isCardPresent: false
    property bool isLoadingAttributes: false
    property bool isTabSelected: true

    Keys.onRightPressed: {
        if(propertylinkScapEntities.activeFocus
                || propertylinkScapCompanies.activeFocus)
            returnToAdvancedSignaturePage()
    }
    Keys.onSpacePressed: {
        if(propertylinkScapEntities.activeFocus
                || propertylinkScapCompanies.activeFocus)
            returnToAdvancedSignaturePage()
    }
    Keys.onReturnPressed: {
        if(propertylinkScapEntities.activeFocus
                || propertylinkScapCompanies.activeFocus)
            returnToAdvancedSignaturePage()
    }

    Keys.onPressed: {
        var index = propertyStackLayout.currentIndex
        var isOnEntitiesTab = index === 0
        var isOnEntityTab = index === 1


        if (event.key == Qt.Key_Escape && isOnEntitiesTab && isTabSelected === false) {
            propertyTabButtonEntities.forceActiveFocus()
            isTabSelected = true
        } else if (event.key == Qt.Key_Escape && isOnEntityTab && isTabSelected === false) {
            propertyTabButtonCompanies.forceActiveFocus()
            isTabSelected = true
        } else {
            Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
        }
    }

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Definitions SCAP Signature --> Data Changed")
            propertyBusyIndicator.running = false
            isCardPresent = true
            propertyButtonLoadEntityAttributes.enabled = isCardPresent && isAnyEntitySelected()
        }
        onSignalCardAccessError: {
            console.log("Definitions SCAP Signature --> onSignalCardAccessError")
            isCardPresent = false
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
            var bodyPopup = ""
            if (error_code == GAPI.NoReaderFound) {
                return;
            }
            else if (error_code == GAPI.NoCardFound) {
                return;
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
            propertyButtonLoadEntityAttributes.enabled = isCardPresent && isAnyEntitySelected()
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            propertyBusyIndicatorAttributes.running = false
            propertyBusyIndicator.running = false
            propertyTabButtonEntities.forceActiveFocus()
        }
        onSignalCardChanged: {
            console.log("Definitions SCAP Signature --> onSignalCardChanged")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ") + controler.autoTr
            var bodyPopup = ""
            if (error_code == GAPI.ET_CARD_REMOVED) {
                isCardPresent = false
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED") + controler.autoTr
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                isCardPresent = true
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED") + controler.autoTr
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN") + controler.autoTr
            }
            propertyButtonLoadEntityAttributes.enabled = isCardPresent && isAnyEntitySelected()
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalSCAPDefinitionsServiceFail: {
            isLoadingAttributes = false
            console.log("Definitions SCAP - Signal SCAP service fail Code = "
                        + pdfsignresult + " isCompany = " + isCompany)
            cmdLoadDialog.close()

            var titlePopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            var bodyPopup = ""
            if(isCompany === true){
                if(pdfsignresult == GAPI.ScapAttributesExpiredError){
                    console.log("ScapAttributesExpiredError")
                    bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_COMPANY_ATTRIBUTES_EXPIRED")
                            + " "
                            + "<a href=\"https://www.autenticacao.gov.pt/a-autenticacao-de-profissionais\">"
                            + "https://www.autenticacao.gov.pt/a-autenticacao-de-profissionais"
                }else if(pdfsignresult == GAPI.ScapZeroAttributesError){
                    console.log("ScapZeroAttributesError")
                    bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_COMPANY_ZERO_ATTRIBUTES")
                            + " "
                            + "<a href=\"https://www.autenticacao.gov.pt/a-autenticacao-de-profissionais\">"
                            + "https://www.autenticacao.gov.pt/a-autenticacao-de-profissionais"
                }else {
                    console.log("ScapGenericError")
                    gapi.startPingSCAP()
                }
            }else{
                var attributeList = []
                var totalCount = 0
                var count = 0

                for (var i = 0; i < entityAttributesModel.count; i++){
                    if(entityAttributesModel.get(i).checkBoxAttr == true){
                        totalCount++
                    }
                }

                for (var i = 0; i < entityAttributesModel.count; i++){
                    if(entityAttributesModel.get(i).checkBoxAttr == true){

                        if(count > 0){
                            if(count == totalCount-1){
                                popupMsg += " e "
                            }else{
                                popupMsg += " , "
                            }
                        }
                        popupMsg += entityAttributesModel.get(i).entityName
                        count++
                    }
                }
                console.log(popupMsg)
                if(pdfsignresult == GAPI.ScapAttributesExpiredError){
                    bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ATTRIBUTES_EXPIRED_FIRST")
                            + " " + popupMsg + " "
                            + qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ATTRIBUTES_EXPIRED_SECOND")
                            + "\n\n"
                            + qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ATTRIBUTES_EXPIRED_THIRD")
                            + " " + popupMsg + "."
                }else if(pdfsignresult == GAPI.ScapZeroAttributesError){
                    console.log("ScapZeroAttributesError")
                    bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ZERO_ATTRIBUTES_FIRST")
                            + " " + popupMsg + "."
                            + "\n\n" + qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ZERO_ATTRIBUTES_SECOND")
                            + " " + popupMsg + "."
                }else {
                    console.log("ScapGenericError")
                    gapi.startPingSCAP()
                }
            }

            popupMsg = ""

            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            // Load attributes from cache (Entities, ShortDescription)
            gapi.startLoadingAttributesFromCache(GAPI.ScapAttrEntities,
                                                 GAPI.ScapAttrDescriptionShort)
            propertyBusyIndicator.running = false
        }
        onSignalSCAPPingSuccess: {
            console.log("Definitions SCAP Signature -->Signal SCAP ping success")
            var titlePopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            var bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_SERVICE_FAIL_FIRST")
                    + "\n\n"
                    + qsTranslate("PageDefinitionsSCAP","STR_SCAP_SERVICE_FAIL_SECOND")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            // Load attributes from cache (Entities, isShortDescription)
            propertyBusyIndicatorAttributes.running = false
            propertyBusyIndicator.running = false
        }
        onSignalSCAPPingFail: {
            console.log("Definitions SCAP - Signal SCAP ping fail")
            var titlePopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            var bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_PING_FAIL_FIRST")
                    + "\n\n"
                    + qsTranslate("PageDefinitionsSCAP","STR_SCAP_PING_FAIL_SECOND")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            // Load attributes from cache (Entities, isShortDescription)
            propertyBusyIndicator.running = false
            propertyBusyIndicatorAttributes.running = false
        }
        onSignalEntityAttributesLoadedError: {
            isLoadingAttributes = false
            console.log("Definitions SCAP - Signal SCAP entities loaded error")
            var titlePopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            var bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_LOAD_ENTITIES_ERROR")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            if(propertyBar.currentIndex == 0)
                propertyBusyIndicatorAttributes.running = false
            propertyListViewEntities.forceActiveFocus()
        }
        onSignalCompanyAttributesLoadedError: {
            isLoadingAttributes = false
            console.log("Definitions SCAP - Signal SCAP company loaded error")
            var titlePopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            var bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_LOAD_COMPANY_ERROR")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            if(propertyBar.currentIndex == 1)
                propertyBusyIndicatorAttributes.running = false
        }
        onSignalSCAPEntitiesLoaded: {
            console.log("Definitions SCAP - Signal SCAP entities loaded")
            entityAttributesModel.clear()
            for (var i = 0; i < entitiesList.length; i++)
            {
                entityAttributesModel.append({
                                                 entityName: entitiesList[i],
                                                 attribute: "",
                                                 checkBoxAttr: false
                                             });
            }

            // Load attributes from cache (Entities, ShortDescription)
            gapi.startLoadingAttributesFromCache(GAPI.ScapAttrEntities,
                                                 GAPI.ScapAttrDescriptionShort)
            propertyBusyIndicator.running = false
            propertyBusyIndicatorAttributes.running = false
        }
        onSignalEntityAttributesLoaded:{
            console.log("Definitions SCAP - Signal SCAP Entity attributes loaded")
            if(entityAttributesModel.count == 0){
                for(var i = 0; i < attribute_list.length; i=i+3)
                {
                    entityAttributesModel.append({
                                                     entityName: attribute_list[i],
                                                     attribute: "<ul><li><b>"+attribute_list[i+1]+"</b> - "+attribute_list[i+2]+"</li></ul>",
                                                     checkBoxAttr: false
                                                 });
                }
            }else{
                // Remove old attributes
                for(i = 0; i < attribute_list.length; i=i+3)
                {
                    for (var j = 0; j < entityAttributesModel.count; j++){
                        if(entityAttributesModel.get(j).entityName === attribute_list[i]){
                            entityAttributesModel.set(j, {"entityName": attribute_list[i], "attribute":""})
                        }
                    }
                }
                // Add new attribute
                for(i = 0; i < attribute_list.length; i=i+3)
                {
                    for (var j = 0; j < entityAttributesModel.count; j++){
                        if(entityAttributesModel.get(j).entityName === attribute_list[i]){
                            entityAttributesModel.set(j, {
                                "entityName": attribute_list[i],
                                "attribute": entityAttributesModel.get(j).attribute 
                                + "<ul><li><b>"+attribute_list[i+1]+"</b> - " + attribute_list[i+2]+"</li></ul>"})
                        }
                    }
                }
            }
            //Load attributes from cache (Companies, ShortDescription)
            propertyBusyIndicatorAttributes.running = false
            gapi.startLoadingAttributesFromCache(GAPI.ScapAttrCompanies,
                                                 GAPI.ScapAttrDescriptionShort)
        }
        onSignalCompanyAttributesLoaded: {
            console.log("Definitions SCAP - Signal SCAP company attributes loaded")
            if (isLoadingAttributes) {
                isLoadingAttributes = false
                var titlePopup = qsTranslate("GAPI","STR_POPUP_SUCESS")
                var bodyPopup = qsTranslate("PageDefinitionsSCAP","STR_SCAP_ATTRIBUTES_LOADED")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
            companyAttributesModel.clear()
            for(var i = 0; i < attribute_list.length; i=i+3)
            {
                attribute_list[i+1] = toTitleCase(attribute_list[i+1])
                // Let's see if company already exists and in what index
                var companyIndex
                for(companyIndex = 0; companyIndex < companyAttributesModel.count; companyIndex++) {
                    if(companyAttributesModel.get(companyIndex).entityName === attribute_list[i]) {
                        break
                    }
                }
                if(companyAttributesModel.count>0 &&
                        companyIndex < companyAttributesModel.count){
                    companyAttributesModel.set(companyIndex,{
                                                   entityName: attribute_list[i],
                                                   attribute: companyAttributesModel.get(companyIndex).attribute
                                                              + "<ul><li><b>"+attribute_list[i+1]+"</b> - "+attribute_list[i+2]+"</li></ul>"
                                               });
                }else{
                    companyAttributesModel.append({
                                                      entityName: attribute_list[i],
                                                      attribute: "<ul><li><b>"+attribute_list[i+1]+"</b> - "+attribute_list[i+2]+"</li></ul>",
                                                      checkBoxAttr: false
                                                  });
                }
            }
            propertyBusyIndicatorAttributes.running = false
        }
        onSignalRemoveSCAPAttributesSucess: {
            console.log("Definitions SCAP - Signal SCAP Signal Remove SCAP Attributes Sucess")
            if (isLoadingAttributes == false) {
                propertyBusyIndicatorAttributes.running = false
                var titlePopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE")
                var bodyPopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE_SUCC")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                if(scapAttrType != GAPI.ScapAttrCompanies){
                    gapi.startGettingEntities()
                    propertyBusyIndicator.running = true
                }
            }
        }
        onSignalRemoveSCAPAttributesFail: {
            console.log("Definitions SCAP - Signal Remove SCAP Attributes Fail")
            propertyBusyIndicatorAttributes.running = false
            var titlePopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            var bodyPopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE_FAIL")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            if(scapAttrType != GAPI.ScapAttrCompanies){
                gapi.startGettingEntities()
                propertyBusyIndicator.running = true
            }
        }
        onSignalCacheNotReadable:{
            propertyBusyIndicatorAttributes.running = false;
            var bodyPopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            var titlePopup = qsTranslate("PageDataApp","STR_CACHE_NOT_READABLE")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            if(scapAttrType != GAPI.ScapAttrCompanies){
                gapi.startGettingEntities()
                propertyBusyIndicator.running = true
            }
        }
        onSignalCacheNotWritable:{
            propertyBusyIndicatorAttributes.running = false;
            var titlePopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            var bodyPopup = qsTranslate("PageDataApp","STR_CACHE_NOT_WRITABLE")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalCacheFolderNotCreated:{
            propertyBusyIndicatorAttributes.running = false;
            var titlePopup = qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            var bodyPopup = qsTranslate("PageDataApp","STR_CACHE_FOLDER_NOT_CREATED")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
        onSignalBeginOAuth: { 
            cmdLoadDialog.open()
        }
        onSignalEndOAuth: {
            cmdLoadDialog.close()
            // Error codes from OAuthResult in OAuthAttributes.h
            switch (oauthResult){
                case 0://OAuthSuccess
                    var titlePopup = qsTranslate("PageDefinitionsSCAP","STR_LOADING_SCAP_ATTRIBUTES")
                    var bodyPopup = qsTranslate("OAuth","STR_OAUTH_SUCCESS_SCAP")
                    break;
                case 1://OAuthTimeoutError
                    var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                    var bodyPopup = qsTranslate("OAuth","STR_OAUTH_TIMEOUT")
                    break;
                case 2://OAuthGenericError
                    var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                    var bodyPopup = qsTranslate("OAuth","STR_OAUTH_GENERIC_ERROR")
                case 3://OAuthConnectionError
                    var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                    var bodyPopup = qsTranslate("OAuth","STR_OAUTH_CONNECTION_ERROR")
                    break;
                case 4://OAuthCancelled
                    console.log("oauth - cancelado")
                    return;
            }
            if (oauthResult != 0)
                propertyBusyIndicatorAttributes.running = false
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
    }

    propertyListViewEntities{
        onFocusChanged: {
            if(propertyListViewEntities.focus)propertyListViewEntities.currentIndex = 0
        }
    }

    Component {
        id: attributeListDelegateEntities
        Rectangle {
            id: container
            width: parent.width - propertyEntitiesListViewScroll.width
                   - Constants.SIZE_ROW_H_SPACE * 0.5
            height: columnItem.height + 10
            Keys.onSpacePressed: {
                checkboxSel.focus = true
            }
            Keys.onTabPressed: {
                checkboxSel.focus = true
                if(propertyListViewEntities.currentIndex == propertyListViewEntities.count -1){
                    propertylinkScapEntities.visible
                        ? propertylinkScapEntities.forceActiveFocus()
                        : propertyButtonRemoveEntityAttributes.forceActiveFocus()
                }else{
                    propertyListViewEntities.currentIndex++
                }
            }
            Keys.onBacktabPressed: {
                checkboxSel.focus = true
                if(propertyListViewEntities.currentIndex == 0){
                    propertyEntitiesText.forceActiveFocus()
                }else{
                    propertyListViewEntities.currentIndex--
                }
            }
            color:  propertyListViewEntities.currentIndex === index && propertyListViewEntities.focus
                    ? Constants.COLOR_MAIN_DARK_GRAY : Constants.COLOR_MAIN_SOFT_GRAY

            Accessible.role: Accessible.CheckBox
            Accessible.name: Functions.filterText(entityText.text + attrTitle.text
                                                  + (attrText.text.length > 0 ? attrText.text : qsTranslate("GAPI","STR_EMPTY_FIELD"))
                                                  )
            CheckBox {
                id: checkboxSel
                height: 25
                width: 50
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
                anchors.top: parent.top
                onCheckedChanged: {
                    entityAttributesModel.get(index).checkBoxAttr = checkboxSel.checked
                    propertyButtonLoadEntityAttributes.enabled = isAnyEntitySelected() && isCardPresent
                    propertyButtonLoadEntityAttributesOAuth.enabled = isAnyEntitySelected()
                    propertyListViewEntities.currentIndex = index
                }
                onFocusChanged: {
                    if(focus) propertyListViewEntities.currentIndex = index
                }
            }
            Column {
                id: columnItem
                anchors.left: checkboxSel.right
                width: parent.width - checkboxSel.width - Constants.SIZE_ROW_H_SPACE * 0.5
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    id: entityText
                    text: '<b>' + qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITY") + '</b> ' + entityName
                    font.family: lato.name
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: attrTitle
                    text: '<b>'+ qsTranslate("PageDefinitionsSCAP","STR_SCAP_ATTR") + '</b>'
                    font.family: lato.name
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: attrText
                    width: parent.width
                    wrapMode: Text.WordWrap
                    text: attribute
                    font.family: lato.name
                }
            }
        }
    }
    propertyListViewCompanies{
        onFocusChanged: {
            if(propertyListViewCompanies.focus)propertyListViewCompanies.currentIndex = 0
        }
    }
    Component {
        id: attributeListDelegateCompanies
        Rectangle {
            id: container
            width: parent.width - propertyCompaniesListViewScroll.width
                   - Constants.SIZE_ROW_H_SPACE * 0.5
            height: columnItem.height + 10
            Keys.onTabPressed: {
                if(propertyListViewCompanies.currentIndex == propertyListViewCompanies.count -1){
                    propertylinkScapCompanies.visible == true
                        ? propertylinkScapCompanies.forceActiveFocus()
                        : propertyButtonRemoveCompanyAttributes.forceActiveFocus()
                }else{
                    propertyListViewCompanies.currentIndex++
                }
            }
            Keys.onBacktabPressed: {
                if(propertyListViewCompanies.currentIndex == 0){
                    propertyCompaniesText.forceActiveFocus()
                }else{
                    propertyListViewCompanies.currentIndex--
                }
            }
            color:  propertyListViewCompanies.currentIndex === index && propertyListViewCompanies.focus
                    ? Constants.COLOR_MAIN_DARK_GRAY : Constants.COLOR_MAIN_SOFT_GRAY
            Accessible.role: Accessible.CheckBox
            Accessible.name: Functions.filterText(entityText.text + attrTitle.text
                                                  + (attrText.text.length > 0 ? attrText.text : qsTranslate("GAPI","STR_EMPTY_FIELD"))
                                                  )
            Column {
                id: columnItem
                width: parent.width - Constants.SIZE_ROW_H_SPACE
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: entityText
                    text: '<b>' + qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITY") + '</b> ' + entityName
                    font.family: lato.name
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: attrTitle
                    text: '<b>'+ qsTranslate("PageDefinitionsSCAP","STR_SCAP_ATTR") + '</b>'
                    font.family: lato.name
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: attrText
                    width: parent.width
                    wrapMode: Text.WordWrap
                    text: attribute
                    font.family: lato.name
                }
            }

        }
    }

    ListModel {
        id: companyAttributesModel
    }

    ListModel {
        id: entityAttributesModel
    }

    Dialog {
        id: cmdLoadDialog
        width: 400
        height: 200
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - width * 0.5
        y: parent.height * 0.5 - height * 0.5
        modal: true

        header: Label {
            id: labelTextTitle
            visible: true
            text: qsTranslate("OAuth","STR_OAUTH")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
            KeyNavigation.tab: textContinueInBrowser
            KeyNavigation.down: textContinueInBrowser
        }

        Item {
            width: parent.width
            height: parent.height

            Item {
                id: rectMessage
                width: parent.width
                height: 50

                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textContinueInBrowser
                    text: qsTranslate("OAuth","STR_CONTINUE_IN_BROWSER")
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: activeFocus ? Constants.SIZE_TEXT_LABEL_FOCUS : Constants.SIZE_TEXT_LABEL
                    font.bold: activeFocus
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                    KeyNavigation.tab: closeCmdLoadDialogButton
                    KeyNavigation.down: closeCmdLoadDialogButton
                }
            }
        }
        Button {
            id: closeCmdLoadDialogButton
            width: Constants.WIDTH_BUTTON
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTranslate("OAuth","STR_ABORT_AUTH")
            font.capitalization: Font.MixedCase
            KeyNavigation.tab: labelTextTitle
            KeyNavigation.down: labelTextTitle
            highlighted: activeFocus
            onClicked: {
                cmdLoadDialog.close()
                gapi.abortSCAPWithCMD()
                propertyBusyIndicatorAttributes.running = false

            }
        }
        onOpened: {
            labelTextTitle.forceActiveFocus()
        }
        onRejected: {
            cmdLoadDialog.open()
        }
        onClosed: {
            propertyTabButtonEntities.forceActiveFocus()
        }

    }

    propertyButtonLoadCompanyAttributes {
        onClicked: {
            console.log("ButtonLoadCompanyAttributes clicked!")
            isLoadingAttributes = true
            propertyBusyIndicatorAttributes.running = true
            propertyPageLoader.attributeListBackup = []
            gapi.startGettingCompanyAttributes(false)
        }
    }
    propertyButtonLoadCompanyAttributesOAuth {
        onClicked: {
            console.log("ButtonLoadCompanyAttributesOAuth clicked!")
            isLoadingAttributes = true
            propertyBusyIndicatorAttributes.running = true
            propertyPageLoader.attributeListBackup = []
            gapi.startGettingCompanyAttributes(true)
        }
    }

    propertyButtonLoadEntityAttributes {
        onClicked: {
            console.log("ButtonLoadEntityAttributes clicked!")
            propertyBusyIndicatorAttributes.running = true
            isLoadingAttributes = true
            var attributeList = []
            var count = 0
            for (var i = 0; i < entityAttributesModel.count; i++){
                if(entityAttributesModel.get(i).checkBoxAttr == true){
                    attributeList[count] = i
                    count++
                }
            }
            console.log("QML AttributeList: ", attributeList)
            propertyPageLoader.attributeListBackup = []
            gapi.startGettingEntityAttributes(attributeList, false)
        }
    }
    propertyButtonLoadEntityAttributesOAuth {
        onClicked: {
            console.log("ButtonLoadEntityAttributesOAuth clicked!")
            propertyBusyIndicatorAttributes.running = true
            isLoadingAttributes = true
            var attributeList = []
            var count = 0
            for (var i = 0; i < entityAttributesModel.count; i++){
                if(entityAttributesModel.get(i).checkBoxAttr == true){
                    attributeList[count] = i
                    count++
                }
            }
            console.log("QML AttributeList: ", attributeList)
            propertyPageLoader.attributeListBackup = []
            gapi.startGettingEntityAttributes(attributeList, true)
        }
    }
    propertyButtonRemoveCompanyAttributes {
        onClicked: {
            console.log("propertyButtonRemoveCompanyAttributes clicked!")
            companyAttributesModel.clear()
            entityAttributesModel.clear()
            propertyBusyIndicatorAttributes.running = true
            propertyPageLoader.attributeListBackup = []
            gapi.startRemovingAttributesFromCache(1)
        }

    }

    propertyButtonRemoveEntityAttributes {
        onClicked: {
            console.log("propertyButtonRemoveEntityAttributes clicked!")
            companyAttributesModel.clear()
            entityAttributesModel.clear()
            propertyBusyIndicatorAttributes.running = true
            propertyPageLoader.attributeListBackup = []
            gapi.startRemovingAttributesFromCache(0)
        }
    }

    propertyMouseArealinkScapEntities {
        onClicked: {
            console.log("propertyMouseArealinkScapEntities clicked!")
            returnToAdvancedSignaturePage()
        }
    }

    propertyMouseArealinkScapCompanies {
        onClicked: {
            console.log("propertyMouseArealinkScapCompanies clicked!")
            returnToAdvancedSignaturePage()
        }
    }
    propertyBar{
        onCurrentIndexChanged: {
            propertyStackLayout.currentIndex = propertyBar.currentIndex
        }
    }
    Component.onCompleted: {
        console.log("Page Definitions SCAP Completed")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
        propertyBusyIndicatorAttributes.running = true
        isLoadingAttributes = false
        propertyTabButtonEntities.forceActiveFocus()
        gapi.startGettingEntities()
    }

    function returnToAdvancedSignaturePage() {
        propertyPageLoader.propertyBackupFromSignaturePage = false
        mainFormID.state = Constants.MenuState.EXPAND
        mainFormID.propertySubMenuListView.model.clear()
        for(var i = 0; i < mainFormID.propertyMainMenuListView.model.get(1).subdata.count; ++i) {
            /*console.log("Sub Menu indice " + i + " - "
                        + mainFormID.propertyMainMenuListView.model.get(1).subdata.get(i).subName);*/
            mainFormID.propertySubMenuListView.model
            .append({
                        "subName": qsTranslate("MainMenuModel",
                                            mainFormID.propertyMainMenuListView.model.get(1).subdata.get(i).name),
                        "expand": mainFormID.propertyMainMenuListView.model.get(1).subdata.get(i)
                        .expand,
                        "url": mainFormID.propertyMainMenuListView.model.get(1).subdata.get(i)
                        .url
                    })
        }
        mainFormID.propertyMainMenuListView.currentIndex = 1
        mainFormID.propertyMainMenuBottomListView.currentIndex = -1
        mainFormID.propertySubMenuListView.currentIndex = -1
        mainFormID.propertyPageLoader.source = "/contentPages/services/PageServicesSignAdvanced.qml"
    }

    function isAnyEntitySelected() {
        for (var i = 0; i < entityAttributesModel.count; i++){
            if(entityAttributesModel.get(i).checkBoxAttr === true){
                return true
            }
        }
        return false
    }

    function toTitleCase(str) {
        return str.replace(/\w\S*/g, function(txt){
            return txt.charAt(0).toUpperCase() + txt.substr(1).toLowerCase();
        });
    }
    function tabDetectKeys(key) {
        if(propertyTabButtonEntities.focus)
            if(key===Qt.Key_Return || key===Qt.Key_Space){
                propertyStackLayout.currentIndex = propertyBar.currentIndex = 0
                propertyEntitiesText.forceActiveFocus()
                isTabSelected = false
            }
        if(propertyTabButtonCompanies.focus)
            if(key===Qt.Key_Return || key===Qt.Key_Space){
                propertyStackLayout.currentIndex = propertyBar.currentIndex = 1
                propertyCompaniesText.forceActiveFocus()
                isTabSelected = false
            }
    }
}
