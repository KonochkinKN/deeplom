import QtQuick 2.7

Item {
    id: drawer
    smooth: true
    property int contentWidth
    property int contentHeight

    signal rectChanged(rect videoRect)

    function updateStrobe(rect){
        d.screenRectangle = d.videoToScreen(rect)
        d.setSelection()
    }

    function getStrobe(){
        return Qt.rect(selection.x, selection.y,
                       selection.width, selection.height)
    }

    function getRealStrobe(){
        return d.screenToVideo(selection)
    }

    function rotateStrobe(angle){
        rot.angle += angle;
    }

    QtObject{
        id: d
        property rect screenRectangle
        property rect videoRectangle

        function setSelection(){
            selection.x = screenRectangle.x
            selection.y = screenRectangle.y
            selection.width = screenRectangle.width
            selection.height = screenRectangle.height
        }

        function videoToScreen(videoRect){
            var newRect = Qt.rect(0,0,0,0);
            var wRate = width / contentWidth
            var hRate = height / contentHeight

            newRect.x = videoRect.x * wRate
            newRect.y = videoRect.y * hRate
            newRect.width = videoRect.width * wRate
            newRect.height = videoRect.height * hRate

            return newRect
        }

        function screenToVideo(screenRect){
            var newRect = Qt.rect(0,0,0,0);
            var wRate =  contentWidth / width
            var hRate =  contentHeight / height

            newRect.x = screenRect.x * wRate
            newRect.y = screenRect.y * hRate
            newRect.width = screenRect.width * wRate
            newRect.height = screenRect.height * hRate

            return newRect
        }
    }

    MouseArea{
        z: parent.z + 1
        anchors.fill: parent
        enabled: true
        preventStealing: true

        property int xClicked
        property int yClicked

        onPositionChanged: {

            var x = mouse.x;
            var y = mouse.y;

            if (x < 0 ) x = 0;
            if (x > width) x = width;
            if (y < 0) y = 0;
            if (y > height) y = height;

            d.screenRectangle.width = Math.abs(xClicked - x)
            d.screenRectangle.height = Math.abs(yClicked - y)

            d.screenRectangle.x = Math.min(x, xClicked)
            d.screenRectangle.y = Math.min(y, yClicked)

            d.setSelection()

        }

        onReleased: {
            d.videoRectangle = d.screenToVideo(d.screenRectangle)
            rectChanged(d.videoRectangle)
        }

        onPressed: {
            xClicked = mouse.x
            yClicked = mouse.y
            d.screenRectangle.x = xClicked
            d.screenRectangle.y = yClicked
            d.screenRectangle.width = 0
            d.screenRectangle.height = 0

            d.setSelection()
        }
    }

    Rotation{ id: rot;}

    Rectangle{
        id: selection

        antialiasing: true
        radius: 5
        color: "#504DD6CD"
        border.color: "red"
        border.width: 2
        transform: rot
    }
}
