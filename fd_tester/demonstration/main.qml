import QtQuick 2.7
import QtMultimedia 5.5
import QtQuick.Controls 1.4
import QtQuick.Controls 2.1 as NewControls
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import awesome.app.fdt 1.0

import "windows"
import "components"

ApplicationWindow{
    id: mainView
    objectName: "mainView"
    title: qsTr("Algorithm tester")
    minimumHeight: 400
    minimumWidth: 400
    visible: true

//    Component.onCompleted: showFullScreen()

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
        onOpened: isLog ? QmlManager.logsPathToClipBoard()
                        : QmlManager.dataPathToClipBoard()

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
        Text{ text: qsTr("%1 path copied to clipboard").
            arg(popup.isLog ? qsTr("Logs") : qsTr("Data"));}

        property bool isLog: true;
    }

    MenuComponent{
        id: menu;
        onLogs: { popup.isLog = true; popup.open();}
        onData: { popup.isLog = false; popup.open();}
        onAbout: about.open()
        onQuit: Qt.quit()
    }

    menuBar: menu

    MessageDialog{
        id: msg
        title: qsTr("Info")
        icon: StandardIcon.Information
        standardButtons: StandardButton.Ok
    }

    MessageDialog{
        id: about
        title: qsTr("About")
        text: QmlManager.mission()
        icon: StandardIcon.Information
        standardButtons: StandardButton.Ok
    }
}
