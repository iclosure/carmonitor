#ifndef SLIDER_WIDGET_H
#define SLIDER_WIDGET_H

#include <QDialog>

class QDoubleSpinBox;

///! # SliderWidget #

class SliderWidget : public QDialog
{
    Q_OBJECT
public:
    explicit SliderWidget(const QString &title, QWidget *parent = 0);
    ~SliderWidget();

    void setRange(double min, double max);
    void setDecimals(double prec);
    void setSingleStep(double val);
    void setPrefix(const QString &prefix);
    void setSuffix(const QString &suffix);

signals:
    void valueChanged(double value);

public slots:
    void setValue(double value);

private slots:
    void stylize();

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool q_mousePressed;
    QPoint q_orgPos;
    QDoubleSpinBox *q_doubleSpinBox;
};

#endif // SLIDER_WIDGET_H
