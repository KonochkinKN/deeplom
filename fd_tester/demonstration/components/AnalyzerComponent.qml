import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import awesome.app.fdt 1.0

ColumnLayout {
    id: mainRow

    property string videoFile
    property string refLog: refLogsList.currentText
    property string testLog: testLogsList.currentText

    MessageDialog{
        id: msg
        title: qsTr("Info")
        standardButtons: StandardButton.Ok
        icon: StandardIcon.Information
    }

    Analyzer{
        id: analyzer;
        refLog: refLogsList.currentText
        logForTest: testLogsList.currentText
        onMessage: {
            msg.text = txt
            msg.open()
        }
    }

    RowLayout{
        id: effRow
        Layout.preferredWidth: parent.width

        Button{
            id: countBtn
            text: qsTr("Count efficiency")
            onClicked: analyzer.analyze()
        }

        ComboBox{
            id: effBox
            model: analyzer.result
            anchors.left: countBtn.right
            anchors.right: parent.right
            anchors.leftMargin: 5
            onCurrentIndexChanged: currentIndex = 0;
            selectByMouse: false
        }
    }

    RowLayout{
        id: listRow
        Layout.preferredWidth: parent.width
        Layout.preferredHeight: parent.height - 2*effRow.height

        CustomListView{
            id: refLogsList
            title: qsTr("Reference logs")
            listModel: QmlManager.refLogsByVideo(videoFile)
            Layout.preferredHeight: parent.height*0.99
            Layout.preferredWidth: parent.width*0.49
        }

        CustomListView{
            id: testLogsList
            title: qsTr("Algorithms logs")
            listModel: QmlManager.testLogsByVideo(videoFile)
            Layout.preferredHeight: parent.height*0.99
            Layout.preferredWidth: parent.width*0.49
        }
    }
}
