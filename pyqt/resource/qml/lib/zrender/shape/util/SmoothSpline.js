/**
 * Catmull-Rom spline
 * @module zrender/shape/util/smoothSpline
 * @author pissang (https://www.github.com/pissang) 
 *         Kener (@Kener, linzhifeng@baidu.com)
 *         errorrik (errorrik@gmail.com)
 */

function SmoothSpline(context) {

    //this.context = context;

    /**
     * @inner
     */
    this.interpolate = function(p0, p1, p2, p3, t) {
        var v0 = (p2 - p0) * 0.5;
        var v1 = (p3 - p1) * 0.5;
        var t2 = t * t;
        var t3 = t * t2;
        return (2 * (p1 - p2) + v0 + v1) * t3
                - (3 * (p1 - p2) + 2 * v0 + v1) * t2
                + v0 * t + p1;
    };

    /**
     * calculate distance between p1 and p2
     * @param {point1} p1
     * @param {point2} p2
     * @return {number}
     */
    this.distance = function(p1, p2) {
        return Math.sqrt(
            (p1[0] - p2[0]) * (p1[0] - p2[0])
            + (p1[1] - p2[1]) * (p1[1] - p2[1])
        );
    };

    /**
     * @alias module:zrender/shape/util/smoothSpline
     * @param {Array} points
     * @param {boolean} isLoop
     * @param {Array} constraint 
     * @return {Array}
     */
    this.splineCurve = function (points, constraint) {
        if (points.length < 2) {
            return points;
        }

        var len = points.length;
        var i, result = [];

        var distance = 0;
        for (i = 1; i < len; i++) {
            distance += this.distance(points[i - 1], points[i]);
        }
            
        var segs = distance / 5;
        segs = Math.max(segs, len)
        for (i = 0; i < segs; i++) {
            var pos = i / (segs - 1) * (len - 1);
            var idx = Math.floor(pos);
            var t = pos - idx;
            var p0 = points[idx === 0 ? idx : idx - 1];
            var p1 = points[idx % len];
            var p2 = points[Math.min(idx + 1, len - 1)];
            var p3 = points[Math.min(idx + 2, len - 1)];

            result.push([
                this.interpolate(p0[0], p1[0], p2[0], p3[0], t),
                this.interpolate(p0[1], p1[1], p2[1], p3[1], t)
            ]);
        }

        return result;
    };

    return this;
}
