import QtQuick 2.7
import QtMultimedia 5.5
import QtQuick.Controls 1.4
import QtQuick.Controls 2.1 as NewControls
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import awesome.app.fdt 1.0

import "windows"

ApplicationWindow{
    id: mainView
    objectName: "mainView"
    title: qsTr("Algorithm tester")
    width: 400
    height: 400
    visible: true

//    Component.onCompleted: showFullScreen()

    QmlManager{ id: mainObj;}

    MainWindow{ anchors.fill: parent;}


    NewControls.Popup {
        id: popup
        x: 0
        y: 0
        width: parent.width*0.8
        height: 30
        modal: true
        focus: true
        closePolicy: NewControls.Popup.CloseOnPressOutside
        onOpened: mainObj.logsPathToClipBoard()
        enter: Transition {
            NumberAnimation {
                property: "opacity";
                from: 0.0; to: 1.0;
                duration: 500;
            }
        }
        exit: Transition {
            NumberAnimation {
                property: "opacity";
                from: 1.0; to: 0.0;
                duration: 500;
            }
        }
        Text{ text: qsTr("Logs path copied to clipboard");}
    }

    MenuBar{
        id: menu

        Menu{
            title: qsTr("File")
            MenuItem{
                text: qsTr("Logs path")
                shortcut: "Ctrl+L"
                onTriggered: popup.open()
            }
            MenuItem{
                text: qsTr("Clear invalid logs")
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
            MenuItem{
                text: qsTr("About Qt")
                onTriggered: mainObj.aboutQt()
            }
        }
    }

    MessageDialog{
        id: about
        title: qsTr("About")
        icon: StandardIcon.Information
        Component.onCompleted: text = mainObj.mission();
    }

    menuBar: menu
}
