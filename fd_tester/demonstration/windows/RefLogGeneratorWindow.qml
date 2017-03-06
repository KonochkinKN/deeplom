import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import "../views"

Window{
    id: window
    width: 1200
    height: 800
    color: "#95500C"
    title: "Reference logs"
    modality: Qt.ApplicationModal

    onVisibleChanged: {
        if (!visible)
            window.destroy(1)
    }

    RefLogGeneratorView{
        id: genView
        anchors.fill: parent
    }
}
