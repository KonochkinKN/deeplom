import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

ColumnLayout{

    property double duration
    property int fps
    property bool hasVideo
    property bool playing
    property int position

    signal seek(int pos);
    signal play()
    signal pause()


    onPositionChanged: {
        if (!slider.pressed && !slider.hovered)
            slider.value = Math.max(0, position/fps)
    }

    Slider{
        id: slider
        Layout.preferredWidth: parent.width
        enabled: hasVideo

        property bool playingState

        minimumValue: 0
        maximumValue: Math.max(duration/1000.0, minimumValue)
        updateValueWhileDragging: true

        onPressedChanged: {
            if (pressed){
                playingState = video.playing
                video.pause()
            }
            else if(playingState)
                video.play()
        }

        onValueChanged: {
            if (pressed){
                if (value > 0)
                    seek(value*fps)
            }
        }
    }

    RowLayout{
        id: playerRow
        Layout.alignment: Qt.AlignHCenter

        Button{
            id: backSec
            text: "⏮"//"❙◀"
            enabled: hasVideo
            onClicked: {
                pause()
                var newPos = position - fps
                if (newPos < 0)
                    newPos = 0
                seek(newPos)
            }
        }

        Button{
            id: backFrame
            text: "⏪"//"◀◀"
            enabled: hasVideo
            onClicked: {
                pause()
                var newPos = position - 1
                if (newPos < 0)
                    newPos = 0
                seek(newPos)
            }
        }

        Button{
            id: playBtn
            text: (playing) ? "⏸" : "▸"//"▶" "▮▮"
            onClicked: (playing) ? pause() : play()
            enabled: hasVideo
        }

        Button{
            id: nextFrame
            text: "⏩"//"▶▶"
            enabled: hasVideo
            onClicked: {
                pause()
                var newPos = position + 1
                if (newPos > duration/1000.0*fps){
                    newPos = duration/1000.0*fps
                }
                seek(newPos)
            }
        }

        Button{
            id: nextSec
            text: "⏭"//""▶❙"
            enabled: hasVideo
            onClicked: {
                pause()
                var newPos = position + fps
                if (newPos > duration/1000.0*fps){
                    newPos = duration/1000.0*fps;
                }
                seek(newPos)
            }
        }
    }
}
