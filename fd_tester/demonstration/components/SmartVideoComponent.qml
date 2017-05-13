import QtQuick 2.7
import QtQuick 2.0
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
    property string iterationTime: videoData.iterationTime

    property int position: videoData.position
    property bool hasVideo: videoData.hasVideo
    property int fps: videoData.fps
    property int videoWidth: videoData.width
    property int videoHeight: videoData.height
    property double duration: videoData.durationSec*1000.0
    property bool playing: videoData.playing
    property int endPosition: videoData.endPosition

    property bool detecting: videoData.isDetecting

    onSourcePathChanged: videoData.source = sourcePath;

    signal paused()
    signal endPosReached()

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

    function startDetecting(){
        videoData.startDetecting();
    }

    function stopDetecting(){
        videoData.stopDetecting();
        videoData.position = 0
        videoData.seek(0)
    }

    function setAlgorithm(alg){
        videoData.setAlgType(alg);
    }

    function setTemplate(temp){
        videoData.setTemplateImage(temp)
    }

    MessageDialog{
        id: msgInfo
        title: qsTr("Info")
        standardButtons: StandardButton.Ok;
        icon: StandardIcon.Information
    }

    SmartVideoData{
        id: videoData;
        onMessage: {
            msgInfo.text = txt
            msgInfo.open()
        }
        onEndReached: {
            if (isDetecting) {
                video.stopDetecting();
                endPosReached();
            }
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

    ContrastDigitalNumber{
        number: iterationTime
        numberColor: "white"
        backgroundColor: "black"
        anchors{
            bottom: parent.bottom
            right: parent.right
        }
    }
}
