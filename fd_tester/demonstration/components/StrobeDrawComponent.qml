import QtQuick 2.7

Item {
    id: drawer
    smooth: true
    property int contentWidth
    property int contentHeight

    property bool shifted: false;

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

    function checkSelection(){
        // check if our strobe is beyond the limits
        var sin = Math.sin(rot.angle*Math.PI/180);
        var cos = Math.cos(rot.angle*Math.PI/180);
        // left-top
        var x0 = selection.x
        var y0 = selection.y
        // right-top
        var x1 = x0 + selection.width*cos;
        var y1 = y0 + selection.width*sin;
        // left-bottom
        var x2 = x0 - selection.height*sin;
        var y2 = y0 + selection.height*cos;
        // right-bottom
        var x3 = x1 + x2 - x0;
        var y3 = y1 + y2 - y0;

        return (x0 > 0 && x0 < drawer.width && y0 > 0 && y0 < drawer.height &&
                x1 > 0 && x1 < drawer.width && y1 > 0 && y1 < drawer.height &&
                x2 > 0 && x2 < drawer.width && y2 > 0 && y2 < drawer.height &&
                x3 > 0 && x3 < drawer.width && y3 > 0 && y3 < drawer.height &&
                selection.width > 0 && selection.height > 0);
    }

    function keyReact(key, abort){
        var diff = (abort) ? -1 : 1;

        if(key == Qt.Key_Shift)
            shifted = (abort) ? false : true;

        if(shifted){
            switch(key){
            case Qt.Key_R:
                rot.angle -= diff;
                break;
            case Qt.Key_Left:
                selection.width -= diff;
                break;
            case Qt.Key_Right:
                selection.width += diff;
                break;
            case Qt.Key_Up:
                selection.height += diff;
                break;
            case Qt.Key_Down:
                selection.height -= diff;
                break;
            }
        }
        else{
            switch(key){
            case Qt.Key_R:
                rot.angle += diff;
                break;
            case Qt.Key_Left:
                selection.x -= diff;
                break;
            case Qt.Key_Right:
                selection.x += diff;
                break;
            case Qt.Key_Up:
                selection.y -= diff;
                break;
            case Qt.Key_Down:
                selection.y += diff;
                break;
            }
        }

        if (!checkSelection()){
           keyReact(key, true)
        }
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
            rot.angle = 0
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
