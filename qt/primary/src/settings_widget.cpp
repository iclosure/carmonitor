#include "precomp.h"
#include "settings_widget.h"
#include "protocol/serialport_proxy.h"

class JButtonClose : public QPushButton
{
public:
    explicit JButtonClose(QWidget *parent = 0)
        : QPushButton(parent) { }
};

SettingsWidget::SettingsWidget(QWidget *parent) :
    QDialog(parent, QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7 ?
                Qt::CustomizeWindowHint : Qt::FramelessWindowHint),
    q_mousePressed(false)
{
    setObjectName("SettingsWidget");
    //
    stylize();

    /// main layout

    QLabel* labelTitle = new QLabel(tr("Settings"), this);
    JButtonClose* buttonClose = new JButtonClose(this);
    buttonClose->setObjectName("buttonClose");
    buttonClose->setToolTip(tr("Close"));

    QHBoxLayout* horiLayoutTitle = new QHBoxLayout();
    horiLayoutTitle->addWidget(labelTitle, 0, Qt::AlignTop);
    horiLayoutTitle->addStretch();
    horiLayoutTitle->addWidget(buttonClose, 0, Qt::AlignTop);

    QLabel* labelPort = new QLabel(tr("Port:"), this);
    QComboBox* comboBoxPort = new QComboBox(this);
    labelPort->setAlignment(Qt::AlignRight);
    comboBoxPort->setMinimumWidth(100);
    QLabel* labelBaudRate = new QLabel(tr("Baud rate:"), this);
    QComboBox* comboBoxBaudRate = new QComboBox(this);
    labelBaudRate->setAlignment(Qt::AlignRight);
    comboBoxBaudRate->setMinimumWidth(100);
    QLabel* labelDataBits = new QLabel(tr("Data bits:"), this);
    QComboBox* comboBoxDataBits = new QComboBox(this);
    labelDataBits->setAlignment(Qt::AlignRight);
    comboBoxDataBits->setMinimumWidth(100);
    QLabel* labelParity = new QLabel(tr("Parity:"), this);
    QComboBox* comboBoxParity = new QComboBox(this);
    labelParity->setAlignment(Qt::AlignRight);
    comboBoxParity->setMinimumWidth(100);
    QLabel* labelStopBits = new QLabel(tr("Stop bits:"), this);
    QComboBox* comboBoxStopBits = new QComboBox(this);
    labelStopBits->setAlignment(Qt::AlignRight);
    comboBoxStopBits->setMinimumWidth(100);

    QGroupBox* grouBoxSettings = new QGroupBox(tr("Setting port"), this);
    grouBoxSettings->setObjectName("grouBoxSettings");
    QFormLayout* formLayoutSettings = new QFormLayout(grouBoxSettings);
    formLayoutSettings->setSpacing(25);
    formLayoutSettings->setContentsMargins(40, 10, 40, 10);
    formLayoutSettings->addRow(labelPort, comboBoxPort);
    formLayoutSettings->addRow(labelBaudRate, comboBoxBaudRate);
    formLayoutSettings->addRow(labelDataBits, comboBoxDataBits);
    formLayoutSettings->addRow(labelParity, comboBoxParity);
    formLayoutSettings->addRow(labelStopBits, comboBoxStopBits);

    // buttons

    QPushButton* buttonOk = new QPushButton(tr("Ok"), this);
    buttonOk->setObjectName("buttonOk");

    QHBoxLayout* horiLayoutButtons= new QHBoxLayout();
    horiLayoutButtons->addStretch();
    horiLayoutButtons->addWidget(buttonOk);

    // all
    QHBoxLayout* horiLayoutSettings = new QHBoxLayout();
    horiLayoutSettings->addStretch();
    horiLayoutSettings->addWidget(grouBoxSettings);
    horiLayoutSettings->addStretch();

    QVBoxLayout* vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->addLayout(horiLayoutTitle);
    vertLayoutMain->addSpacing(5);
    vertLayoutMain->addLayout(horiLayoutSettings);
    vertLayoutMain->addStretch();
    vertLayoutMain->addLayout(horiLayoutButtons);

    //!
    SerialPortProxy* serialPortProxy = qAppProperity<SerialPortProxy*>("SerialPortProxy");

    /// signal-slot
    connect(comboBoxBaudRate, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
            this, [=](const QString &text) {
        if (text == tr("Customers")) {
            comboBoxBaudRate->setEditable(true);
            comboBoxBaudRate->setValidator(new QRegExpValidator(QRegExp("[1-9](\\d){8}"), this));
            comboBoxBaudRate->clearEditText();
        } else {
            comboBoxBaudRate->setValidator(Q_NULLPTR);
            comboBoxBaudRate->setEditable(false);
        }
    });
    connect(buttonOk, &QPushButton::clicked, [=]() {
        if (comboBoxBaudRate->currentText().isEmpty()) {
            QMessageBox::critical(this, tr("Error"), tr("BaudRate is invalid!"), QMessageBox::Ok);
            comboBoxBaudRate->setFocus();
            return;
        }
        SerialPortConfig config;
        config.port = comboBoxPort->currentText();
        config.baudRate = comboBoxBaudRate->currentText().toInt();
        config.dataBits = comboBoxDataBits->currentText().toInt();
        switch (comboBoxParity->currentText()[0].toUpper().toLatin1()) {
        case 'N': config.parity = QSerialPort::NoParity; break;
        case 'E': config.parity = QSerialPort::EvenParity; break;
        case 'O': config.parity = QSerialPort::OddParity; break;
        case 'S': config.parity = QSerialPort::SpaceParity; break;
        case 'M': config.parity = QSerialPort::MarkParity; break;
        default: Q_ASSERT(false); break;
        }
        switch (comboBoxStopBits->currentIndex()) {
        case 0: config.stopBits = QSerialPort::OneStop; break;
        case 1: config.stopBits = QSerialPort::OneAndHalfStop; break;
        case 2: config.stopBits = QSerialPort::TwoStop; break;
        default: Q_ASSERT(false); break;
        }
        serialPortProxy->setConfig(config);
        serialPortProxy->start();

        this->accept();
    });

    connect(buttonClose, &QPushButton::clicked, this, &QWidget::close);

    /// finallt initialize

    QStringList portsName;
    foreach (auto item , QSerialPortInfo::availablePorts()) {
        portsName.append(item.portName());
    }
    portsName.sort();
    comboBoxPort->addItems(portsName);
    foreach (auto item , QSerialPortInfo::standardBaudRates()) {
        comboBoxBaudRate->addItem(QString("%1").arg(item));
    }
    comboBoxBaudRate->addItem(tr("Customers"));
    comboBoxDataBits->addItems(QStringList()<<"5"<<"6"<<"7"<<"8");
    comboBoxParity->addItems(QStringList()<<"None"<<"Even"<<"Odd"<<"Space"<<"Mark");
    comboBoxStopBits->addItems(QStringList()<<"1"<<"1.5"<<"2");
    comboBoxPort->setCurrentText(serialPortProxy->config().port);
    if (-1 == comboBoxBaudRate->findText(QString("%1").arg(serialPortProxy->config().baudRate))) {
        comboBoxBaudRate->setCurrentText(tr("Customers"));
    }
    comboBoxBaudRate->setCurrentText(QString("%1").arg(serialPortProxy->config().baudRate));
    comboBoxDataBits->setCurrentText(QString("%1").arg(serialPortProxy->config().dataBits));
    switch (serialPortProxy->config().parity) {
    case QSerialPort::NoParity: comboBoxParity->setCurrentText("None"); break;
    case QSerialPort::EvenParity: comboBoxParity->setCurrentText("Even"); break;
    case QSerialPort::OddParity: comboBoxParity->setCurrentText("Odd"); break;
    case QSerialPort::SpaceParity: comboBoxParity->setCurrentText("Space"); break;
    case QSerialPort::MarkParity: comboBoxParity->setCurrentText("Mark"); break;
    default: Q_ASSERT(false); break;
    }
    switch (serialPortProxy->config().stopBits) {
    case QSerialPort::OneStop: comboBoxStopBits->setCurrentIndex(0); break;
    case QSerialPort::OneAndHalfStop: comboBoxStopBits->setCurrentIndex(1); break;
    case QSerialPort::TwoStop: comboBoxStopBits->setCurrentIndex(2); break;
    default: Q_ASSERT(false); break;
    }
}

