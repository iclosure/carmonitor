#include "precomp.h"
#include "serialport_proxy.h"

#ifdef SERIALPORT_SIMULATE
#include "../simulate/serialport_simulate.h"
#endif  // SERIALPORT_SIMULATE

SerialPortProxy::SerialPortProxy(QObject *parent)
    : QObject(parent)
    , q_currIndex(0)
    , q_currFrameId(0)
    , q_frameSize(0)
  #ifdef SERIALPORT_SIMULATE
    , q_serialPortSimulate(new SerialPortSimulate(this))
  #endif  // SERIALPORT_SIMULATE
{
    setObjectName("SerialPortProxy");

    q_serialPort = new QSerialPort(this);

    ////////////////////////////////////////////////////////////////////

    connect(q_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, [=](QSerialPort::SerialPortError error) {
        switch (error) {
        case QSerialPort::NoError: emit serialPortError(error, tr("No error")); return;
        case QSerialPort::DeviceNotFoundError: emit serialPortError(error, tr("Device not found")); break;
        case QSerialPort::PermissionError: emit serialPortError(error, tr("Permission error")); break;
        case QSerialPort::OpenError: emit serialPortError(error, tr("Open error")); break;
        case QSerialPort::ParityError: emit serialPortError(error, tr("Parity error")); break;
        case QSerialPort::FramingError: emit serialPortError(error, tr("Framing error")); break;
        case QSerialPort::BreakConditionError: emit serialPortError(error, tr("Break condition error")); break;
        case QSerialPort::WriteError: emit serialPortError(error, tr("Write error")); break;
        case QSerialPort::ReadError: emit serialPortError(error, tr("Read error")); break;
        case QSerialPort::ResourceError: emit serialPortError(error, tr("Resource error")); break;
        case QSerialPort::UnsupportedOperationError: emit serialPortError(error, tr("Unsupported operation")); break;
        case QSerialPort::UnknownError: emit serialPortError(error, tr("Unknown error")); break;
        case QSerialPort::TimeoutError: emit serialPortError(error, tr("Timeout")); break;
        case QSerialPort::NotOpenError: emit serialPortError(error, tr("Not open")); break;
        default: Q_ASSERT(false); break;
        }
        qErrnoWarning(error, "Serialport opend failed!");
    });
    connect(q_serialPort, &QSerialPort::readyRead, this, &SerialPortProxy::readData);

    ////////////////////////////////////////////////////////////////////

    QSettings settings;
    // Group serialport properities
    settings.beginGroup("Settings/" + objectName() + "/SerialPort");
    q_serialConfig.port = settings.value("port", "COM1").toString();
    QStringList infoList = settings.value("info", "115200-8-N-1").toString().split('-');
    q_serialConfig.baudRate = infoList[0].toInt();
    q_serialConfig.dataBits = infoList[1].toInt();
    switch (infoList[2][0].toUpper().toLatin1()) {
    case 'N': q_serialConfig.parity = QSerialPort::NoParity; break;
    case 'E': q_serialConfig.parity = QSerialPort::EvenParity; break;
    case 'O': q_serialConfig.parity = QSerialPort::OddParity; break;
    case 'S': q_serialConfig.parity = QSerialPort::SpaceParity; break;
    case 'M': q_serialConfig.parity = QSerialPort::MarkParity; break;
    default: Q_ASSERT(false); break;
    }
    switch (int(infoList[3].toFloat()*10)) {
    case 10: q_serialConfig.stopBits = QSerialPort::OneStop; break;
    case 15: q_serialConfig.stopBits = QSerialPort::OneAndHalfStop; break;
    case 20: q_serialConfig.stopBits = QSerialPort::TwoStop; break;
    default: Q_ASSERT(false); break;
    }
    settings.endGroup();

    //
    //qRegisterMetaType<SerialPortSend>();
    //qRegisterMetaType<SerialPortRecv>();
}

SerialPortProxy::~SerialPortProxy()
{
}

