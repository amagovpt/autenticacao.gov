import QtQuick 2.4

import "../../scripts/Constants.js" as Constants

PageDefinitionsSCAPForm {

    Connections {
            target: gapi
            onSignalSCAPEntitiesLoaded: {
                console.log("Definitions SCAP - Signal SCAP entities loaded")

                for (var i = 0; i < entitiesList.length; i++)
                {
                    entityAttributesModel.append({
                        entityName: entitiesList[i], attribute: ""
                    });
                }

                propertyBusyIndicator.running = false
            }

            onSignalCompanyAttributesLoaded: {
                console.log("Definitions SCAP - Signal SCAP company attributes loaded")

                for (var company in attribute_map) {

                    companyAttributesModel.append({
                        entityName: company, attribute: attribute_map[company]
                    });
                }

                propertyBusyIndicator.running = false
            }
    }

    Component {
        id: attributeListDelegate
        Item {
            width: parent.width
            height: 40
            id: container
            Column {
                Text {
                    text: '<b>Entidade:</b> ' + entityName
                }
                Text {
                    text: attribute.length > 0 ? '      <b>Atributo:</b> ' + attribute : ""
                }
                /*
                MouseArea {
                    id: mouse_area1
                    z: 1
                    hoverEnabled: false
                    anchors.fill: parent

                    onClicked:{
                        console.log("AttribuetList item clicked");
                        container.ListView.view.currentIndex = index
                        container.forceActiveFocus()
                    }
                } */
            }

        }
    }

    Component {
        id: companiesAttributeListDelegate
        Item {
            width: parent.width
            height: 40
            Column {
                Text {
                    text: '<b>Empresa:</b> ' + entityName
                }
                Text {
                    text: attribute.length > 0 ? '      <b>Atributo:</b> ' + attribute : ""
                }
            }
        }
    }

    ListModel {
            id: companyAttributesModel

            onCountChanged: {
            }
    }

    ListModel {
            id: entityAttributesModel

            onCountChanged: {
            }
    }

    propertyButtonLoadCompanyAttributes {
           onClicked: {
               console.log("ButtonLoadCompanyAttributes clicked!")
               propertyBusyIndicator.running = true
               gapi.startGettingCompanyAttributes()
           }

    }

    propertyButtonLoadEntityAttributes {
        onClicked: {
            console.log("ButtonLoadEntityAttributes clicked!")
        }
    }

    Component.onCompleted: {
        console.log("Page Definitions SCAP Completed")
        propertyBusyIndicator.running = true
        gapi.startGettingEntities()
        gapi.loadCompanyAttributesFromCache()
    }
}