SettingsWidget::~SettingsWidget()
{

}

void SettingsWidget::stylize()
{
    switch (QSysInfo::windowsVersion()) {
    case QSysInfo::WV_WINDOWS7:
        //setWindowFlags(Qt::CustomizeWindowHint);
        if (QtWin::isCompositionEnabled()) {
            QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
            setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_NoSystemBackground, false);
            setStyleSheet("SettingsWidget { background: transparent; }");
        } else {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
            setStyleSheet(QString("SettingsWidget { background: %1; }").arg(QtWin::realColorizationColor().name()));
        }
        break;
    default:
        setStyleSheet("QWidget#SettingsWidget{"
                      " border: 1px solid rgb(100, 100, 64);"
                      " border-radius: 6px;"
                      " background-color: rgb(32, 32, 32);"
                      "}");
        break;
    }
}

void SettingsWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        q_mousePressed = true;
        q_orgPos = e->pos();
        setCursor(Qt::ClosedHandCursor);
    }

    QDialog::mousePressEvent(e);
}

void SettingsWidget::mouseReleaseEvent(QMouseEvent *e)
{
    q_mousePressed = false;
    setCursor(Qt::ArrowCursor);

    QDialog::mouseMoveEvent(e);
}

void SettingsWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (q_mousePressed) {
        QPoint curPos = e->pos();
        QRect curGeom = geometry();
        move(curGeom.topLeft() + curPos - q_orgPos);
    }

    QDialog::mouseMoveEvent(e);
}

