#ifndef SERIALPORT_PROXY_H
#define SERIALPORT_PROXY_H

#include <QMutex>
#include "protocol.h"
#include <QSerialPort>

#ifdef SERIALPORT_SIMULATE
class SerialPortSimulate;
#endif  // SERIALPORT_SIMULATE

// serialport informations
struct SerialPortConfig
{
    QString port;
    int baudRate;
    int dataBits;
    int parity;
    int stopBits;

    QString toString(bool prePort = true) const
    {
        QString text;
        if (prePort) {
            text.append(port + ": ");
        }
        text.append(QString("%1").arg(baudRate));
        text.append(QString("-%1").arg(dataBits));
        switch (parity) {
        case QSerialPort::NoParity: text.append("-N"); break;
        case QSerialPort::EvenParity: text.append("-E"); break;
        case QSerialPort::OddParity: text.append("-O"); break;
        case QSerialPort::SpaceParity: text.append("-S"); break;
        case QSerialPort::MarkParity: text.append("-M"); break;
        default: Q_ASSERT(false); return QString();
        }
        switch (stopBits) {
        case QSerialPort::OneStop: text.append("-1"); break;
        case QSerialPort::OneAndHalfStop: text.append("-1.5"); break;
        case QSerialPort::TwoStop: text.append("-2"); break;
        default: Q_ASSERT(false); return QString();
        }

        return text;
    }
};

///! # SerialPortProxy #

class QSerialPort;

class SerialPortProxy : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortProxy(QObject *parent = 0);
    ~SerialPortProxy();

    const SerialPortConfig & config() const { return q_serialConfig; }
    void setConfig(const SerialPortConfig &value);

    static quint16 calcCRC16(void* buffer, int size)
    {
        Q_ASSERT(size > 4);
        return crc16_8005(&((quint8*)buffer)[1], size - 4);
    }

    friend quint16 crc16_8005(const quint8* buffer, int length);

    static quint8 serialPortSendSum(const SerialPortSend &data);
    static quint8 serialPortRecvSum(const SerialPortRecv &data);
    static quint16 swapUint16(quint16 value);
    static quint32 swapUint32(quint32 value);
    static quint32 convertFromASCII(quint32 value);
    static quint32 convertToASCII(quint32 value);

signals:
    void stateChanged(const QString &info);
    void serialPortError(QSerialPort::SerialPortError error, const QString &info);
    void displayRespond(const SerialPortRecv &data, const QDateTime &dateTime);

public slots:
    void start();
    void stop();
    void save();
    void restore();
    int writeData(const char *data, int size);
    int readData();

protected:
    void unpack(int recvCount);
    void dispatch();

private:
    QSerialPort* q_serialPort;
    SerialPortConfig q_serialConfig;

    // stream-way parse received data
    int    q_currIndex;         //
    int    q_frameSize;         //
    quint8 q_currFrameId;       //
    quint8 q_streamBuff[4096];  //
    quint8 q_newBuff[4096];     //
    quint8 q_pkgBuff[100];      //

    SerialPortRecv q_recvData;

#ifdef SERIALPORT_SIMULATE
    SerialPortSimulate*    q_serialPortSimulate;
#endif  // SERIALPORT_SIMULATE
};

#endif // SERIALPORT_PROXY_H
