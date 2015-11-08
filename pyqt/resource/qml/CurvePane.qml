import QtQuick 2.0
import 'curve'

Rectangle {
    id: root

    color: '#101010'

    radius: 2
    border.width: 2
    border.color: Qt.darker(color, 1.2)

    Column {
        id: areaLeft
        anchors {
            left: parent.left; right: areaCenter.left
            top: parent.top; bottom: parent.bottom
            margins: 1
        }
        spacing: 1
        Curve {
            id: canvasMainP
            width: parent.width
            height: parent.height/2
        }
        Curve {
            id: canvasMainV
            width: parent.width
            height: parent.height/2
        }
    }
    Item {
        id: areaCenter
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top; bottom: parent.bottom
            margins: 1
        }
        width: 300
        clip: true
        Column {
            anchors {
                centerIn: parent
                leftMargin: 5; rightMargin: 5
            }
            spacing: height/7
            Row {
                JTextInput {
                    id: text11
                    text: '23.3455'
                    width: 100
                }
                JText {
                    id: name1
                    text: '刹车指令'
                    width: areaCenter.width - 210
                }
                JTextInput {
                    id: text12
                    text: '23.3455'
                    width: 100
                }
            }
            Row {
                JTextInput {
                    id: text21
                    text: '23.3455'
                    width: 100
                }
                JText {
                    id: name2
                    text: '主刹车压力'
                    width: areaCenter.width - 210
                }
                JTextInput {
                    id: text22
                    text: '23.3455'
                    width: 100
                }
            }
            Row {
                JTextInput {
                    id: text31
                    text: '23.3455'
                    width: 100
                }
                JText {
                    id: name3
                    text: '副刹车压力'
                    width: areaCenter.width - 210
                }
                JTextInput {
                    id: text32
                    text: '23.3455'
                    width: 100
                }
            }
            Row {
                JTextInput {
                    id: text41
                    text: '23.3455'
                    width: 100
                }
                JText {
                    id: name4
                    text: '转速显示'
                    width: areaCenter.width - 210
                }
                JTextInput {
                    id: text42
                    text: '23.3455'
                    width: 100
                }
            }
            Row {
                JTextInput {
                    id: text51
                    text: '23.3455'
                    width: 100
                }
                JText {
                    id: name5
                    text: '理论转速'
                    width: areaCenter.width - 210
                }
                JTextInput {
                    id: text52
                    text: '23.3455'
                    width: 100
                }
            }
        }

    }
    Column {
        id: areaRight
        anchors {
            left: areaCenter.right; right: parent.right
            top: parent.top; bottom: parent.bottom
            margins: 1
        }
        spacing: 1
        Curve {
            id: canvasAssisP
            width: parent.width
            height: parent.height/2
        }
        Curve {
            id: canvasAssisV
            width: parent.width
            height: parent.height/2
        }
    }
}
