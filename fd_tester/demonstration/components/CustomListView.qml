import QtQuick 2.7

Rectangle{
    id: mainItem
    color: "white"
    clip: true

    property variant listModel
    property string title

    property int currentIndex: list.model.length > 0 ? list.currentIndex : -1
    property string currentText: (list.currentIndex >= 0) ? list.currentItem.text : ""

    Component{
        id: listDelegate

        Rectangle{
            property string text: textField.text
            color: (index == list.currentIndex)
                   ? "orange" : ((index % 2) ? "gray" : "darkgray")
            width: parent.width
            height: childrenRect.height

            Text{
                id: textField
                text: modelData
                width: parent.width
                horizontalAlignment: Qt.AlignHCenter
            }

            MouseArea{
                anchors.fill: parent
                onClicked: list.currentIndex = index
            }
        }
    }

    Component{
        id: listHeader

        Text{
            id: textHeader
            text: mainItem.title
            color: "white"
            font.bold: true
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            width: parent.width
            height: text.length == 0 ? 0 : 20

            Rectangle{
                color: "black"
                z: parent.z - 2
                anchors.fill: parent
            }
        }
    }

    ListView {
        id: list
        anchors.fill: parent

        header: listHeader
        headerPositioning: ListView.OverlayHeader

        delegate: listDelegate

        model: listModel
    }
}
