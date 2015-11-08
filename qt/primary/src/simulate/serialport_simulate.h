#ifndef SERIALPORT_SIMULATE_H
#define SERIALPORT_SIMULATE_H

#ifdef SERIALPORT_SIMULATE

#include <QSerialPort>
#include <QTimerEvent>
#include "../protocol/protocol.h"
#include "../protocol/serialport_proxy.h"

class SerialPortSimulate : public QObject
{
    Q_OBJECT
public:
    SerialPortSimulate(QObject * parent = 0)
        : QObject(parent)
        , q_currIndex(0)
        , q_currFrameId(0)
        , q_frameSize(0)
    {
        setObjectName("SerialPortSimulate");

        q_serialPort = new QSerialPort(this);

        ////////////////////////////////////////////////////////////////////

        connect(q_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                this, [=](QSerialPort::SerialPortError error) {
            switch (error) {
            case QSerialPort::NoError: emit serialPortError(tr("No error")); return;
            case QSerialPort::DeviceNotFoundError: emit serialPortError(tr("Device not found")); break;
            case QSerialPort::PermissionError: emit serialPortError(tr("Permission error")); break;
            case QSerialPort::OpenError: emit serialPortError(tr("Open error")); break;
            case QSerialPort::ParityError: emit serialPortError(tr("Parity error")); break;
            case QSerialPort::FramingError: emit serialPortError(tr("Framing error")); break;
            case QSerialPort::BreakConditionError: emit serialPortError(tr("Break condition error")); break;
            case QSerialPort::WriteError: emit serialPortError(tr("Write error")); break;
            case QSerialPort::ReadError: emit serialPortError(tr("Read error")); break;
            case QSerialPort::ResourceError: emit serialPortError(tr("Resource error")); break;
            case QSerialPort::UnsupportedOperationError: emit serialPortError(tr("Unsupported operation")); break;
            case QSerialPort::UnknownError: emit serialPortError(tr("Unknown error")); break;
            case QSerialPort::TimeoutError: emit serialPortError(tr("Timeout")); break;
            case QSerialPort::NotOpenError: emit serialPortError(tr("Not open")); break;
            default: Q_ASSERT(false); break;
            }
            qErrnoWarning(error, "Serialport opend failed!");
        });
        connect(q_serialPort, &QSerialPort::readyRead, this, &SerialPortSimulate::readData);
        ////////////////////////////////////////////////////////////////////

        //
        q_serialPort->setPortName("COM5");
        if (q_serialPort->open(QIODevice::ReadWrite)) {
            q_serialPort->setBaudRate(QSerialPort::Baud115200);
            q_serialPort->setDataBits(QSerialPort::Data8);
            q_serialPort->setParity(QSerialPort::OddParity);
            q_serialPort->setStopBits(QSerialPort::OneStop);
            emit stateChanged(tr("Open"));
        }

        memset(&q_sendData, 0, sizeof(SerialPortSend));
        //
        memset(&q_recvData, 0, sizeof(SerialPortRecv));
        Q_ASSERT(_countof(SerialHeader) == 2);
        Q_ASSERT(_countof(SerialTail) == 1);
        q_recvData.header = *(quint16*)SerialHeader;
        q_recvData.length = sizeof(SerialPortRecv);
        q_recvData.tail = *(quint8*)SerialTail;

        //

        q_recvData.lMBrakeP = SerialPortProxy::swapUint16(0);
        q_recvData.rMBrakeP = SerialPortProxy::swapUint16(0);

        q_recvData.lABrakeP = SerialPortProxy::swapUint16(5);
        q_recvData.rABrakeP = SerialPortProxy::swapUint16(5);

        q_recvData.lMRotateR = SerialPortProxy::swapUint16(0);
        q_recvData.rMRotateR = SerialPortProxy::swapUint16(0);

        q_recvData.lARotateR = SerialPortProxy::swapUint16(5);
        q_recvData.rARotateR = SerialPortProxy::swapUint16(5);

        q_recvData.lWheelSpd = convertToASCII(100);
        q_recvData.rWheelSpd = convertToASCII(100);

        //
        q_timerId = startTimer(100, Qt::PreciseTimer);
    }

    ~SerialPortSimulate()
    {

    }

    void start()
    {
        if (!q_serialPort->isOpen()) {
            q_serialPort->open(QIODevice::ReadWrite);
            q_timerId = startTimer(100, Qt::PreciseTimer);
        }
    }

    void stop()
    {
        if (q_serialPort->isOpen()) {
            killTimer(q_timerId);
            q_serialPort->close();
        }
    }

    void save()
    {
        setProperty("portState", q_serialPort->isOpen());
        this->stop();
    }

    void restore()
    {
        property("portState").toBool() ? this->start() : this->stop();
    }

signals:
    void stateChanged(const QString &info);
    void serialPortError(const QString &info);

public slots:
    int writeData(const char *data, int size)
    {
        if (!q_serialPort->isOpen()) {
            //QMessageBox::critical(Q_NULLPTR, tr("Error"), tr("serialport '%1' is not open")
            //s                      .arg(q_serialPort->portName()), QMessageBox::Ok);
            killTimer(q_timerId);
            return 0;
        }

        return (int)q_serialPort->write(data, size);

    }

