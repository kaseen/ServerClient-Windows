import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Item {
    anchors.fill: parent
    anchors.topMargin: parent.height/2.4
    
    Connections {
        target: client
        function onConnectionSuccess(ba){
            mainLoader.source = "ClientUI.qml"
        }
    }
    
    ColumnLayout {
        width: parent.width
        spacing: 15
        TextField {
            id: ip
            Layout.preferredWidth: 100
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            placeholderText: qsTr("Enter IP")
            text: "localhost"
        }
        TextField {
            id: port
            Layout.preferredWidth: 100
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            placeholderText: qsTr("Enter Port")
            text: "12345"
        }
        Button {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("login")
            onClicked: {
                client.connectToHost(ip.text, port.text)
            }
        }
    }
}
