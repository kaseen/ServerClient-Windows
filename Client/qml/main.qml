import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Client")
    
    Loader {
        anchors.fill: parent
        id: mainLoader
        source: "LoginPage.qml"
    }
}
