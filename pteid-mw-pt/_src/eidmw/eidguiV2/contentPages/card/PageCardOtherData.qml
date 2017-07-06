import QtQuick 2.6

//Import C++ defined enums
import eidguiV2 1.0

PageCardOtherDataForm {

/*
	Connections {
		target: gapi
		onSignalCardIdentifyChanged: {
			console.log("OtherData --> Data Changed")
			console.trace();
            //TODO: complete this
            propertyTextBoxNIF.propertyDateField.text = gapi.getDataCardIdentifyValue(GAPI.NIF)
            
        }
    }
    */

          propertyTextBoxNIF {
          	propertyDateField.text : gapi.getDataCardIdentifyValue(GAPI.NIF)
          }
          propertyTextBoxNISS {
          	propertyDateField.text : gapi.getDataCardIdentifyValue(GAPI.NISS)
          }
          propertyTextBoxNSNS {
          	propertyDateField.text : gapi.getDataCardIdentifyValue(GAPI.NSNS)
          }
          propertyTextBoxCardVersion {
          	propertyDateField.text : gapi.getDataCardIdentifyValue(GAPI.Documentversion)
          }
          propertyTextBoxIssueDate {
          	propertyDateField.text : gapi.getDataCardIdentifyValue(GAPI.Validitybegindate)
          }
          propertyTextBoxIssuingEntity {
          	propertyDateField.text : gapi.getDataCardIdentifyValue(GAPI.IssuingEntity)
          }
          propertyTextBoxDocumentType {
          	propertyDateField.text : gapi.getDataCardIdentifyValue(GAPI.Documenttype)
          }
          propertyTextBoxPlaceOfRequest {
          	propertyDateField.text : gapi.getDataCardIdentifyValue(GAPI.PlaceOfRequest)
          }
          propertyTextBoxCardState {
			      propertyDateField.text : gapi.getCardActivation()
		      }
}