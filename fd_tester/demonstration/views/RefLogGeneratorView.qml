import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import awesome.app.fdt 1.0

import "../components"

ColumnLayout{
    id: mainColumn

    RefLogGenerator{ id: generator;}

    RowLayout{
        id: titleRow
        Layout.preferredWidth: parent.width*0.8
        Layout.alignment: Qt.AlignHCenter

        Label{ text: "Log title:";}
        TextArea{
            id: title
            Layout.preferredWidth: parent.width*0.8
            Layout.preferredHeight: 25
        }
    }

    VideoComponent{
        id: video
        sourcePath: "/home/kostyan/Видео/digit_1.avi"
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
        onRectChanged: console.log(videoRect)
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

    /*  $ key catcher $
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
    Keys.onPressed: strobe.keyReact(event.key)
    Keys.onReleased: strobe.keyReact(event.key, event.key == Qt.Key_Shift)
    // key catcher end
}
