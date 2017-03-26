import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import awesome.app.fdt 1.0

import "../components"

RowLayout{
    id: mainRow

    signal close();

    function init(){
        video.setAlgorithm(QmlManager.ORB)
        videoFile.update()
        objectFile.update()
    }

    Component.onCompleted: init();

    QmlManager{ id: man;}

    Storage{ id: storage;}

    ColumnLayout{
        id: videoCol
        Layout.alignment: Qt.AlignCenter

        FileSelectionComponent{
            id: videoFile
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: video.width
            title: qsTr("Video file selection")
            dialogTitle: qsTr("Select a video")
            name: Storage.VideoFile
            listModel: storage.loadFilePaths(name)
            enabled: !video.detecting
        }

        FileSelectionComponent{
            id: objectFile
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: video.width
            title: qsTr("Object file selection")
            dialogTitle: qsTr("Select an object")
            name: Storage.ObjectFile
            listModel: storage.loadFilePaths(name)
            onFilePathChanged: video.setTemplate(filePath)
            enabled: !video.detecting
        }

        SmartVideoComponent{
            id: video
            sourcePath: videoFile.filePath
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: video.hasVideo
                                   ? Math.min(video.videoWidth, 640) : 640
            Layout.preferredHeight: video.hasVideo
                                    ? Math.min(video.videoHeight, 480) : 480

            onPositionChanged: controls.position = video.position
        }

        VideoControlBar{
            id: controls
            enabled: !video.detecting
            duration: video.duration
            anchors.left: video.left
            anchors.right: video.right
            anchors.top: video.bottom
            fps: video.fps
            hasVideo: video.hasVideo
            playing: video.playing
            position: video.position
            onPause: video.pause()
            onPlay: video.play()
            onSeek: video.seek(pos)
        }

        Button{
            id: startBut
            text: (video.detecting) ? qsTr("Stop") : qsTr("Start")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                if(!video.detecting) video.startDetecting()
                else {
                    video.stopDetecting()
                    storage.saveFilePath(videoFile.name, videoFile.filePath)
                    storage.saveFilePath(objectFile.name, objectFile.filePath)
                }
            }
        }

        Button{
            id: closeBut
            text: qsTr("Close")
            Layout.alignment: Qt.AlignHCenter
            onClicked: mainRow.close()
        }
    }

    ColumnLayout{
        id: dataCol
        Layout.alignment: Qt.AlignCenter
        Text{
            text: qsTr("Object")
            horizontalAlignment: Text.AlignRight
            anchors.bottom: objRect.top
            anchors.left: objRect.left
            color: "white"
            Rectangle{
                color: "#41454A";
                height: 30
                width: objRect.width
                z: parent.z - 1
            }
        }

        Rectangle{
            id: objRect
            color: "#41454A"
            visible: (objectFile.filePath != "")
            Layout.preferredWidth: mainRow.width*0.3
            Layout.preferredHeight: mainRow.height*0.3
            Layout.alignment: Qt.AlignHCenter

            Image{
                id: template
                anchors.fill: parent
                Layout.alignment: Qt.AlignCenter
                fillMode: Image.PreserveAspectFit
                source: objectFile.filePath
            }
        }

        Rectangle{
            id: algRect
            color: "#41454A"
            anchors.left: objRect.left
            anchors.top: objRect.bottom
            anchors.topMargin: 10
            Layout.preferredWidth: 100
            Layout.preferredHeight: childrenRect.height
            Layout.alignment: Qt.AlignHCenter

            GroupBox{
                id: algBox
                enabled: !video.detecting
                Layout.preferredWidth: 100
                Layout.preferredHeight: 100
                Layout.alignment: Qt.AlignCenter
                flat: false
                title: "Algorithms"

                Column{
                    Repeater{
                        id: algList
                        model: man.algorithms().slice(1);

                        RadioButton{
                            text: modelData
                            Component.onCompleted: checked = !index;
                            onClicked: {
                                for(var i = 0; i < parent.children.length-1; i++)
                                    parent.children[i].checked = false
                                checked = true;
                                video.setAlgorithm(index+1);
                            }
                        }
                    }
                }
            }
        }
    }
}
