import QtQuick 2.6

PageSecurityCertificatesForm {
    Component.onCompleted: {
        // Init Date Field with the model first option
        propertyTextEntity.propertyDateField.text = propertyAcordion.model[0].entity
        propertyTextAuth.propertyDateField.text = propertyAcordion.model[0].auth
        propertyTextValid.propertyDateField.text = propertyAcordion.model[0].valid
        propertyTextUntil.propertyDateField.text = propertyAcordion.model[0].until
        propertyTextKey.propertyDateField.text = propertyAcordion.model[0].key
        propertyTextStatus.propertyDateField.text = propertyAcordion.model[0].status
    }
}
