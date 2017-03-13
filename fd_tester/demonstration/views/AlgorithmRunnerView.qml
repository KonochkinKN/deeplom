import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import awesome.app.fdt 1.0

import "../components"

RowLayout{
    id: mainRow

    function init(){
        video.setTemplate("/home/kostyan/Изображения/Выделение_017.png");
        video.setAlgorithm(1)
    }

    Component.onCompleted: init();

    Application{ id: app}

    Rectangle{
        color: "#41454A"
        Layout.preferredWidth: 100
        Layout.preferredHeight: childrenRect.height
        Layout.alignment: Qt.AlignHCenter
        anchors.top: videoCol.top

        GroupBox{
            id: algBox
            Layout.preferredWidth: 100
            Layout.preferredHeight: 100
            Layout.alignment: Qt.AlignCenter
            flat: false
            title: "Algorithms"

            Column{
                Repeater{
                    id: algList
                    model: app.algorithms().slice(1);
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

    ColumnLayout{
        id: videoCol
        Layout.alignment: Qt.AlignCenter

        VideoComponent{
            id: video
            sourcePath: "/home/kostyan/Видео/edit_2.avi"
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

        Item{ width: 10; height:10;}

        Button{
            id: startBut
            text: (video.detecting) ? "Stop" : "Start"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                if(!video.detecting) video.startDetecting()
                else video.stopDetecting()
            }
        }
    }


    Image{
        id: template
        Layout.preferredWidth: parent.width*0.2
        Layout.preferredHeight: parent.height*0.2
        Layout.alignment: Qt.AlignCenter
        anchors.top: videoCol.top
        fillMode: Image.PreserveAspectFit
        source: "file://home/kostyan/Изображения/Выделение_017.png"
    }
}
