import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QtMultimedia 5.0

import awesome.app.fdt 1.0

VideoOutput{
    id: video
    source: videoData
    clip: true
    fillMode: VideoOutput.PreserveAspectFit

    property string sourcePath
    property string refLogFile
    property string testLogFile

    property int position: videoData.position
    property bool hasVideo: videoData.hasVideo
    property int fps: videoData.fps
    property int videoWidth: videoData.width
    property int videoHeight: videoData.height
    property double duration: videoData.durationSec*1000.0
    property bool playing: videoData.playing

    onSourcePathChanged: videoData.source = sourcePath;

    signal paused()

    function seek(pos){
        videoData.seek(pos)
    }

    function play(){
        videoData.play()
    }

    function pause(){
        videoData.pause()
        paused()
    }

    function stop(){
        videoData.stop()
    }

    function setRefLog(log){
        videoData.setRefLog(log)
    }

    function setTestLog(log){
        videoData.setRefLog(log)
    }

    MessageDialog{
        id: msgInfo
        title: qsTr("Info")
        standardButtons: StandardButton.Ok;
        icon: StandardIcon.Information
    }

    CompareVideoData{
        id: videoData
        refLog: refLogFile
        testLog: testLogFile
        onMessage: {
            msgInfo.text = txt
            msgInfo.open()
        }
    }

    LinearGradient{
        anchors.fill: parent
        start: Qt.point(0,height)
        end: Qt.point(width, 0)
        z: videoData.hasVideo ? (parent.z - 1) : (parent.z + 1)

        gradient: Gradient{
            GradientStop {position: 0.0; color: "#1D1D1D"}
            GradientStop {position: 0.5; color: "#B7B7B7"}
            GradientStop {position: 1.0; color: "#1D1D1D"}
        }
    }

    ContrastDigitalNumber{
        number: video.position
        anchors{
            top: parent.top
            right: parent.right
        }
    }
}
