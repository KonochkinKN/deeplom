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

    Component.onCompleted: videoFile.update()

    Storage{ id: storage;}

    AnalyzerComponent{
        id: analyzer
        videoFile: videoFile.filePath
        Layout.preferredHeight: parent.height*0.6
        Layout.preferredWidth: 400
        Layout.alignment: Qt.AlignHCenter
    }

    ColumnLayout{
        FileSelectionComponent{
            id: videoFile
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: video.width
            title: qsTr("Video file selection")
            dialogTitle: qsTr("Select a video")
            name: Storage.VideoFile
            listModel: storage.loadFilePaths(name)
        }

        CompareVideoComponent{
            id: video
            sourcePath: videoFile.filePath
            refLogFile: analyzer.refLog
            testLogFile: analyzer.testLog
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: video.hasVideo
                                   ? Math.min(video.videoWidth, 640) : 640
            Layout.preferredHeight: video.hasVideo
                                    ? Math.min(video.videoHeight, 480) : 480

            onPositionChanged: controls.position = video.position
        }

        VideoControlBar{
            id: controls
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
            id: closeBut
            text: qsTr("Close")
            Layout.alignment: Qt.AlignHCenter
            onClicked: mainRow.close()
        }
    }
}
