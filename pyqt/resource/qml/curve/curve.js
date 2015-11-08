Qt.include('qrc:/carmonitor/qml/lib/zrender/SmoothSpline.js')

function drawSpline(context, data, constraint) {
    //
    var points = []
    var i, minY = 0, maxY = 0;
    data.forEach(function(y){ minY = Math.min(minY, y); maxY = Math.max(maxY, y); })
    var deltaY = Math.max(1, maxY - minY)
    for (i = 0; i < data.length; i++) {
        points.push([constraint[0][0] + i * (constraint[1][0] / (data.length - 1)),
                    (constraint[1][1] - data[i] * constraint[1][1] / deltaY)])
    }

    //
    var spline = new SmoothSpline(context)
    var result = spline.splineCurve(points, constraint)

    context.beginPath()
    context.moveTo(result[0][0], result[0][1]);
    for (i = 1; i < result.length; i++) {
        context.lineTo(result[i][0], result[i][1]);
    }
    context.stroke()

    //
    context.beginPath()
    for (i = 0; i < points.length; i++) {
        context.moveTo(points[i][0] + 3, points[i][1])
        context.arc(points[i][0], points[i][1], 3, 0, Math.PI * 2)
    }
    context.stroke()
    context.fill()
}
