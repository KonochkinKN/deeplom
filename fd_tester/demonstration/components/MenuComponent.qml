import QtQuick 2.7
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import awesome.app.fdt 1.0

MenuBar{
    id: menu

    signal logs()
    signal data()
    signal quit()
    signal about()

    Menu{
        title: qsTr("File")
        MenuItem{
            text: qsTr("Logs path")
            shortcut: "Ctrl+L"
            onTriggered: menu.logs()
        }
        MenuItem{
            text: qsTr("Data path")
            shortcut: "Ctrl+D"
            onTriggered: menu.data()
        }
        MenuItem{
            text: qsTr("Quit")
            shortcut: "Ctrl+Q"
            onTriggered: menu.quit()
        }
    }

    Menu{
        title: qsTr("Clean")
        MenuItem{
            text: qsTr("Clear logs")
            shortcut: "Ctrl+C"
            onTriggered: {
                msg.text = QmlManager.cleanLogs();
                msg.open();
            }
        }
        MenuItem{
            text: qsTr("Clear invalid logs")
            shortcut: "Ctrl+I"
            onTriggered: {
                msg.text = QmlManager.cleanInvalidLogs();
                msg.open();
            }
        }
        MenuItem{
            text: qsTr("Clean data")
            shortcut: "Ctrl+R"
            onTriggered: {
                msg.text = QmlManager.cleanAnalyzerData()
                msg.open()
            }
        }
    }

    Menu{
        title: qsTr("Help")
        MenuItem{
            text: qsTr("About")
            onTriggered: menu.about()
        }
        MenuItem{
            text: qsTr("About Qt")
            onTriggered: QmlManager.aboutQt()
        }
    }
}
