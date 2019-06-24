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

    Keys.onPressed: {
        var index = propertyStackLayout.currentIndex

        var isOnEntitiesTab = index === 0 && propertyTabButtonEntities.focus === true
        var isOnEntityTab = index === 1 && propertyTabButtonCompanies.focus === true

        if (isOnEntitiesTab) {
            propertyTabButtonEntities.forceActiveFocus()
        } else if (isOnEntityTab) {
            propertyTabButtonCompanies.forceActiveFocus()
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
            propertyTabButtonEntities.forceActiveFocus()
        }
        onSignalCardAccessError: {
            console.log("Definitions SCAP Signature --> onSignalCardAccessError")
            isCardPresent = false
            if (error_code == GAPI.NoReaderFound) {
                return;
            }
            else if (error_code == GAPI.NoCardFound) {
                return;
            }
            else if (error_code == GAPI.SodCardReadError) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR") + controler.autoTr
            }
            else if (error_code == GAPI.CardUserPinCancel) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
               }
            else if (error_code == GAPI.CardPinTimeout) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR") + controler.autoTr
            }
            propertyButtonLoadEntityAttributes.enabled = isCardPresent && isAnyEntitySelected()
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            propertyBusyIndicatorAttributes.running = false
            propertyBusyIndicator.running = false
            propertyTabButtonEntities.forceActiveFocus()
        }
        onSignalCardChanged: {
            console.log("Definitions SCAP Signature --> onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                isCardPresent = false
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED") + controler.autoTr
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                isCardPresent = true
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED") + controler.autoTr
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN") + controler.autoTr
            }
            propertyButtonLoadEntityAttributes.enabled = isCardPresent && isAnyEntitySelected()
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalSCAPDefinitionsServiceFail: {
            isLoadingAttributes = false
            console.log("Definitions SCAP - Signal SCAP service fail Code = "
                        + pdfsignresult + " isCompany = " + isCompany)
            cmdLoadDialog.close()
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")

            if(isCompany === true){
                if(pdfsignresult == GAPI.ScapAttributesExpiredError){
                    console.log("ScapAttributesExpiredError")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("PageDefinitionsSCAP","STR_SCAP_COMPANY_ATTRIBUTES_EXPIRED")
                            + " "
                            + "<a href=\"https://www.autenticacao.gov.pt/a-autenticacao-de-profissionais\">"
                            + "https://www.autenticacao.gov.pt/a-autenticacao-de-profissionais"
                }else if(pdfsignresult == GAPI.ScapZeroAttributesError){
                    console.log("ScapZeroAttributesError")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("PageDefinitionsSCAP","STR_SCAP_COMPANY_ZERO_ATTRIBUTES")
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
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ATTRIBUTES_EXPIRED_FIRST")
                            + " " + popupMsg + " "
                            + qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ATTRIBUTES_EXPIRED_SECOND")
                            + "\n\n"
                            + qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ATTRIBUTES_EXPIRED_THIRD")
                            + " " + popupMsg + "."
                }else if(pdfsignresult == GAPI.ScapZeroAttributesError){
                    console.log("ScapZeroAttributesError")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ZERO_ATTRIBUTES_FIRST")
                            + " " + popupMsg + "."
                            + "\n\n" + qsTranslate("PageDefinitionsSCAP","STR_SCAP_ENTITIES_ZERO_ATTRIBUTES_SECOND")
                            + " " + popupMsg + "."
                }else {
                    console.log("ScapGenericError")
                    gapi.startPingSCAP()
                }
            }

            popupMsg = ""

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            // Load attributes from cache (Entities, isShortDescription)
            gapi.startLoadingAttributesFromCache(0, 0)
            propertyBusyIndicator.running = false
        }
        onSignalSCAPPingSuccess: {
            console.log("Definitions SCAP Signature -->Signal SCAP ping success")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_SERVICE_FAIL_FIRST")
                    + "\n\n"
                    + qsTranslate("PageDefinitionsSCAP","STR_SCAP_SERVICE_FAIL_SECOND")
             mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            // Load attributes from cache (Entities, isShortDescription)
            propertyBusyIndicatorAttributes.running = false
            propertyBusyIndicator.running = false
        }
        onSignalSCAPPingFail: {
            console.log("Definitions SCAP - Signal SCAP ping fail")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_PING_FAIL_FIRST")
                    + "\n\n"
                    + qsTranslate("PageDefinitionsSCAP","STR_SCAP_PING_FAIL_SECOND")
             mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            // Load attributes from cache (Entities, isShortDescription)
            propertyBusyIndicator.running = false
            propertyBusyIndicatorAttributes.running = false
        }
        onSignalEntityAttributesLoadedError: {
            isLoadingAttributes = false
            console.log("Definitions SCAP - Signal SCAP entities loaded error")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_LOAD_ENTITIES_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            if(propertyBar.currentIndex == 0)
                propertyBusyIndicatorAttributes.running = false
            propertyListViewEntities.forceActiveFocus()

        }
        onSignalCompanyAttributesLoadedError: {
            isLoadingAttributes = false
            console.log("Definitions SCAP - Signal SCAP company loaded error")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_LOAD_COMPANY_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
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

            // Load attributes from cache (Entities, isShortDescription)
            gapi.startLoadingAttributesFromCache(0, 0)
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
            //Load attributes from cache (Companies, isShortDescription)
            propertyBusyIndicatorAttributes.running = false
            gapi.startLoadingAttributesFromCache(1, 0)
        }
        onSignalCompanyAttributesLoaded: {
            console.log("Definitions SCAP - Signal SCAP company attributes loaded")
            if (isLoadingAttributes) {
                isLoadingAttributes = false
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("GAPI","STR_POPUP_SUCESS")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("PageDefinitionsSCAP","STR_SCAP_ATTRIBUTES_LOADED")
            
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
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

            if(propertyTabButtonEntities.focus)
                propertyTabButtonEntities.forceActiveFocus()
            if(propertyTabButtonCompanies.focus)
                propertyCompaniesText.forceActiveFocus()
        }
        onSignalRemoveSCAPAttributesSucess: {
            console.log("Definitions SCAP - Signal SCAP Signal Remove SCAP Attributes Sucess")
            propertyBusyIndicatorAttributes.running = false
               mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE_SUCC")

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            if(isCompanies == false){
                gapi.startGettingEntities()
                propertyBusyIndicator.running = true
            }
        }
        onSignalRemoveSCAPAttributesFail: {
            console.log("Definitions SCAP - Signal Remove SCAP Attributes Fail")
            propertyBusyIndicatorAttributes.running = false
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE_FAIL")

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            if(isCompanies == false){
                gapi.startGettingEntities()
                propertyBusyIndicator.running = true
            }
        }
        onSignalCacheNotReadable:{
            propertyBusyIndicatorAttributes.running = false;
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CACHE_NOT_READABLE")

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();

            if(isCompanies == false){
                gapi.startGettingEntities()
                propertyBusyIndicator.running = true
            }
        }
        onSignalCacheNotWritable:{
            propertyBusyIndicatorAttributes.running = false;
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CACHE_NOT_WRITABLE")

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalCacheFolderNotCreated:{
            propertyBusyIndicatorAttributes.running = false;
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDataApp","STR_CLEAR_CACHE")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDataApp","STR_CACHE_FOLDER_NOT_CREATED")

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalBeginOAuth: { 
            cmdLoadDialog.open()
        }
        onSignalEndOAuth: {
            cmdLoadDialog.close()
            // Error codes from OAuthResult in applayer/OAuthAttributes.h
            switch (oauthResult){
            case 0://OAuthSuccess
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("PageDefinitionsSCAP","STR_LOADING_SCAP_ATTRIBUTES")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("OAuth","STR_OAUTH_SUCCESS_SCAP")
                break;
            case 1://OAuthTimeoutError
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("OAuth","STR_OAUTH_TIMEOUT")
                break;
            case 2://OAuthGenericError  
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("OAuth","STR_OAUTH_GENERIC_ERROR")
            case 3://OAuthConnectionError
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("OAuth","STR_OAUTH_CONNECTION_ERROR")
                break;
            case 4://OAuthCancelled
                console.log("oauth - cancelado")
                return;
            }
            if (oauthResult != 0)
                propertyBusyIndicatorAttributes.running = false
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
                
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
                    propertyButtonRemoveEntityAttributes.forceActiveFocus()
                }else{
                    propertyListViewEntities.currentIndex++
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
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: attrTitle
                    text: '<b>'+ qsTranslate("PageDefinitionsSCAP","STR_SCAP_ATTR") + '</b>'
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: attrText
                    width: parent.width
                    wrapMode: Text.WordWrap
                    text: attribute
                }
            }
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
                    propertyListViewCompanies.currentIndex = 0
                    propertyButtonRemoveCompanyAttributes.forceActiveFocus()
                }else{
                    propertyListViewCompanies.currentIndex++
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
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: attrTitle
                    text: '<b>'+ qsTranslate("PageDefinitionsSCAP","STR_SCAP_ATTR") + '</b>'
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    id: attrText
                    width: parent.width
                    wrapMode: Text.WordWrap
                    text: attribute
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
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
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
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }
        }
        Button {
            width: Constants.WIDTH_BUTTON
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            font.pixelSize: Constants.SIZE_TEXT_FIELD
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTranslate("OAuth","STR_ABORT_AUTH")
            font.capitalization: Font.MixedCase
            onClicked: {
                cmdLoadDialog.close()
                gapi.abortSCAPWithCMD()
                propertyBusyIndicatorAttributes.running = false

            }
        }
        onRejected: {
            cmdLoadDialog.open()
        }

    }

    propertyButtonLoadCompanyAttributes {
        onClicked: {
            console.log("ButtonLoadCompanyAttributes clicked!")
            isLoadingAttributes = true
            companyAttributesModel.clear()
            propertyBusyIndicatorAttributes.running = true
            propertyPageLoader.attributeListBackup = []
            gapi.startGettingCompanyAttributes(false)
        }
    }
    propertyButtonLoadCompanyAttributesOAuth {
        onClicked: {
            console.log("ButtonLoadCompanyAttributesOAuth clicked!")
            isLoadingAttributes = true
            companyAttributesModel.clear()
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

    propertyMouseAreaTextSignaturePageLinkEntities {
        onClicked: {
            console.log("propertyMouseAreaTextSignaturePageLinkEntities clicked!")
            returnToAdvancedSignaturePage()
        }
    }

    propertyMouseAreaTextSignaturePageLinkCompanies {
        onClicked: {
            console.log("propertyMouseAreaTextSignaturePageLinkCompanies clicked!")
            returnToAdvancedSignaturePage()
        }
    }

    Component.onCompleted: {
        console.log("Page Definitions SCAP Completed")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
        propertyBusyIndicatorAttributes.running = true
        isLoadingAttributes = false
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
            }
        if(propertyTabButtonCompanies.focus)
            if(key===Qt.Key_Return || key===Qt.Key_Space){
                propertyStackLayout.currentIndex = propertyBar.currentIndex = 1
                propertyCompaniesText.forceActiveFocus()
            }
    }
}
