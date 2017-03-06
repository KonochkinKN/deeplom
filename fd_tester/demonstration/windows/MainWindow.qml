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
            id: btn
            text: "Create reference logs"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: Qt.createQmlObject('RefLogGeneratorWindow{
                                            visible: true}', mainView)
        }

        Button{
            text: "Create testing logs"
            onClicked: console.log("Fuck you!")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Button{
            text: "Analyze"
            onClicked: console.log("Fuck you twice!")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
    }
}
