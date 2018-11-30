import QtQuick 2.6
import QtQuick.Controls 2.1

//Import C++ defined enums
import eidguiV2 1.0

import "../../scripts/Constants.js" as Constants

PageDefinitionsSCAPForm {

    property string popupMsg: ""

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Definitions SCAP Signature --> Data Changed")
            propertyBusyIndicator.running = true
            propertyBusyIndicatorAttributes.running = true
            gapi.startGettingEntities()
            propertyBusyIndicator.running = false
        }
        onSignalCardAccessError: {
            console.log("Definitions SCAP Signature --> onSignalCardAccessError")
            if (error_code == GAPI.NoReaderFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER") + controler.autoTr
            }
            else if (error_code == GAPI.NoCardFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_NO_CARD") + controler.autoTr
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
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            propertyBusyIndicatorAttributes.running = false
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Definitions SCAP Signature --> onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED") + controler.autoTr
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
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

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalSCAPDefinitionsServiceFail: {
            console.log("Definitions SCAP - Signal SCAP service fail Code = "
                        + pdfsignresult + " isCompany = " + isCompany)
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
            // Load attributes from cache (Entities, isShortDescription)
            propertyBusyIndicator.running = false
            propertyBusyIndicatorAttributes.running = false
        }
        onSignalEntityAttributesLoadedError: {
            console.log("Definitions SCAP - Signal SCAP entities loaded error")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_LOAD_ENTITIES_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            if(propertyBar.currentIndex == 0)
                propertyBusyIndicatorAttributes.running = false
        }
        onSignalCompanyAttributesLoadedError: {
            console.log("Definitions SCAP - Signal SCAP company loaded error")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDefinitionsSCAP","STR_SCAP_LOAD_COMPANY_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
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

        }
        onSignalEntityAttributesLoaded:{
            console.log("Definitions SCAP - Signal SCAP Entity attributes loaded")
            if(entityAttributesModel.count == 0){
                for(var i = 0; i < attribute_list.length; i=i+3)
                {
                    entityAttributesModel.append({
                                                     entityName: attribute_list[i],
                                                     attribute: "<ul><li>"+attribute_list[i+2]+"</li></ul>",
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
                            entityAttributesModel.set(j, {"entityName": attribute_list[i], "attribute":
                                                          entityAttributesModel.get(j).attribute
                                                          + "<ul><li>"+attribute_list[i+2]+"</li></ul>"})
                        }
                    }
                }
            }
            //Load attributes from cache (Companies, isShortDescription)
            gapi.startLoadingAttributesFromCache(1, 0)

        }
        onSignalCompanyAttributesLoaded: {
            console.log("Definitions SCAP - Signal SCAP company attributes loaded")
            companyAttributesModel.clear()
            for(var i = 0; i < attribute_list.length; i=i+3)
            {
                if(companyAttributesModel.count>0 &&
                        companyAttributesModel.get(companyAttributesModel.count-1).entityName === attribute_list[i]){
                    companyAttributesModel.set(companyAttributesModel.count-1,{
                                                   entityName: attribute_list[i],
                                                   attribute: companyAttributesModel.get
                                                              (companyAttributesModel.count-1).attribute
                                                              + "<ul><li>"+attribute_list[i+2]+"</li></ul>"
                                               });
                }else{
                    companyAttributesModel.append({
                                                      entityName: attribute_list[i],
                                                      attribute: "<ul><li>"+attribute_list[i+2]+"</li></ul>",
                                                      checkBoxAttr: false
                                                  });
                }
            }
            propertyBusyIndicatorAttributes.running = false
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
    }

    Component {
        id: attributeListDelegate
        Rectangle {
            width: parent.width - propertyEntitiesListViewScroll.width
                   - Constants.SIZE_ROW_H_SPACE * 0.5
            height: columnItem.height + 10
            id: container
            color: Constants.COLOR_MAIN_SOFT_GRAY
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
                    propertyButtonLoadEntityAttributes.enabled = false
                    for (var i = 0; i < entityAttributesModel.count; i++){
                        if(entityAttributesModel.get(i).checkBoxAttr === true){
                            propertyButtonLoadEntityAttributes.enabled = true
                            break
                        }
                    }
                }
            }
            Column {
                id: columnItem
                anchors.left: checkboxSel.right
                width: parent.width - checkboxSel.width - Constants.SIZE_ROW_H_SPACE * 0.5
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    text: '<b>Entidade:</b> ' + entityName
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    text: "<b>Atributos:</b>"
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
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
            width: parent.width - propertyCompaniesListViewScroll.width
                   - Constants.SIZE_ROW_H_SPACE * 0.5
            height: columnItem.height + 10
            id: container
            color: Constants.COLOR_MAIN_SOFT_GRAY
            Column {
                id: columnItem
                width: parent.width - Constants.SIZE_ROW_H_SPACE
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    text: '<b>Entidade:</b> ' + entityName
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
                    text: "<b>Atributos:</b>"
                    width: parent.width
                    wrapMode: Text.WordWrap
                }
                Text {
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

    propertyButtonLoadCompanyAttributes {
        onClicked: {
            console.log("ButtonLoadCompanyAttributes clicked!")
            companyAttributesModel.clear()
            propertyBusyIndicatorAttributes.running = true
            propertyPageLoader.attributeListBackup = []
            gapi.startGettingCompanyAttributes()
        }

    }

    propertyButtonLoadEntityAttributes {
        onClicked: {
            console.log("ButtonLoadEntityAttributes clicked!")
            propertyBusyIndicatorAttributes.running = true
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
            gapi.startGettingEntityAttributes(attributeList)
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
    }

    function returnToAdvancedSignaturePage() {
        propertyPageLoader.propertyBackupFromSignaturePage = false
        mainFormID.state = "STATE_EXPAND"
        mainFormID.propertySubMenuListView.model.clear()
        for(var i = 0; i < mainFormID.propertyMainMenuListView.model.get(1).subdata.count; ++i) {
            console.log("Sub Menu indice " + i + " - "
                        + mainFormID.propertyMainMenuListView.model.get(1).subdata.get(i).subName);
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
}
