import QtQuick 2.6
import QtQuick 2.0
import QtQuick.Window 2.2

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

Item {
    anchors.fill: parent
    visible: true

    property alias propertyAcordion: acordion
    property alias propertyTextEntity: textEntity
    property alias propertyTextAuth: textAuth
    property alias propertyTextValid: textValid
    property alias propertyTextUntil: textUntil
    property alias propertyTextKey: textKey
    property alias propertyTextStatus: textStatus

    Item {
        id: rowTop
        width: parent.width
        height: parent.height

        Item{
            id: rectTop
            width: parent.width
            height: parent.height * 0.4
            Components.Accordion {
                id: acordion
                anchors.fill: parent
                anchors.margins: 10

                model: [
                    {
                        'entity': 'Baltimore CyberTrust Root',
                        'auth': 'Baltimore CyberTrust Root',
                        'valid': '23/10/2014',
                        'until': '21/10/2019',
                        'key': '2048',
                        'status': 'Válido',
                        'children': [
                            {
                                'entity': 'ECRaizEstado',
                                'auth': 'Baltimore CyberTrust Root',
                                'valid': '23/10/2014',
                                'until': '21/10/2019',
                                'key': '4096',
                                'status': 'Válido',
                                'children': [
                                    {
                                        'entity': 'Cartão de Cidadão 002',
                                        'auth': 'ECRaizEstado',
                                        'valid': '23/10/2014',
                                        'until': '21/10/2019',
                                        'key': '4096',
                                        'status': 'Válido',
                                        'children': [
                                            {
                                                'entity': 'EC de Autenticação do Cartão de Cidadão 0009',
                                                'auth': 'Cartão de Cidadão 002',
                                                'valid': '23/10/2014',
                                                'until': '21/10/2019',
                                                'key': '2048',
                                                'status': 'Válido',
                                                'children': [
                                                    {
                                                        'entity': 'ADRIANO JOSÉ RIBEIRO CAMPOS',
                                                        'auth': 'EC de Autenticação do Cartão de Cidadão 0009',
                                                        'valid': '23/10/2014',
                                                        'until': '21/10/2019',
                                                        'key': '1024',
                                                        'status': 'Válido'
                                                    }
                                                ]
                                            },
                                            {
                                                'entity': 'EC de Assinatura Digital Qualificada do Cartão de Cidadão 0009',
                                                'auth': 'Cartão de Cidadão 002',
                                                'valid': '23/10/2014',
                                                'until': '21/10/2019',
                                                'key': '2048',
                                                'status': 'Válido',
                                                'children': [
                                                    {
                                                        'entity': 'ADRIANO JOSÉ RIBEIRO CAMPOS',
                                                        'auth': 'EC de Assinatura Digital Qualificada do Cartão de Cidadão 0009',
                                                        'valid': '23/10/2014',
                                                        'until': '21/10/2019',
                                                        'key': '1024',
                                                        'status': 'Válido'
                                                    }
                                                ]
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                    }
                ]
            }
        }
        Item{
            id: rectBottom
            width: parent.width
            height: parent.height * 0.6
            anchors.top: rectTop.bottom
            anchors.topMargin: Constants.SIZE_ROW_V_SPACE

            Item{
                id: rectEntity
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                Components.LabelTextBoxForm{
                    id: textEntity
                    propertyDateText.text: "Cidadão / Entidade"
                    propertyDateField.text: ""
                }
            }
            Item{
                id: rectAuth
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectEntity.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    id: textAuth
                    propertyDateText.text: "Entidade Emissora"
                    propertyDateField.text: ""
                }
            }
            Item{
                id: rectValid
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectAuth.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    id: textValid
                    propertyDateText.text: "Valido desde"
                    propertyDateField.text: ""
                }
            }
            Item{
                id: rectUntil
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectValid.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    id: textUntil
                    propertyDateText.text: "Valid"
                    propertyDateField.text: ""
                }
            }
            Item{
                id: rectKey
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectUntil.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    id: textKey
                    propertyDateText.text: "Tamanho da chave"
                    propertyDateField.text: ""
                }
            }
            Item{
                id: rectStatus
                width: parent.width
                height: Constants.SIZE_TEXT_LABEL
                        + Constants.SIZE_TEXT_V_SPACE
                        + 2 * Constants.SIZE_TEXT_FIELD
                anchors.top: rectKey.bottom
                anchors.topMargin: Constants.SIZE_ROW_V_SPACE
                Components.LabelTextBoxForm{
                    id: textStatus
                    propertyDateText.text: "Estado do certificado"
                    propertyDateField.text: ""
                }
            }
        }
    }
}
