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

    function getFilePath(url){
        return (url.slice(url.indexOf("///")+3))
    }
    
    Connections {
        target: client
        function onNewMessageQML(ba){
            listModelMessages.append({message: ba + ""})
        }
    }

    Labs.FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: "file:///C:/"
        //nameFilters: "*.exe"
        onAccepted: {
            filename.text = getName(this.file.toString())
            sendFile.enabled = true
        }
        function send(){
            client.sendFile(getFilePath(this.file.toString()))
        }
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout{
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
            ColumnLayout{
                Layout.alignment: Qt.AlignTop
                Button {
                    Layout.preferredWidth: 100
                    text: "Choose file"
                    onClicked: fileDialog.open()
                }
                TextField {
                    Layout.preferredWidth: 100
                    readOnly: true
                    id: filename
                }
                Button {
                    Layout.preferredWidth: 100
                    id: sendFile
                    enabled: false
                    text: "Send file"
                    onClicked: fileDialog.send()
                }
            }
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
