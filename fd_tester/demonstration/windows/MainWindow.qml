import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

import "../views"

Rectangle {

    id: main
    color: "#95500C"

    ColumnLayout{
        anchors.fill: parent

        Image{
            id: logo
            Layout.alignment: Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
            source: "qrc:/design/logo.png"
        }

        Button{
            id: refBtn
            anchors{
                top: logo.bottom
                topMargin: 10
            }
            text: qsTr("Create reference logs")
            Layout.alignment: Qt.AlignHCenter
            onClicked: Qt.createQmlObject('RefLogGeneratorWindow{
                                            visible: true}', mainView)
        }

        Button{
            id: testBtn
            anchors{
                top: refBtn.bottom
                topMargin: 10
            }
            text: qsTr("Create testing logs")
            Layout.alignment: Qt.AlignHCenter
            onClicked: Qt.createQmlObject('AlgorithmRunnerWindow{
                                            visible: true}', mainView)
        }

        Button{
            anchors{
                top: testBtn.bottom
                topMargin: 10
            }
            text: qsTr("Analyze")
            Layout.alignment: Qt.AlignHCenter
            onClicked: Qt.createQmlObject('AnalyzerWindow{
                                            visible: true}', mainView)
        }
    }
}
