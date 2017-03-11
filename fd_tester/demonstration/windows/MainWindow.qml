import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

import "../views"

Rectangle {

    id: main
    color: "#95500C"

    ColumnLayout{
        anchors.fill: parent

        Button{
            text: "Create reference logs"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: Qt.createQmlObject('RefLogGeneratorWindow{
                                            visible: true}', mainView)
        }

        Button{
            text: "Create testing logs"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked:  Qt.createQmlObject('AlgorithmRunnerWindow{
                                            visible: true}', mainView)
        }

        Button{
            text: "Analyze"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: console.log("Nope!")
        }
    }
}
