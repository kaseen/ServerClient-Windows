import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Client")
    
    Connections {
        target: client
        function onNewMessage(ba){
            listModelMessages.append({message: ba + ""})
        }
    }

    ColumnLayout {
        anchors.fill: parent
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ListModel {
                id: listModelMessages
            }
            delegate: ItemDelegate {
                text: message
            }
            ScrollBar.vertical: ScrollBar {}
        }
        RowLayout {
           Layout.fillWidth: true
           TextField {
               id: textFieldMessage
               placeholderText: qsTr("Your message...")
               Layout.fillWidth: true
               onAccepted: buttonSend.clicked()
           }
           Button {
               id: buttonSend
               text: qsTr("Send")
               onClicked: {
                   client.sendMessage(textFieldMessage.text + "\n")
                   textFieldMessage.clear()
               }
           }
        }
    }
}
