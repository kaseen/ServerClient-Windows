import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
//import QtQuick.Dialogs 1.5
import Qt.labs.platform as Labs

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Client")

    function getName(url){
        return (url.slice(url.lastIndexOf("/")+1))
    }
    
    Connections {
        target: client
        function onNewMessage(ba){
            listModelMessages.append({message: ba + ""})
        }
    }

    Labs.FileDialog {
        id: fileDialog
        title: "Please choose a file"
        //folder: "file:///C:/"
        //nameFilters: "*.exe"
        folder: "file:///C:/Users/tasic/OneDrive/Documents/test/"
        onAccepted: {
            filename.text = getName(this.file.toString())
        }
    }

    ColumnLayout {
        anchors.fill: parent
        Button {
            text: "Open file dialog"
            onClicked: fileDialog.open()
        }
        Label {
            id: filename
        }
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
