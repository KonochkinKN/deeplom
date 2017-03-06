import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import awesome.app 1.0

import "../components"

ColumnLayout{
    id: mainColumn

    RefLogGenerator{ id: generator;}

    VideoComponent{
        id: video
        sourcePath: "/home/kostyan/digit_1.avi"
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
}
