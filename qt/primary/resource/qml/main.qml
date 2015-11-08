import QtQuick 2.0
import QtQuick.Controls 1.2
import 'dial'
import 'dashboard'

Rectangle {
    id: root

    color: '#070707'

    Item {
        id: areaTop
        anchors {
            left: parent.left; top: parent.top; right: parent.right
            margins: 5
        }
        height: 110
        DashBoard {
            id: lcDashBoard  // left-command dashboard
            anchors {
                left: parent.left; leftMargin: 230
                verticalCenter: parent.verticalCenter
            }
            width: parent.height
            height: parent.height
            text: '左指令旋钮'
        }

        Slider {
            anchors {
                horizontalCenter: areaTop.horizontalCenter
                verticalCenter: areaTop.verticalCenter
            }
            minimumValue: 0
            maximumValue: 100
            orientation: Qt.Horizontal
            stepSize: 2
        }

        DashBoard {
            id: rcDashBoard  // right-command dashboard
            anchors {
                right: parent.right; rightMargin: 230
                verticalCenter: parent.verticalCenter
            }
            width: parent.height
            height: parent.height
            text: '右指令旋钮'
        }
    }
    CurvePane {
        id: areaCenter
        anchors {
            left: parent.left; right: parent.right
            top: areaTop.bottom; bottom: areaBottom.top
            margins: 5
        }
    }
    Item {
        id: areaBottom
        anchors {
            left: parent.left; bottom: parent.bottom; right: parent.right
            leftMargin: 150; rightMargin: 150
        }
        height: 250
        Column {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            spacing: 5
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 20
                DashBoard {
                    id: dashBoardL11  //
                    width: 110
                    height: 110
                    text: '电源开关'
                }
                DashBoard {
                    id: dashBoardL12  //
                    width: 110
                    height: 110
                    text: '调速增益'
                }
            }
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                DashBoard {
                    id: dashBoardL21  //
                    width: 130
                    height: 130
                    text: '调速旋钮'
                }
                DashBoard {
                    id: dashBoardL22  //
                    width: 130
                    height: 130
                    text: '打滑调节'
                }
            }
        }
        Column {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            spacing: 5
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 20
                DashBoard {
                    id: dashBoardR11  //
                    width: 110
                    height: 110
                    text: '电源开关'
                }
                DashBoard {
                    id: dashBoardR12  //
                    width: 110
                    height: 110
                    text: '调速增益'
                }
            }
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                DashBoard {
                    id: dashBoardR21  //
                    width: 130
                    height: 130
                    text: '调速旋钮'
                }
                DashBoard {
                    id: dashBoardR22  //
                    width: 130
                    height: 130
                    text: '打滑调节'
                }
            }
        }
    }

    ButtonQuit {
        id: buttonQuit
        anchors {
            top: parent.top
            right: parent.right
        }
    }
}
