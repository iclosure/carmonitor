import QtQuick 2.0
import QtWebKit 3.0
import 'curve.js' as CurveJS
/*
WebView {
    id: webview
    objectName: 'curve'

    url: 'curve.html'
    onNavigationRequested: {
        // detect URL scheme prefix, most likely an external link
        var schemaRE = /^\w+:/;
        if (schemaRE.test(request.url)) {
            request.action = WebView.AcceptRequest;
        } else {
            request.action = WebView.IgnoreRequest;
            // delegate request.url here
        }
    }
}
*/

Rectangle {
    id: root

    color: '#202020'
    radius: 2

    Canvas {
        id: canvas

        anchors.fill: parent
        anchors.margins: 1

        property var curves: [
            [1],
        ]

        onPaint: {
            var context = getContext('2d')
            context.lineWidth = 1
            context.strokeStyle = '#5fd43b'
            context.fillStyle = 'green'
            context.clearRect(region.x, region.y, region.width, region.height)

            for (var i in curves) {
                CurveJS.drawSpline(context, curves[i], [[region.x, region.y], [region.width, region.height]])
            }
        }

        Timer {
            interval: 200; running: true; repeat: true
            onTriggered: {
                for (var i in parent.curves) {
                    if (parent.curves[i].length > 20) {
                        parent.curves[i].shift()
                    }
                    parent.curves[i].push(Math.random() % 100)
                }
                parent.requestPaint()
            }
        }
    }
}