void SerialPortProxy::setConfig(const SerialPortConfig &value)
{
    // Save config
    q_serialConfig.port = value.port;
    q_serialConfig.baudRate = value.baudRate;
    q_serialConfig.dataBits = value.dataBits;
    q_serialConfig.parity = value.parity;
    q_serialConfig.stopBits = value.stopBits;

    //
    QSettings settings;

    // SerialPort
    settings.beginGroup("Settings/" + objectName() + "/SerialPort");
    settings.setValue("port", q_serialConfig.port);
    settings.setValue("info", q_serialConfig.toString(false));
    settings.endGroup();
}

quint8 SerialPortProxy::serialPortSendSum(const SerialPortSend &data)
{
    quint8 sum = 0;
    int offsetSum = offsetof(SerialPortSend, sum);
    for (int i = 0; i < offsetSum; ++i) {
        sum += ((quint8 *)&data)[i];
    }

    return sum;
}

quint8 SerialPortProxy::serialPortRecvSum(const SerialPortRecv &data)
{
    quint8 sum = 0;
    int offsetSum = offsetof(SerialPortRecv, sum);
    for (int i = 0; i < offsetSum; ++i) {
        sum += ((quint8 *)&data)[i];
    }

    return sum;
}

quint16 SerialPortProxy::swapUint16(quint16 value)
{
    return (value << 8) | (value >> 8);
}

quint32 SerialPortProxy::swapUint32(quint32 value)
{
    return ((value & 0x000000ff) << 24) |
            ((value & 0x0000ff00) << 8) |
            ((value & 0x00ff0000) >> 8) |
            ((value & 0xff000000) >> 24);
}

quint32 SerialPortProxy::convertFromASCII(quint32 value)
{
    char buffer[5] = {0};
    memcpy(buffer, &value, sizeof(value));
    return QString(buffer).toInt(0, 16);
}

quint32 SerialPortProxy::convertToASCII(quint32 value)
{
    QString str = QString("%1").arg(value, 4, 16, QChar('0'));
    char buffer[4] = {0};
    buffer[0] = str.at(0).toLatin1();
    buffer[1] = str.at(1).toLatin1();
    buffer[2] = str.at(2).toLatin1();
    buffer[3] = str.at(3).toLatin1();
    memcpy(&value, buffer, sizeof(value));
    return value;
}

void SerialPortProxy::start()
{
    if (q_serialPort->isOpen()) {
        q_serialPort->close();
    }

    // Config serialport properities
    q_serialPort->setPortName(q_serialConfig.port);

    if (q_serialPort->open(QIODevice::ReadWrite)) {
        q_serialPort->setBaudRate(q_serialConfig.baudRate);
        q_serialPort->setDataBits((QSerialPort::DataBits)q_serialConfig.dataBits);
        q_serialPort->setParity((QSerialPort::Parity)q_serialConfig.parity);
        q_serialPort->setStopBits((QSerialPort::StopBits)q_serialConfig.stopBits);
        emit stateChanged(tr("Open"));
    }
}

void SerialPortProxy::stop()
{
    if (q_serialPort->isOpen()) {
        q_serialPort->close();
        emit stateChanged(tr("Close"));
    }
}

void SerialPortProxy::save()
{
    setProperty("portState", q_serialPort->isOpen());
    this->stop();
#ifdef SERIALPORT_SIMULATE
    q_serialPortSimulate->save();
#endif  // SERIALPORT_SIMULATZE
}

void SerialPortProxy::restore()
{
    property("portState").toBool() ? this->start() : this->stop();
#ifdef SERIALPORT_SIMULATE
    q_serialPortSimulate->restore();
#endif  // SERIALPORT_SIMULATZE
}

int SerialPortProxy::writeData(const char *data, int size)
{
    if (!q_serialPort->isOpen()) {
        //QMessageBox::critical(Q_NULLPTR, tr("Error"), tr("serialport '%1' is not open")
        //                      .arg(q_serialPort->portName()), QMessageBox::Ok);
        return 0;
    }

    return (int)q_serialPort->write(data, size);
}

int SerialPortProxy::readData()
{
    int count = (int)q_serialPort->read((char*)q_streamBuff, 4096);
    unpack(count);
    return count;
}

