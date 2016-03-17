#include "precomp.h"
#include "main_widget.h"
#include "protocol/serialport_proxy.h"
#include "database/databasemgr.h"
#include "curve.h"
#include "curve_widget.h"
#include "slider_widget.h"
#include "settings_widget.h"
#include "history_widget.h"

///! # JSwitchButton #

JSwitchButton::JSwitchButton(QWidget *parent)
    : QPushButton(parent)
    , q_isOn(false)
{
    setObjectName("JSwitchButton");
    setCheckable(true);
    setChecked(false);

    q_pixmap = QPixmap(":/carmonitor/image/button-switch.png");

    setFixedSize(QSize(q_pixmap.width(), q_pixmap.height()/2));

    connect(this, &QPushButton::clicked, this, [=]() {
        setState(!q_isOn);
    });
}

JSwitchButton::JSwitchButton(const QPixmap &pixmap, QWidget *parent)
    : QPushButton(parent)
    , q_isOn(false)
    , q_pixmap(pixmap)
{
    setObjectName("JSwitchButton");
    setCheckable(true);
    setChecked(false);

    setFixedSize(QSize(q_pixmap.width(), q_pixmap.height()/2));

    connect(this, &QPushButton::clicked, this, [=]() {
        setState(!q_isOn);
    });
}

void JSwitchButton::setSize(int w, int h)
{
    resize(w, h);
    repaint();
}

void JSwitchButton::setState(bool state)
{
    if (q_isOn != state) {
        q_isOn = state;
        setChecked(q_isOn);
        repaint();
        emit stateChanged(q_isOn);
    }
}

void JSwitchButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int h = q_pixmap.height()/2;
    painter.drawPixmap(rect(), q_pixmap, QRect(0, h * (q_isOn ? 1 : 0),
                                               q_pixmap.width(),  h));
}

///! # JTracksipButton #

JTracksipButton::JTracksipButton(QWidget *parent)
    : QPushButton(parent)
    , q_isOn(false)
{
    setObjectName("JTracksipButton");
    //setCheckable(true);
    //setChecked(false);

    q_pixmap = QPixmap(":/carmonitor/image/button-switch.png");

    setFixedSize(QSize(q_pixmap.width(), q_pixmap.height()/2));
#if 0
    connect(this, &QPushButton::pressed, this, [=]() {
        setState(true);
    });
    connect(this, &QPushButton::released, this, [=]() {
        setState(false);
    });
#else
    connect(this, &QPushButton::clicked, this, [=]() {
        setState(!q_isOn);
    });
#endif  // 0
}

JTracksipButton::JTracksipButton(const QPixmap &pixmap, QWidget *parent)
    : QPushButton(parent)
    , q_isOn(false)
    , q_pixmap(pixmap)
{
    setObjectName("JTracksipButton");
    //setCheckable(true);
    //setChecked(false);

    setFixedSize(QSize(q_pixmap.width(), q_pixmap.height()/2));

#if 0
    connect(this, &QPushButton::pressed, this, [=]() {
        setState(true);
    });
    connect(this, &QPushButton::released, this, [=]() {
        setState(false);
    });
#else
    connect(this, &QPushButton::clicked, this, [=]() {
        setState(!q_isOn);
    });
#endif  // 0
}

void JTracksipButton::setSize(int w, int h)
{
    resize(w, h);
    repaint();
}

void JTracksipButton::setState(bool state)
{
    if (q_isOn != state) {
        q_isOn = state;
        //setChecked(q_isOn);
        repaint();
        emit stateChanged(q_isOn);
    }
}

void JTracksipButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int h = q_pixmap.height()/2;
    painter.drawPixmap(rect(), q_pixmap, QRect(0, h * (q_isOn ? 1 : 0),
                                               q_pixmap.width(),  h));
}
/*
bool JTracksipButton::event(QEvent *e)
{
    qDebug() << e->type();

    switch (e->type()) {
    case QEvent::TouchBegin:
        setState(true);
        break;
    case QEvent::TouchEnd:
        setState(false);
        break;
    default:
        break;
    }

    return QPushButton::event(e);
}
*/
///! # JButtonDash #

JButtonDash::JButtonDash(QWidget *parent)
    : QPushButton(parent)
{
    setObjectName("JButtonDash");
}

JButtonDash::JButtonDash(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    setObjectName("JButtonDash");
}

