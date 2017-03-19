import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import awesome.app.fdt 1.0

import "../components"

ColumnLayout{
    id: mainColumn

    RefLogGenerator{
        id: generator;
        onMessage: {
            msgInfo.text = txt;
            msgInfo.open()
        }
        currentFrame: video.position
        videoFile: "/home/kostyan/Видео/digit_1.avi"
    }

    MessageDialog{
        id: msgInfo
        title: "Info"
        standardButtons: StandardButton.Ok;
        icon: StandardIcon.Information
    }

    TextField{
        id: title
        Layout.preferredWidth: parent.width*0.2
        Layout.alignment: Qt.AlignHCenter
        anchors{
            left: video.left
            bottom: video.top
            margins: 20
        }
        Layout.preferredHeight: 25
        placeholderText: "Enter log title"
        onTextChanged: generator.setTitle(text);
        focus: true;
        Keys.onPressed:{
            if (event.key == Qt.Key_Enter-1) {
                focus = false;
                mainColumn.focus = true;
            }
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

    RowLayout{
        id: naviRow
        Layout.alignment: Qt.AlignHCenter
        anchors.top: controls.bottom
        anchors.margins: 10
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
            id: nextBtn
            text: ">>"
            enabled: generator.isWriting
            onClicked: {
                generator.saveStrobe(strobe.getRealStrobe(), strobe.getAngle())
                video.seek(video.position+1);
            }
        }
    }

    RowLayout{
        id: controlRow
        Layout.alignment: Qt.AlignHCenter
        anchors.top: naviRow.bottom
        anchors.margins: 10
        Button{
            id: startBtn
            text: (generator.isWriting) ? "Stop & save"
                                        : "Start"
            onClicked: {
                if (!generator.isWriting) {
                    generator.firstFrame = video.position
                    generator.startWriting()
                }
                else{
                    generator.stopWriting()
                    generator.saveLog()
                }
            }
        }
        Button{
            id: clearBtn
            text: "Clear strobe"
            enabled: generator.isWriting
            onClicked: strobe.updateStrobe(Qt.rect(0,0,0,0))
        }
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
    Keys.onPressed: strobe.keyReact(event.key);
    Keys.onReleased: strobe.keyReact(event.key, event.key == Qt.Key_Shift)
    // key catcher end
}