    int readData()
    {
        int count = (int)q_serialPort->read((char*)q_streamBuff, 4096);
        unpack(count);
        return count;
    }

protected:
    void unpack(int recvCount)
    {
        for (int i = 0; i < recvCount; i++) {
            if (q_currIndex < offsetof(SerialPortSend, length)) {     // 1. frame-header

                q_frameSize = 0;	//

                if (q_streamBuff[i] == SerialHeader[q_currIndex]) {
                    q_newBuff[q_currIndex++] = q_streamBuff[i];
                } else {
                    q_currIndex = 0;
                }
                continue;

            } else if (q_currIndex == offsetof(SerialPortSend, length)) {  // 2. frame-length

                switch (q_streamBuff[i]) {
                case sizeof(SerialPortSend):
                    break;
                default:
                    q_currIndex = 0;    // invalid frame
                    continue;
                }

                q_frameSize = q_streamBuff[i];
                q_newBuff[q_currIndex++] = q_streamBuff[i];
                continue;

            } else if (q_currIndex == offsetof(SerialPortSend, index)) {  // 3 frame-id

                q_currFrameId = q_streamBuff[i];
                q_newBuff[q_currIndex++] = q_streamBuff[i];
                continue;

            } else if (q_currIndex < q_frameSize) {  // 4. receive data in a frame

                q_newBuff[q_currIndex++] = q_streamBuff[i];
            }

            if (q_currIndex == q_frameSize) {    // reveice full a frame successfully

                // 5. parity-sum
                quint8 sum = 0;
                for (int n = 0; n < offsetof(SerialPortSend, sum); ++n) {
                    sum += q_newBuff[n];
                }
                if (q_newBuff[offsetof(SerialPortSend, sum)] != sum) {
                    q_currIndex = 0;    // invalid frame
                    continue;
                }

                // 6. frame-tail

                if (q_newBuff[offsetof(SerialPortSend, tail)] != *(quint8*)SerialTail) {
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

    void dispatch()
    {
        memcpy(&q_sendData, q_pkgBuff, sizeof(SerialPortSend));

        // send
        q_recvData.index++;
        q_recvData.lWheelSpd = convertToASCII((quint32)(q_sendData.lWheelSpd / 42.94967296));
        q_recvData.rWheelSpd = convertToASCII((quint32)(q_sendData.rWheelSpd / 42.94967296));
        q_recvData.sum = SerialPortProxy::serialPortRecvSum(q_recvData);
        writeData((char*)&q_recvData, sizeof(q_recvData));
    }

    static quint32 stepWheelSpd(quint32 value, quint32 step, quint32 max)
    {
        quint32 v = SerialPortProxy::convertFromASCII(value) + step;
        if (v > max) {
            v = 0;
        }
        return convertToASCII(v);
    }

    static quint32 convertToASCII(quint32 value)
    {
        const char *buffer = (QString("%1").arg(value, 5, 16, QChar('0'))).toStdString().data();
        memcpy(&value, buffer + 1, sizeof(value));
        return value;
    }

    quint16 stepInc(quint16 value, quint16 step, int max)
    {
         quint16 v = SerialPortProxy::swapUint16(value) + step/* + qrand() % 50*/;
         if (v > max) {
             v = 0;
         }
        return SerialPortProxy::swapUint16(v);
    }

    void timerEvent(QTimerEvent *event)
    {
        int step = 1;
        if (event->timerId() == q_timerId) {
            q_recvData.index++;
            if (q_sendData.ctrlWord.lPowerSw) {
                q_recvData.lMBrakeP = stepInc(q_recvData.lMBrakeP, step, 30);
                q_recvData.lABrakeP = stepInc(q_recvData.lABrakeP, step, 30);
                q_recvData.lMRotateR = stepInc(q_recvData.lMRotateR, step, 30);
                q_recvData.lARotateR = stepInc(q_recvData.lARotateR, step, 30);
                q_recvData.lWheelSpd = stepWheelSpd(q_recvData.lWheelSpd, step, 3000);
            }
            if (q_sendData.ctrlWord.rPowerSw) {
                q_recvData.rMBrakeP = stepInc(q_recvData.rMBrakeP, step, 30);
                q_recvData.rABrakeP = stepInc(q_recvData.rABrakeP, step, 30);
                q_recvData.rMRotateR = stepInc(q_recvData.rMRotateR, step, 30);
                q_recvData.rARotateR = stepInc(q_recvData.rARotateR, step, 30);
                q_recvData.rWheelSpd = stepWheelSpd(q_recvData.rWheelSpd, step, 3000);
            }
            q_recvData.sum = SerialPortProxy::serialPortRecvSum(q_recvData);
            writeData((char*)&q_recvData, sizeof(q_recvData));
        }
    }

private:
    QSerialPort* q_serialPort;
    int    q_timerId;
    // stream-way parse received data
    int    q_currIndex;        //
    int    q_frameSize;        //
    quint8 q_currFrameId;      //
    quint8 q_streamBuff[4096]; //
    quint8 q_newBuff[4096];    //
    quint8 q_pkgBuff[100];     //
    SerialPortSend  q_sendData;
    SerialPortRecv  q_recvData;
};

#endif  // SERIALPORT_SIMULATE

#endif // SERIALPORT_SIMULATE_H
