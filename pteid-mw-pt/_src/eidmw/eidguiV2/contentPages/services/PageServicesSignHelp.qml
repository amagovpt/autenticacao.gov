import QtQuick 2.6
import QtQuick.Controls 2.1

PageServicesSignHelpForm {

    propertyTextSubTitle{
        onLinkActivated: {
            Qt.openUrlExternally(link)
        }
    }

    propertyTextAutenticacaoGovLink{
        onLinkActivated: {
            Qt.openUrlExternally(link)
        }
    }
}
