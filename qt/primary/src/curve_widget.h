#ifndef CURVE_WIDGET_H
#define CURVE_WIDGET_H

#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_picker.h>
#include <QDateTime>

///! # class TimeScaleDraw #

class TimeScaleDraw : public QwtScaleDraw
{
public:
    TimeScaleDraw();

public:
    void setFormat(const QString &format);

protected:
    virtual QwtText label(double) const;

private:
    QString q_format;
};

///! # class TimePlotPicker #

class TimePlotPicker : public QwtPlotPicker
{
public:
    explicit TimePlotPicker(QWidget *canvas);
    explicit TimePlotPicker(int xAxis, int yAxis, QWidget *canvas);
    explicit TimePlotPicker(int xAxis, int yAxis,
        RubberBand rubberBand, DisplayMode trackerMode, QWidget *canvas);

protected:
    virtual QwtText trackerTextF(const QPointF &point) const;

private:
};

///! # class CurveWidget #

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;
class QwtPlotZoomer;
class Curve;

class CurveWidget : public QwtPlot
{
    Q_OBJECT
public:
    explicit CurveWidget(QWidget *parent = 0);
    explicit CurveWidget( const QwtText &title, QWidget *parent = 0, bool zoom = false);
    ~CurveWidget();

    void addCurve(const QString &title, const QPen &pen,
                  const QVector<QPointF> &samples = QVector<QPointF>());
    void removeCurve(Curve *curve);
    void autoScale();
    void clear();

    void setTitle(const QString &text);
    bool setTitle(int index, const QString &text);
    Curve *curve(int index) const;
    QVector<Curve*> &curves() { return q_curves; }

    void setScaleLabelFormat(const QString &format);

    bool enableDBClick() const;
    void setEnableDBClick(bool enable);

    void setNormalScale();

signals:
    void doubleClicked(bool checked);

public slots:

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    void initialize(bool zoom);

private:
    bool q_enableDBClick;
    bool q_showNormal;
    int q_width;
    QwtPlotMarker *q_origin;
    QVector<Curve*> q_curves;
    QwtPlotZoomer* q_zoomer;
};

#endif // CURVE_WIDGET_H
