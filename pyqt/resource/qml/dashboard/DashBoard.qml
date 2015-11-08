import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.0

Item {
    id: root

    property alias text: name.text

    Rectangle {
        id: dashBoard
        color: '#070707'
        width: parent.height - name.height
        height: parent.height - name.height
        radius: width/2

        border.width: 2
        border.color: Qt.darker(color, 1.5)
        clip: true
        RadialGradient {
            clip: true
            //anchors.fill: parent
            width: parent.width
            height: parent.height
            horizontalRadius: parent.width/2
            verticalRadius: parent.width/2
            gradient: Gradient {
                GradientStop { position: 0.0; color: '#F0F0F0' }
                //GradientStop { position: 0.9; color: '#505050' }
                GradientStop { position: 1.0; color: '#070707' }
            }
        }
    }

    Text {
        id: name
        anchors {
            top: dashBoard.bottom; topMargin: 2
            horizontalCenter: dashBoard.horizontalCenter
        }
        font.pixelSize: 16
        color: "#5fd43b"
        font { family: "微软雅黑" }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        smooth: true
    }
}
