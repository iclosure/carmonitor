#ifndef CURVE_H
#define CURVE_H

#include <qwt_plot_curve.h>

class Curve : public QwtPlotCurve
{
public:
    explicit Curve(const QString &title);

    void setSize(size_t size);
    size_t size() { return q_points.size(); }
    void append(const QPointF &point);
    void append(qreal y);
    void append(qreal x, qreal y);
    void setSamples(const QVector<QPointF> &samples);
    void shift(const QPointF &point);
    void shift(qreal y);
    void shift(qreal x, qreal y);
    int count() const { return q_points.count(); }
    QVector<QPointF> &samples() { return q_points; }

    int sheftCount() const { return q_sheftCount; }
    void setSheftCount(int value) { q_sheftCount = value; }

    void setLinePen(const QPen& pen);

private:
    void update();

private:
    int q_sheftCount;
    QVector<QPointF>    q_points;
};

#endif // CURVE_H
