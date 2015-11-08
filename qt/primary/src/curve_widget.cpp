#include "precomp.h"
#include "curve_widget.h"
#include "qwt_plot_canvas.h"
#include "qwt_painter.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_legenditem.h>
#include <qwt_picker_machine.h>
#include <qwt_text_label.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_abstract_legend.h>
#include <qwt_plot_zoomer.h>
#include "curve.h"
#include "main_widget.h"

///! # class TimeScaleDraw #

TimeScaleDraw::TimeScaleDraw()
    : q_format("HH:mm:ss")
{
    //setLabelRotation(-10);
}

void TimeScaleDraw::setFormat(const QString &format)
{
    q_format = format;
}

QwtText TimeScaleDraw::label(double v) const
{
    return QwtText(QDateTime::fromMSecsSinceEpoch((quint64)v).toString(q_format));
}

///! # class TimePlotPicker #

TimePlotPicker::TimePlotPicker(QWidget *canvas)
    : QwtPlotPicker(canvas)
{

}

TimePlotPicker::TimePlotPicker(int xAxis, int yAxis, QWidget *canvas)
    : QwtPlotPicker(xAxis, yAxis, canvas)
{

}

TimePlotPicker::TimePlotPicker(int xAxis, int yAxis,
                               QwtPicker::RubberBand rubberBand,
                               QwtPicker::DisplayMode trackerMode, QWidget *canvas)
    : QwtPlotPicker(xAxis, yAxis, rubberBand, trackerMode, canvas)
{

}

QwtText TimePlotPicker::trackerTextF(const QPointF &point) const
{
    switch (rubberBand())
    {
    case CrossRubberBand:
        return QString("(%1, %2)")
                .arg(QDateTime::fromMSecsSinceEpoch((quint64)point.x()).toString("yyyy/M/d HH:mm:ss"))
                .arg(point.y());
    default:
        break;
    }

    return QwtText();
}

///! # class JLegendItem #

class JLegendItem: public QwtPlotLegendItem
{
public:
    JLegendItem()
    {
        setRenderHint(QwtPlotItem::RenderAntialiased);
        setAlignment(Qt::AlignTop | Qt::AlignRight);

        setTextPen(QColor(200, 200, 200, 150));
        setBorderPen(QColor(150, 150, 150, 50));
        setBackgroundBrush(QColor(150, 150, 150, 20));
        setBorderRadius(3);
        setMargin(2);
        setSpacing(10);
        setItemMargin(0);

        QFont _font = font();
        _font.setPointSize(8);
        setFont(_font);
    }
};

///
/// \brief The Zoomer class
///
class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer( int xAxis, int yAxis, QWidget *canvas ):
        QwtPlotZoomer( xAxis, yAxis, canvas )
    {
        setTrackerMode( QwtPicker::AlwaysOff );
        setRubberBand( QwtPicker::NoRubberBand );

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

        setMousePattern( QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier );
        setMousePattern( QwtEventPattern::MouseSelect3,
            Qt::RightButton );
    }

    virtual QwtText trackerTextF(const QPointF & point) const
    {
        switch (rubberBand())
        {
        case RectRubberBand:
            return QString("(%1, %2)")
                    .arg(QDateTime::fromMSecsSinceEpoch((quint64)point.x()).toString("yyyy/M/d HH:mm:ss"))
                    .arg(point.y());
        default:
            break;
        }

        return QwtText();
    }
};

///

CurveWidget::CurveWidget(QWidget *parent) :
    QwtPlot(parent),
    q_enableDBClick(false),
    q_showNormal(true),
    q_zoomer(0)
{
    setObjectName("CurveWidget");
    initialize(false);
}

CurveWidget::CurveWidget(const QwtText &title, QWidget *parent, bool zoom) :
    QwtPlot(title, parent),
    q_enableDBClick(false),
    q_showNormal(true),
    q_zoomer(0)
{
    setObjectName("CurveWidget");
    initialize(zoom);
}

CurveWidget::~CurveWidget()
{
}