///! # MainWidget #

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint/* | Qt::WindowStaysOnTopHint*/)
    , q_lTheorySpd(0)
    , q_rTheorySpd(0)
{
    /// mainwindow properties

    setObjectName("MainWidget");
    setWindowIcon(QIcon(":/image/default/app.icon"));
    setWindowTitle(tr("%1 V%2").arg(qApp->applicationDisplayName()).arg(qApp->applicationVersion()));
    resize(800, 480);

    //setCursor(QCursor(QPixmap(":/carmonitor/image/touchpoint.png")));

    /// mainwindow layout

    //! top

    QGroupBox* groupBoxTop = new QGroupBox(this);
    groupBoxTop->setObjectName("groupBoxTop");

    // command dashboard
    JSwitchButton* buttonLeftPower = new JSwitchButton(groupBoxTop);
    JSwitchButton* buttonRightPower = new JSwitchButton(groupBoxTop);
    QPushButton* buttonSettings = new QPushButton(groupBoxTop);
    QPushButton* buttonHistory = new QPushButton(groupBoxTop);
    QPushButton* buttonQuit = new QPushButton(groupBoxTop);
    buttonLeftPower->setObjectName("buttonLeftPower");
    buttonLeftPower->setObjectName("buttonRightPower");
    buttonSettings->setObjectName("buttonSettings");
    buttonHistory->setObjectName("buttonHistory");
    buttonQuit->setObjectName("buttonQuit");

    QWidget *areaPortState = new QWidget(this);
    //areaPortState->setMaximumHeight(buttonLeftPower->height());
    areaPortState->setObjectName("areaPortState");
    areaPortState->setStyleSheet("QWidget#areaPortState{border-radius:3px;"
                                 "border:1px solid #505050; background:rgba(64,64,64,50);}");
    QVBoxLayout *vertLayoutPortState = new QVBoxLayout(areaPortState);
    vertLayoutPortState->setContentsMargins(50, 2, 50, 2);
    vertLayoutPortState->setSpacing(3);
    JSwitchButton *buttonPortState = new JSwitchButton(QPixmap(":/carmonitor/image/button-port-state.png"), areaPortState);
//    buttonPortState->setFixedSize(100, 40);
    vertLayoutPortState->addWidget(new QLabel(tr("Port State"), areaPortState), 0, Qt::AlignHCenter);
    vertLayoutPortState->addWidget(buttonPortState);

    //
    QHBoxLayout* horiLayoutTop = new QHBoxLayout(groupBoxTop);
    horiLayoutTop->setContentsMargins(0, 0, 0, 0);
    horiLayoutTop->addSpacing(25);
    horiLayoutTop->addWidget(buttonSettings, 0, Qt::AlignLeft);
    horiLayoutTop->addSpacing(20);
    horiLayoutTop->addWidget(buttonHistory, 0, Qt::AlignLeft);
    horiLayoutTop->addSpacing(65);
    horiLayoutTop->addWidget(buttonLeftPower);
    horiLayoutTop->addWidget(new QLabel(tr("Left Power")));
    horiLayoutTop->addStretch();
    horiLayoutTop->addWidget(areaPortState, 0, Qt::AlignTop);
    horiLayoutTop->addStretch();
    horiLayoutTop->addWidget(new QLabel(tr("Right Power")));
    horiLayoutTop->addWidget(buttonRightPower);
    horiLayoutTop->addSpacing(150);
    horiLayoutTop->addWidget(buttonQuit, 0, Qt::AlignRight);
    horiLayoutTop->addSpacing(25);

    //! middle

    // curves
    CurveWidget* curveLBP = new CurveWidget(tr("LBP(MPa)"), this);
    CurveWidget* curveLRP = new CurveWidget(tr("LRR(r/min)"), this);
    CurveWidget* curveRBP = new CurveWidget(tr("RBP(MPa)"), this);
    CurveWidget* curveRRP = new CurveWidget(tr("RRR(r/min)"), this);

    //
    curveLBP->setAxisScale(QwtPlot::yLeft, 0, 30, 5);
    curveRBP->setAxisScale(QwtPlot::yLeft, 0, 30, 5);
    curveLRP->setAxisScale(QwtPlot::yLeft, 0, 3000, 500);
    curveRRP->setAxisScale(QwtPlot::yLeft, 0, 3000, 500);

    //
    curveLBP->setEnableDBClick(true);
    curveLBP->setTitle(0, tr("Major"));
    curveLBP->setTitle(1, tr("Minor"));
    curveLRP->setEnableDBClick(true);
    curveLRP->setTitle(0, tr("Theory"));
    curveLRP->setTitle(1, tr("Real"));
    curveRBP->setEnableDBClick(true);
    curveRBP->setTitle(0, tr("Major"));
    curveRBP->setTitle(1, tr("Minor"));
    curveRRP->setEnableDBClick(true);
    curveRRP->setTitle(0, tr("Theory"));
    curveRRP->setTitle(1, tr("Real"));

    // areaMiddle
    QWidget* areaMiddle = new QWidget(this);
    areaMiddle->setObjectName("areaMiddle");
    areaMiddle->setFixedWidth(280);

    //
    QGroupBox* groupBoxStatus = new QGroupBox(this);
    groupBoxStatus->setObjectName("groupBoxStatus");

    // status view
    QGridLayout* gridLayoutStatus = new QGridLayout(groupBoxStatus);
    gridLayoutStatus->setContentsMargins(5, 5, 5, 5);
    gridLayoutStatus->setHorizontalSpacing(8);
    gridLayoutStatus->setVerticalSpacing(3);

    // Brake Command
    QLineEdit* editLeftBrakeCmd = new QLineEdit(groupBoxStatus);
    QLineEdit* editRightBrakeCmd = new QLineEdit(groupBoxStatus);
    editLeftBrakeCmd->setObjectName("editLeftBrakeCmd");
    editRightBrakeCmd->setObjectName("editRightBrakeCmd");
    editLeftBrakeCmd->setReadOnly(true);
    editRightBrakeCmd->setReadOnly(true);
    gridLayoutStatus->addWidget(new QLabel(tr("Brake Command:"), groupBoxStatus),
                                0, 0, 1, 2, Qt::AlignCenter);
    gridLayoutStatus->addWidget(editLeftBrakeCmd, 1, 0, 1, 1);
    gridLayoutStatus->addWidget(editRightBrakeCmd, 1, 1, 1, 1);

    // Major Brake Pressure
    QLineEdit* editMLeftBrakeP = new QLineEdit(groupBoxStatus);
    QLineEdit* editMRightBrakeP = new QLineEdit(groupBoxStatus);
    editMLeftBrakeP->setObjectName("editMLeftBrakeP");
    editMRightBrakeP->setObjectName("editMRightBrakeP");
    editMLeftBrakeP->setReadOnly(true);
    editMRightBrakeP->setReadOnly(true);
    gridLayoutStatus->addWidget(new QLabel(tr("Major Brake Pressure:"), groupBoxStatus),
                                2, 0, 1, 2, Qt::AlignCenter);
    gridLayoutStatus->addWidget(editMLeftBrakeP, 3, 0, 1, 1);
    gridLayoutStatus->addWidget(editMRightBrakeP, 3, 1, 1, 1);

    // Assistant Brake Pressure
    QLineEdit* editALeftBrakeP = new QLineEdit(groupBoxStatus);
    QLineEdit* editARightBrakeP = new QLineEdit(groupBoxStatus);
    editALeftBrakeP->setObjectName("editALeftBrakeP");
    editARightBrakeP->setObjectName("editARightBrakeP");
    editALeftBrakeP->setReadOnly(true);
    editARightBrakeP->setReadOnly(true);
    gridLayoutStatus->addWidget(new QLabel(tr("Assistant Brake Pressure:"), groupBoxStatus),
                                4, 0, 1, 2, Qt::AlignCenter);
    gridLayoutStatus->addWidget(editALeftBrakeP, 5, 0, 1, 1);
    gridLayoutStatus->addWidget(editARightBrakeP, 5, 1, 1, 1);

    // Rotation Rate
    QLineEdit* editLeftRotateRate = new QLineEdit(groupBoxStatus);
    QLineEdit* editRightRotateRate = new QLineEdit(groupBoxStatus);
    editLeftRotateRate->setObjectName("editLeftRotateRate");
    editRightRotateRate->setObjectName("editRightRotateRate");
    editLeftRotateRate->setReadOnly(true);
    editRightRotateRate->setReadOnly(true);
    gridLayoutStatus->addWidget(new QLabel(tr("Real Rotation Rate:"), groupBoxStatus),
                                6, 0, 1, 2, Qt::AlignCenter);
    gridLayoutStatus->addWidget(editLeftRotateRate, 7, 0, 1, 1);
    gridLayoutStatus->addWidget(editRightRotateRate, 7, 1, 1, 1);

    // Theory Rotation Rate
    QLineEdit* editTheoryLeftRotateRate = new QLineEdit(groupBoxStatus);
    QLineEdit* editTheoryRightRotateRate = new QLineEdit(groupBoxStatus);
    editTheoryLeftRotateRate->setObjectName("editTheoryLeftRotateRate");
    editTheoryRightRotateRate->setObjectName("editTheoryRightRotateRate");
    editTheoryLeftRotateRate->setReadOnly(true);
    editTheoryRightRotateRate->setReadOnly(true);
    gridLayoutStatus->addWidget(new QLabel(tr("Theory Rotation Rate:"), groupBoxStatus),
                                8, 0, 1, 2, Qt::AlignCenter);
    gridLayoutStatus->addWidget(editTheoryLeftRotateRate, 9, 0, 1, 1);
    gridLayoutStatus->addWidget(editTheoryRightRotateRate, 9, 1, 1, 1);

    //!
    QGroupBox* groupBoxCtrl = new QGroupBox(this);
    groupBoxCtrl->setObjectName("groupBoxCtrl");

    // status view
    QGridLayout* gridLayoutCtrl = new QGridLayout(groupBoxCtrl);
    gridLayoutCtrl->setContentsMargins(5, 5, 5, 5);
    gridLayoutCtrl->setSpacing(20);

    // left-button
    QPushButton* buttonLeftDashboard = new JButtonDash(tr("Left command"), groupBoxCtrl);
    QPushButton* buttonLeftSpeedGain = new JButtonDash(tr("Left Speed Gain"), groupBoxCtrl);
    QPushButton* buttonLeftSpeedKnob = new JButtonDash(tr("Left Speed Knob"), groupBoxCtrl);
    //QPushButton* buttonLeftSpeedSwitch = new JButtonDash(tr("Left Speed Off"), groupBoxCtrl);
    JTracksipButton* buttonLeftTracksip = new JTracksipButton(/*tr("Left Trackslip"), */groupBoxCtrl);
    buttonLeftTracksip->setFixedSize(110, 45);
    buttonLeftDashboard->setObjectName("buttonLeftDashboard");
    buttonLeftSpeedGain->setObjectName("buttonLeftSpeedGain");
    buttonLeftSpeedKnob->setObjectName("buttonLeftSpeedKnob");
    //buttonLeftSpeedSwitch->setObjectName("buttonLeftSpeedSwitch");
    //buttonLeftSpeedSwitch->setCheckable(true);
    buttonLeftTracksip->setObjectName("buttonLeftTracksip");

    // right-button
    QPushButton* buttonRightDashboard = new JButtonDash(tr("Right command"), groupBoxCtrl);
    QPushButton* buttonRightSpeedGain = new JButtonDash(tr("Right Speed Gain"), groupBoxCtrl);
    QPushButton* buttonRightSpeedKnob = new JButtonDash(tr("Right Speed Knob"), groupBoxCtrl);
    //QPushButton* buttonRightSpeedSwitch = new JButtonDash(tr("Right Speed Off"), groupBoxCtrl);
    JTracksipButton* buttonRightTracksip = new JTracksipButton(/*tr("Right Trackslip"), */groupBoxCtrl);
    buttonRightTracksip->setFixedSize(110, 45);
    buttonRightDashboard->setObjectName("buttonRightDashboard");
    buttonRightSpeedGain->setObjectName("buttonRightSpeedGain");
    buttonRightSpeedKnob->setObjectName("buttonRightSpeedKnob");
    //buttonRightSpeedSwitch->setObjectName("buttonRightSpeedSwitch");
    //buttonRightSpeedSwitch->setCheckable(true);
    buttonRightTracksip->setObjectName("buttonRightTracksip");

    QHBoxLayout *horiLayoutTracksip = new QHBoxLayout();
    horiLayoutTracksip->setContentsMargins(0, 0, 0, 0);
    horiLayoutTracksip->setSpacing(5);
    horiLayoutTracksip->addWidget(buttonLeftTracksip);
    horiLayoutTracksip->addWidget(new QLabel(tr("Tracksip"), this));
    horiLayoutTracksip->addWidget(buttonRightTracksip);
    gridLayoutCtrl->addLayout(horiLayoutTracksip, 0, 0, 1, 2);

    //
    gridLayoutCtrl->addWidget(buttonLeftDashboard, 1, 0, 1, 1, Qt::AlignLeft);
    gridLayoutCtrl->addWidget(buttonRightDashboard, 1, 1, 1, 1, Qt::AlignRight);

    QHBoxLayout *horiLayoutSpeedGain = new QHBoxLayout();
    horiLayoutSpeedGain->setContentsMargins(0, 0, 0, 0);
    horiLayoutSpeedGain->setSpacing(5);
    horiLayoutSpeedGain->addWidget(buttonLeftSpeedGain);
    horiLayoutSpeedGain->addWidget(new QLabel(tr("(Speed Gain)"), this), 0, Qt::AlignHCenter);
    horiLayoutSpeedGain->addWidget(buttonRightSpeedGain);
    gridLayoutCtrl->addLayout(horiLayoutSpeedGain, 2, 0, 1, 2);

    QHBoxLayout *horiLayoutSpeedKnob = new QHBoxLayout();
    horiLayoutSpeedKnob->setContentsMargins(0, 0, 0, 0);
    horiLayoutSpeedKnob->setSpacing(5);
    horiLayoutSpeedKnob->addWidget(buttonLeftSpeedKnob);
    horiLayoutSpeedKnob->addWidget(new QLabel(tr("(Speed Knob)"), this), 0, Qt::AlignHCenter);
    horiLayoutSpeedKnob->addWidget(buttonRightSpeedKnob);
    gridLayoutCtrl->addLayout(horiLayoutSpeedKnob, 3, 0, 1, 2);
/*
    QHBoxLayout *horiLayoutSpeedSwitch = new QHBoxLayout();
    horiLayoutSpeedSwitch->setContentsMargins(0, 0, 0, 0);
    horiLayoutSpeedSwitch->setSpacing(5);
    horiLayoutSpeedSwitch->addWidget(buttonLeftSpeedSwitch);
    //horiLayoutSpeedSwitch->addWidget(new QLabel(tr("(Speed Switch)"), this), 0, Qt::AlignHCenter);
    horiLayoutSpeedSwitch->addWidget(new QLabel(this), 0, Qt::AlignHCenter);
    horiLayoutSpeedSwitch->addWidget(buttonRightSpeedSwitch);
    gridLayoutCtrl->addLayout(horiLayoutSpeedSwitch, 4, 0, 1, 2);
*/
    //
    QVBoxLayout *vertLayoutMid = new QVBoxLayout(areaMiddle);
    vertLayoutMid->setContentsMargins(0, 0, 0, 0);
    vertLayoutMid->setSpacing(0);
    vertLayoutMid->addWidget(groupBoxStatus);
    vertLayoutMid->addWidget(groupBoxCtrl);
    vertLayoutMid->addSpacing(20);

    //
    QGridLayout* gridLayoutBottom = new QGridLayout();
    gridLayoutBottom->setContentsMargins(0, 0, 0, 0);
    gridLayoutBottom->setSpacing(1);
    gridLayoutBottom->addWidget(curveLBP, 0, 0, 1, 1);
    gridLayoutBottom->addWidget(curveLRP, 1, 0, 1, 1);
    gridLayoutBottom->addWidget(areaMiddle, 0, 1, 2, 1);
    gridLayoutBottom->addWidget(curveRBP, 0, 2, 1, 1);
    gridLayoutBottom->addWidget(curveRRP, 1, 2, 1, 1);

    // main layout
    QVBoxLayout* vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(5, 5, 5, 5);
    vertLayoutMain->addWidget(groupBoxTop);
    vertLayoutMain->addLayout(gridLayoutBottom);

    /// global properties

    qAppSetProperity<MainWidget*>(this);
    SerialPortProxy* serialPortProxy = qAppSetProperity<SerialPortProxy*>(new SerialPortProxy(this));

    /// signal-slots

    connect(buttonSettings, &QPushButton::clicked, this, [=](){
        serialPortProxy->save();
        SettingsWidget settingsWidget(this);
        settingsWidget.exec();
        serialPortProxy->restore();
    });

    connect(buttonHistory, &QPushButton::clicked, this, [=](){
        serialPortProxy->save();
        DataBaseMgr::instance().save();
        HistoryWidget historyWidget(this);
        historyWidget.showFullScreen();
        historyWidget.exec();
        DataBaseMgr::instance().restore();
        serialPortProxy->restore();
    });

    connect(buttonPortState, &JSwitchButton::clicked, this, [=](bool checked){
        if (checked) {
            serialPortProxy->start();
        } else {
            serialPortProxy->stop();
        }
    });

    connect(buttonQuit, &QPushButton::clicked, this, [=](){
        if (QMessageBox::warning(this, tr("Warning"), tr("Are you sure to quit?"), QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::Yes) {
            close();
        }
    });

    connect(curveLBP, &CurveWidget::doubleClicked, this, [=](bool checked){
        groupBoxTop->setVisible(checked);
        areaMiddle->setVisible(checked);
        curveLRP->setVisible(checked);
        curveRBP->setVisible(checked);
        curveRRP->setVisible(checked);
    });

    connect(curveLRP, &CurveWidget::doubleClicked, this, [=](bool checked){
        groupBoxTop->setVisible(checked);
        areaMiddle->setVisible(checked);
        curveLBP->setVisible(checked);
        curveRBP->setVisible(checked);
        curveRRP->setVisible(checked);
    });

    connect(curveRBP, &CurveWidget::doubleClicked, this, [=](bool checked){
        groupBoxTop->setVisible(checked);
        areaMiddle->setVisible(checked);
        curveLBP->setVisible(checked);
        curveLRP->setVisible(checked);
        curveRRP->setVisible(checked);
    });

    connect(curveRRP, &CurveWidget::doubleClicked, this, [=](bool checked){
        groupBoxTop->setVisible(checked);
        areaMiddle->setVisible(checked);
        curveLBP->setVisible(checked);
        curveLRP->setVisible(checked);
        curveRBP->setVisible(checked);
    });

    // switch-power
    connect(buttonLeftPower, &JSwitchButton::stateChanged, this, [=](bool state) {
        q_sendData.ctrlWord.lPowerSw = (state ? 1 : 0);
        q_sendData.sum = SerialPortProxy::serialPortSendSum(q_sendData);
        serialPortProxy->writeData((char*)&q_sendData, sizeof(q_sendData));
    });
    connect(buttonRightPower, &JSwitchButton::stateChanged, this, [=](bool state) {
        q_sendData.ctrlWord.rPowerSw = (state ? 1 : 0);
        q_sendData.sum = SerialPortProxy::serialPortSendSum(q_sendData);
        serialPortProxy->writeData((char*)&q_sendData, sizeof(q_sendData));
    });

    // switch-track
    connect(buttonLeftTracksip, &JTracksipButton::stateChanged, this, [=](bool state) {
        q_sendData.ctrlWord.lTracksip = (state ? 1 : 0);
        q_sendData.sum = SerialPortProxy::serialPortSendSum(q_sendData);
        serialPortProxy->writeData((char*)&q_sendData, sizeof(q_sendData));
    });
    connect(buttonRightTracksip, &JTracksipButton::stateChanged, this, [=](bool state) {
        q_sendData.ctrlWord.rTracksip = (state ? 1 : 0);
        q_sendData.sum = SerialPortProxy::serialPortSendSum(q_sendData);
        serialPortProxy->writeData((char*)&q_sendData, sizeof(q_sendData));
    });

    //connect(buttonLeftDashboard, &QPushButton::clicked, this, &MainWidget::execSliderWidget);
    //connect(buttonRightDashboard, &QPushButton::clicked, this, &MainWidget::execSliderWidget);
    connect(buttonLeftSpeedGain, &QPushButton::clicked, this, &MainWidget::execSliderWidget);
    connect(buttonLeftSpeedKnob, &QPushButton::clicked, this, &MainWidget::execSliderWidget);
    connect(buttonRightSpeedGain, &QPushButton::clicked, this, &MainWidget::execSliderWidget);
    connect(buttonRightSpeedKnob, &QPushButton::clicked, this, &MainWidget::execSliderWidget);
/*
    connect(buttonLeftSpeedSwitch, &QPushButton::clicked, this, [=](bool checked) {
        SerialPortSend sendData;
        memcpy(&sendData, &q_sendData, sizeof(SerialPortSend));
        if (!checked) {
            sendData.lWheelSpd = 0;
        }
        q_sendData.sum = SerialPortProxy::serialPortSendSum(sendData);
        serialPortProxy->writeData((char*)&sendData, sizeof(sendData));
    });

    connect(buttonRightSpeedSwitch, &QPushButton::clicked, this, [=](bool checked) {
        SerialPortSend sendData;
        memcpy(&sendData, &q_sendData, sizeof(SerialPortSend));
        if (!checked) {
            sendData.rWheelSpd = 0;
        }
        q_sendData.sum = SerialPortProxy::serialPortSendSum(sendData);
    });
*/
    connect(serialPortProxy, &SerialPortProxy::stateChanged, this, [=](const QString &info) {
        QString text = tr("SerialPort: [%1] %2")
                .arg(serialPortProxy->config().toString())
                .arg(info);
        qDebug() << text;
    });

    connect(serialPortProxy, &SerialPortProxy::serialPortError, this,
            [=](QSerialPort::SerialPortError error, const QString &info) {
        switch (error) {
        case QSerialPort::NoError:
            buttonPortState->setState(true);
            break;
        default:
            buttonPortState->setState(false);
            break;
        }
        QString text = tr("SerialPort: [%1] %2")
                .arg(serialPortProxy->config().toString())
                .arg(info);
        qDebug() << text;
    });

    connect(serialPortProxy, &SerialPortProxy::displayRespond,
            this, [=](const SerialPortRecv &data, const QDateTime &dateTime) {
        QString suffix;

        // Major Brake Pressure
        suffix = editMLeftBrakeP->text().section(' ', 1, 1).prepend(' ');
        editMLeftBrakeP->setText(QString("%1").arg(data.lMBrakeP*1.0, 0, 'f', 2).append(suffix));
        suffix = editMRightBrakeP->text().section(' ', 1, 1).prepend(' ');
        editMRightBrakeP->setText(QString("%1").arg(data.rABrakeP*1.0, 0, 'f', 2).append(suffix));

        // Assistent Brake Pressure
        suffix = editALeftBrakeP->text().section(' ', 1, 1).prepend(' ');
        editALeftBrakeP->setText(QString("%1").arg(data.lMBrakeP*1.0, 0, 'f', 2).append(suffix));
        suffix = editARightBrakeP->text().section(' ', 1, 1).prepend(' ');
        editARightBrakeP->setText(QString("%1").arg(data.rABrakeP*1.0, 0, 'f', 2).append(suffix));

        // Rotation Rate
        suffix = editLeftRotateRate->text().section(' ', 1, 1).prepend(' ');
        editLeftRotateRate->setText(QString("%1").arg(data.lWheelSpd*1.0, 0, 'f', 2).append(suffix));
        suffix = editRightRotateRate->text().section(' ', 1, 1).prepend(' ');
        editRightRotateRate->setText(QString("%1").arg(data.rWheelSpd*1.0, 0, 'f', 2).append(suffix));

        ///! curve
        quint64 timeT = dateTime.toMSecsSinceEpoch();

        // curve - LBP
        curveLBP->curve(0)->shift(timeT, data.lMBrakeP);
        curveLBP->curve(1)->shift(timeT, data.lABrakeP);
        // curve - LRR
        curveLRP->curve(0)->shift(timeT, data.lWheelSpd);
        curveLRP->curve(1)->shift(timeT, q_lTheorySpd);
        // curve - RBP
        curveRBP->curve(0)->shift(timeT, data.rMBrakeP);
        curveRBP->curve(1)->shift(timeT, data.rABrakeP);
        // curve - RRR
        curveRRP->curve(0)->shift(timeT, data.rWheelSpd);
        curveRRP->curve(1)->shift(timeT, q_rTheorySpd);

        // save as database
        if (!DataBaseMgr::instance().write(data, q_lTheorySpd, q_rTheorySpd, timeT)) {
            //Q_ASSERT(false);
        }
    });

    //! finally initialize

    // Major Brake Pressure
    editMLeftBrakeP->setText("0 MPa");
    editMRightBrakeP->setText("0 MPa");
    // Assistant Brake Pressure
    editALeftBrakeP->setText("0 MPa");
    editARightBrakeP->setText("0 MPa");
    // Rotation Rate
    editLeftRotateRate->setText("0 r/min");
    editRightRotateRate->setText("0 r/min");
    // Theory Rotation Rate
    editTheoryLeftRotateRate->setText("0 r/min");
    editTheoryRightRotateRate->setText("0 r/min");

    //
    memset(&q_sendData, 0, sizeof(SerialPortSend));
    Q_ASSERT(_countof(SerialHeader) == 2);
    Q_ASSERT(_countof(SerialTail) == 1);
    q_sendData.header = *(quint16*)SerialHeader;
    q_sendData.length = sizeof(SerialPortSend);
    q_sendData.ctrlWord.lTracksip = 0;
    q_sendData.ctrlWord.rTracksip = 0;
    q_sendData.tail = *(quint8*)SerialTail;

    // SQL
    QString sqlName = QApplication::applicationDirPath()
            .append("/../data/cm-")
            .append(QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd-HH-mm-ss"))
            .append(".db");
    if (!DataBaseMgr::instance().create(sqlName)) {
        Q_ASSERT(false);
    }

    // start serialport
    serialPortProxy->start();

    //
    buttonLeftTracksip->setState(q_sendData.ctrlWord.lTracksip == 1);
    buttonRightTracksip->setState(q_sendData.ctrlWord.rTracksip == 1);
}

void MainWidget::execSliderWidget()
{
    SerialPortProxy* serialPortProxy = qAppProperity<SerialPortProxy*>("SerialPortProxy");
    if (!serialPortProxy) {
        return;
    }

    QString title;
    if (sender()->inherits("QPushButton")) {
        title = qobject_cast<QPushButton*>(sender())->text();
    }

    SliderWidget sliderWidget(title, this);

    QString objName = sender()->objectName();
    if (objName == "buttonLeftSpeedGain") {
        QLineEdit *lineEdit = findChild<QLineEdit*>("editTheoryLeftRotateRate");
        if (lineEdit) {
            QString suffix = lineEdit->text().section(' ', 1, 1).prepend(' ');
            sliderWidget.setRange(0, 3000);
            //sliderWidget.setDecimals(2);
            //sliderWidget.setSingleStep(0.01);
            sliderWidget.setSuffix(suffix);
            sliderWidget.setValue(lineEdit->text().section(' ', 0, 0).toDouble());
            connect(&sliderWidget, &SliderWidget::valueChanged, this, [=](double value){
                q_lTheorySpd = (quint32)value;
                QPushButton *buttonLeftSpeedSwitch = this->findChild<QPushButton*>("buttonLeftSpeedSwitch");
                if (buttonLeftSpeedSwitch) {
                    buttonLeftSpeedSwitch->setText(value == 0.0 ? tr("Left Speed Off") : tr("Left Speed On"));
                }
                lineEdit->setText(QString("%1").arg(value, 0, 'f', 2).append(suffix));

                // send
                q_sendData.index++;

                q_sendData.lWheelSpd = (quint32)(value * 42.94967296);
                q_sendData.sum = SerialPortProxy::serialPortSendSum(q_sendData);
                serialPortProxy->writeData((char*)&q_sendData, sizeof(q_sendData));
            });
        }
    } else if (objName == "buttonRightSpeedGain") {
        QLineEdit *lineEdit = findChild<QLineEdit*>("editTheoryRightRotateRate");
        if (lineEdit) {
            QString suffix = lineEdit->text().section(' ', 1, 1).prepend(' ');
            sliderWidget.setRange(0, 3000);
            //sliderWidget.setDecimals(2);
            //sliderWidget.setSingleStep(0.01);
            sliderWidget.setSuffix(suffix);
            sliderWidget.setValue(lineEdit->text().section(' ', 0, 0).toDouble());
            connect(&sliderWidget, &SliderWidget::valueChanged, this, [=](double value){
                q_rTheorySpd = (quint32)value;
                QPushButton *buttonRightSpeedSwitch = this->findChild<QPushButton*>("buttonRightSpeedSwitch");
                if (buttonRightSpeedSwitch) {
                    buttonRightSpeedSwitch->setText(value == 0.0 ? tr("Right Speed Off") : tr("Right Speed On"));
                }
                lineEdit->setText(QString("%1").arg(value, 0, 'f', 2).append(suffix));

                // send
                q_sendData.index++;
                q_sendData.rWheelSpd = (quint32)(value * 42.94967296);
                q_sendData.sum = SerialPortProxy::serialPortSendSum(q_sendData);
                serialPortProxy->writeData((char*)&q_sendData, sizeof(q_sendData));
            });
        }
    } else if (objName == "buttonLeftSpeedKnob") {
        QLineEdit *lineEdit = findChild<QLineEdit*>("editTheoryLeftRotateRate");
        if (lineEdit) {
            QString suffix = lineEdit->text().section(' ', 1, 1).prepend(' ');
            double currValue = lineEdit->text().section(' ', 0, 0).toDouble();
            double min = qMax<double>(0, currValue - 50);
            double max = qMin<double>(3000, currValue + 50);
            if (max < 100) {
                max = min + 100;
            }
            if (min > 30000 - 100) {
                min = max - 100;
            }
            sliderWidget.setRange(min, max);
            //sliderWidget.setDecimals(2);
            //sliderWidget.setSingleStep(0.01);
            sliderWidget.setSuffix(suffix);
            sliderWidget.setValue(lineEdit->text().section(' ', 0, 0).toDouble());
            connect(&sliderWidget, &SliderWidget::valueChanged, this, [=](double value){
                q_lTheorySpd = (quint32)value;
                QPushButton *buttonLeftSpeedSwitch = this->findChild<QPushButton*>("buttonLeftSpeedSwitch");
                if (buttonLeftSpeedSwitch) {
                    buttonLeftSpeedSwitch->setText(value == 0.0 ? tr("Left Speed Off") : tr("Left Speed On"));
                }
                lineEdit->setText(QString("%1").arg(value, 0, 'f', 2).append(suffix));

                // send
                q_sendData.index++;

                q_sendData.lWheelSpd = (quint32)(value * 42.94967296);
                q_sendData.sum = SerialPortProxy::serialPortSendSum(q_sendData);
                serialPortProxy->writeData((char*)&q_sendData, sizeof(q_sendData));
            });
        }
    } else if (objName == "buttonRightSpeedKnob") {
        QLineEdit *lineEdit = findChild<QLineEdit*>("editTheoryRightRotateRate");
        if (lineEdit) {
            QString suffix = lineEdit->text().section(' ', 1, 1).prepend(' ');
            double currValue = lineEdit->text().section(' ', 0, 0).toDouble();
            double min = qMax<double>(0, currValue - 50);
            double max = qMin<double>(3000, currValue + 50);
            if (max < 100) {
                max = min + 100;
            }
            if (min > 30000 - 100) {
                min = max - 100;
            }
            sliderWidget.setRange(min, max);
            //sliderWidget.setDecimals(2);
            //sliderWidget.setSingleStep(0.01);
            sliderWidget.setSuffix(suffix);
            sliderWidget.setValue(lineEdit->text().section(' ', 0, 0).toDouble());
            connect(&sliderWidget, &SliderWidget::valueChanged, this, [=](double value){
                q_rTheorySpd = (quint32)value;
                QPushButton *buttonRightSpeedSwitch = this->findChild<QPushButton*>("buttonRightSpeedSwitch");
                if (buttonRightSpeedSwitch) {
                    buttonRightSpeedSwitch->setText(value == 0.0 ? tr("Right Speed Off") : tr("Right Speed On"));
                }
                lineEdit->setText(QString("%1").arg(value, 0, 'f', 2).append(suffix));

                // send
                q_sendData.index++;
                q_sendData.rWheelSpd = (quint32)(value * 42.94967296);
                q_sendData.sum = SerialPortProxy::serialPortSendSum(q_sendData);
                serialPortProxy->writeData((char*)&q_sendData, sizeof(q_sendData));
            });
        }
    } else {
        Q_ASSERT(false);
        return;
    }

    sliderWidget.exec();
}

void MainWidget::showEvent(QShowEvent *event)
{
    if (true) {
        readSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    if (true) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWidget::readSettings()
{
    QSettings settings;
    settings.beginGroup("Settings/" + objectName());
    settings.endGroup();
}

void MainWidget::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Settings/" + objectName());
    settings.endGroup();
}
