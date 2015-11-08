import QtQuick 2.0

Canvas {
    id: canvas
    width: 48
    height: 48
    onPaint: {
        var context = getContext('2d')
        context.beginPath()
        context.fillStyle = Qt.lighter('#101010', 2.0)
        context.arc(width, 0, width, 90 * Math.PI/180, 180 * Math.PI/180);
        context.lineTo(width, 0)
        context.fill()
    }
    Image {
        id: image
        source: 'qrc:/carmonitor/image/close-normal-2.png'
        width: 25
        height: 25
        anchors {
            right: parent.right; rightMargin: 6
            top: parent.top; topMargin: 6
        }

        fillMode: Image.Stretch
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: Qt.quit()
    }
}
