import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle {

    id: main
    color: "#9752B3"

    ColumnLayout{
        anchors.fill: parent

        Button{
            id: btn
            text: "Click me!"
            onClicked: console.log("EEEE")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Button{
            text: "Dont click me!"
            onClicked: console.log("Fuck you!")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

    }
}
