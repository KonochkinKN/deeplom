import QtQuick 2.0

Item{
    property int number
    property string numberColor: "black"
    property string backgroundColor: "white"

    width: digitalText.text.length*10 + 4
    height: digitalText.height + 4

    FontLoader{
        id: fontLoader
        source: "qrc:/design/LCDNova.ttf"
    }

    Rectangle{
        color: backgroundColor
        anchors.fill: parent

        Text{
            id: digitalText
            text: number
            color: numberColor

            font.family: fontLoader.name
            font{
                bold: true
                pointSize: 15
            }

            anchors{
                right: parent.right
                rightMargin: 2
                verticalCenter: parent.verticalCenter
            }
        }
    }
}
