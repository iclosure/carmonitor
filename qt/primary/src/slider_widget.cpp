#include "precomp.h"
#include "slider_widget.h"

///! # JButtonClose #

class JButtonClose : public QPushButton
{
public:
    explicit JButtonClose(QWidget *parent = 0)
        : QPushButton(parent) { }
};

///! # SliderWidget #

SliderWidget::SliderWidget(const QString &title, QWidget *parent) :
    QDialog(parent, QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7 ?
                Qt::CustomizeWindowHint : Qt::FramelessWindowHint),
    q_mousePressed(false)
{
    setObjectName("SliderWidget");
    resize(500, 150);

    //
    stylize();

    /// main layout

    QLabel* labelTitle = new QLabel(title, this);
    JButtonClose* buttonClose = new JButtonClose(this);
    buttonClose->setObjectName("buttonClose");
    buttonClose->setToolTip(tr("Close"));

    QHBoxLayout* horiLayoutTitle = new QHBoxLayout();
    horiLayoutTitle->setContentsMargins(6, 0, 6, 6);
    horiLayoutTitle->addWidget(labelTitle, 0, Qt::AlignTop);
    horiLayoutTitle->addStretch();
    horiLayoutTitle->addWidget(buttonClose, 0, Qt::AlignTop);

    q_doubleSpinBox = new QDoubleSpinBox(this);
    q_doubleSpinBox->setObjectName("doubleSpinBox");
    q_doubleSpinBox->setMinimumWidth(200);
    q_doubleSpinBox->setRange(0, 6000);
    q_doubleSpinBox->setDecimals(2);
    q_doubleSpinBox->setSingleStep(0.01);

    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setObjectName("slider");
    slider->setRange((int)q_doubleSpinBox->minimum(), (int)q_doubleSpinBox->maximum());

    QVBoxLayout* vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->addLayout(horiLayoutTitle);
    vertLayoutMain->addWidget(q_doubleSpinBox, 0, Qt::AlignHCenter);
    vertLayoutMain->addSpacing(5);
    vertLayoutMain->addWidget(slider);

    /// signal-slot

    connect(slider, &QSlider::rangeChanged, this, [=](int min, int max) {
        q_doubleSpinBox->setRange(min, max);
    });

    connect(q_doubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, [=](double val){
        slider->setValue((int)val);
        emit valueChanged(val);
    });
    connect(slider, &QSlider::valueChanged, this, [=](int val){
        double v = q_doubleSpinBox->value();
        q_doubleSpinBox->setValue(val + (v - (int)v));
    });
    connect(buttonClose, &QPushButton::clicked, this, &QWidget::close);

    /// finallt initialize
}

SliderWidget::~SliderWidget()
{

}

void SliderWidget::setValue(double value)
{
    q_doubleSpinBox->setValue(value);
}

void SliderWidget::setRange(double min, double max)
{
    QSlider *slider = findChild<QSlider*>("slider");
    if (slider) {
        slider->setRange(min, max);
    }
}

void SliderWidget::setDecimals(double prec)
{
    QDoubleSpinBox *doubleSpinBox = findChild<QDoubleSpinBox*>("doubleSpinBox");
    if (doubleSpinBox) {
        doubleSpinBox->setDecimals(prec);
    }
}

void SliderWidget::setSingleStep(double val)
{
    QDoubleSpinBox *doubleSpinBox = findChild<QDoubleSpinBox*>("doubleSpinBox");
    if (doubleSpinBox) {
        doubleSpinBox->setSingleStep(val);
    }
}

void SliderWidget::setPrefix(const QString &prefix)
{
    QDoubleSpinBox *doubleSpinBox = findChild<QDoubleSpinBox*>("doubleSpinBox");
    if (doubleSpinBox) {
        doubleSpinBox->setPrefix(prefix);
    }
}

void SliderWidget::setSuffix(const QString &suffix)
{
    QDoubleSpinBox *doubleSpinBox = findChild<QDoubleSpinBox*>("doubleSpinBox");
    if (doubleSpinBox) {
        doubleSpinBox->setSuffix(suffix);
    }
}

void SliderWidget::stylize()
{
    switch (QSysInfo::windowsVersion()) {
    case QSysInfo::WV_WINDOWS7:
        //setWindowFlags(Qt::CustomizeWindowHint);
        if (QtWin::isCompositionEnabled()) {
            QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
            setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_NoSystemBackground, false);
            setStyleSheet("SliderWidget { background: transparent; }");
        } else {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
            setStyleSheet(QString("SliderWidget { background: %1; }").arg(QtWin::realColorizationColor().name()));
        }
        break;
    default:
        setStyleSheet("QWidget#SliderWidget{"
                      " border: 1px solid rgb(100, 100, 64);"
                      " border-radius: 6px;"
                      " background-color: rgb(32, 32, 32);"
                      "}");
        break;
    }
}

void SliderWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        q_mousePressed = true;
        q_orgPos = e->pos();
        setCursor(Qt::ClosedHandCursor);
    }

    QDialog::mousePressEvent(e);
}

void SliderWidget::mouseReleaseEvent(QMouseEvent *e)
{
    q_mousePressed = false;
    setCursor(Qt::ArrowCursor);

    QDialog::mouseMoveEvent(e);
}

void SliderWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (q_mousePressed) {
        QPoint curPos = e->pos();
        QRect curGeom = geometry();
        move(curGeom.topLeft() + curPos - q_orgPos);
    }

    QDialog::mouseMoveEvent(e);
}