void SerialPortProxy::unpack(int recvCount)
{
    for (int i = 0; i < recvCount; i++) {
        if (q_currIndex < offsetof(SerialPortRecv, length)) {     // 1. frame-header

            q_frameSize = 0;	//

            if (q_streamBuff[i] == SerialHeader[q_currIndex]) {
                q_newBuff[q_currIndex++] = q_streamBuff[i];
            } else {
                q_currIndex = 0;
            }
            continue;

        } else if (q_currIndex == offsetof(SerialPortRecv, length)) {  // 2. frame-size

            switch (q_streamBuff[i]) {
            case sizeof(SerialPortRecv):
                break;
            default:
                q_currIndex = 0;    // invalid frame
                continue;
            }

            q_frameSize = q_streamBuff[i];
            q_newBuff[q_currIndex++] = q_streamBuff[i];
            continue;

        } else if (q_currIndex == offsetof(SerialPortRecv, index)) {  // 3 frame-id

            q_currFrameId = q_streamBuff[i];
            q_newBuff[q_currIndex++] = q_streamBuff[i];
            continue;

        } else if (q_currIndex < q_frameSize) { // 4. receive data in a frame

            q_newBuff[q_currIndex++] = q_streamBuff[i];
        }

        if (q_currIndex == q_frameSize) {    // reveice full a frame successfully

            // 5. parity-sum
            quint8 sum = 0;
            for (int n = 0; n < offsetof(SerialPortRecv, sum); ++n) {
                sum += q_newBuff[n];
            }
            if (q_newBuff[offsetof(SerialPortRecv, sum)] != sum) {
                q_currIndex = 0;    // invalid frame
                continue;
            }

            // 6. frame-tail

            if (q_newBuff[offsetof(SerialPortRecv, tail)] != *(quint8*)SerialTail) {
                q_currIndex = 0;    // invalid frame
                continue;
            }

            // 7. save as...

            memcpy(q_pkgBuff, q_newBuff, q_frameSize);

            // 8. dispatch

            dispatch();

            // 9. reset

            q_currIndex = 0;
        }
    }
}

void SerialPortProxy::dispatch()
{
    memcpy(&q_recvData, q_pkgBuff, sizeof(SerialPortRecv));

    // convert
    q_recvData.lMBrakeP = swapUint16(q_recvData.lMBrakeP);
    q_recvData.lABrakeP = swapUint16(q_recvData.lABrakeP);
    q_recvData.rMBrakeP = swapUint16(q_recvData.rMBrakeP);
    q_recvData.rABrakeP = swapUint16(q_recvData.rABrakeP);

    q_recvData.lMRotateR = swapUint16(q_recvData.lMRotateR);
    q_recvData.lARotateR = swapUint16(q_recvData.lARotateR);
    q_recvData.rMRotateR = swapUint16(q_recvData.rMRotateR);
    q_recvData.rARotateR = swapUint16(q_recvData.rARotateR);

    // lWheelSpd
    q_recvData.lWheelSpd = convertFromASCII(q_recvData.lWheelSpd);
    q_recvData.rWheelSpd = convertFromASCII(q_recvData.rWheelSpd);

    emit displayRespond(q_recvData, QDateTime::currentDateTime());
}

// CRC16-8005H:
quint16 crc16_8005(const quint8* buffer, int length)
{
    int cnta;
    quint16 cntb;
    quint16 ndata;
    quint16 crcval;
    static quint16 crctable[256];
    static bool initiated = false;

    if (initiated == false) {
        // initialize crc table
        for (cnta = 0; cnta < 256; cnta++) {
            ndata = (quint16)(cnta << 8);
            crcval = 0;
            for (cntb = 0; cntb < 8; cntb++) {
                if (((ndata ^ crcval) & 0x8000) == 0x8000) {
                    crcval = (crcval << 1) ^ 0x8005;
                } else {
                    crcval <<= 1;
                }

                ndata <<= 1;
            }

            crctable[cnta] = crcval;
        }

        initiated = true;
    }

    // calculate crc
    crcval = 0;
    for (cnta = 0; cnta < length; cnta++) {
        crcval = (crcval << 8) ^ ((quint16)crctable[(crcval >> 8) ^ ((quint16)buffer[cnta])]);
    }

    return crcval;
}
