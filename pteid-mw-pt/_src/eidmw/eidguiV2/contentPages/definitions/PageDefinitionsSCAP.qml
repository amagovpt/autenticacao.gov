import QtQuick 2.6
import QtQuick.Controls 2.1

//Import C++ defined enums
import eidguiV2 1.0

import "../../scripts/Constants.js" as Constants

PageDefinitionsSCAPForm {

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
            else {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_ERROR") + controler.autoTr
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR") + controler.autoTr
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();

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
        onSignalEntityAttributesLoadedError: {
            console.log("Definitions SCAP - Signal SCAP entities loaded error")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDifinitionsSCAP","STR_SCAP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDifinitionsSCAP","STR_SCAP_LOAD_ENTITIES_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            if(propertyBar.currentIndex == 0)
                propertyBusyIndicatorAttributes.running = false
        }
        onSignalCompanyAttributesLoadedError: {
            console.log("Definitions SCAP - Signal SCAP company loaded error")
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageDifinitionsSCAP","STR_SCAP_ERROR")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageDifinitionsSCAP","STR_SCAP_LOAD_COMPANY_ERROR")
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
                                                 entityName: entitiesList[i], attribute: "", checkBoxAttr: false
                                             });
            }

            // Load attributes from cache (Entities, isShortDescription)
            gapi.startLoadingAttributesFromCache(0, 0)
            propertyBusyIndicator.running = false

        }
        onSignalEntityAttributesLoaded:{
            console.log("Definitions SCAP - Signal SCAP Entity attributes loaded")
            propertyButtonRemoveEntityAttributes.enabled = false
            if(entityAttributesModel.count == 0){
                for(var i = 0; i < attribute_list.length; i=i+3)
                {
                    entityAttributesModel.append({
                                                     entityName: attribute_list[i], attribute: "<ul><li>"+attribute_list[i+2]+"</li></ul>"
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
                        propertyButtonRemoveEntityAttributes.enabled = true
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
                                                      entityName: attribute_list[i], attribute: "<ul><li>"+attribute_list[i+2]+"</li></ul>"
                                                  });
                }
            }
            propertyBusyIndicatorAttributes.running = false
        }
        onSignalRemoveSCAPAttributesSucess: {
            console.log("Definitions SCAP - Signal SCAP Signal Remove SCAP Attributes Sucess")
            propertyBusyIndicatorAttributes.running = false
            if(isCompanies == false){
                gapi.startGettingEntities()
                propertyBusyIndicator.running = true
            }
        }
        onSignalRemoveSCAPAttributesFail: {
            console.log("Definitions SCAP - Signal Remove SCAP Attributes Fail")
            propertyBusyIndicatorAttributes.running = false
            if(isCompanies == false){
                gapi.startGettingEntities()
                propertyBusyIndicator.running = true
            }
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
            propertyPageLoader.propertyBackupSignAdd = false
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
            propertyPageLoader.propertyBackupSignAdd = false
            gapi.startGettingEntityAttributes(attributeList)
        }
    }
    propertyButtonRemoveCompanyAttributes {
        onClicked: {
            console.log("propertyButtonRemoveCompanyAttributes clicked!")
            companyAttributesModel.clear()
            propertyBusyIndicatorAttributes.running = true
            propertyPageLoader.attributeListBackup = []
            propertyPageLoader.propertyBackupSignAdd = false
            gapi.startRemovingAttributesFromCache(1)
        }

    }

    propertyButtonRemoveEntityAttributes {
        onClicked: {
            console.log("propertyButtonRemoveEntityAttributes clicked!")
            entityAttributesModel.clear()
            propertyBusyIndicatorAttributes.running = true
            propertyPageLoader.attributeListBackup = []
            propertyPageLoader.propertyBackupSignAdd = false
            gapi.startRemovingAttributesFromCache(0)
        }
    }

    Component.onCompleted: {
        console.log("Page Definitions SCAP Completed")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }
}
