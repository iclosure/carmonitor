#include "precomp.h"
#include "curve.h"
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

Curve::Curve(const QString &title) :
    QwtPlotCurve(title),
    q_sheftCount(100)
{
    setRenderHint(QwtPlotItem::RenderAntialiased);
    setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(QColor(64, 64, 64, 200)),
                            QPen(QColor(151,187,205,200), 1.0), QSize(2, 2)));
    //setStyle(QwtPlotCurve::Lines);
    /*setCurveAttribute(QwtPlotCurve::Fitted);
    setPaintAttribute(QwtPlotCurve::ClipPolygons);
    setPaintAttribute(QwtPlotCurve::MinimizeMemory);
    setPaintAttribute(QwtPlotCurve::ImageBuffer);*/
}

void Curve::setSize(size_t size)
{
   q_points.resize(size);
}

void Curve::append(const QPointF &point)
{
    q_points.append(point);
    update();
}

void Curve::append(qreal y)
{
    append(QPointF(q_points.count(), y));
}

void Curve::append(qreal x, qreal y)
{
    append(QPointF(x, y));
}

void Curve::setSamples(const QVector<QPointF> &samples)
{
    q_points.clear();
    q_points = samples;
    update();
}

void Curve::sheft(const QPointF &point)
{
    q_points.append(point);
    if (q_points.count() > q_sheftCount) {
        q_points.removeFirst();
    }
    update();
}

void Curve::sheft(qreal y)
{
    q_points.append(QPoint(q_points.count(), y));
    if (q_points.count() > q_sheftCount) {
        for (int i = 1; i < q_points.count(); i++) {
            q_points[i - 1].setY(q_points[i].y());
        }
        q_points.removeLast();
    }
    update();
}

void Curve::sheft(qreal x, qreal y)
{
    sheft(QPointF(x, y));
    update();
}

void Curve::setLinePen(const QPen &pen)
{
    const_cast<QwtSymbol*>(symbol())->setBrush(QBrush(pen.color()));
    setPen(pen);
}

void Curve::update()
{
    QwtPlotCurve::setSamples(q_points);
}