void CurveWidget::initialize(bool zoom)
{
    setContentsMargins(2, 2, 2, 2);
    setMinimumHeight(338);

    int _width = QApplication::desktop()->availableGeometry().width();
    switch (_width) {
    case 1280: q_width = 493; break;
    case 1366: q_width = 532; break;
    default: q_width = 493; break;
    }

    setFixedWidth(q_width);
    plotLayout()->setAlignCanvasToScales( true);
    setAutoReplot();
    //setAxisAutoScale(QwtPlot::yLeft, false);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(QColor(80, 80, 80, 100), 0.5, Qt::SolidLine);
    grid->enableX(true);
    grid->enableY(true);
    grid->attach(this);

    q_origin = new QwtPlotMarker();
    q_origin->setLineStyle( QwtPlotMarker::Cross);
    q_origin->setLinePen(QColor(100, 100, 100, 100), 1.0, Qt::DashLine );
    q_origin->attach(this);

    setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());

    TimePlotPicker* picker = new TimePlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas());
    picker->setStateMachine(new QwtPickerDragPointMachine());
    picker->setRubberBandPen(QColor(100, 100, 100, 200));
    picker->setTrackerPen(QColor(128, 128, 200, 200));

    // zoomer
    if (zoom) {
        q_zoomer = new Zoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas());
        q_zoomer->setRubberBand(QwtPicker::RectRubberBand);
        q_zoomer->setRubberBandPen(QColor(Qt::green));
        q_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
        q_zoomer->setTrackerPen(QColor(Qt::white));
    }

    // scale
    //setAxisScale( QwtPlot::xBottom, 0, 200 );
    //setAxisScale( QwtPlot::yLeft, 400, 800 );

    //! curve

    //srand(QDateTime::currentDateTime().fromMSecsSinceEpoch());

    //
    QVector<QPointF> samples(0);
    addCurve("Curve 1", QPen(QColor("#0c78a6"), 1), samples);
    addCurve("Curve 2", QPen(QColor("#5fd43b"), 1), samples);

    // Legend
    QwtPlotLegendItem* legendItem = new JLegendItem;
    legendItem->setMaxColumns(q_curves.count());
    legendItem->attach(this);
}

void CurveWidget::addCurve(const QString &title, const QPen &pen, const QVector<QPointF> &samples)
{
    Curve* curve = new Curve(title);
    curve->setLinePen(pen);
    curve->attach(this);
    curve->setSamples(samples);
    curve->setSheftCount(100);
    autoScale();

    q_curves.push_back(curve);
}

void CurveWidget::removeCurve(Curve *curve)
{
    if (q_curves.contains(curve)) {
        for (int i = 0; i < q_curves.count(); ++i) {
            if (q_curves[i] == curve) {
                q_curves[i]->detach();
                delete q_curves[i];
                q_curves.removeAt(i);
                break;
            }
        }
    }
}

void CurveWidget::autoScale()
{
    setAxisAutoScale(QwtPlot::xBottom);
    setAxisAutoScale(QwtPlot::yLeft);
}

void CurveWidget::clear()
{
    Curve* curve;
    foreach (curve, q_curves) {
        curve->setSamples(QVector<QPointF>());
        curve->detach();
        delete curve;
    }

    q_curves.clear();
}

void CurveWidget::setTitle(const QString &text)
{
    QwtPlot::setTitle(text);
}

bool CurveWidget::setTitle(int index, const QString &text)
{
    if (index < 0 || index >= q_curves.count()) {
        return false;
    }

    q_curves[index]->setTitle(text);

    return true;
}

Curve *CurveWidget::curve(int index) const
{
    if (index < 0 || index >= q_curves.count()) {
        return 0;
    }

    return q_curves.at(index);
}

void CurveWidget::setScaleLabelFormat(const QString &format)
{
    TimeScaleDraw *timeScaleDraw = reinterpret_cast<TimeScaleDraw*>(axisScaleDraw(QwtPlot::xBottom));
    if (timeScaleDraw) {
        timeScaleDraw->setFormat(format);
    }
}

bool CurveWidget::enableDBClick() const
{
    return q_enableDBClick;
}

void CurveWidget::setEnableDBClick(bool enable)
{
    q_enableDBClick = enable;
}

void CurveWidget::setNormalScale()
{
    if (q_zoomer) {

    }
}

void CurveWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!q_enableDBClick) {
        return;
    }

    QwtPlot::mouseDoubleClickEvent(event);

    q_showNormal = !q_showNormal;
    Q_EMIT doubleClicked(q_showNormal);

    //
    QString format("HH:mm:ss");
    if (q_showNormal) {
        setScaleLabelFormat("HH:mm:ss");
        setFixedWidth(q_width);
    } else {
        setScaleLabelFormat("yyyy/MM/dd\n  HH:mm:ss");
        setMinimumWidth(q_width);
        setMaximumWidth(10000000);
    }
}
