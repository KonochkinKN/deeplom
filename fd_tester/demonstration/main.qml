import QtQuick 2.7
import QtMultimedia 5.5
import QtQuick.Controls 1.4
//import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

import "windows"

ApplicationWindow{
    id: mainView
    objectName: "mainView"

    title: qsTr("algorithm tester")
    width: 300
    height: 150
    visible: true

//    Application{
//        id: mainObj
//    }

    MainWindow{
        anchors.fill: parent
    }

    MenuBar{
        id: menu

        Menu{
            title: qsTr("File")

            MenuItem{
                text: qsTr("Clean up")
                shortcut: "Ctrl+C"
            }

            MenuItem{
                text: qsTr("Quit")
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }

        Menu{
            title: qsTr("Help")
            MenuItem{
                text: qsTr("About")
                onTriggered: about.open()
            }
        }
    }

    MessageDialog{
        id: about
        title: qsTr("About")
        text: qsTr("Testing software info.")
    }

    menuBar: menu
}

