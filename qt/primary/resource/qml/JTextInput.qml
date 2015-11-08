import QtQuick 2.0

Rectangle {
    id: root

    property alias text: text1.text

    color: Qt.darker('steelblue', 2.0)
    radius: 2
    border.width: 1
    border.color: Qt.darker('steelblue', 2.5)
    width: 80
    height: 25
    TextInput {
        id: text1
        anchors {
            left: parent.left; leftMargin: 4
            right: parent.right; rightMargin: 4
            verticalCenter: parent.verticalCenter
        }
        color: "#5fd43b"
        clip: true
    }
}
