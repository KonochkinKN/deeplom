import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import awesome.app.fdt 1.0

import "../components"

ColumnLayout{
    id: mainColumn
    signal close()

    Storage{ id: storage;}

    Component.onCompleted: videoFile.update()

    RefLogGenerator{
        id: generator;
        onMessage: {
            msgInfo.text = txt;
            msgInfo.open()
        }
        currentFrame: video.position
        videoFile: videoFile.filePath
    }

    MessageDialog{
        id: msgInfo
        title: qsTr("Info")
        standardButtons: StandardButton.Ok;
        icon: StandardIcon.Information
    }

    MessageDialog{
        id: closeDialog
        title: qsTr("Closing comfirmation")
        text: qsTr("Do you want to leave without saving?")
        standardButtons: StandardButton.Yes | StandardButton.No
        icon: StandardIcon.Question
        onYes: mainColumn.close()
    }

    TextField{
        id: title
        enabled: !generator.isWriting
        Layout.preferredWidth: parent.width*0.2
        Layout.alignment: Qt.AlignHCenter
        anchors{
            left: video.left
            bottom: video.top
            bottomMargin: 20
        }
        Layout.preferredHeight: 25
        placeholderText: qsTr("Enter log title...")
        onTextChanged: generator.setTitle(text);
        focus: true;
        Keys.onPressed:{
            if (event.key == Qt.Key_Enter-1) {
                focus = false;
                mainColumn.focus = true;
            }
        }
    }

    FileSelectionComponent{
        id: videoFile
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: video.width
        title: qsTr("Video file selection")
        dialogTitle: qsTr("Select a video")
        name: Storage.VideoFile
        listModel: storage.loadFilePaths(name)
        enabled: !generator.isWriting
    }

    VideoComponent{
        id: video
        sourcePath: videoFile.filePath
        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: video.hasVideo
                               ? Math.min(video.videoWidth, 640) : 640
        Layout.preferredHeight: video.hasVideo
                                ? Math.min(video.videoHeight, 480) : 480

        onPositionChanged: controls.position = video.position
    }

    StrobeDrawComponent{
        id: strobe
        contentWidth: video.videoWidth
        contentHeight: video.videoHeight
        z: parent.z+1
        x: video.contentRect.x + video.x
        y: video.contentRect.y + video.y
        width: video.contentRect.width
        height: video.contentRect.height
    }

    VideoControlBar{
        id: controls
        enabled: !generator.isWriting && video.hasVideo
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

    RowLayout{
        id: detectRow
        Layout.alignment: Qt.AlignHCenter
        anchors.top: controls.bottom
        anchors.margins: 10

        Button{
            id: startBtn
            enabled: video.hasVideo
            text: (generator.isWriting) ? qsTr("Stop'n'save")
                                        : qsTr("Start")
            onClicked: {
                if (!generator.isWriting) {
                    generator.firstFrame = video.position
                    generator.startWriting()
                }
                else{
                    generator.stopWriting()
                    storage.saveFilePath(videoFile.name, videoFile.filePath)
                }
            }
        }

        Button{
            id: prevBtn
            text: "<<"
            enabled: generator.isWriting
            onClicked: {
                if (video.position > 0)
                    video.seek(video.position-1)
            }
        }

        Button{
            id: clearBtn
            text: qsTr("Clear strobe")
            enabled: generator.isWriting
            onClicked: strobe.updateStrobe(Qt.rect(0,0,0,0))
        }

        Button{
            id: nextBtn
            text: ">>"
            enabled: generator.isWriting
            onClicked: {
                generator.saveStrobe(strobe.getRealStrobe(), strobe.getAngle())
                video.seek(video.position+1);
            }
        }

        Button{
            id: closeBtn
            text: qsTr("Close")
            Layout.alignment: Qt.AlignHCenter
            anchors.margins: 10
            onClicked: {
                if (generator.isWriting){
                    closeDialog.open()
                }
                else
                    mainColumn.close()
            }
        }
    }

    /*! $ key catcher $
        R       = rotate clockwise
        right   = move right
        left    = move left
        up      = move up
        down    = move down

        Shift +
        R       = rotate counterclockwise
        right   = width++
        left    = width--
        up      = height++
        down    = height--
      */
    focus: true;
    Keys.onPressed: strobe.keyReact(event.key);
    Keys.onReleased: strobe.keyReact(event.key, event.key == Qt.Key_Shift)
    // key catcher end
}
