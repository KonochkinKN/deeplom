import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import awesome.app.fdt 1.0

import "../components"

RowLayout{
    id: mainRow

    signal close();

    QmlManager{ id: man;}

    CustomListView{
        id: list
        title: "test"
        listModel: {1; 2; 3; 4}
        Layout.preferredWidth: 200
        Layout.preferredHeight: parent.height*0.8
    }

    Button{
        id: closeBut
        text: qsTr("Close")
        Layout.alignment: Qt.AlignHCenter
        onClicked: mainRow.close()
    }
}
