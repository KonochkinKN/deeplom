import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import "../views"

Window{
    id: window
    width: 1200
    height: 800
    color: "#95500C"
    title: "Alrotithms"
    modality: Qt.ApplicationModal

    property bool closeWindow: false

    onVisibleChanged: {
        if (!visible)
            window.destroy(1)
    }

    Component.onCompleted: showFullScreen()

    onClosing: close.accepted = closeWindow

    AlgorithmRunnerView{
        id: runView
        anchors.fill: parent
        onClose: {
            closeWindow = true
            window.close()
        }
    }
}
