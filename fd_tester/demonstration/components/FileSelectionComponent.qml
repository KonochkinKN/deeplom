import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Qt.labs.folderlistmodel 2.1

GroupBox{

    property string dialogTitle
    property string filePath
    property var listModel
    property int name

    function update(){
        for(var i = 0; i < listModel.length; i++){
            if(list.find(listModel[i]) === -1){
                comboModel.append({text: listModel[i]});
            }
        }
        filePath = list.currentText;
    }

    function add(file){
        var reEXT = /\/[^\/]\w*\.\w*$/;
        var reURL = /^file\:\/\//;
        var pos = file.search(reEXT);
        if(pos !== -1){
            var path = file.substring(0, pos + 1);
            var File = file;
            if(file.search(reURL) === -1){
                if(file.charAt(0) == '/'){
                    File = "file://" + file;
                    path = "file://" + path;
                }
                else
                    return;
            }
            folderModel.fileToGet = file;
            folderModel.fileToSearch = File;
            folderModel.folder = "";
            folderModel.folder = path;
        }
    }

    Timer{
        id: timer
        interval: 500
        onTriggered: add(list.editText)
    }

    FolderListModel{
        id: folderModel
        property string fileToGet;
        property string fileToSearch;
        onFolderChanged: {
            if(folderModel.indexOf(fileToSearch) !== -1){
                if(list.find(fileToGet) === -1){
                    comboModel.append({text: fileToGet});
                }
                list.currentIndex = list.find(fileToGet)
                filePath = list.currentText;
            }
        }
    }

    RowLayout{
        spacing: 2
        clip: true
        anchors.fill: parent

        ComboBox{
            id: list
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            editable: true
            model: ListModel{ id: comboModel;}
            onAccepted: {
                list.currentIndex = 0;
                filePath = list.currentText;
            }
            onEditTextChanged: timer.restart()
            onCurrentIndexChanged: filePath = currentText;

            style: ComboBoxStyle{
                dropDownButtonWidth: 5
                selectedTextColor: "white"
                selectionColor: "#31363B"
                textColor: "black"
            }
        }

        Button{
            text: qsTr("Select")
            onClicked: fileDialog.visible = true;
        }

        FileDialog {
            id: fileDialog
            title: dialogTitle
            selectMultiple: false
            selectFolder: false
            onAccepted: {
                if (list.find(fileUrl.toString()) === -1 && fileUrl.toString() != "") {
                    comboModel.append({text: fileUrl.toString()});
                }
                list.currentIndex = list.find(fileUrl.toString())
                filePath = list.currentText;
            }
        }
    }
}

